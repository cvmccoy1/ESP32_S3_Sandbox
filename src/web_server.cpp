#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>

#include "log.h"
#include "web_server.h"
#include "fan.h"
#include "ssr.h"
#include "buzzer.h"
#include "rgb_led.h"
#include "web_pages.h"

#define WEB_SERVER_PORT 80

AsyncWebServer server(WEB_SERVER_PORT);
AsyncWebSocket webSocket("/ws");

// Shared state - updated from main loop via UpdateWebServer()
static float g_temperature = 0.0f;
static float g_fanDutyCycle = 0.0f;
static int g_fanRpm = 0;
static bool g_ssrOn = false;
static bool g_autoMode = true;
static const char* g_stageName = "IDLE";
static portMUX_TYPE g_webMux = portMUX_INITIALIZER_UNLOCKED;

static char g_jsonBuf[384];

static void buildJson() {
  snprintf(g_jsonBuf, sizeof(g_jsonBuf),
    "{\"temp\":%.1f,\"fan\":%.1f,\"rpm\":%d,\"ssr\":\"%s\",\"mode\":\"%s\",\"stage\":\"%s\",\"uptime\":%lu}",
    g_temperature, g_fanDutyCycle, g_fanRpm,
    g_ssrOn ? "on" : "off",
    g_autoMode ? "auto" : "manual",
    g_stageName, millis() / 1000);
}

static void broadcastStatus() {
  if (webSocket.count()) {
    buildJson();
    webSocket.textAll(g_jsonBuf);
  }
}

static void handleControl(const char* action) {
  if (strcmp(action, "toggle_ssr") == 0) {
    taskENTER_CRITICAL(&g_webMux);
    g_ssrOn = !g_ssrOn;
    SetSSRState(g_ssrOn);
    SetRGBLEDColor(ON_OFF_LED_INDEX, g_ssrOn ? COLOR_GREEN : COLOR_OFF);
    taskEXIT_CRITICAL(&g_webMux);
  } else if (strcmp(action, "toggle_mode") == 0) {
    taskENTER_CRITICAL(&g_webMux);
    g_autoMode = !g_autoMode;
    taskEXIT_CRITICAL(&g_webMux);
  } else if (strcmp(action, "buzz") == 0) {
    Buzz();
  }
}

static void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
  (void)server;
  (void)arg;
  switch (type) {
    case WS_EVT_CONNECT:
      Slog.printf(PSTR("[WS] Client %u connected from %s\n"), client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Slog.printf(PSTR("[WS] Client %u disconnected\n"), client->id());
      break;
    case WS_EVT_PONG:
      break;
    case WS_EVT_ERROR:
      Slog.printf(PSTR("[WS] Error (client %u)\n"), client->id());
      break;
    case WS_EVT_DATA: {
      AwsFrameInfo* info = (AwsFrameInfo*)arg;
      if (info->opcode != WS_TEXT && info->opcode != WS_BINARY) {
        break;
      }
      if (info->final && info->index == 0 && info->len <= 63) {
        char msg[64];
        if (info->opcode == WS_TEXT) {
          memcpy(msg, data, info->len);
          msg[info->len] = '\0';
        } else {
          memcpy(msg, data, info->len);
          msg[info->len] = '\0';
        }
        Slog.printf(PSTR("[WS] Msg from %u: %s\n"), client->id(), msg);

        if (strncmp(msg, "toggle_ssr", 10) == 0) {
          handleControl("toggle_ssr");
        } else if (strncmp(msg, "toggle_mode", 11) == 0) {
          handleControl("toggle_mode");
        } else if (strncmp(msg, "buzz", 4) == 0) {
          handleControl("buzz");
        } else if (strncmp(msg, "get_status", 10) == 0) {
          buildJson();
          client->text(g_jsonBuf);
          return;
        } else if (strncmp(msg, "set_fan:", 8) == 0) {
          int duty = atoi(msg + 8);
          duty = constrain(duty, 0, 100);
          taskENTER_CRITICAL(&g_webMux);
          if (!g_autoMode) {
            SetFanDutyCycle(duty);
          }
          taskEXIT_CRITICAL(&g_webMux);
        }
        broadcastStatus();
      }
      break;
    }
  }
}

void SetupWebServer() {
  Slog.println(PSTR("[Web] Starting AsyncWebServer..."));

  webSocket.onEvent(onWsEvent);
  server.addHandler(&webSocket);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", (PGM_P)INDEX_HTML);
  });

  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* request) {
    buildJson();
    request->send(200, "application/json", g_jsonBuf);
  });

  server.on("/api/control", HTTP_POST, [](AsyncWebServerRequest* request) {
    String action;
    if (request->hasParam("action", true)) {
      action = request->getParam("action", true)->value();
    } else {
      request->send(400, "text/plain", "Missing action");
      return;
    }

    if (action == "toggle_ssr") {
      handleControl("toggle_ssr");
      char buf[64];
      snprintf(buf, sizeof(buf), "{\"ssr\":\"%s\"}", g_ssrOn ? "on" : "off");
      request->send(200, "application/json", buf);
    } else if (action == "toggle_mode") {
      handleControl("toggle_mode");
      char buf[64];
      snprintf(buf, sizeof(buf), "{\"mode\":\"%s\"}", g_autoMode ? "auto" : "manual");
      request->send(200, "application/json", buf);
    } else if (action == "buzz") {
      handleControl("buzz");
      request->send(200, "application/json", "{\"buzz\":true}");
    } else {
      request->send(400, "text/plain", "Unknown action");
      return;
    }
    broadcastStatus();
  });

  server.on("/api/fan", HTTP_POST, [](AsyncWebServerRequest* request) {
    if (!request->hasParam("duty", true)) {
      request->send(400, "text/plain", "Missing duty");
      return;
    }
    int duty = request->getParam("duty", true)->value().toInt();
    duty = constrain(duty, 0, 100);
    taskENTER_CRITICAL(&g_webMux);
    if (!g_autoMode) {
      SetFanDutyCycle(duty);
    }
    taskEXIT_CRITICAL(&g_webMux);
    char buf[32];
    snprintf(buf, sizeof(buf), "{\"duty\":%d}", duty);
    request->send(200, "application/json", buf);
  });

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.onNotFound([](AsyncWebServerRequest* request) {
    if (request->method() == HTTP_OPTIONS) {
      request->send(204);
    } else {
      request->send(404, "text/plain", "Not found");
    }
  });

  server.begin();
  Slog.printf(PSTR("[Web] Ready at http://%s:%d\n"), WiFi.localIP().toString().c_str(), WEB_SERVER_PORT);
}

void UpdateWebServer(float temperature, float fanDutyCycle, int fanRpm, bool ssrOn, const char* stageName) {
  taskENTER_CRITICAL(&g_webMux);
  g_temperature = temperature;
  g_fanDutyCycle = fanDutyCycle;
  g_fanRpm = fanRpm;
  g_ssrOn = ssrOn;
  g_stageName = stageName;
  taskEXIT_CRITICAL(&g_webMux);

  broadcastStatus();
}

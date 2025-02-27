// Include Arduino framework
#include <Arduino.h>
#include <WiFiManager.h>

#include "log.h"
#include "my_wifi.h"
#include "lcd.h"

#define WIFI_LED_PIN   2        //GPIO2
#define WIFI_LED_OFF   HIGH
#define WIFI_LED_ON    LOW


void SetupWifi()
{
  // Setup the Wifi LED GPIO
  pinMode(WIFI_LED_PIN, OUTPUT);

  bool wifiLedState = WIFI_LED_OFF;
  digitalWrite(WIFI_LED_PIN, wifiLedState);     // Initialize WIFI LED in the OFF state 

  // Explicitly set wifi mode to station mode: the ESP32 connects to an access point
  WiFi.mode(WIFI_STA); 

  //WiFiManager local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
   
  // Reset settings - wipe stored credentials for testing - these are stored by the esp library
  //wm.resetSettings();

  // Automatically connect using saved credentials
  //   If connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  //   If empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())

  Slog.println(PSTR("[Wifi]: Attempting to connect to access point"));
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect(PSTR("AutoConnectAP")); // anonymous ap
  // res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

  if(!res) 
  {
    Slog.println(PSTR("[Wifi]: ERROR - Failed to connect to access point"));
    // ESP.restart();
  } 
  else 
  {
    Slog.println(PSTR("[Wifi]: Connection successful"));
    digitalWrite(WIFI_LED_PIN, WIFI_LED_ON);   // Turn on the Wifi LED to signify success
  }
}

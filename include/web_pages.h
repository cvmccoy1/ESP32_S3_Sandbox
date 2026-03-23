#pragma once

// Embedded web dashboard served from PROGMEM
// Dashboard provides real-time temperature, fan speed, RPM monitoring
// with SSR and mode control via WebSocket.

static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Reflow Controller</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:'Segoe UI',Arial,sans-serif;background:#1a1a2e;color:#e0e0e0;min-height:100vh;display:flex;flex-direction:column;align-items:center;padding:12px}
h1{color:#00d4ff;font-size:1.5em;margin-bottom:4px}
@media(min-width:500px){body{padding:40px 20px}}
.uptime{color:#888;font-size:0.8em;margin-bottom:4px}
.ip{color:#00d4ff;font-size:0.8em;margin-bottom:16px}
@media(min-width:500px){.uptime,.ip{font-size:0.85em;margin-bottom:20px}}
@media(min-width:500px){h1{font-size:1.6em}}
.dashboard{display:grid;grid-template-columns:1fr 1fr;gap:16px;width:100%;max-width:640px}
@media(max-width:480px){.dashboard{grid-template-columns:1fr}}
.card{background:#16213e;border-radius:12px;padding:16px;border:1px solid #0f3460;display:flex;flex-direction:column;align-items:center;gap:6px}
@media(min-width:500px){.card{padding:20px;gap:8px}}
.card.wide{grid-column:1/-1}
.card-label{font-size:0.75em;color:#888;text-transform:uppercase;letter-spacing:1px}
.temp-value{font-size:3em;font-weight:700;color:#00d4ff;line-height:1}
.temp-unit{font-size:0.9em;color:#888}
.temp-bar{width:100%;height:8px;background:#0f3460;border-radius:4px;margin-top:4px;overflow:hidden}
.temp-bar-fill{height:100%;border-radius:4px;transition:width 0.5s,background 0.5s;min-width:2px}
.temp-bar-fill.cold{background:#2196f3}
.temp-bar-fill.warm{background:#4caf50}
.temp-bar-fill.hot{background:#ff9800}
.temp-bar-fill.critical{background:#f44336}
.stage-badge{font-size:1.1em;font-weight:600;padding:6px 20px;border-radius:20px;text-transform:uppercase;letter-spacing:1px}
.stage-idle{background:#333;color:#888}
.stage-preheat{background:#1a237e;color:#5c6bc0}
.stage-soak{background:#e65100;color:#ffb74d}
.stage-reflow{background:#b71c1c;color:#ef5350}
.stage-cooling{background:#01579b;color:#4fc3f7}
.stage-complete{background:#1b5e20;color:#81c784}
.fan-value{font-size:2.5em;font-weight:700;color:#ff9800}
.fan-unit{font-size:0.9em;color:#888}
.rpm{font-size:1em;color:#ffb74d;margin-top:-4px}
.controls{display:grid;grid-template-columns:1fr 1fr;gap:12px;width:100%;max-width:640px;margin-top:4px}
@media(max-width:480px){.controls{grid-template-columns:1fr}}
.btn{background:#0f3460;border:none;border-radius:8px;padding:12px 16px;color:#e0e0e0;font-size:0.9em;cursor:pointer;transition:background 0.2s,transform 0.1s;width:100%}
.btn:hover{background:#1a5276}
.btn:active{transform:scale(0.97)}
.btn.on{background:#1b5e20;color:#81c784}
.btn.off{background:#7b1fa2;color:#ce93d8}
.btn.auto{background:#1a237e;color:#9fa8da}
.btn.manual{background:#e65100;color:#ffb74d}
.slider-wrap{width:100%;margin-top:4px}
.slider{width:100%;accent-color:#ff9800;cursor:pointer}
.slider-label{display:flex;justify-content:space-between;font-size:0.8em;color:#888;margin-top:2px}
.ws-status{display:inline-block;width:8px;height:8px;border-radius:50%;background:#888;margin-right:6px;vertical-align:middle}
.ws-status.connected{background:#4caf50}
.ws-status.disconnected{background:#f44336}
</style>
</head>
<body>
<h1>Reflow Controller</h1>
<div class="uptime" id="uptime">Uptime: 0s</div>
<div class="ip" id="ip">IP: connecting...</div>

<div class="dashboard">
  <div class="card">
    <div class="card-label">Temperature</div>
    <div class="temp-value"><span id="temp">--.-</span><span class="temp-unit">C</span></div>
    <div class="temp-bar"><div class="temp-bar-fill" id="tempBar"></div></div>
  </div>
  <div class="card">
    <div class="card-label">Fan Speed</div>
    <div class="fan-value"><span id="fan">--</span><span class="fan-unit">%</span></div>
    <div class="rpm"><span id="rpm">---</span> RPM</div>
  </div>
  <div class="card wide">
    <div class="card-label">Stage</div>
    <div class="stage-badge stage-idle" id="stage">IDLE</div>
  </div>
</div>

<div class="controls">
  <button class="btn" id="ssrBtn" onclick="toggleSSR()">SSR: OFF</button>
  <button class="btn auto" id="modeBtn" onclick="toggleMode()">Mode: AUTO</button>
  <button class="btn" id="buzzBtn" onclick="buzz()">Buzz</button>
  <button class="btn" id="refreshBtn" onclick="refreshStatus()">Refresh</button>
</div>

<div class="controls" style="margin-top:8px">
  <div class="card wide slider-wrap">
    <div class="card-label">Manual Fan Control <span id="fanSliderVal" style="color:#ff9800">--</span>%</div>
    <input type="range" class="slider" id="fanSlider" min="0" max="100" value="0" oninput="setFan(this.value)">
    <div class="slider-label"><span>0%</span><span>100%</span></div>
  </div>
</div>

<div style="margin-top:16px;font-size:0.75em;color:#555">
  <span class="ws-status" id="wsStatus"></span><span id="wsLabel">WebSocket: disconnected</span>
</div>

<script>
let ws = null;
let autoMode = true;
let ssrOn = false;

function connectWs() {
  const proto = location.protocol === 'https:' ? 'wss:' : 'ws:';
  ws = new WebSocket(`${proto}//${location.host}`);
  ws.onopen = () => {
    document.getElementById('wsStatus').className = 'ws-status connected';
    document.getElementById('wsLabel').textContent = 'WebSocket: connected';
    refreshStatus();
  };
  ws.onmessage = (e) => {
    if (typeof e.data !== 'string') return;
    try {
      const d = JSON.parse(e.data);
      document.getElementById('temp').textContent = d.temp.toFixed(1);
      document.getElementById('fan').textContent = d.fan.toFixed(0);
      document.getElementById('rpm').textContent = d.rpm;
      document.getElementById('uptime').textContent = 'Uptime: ' + formatUptime(d.uptime);
      if (d.ip) document.getElementById('ip').textContent = 'IP: ' + d.ip;
      const pct = Math.min(100, (d.temp / 300) * 100);
      const bar = document.getElementById('tempBar');
      bar.style.width = pct + '%';
      bar.className = 'temp-bar-fill ' + tempClass(d.temp);
      const stage = document.getElementById('stage');
      stage.textContent = d.stage;
      stage.className = 'stage-badge stage-' + d.stage.toLowerCase();
      autoMode = d.mode === 'auto';
      const modeBtn = document.getElementById('modeBtn');
      modeBtn.textContent = 'Mode: ' + (autoMode ? 'AUTO' : 'MANUAL');
      modeBtn.className = 'btn ' + (autoMode ? 'auto' : 'manual');
      ssrOn = d.ssr === 'on';
      const ssrBtn = document.getElementById('ssrBtn');
      ssrBtn.textContent = 'SSR: ' + (ssrOn ? 'ON' : 'OFF');
      ssrBtn.className = 'btn ' + (ssrOn ? 'on' : 'off');
      const fanSlider = document.getElementById('fanSlider');
      fanSlider.value = d.fan;
      fanSlider.disabled = autoMode;
      document.getElementById('fanSliderVal').textContent = d.fan.toFixed(0);
    } catch(err) {}
  };
  ws.onclose = () => {
    document.getElementById('wsStatus').className = 'ws-status disconnected';
    document.getElementById('wsLabel').textContent = 'WebSocket: disconnected (reconnecting...)';
    setTimeout(connectWs, 3000);
  };
  ws.onerror = () => ws.close();
}

function tempClass(t) {
  if (t < 30) return 'cold';
  if (t < 50) return 'warm';
  if (t < 70) return 'hot';
  return 'critical';
}

function formatUptime(s) {
  if (s < 60) return s + 's';
  if (s < 3600) return Math.floor(s/60) + 'm ' + (s%60) + 's';
  return Math.floor(s/3600) + 'h ' + Math.floor((s%3600)/60) + 'm';
}

function wsSend(msg) { if (ws && ws.readyState === WebSocket.OPEN) ws.send(msg); }
function toggleSSR() { wsSend('toggle_ssr'); }
function toggleMode() { wsSend('toggle_mode'); }
function buzz() { wsSend('buzz'); }
function refreshStatus() { wsSend('get_status'); }
function setFan(v) {
  document.getElementById('fanSliderVal').textContent = v;
  wsSend('set_fan:' + v);
}

connectWs();
</script>
</body>
</html>
)rawliteral";

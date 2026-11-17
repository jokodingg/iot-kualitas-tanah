#include "web.h"

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "config.h"
#include "sensors.h"
#include "soil_logic.h"

extern PumpController pump;
extern Reading lastReading;

static AsyncWebServer server(80);

static const char PAGE[] PROGMEM = R"HTML(<!doctype html>
<html lang="id"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Kualitas Tanah</title>
<style>
body{font-family:system-ui,sans-serif;margin:0;padding:16px;background:#f4f1ea;color:#2b2a26}
h1{font-size:1.3rem;margin:0 0 16px}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.card{background:#fff;border-radius:10px;padding:14px}
.label{font-size:.8rem;color:#6b675e}
.val{font-size:1.6rem;font-weight:600;margin-top:4px}
.row{display:flex;gap:10px;margin-top:16px}
button{flex:1;font-size:1rem;padding:14px;border:0;border-radius:10px;background:#3d6b35;color:#fff}
button:disabled{background:#b9b5aa}
#mode{background:#2b2a26}
</style></head><body>
<h1>Kualitas Tanah</h1>
<div class="grid">
<div class="card"><div class="label">Kelembapan</div><div class="val" id="m">-</div></div>
<div class="card"><div class="label">Suhu tanah</div><div class="val" id="t">-</div></div>
<div class="card"><div class="label">EC (relatif)</div><div class="val" id="ec">-</div></div>
<div class="card"><div class="label">Pompa</div><div class="val" id="p">-</div></div>
</div>
<div class="row"><button id="mode">Mode: -</button></div>
<div class="row"><button id="on">Pompa ON</button><button id="off">Pompa OFF</button></div>
<script>
let s={};
function show(d){s=d;
m.textContent=d.moisture.toFixed(0)+' %';
t.textContent=d.tempValid?d.tempC.toFixed(1)+' °C':'--';
ec.textContent=d.ecUs<0?'--':d.ecUs.toFixed(0)+' µS/cm';
p.textContent=d.pumpOn?'ON':'OFF';
mode.textContent='Mode: '+(d.mode=='auto'?'AUTO':'MANUAL');
const man=d.mode=='manual';on.disabled=!man;off.disabled=!man;}
function get(){fetch('/data').then(r=>r.json()).then(show).catch(()=>{});}
function post(u){fetch(u,{method:'POST'}).then(r=>r.json()).then(show);}
mode.onclick=()=>post('/mode?m='+(s.mode=='auto'?'manual':'auto'));
on.onclick=()=>post('/pump?on=1');off.onclick=()=>post('/pump?on=0');
get();setInterval(get,2000);
</script></body></html>)HTML";

static String stateJson() {
  char buf[200];
  snprintf(buf, sizeof(buf),
           "{\"moisture\":%.1f,\"tempC\":%.1f,\"tempValid\":%s,\"ecUs\":%.0f,\"pumpOn\":%s,\"mode\":\"%s\"}",
           lastReading.moisture, lastReading.tempC, lastReading.tempValid ? "true" : "false",
           lastReading.ecUs, pump.isOn() ? "true" : "false",
           pump.mode() == PumpMode::Auto ? "auto" : "manual");
  return String(buf);
}

static void sendState(AsyncWebServerRequest* req) {
  req->send(200, "application/json", stateJson());
}

void webBegin() {
  WiFi.softAP(AP_SSID, AP_PASS);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send(200, "text/html", PAGE);
  });
  server.on("/data", HTTP_GET, sendState);

  // Only change the controller's state here. loop() drives the relay,
  // so the 10 s limit applies to manual runs as well.
  server.on("/mode", HTTP_POST, [](AsyncWebServerRequest* req) {
    String m = req->hasParam("m") ? req->getParam("m")->value() : "";
    if (m == "auto") pump.setMode(PumpMode::Auto);
    else if (m == "manual") pump.setMode(PumpMode::Manual);
    else return req->send(400, "text/plain", "m harus auto atau manual");
    sendState(req);
  });
  server.on("/pump", HTTP_POST, [](AsyncWebServerRequest* req) {
    String on = req->hasParam("on") ? req->getParam("on")->value() : "";
    if (on != "1" && on != "0") return req->send(400, "text/plain", "on harus 1 atau 0");
    if (pump.mode() != PumpMode::Manual) return req->send(409, "text/plain", "ubah ke mode manual dulu");
    pump.manualSet(on == "1");
    sendState(req);
  });

  server.begin();
}

# Alat Kualitas Tanah + Siram Otomatis Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build firmware for an ESP32-C3 Super Mini that reads soil moisture, EC, and temperature; shows the readings on a 16x2 LCD; waters automatically with a pump; and (in stage 2) serves a local web dashboard.

**Architecture:** The pure logic lives in `lib/soil_logic`: moisture conversion, EC conversion, and the pump state machine. It has no Arduino dependency, so it can be tested on the PC (`pio test -e native`). `src/` holds only the hardware code (sensors, LCD, relay, web), which calls the logic library. Hardware is verified manually on the device.

**Tech Stack:** PlatformIO, Arduino framework (espressif32), Unity test, libraries: `OneWire`, `DallasTemperature`, `LiquidCrystal_I2C`, `ESPAsyncWebServer` + `AsyncTCP` (stage 2).

**Spec:** `docs/superpowers/specs/2026-11-16-alat-kualitas-tanah-design.md`

## Global Constraints

- Pins: moisture GPIO0, TDS GPIO1, DS18B20 GPIO3, relay GPIO4, I2C SDA GPIO8 / SCL GPIO9.
- Sensors are read every 2000 ms; each ADC value is the average of 10 samples.
- Pump: ON when moisture < 35%, OFF when moisture > 55% or after 10000 ms on, then 60000 ms lockout. All values are constants in `include/config.h`.
- Manual mode is also cut off at 10000 ms.
- LCD line 1 `M:42% T:28.5C`, line 2 `EC:850us P:OFF`.
- Stage 2: WiFi AP mode, page at `192.168.4.1`, JSON endpoint `/data`, refreshed every 2 s.
- No cloud.

## Review Focus

1. DS18B20 disconnected (reads `-127`): EC must use a fallback of 25 °C, and the LCD shows `T:--`. Test: `test_ec_uses_25c_when_temp_invalid` (Task 1).
2. ADC outside the calibration range (sensor drier than air or wetter than water): moisture is clamped to 0–100. Test: `test_moisture_clamped` (Task 1).
3. `millis()` overflow: the 10 s and 60 s timers must use unsigned subtraction. Test: `test_pump_timer_survives_millis_wrap` (Task 1).
4. Pump ON from the web in MANUAL mode with no OFF request: the pump must still stop at 10 s. Test: `test_manual_on_capped` (Task 1).
5. Switching MANUAL→AUTO while the pump is on: the pump must follow the AUTO rules on the next tick, not stay on. Test: `test_mode_switch_reevaluates` (Task 1).

---

### Task 1: Project setup + logic library (TDD on PC)

**Files:**
- Create: `platformio.ini`, `include/config.h`, `lib/soil_logic/soil_logic.h`, `lib/soil_logic/soil_logic.cpp`, `test/test_logic/test_main.cpp`, `.gitignore` (`.pio/`)

**Interfaces:**
- Produces:
  - `float moisturePercent(int adc, int adcDry, int adcWet)` — linear, clamped to 0–100 (dry = higher ADC value).
  - `float ecFromVoltage(float volts, float tempC)` — µS/cm. `tempC` outside -20..80 → use 25.
  - `enum class PumpMode { Auto, Manual };`
  - `class PumpController { PumpController(float onBelow, float offAbove, uint32_t maxOnMs, uint32_t lockoutMs); bool update(uint32_t nowMs, float moisture); void setMode(PumpMode); PumpMode mode() const; void manualSet(bool on); bool isOn() const; }`

- [ ] **Step 1: `git init` and create `platformio.ini`**

Two envs: `[env:c3]` with `platform = espressif32`, `board = esp32-c3-devkitm-1`, `framework = arduino`, `build_flags = -DARDUINO_USB_CDC_ON_BOOT=1`, `monitor_speed = 115200`, and `lib_deps` for the libraries above; and `[env:native]` with `platform = native`, `test_framework = unity`.

- [ ] **Step 2: Write `include/config.h`**

Pins, `ON_BELOW=35`, `OFF_ABOVE=55`, `MAX_ON_MS=10000`, `LOCKOUT_MS=60000`, `READ_INTERVAL_MS=2000`, `ADC_SAMPLES=10`, `ADC_DRY=3000`, `ADC_WET=1300` (placeholder values, replaced in Task 2), `AP_SSID="KualitasTanah"`, `AP_PASS="tanah123"`.

- [ ] **Step 3: Write the failing tests**

```cpp
void test_moisture_endpoints()   { TEST_ASSERT_FLOAT_WITHIN(0.1, 0,   moisturePercent(3000,3000,1300));
                                   TEST_ASSERT_FLOAT_WITHIN(0.1, 100, moisturePercent(1300,3000,1300));
                                   TEST_ASSERT_FLOAT_WITHIN(0.5, 50,  moisturePercent(2150,3000,1300)); }
void test_moisture_clamped()     { TEST_ASSERT_EQUAL_FLOAT(0,   moisturePercent(3500,3000,1300));
                                   TEST_ASSERT_EQUAL_FLOAT(100, moisturePercent(900,3000,1300)); }
void test_ec_zero_volts()        { TEST_ASSERT_EQUAL_FLOAT(0, ecFromVoltage(0, 25)); }
void test_ec_known_point()       { TEST_ASSERT_FLOAT_WITHIN(1, 735.0, ecFromVoltage(1.0, 25)); }
void test_ec_temp_compensation() { TEST_ASSERT_TRUE(ecFromVoltage(1.0, 35) < ecFromVoltage(1.0, 25)); }
void test_ec_uses_25c_when_temp_invalid() { TEST_ASSERT_EQUAL_FLOAT(ecFromVoltage(1.0,25), ecFromVoltage(1.0,-127)); }
// PumpController p(35,55,10000,60000):
void test_pump_on_when_dry()     // update(0, 30) == true
void test_pump_hysteresis()      // after ON: update(t,45) stays true; update(t,56) == false
void test_pump_max_on()          // update(0,30) true; update(10000,30) == false
void test_pump_lockout()         // after OFF at 10000: update(69999,30) false; update(70000,30) true
void test_pump_timer_survives_millis_wrap() // start at 0xFFFFF000, off after 10000 ms
void test_manual_on_capped()     // Manual, manualSet(true): update(5000,80) true; update(10000,80) false
void test_manual_ignores_moisture() // Manual, no manualSet: update(0,10) false
void test_mode_switch_reevaluates() // Manual ON at moisture 80, setMode(Auto), update -> false
```

- [ ] **Step 4: Run `pio test -e native`** — Expected: FAIL (compile error, functions not defined).

- [ ] **Step 5: Implement `soil_logic.cpp`**

EC uses the DFRobot formula:

```cpp
float comp = 1.0f + 0.02f * (t - 25.0f);
float v = volts / comp;
float tds = (133.42f*v*v*v - 255.86f*v*v + 857.39f*v) * 0.5f;  // ppm
return tds * 2.0f;                                           // µS/cm
```

PumpController: store `onSinceMs` and `offSinceMs`, and compare with `now - since >= x` (unsigned). Lockout applies only after an OFF that ended an ON period; at boot there is no lockout. Manual OFF via `manualSet(false)` takes effect immediately.

- [ ] **Step 6: Run `pio test -e native`** — Expected: all PASS.

- [ ] **Step 7: Commit** `feat: soil logic library with tests`

### Task 2: Sensors + LCD + relay on the device (stage 1)

**Files:**
- Create: `src/main.cpp`, `src/sensors.h`, `src/sensors.cpp`, `src/display.h`, `src/display.cpp`

**Interfaces:**
- Consumes: all of Task 1.
- Produces:
  - `struct Reading { float moisture; float tempC; bool tempValid; float ecUs; };`
  - `void sensorsBegin(); Reading sensorsRead();`
  - `void displayBegin(); void displayShow(const Reading&, bool pumpOn);`
  - In `main.cpp`: global `PumpController pump` and `Reading lastReading` (used by Task 3).

- [ ] **Step 1: Implement `sensors.cpp`**

`analogReadMilliVolts` averaged over 10 samples. Moisture uses the raw ADC value with `ADC_DRY`/`ADC_WET`. The TDS voltage goes to `ecFromVoltage`. For DS18B20, `DEVICE_DISCONNECTED_C` → `tempValid=false`. Use `setWaitForConversion(false)` so `loop()` does not block 750 ms.

- [ ] **Step 2: Implement `display.cpp`**

LCD at address 0x27 (try 0x3F if the screen stays blank). `Wire.begin(8, 9)`. Use the format from Global Constraints, with `T:--` when temperature is invalid. Pad each line to 16 characters so leftover characters are overwritten.

- [ ] **Step 3: Implement `main.cpp`**

A non-blocking `loop` with `millis()`. Every 2000 ms: read sensors → `pump.update` → `digitalWrite(PIN_RELAY, ...)` → update the display → print one Serial line. Check whether the relay module is active-LOW; if so, use `RELAY_ACTIVE_LOW` in config.

- [ ] **Step 4: Build** `pio run -e c3` — Expected: SUCCESS.

- [ ] **Step 5: Calibrate on the device**

Upload and open `pio device monitor`. Read the raw ADC value with the moisture sensor in air, then in a glass of water. Write the values into `ADC_DRY`/`ADC_WET`.

- [ ] **Step 6: Manual check**

- Dry soil → the pump runs for at most 10 s and the LCD shows `P:ON`.
- Wet soil → the pump stays off.
- Unplug the DS18B20 → the LCD shows `T:--` and nothing crashes.

- [ ] **Step 7: Commit** `feat: stage 1 sensors, LCD, auto watering`

### Task 3: Local web dashboard (stage 2)

**Files:**
- Create: `src/web.h`, `src/web.cpp`
- Modify: `src/main.cpp` (call `webBegin()` in `setup`)

**Interfaces:**
- Consumes: `lastReading`, `pump` from Task 2.
- Produces: `void webBegin();`
  - `GET /` → an HTML page stored as a `PROGMEM` string. It calls `fetch('/data')` every 2 s.
  - `GET /data` → `{"moisture":42.0,"tempC":28.5,"tempValid":true,"ecUs":850,"pumpOn":false,"mode":"auto"}`
  - `POST /mode?m=auto|manual` and `POST /pump?on=1|0` → 200 with the same JSON; invalid parameter → 400.

- [ ] **Step 1: Implement `web.cpp`**

`WiFi.softAP(AP_SSID, AP_PASS)`. Mode and pump changes go only through `pump.setMode`/`pump.manualSet`. The relay is still driven from `loop()`, so the 10 s limit still applies. Use a simple page: four values, a mode toggle, and an ON/OFF button that is active only in manual mode.

- [ ] **Step 2: Build** `pio run -e c3` — Expected: SUCCESS.

- [ ] **Step 3: Manual check**

- Connect a phone to `KualitasTanah` and open `192.168.4.1`. The values match the LCD and update every 2 s.
- In manual mode, press ON → the pump turns off by itself after 10 s.
- Switch back to auto → the pump follows the moisture readings.
- `curl -X POST "192.168.4.1/pump?on=x"` → 400.

- [ ] **Step 4: Commit** `feat: stage 2 local web dashboard`

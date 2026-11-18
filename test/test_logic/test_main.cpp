#include <unity.h>
#include "soil_logic.h"

void setUp() {}
void tearDown() {}

static PumpController make() { return PumpController(35, 55, 10000, 60000); }

void test_moisture_endpoints() {
  TEST_ASSERT_FLOAT_WITHIN(0.1, 0, moisturePercent(3000, 3000, 1300));
  TEST_ASSERT_FLOAT_WITHIN(0.1, 100, moisturePercent(1300, 3000, 1300));
  TEST_ASSERT_FLOAT_WITHIN(0.5, 50, moisturePercent(2150, 3000, 1300));
}
void test_moisture_clamped() {
  TEST_ASSERT_EQUAL_FLOAT(0, moisturePercent(3500, 3000, 1300));
  TEST_ASSERT_EQUAL_FLOAT(100, moisturePercent(900, 3000, 1300));
}
void test_pump_on_when_dry() {
  auto p = make();
  TEST_ASSERT_TRUE(p.update(0, 30));
}
void test_pump_hysteresis() {
  auto p = make();
  p.update(0, 30);
  TEST_ASSERT_TRUE(p.update(2000, 45));
  TEST_ASSERT_FALSE(p.update(4000, 56));
}
void test_pump_stays_off_in_band() {
  auto p = make();
  TEST_ASSERT_FALSE(p.update(0, 45));
}
void test_pump_max_on() {
  auto p = make();
  TEST_ASSERT_TRUE(p.update(0, 30));
  TEST_ASSERT_TRUE(p.update(9999, 30));
  TEST_ASSERT_FALSE(p.update(10000, 30));
}
void test_pump_lockout() {
  auto p = make();
  p.update(0, 30);
  p.update(10000, 30);
  TEST_ASSERT_FALSE(p.update(69999, 30));
  TEST_ASSERT_TRUE(p.update(70000, 30));
}
void test_pump_timer_survives_millis_wrap() {
  auto p = make();
  uint32_t t0 = 0xFFFFF000u;
  TEST_ASSERT_TRUE(p.update(t0, 30));
  TEST_ASSERT_TRUE(p.update(t0 + 9999u, 30));
  TEST_ASSERT_FALSE(p.update(t0 + 10000u, 30));
}
void test_manual_on_capped() {
  auto p = make();
  p.setMode(PumpMode::Manual);
  p.manualSet(true);
  TEST_ASSERT_TRUE(p.update(0, 80));
  TEST_ASSERT_TRUE(p.update(5000, 80));
  TEST_ASSERT_FALSE(p.update(10000, 80));
}
void test_manual_off_immediate() {
  auto p = make();
  p.setMode(PumpMode::Manual);
  p.manualSet(true);
  p.update(0, 80);
  p.manualSet(false);
  TEST_ASSERT_FALSE(p.update(1000, 80));
}
void test_manual_ignores_moisture() {
  auto p = make();
  p.setMode(PumpMode::Manual);
  TEST_ASSERT_FALSE(p.update(0, 10));
}
void test_mode_switch_reevaluates() {
  auto p = make();
  p.setMode(PumpMode::Manual);
  p.manualSet(true);
  TEST_ASSERT_TRUE(p.update(0, 80));
  p.setMode(PumpMode::Auto);
  TEST_ASSERT_FALSE(p.update(2000, 80));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_moisture_endpoints);
  RUN_TEST(test_moisture_clamped);
  RUN_TEST(test_pump_on_when_dry);
  RUN_TEST(test_pump_hysteresis);
  RUN_TEST(test_pump_stays_off_in_band);
  RUN_TEST(test_pump_max_on);
  RUN_TEST(test_pump_lockout);
  RUN_TEST(test_pump_timer_survives_millis_wrap);
  RUN_TEST(test_manual_on_capped);
  RUN_TEST(test_manual_off_immediate);
  RUN_TEST(test_manual_ignores_moisture);
  RUN_TEST(test_mode_switch_reevaluates);
  return UNITY_END();
}

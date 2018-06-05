#include <stdbool.h>
#include <stdio.h>
#include <munit/munit.h>
#include "pump_control.h"

#define MUNIT_TESTCASE(name) MunitResult name(const MunitParameter params[], void* user_data)
#define MUNIT_TEST(name) {#name, name, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
#define MUNIT_TEST_END   {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}

static MUNIT_TESTCASE(test_temperature_conversion) {
	for (int i = VAL_0_DEG; i <= VAL_100_DEG; ++i)
		munit_assert_int(i, ==, sensor_temp(real_temp(i)));

	return MUNIT_OK;
}

static MUNIT_TESTCASE(test_state_sensor_error) {
	munit_assert_int(get_pump_state(STATE_TEMP_LOW, 0, 0, 0), ==, SENSOR_ERROR);
	munit_assert_int(get_pump_state(STATE_TEMP_LOW, 0xffff, 0xffff, 0xffff), ==, SENSOR_ERROR);

	return MUNIT_OK;
}

static MUNIT_TESTCASE(test_state_change_temp_low) {
	munit_assert_int(get_pump_state_f(STATE_TEMP_LOW, 40, 50, 30), ==, STATE_TEMP_LOW);

	munit_assert_int(get_pump_state_f(STATE_PUMPING, 70, 80, 60), ==, STATE_TEMP_LOW);
	munit_assert_int(get_pump_state_f(STATE_TEMP_LOW, 70, 80, 60), ==, STATE_TEMP_LOW);

	munit_assert_int(get_pump_state_f(STATE_TEMP_OK, 55, 60, 60), ==, STATE_TEMP_LOW);
	munit_assert_int(get_pump_state_f(STATE_TEMP_LOW, 55, 60, 60), ==, STATE_TEMP_LOW);

	return MUNIT_OK;
}

static MUNIT_TESTCASE(test_state_change_pumping) {
	munit_assert_int(get_pump_state_f(STATE_TEMP_LOW, 62, 50, 30), ==, STATE_PUMPING);
	munit_assert_int(get_pump_state_f(STATE_PUMPING, 62, 50, 30), ==, STATE_PUMPING);

	munit_assert_int(get_pump_state_f(STATE_TEMP_OK, 75, 60, 60), ==, STATE_PUMPING);
	munit_assert_int(get_pump_state_f(STATE_PUMPING, 75, 60, 60), ==, STATE_PUMPING);

	munit_assert_int(get_pump_state_f(STATE_PUMPING, 65, 60, 59), ==, STATE_PUMPING);

	return MUNIT_OK;
}

static MUNIT_TESTCASE(test_state_change_temp_ok) {
	munit_assert_int(get_pump_state_f(STATE_PUMPING, 60, 50, 56), ==, STATE_TEMP_LOW); // XXX
	munit_assert_int(get_pump_state_f(STATE_TEMP_OK, 60, 50, 55), ==, STATE_TEMP_LOW); // XXX

	munit_assert_int(get_pump_state_f(STATE_TEMP_OK, 60, 50, 50), ==, STATE_TEMP_OK);

	return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {
	MUNIT_TEST(test_temperature_conversion),
	MUNIT_TEST(test_state_sensor_error),
	MUNIT_TEST(test_state_change_temp_low),
	MUNIT_TEST(test_state_change_pumping),
	MUNIT_TEST(test_state_change_temp_ok),
	MUNIT_TEST_END
};

static const MunitSuite test_suite = {
	"", test_suite_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char** argv) {
	return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}

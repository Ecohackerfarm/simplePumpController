#include <stdbool.h>
#include <stdio.h>
#include <munit/munit.h>
#include "pump_control.h"

#define MUNIT_TESTCASE(name) MunitResult name(const MunitParameter params[], void* user_data)
#define MUNIT_TEST(name) {#name, name, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
#define MUNIT_TEST_END   {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}

#define PUMP_TEST_RAW(state, src, target, dest, expected) \
	munit_assert_int(get_pump_state(state, src, target, dest), ==, expected)

#define PUMP_TEST(state, src, target, dest, expected) \
	munit_assert_int(get_pump_state_f(state, src, target, dest), ==, expected)

static MUNIT_TESTCASE(test_temperature_conversion) {
	for (int i = VAL_0_DEG; i <= VAL_100_DEG; ++i)
		munit_assert_int(i, ==, sensor_temp(real_temp(i)));

	return MUNIT_OK;
}

static MUNIT_TESTCASE(test_state_sensor_error) {
	PUMP_TEST_RAW(STATE_TEMP_LOW, 0, 0, 0, SENSOR_ERROR);
	PUMP_TEST_RAW(STATE_TEMP_LOW, 0xffff, 0xffff, 0xffff, SENSOR_ERROR);

	PUMP_TEST_RAW(STATE_PUMPING, 0, 0, 0, SENSOR_ERROR);
	PUMP_TEST_RAW(STATE_PUMPING, 0xffff, 0xffff, 0xffff, SENSOR_ERROR);

	return MUNIT_OK;
}

static MUNIT_TESTCASE(test_state_change_temp_low) {

	PUMP_TEST(STATE_TEMP_LOW, 40, 50, 20, STATE_TEMP_LOW);

	PUMP_TEST(STATE_PUMPING, 60, 50, 55, STATE_TEMP_LOW);

	PUMP_TEST(STATE_PUMPING, 50, 50, 50, STATE_TEMP_LOW);

	PUMP_TEST(STATE_PUMPING, 45, 50, 40, STATE_TEMP_LOW);
	PUMP_TEST(STATE_TEMP_LOW, 45, 50, 40, STATE_TEMP_LOW);
	PUMP_TEST(STATE_TEMP_LOW, 46, 50, 40, STATE_TEMP_LOW);
	PUMP_TEST(STATE_TEMP_LOW, 55, 50, 40, STATE_TEMP_LOW);

	return MUNIT_OK;
}

static MUNIT_TESTCASE(test_state_change_pumping) {

	PUMP_TEST(STATE_TEMP_LOW, 56, 50, 40, STATE_PUMPING);
	PUMP_TEST(STATE_PUMPING, 56, 50, 40, STATE_PUMPING);
	PUMP_TEST(STATE_PUMPING, 50, 50, 40, STATE_PUMPING);

	PUMP_TEST(STATE_PUMPING, 60, 50, 54, STATE_PUMPING);

	return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {
	MUNIT_TEST(test_temperature_conversion),
	MUNIT_TEST(test_state_sensor_error),
	MUNIT_TEST(test_state_change_temp_low),
	MUNIT_TEST(test_state_change_pumping),
	MUNIT_TEST_END
};

static const MunitSuite test_suite = {
	"", test_suite_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char** argv) {
	return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}

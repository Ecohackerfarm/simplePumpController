#include <stdbool.h>
#include <stdio.h>
#include <munit/munit.h>
#include "pump_control.h"

static MunitResult test_temperature_conversion(const MunitParameter params[], void* user_data) {
	for (int i = VAL_0_DEG; i <= VAL_100_DEG; ++i)
		munit_assert_int(i, ==, sensor_temp(real_temp(i)));

	return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {
	{"test_temperature_conversion,", test_temperature_conversion, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

static const MunitSuite test_suite = {
	"", test_suite_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char** argv) {
	return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}

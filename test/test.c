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

static MunitTest test_suite_tests[] = {
	MUNIT_TEST(test_temperature_conversion),
	MUNIT_TEST_END
};

static const MunitSuite test_suite = {
	"", test_suite_tests, NULL, 0, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char** argv) {
	return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}

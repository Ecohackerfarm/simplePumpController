#include <stdint.h>

#define VAL_HYST	6
#define VAL_0_DEG	430
#define VAL_100_DEG	625

typedef enum {
	SENSOR_ERROR,
	STATE_TEMP_LOW,
	STATE_PUMPING,
	STATE_TEMP_OK
} pump_state_t;

static inline float real_temp(uint16_t input) {
        return (input - VAL_0_DEG) * 100.f / (VAL_100_DEG - VAL_0_DEG);
}

static inline uint16_t sensor_temp(float temp) {
	return temp * (VAL_100_DEG - VAL_0_DEG) / 100.f + VAL_0_DEG;
}

static inline pump_state_t get_pump_state(pump_state_t state, uint16_t source_temp, uint16_t target_temp, uint16_t dest_temp) {
	if (source_temp < VAL_0_DEG || source_temp > VAL_100_DEG)
		return SENSOR_ERROR;

	if (target_temp < VAL_0_DEG || target_temp > VAL_100_DEG)
		return SENSOR_ERROR;

	if (dest_temp < VAL_0_DEG || dest_temp > VAL_100_DEG)
		return SENSOR_ERROR;

	if (source_temp < dest_temp)
		return STATE_TEMP_LOW;

	if (source_temp < target_temp - VAL_HYST)
		return STATE_TEMP_LOW;

	if (source_temp > target_temp + VAL_HYST)
		return STATE_PUMPING;

	if (dest_temp > target_temp + VAL_HYST)
		return STATE_TEMP_OK;

	return state;
}

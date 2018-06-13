#include <stdint.h>

// values are sensor values
#define VAL_HYST	10	// Hysteresis
#define VAL_TEMP_LOSS	10	// Temperature lost on the way
#define VAL_0_DEG	430
#define VAL_100_DEG	625

typedef enum {
	SENSOR_ERROR,
	STATE_TEMP_LOW,
	STATE_PUMPING
} pump_state_t;

// helper function to convert sensor value to temperature in °C
static inline float real_temp(uint16_t input) {
        return (input - VAL_0_DEG) * 100.f / (VAL_100_DEG - VAL_0_DEG);
}

// helper function to convert temperature in °C to sensor value
static inline uint16_t sensor_temp(float temp) {
	return temp * (VAL_100_DEG - VAL_0_DEG) / 100.f + VAL_0_DEG;
}

// get new state based on previous state and sensor readings
static inline pump_state_t get_pump_state(pump_state_t state, uint16_t source_temp, uint16_t target_temp, uint16_t dest_temp) {
	if (source_temp < VAL_0_DEG || source_temp > VAL_100_DEG)
		return SENSOR_ERROR;

	if (target_temp < VAL_0_DEG || target_temp > VAL_100_DEG)
		return SENSOR_ERROR;

	if (dest_temp < VAL_0_DEG || dest_temp > VAL_100_DEG)
		return SENSOR_ERROR;

	// pump until the destination is as hot as the source
	if (source_temp <= dest_temp + VAL_TEMP_LOSS)
		return STATE_TEMP_LOW;

	// pump unitl the source temp dropped below the desired temperature
	if (source_temp < target_temp - VAL_HYST)
		return STATE_TEMP_LOW;

	// start pumping if source temp has reached the desired temperature
	// and is higher than the destination temperature
	if (source_temp > target_temp + VAL_HYST && source_temp > dest_temp + VAL_TEMP_LOSS)
		return STATE_PUMPING;

	return state;
}

// helper function to make writing tetscases easier
static inline pump_state_t get_pump_state_f(pump_state_t state, uint16_t source_temp, uint16_t target_temp, uint16_t dest_temp) {
	return get_pump_state(state, sensor_temp(source_temp), sensor_temp(target_temp), sensor_temp(dest_temp));
}

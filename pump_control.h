#include <stdint.h>

#define VAL_HYST	6
#define VAL_0_DEG	430
#define VAL_100_DEG	625

static inline float real_temp(uint16_t input) {
        return (input - VAL_0_DEG) * 100.f / (VAL_100_DEG - VAL_0_DEG);
}

static inline uint16_t sensor_temp(float temp) {
	return temp * (VAL_100_DEG - VAL_0_DEG) / 100.f + VAL_0_DEG;
}

#include "helpers.h"

uint16_t* memset16(uint16_t *m, uint16_t val, int count) {
	uint16_t *buf = m;

	while (count--)
		*buf++ = val;
	return m;
}

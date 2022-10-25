#ifndef ST7789_LCD_SRC_HELPERS_H_
#define ST7789_LCD_SRC_HELPERS_H_

#include <stdint.h>

#define abs(x) ((x) > 0 ? (x) : -(x))
uint16_t* memset16(uint16_t *m, uint16_t val, int count);

#ifndef swap_uint16_t
#define swap_uint16_t(a, b)                                                   \
  {                                                                            \
	uint16_t t = a;                                                            \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif
#endif /* ST7789_LCD_SRC_HELPERS_H_ */

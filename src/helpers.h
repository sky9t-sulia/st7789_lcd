#ifndef ST7789_LCD_SRC_HELPERS_H_
#define ST7789_LCD_SRC_HELPERS_H_

#include <stdint.h>

#define abs(x) ((x) > 0 ? (x) : -(x))
#define swap_color(color) ((color << 8) | (color >> 8))

#define swap_uint16_t(a, b)                                                    \
  {                                                                            \
	uint16_t t = a;                                                            \
    a = b;                                                                     \
    b = t;                                                                     \
  }

uint16_t* memset16(uint16_t *m, uint16_t val, int count);

#endif /* ST7789_LCD_SRC_HELPERS_H_ */

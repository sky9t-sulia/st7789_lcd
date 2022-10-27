#ifndef ST7789_H_
#define ST7789_H_

#include <stdint.h>
#include <malloc.h>

#include "helpers.h"
#include "constants.h"
#include "adapters/adapter.h"

class ST7789_LCD {
public:
	ST7789_LCD(uint16_t width, uint16_t height, ST7789_HW_Adapter *adapter);
	void set_rotation(uint8_t m);
	void fill_screen(uint16_t color);
	void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
	void draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
			uint16_t color);
	void draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
			uint16_t color);
	void draw_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
	void draw_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
	void draw_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
private:
	uint16_t hor_len;
	uint16_t width;
	uint16_t height;
	uint16_t x_shift;
	uint16_t y_shift;
	uint8_t rotation;
	ST7789_HW_Adapter *hw;
	void init();
	void set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};


#endif /* ST7789_H_ */

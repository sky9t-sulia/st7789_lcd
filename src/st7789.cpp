#include "st7789.h"

ST7789_LCD::ST7789_LCD(uint16_t width, uint16_t height,
		ST7789_HW_Adapter *adapter) {
	this->width = width;
	this->height = height;
	this->hw = adapter;
	this->rotation = 0;
	this->x_shift = 0;
	this->y_shift = 0;
	this->hor_len = 40;

	this->init();
}

void ST7789_LCD::init() {
	hw->rst_set();

	hw->delayms(120);
	hw->send_command(ST7789_COLMOD);		//	Set color mode
	hw->send_small_data(ST7789_COLOR_MODE_16bit);
	hw->send_command(0xB2);				//	Porch control
	{
		uint8_t data[] = { 0x0C, 0x0C, 0x00, 0x33, 0x33 };
		hw->send_data(data, sizeof(data));
	}
	set_rotation(this->rotation);	//	MADCTL (Display Rotation)

	/* Internal LCD Voltage generator settings */
	hw->send_command(0XB7);				//	Gate Control
	hw->send_small_data(0x35);			//	Default value
	hw->send_command(0xBB);				//	VCOM setting
	hw->send_small_data(0x19);			//	0.725v (default 0.75v for 0x20)
	hw->send_command(0xC0);				//	LCMCTRL
	hw->send_small_data(0x2C);			//	Default value
	hw->send_command(0xC2);				//	VDV and VRH command Enable
	hw->send_small_data(0x01);			//	Default value
	hw->send_command(0xC3);				//	VRH set
	hw->send_small_data(0x12);		//	+-4.45v (default +-4.1v for 0x0B)
	hw->send_command(0xC4);				//	VDV set
	hw->send_small_data(0x20);			//	Default value
	hw->send_command(0xC6);			//	Frame rate control in normal mode
	hw->send_small_data(0x0F);			//	Default value (60HZ)
	hw->send_command(0xD0);				//	Power control
	hw->send_small_data(0xA4);			//	Default value
	hw->send_small_data(0xA1);			//	Default value
	/**************** Division line ****************/

	hw->send_command(0xE0);
	{
		uint8_t data[] = { 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C,
				0x18, 0x0D, 0x0B, 0x1F, 0x23 };
		hw->send_data(data, sizeof(data));
	}

	hw->send_command(0xE1);
	{
		uint8_t data[] = { 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51,
				0x2F, 0x1F, 0x1F, 0x20, 0x23 };
		hw->send_data(data, sizeof(data));
	}
	hw->send_command(ST7789_INVON);	//	Inversion ON
	hw->send_command(ST7789_SLPOUT);	//	Out of sleep mode
	hw->send_command(ST7789_NORON);	//	Normal Display on
	hw->send_command(ST7789_DISPON);	//	Main screen turned on

	this->hw->delayms(50);
	this->fill_screen(BLACK);			//	Fill with Black.
}

void ST7789_LCD::set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	uint16_t x_start = x0 + this->x_shift, x_end = x1 + this->x_shift;
	uint16_t y_start = y0 + this->y_shift, y_end = y1 + this->y_shift;

	/* Column Address set */
	hw->send_command(ST7789_CASET);
	{
		uint8_t data[] = { (uint8_t)(x_start >> 8), (uint8_t)(x_start & 0xFF), (uint8_t)(x_end >> 8), (uint8_t)(x_end & 0xFF) };
		hw->send_data(data, sizeof(data));
	}

	/* Row Address set */
	hw->send_command(ST7789_RASET);
	{
		uint8_t data[] = { (uint8_t)(y_start >> 8), (uint8_t)(y_start & 0xFF),
				(uint8_t)(y_end >> 8), (uint8_t)(y_end & 0xFF) };
		hw->send_data(data, sizeof(data));
	}
	/* Write to RAM */
	hw->send_command(ST7789_RAMWR);
}

/**
 * ------------------------------------------------------------------------
 * PUBLIC METHODS
 * ------------------------------------------------------------------------
 */

/**
 * @brief Set the rotation direction of the display
 * @param m -> rotation parameter(please refer it in st7789.h)
 * @return none
 */
void ST7789_LCD::set_rotation(uint8_t m) {
	hw->send_command(ST7789_MADCTL);	// MADCTL

	uint8_t rotation;
	switch (m) {
	case 0:
		rotation = (uint8_t)ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB;
		break;
	case 1:
		rotation = (uint8_t)ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB;
		break;
	case 2:
		rotation = (uint8_t)ST7789_MADCTL_RGB;
		break;
	case 3:
		rotation = (uint8_t)ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB;
		break;
	default:
		break;
	}

	hw->send_data(&rotation, sizeof(rotation));
}

/**
 * @brief Fill the DisplayWindow with single color
 * @param color -> color to Fill with
 * @return none
 */
void ST7789_LCD::fill_screen(uint16_t color) {
	this->draw_fill_rect(0, 0, this->width, this->height, color);
}

/**
 * @brief Draw a Pixel
 * @param x&y -> coordinate to Draw
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789_LCD::draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
	if ((x >= this->width) || (y >= this->height))
		return;

	color = swap_color(color);

	this->set_address_window(x, y, x, y);
	hw->send_data(&color, sizeof(color));
}

/**
 * @brief Draw a line with single color
 * @param x1&y1 -> coordinate of the start point
 * @param x2&y2 -> coordinate of the end point
 * @param color -> color of the line to Draw
 * @return none
 */
void ST7789_LCD::draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
		uint16_t color) {
	uint16_t steep = abs(y2 - y1) > abs(x2 - x1);
	if (steep) {
		swap_uint16_t(x1, y1);
		swap_uint16_t(x2, y2);
	}

	if (x1 > x2) {
		swap_uint16_t(x1, x2);
		swap_uint16_t(y1, y2);
	}

	int16_t dx, dy;
	dx = x2 - x1;
	dy = abs(y2 - y1);

	int16_t err = dx / 2;
	int16_t ystep = (y1 < y2) ? 1 : -1;

	for (; x1 <= x2; x1++) {
		if (steep) {
			this->draw_pixel(y1, x1, color);
		} else {
			this->draw_pixel(x1, y1, color);
		}
		err -= dy;
		if (err < 0) {
			y1 += ystep;
			err += dx;
		}
	}

}

// Draw rectangle
// x X coordinate
// y Y coordinate
// width
// height
// color:color
void ST7789_LCD::draw_rect(uint16_t x, uint16_t y, uint16_t width,
		uint16_t height, uint16_t color) {
	this->draw_line(x, y, x + width, y, color);
	this->draw_line(x + width, y, x + width, y + height, color);
	this->draw_line(x + width, y + height, x, y + height, color);
	this->draw_line(x, y + height, x, y, color);
}

// Draw rectangle
// x X coordinate
// y Y coordinate
// width
// height
// color:color
void ST7789_LCD::draw_fill_rect(uint16_t x, uint16_t y, uint16_t width,
		uint16_t height, uint16_t color) {
	uint16_t i;

	color = swap_color(color);

	uint16_t size = this->hor_len * width * sizeof(uint16_t);
	uint16_t *buff = (uint16_t*) malloc(size);

	this->set_address_window(0, 0, width - 1, height - 1);

	if (hw->use_dma) {
		for (i = 0; i < height / this->hor_len; i++) {
			memset16(buff, color, size);
			hw->send_data(buff, size);
		}

		this->hw->spi_wait();
		free(buff);
	} else {
		uint16_t j;
		for (i = 0; i < width; i++) {
			for (j = 0; j < height; j++) {
				uint16_t data[] = { (uint16_t) color };
				hw->send_data(data, sizeof(data));
			}
		}
	}
}

// Draw circle
// x:Central X coordinate
// y:Central Y coordinate
// r:radius
// color:color
void ST7789_LCD::draw_circle(uint16_t x, uint16_t y, uint16_t r,
		uint16_t color) {
	int16_t _x;
	int16_t _y;
	int16_t err;
	int16_t old_err;

	_x = 0;
	_y = -r;
	err = 2 - 2 * r;
	do {
		this->draw_pixel(x - _x, y + _y, color);
		this->draw_pixel(x - _y, y - _x, color);
		this->draw_pixel(x + _x, y - _y, color);
		this->draw_pixel(x + _y, y + _x, color);
		if ((old_err = err) <= _x) {
			err += ++_x * 2 + 1;
		}
		if (old_err > _y || err > _x) {
			err += ++_y * 2 + 1;
		}
	} while (_y < 0);
}

// Draw circle of filling
// x0:Central X coordinate
// y0:Central Y coordinate
// r:radius
// color:color
void ST7789_LCD::draw_fill_circle(uint16_t x, uint16_t y, uint16_t r,
		uint16_t color) {
	int16_t _x;
	int16_t _y;
	int16_t err;
	int16_t old_err;
	int16_t change_x;

	_x = 0;
	_y = -r;
	err = 2 - 2 * r;
	change_x = 1;
	do {
		if (change_x) {
			this->draw_line(x - _x, y - _y, x - _x, y + _y, color);
			this->draw_line(x + _x, y - _y, x + _x, y + _y, color);
		}
		change_x = (old_err = err) <= x;
		if (change_x) {
			err += ++_x * 2 + 1;
		}
		if (old_err > _y || err > _x) {
			err += ++_y * 2 + 1;
		}
	} while (_y <= 0);
}

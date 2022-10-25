#include "st7789.h"

#ifdef USE_DMA
uint16_t DMA_MIN_SIZE = 16;
#define HOR_LEN 40 // Mind the resolution of your screen!
#endif

ST7789_LCD::ST7789_LCD(uint16_t width, uint16_t height, ST7789_HW_Adapter *adapter) {
	this->width = width;
	this->height = height;
	this->hw = adapter;
	this->rotation = 0;
	this->x_shift = 0;
	this->y_shift = 0;

	this->init();
}

void ST7789_LCD::send_command(uint8_t command) {
	this->hw->cs_set();
	this->hw->dc_unset();
	this->hw->spi_transmit(&command, sizeof(command));
	this->hw->cs_unset();
}

void ST7789_LCD::init() {
	this->hw->cs_unset();
	this->hw->rst_unset();
	this->hw->delayms(1);

	this->hw->rst_set();

	this->hw->delayms(120);
	this->send_command(ST7789_COLMOD);		//	Set color mode
	this->send_small_data(ST7789_COLOR_MODE_16bit);
	this->send_command(0xB2);				//	Porch control
	{
		uint8_t data[] = { 0x0C, 0x0C, 0x00, 0x33, 0x33 };
		this->send_data(data, sizeof(data));
	}
	this->set_rotation(this->rotation);	//	MADCTL (Display Rotation)

	/* Internal LCD Voltage generator settings */
	this->send_command(0XB7);				//	Gate Control
	this->send_small_data(0x35);			//	Default value
	this->send_command(0xBB);				//	VCOM setting
	this->send_small_data(0x19);			//	0.725v (default 0.75v for 0x20)
	this->send_command(0xC0);				//	LCMCTRL
	this->send_small_data(0x2C);			//	Default value
	this->send_command(0xC2);				//	VDV and VRH command Enable
	this->send_small_data(0x01);			//	Default value
	this->send_command(0xC3);				//	VRH set
	this->send_small_data(0x12);		//	+-4.45v (default +-4.1v for 0x0B)
	this->send_command(0xC4);				//	VDV set
	this->send_small_data(0x20);			//	Default value
	this->send_command(0xC6);			//	Frame rate control in normal mode
	this->send_small_data(0x0F);			//	Default value (60HZ)
	this->send_command(0xD0);				//	Power control
	this->send_small_data(0xA4);			//	Default value
	this->send_small_data(0xA1);			//	Default value
	/**************** Division line ****************/

	this->send_command(0xE0);
	{
		uint8_t data[] = { 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C,
				0x18, 0x0D, 0x0B, 0x1F, 0x23 };
		this->send_data(data, sizeof(data));
	}

	this->send_command(0xE1);
	{
		uint8_t data[] = { 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51,
				0x2F, 0x1F, 0x1F, 0x20, 0x23 };
		this->send_data(data, sizeof(data));
	}
	this->send_command(ST7789_INVON);	//	Inversion ON
	this->send_command(ST7789_SLPOUT);	//	Out of sleep mode
	this->send_command(ST7789_NORON);	//	Normal Display on
	this->send_command(ST7789_DISPON);	//	Main screen turned on

	this->hw->delayms(50);
	this->fill_screen(BLACK);			//	Fill with Black.
}

/**
 * @brief Write data to ST7789 controller
 * @param buff -> pointer of data buffer
 * @param buff_size -> size of the data buffer
 * @return none
 */
void ST7789_LCD::send_data(uint8_t *buff, size_t buff_size) {
	this->hw->cs_set();
	this->hw->dc_set();

	// split data in small chunks because HAL can't send more than 64K at once
	while (buff_size > 0) {
		uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;
#ifdef USE_DMA
		if (DMA_MIN_SIZE <= buff_size) {
			this->hw->spi_transmit_dma(buff, chunk_size);
			this->hw->spi_wait();
		} else {
			this->hw->spi_transmit(buff, chunk_size);
		}
#else
		this->hw->spi_transmit(buff, chunk_size);
#endif

		buff += chunk_size;
		buff_size -= chunk_size;
	}

	this->hw->cs_unset();
}

void ST7789_LCD::send_small_data(uint8_t data) {
	this->hw->cs_set();
	this->hw->dc_set();
	this->hw->spi_transmit(&data, sizeof(data));
	this->hw->cs_unset();
}

void ST7789_LCD::set_address_window(uint16_t x0, uint16_t y0, uint16_t x1,
		uint16_t y1) {
	this->hw->cs_set();
	uint16_t x_start = x0 + this->x_shift, x_end = x1 + this->x_shift;
	uint16_t y_start = y0 + this->y_shift, y_end = y1 + this->y_shift;

	/* Column Address set */
	this->send_command(ST7789_CASET);
	{
		uint8_t data[] = { (uint8_t) (x_start >> 8), (uint8_t) (x_start & 0xFF),
				(uint8_t) (x_end >> 8), (uint8_t) (x_end & 0xFF) };
		this->send_data(data, sizeof(data));
	}

	/* Row Address set */
	this->send_command(ST7789_RASET);
	{
		uint8_t data[] = { (uint8_t) (y_start >> 8), (uint8_t) (y_start & 0xFF),
				(uint8_t) (y_end >> 8), (uint8_t) (y_end & 0xFF) };
		this->send_data(data, sizeof(data));
	}
	/* Write to RAM */
	this->send_command(ST7789_RAMWR);
	this->hw->cs_unset();
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
	this->send_command(ST7789_MADCTL);	// MADCTL
	switch (m) {
	case 0:
		this->send_small_data(
		ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
		break;
	case 1:
		this->send_small_data(
		ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	case 2:
		this->send_small_data(ST7789_MADCTL_RGB);
		break;
	case 3:
		this->send_small_data(
		ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	default:
		break;
	}
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
	if ((x < 0) || (x >= this->width) || (y < 0) || (y >= this->height))
		return;

	uint8_t data[] = { (uint8_t) (color >> 8), (uint8_t) (color & 0xFF) };

	this->set_address_window(x, y, x, y);
	this->hw->cs_set();
	this->send_data(data, sizeof(data));
	this->hw->cs_unset();
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

	color = ST7789_COLOR_SWAP(color);

	uint16_t size = HOR_LEN * width * sizeof(uint16_t);
	uint16_t *buff = (uint16_t*) malloc(size);

	this->set_address_window(0, 0, width - 1, height - 1);
	this->hw->cs_set();

#ifdef USE_DMA
	for (i = 0; i < height / HOR_LEN; i++) {
		memset16(buff, color, size);
		this->send_data((uint8_t*) buff, size);
	}

	this->hw->spi_wait();
	free(buff);
#else
	uint16_t j;
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			uint8_t data[] =
					{ (uint8_t) color };
			this->send_data(data, sizeof(data));
		}
	}
#endif

	this->hw->cs_unset();
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

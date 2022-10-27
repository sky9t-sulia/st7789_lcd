#ifndef ST7789_LCD_SRC_ADAPTERS_ADAPTER_H_
#define ST7789_LCD_SRC_ADAPTERS_ADAPTER_H_

#include "stdint.h"

class ST7789_HW_Adapter {
public:
	uint8_t use_dma = 1;
	uint8_t dma_min_size = 16;

	virtual ~ST7789_HW_Adapter() {};
	virtual void delayms(uint16_t ms)  = 0;
	virtual void cs_set()  = 0;
	virtual void cs_unset() = 0;
	virtual void dc_set() = 0;
	virtual void dc_unset() = 0;
	virtual void rst_set() = 0;
	virtual void rst_unset() = 0;
	virtual void send_command(uint8_t command) = 0;
	virtual void send_small_data(uint8_t data) = 0;
	virtual void send_data(uint16_t *buff, int buff_size) = 0;
	virtual void send_data(uint8_t *buff, int buff_size) = 0;
	virtual void spi_wait() = 0;
};


#endif /* ST7789_LCD_SRC_ADAPTERS_ADAPTER_H_ */

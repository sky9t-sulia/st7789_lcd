/*
 * adapter.h
 *
 *  Created on: 25 окт. 2022 г.
 *      Author: umudo
 */

#ifndef ST7789_LCD_SRC_ADAPTERS_ADAPTER_H_
#define ST7789_LCD_SRC_ADAPTERS_ADAPTER_H_

#include "stdint.h"

class ST7789_HW_Adapter {
public:
	virtual ~ST7789_HW_Adapter() {};
	virtual void delayms(uint16_t ms)  = 0;
	virtual void cs_set()  = 0;
	virtual void cs_unset() = 0;
	virtual void dc_set() = 0;
	virtual void dc_unset() = 0;
	virtual void rst_set() = 0;
	virtual void rst_unset() = 0;
	virtual void spi_transmit(uint8_t *command, int size) = 0;
	virtual void spi_transmit_dma(uint8_t *buff, int size) = 0;
	virtual void spi_wait() = 0;
};


#endif /* ST7789_LCD_SRC_ADAPTERS_ADAPTER_H_ */

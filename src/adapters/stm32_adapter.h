#ifndef ST7789_LCD_SRC_ADAPTERS_STM32_ADAPTER_H_
#define ST7789_LCD_SRC_ADAPTERS_STM32_ADAPTER_H_

#include "stm32f4xx_hal.h"
#include "adapter.h"

typedef struct {
	GPIO_TypeDef *cs_port;
	GPIO_TypeDef *dc_port;
	GPIO_TypeDef *rst_port;
	uint16_t cs_pin;
	uint16_t dc_pin;
	uint16_t rst_pin;
} st7789_stm32_adapter_pinout_t;

class ST7789_STM32_Adapter : public ST7789_HW_Adapter {
public:
	ST7789_STM32_Adapter();
	st7789_stm32_adapter_pinout_t *pinout;
	SPI_HandleTypeDef *handle;
	void delayms(uint16_t ms);
	void cs_set();
	void cs_unset();
	void dc_set();
	void dc_unset();
	void rst_set();
	void rst_unset();
	void spi_transmit(uint8_t *command, int size);
	void spi_transmit_dma(uint8_t *buff, int size);
	void spi_wait();
};

#endif /* ST7789_LCD_SRC_ADAPTERS_STM32_ADAPTER_H_ */

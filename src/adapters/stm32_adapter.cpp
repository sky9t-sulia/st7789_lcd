#include "stm32_adapter.h"

ST7789_STM32_Adapter::ST7789_STM32_Adapter() {

}

void ST7789_STM32_Adapter::delayms(uint16_t ms) {
	HAL_Delay(ms);
}

void ST7789_STM32_Adapter::send_command(uint8_t command) {
	cs_set();
	dc_unset();
	HAL_SPI_Transmit(this->handle, &command, sizeof(command), HAL_MAX_DELAY);
	cs_unset();
}

void ST7789_STM32_Adapter::send_small_data(uint8_t data) {
	cs_set();
	dc_set();
	HAL_SPI_Transmit(this->handle, &data, sizeof(data), HAL_MAX_DELAY);
	cs_unset();
}

void ST7789_STM32_Adapter::send_data(uint8_t *buff, int buff_size) {
	cs_set();
	dc_set();

	while (buff_size > 0) {
		uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;

		if (this->use_dma) {
			if (this->dma_min_size <= buff_size) {
				HAL_SPI_Transmit_DMA(this->handle, buff, chunk_size);
				spi_wait();
			} else {
				HAL_SPI_Transmit(this->handle, buff, chunk_size, HAL_MAX_DELAY);
			}
		} else {
			HAL_SPI_Transmit(this->handle, buff, chunk_size, HAL_MAX_DELAY);
		}

		buff += chunk_size;
		buff_size -= chunk_size;
	}

	cs_unset();
}

void ST7789_STM32_Adapter::send_data(uint16_t *buff, int buff_size) {
	send_data((uint8_t*)buff, buff_size);
}

void ST7789_STM32_Adapter::spi_wait() {
	while (this->handle->hdmatx->State != HAL_DMA_STATE_READY) {
	}
}

void ST7789_STM32_Adapter::cs_set() {
	HAL_GPIO_WritePin(this->pinout->cs_port, this->pinout->cs_pin,
			GPIO_PIN_RESET);
}

void ST7789_STM32_Adapter::cs_unset() {
	HAL_GPIO_WritePin(this->pinout->cs_port, this->pinout->cs_pin,
			GPIO_PIN_SET);
}

void ST7789_STM32_Adapter::dc_set() {
	HAL_GPIO_WritePin(this->pinout->dc_port, this->pinout->dc_pin,
			GPIO_PIN_SET);
}

void ST7789_STM32_Adapter::dc_unset() {
	HAL_GPIO_WritePin(this->pinout->dc_port, this->pinout->dc_pin,
			GPIO_PIN_RESET);
}

void ST7789_STM32_Adapter::rst_set() {
	HAL_GPIO_WritePin(this->pinout->rst_port, this->pinout->rst_pin,
			GPIO_PIN_SET);
}

void ST7789_STM32_Adapter::rst_unset() {
	HAL_GPIO_WritePin(this->pinout->rst_port, this->pinout->rst_pin,
			GPIO_PIN_RESET);
}

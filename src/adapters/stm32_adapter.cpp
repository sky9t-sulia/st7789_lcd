/*
 * stm32.c
 *
 *  Created on: 25 окт. 2022 г.
 *      Author: umudo
 */

#include "stm32_adapter.h"

ST7789_STM32_Adapter::ST7789_STM32_Adapter() {

}

void ST7789_STM32_Adapter::delayms(uint16_t ms) {
	HAL_Delay(ms);
}

void ST7789_STM32_Adapter::spi_transmit(uint8_t *command, int size) {
	HAL_SPI_Transmit(this->handle, command, size, HAL_MAX_DELAY);
}

void ST7789_STM32_Adapter::spi_transmit_dma(uint8_t *buff, int size) {
	HAL_SPI_Transmit_DMA(this->handle, buff, size);
}

void ST7789_STM32_Adapter::spi_wait() {
	while (this->handle->hdmatx->State != HAL_DMA_STATE_READY) {
	}
}

void ST7789_STM32_Adapter::cs_set() {
	HAL_GPIO_WritePin(this->pinout->cs_port, this->pinout->cs_pin, GPIO_PIN_RESET);
}

void ST7789_STM32_Adapter::cs_unset() {
	HAL_GPIO_WritePin(this->pinout->cs_port, this->pinout->cs_pin, GPIO_PIN_SET);
}

void ST7789_STM32_Adapter::dc_set() {
	HAL_GPIO_WritePin(this->pinout->dc_port, this->pinout->dc_pin, GPIO_PIN_SET);
}

void ST7789_STM32_Adapter::dc_unset() {
	HAL_GPIO_WritePin(this->pinout->dc_port, this->pinout->dc_pin, GPIO_PIN_RESET);
}

void ST7789_STM32_Adapter::rst_set() {
	HAL_GPIO_WritePin(this->pinout->rst_port, this->pinout->rst_pin, GPIO_PIN_SET);
}

void ST7789_STM32_Adapter::rst_unset() {
	HAL_GPIO_WritePin(this->pinout->rst_port, this->pinout->rst_pin, GPIO_PIN_RESET);
}

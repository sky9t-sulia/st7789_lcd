# ST7789 LCD C++ Library for esp32, rp2020, stm32

⚠️Note: Currently only **STM32** supported. Other platforms is in development⚠️

&nbsp;

Originally code was looked from *https://github.com/Floyd-Fish/ST7789-STM32*

This library is written on C++ and can be used for **ESP32**, **RP2020**, and **STM32** using provided adapters.

&nbsp;
## Setup
Include adapter header for your platform and the library itself for example **STM32**:.


```cpp
#include <st7789_lcd/src/adapters/stm32_adapter.h>
#include <st7789_lcd/src/st7789.h>
```

Then you need to:
1. Fill pin configuration into ```st7789_stm32_adapter_pinout_t```
2. Create instanse of adapter for stm32 and setup it.
3. Create instanse of lcd.
&nbsp;
```cpp
st7789_stm32_adapter_pinout_t pinout = { 
    .cs_port = GPIOA,           // CS port
    .dc_port = GPIOA,           // CS pin on display
    .rst_port = GPIOA,          // ...
    .cs_pin = GPIO_PIN_2, 
    .dc_pin = GPIO_PIN_1, 
    .rst_pin = GPIO_PIN_0 
};

ST7789_STM32_Adapter adapter;
adapter.pinout = &pinout;
adapter.handle = &hspi1;    // For stm32 its the SPI_HandleTypeDef

ST7789_LCD lcd(240, 320, &adapter);

lcd.fill_screen(YELLOW);

```
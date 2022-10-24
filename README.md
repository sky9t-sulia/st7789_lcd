# ST7789 LCD C++ Library

The codebase was looked from https://github.com/Floyd-Fish/ST7789-STM32 

## Key features
This library is written on C++ and can be used for esp32, RP2020, and stm32 using adapters.

&nbsp;
## Setup
Include adapter header for your platform and the library itself.

&nbsp;

Include headers

```cpp
#include <st7789_lcd/src/adapters/stm32_adapter.h>
#include <st7789_lcd/src/st7789.h>
```

Then you need to:
1. Create and setup adapter.
2. Create instanse of lcd.

&nbsp;

```cpp
st7789_stm32_adapter_pinout_t pinout = { 
    .cs_port = GPIOA, 
    .dc_port = GPIOA, 
    .rst_port = GPIOA, 
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
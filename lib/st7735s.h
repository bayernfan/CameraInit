#ifndef ST7735S_H
#define ST7735S_H

#include <stdint.h>

#define LCD_SPIx SPI2
#define LCD_RCC_APBxPeriph_SPIx RCC_APB1Periph_SPI2

#ifndef CAMERA_WITH_LCD
    #define LCD_PORT GPIOA
    #define LCD_REST_PIN GPIO_Pin_6
    #define LCD_DC_PIN GPIO_Pin_7

    #define LCD_CS_PORT GPIOA
    #define LCD_CS_PIN GPIO_Pin_4

    #define LCD_PORT_BKL GPIOA
    #define GPIO_PIN_BKL GPIO_Pin_1
#else
    #define LCD_PORT GPIOB
    #define LCD_REST_PIN GPIO_Pin_6
    #define LCD_DC_PIN GPIO_Pin_7

    #define LCD_CS_PORT GPIOB
    #define LCD_CS_PIN GPIO_Pin_8

    #define LCD_PORT_BKL GPIOB
    #define GPIO_PIN_BKL GPIO_Pin_9
#endif

#define RGB(R, G, B) (((R) & 0x001f) | (((G) & 0x003f) << 5) | (((B) & 0x001f) << 11))

#define RED RGB(0x003f, 0, 0) // 0xf800
#define GREEN RGB(0, 0x003f, 0) // 0x07e0
#define BLUE RGB(0, 0, 0x003f) // 0x001f
#define YELLOW RGB(0x003f, 0x003f, 0) // 0xffe0
#define WHITE 0xffff
#define BLACK 0x0000
#define PURPLE RGB(0x003f, 0, 0x003f) // 0xf81f

#define ST7735_width 128
#define ST7735_height 160

void ST7735_init(void);
void ST7735_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7735_pushColor(uint16_t *color, int cnt);
void ST7735_backLight(uint8_t on);

#endif

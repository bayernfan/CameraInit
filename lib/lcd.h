#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include "st7735s.h"

#define LCD_init ST7735_init
#define LCD_setAddrWindow ST7735_setAddrWindow
#define LCD_pushColor ST7735_pushColor
#define LCD_backLight ST7735_backLight
#define LCD_width ST7735_width
#define LCD_height ST7735_height

#define ASCII_WIDTH 5
#define ASCII_HEIGHT 8

void LCD_writeASCII(uint16_t x0, uint16_t y0, uint8_t ch, uint16_t color);
void LCD_fillScreen(uint16_t color);

void TTY_printChar(uint8_t ch, uint16_t color);
void TTY_printString(char const *str, uint16_t color);

#endif

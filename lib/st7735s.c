#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include "spi.h"
#include "delay.h"
#include "st7735s.h"

#define SPI_SPPED SPI_FAST

#define SPI_LCD_CS_LOW()       GPIO_ResetBits(LCD_CS_PORT, LCD_CS_PIN)
#define SPI_LCD_CS_HIGH()      GPIO_SetBits(LCD_CS_PORT, LCD_CS_PIN)

#define SPI_LCD_REST()       GPIO_ResetBits(LCD_PORT, LCD_REST_PIN)
#define SPI_LCD_NO_REST()     GPIO_SetBits(LCD_PORT, LCD_REST_PIN)

#define SPI_LCD_DATA_W()       GPIO_ResetBits(LCD_PORT, LCD_DC_PIN)
#define SPI_LCD_DATA_R()       GPIO_SetBits(LCD_PORT, LCD_DC_PIN)


#define uchar unsigned char 
#define uint  unsigned int

static void lcd_Init(void);
static void ST7735S_CPT144_Initial(void);
static void LCD_WriteCommand(uint8_t c);
static void LCD_WriteByte(uint8_t dat);
static void LCD_WriteHalfWord(uint16_t dat16);

void ST7735_init(void) {    
    lcd_Init();
    ST7735S_CPT144_Initial();
}

void ST7735_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {    
    LCD_WriteCommand(0x2A);    //设置列地址
    LCD_WriteHalfWord(y0);
    LCD_WriteHalfWord(y1);
    
    LCD_WriteCommand(0x2B);    //设置行地址
    LCD_WriteHalfWord(x0);
    LCD_WriteHalfWord(x1);
    
    LCD_WriteCommand(0x2C);
}

void ST7735_pushColor(uint16_t *color, int cnt) {
    SPI_LCD_CS_LOW();
    SPI_LCD_DATA_R();
    (void) spiReadWrite16(LCD_SPIx, 0, color, cnt, SPI_SPPED);
    SPI_LCD_CS_HIGH();
}

void ST7735_backLight(uint8_t on) {
    if (on) {
        GPIO_WriteBit(LCD_PORT_BKL, GPIO_PIN_BKL, Bit_SET);
    } else {
        GPIO_WriteBit(LCD_PORT_BKL, GPIO_PIN_BKL, Bit_RESET);
    }
}

void lcd_Init(void) {
    GPIO_InitTypeDef  GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); // todo
    RCC_APB1PeriphClockCmd(LCD_RCC_APBxPeriph_SPIx, ENABLE); 

    /* GPIO common configuration */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    
    GPIO_InitStruct.GPIO_Pin = LCD_REST_PIN | LCD_DC_PIN; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = LCD_CS_PIN;
    GPIO_Init(LCD_CS_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_PIN_BKL;
    GPIO_Init(LCD_PORT_BKL, &GPIO_InitStruct);    

    spiInit(LCD_SPIx);
}

void LCD_WriteCommand(uint8_t c) {
    SPI_LCD_CS_LOW();
    SPI_LCD_DATA_W();
    (void) spiReadWrite(LCD_SPIx, 0, &c, 1, SPI_SPPED);  
    SPI_LCD_CS_HIGH();
}

void LCD_WriteByte(uint8_t dat) {
    SPI_LCD_CS_LOW();
    SPI_LCD_DATA_R();
    (void) spiReadWrite(LCD_SPIx, 0, &dat, 1, SPI_SPPED);
    SPI_LCD_CS_HIGH();
}

void LCD_WriteHalfWord(uint16_t dat16) {
    SPI_LCD_CS_LOW();
    SPI_LCD_DATA_R();
    (void) spiReadWrite16(LCD_SPIx, 0, &dat16, 1, SPI_SPPED);
    SPI_LCD_CS_HIGH();
}

void ST7735S_CPT144_Initial() { 
    //------------------------------------ST7735R Reset Sequence-----------------------------------------// 
    SPI_LCD_NO_REST(); 
    Delay(1);
    SPI_LCD_REST();
    Delay(1);
    SPI_LCD_NO_REST();    
    Delay(150);
    
    LCD_WriteCommand(0x01); // SWRESET Software reset
    Delay(150);
    //--------------------------------End ST7735S Reset Sequence ---------------------------------------//
    LCD_WriteCommand(0x11); //Sleep out
    Delay(150); //Delay 120ms
    //------------------------------------ST7735S Frame rate-------------------------------------------------//
    LCD_WriteCommand(0xB1); //Frame rate 80Hz
    LCD_WriteByte(0x02);
    LCD_WriteByte(0x35);
    LCD_WriteByte(0x36);
    LCD_WriteCommand(0xB2); //Frame rate 80Hz
    LCD_WriteByte(0x02);
    LCD_WriteByte(0x35);
    LCD_WriteByte(0x36);
    LCD_WriteCommand(0xB3); //Frame rate 80Hz
    LCD_WriteByte(0x02);
    LCD_WriteByte(0x35);
    LCD_WriteByte(0x36);
    LCD_WriteByte(0x02);
    LCD_WriteByte(0x35);
    LCD_WriteByte(0x36);
    //------------------------------------End ST7735S Frame rate-------------------------------------------//
    LCD_WriteCommand(0xB4); //Dot inversion
    LCD_WriteByte(0x03);
    //------------------------------------ST7735S Power Sequence-----------------------------------------//
    LCD_WriteCommand(0xC0);
    LCD_WriteByte(0xA2);
    LCD_WriteByte(0x02);
    LCD_WriteByte(0x84);
    LCD_WriteCommand(0xC1);
    LCD_WriteByte(0xC5);
    LCD_WriteCommand(0xC2);
    LCD_WriteByte(0x0D);
    LCD_WriteByte(0x00);
    LCD_WriteCommand(0xC3);
    LCD_WriteByte(0x8D);
    LCD_WriteByte(0x2A);
    LCD_WriteCommand(0xC4);
    LCD_WriteByte(0x8D);
    LCD_WriteByte(0xEE);
    //---------------------------------End ST7735S Power Sequence---------------------------------------//
    LCD_WriteCommand(0xC5); //VCOM
    LCD_WriteByte(0x0a);
    LCD_WriteCommand(0x36); //MX, MY, RGB mode
    LCD_WriteByte(0xC8);
    //------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    LCD_WriteCommand(0XE0);
    LCD_WriteByte(0x12);
    LCD_WriteByte(0x1C);
    LCD_WriteByte(0x10);
    LCD_WriteByte(0x18);
    LCD_WriteByte(0x33);
    LCD_WriteByte(0x2C);
    LCD_WriteByte(0x25);
    LCD_WriteByte(0x28);
    LCD_WriteByte(0x28);
    LCD_WriteByte(0x27);
    LCD_WriteByte(0x2F);
    LCD_WriteByte(0x3C);
    LCD_WriteByte(0x00);
    LCD_WriteByte(0x03);
    LCD_WriteByte(0x03);
    LCD_WriteByte(0x10);
    LCD_WriteCommand(0XE1);
    LCD_WriteByte(0x12);
    LCD_WriteByte(0x1C);
    LCD_WriteByte(0x10);
    LCD_WriteByte(0x18);
    LCD_WriteByte(0x2D);
    LCD_WriteByte(0x28);
    LCD_WriteByte(0x23);
    LCD_WriteByte(0x28);
    LCD_WriteByte(0x28);
    LCD_WriteByte(0x26);
    LCD_WriteByte(0x2F);
    LCD_WriteByte(0x3B);
    LCD_WriteByte(0x00);
    LCD_WriteByte(0x03);
    LCD_WriteByte(0x03);
    LCD_WriteByte(0x10);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
    LCD_WriteCommand(0x3A); //65k mode
    LCD_WriteByte(0x05);
    LCD_WriteCommand(0x29); //Display on 
    Delay(150);
}

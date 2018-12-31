#include <stm32f10x.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "spi.h"

static const uint16_t speeds[] = {
    [SPI_SLOW] = SPI_BaudRatePrescaler_64,
    [SPI_MEDIUM] = SPI_BaudRatePrescaler_8,
    [SPI_FAST] = SPI_BaudRatePrescaler_2
};

void spiInit(SPI_TypeDef *SPIx) {
    spiInit_v2(SPIx, SPI_CPOL_High, SPI_CPHA_2Edge);
}

void spiInit_v2(SPI_TypeDef *SPIx, uint16_t SPI_CPOL_x, uint16_t SPI_CPHA_xEdge) {
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_StructInit(&GPIO_InitStructure);
    SPI_StructInit(&SPI_InitStructure);
    
    if (SPIx == SPI2) {        
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);    
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);    
        
        // initialize SCK, MOSI
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        // initialize MISO
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOB, &GPIO_InitStructure);        
    } else {
        return;
    }

    SPI_Cmd(SPIx, DISABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_x;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_xEdge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = speeds[SPI_SLOW];
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPIx, &SPI_InitStructure);    
    SPI_Cmd(SPIx, ENABLE);
}

int spiReadWrite(SPI_TypeDef *SPIx, uint8_t *rbuf,
    const uint8_t *tbuf, int cnt, enum spiSpeed speed
) {
    int i;
    
    SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256) | speeds[speed];
    
    for (i = 0; i < cnt; i++) {
        if (tbuf) {            
            SPI_I2S_SendData(SPIx, *tbuf++);
        } else {
            SPI_I2S_SendData(SPIx, 0xff);
        }
        while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
        if (rbuf) {
            *rbuf++ = SPI_I2S_ReceiveData(SPIx);
        } else {
            SPI_I2S_ReceiveData(SPIx);
        }
    }
    return 1;
}

int spiReadWrite16(SPI_TypeDef *SPIx, uint16_t *rbuf,
    const uint16_t *tbuf, int cnt, enum spiSpeed speed
) {
    int i;
    
    SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256) | speeds[speed];
    SPI_DataSizeConfig(SPIx, SPI_DataSize_16b);
    
    for (i = 0; i < cnt; i++) {
        SPI_I2S_SendData(SPIx, tbuf ? *tbuf++ : 0xff);
        while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
        if (rbuf) {
            *rbuf++ = SPI_I2S_ReceiveData(SPIx);
        } else {
            SPI_I2S_ReceiveData(SPIx);
        }
    }
    
    SPI_DataSizeConfig(SPIx, SPI_DataSize_8b);
    return i;
}

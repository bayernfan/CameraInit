#ifndef _SCCB_H
#define _SCCB_H

#include <stm32f10x_gpio.h>

typedef struct {
    uint32_t RCC_APB2Periph_GPIO_SIO;
    GPIO_TypeDef *SIO_Port;
    uint16_t SCL_Pin;
    uint16_t SDA_Pin;
} SCCB_InitTypeDef;

void SCCB_init(SCCB_InitTypeDef *SCCB_InitStruct);
ErrorStatus SCCB_3PhaseWrite(SCCB_InitTypeDef *SCCB_InitStruct,
    uint8_t const id, uint8_t const address, uint8_t const value);
ErrorStatus SCCB_2PhaseWrite(SCCB_InitTypeDef *SCCB_InitStruct,
    uint8_t const id, uint8_t const address);
ErrorStatus SCCB_2PhaseRead(SCCB_InitTypeDef *SCCB_InitStruct,
    uint8_t const id, uint8_t *value);

#endif /* _SCCB_H */

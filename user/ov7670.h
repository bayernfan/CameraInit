#ifndef H_OV7660_H
#define H_OV7660_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_i2c.h>
#include "sccb.h"
#include "pwm.h"

#define SIO_SPEED                       50
#define SIO_MCU_ADDRESS                 0x00
#define SIO_DEVICE_ADDRESS              0x73

#define OV7670_YUV_N_WIDTH              640
#define OV7670_YUV_N_HEIGHT             480

#define SENSOR_N_DIMISIONS              2

typedef struct {
    uint32_t GPIO_Remap;
    GPIO_TypeDef *Port;
    uint16_t Pin;
    uint32_t Rate;
    TIM_TypeDef *TIM;
    uint16_t TIM_Channel;
} XCLK_InitTypeDef;

typedef struct {
    GPIO_TypeDef *REST_Port;
    uint16_t REST_Pin;
    GPIO_TypeDef *PWDN_Port;
    uint16_t PWDN_Pin;
    GPIO_TypeDef *HREF_Port;
    uint16_t HREF_Pin;
    GPIO_TypeDef *VSYNC_Port;
    uint16_t VSYNC_Pin;
    GPIO_TypeDef *PCLK_Port;
    uint16_t PCLK_Pin;
    XCLK_InitTypeDef XCLK;
    GPIO_TypeDef *DATA_Port;
    SCCB_InitTypeDef SCCB_InitStruct;
    uint16_t windowAnchor[SENSOR_N_DIMISIONS];
    uint16_t windowSize[SENSOR_N_DIMISIONS];
} OV7670_InitTypeDef;

int OV7670_init(OV7670_InitTypeDef *OV7670_InitStruct);

#endif /* H_OV7660_H */

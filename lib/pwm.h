#ifndef PWM_H
#define PWM_H

#include <stm32f10x_tim.h>
#include <stm32f10x_gpio.h>

typedef struct {
    uint32_t GPIO_Remap;
    GPIO_TypeDef *PWM_Port;
    uint16_t PWM_Pin;
    uint32_t Rate;
    uint32_t CounterRate;
    uint16_t Compare;
    TIM_TypeDef *PWM_TIM;
    uint16_t PWM_TIM_Channel;
} PWM_InitTypeDef;

int PWM_init(PWM_InitTypeDef *PWM_InitStructure);

#endif

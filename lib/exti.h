#ifndef EXTI_H
#define EXTI_H

#include <stm32f10x_gpio.h>
#include <stm32f10x_exti.h>

int EXTI_open(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin_x, EXTITrigger_TypeDef EXTITrigger_Type);

#endif

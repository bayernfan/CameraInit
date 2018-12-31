#include <stm32f10x_rcc.h>
#include <misc.h>
#include "exti.h"



int EXTI_open(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin_x, EXTITrigger_TypeDef EXTI_Trigger_x) {
    uint8_t GPIO_PortSourceGPIOx, GPIO_PinSourcex;
    uint32_t EXTI_Linex, RCC_APB2Periph_GPIOx, EXTIx_IRQn;

#define if_GPIOX_then_GPIO_PortSourceGPIOX(X) \
    if (GPIO##X == GPIOx) { \
        GPIO_PortSourceGPIOx = GPIO_PortSourceGPIO##X; \
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIO##X; \
    }
    
    if_GPIOX_then_GPIO_PortSourceGPIOX(G)
    else if_GPIOX_then_GPIO_PortSourceGPIOX(F)
    else if_GPIOX_then_GPIO_PortSourceGPIOX(E)
    else if_GPIOX_then_GPIO_PortSourceGPIOX(D)
    else if_GPIOX_then_GPIO_PortSourceGPIOX(C)
    else if_GPIOX_then_GPIO_PortSourceGPIOX(B)
    else if_GPIOX_then_GPIO_PortSourceGPIOX(A)
    else return -1;

#define if_GPIO_Pin_X_then_GPIO_PinSourceX(X) \
    if (GPIO_Pin_##X == GPIO_Pin_x) { \
        EXTI_Linex = EXTI_Line##X; \
        GPIO_PinSourcex = GPIO_PinSource##X; \
        EXTIx_IRQn = EXTI15_10_IRQn; \
    }
    
    if_GPIO_Pin_X_then_GPIO_PinSourceX(15)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(14)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(13)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(12)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(11)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(10)
        
#undef if_GPIO_Pin_X_then_GPIO_PinSourceX
#define if_GPIO_Pin_X_then_GPIO_PinSourceX(X) \
    if (GPIO_Pin_##X == GPIO_Pin_x) { \
        EXTI_Linex = EXTI_Line##X; \
        GPIO_PinSourcex = GPIO_PinSource##X; \
        EXTIx_IRQn = EXTI9_5_IRQn; \
    }
    
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(9)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(8)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(7)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(6)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(5)
                
#undef if_GPIO_Pin_X_then_GPIO_PinSourceX
#define if_GPIO_Pin_X_then_GPIO_PinSourceX(X) \
    if (GPIO_Pin_##X == GPIO_Pin_x) { \
        EXTI_Linex = EXTI_Line##X; \
        GPIO_PinSourcex = GPIO_PinSource##X; \
        EXTIx_IRQn = EXTI##X##_IRQn; \
    }
    
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(4)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(3)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(2)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(1)
    else if_GPIO_Pin_X_then_GPIO_PinSourceX(0)
    else return -1;
    
    // configure GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    
    // configure EXTI
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOx, GPIO_PinSourcex);
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Linex;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_x;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    // Configure NVIC   
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTIx_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    return 0;
}

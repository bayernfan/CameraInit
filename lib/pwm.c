#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_gpio.h>
#include "pwm.h"

#define THIS PWM_InitStructure

int PWM_init(PWM_InitTypeDef *PWM_InitStructure) {
    uint32_t RCC_APBxPeriph_GPIOx;
    uint32_t RCC_APBxPeriph_TIMx;
    void (*TIM_OCxInit)(TIM_TypeDef *, TIM_OCInitTypeDef *);
    void (*RCC_APBxPeriphClockCmd_for_TIM)(uint32_t, FunctionalState);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;    
    
    if (GPIOA == THIS->PWM_Port) {
        RCC_APBxPeriph_GPIOx = RCC_APB2Periph_GPIOA;
    } else if (GPIOB == THIS->PWM_Port) {        
        RCC_APBxPeriph_GPIOx = RCC_APB2Periph_GPIOB;
    } else if (GPIOC == THIS->PWM_Port) {        
        RCC_APBxPeriph_GPIOx = RCC_APB2Periph_GPIOC;
    } else {
        return -1;
    }    
    
    if (TIM1 == THIS->PWM_TIM) {
        RCC_APBxPeriphClockCmd_for_TIM = RCC_APB2PeriphClockCmd;
        RCC_APBxPeriph_TIMx = RCC_APB2Periph_TIM1;
    } else if (TIM2 == THIS->PWM_TIM) {
        RCC_APBxPeriphClockCmd_for_TIM = RCC_APB1PeriphClockCmd;
        RCC_APBxPeriph_TIMx = RCC_APB1Periph_TIM2;
    } else if (TIM3 == THIS->PWM_TIM) {
        RCC_APBxPeriphClockCmd_for_TIM = RCC_APB1PeriphClockCmd;
        RCC_APBxPeriph_TIMx = RCC_APB1Periph_TIM3;
    } else if (TIM4 == THIS->PWM_TIM) {
        RCC_APBxPeriphClockCmd_for_TIM = RCC_APB1PeriphClockCmd;
        RCC_APBxPeriph_TIMx = RCC_APB1Periph_TIM4;
    } else {
        return -1;
    }
    
    if (TIM_Channel_1 == THIS->PWM_TIM_Channel) {
        TIM_OCxInit = TIM_OC1Init;
    } else if (TIM_Channel_2 == THIS->PWM_TIM_Channel) {
        TIM_OCxInit = TIM_OC2Init;
    } else if (TIM_Channel_3 == THIS->PWM_TIM_Channel) {
        TIM_OCxInit = TIM_OC3Init;
    } else if (TIM_Channel_4 == THIS->PWM_TIM_Channel) {
        TIM_OCxInit = TIM_OC4Init;
    } else {
        return -1;
    }
    
    // initialize GPIO
    RCC_APB2PeriphClockCmd(RCC_APBxPeriph_GPIOx, ENABLE); // RCC_APB2Periph_GPIOA
    
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = THIS->PWM_Pin; // GPIO_Pin_1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_Init(THIS->PWM_Port, &GPIO_InitStructure); // GPIOA
    
    // initialize timer
    RCC_APBxPeriphClockCmd_for_TIM(RCC_APBxPeriph_TIMx, ENABLE); // RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ...

    if (THIS->GPIO_Remap) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // todo
        GPIO_PinRemapConfig(THIS->GPIO_Remap, ENABLE); // todo        
    }
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / THIS->CounterRate - 1;
    TIM_TimeBaseStructure.TIM_Period = THIS->CounterRate / THIS->Rate - 1 ; // 1e3
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(THIS->PWM_TIM, &TIM_TimeBaseStructure); // TIM2
    
    // PWM1 MMode configuration
    // Edge -aligned; not single pulse mode
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCxInit(THIS->PWM_TIM, &TIM_OCInitStructure); // TIM_OC2Init(TIM2, ...
    
    // enable timer
    TIM_Cmd(THIS->PWM_TIM, ENABLE); // TIM2  

    if (TIM_Channel_1 == THIS->PWM_TIM_Channel) {
        TIM_SetCompare1(THIS->PWM_TIM, THIS->Compare);
    } else if (TIM_Channel_2 == THIS->PWM_TIM_Channel) {
        TIM_SetCompare2(THIS->PWM_TIM, THIS->Compare);
    } else if (TIM_Channel_3 == THIS->PWM_TIM_Channel) {
        TIM_SetCompare3(THIS->PWM_TIM, THIS->Compare);
    } else if (TIM_Channel_4 == THIS->PWM_TIM_Channel) {
        TIM_SetCompare4(THIS->PWM_TIM, THIS->Compare);
    } else {
        return -1;
    }
    
    
    return 0;
}

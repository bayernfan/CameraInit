#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "led.h"

void LED_init(void) {    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Enable Peripheral Clocks ... (1) ...
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // Configure Pins ... (2) ...
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void LED_on(void) {
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}
    
void LED_off(void) {
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);    
}

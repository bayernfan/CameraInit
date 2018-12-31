#include <stm32f10x.h>
#include "delay.h"

static __IO uint32_t TimingDelay;

void Use_SysTick_Delay() {
    if (SysTick_Config(SystemCoreClock / 1000000))
        while (1);
}

#ifndef USE_SOFTWARE_DELAY
void SysTick_Handler(void) {
    if (TimingDelay != 0x00)
        TimingDelay--;
}
#endif

void Delay_us(uint32_t nTime) {
#ifdef USE_SOFTWARE_DELAY
    TimingDelay = nTime;
    switch (TimingDelay) {
        case 0: case 1:
            return;
        case 2:
            TimingDelay--; TimingDelay--;
            return;
        default:
            TimingDelay -= 3;
            TimingDelay = (TimingDelay << 1) + (TimingDelay >> 1);
    }
    while (TimingDelay != 0) TimingDelay--;
#else    
    TimingDelay = nTime;
    while (TimingDelay != 0) ;
#endif    
}

void Delay_ms(uint32_t nTime) {
#ifdef USE_SOFTWARE_DELAY
    TimingDelay = nTime * USE_SOFTWARE_DELAY;
    while (TimingDelay != 0) TimingDelay--;
#else    
    TimingDelay = nTime * 1000;
    while (TimingDelay != 0) ;
#endif    
}

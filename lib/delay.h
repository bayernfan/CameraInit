#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>
#include "stm32f10x_conf.h"

void Use_SysTick_Delay(void);
void Delay_us(uint32_t nTime);
void Delay_ms(uint32_t nTime);

#define Delay Delay_ms

#endif

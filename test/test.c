#include "pwm.h"

static void test_PWM(void);

#ifdef UNIT_TEST
int main(void)
#else
static int Main(void)
#endif
{
    test_PWM();
    while (1);
}

void test_PWM(void){    
    PWM_InitTypeDef PWM_InitStruct;
    PWM_InitStruct.PWM_Pin = GPIO_Pin_11;
    PWM_InitStruct.PWM_Port = GPIOB;
    PWM_InitStruct.PWM_TIM = TIM2;
    PWM_InitStruct.PWM_TIM_Channel = TIM_Channel_4;
    PWM_InitStruct.CounterRate = 2000000;
    PWM_InitStruct.Rate = 1000000;
    PWM_InitStruct.Compare = 1;
    PWM_init(&PWM_InitStruct);
}

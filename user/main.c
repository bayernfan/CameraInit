#include <stm32f10x_rcc.h>
#include <misc.h>
#include "delay.h"
#include "ov7670.h"
#include "exti.h"
#include "st7735s.h"
#include "lcd.h"

#define VGA_N_COLS 640
#define VGA_N_ROWS 480
#define QVGA_N_COLS (VGA_N_COLS >> 1)
#define QVGA_N_ROWS (VGA_N_ROWS >> 1)
#define QQVGA_N_COLS (VGA_N_COLS >> 2)
#define QQVGA_N_ROWS (VGA_N_ROWS >> 2)

#define FRAME_N_COLS  ST7735_height
#define FRAME_N_ROWS ST7735_width
#define N_CAMERAS 1
#define N_ROWS_BUF (1 << 3)

#define LOG_DEBUG(MSG) TTY_printString((MSG), YELLOW) // __nop()

static uint16_t volatile pixel_buf[N_CAMERAS][N_ROWS_BUF][FRAME_N_COLS];

static OV7670_InitTypeDef OV7670_cameras[N_CAMERAS];
static DMA_InitTypeDef DMA_InitStructs[N_CAMERAS];
static TIM_ICInitTypeDef TIM_ICInitStructs[N_CAMERAS];
static NVIC_InitTypeDef NVIC_InitStructs[N_CAMERAS];
    
static uint16_t const line_length = ST7735_height < FRAME_N_COLS ? ST7735_height : FRAME_N_COLS;

static int volatile visual_active = 0;
static uint16_t volatile i_row_in = 0;
static uint16_t volatile i_row_out = 0;

#ifdef UNIT_TEST
static int Main(void)
#else
int main(void)
#endif
{
    OV7670_InitTypeDef *camera;
    DMA_InitTypeDef *dma;
    TIM_ICInitTypeDef *ic;
    NVIC_InitTypeDef *NVIC_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    
    uint8_t i_camera;
#ifndef USE_SOFTWARE_DELAY
    Use_SysTick_Delay();
#endif
    
    LCD_init();
    LCD_backLight(1);
    LCD_fillScreen(BLUE);


    Delay_ms(300);

    for (i_camera = 0; i_camera < N_CAMERAS; i_camera++) {
        camera = &OV7670_cameras[i_camera];
                
        switch (i_camera) {
            case 0:
                camera->REST_Port = GPIOC;
                camera->REST_Pin = GPIO_Pin_13;
                camera->PWDN_Port = GPIOC;
                camera->PWDN_Pin = GPIO_Pin_15;  
                camera->HREF_Port = GPIOB;
                camera->HREF_Pin = GPIO_Pin_1;
                camera->VSYNC_Port = GPIOC;
                camera->VSYNC_Pin = GPIO_Pin_14;
                camera->PCLK_Port = GPIOB;
                camera->PCLK_Pin = GPIO_Pin_0;
                camera->XCLK.GPIO_Remap = GPIO_FullRemap_TIM2;
                camera->XCLK.Port = GPIOB; // todo
                camera->XCLK.Pin = GPIO_Pin_11; // todo
                camera->XCLK.TIM = TIM2; // todo
                camera->XCLK.TIM_Channel = TIM_Channel_4; // todo
                camera->XCLK.Rate = 4000000;
                camera->DATA_Port = GPIOA;
                camera->SCCB_InitStruct.RCC_APB2Periph_GPIO_SIO = RCC_APB2Periph_GPIOA;
                camera->SCCB_InitStruct.SIO_Port = GPIOA;
                camera->SCCB_InitStruct.SCL_Pin = GPIO_Pin_9;
                camera->SCCB_InitStruct.SDA_Pin = GPIO_Pin_8;
                camera->windowAnchor[0] = (QVGA_N_COLS - FRAME_N_COLS) >> 1;
                camera->windowAnchor[1] = (QVGA_N_ROWS - FRAME_N_ROWS) >> 1;
                camera->windowSize[0] = FRAME_N_COLS-1;
                camera->windowSize[1] = FRAME_N_ROWS-1;
                break;
            case 1:
                // todo
                break;
            default:
                continue;
        }
        
        OV7670_init(camera);
    }

    for (i_camera = 0;i_camera < N_CAMERAS; i_camera++){
        camera = &OV7670_cameras[i_camera];        
        EXTI_open(camera->HREF_Port, camera->HREF_Pin, EXTI_Trigger_Rising);
        EXTI_open(camera->VSYNC_Port, camera->VSYNC_Pin, EXTI_Trigger_Rising | EXTI_Trigger_Falling);
        
        dma = &DMA_InitStructs[i_camera];
        dma->DMA_PeripheralBaseAddr = (uint32_t) &camera->DATA_Port->IDR;
        dma->DMA_DIR = DMA_DIR_PeripheralSRC;
        dma->DMA_BufferSize = FRAME_N_COLS << 1;
        dma->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        dma->DMA_MemoryInc = DMA_MemoryInc_Enable;
        dma->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        dma->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        dma->DMA_Mode = DMA_Mode_Circular;
        dma->DMA_Priority = DMA_Priority_VeryHigh;
        dma->DMA_M2M = DMA_M2M_Disable;
        
        NVIC_InitStruct = &NVIC_InitStructs[i_camera];
        NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStruct->NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStruct->NVIC_IRQChannelCmd = ENABLE;
        switch (i_camera) {
            case 0:
                NVIC_InitStruct->NVIC_IRQChannel = DMA1_Channel2_IRQn;
            
                RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE );
                break;
            default:
                ;
        }
    }    
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Period = 256;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    
    for (i_camera = 0; i_camera < N_CAMERAS; i_camera++) {
        ic = &TIM_ICInitStructs[i_camera];
                
        ic->TIM_ICPolarity = TIM_ICPolarity_Rising;
        ic->TIM_ICSelection = TIM_ICSelection_DirectTI;
        ic->TIM_ICPrescaler = TIM_ICPSC_DIV1;
        ic->TIM_ICFilter = 0;
        
        switch (i_camera) {
            case 0:
                RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
                TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
                ic->TIM_Channel = TIM_Channel_3;
                TIM_ICInit(TIM3, ic);
                break;
            case 1:
                // todo
                break;
            default:
                continue;
        }
    }

    while (1) ;
}

static void enable_dma(uint8_t i_dma, uint16_t i_row) {
    DMA_InitTypeDef* dma = &DMA_InitStructs[i_dma];
    NVIC_InitTypeDef *NVIC_InitStruct = &NVIC_InitStructs[i_dma];
    
    dma->DMA_MemoryBaseAddr = (uint32_t)pixel_buf[i_dma][i_row & (N_ROWS_BUF - 1)];
    
    switch (i_dma) {
        case 0:
            NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
            NVIC_Init(NVIC_InitStruct);
        
            DMA_DeInit(DMA1_Channel2);
            DMA_Init(DMA1_Channel2, dma);
            DMA_Cmd(DMA1_Channel2, ENABLE);
            TIM_DMACmd(TIM3, TIM_DMA_CC3, ENABLE);
            DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
            TIM_Cmd(TIM3, ENABLE);
            break;
        default:
            ;
    }
}

static void disable_dma(uint8_t i_dma) {    
    switch (i_dma) {
        case 0:
            TIM_Cmd(TIM3, DISABLE);
            DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, DISABLE);
            TIM_DMACmd(TIM3, TIM_DMA_CC3, DISABLE);
            DMA_Cmd(DMA1_Channel2, DISABLE);
            break;
        default:
            ;
    }
}

void EXTI15_10_IRQHandler(void) { // camera 0, VSYNC
    OV7670_InitTypeDef *camera;
    if (EXTI_GetFlagStatus(EXTI_Line14) != RESET) {
        camera = &OV7670_cameras[0];
        if (0 == GPIO_ReadInputDataBit(camera->HREF_Port, camera->HREF_Pin))
        {
            if (GPIO_ReadInputDataBit(camera->VSYNC_Port, camera->VSYNC_Pin)) {
                visual_active = 0;
                i_row_in = 0;
            } else {
                visual_active = 1;
                i_row_out = 0;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
}

void EXTI1_IRQHandler(void) { // camera 0, HREF
    if (EXTI_GetFlagStatus(EXTI_Line1) != RESET) {
        if (visual_active) {
            enable_dma(0, i_row_in);
            i_row_in++;
        }
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void DMA1_Channel2_IRQHandler(void) { // camera 0, DMA complete
    if (DMA_GetITStatus(DMA1_IT_TC2)) {        
        uint16_t i_row = i_row_out++;
        disable_dma(0);
        if (i_row < ST7735_width) {
            uint16_t i_col_buf = FRAME_N_COLS;
            uint16_t const i_row_lcd = ST7735_width - 1 - i_row;
            uint16_t volatile *pixel_line = pixel_buf[0][i_row & (N_ROWS_BUF - 1)];
                        
            ST7735_setAddrWindow(0, i_row_lcd, line_length - 1, i_row_lcd);            
            while (i_col_buf-- != 0)
                pixel_line[i_col_buf] = __REV16(pixel_line[i_col_buf]);
            ST7735_pushColor((uint16_t *)pixel_line, line_length);
        }
        DMA_ClearITPendingBit(DMA1_IT_TC2);
    }
}

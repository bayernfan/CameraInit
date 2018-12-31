#include "ov7670.h"
#include "delay.h"
#include "sccb.h"
#include "pwm.h"

#define OV7670_WR_ID 0x42
#define OV7670_RD_ID 0x43

static void OV7670_InitRCC(OV7670_InitTypeDef *OV7670_InitStruct);
static void OV7670_InitGPIO(OV7670_InitTypeDef *OV7670_InitStruct);
static void OV7670_InitXCLK(OV7670_InitTypeDef *OV7670_InitStruct);
static ErrorStatus OV7670_InitSensor(OV7670_InitTypeDef *OV7670_InitStruct);
static ErrorStatus OV7670_WriteReg(OV7670_InitTypeDef *OV7670_InitStruct,
    uint8_t reg, uint8_t value);
static ErrorStatus OV7670_ReadReg(OV7670_InitTypeDef *OV7670_InitStruct,
    uint8_t const reg, uint8_t *value);

static void OV7670_WriteRegs(OV7670_InitTypeDef *OV7670_InitStruct);

#define THIS OV7670_InitStruct

#define _writeReg(reg, value) OV7670_WriteReg(THIS, (uint8_t)(reg), (uint8_t)(value))
#define _readReg(reg, value) OV7670_ReadReg(THIS, (uint8_t)(reg), (uint8_t)(value))
#define _writeRegs() OV7670_WriteRegs(THIS)

int OV7670_init(OV7670_InitTypeDef *OV7670_InitStruct) {
    OV7670_InitRCC(THIS);
    OV7670_InitGPIO(THIS);
    OV7670_InitXCLK(THIS);
    SCCB_init(&THIS->SCCB_InitStruct);
    return OV7670_InitSensor(THIS);
}

#define if_GPIOx_then_add_RCC_APB2Periph_GPIOx(GPIOx, x) \
    if (GPIOx == GPIO##x) { \
        RCC_APB2Periph_GPIOx |= RCC_APB2Periph_GPIO##x; \
    }

#define add_RCC_APB2Periph_GPIOx_by_GPIOx(GPIOx) \
    if_GPIOx_then_add_RCC_APB2Periph_GPIOx(GPIOx, A) \
    else if_GPIOx_then_add_RCC_APB2Periph_GPIOx(GPIOx, B) \
    else if_GPIOx_then_add_RCC_APB2Periph_GPIOx(GPIOx, C) \
    else if_GPIOx_then_add_RCC_APB2Periph_GPIOx(GPIOx, D) \
    else if_GPIOx_then_add_RCC_APB2Periph_GPIOx(GPIOx, E) \
    else if_GPIOx_then_add_RCC_APB2Periph_GPIOx(GPIOx, F) \
    else if_GPIOx_then_add_RCC_APB2Periph_GPIOx(GPIOx, G)

void OV7670_InitRCC(OV7670_InitTypeDef *OV7670_InitStruct) {
    uint32_t RCC_APB2Periph_GPIOx = 0;

    add_RCC_APB2Periph_GPIOx_by_GPIOx(THIS->HREF_Port);
    add_RCC_APB2Periph_GPIOx_by_GPIOx(THIS->VSYNC_Port);
    add_RCC_APB2Periph_GPIOx_by_GPIOx(THIS->PCLK_Port);
    add_RCC_APB2Periph_GPIOx_by_GPIOx(THIS->DATA_Port);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);
}

void OV7670_InitGPIO(OV7670_InitTypeDef *OV7670_InitStruct) {    
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    
    GPIO_InitStruct.GPIO_Pin = THIS->REST_Pin;
    GPIO_Init(THIS->REST_Port, &GPIO_InitStruct);
    GPIO_SetBits(THIS->REST_Port, THIS->REST_Pin);

    GPIO_InitStruct.GPIO_Pin = THIS->PWDN_Pin;
    GPIO_Init(THIS->PWDN_Port, &GPIO_InitStruct);
    GPIO_ResetBits(THIS->PWDN_Port, THIS->PWDN_Pin); 

    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    
    GPIO_InitStruct.GPIO_Pin = THIS->HREF_Pin;
    GPIO_Init(THIS->HREF_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = THIS->VSYNC_Pin;
    GPIO_Init(THIS->VSYNC_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = THIS->PCLK_Pin;
    GPIO_Init(THIS->PCLK_Port, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
        GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(THIS->DATA_Port, &GPIO_InitStruct);
}

void OV7670_InitXCLK(OV7670_InitTypeDef *OV7670_InitStruct) {
    PWM_InitTypeDef PWM_InitStruct;

    PWM_InitStruct.PWM_Pin = THIS->XCLK.Pin;
    PWM_InitStruct.PWM_Port = THIS->XCLK.Port;
    PWM_InitStruct.PWM_TIM = THIS->XCLK.TIM;
    PWM_InitStruct.PWM_TIM_Channel = THIS->XCLK.TIM_Channel;
    PWM_InitStruct.CounterRate = THIS->XCLK.Rate << 1;
    PWM_InitStruct.Rate = THIS->XCLK.Rate;
    PWM_InitStruct.Compare = 1;
    PWM_init(&PWM_InitStruct);
}

ErrorStatus OV7670_InitSensor(OV7670_InitTypeDef *OV7670_InitStruct) {    
    if (SUCCESS != _writeReg(0x12, 0x80)) // Common_Control_7 = resets_all_registers_to_default_values
        return ERROR;
    
    Delay_ms(100);
    _writeRegs();
    
    
	return SUCCESS;
}

#define REG_BUF_N_BYTES 2

ErrorStatus OV7670_WriteReg(OV7670_InitTypeDef *OV7670_InitStruct,
    uint8_t const reg, uint8_t const value)
{
    return SCCB_3PhaseWrite(&THIS->SCCB_InitStruct, OV7670_WR_ID, reg, value);
}

ErrorStatus OV7670_ReadReg(OV7670_InitTypeDef *OV7670_InitStruct,
    uint8_t const reg, uint8_t *value)
{
    if (SUCCESS != SCCB_2PhaseWrite(&THIS->SCCB_InitStruct, OV7670_WR_ID, reg))
        return ERROR;
    return SCCB_2PhaseRead(&THIS->SCCB_InitStruct, OV7670_RD_ID, value);
}

void OV7670_WriteRegs(OV7670_InitTypeDef *OV7670_InitStruct) {
     uint16_t startx; 
     uint16_t starty;
     uint16_t endx;
     uint16_t endy;
    
     endx = THIS->windowAnchor[0];
     startx = THIS->windowAnchor[0] + THIS->windowSize[0];
     starty = THIS->windowAnchor[1];
     endy = (THIS->windowAnchor[1] + THIS->windowSize[1]) << 1;
  
    _writeReg(0x11,(0x040|0x00)); // use external clock | internal clock pre-scalar
    _writeReg(0x3a, 0x04);
    _writeReg(0x3d, 0xc2);

    _writeReg(0x12, 0x14); // output QVGA | RGB
    _writeReg(0x40, 0xd0); // output range 00 to ff | RGB 565

    _writeReg(0x1e, 0x17); // mirror / vertical filp

    /* frame sizes */
    _writeReg(0x17, startx >> 3);
    _writeReg(0x18, endx >> 3);
    _writeReg(0x32, 0x80 | ((endx & 0x07) << 3) | (startx & 0x07));
    _writeReg(0x19, starty >> 1); // (starty * 2) >> 2
    _writeReg(0x1a, endy >> 2);
    _writeReg(0x03, 0x00 | ((endy & 0x03) << 2) | ((starty << 1) & 0x03)); // ... ((starty * 2) & 0x03) ...
    
    _writeReg(0x0c, 0x00);//
    _writeReg(0x3e, 0x00);//pclk分频  

    /* test control */
    _writeReg(0x70, 0x3a);
    _writeReg(0x71, 0x35);
    _writeReg(0x72, 0x11);
    _writeReg(0x73, 0xf0);

    _writeReg(0x55, 0x00);
    _writeReg(0x56, 0x80);
    
    _writeReg(0xa2, 0x02);
    _writeReg(0x7a, 0x20);
    _writeReg(0x7b, 0x10);
    _writeReg(0x7c, 0x1e);
    _writeReg(0x7d, 0x35);
    _writeReg(0x7e, 0x5a);
    _writeReg(0x7f, 0x69);
    _writeReg(0x80, 0x76);
    _writeReg(0x81, 0x80);
    _writeReg(0x82, 0x88);
    _writeReg(0x83, 0x8f);
    _writeReg(0x84, 0x96);
    _writeReg(0x85, 0xa3);
    _writeReg(0x86, 0xaf);
    _writeReg(0x87, 0xc4);
    _writeReg(0x88, 0xd7);
    _writeReg(0x89, 0xe8);
    _writeReg(0x13, 0xef);

    _writeReg(0x00, 0x00);// gain control
    _writeReg(0x07, 0x00);
    _writeReg(0x10, 0x00);
    _writeReg(0x04, 0x00);

    _writeReg(0x0d, 0x00);
    _writeReg(0x42, 0x40);
    _writeReg(0x14, 0x18);
    _writeReg(0xa5, 0x08);
    _writeReg(0xab, 0x08);
    _writeReg(0x24, 0x75);
    _writeReg(0x25, 0x63);
    _writeReg(0x26, 0xd4);
    _writeReg(0x9f, 0x78);
    _writeReg(0xa0, 0x68);
    _writeReg(0xa1, 0x03);
    _writeReg(0xa6, 0xd8);
    _writeReg(0xa7, 0xd8);
    _writeReg(0xa8, 0xf0);
    _writeReg(0xa9, 0x90);
    _writeReg(0xaa, 0x14);
    _writeReg(0x13, 0xe5);                           
    _writeReg(0x0e, 0x61);
    _writeReg(0x0f, 0x4b);
    _writeReg(0x16, 0x02);
    _writeReg(0x21, 0x02);
    _writeReg(0x22, 0x91);
    _writeReg(0x29, 0x07);
    _writeReg(0x33, 0x0b);
    _writeReg(0x35, 0x0b);
    _writeReg(0x37, 0x1d);
    _writeReg(0x38, 0x71);
    _writeReg(0x39, 0x2a);
    _writeReg(0x3c, 0x78);
    _writeReg(0x4d, 0x40);
    _writeReg(0x4e, 0x20);
    _writeReg(0x69, 0x00);

    _writeReg(0x6b, 0x0a); // bypass PLL | bypass internal regulator

    _writeReg(0x74, 0x10);
    _writeReg(0x8d, 0x4f);
    _writeReg(0x8e, 0x00);
    _writeReg(0x8f, 0x00);
    _writeReg(0x90, 0x00);
    _writeReg(0x91, 0x00);
    _writeReg(0x92, 0x19);
    _writeReg(0x96, 0x00);
    _writeReg(0x9a, 0x80);
    _writeReg(0xb0, 0x84);
    _writeReg(0xb1, 0x0c);
    _writeReg(0xb2, 0x0e);
    _writeReg(0xb3, 0x82);
    _writeReg(0xb8, 0x0a);
    _writeReg(0x43, 0x0a);
    _writeReg(0x44, 0xf0);
    _writeReg(0x45, 0x34);
    _writeReg(0x46, 0x58);
    _writeReg(0x47, 0x28);
    _writeReg(0x48, 0x3a);
    _writeReg(0x59, 0x88);
    _writeReg(0x5a, 0x88);
    _writeReg(0x5b, 0x44);
    _writeReg(0x5c, 0x67);
    _writeReg(0x5d, 0x49);
    _writeReg(0x5e, 0x0e);
    _writeReg(0x64, 0x04);
    _writeReg(0x65, 0x20);
    _writeReg(0x66, 0x05);
    _writeReg(0x94, 0x04);
    _writeReg(0x95, 0x08);
    _writeReg(0x6c, 0x0a);
    _writeReg(0x6d, 0x55);
    _writeReg(0x6e, 0x11);
    _writeReg(0x6f, 0x9f);
    _writeReg(0x6a, 0x40);
    _writeReg(0x01, 0x40);
    _writeReg(0x02, 0x40);
    _writeReg(0x13, 0xe7);
    _writeReg(0x4f, 0x86);
    _writeReg(0x50, 0x86);
    _writeReg(0x51, 0x00);
    _writeReg(0x52, 0x23);
    _writeReg(0x53, 0x62);
    _writeReg(0x54, 0x86);
    _writeReg(0x58, 0x9e);
    _writeReg(0x41, 0x08);
    _writeReg(0x3f, 0x1f);
    _writeReg(0x75, 0x10);
    _writeReg(0x76, 0xc0);
    _writeReg(0x4c, 0xff);
    _writeReg(0x77, 0x01);
    _writeReg(0x3d, 0xc2);
    _writeReg(0x4b, 0x09);
    _writeReg(0xc9, 0xf0);
    
    _writeReg(0x41, 0x3a);

    _writeReg(0x34, 0x11);
    _writeReg(0x3b, 0x0a);
    _writeReg(0xa4, 0x88);                                               
    _writeReg(0x96, 0x00);
    _writeReg(0x97, 0x30);
    _writeReg(0x98, 0x20);
    _writeReg(0x99, 0x30);
    _writeReg(0x9a, 0x84);
    _writeReg(0x9b, 0x29);
    _writeReg(0x9c, 0x03);
    _writeReg(0x9d, 0x4b);
    _writeReg(0x9e, 0x3f);
    _writeReg(0x78, 0x04);
    _writeReg(0x79, 0x01);
    _writeReg(0xc8, 0xf0);
    _writeReg(0x79, 0x0f);
    _writeReg(0xc8, 0x00);
    _writeReg(0x79, 0x10);
    _writeReg(0xc8, 0x7e);
    _writeReg(0x79, 0x0a);
    _writeReg(0xc8, 0x80);
    _writeReg(0x79, 0x0b);
    _writeReg(0xc8, 0x01);
    _writeReg(0x79, 0x0c);
    _writeReg(0xc8, 0x0f);
    _writeReg(0x79, 0x0d);
    _writeReg(0xc8, 0x20);
    _writeReg(0x79, 0x09);
    _writeReg(0xc8, 0x80);
    _writeReg(0x79, 0x02);
    _writeReg(0xc8, 0xc0);
    _writeReg(0x79, 0x03);
    _writeReg(0xc8, 0x40);
    _writeReg(0x79, 0x05);
    _writeReg(0xc8, 0x30);
    _writeReg(0x79, 0x26);
    _writeReg(0x2d, 0x00);
    _writeReg(0x2e, 0x00);
}

#include "sccb.h"
#include "delay.h"

static void SCCB_InitGPIO(SCCB_InitTypeDef *SCCB_InitStruct);
static void SCCB_TurnToOutput(SCCB_InitTypeDef *SCCB_InitStruct);
static void SCCB_TurnToInput(SCCB_InitTypeDef *SCCB_InitStruct);
static void SCCB_WriteStart(SCCB_InitTypeDef *SCCB_InitStruct);
static void SCCB_WriteStop(SCCB_InitTypeDef *SCCB_InitStruct);
static void SCCB_WriteNA(SCCB_InitTypeDef *SCCB_InitStruct);
static uint8_t SCCB_WriteByte(SCCB_InitTypeDef *SCCB_InitStruct, uint8_t m_data);
static uint8_t SCCB_ReadByte(SCCB_InitTypeDef *SCCB_InitStruct);

#define THIS SCCB_InitStruct

#define _initGPIO()         SCCB_InitGPIO(THIS)
#define _turnToIutput()     SCCB_TurnToInput(THIS)
#define _turnToOutput()     SCCB_TurnToOutput(THIS)
#define _writeStart()       SCCB_WriteStart(THIS)
#define _writeStop()        SCCB_WriteStop(THIS)
#define _writeNA()          SCCB_WriteNA(THIS)
#define _writeByte(value)   SCCB_WriteByte(THIS, value)
#define _readByte()         SCCB_ReadByte(THIS)

#define _setSCL()           GPIO_SetBits(THIS->SIO_Port, THIS->SCL_Pin)
#define _resetSCL()         GPIO_ResetBits(THIS->SIO_Port, THIS->SCL_Pin)

#define _setSDA()           GPIO_SetBits(THIS->SIO_Port, THIS->SDA_Pin)
#define _resetSDA()         GPIO_ResetBits(THIS->SIO_Port, THIS->SDA_Pin)

#define _getSDA()	    GPIO_ReadInputDataBit(THIS->SIO_Port, THIS->SDA_Pin)


void SCCB_init(SCCB_InitTypeDef *SCCB_InitStruct) {
    _initGPIO();
}

ErrorStatus SCCB_3PhaseWrite(SCCB_InitTypeDef *SCCB_InitStruct,
    uint8_t const id, uint8_t const address, uint8_t const value)
{
    _writeStart();
    if (SUCCESS != _writeByte(id)) goto except;
    Delay_us(100);
    if (SUCCESS != _writeByte(address)) goto except;
    Delay_us(100);
    if (SUCCESS != _writeByte(value)) goto except;
    Delay_us(100);
    _writeStop();
    Delay_us(100);
    return SUCCESS;
except:
    _writeStop();
    Delay_us(100);
    return ERROR;
}

ErrorStatus SCCB_2PhaseWrite(SCCB_InitTypeDef *SCCB_InitStruct,
    uint8_t const id, uint8_t const address)
{
    _writeStart();
    if (SUCCESS != _writeByte(id)) goto except;
    Delay_us(100);
    if (SUCCESS != _writeByte(address)) goto except;
    _writeStop();
    Delay_us(100);
    return SUCCESS;
except:
    _writeStop();
    Delay_us(100);
    return ERROR;
}

ErrorStatus SCCB_2PhaseRead(SCCB_InitTypeDef *SCCB_InitStruct,
    uint8_t const id, uint8_t *value)
{
    _writeStart();
    if (SUCCESS != _writeByte(id)) goto except;
    Delay_us(100);
    *value = _readByte();
    _writeNA();
    _writeStop();
    Delay_us(100);
    return SUCCESS;
except:
    _writeStop();
    Delay_us(100);
    return ERROR;
}        


void SCCB_InitGPIO(SCCB_InitTypeDef *SCCB_InitStruct) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(THIS->RCC_APB2Periph_GPIO_SIO, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = THIS->SCL_Pin | THIS->SDA_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(THIS->SIO_Port, &GPIO_InitStructure); 
}

void SCCB_TurnToOutput(SCCB_InitTypeDef *SCCB_InitStruct) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin =  THIS->SDA_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(THIS->SIO_Port, &GPIO_InitStructure);
}

void SCCB_TurnToInput(SCCB_InitTypeDef *SCCB_InitStruct) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin =  THIS->SDA_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(THIS->SIO_Port, &GPIO_InitStructure);
}

void SCCB_WriteStart(SCCB_InitTypeDef *SCCB_InitStruct) {
    _setSDA();
    Delay_us(500);

    _setSCL();
    Delay_us(500);
 
    _resetSDA();
    Delay_us(500);

    _resetSCL();
    Delay_us(500);
}

void SCCB_WriteStop(SCCB_InitTypeDef *SCCB_InitStruct) {
    _resetSDA();
    Delay_us(500);
 
    _setSCL();	
    Delay_us(500);  

    _setSDA();	
    Delay_us(500);
}

void SCCB_WriteNA(SCCB_InitTypeDef *SCCB_InitStruct) {	
	_setSDA();	
	Delay_us(500);
	
	_setSCL();	
	Delay_us(500);
	
	_resetSCL();	
	Delay_us(500);
	
	_resetSDA();	
	Delay_us(500);
}

uint8_t SCCB_WriteByte(SCCB_InitTypeDef *SCCB_InitStruct, uint8_t m_data) {
	uint8_t j, tem;

    for(j = 0; j < 8; j++) {
		if((m_data << j) & 0x80)
            _setSDA();
        else
            _resetSDA();
		Delay_us(500);
        
		_setSCL();	
		Delay_us(500);
        
		_resetSCL();	
		Delay_us(500);
	}
	Delay_us(100);
    
	_turnToIutput();
	Delay_us(500);
    
	_setSCL();	
	Delay_us(100);
    
	if(_getSDA())
        tem = 0;   //SDA=1发送失败，返回0}
	else
        tem = 1;   //SDA=0发送成功，返回1
	_resetSCL();	
	Delay_us(500);    
    _turnToOutput();
    
	return tem;  
}

uint8_t SCCB_ReadByte(SCCB_InitTypeDef *SCCB_InitStruct) {
	uint8_t read, j;
	read = 0x00;
	
	_turnToIutput();
	Delay_us(500);
    
	for(j = 8; j > 0; j--) {		     
		Delay_us(500);
        
		_setSCL();
		Delay_us(500);        
		read <<= 1;
		if(_getSDA()) read ++;
		_resetSCL();
		Delay_us(500);
	}
	_turnToOutput();
    
	return read;
}

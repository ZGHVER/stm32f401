#ifndef __JDY_H_
#define __JDY_H_

#include"stm32f4xx.h"
#include"PinDef.h"


#define JDY_UART                USART2
#define JDY_UART_CLKSouce       RCC_APB1Periph_USART2
#define JDY_UART_BaundRate      9600 

#define JDY_UART_TX_GPIO        GPIOA
#define JDY_GPIO_TX_CLKSouce    RCC_AHB1Periph_GPIOA
#define JDY_TXPin               GPIO_Pin_3
#define JDY_TXPinSouce          GPIO_PinSource3

#define JDY_UART_RX_GPIO        GPIOA
#define JDY_GPIO_RX_CLKSouce    RCC_AHB1Periph_GPIOA
#define JDY_RXPin               GPIO_Pin_2
#define JDY_RXPinSouce          GPIO_PinSource2  

#define JDY_TX_RX_AF            GPIO_AF_USART2

#define JDY_SET_GPIO            GPIOA
#define JDY_GPIO_SET_CLK        RCC_AHB1Periph_GPIOA
#define JDY_SETPin              GPIO_Pin_14

#define JDY_CS_GPIO             GPIOA
#define JDY_GPIO_CS_CLK         RCC_AHB1Periph_GPIOA
#define JDY_CSPin               GPIO_Pin_15


#define JDY_SET                 PAout(2)         
#define JDY_CS                  PAout(3)     

#define JDY_BaundRate_1200      '1'
#define JDY_BaundRate_2400      '2'
#define JDY_BaundRate_4800      '3'
#define JDY_BaundRate_9600      '4'
#define JDY_BaundRate_14400     '5'
#define JDY_BaundRate_19200     '6'

static const uint8_t BaundRate[] = {'A', 'T', '+', 'B', 'A', 'U', 'D'}; 
static const uint8_t C_SizeOf_BaundRate = 7;
static const uint8_t RFID[] = {'A', 'T', '+', 'R', 'F', 'I', 'D'}; 
static const uint8_t C_SizeOf_RFID = 7;
static const uint8_t DVID[] = {'A', 'T', '+', 'D', 'V', 'I', 'D'}; 
static const uint8_t C_SizeOf_DVID = 7;
static const uint8_t RFC[] = {'A', 'T', '+', 'R', 'F', 'C'}; 
static const uint8_t C_SizeOf_RFC = 6;
static const uint8_t POWE[] = {'A', 'T', '+', 'P', 'O', 'W', 'E'}; 
static const uint8_t C_SizeOf_POWE = 7;
static const uint8_t CLSS[] = {'A', 'T', '+', 'C', 'L', 'S', 'S'}; 
static const uint8_t C_SizeOf_CLSS = 7;

void JDY_Init(void);
void JDY_Set_BaundRate(uint8_t BaundRate_code);
void JDY_Set_RFID(uint8_t[]);
void JDY_Set_DVID(uint8_t[]);
void JDY_Set_RFC(uint8_t);
void JDY_Set_POWE(uint8_t);
void JDY_Set_CLSS(uint8_t);

#endif

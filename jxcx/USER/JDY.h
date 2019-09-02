#ifndef __JDY_H_
#define __JDY_H_

#include"stm32f4xx.h"
#include"PinDef.h"


#define JDY_UART                USART2
#define JDY_UART_CLKSouce       RCC_APB1Periph_USART2
#define JDY_UART_BoundRate      19200 

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

#define JDY_BoundRate_1200      1
#define JDY_BoundRate_2400      2
#define JDY_BoundRate_4800      3
#define JDY_BoundRate_9600      4
#define JDY_BoundRate_14400     5
#define JDY_BoundRate_19200     6

#define JDY_SET                 PAout(2)         
#define JDY_CS                  PAout(3)     



void JDY_Init();
void JDY_Set_BoundRate(uint8_t boundrate_code);



#endif
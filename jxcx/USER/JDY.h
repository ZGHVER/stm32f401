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

#define JDY_SET_GPIO            GPIOB
#define JDY_GPIO_SET_CLK        RCC_AHB1Periph_GPIOB
#define JDY_SETPin              GPIO_Pin_2

#define JDY_CS_GPIO             GPIOB
#define JDY_GPIO_CS_CLK         RCC_AHB1Periph_GPIOB
#define JDY_CSPin               GPIO_Pin_3

#define JDY_SET                 PBout(2)         
#define JDY_CS                  PBout(3)     



void JDY_Init();




#endif
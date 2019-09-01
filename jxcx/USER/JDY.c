#include"JDY.h"

__STATIC_INLINE void JDY_Pin_Init();
__STATIC_INLINE void JDY_UART_Init();

void JDY_Init(){
    JDYPin_Init();
    JDY_UARTInit();
}

__STATIC_INLINE void JDY_Pin_Init(){
    GPIO_InitTypeDef InitTYpe;
    InitTYpe.GPIO_Mode = GPIO_Mode_OUT;
    InitTYpe.GPIO_OType = GPIO_OType_PP;
    InitTYpe.GPIO_PuPd = GPIO_PuPd_UP;
    InitTYpe.GPIO_Speed = GPIO_Speed_50MHz;

    InitTYpe.GPIO_Pin = JDY_CSPin;
    RCC_AHB1PeriphClockCmd(JDY_GPIO_CS_CLK, ENABLE);
    GPIO_Init(JDY_CS_GPIO, &InitTYpe);

    InitTYpe.GPIO_Pin = JDY_SETPin;
    RCC_AHB1PeriphClockCmd(JDY_GPIO_SET_CLK, ENABLE);
    GPIO_Init(JDY_SET_GPIO, &InitTYpe);
}

__STATIC_INLINE void JDY_UART_Init(){
    GPIO_InitTypeDef InitType;
    InitType.GPIO_Mode = GPIO_Mode_AF;
    InitType.GPIO_PuPd = GPIO_PuPd_UP;
    InitType.GPIO_Speed = GPIO_Speed_50MHz;

    InitType.GPIO_Pin = JDY_TXPin;
    RCC_AHB1PeriphClockCmd(JDY_GPIO_TX_CLKSouce, ENABLE);
    GPIO_Init(JDY_UART_TX_GPIO, &InitType);

    InitType.GPIO_Pin = JDY_RXPin;
    RCC_AHB1PeriphClockCmd(JDY_GPIO_RX_CLKSouce, ENABLE);
    GPIO_Init(JDY_UART_RX_GPIO, &InitType);

    GPIO_PinAFConfig(JDY_UART_TX_GPIO, JDY_RXPinSouce, JDY_TX_RX_AF);
    GPIO_PinAFConfig(JDY_UART_TX_GPIO, JDY_TXPinSouce, JDY_TX_RX_AF);

    USART_InitTypeDef Usart_init;
    Usart_init.USART_BaudRate = JDY_UART_BoundRate;//波特率
    Usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流
    Usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    Usart_init.USART_Parity = USART_Parity_No;//奇偶校验
    Usart_init.USART_StopBits = USART_StopBits_1;//停止位
    Usart_init.USART_WordLength = USART_WordLength_8b;//数据位
    RCC_APB1PeriphClockCmd(JDY_UART_CLKSouce, ENABLE);
    USART_Init(JDY_UART, &Usart_init);
}


#include"JDY.h"
#include"oled.h"
// VCC RXD TXD SET CS GND 
__STATIC_INLINE void JDY_Pin_Init();
__STATIC_INLINE void JDY_UART_Init();

char s[20];
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
    Usart_init.USART_BaudRate = JDY_UART_BoundRate;                             //波特率
    Usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;      //硬件流
    Usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    Usart_init.USART_Parity = USART_Parity_No;                                  //奇偶校验
    Usart_init.USART_StopBits = USART_StopBits_1;                               //停止位
    Usart_init.USART_WordLength = USART_WordLength_8b;                          //数据位
    RCC_APB1PeriphClockCmd(JDY_UART_CLKSouce, ENABLE);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef USART2_NvicInit;
    USART2_NvicInit.NVIC_IRQChannel = USART2_IRQn;                       //中断频道
    USART2_NvicInit.NVIC_IRQChannelCmd = ENABLE;
    USART2_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //抢占优先级，主优先级
    USART2_NvicInit.NVIC_IRQChannelSubPriority = 0;            //提交优先级，次优先级
    NVIC_Init(&USART2_NvicInit);
    
    USART_Init(JDY_UART, &Usart_init);

}




void JDY_Set_BoundRate(uint8_t boundrate_code){
    JDY_CS = 0;
    JDY_SET = 0;
    sprintf(s, "AT+BAUD<%d>\r\n", boundrate_code);
    for(u8 i = 0; i < 20; i++){
        USART_SendData(JDY_UART, s[i]);
        if(s[i] == '\n')
            break;
    }
    JDY_CS = 0;
    JDY_SET = 1;
}

__STATIC_INLINE void JDY_RXHandler(){
    static uint8_t c = 0;
    uint16_t ResData = USART_ReceiveData(JDY_UART);
    OLED_ShowChar(0, c, (uint8_t)ResData, 12, 1);
    c += 8;
    
}

void USART2_IRQHandler(){
    if(USART_GetITStatus(JDY_UART, USART_IT_RXNE) == SET){
        JDY_RXHandler();
    }
    USART_ClearITPendingBit(JDY_UART, USART_IT_RXNE);
}


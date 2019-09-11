
#include"JDY.h"
#include"oled.h"
// VCC RXD TXD SET CS GND 

volatile uint8_t temp_String[40];
volatile uint8_t temp_Index = 0;
volatile uint8_t isResFinish = 0;

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
    Usart_init.USART_BaudRate = JDY_UART_BaundRate;                             //波特率
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
    USART_Cmd(USART2, ENABLE);

}

void JDY_Init(){
    JDY_Pin_Init();
    JDY_UART_Init();
}

__STATIC_INLINE void JDY_SendString(uint8_t s[], uint8_t s_size){
    for(uint8_t i = 0; i < s_size; i++){
        USART_SendData(JDY_UART, s[i]);
        while(!USART_GetFlagStatus(JDY_UART, USART_FLAG_TC));
    }
}

void JDY_Set_BaundRate(uint8_t BaundRate_code){
    JDY_CS = 0;
    JDY_SET = 0;

    JDY_SendString((uint8_t*)BaundRate, C_SizeOf_BaundRate);
    USART_SendData(JDY_UART, BaundRate_code);
    while(!USART_GetFlagStatus(JDY_UART, USART_FLAG_TC));
    JDY_SendString("\r\n", 2);
    JDY_CS = 0;
    JDY_SET = 1;
}

void JDY_Set_RFID(uint8_t RFID_code[]){
    JDY_CS = 0;
    JDY_SET = 0;
    JDY_SendString((uint8_t*)RFID, C_SizeOf_RFID);
    JDY_SendString(RFID_code, 4);
    JDY_SendString("\r\n", 2);
    JDY_CS = 0;
    JDY_SET = 1;

}

void JDY_Set_DVID(uint8_t DVID_code[]){
    JDY_CS = 0;
    JDY_SET = 0;
    JDY_SendString((uint8_t*)DVID, C_SizeOf_DVID);
    JDY_SendString(DVID_code, 4);
    JDY_SendString("\r\n", 2);
    JDY_CS = 0;
    JDY_SET = 1;

}

void JDY_Set_RFC(uint8_t RFC_code){
    JDY_CS = 0;
    JDY_SET = 0;
    JDY_SendString((uint8_t*)RFC, C_SizeOf_RFC);
    JDY_SendString(&RFC_code, 3);
    JDY_SendString("\r\n", 2);
    JDY_CS = 0;
    JDY_SET = 1;

}

void JDY_Set_POWE(uint8_t POWE_code){
    JDY_CS = 0;
    JDY_SET = 0;
    JDY_SendString((uint8_t*)POWE, C_SizeOf_POWE);
    USART_SendData(JDY_UART, POWE_code);
    while(!USART_GetFlagStatus(JDY_UART, USART_FLAG_TC));
    JDY_SendString("\r\n", 2);
    JDY_CS = 0;
    JDY_SET = 1;

}

void JDY_Set_CLSS(uint8_t CLSS_code){
    JDY_CS = 0;
    JDY_SET = 0;
    JDY_SendString((uint8_t*)CLSS, C_SizeOf_CLSS);
    USART_SendData(JDY_UART, CLSS_code);
    while(!USART_GetFlagStatus(JDY_UART, USART_FLAG_TC));
    JDY_SendString("\r\n", 2);
    JDY_CS = 0;
    JDY_SET = 1;
}

__STATIC_INLINE void JDY_RXHandler(){
    static uint8_t last = 0;
    uint16_t ResData = USART_ReceiveData(JDY_UART);
    if(ResData == '\r')
        last = 1;
    else if(last == 1){
        if(ResData == '\n'){
            isResFinish = 1;
            last = 0;
        }
        else{
            isResFinish = 2;
            last = 0;
            temp_Index = 0;
        }
    }else{ 
        temp_String[temp_Index++] = ResData;
        OLED_ShowChar(temp_Index * 8, 0, ResData, 12, 1);
        temp_Index++;
    }
}

void USART2_IRQHandler(){
    if(USART_GetITStatus(JDY_UART, USART_IT_RXNE) == SET){
        JDY_RXHandler();
    }
    USART_ClearITPendingBit(JDY_UART, USART_IT_RXNE);
}


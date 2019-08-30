#include"CRMERA.h"
#include"delay.h"
#include"oled.h"
volatile uint8_t TData[7];
volatile uint8_t Finish = 0;


void CAMERA_Init(){

    GPIO_InitTypeDef ioAc;
    ioAc.GPIO_Pin = GPIO_Pin_2;
    ioAc.GPIO_Mode = GPIO_Mode_OUT;
    ioAc.GPIO_OType = GPIO_OType_PP;
    ioAc.GPIO_PuPd = GPIO_PuPd_UP;
    ioAc.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &ioAc);




    GPIO_InitTypeDef ioA;
    ioA.GPIO_Pin = GPIO_Pin_12;
    ioA.GPIO_Mode = GPIO_Mode_AF;
    ioA.GPIO_PuPd = GPIO_PuPd_UP;
    ioA.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &ioA);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USART6);

    USART_InitTypeDef Usart6_init;
    Usart6_init.USART_BaudRate = 115200;//波特率
    Usart6_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流
    Usart6_init.USART_Mode = USART_Mode_Rx;
    Usart6_init.USART_Parity = USART_Parity_No;//奇偶校验
    Usart6_init.USART_StopBits = USART_StopBits_1;//停止位
    Usart6_init.USART_WordLength = USART_WordLength_8b;//数据位
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
    USART_Init(USART6, &Usart6_init);

    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

    NVIC_InitTypeDef usart6_NvicInit;
    usart6_NvicInit.NVIC_IRQChannel = USART6_IRQn;                       //中断频道
    usart6_NvicInit.NVIC_IRQChannelCmd = ENABLE;
    usart6_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //抢占优先级，主优先级
    usart6_NvicInit.NVIC_IRQChannelSubPriority = 1;            //提交优先级，次优先级
    NVIC_Init(&usart6_NvicInit);
    USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);
    USART_Cmd(USART6, ENABLE);
}

uint8_t CAMERA_ResData(uint16_t* data){
    int16_t tims = 1500;
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
    GPIOA->ODR &= ~GPIO_Pin_2;
    while(Finish == 0 && tims > 0){
        delay_us(50);
        tims --;
    }
    if(tims == 0){
        GPIOA->ODR |= GPIO_Pin_2;
        return 0;
    }
    data[0] = TData[0] + TData[1];
    data[1] = TData[2] + TData[3];
    data[2] = TData[4] + TData[5];
    data[3] = TData[6];
    Finish = 0;
    USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);
    return 1;
}

volatile uint16_t res = 0;

void USART6_IRQHandler(){
    static uint8_t Counter = 0;
    GPIOA->ODR |= GPIO_Pin_2;

    if(USART_GetITStatus(USART6, USART_IT_RXNE) != SET){
        USART_ClearITPendingBit(USART6, USART_IT_RXNE); 
        return;
    }
     
    res = USART_ReceiveData(USART6);
    TData[Counter++] = res;
    if(Counter == 7){
        Counter = 0;
        Finish = 1;
    }

    USART_ClearITPendingBit(USART6, USART_IT_RXNE); 

}
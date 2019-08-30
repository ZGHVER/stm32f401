#include"serial_key.h"
#include"stm32f4xx_usart.h"


ACTION_FUN action_list[17];
void SE_KeyMap_init(){
    
    for(int i = 0; i < 16; i++)
        action_list[i] = VOID_FUN;

    GPIO_InitTypeDef ioA;
    ioA.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    ioA.GPIO_Mode = GPIO_Mode_AF;
    ioA.GPIO_OType = GPIO_OType_PP;
    ioA.GPIO_PuPd = GPIO_PuPd_UP;
    ioA.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &ioA);

    GPIO_PinAFConfig(GPIOA, EXTI_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, EXTI_PinSource3, GPIO_AF_USART2);



    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    USART_InitTypeDef Usart2_init;
    Usart2_init.USART_BaudRate = 115200;
    Usart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    Usart2_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    Usart2_init.USART_Parity = USART_Parity_No;
    Usart2_init.USART_StopBits = USART_StopBits_1;
    Usart2_init.USART_WordLength = USART_WordLength_8b;

    USART_Init(USART2, &Usart2_init);
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    NVIC_InitTypeDef USART2_NvicInit;
    USART2_NvicInit.NVIC_IRQChannel = USART2_IRQn;                       //�ж�Ƶ��
    USART2_NvicInit.NVIC_IRQChannelCmd = ENABLE;
    USART2_NvicInit.NVIC_IRQChannelPreemptionPriority = 0;     //��ռ���ȼ��������ȼ�
    USART2_NvicInit.NVIC_IRQChannelSubPriority = 1;            //�ύ���ȼ��������ȼ�
    NVIC_Init(&USART2_NvicInit);
    
    USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(){
	  unsigned char resdat;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET){
				  resdat = USART_ReceiveData(USART2);
        if(resdat >= 0x01 && resdat <= 0x10)
            action_list[resdat]();
    }
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}

void SE_KeyMap_setFun(ACTION_FUN fun, u8 key_number){
    action_list[key_number] = fun;
}

void VOID_FUN(){
}

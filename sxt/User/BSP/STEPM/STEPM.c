#include "STEPM.h"
#include "os.h"
#include"core_cm4.h"

__IO uint16_t tim3_counte = 0;
__IO uint16_t tim3_t = 2048;
__IO uint16_t tim4_counte = 0;
__IO uint16_t tim4_t = 1500;

void STEPM_Init(){
    GPIO_InitTypeDef ioB;
    ioB.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    ioB.GPIO_Mode = GPIO_Mode_AF;
    ioB.GPIO_OType = GPIO_OType_PP;
    ioB.GPIO_PuPd = GPIO_PuPd_UP;
    ioB.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_Init(GPIOB, &ioB);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseInitTypeDef tim3;
    tim3.TIM_ClockDivision = TIM_CKD_DIV1;
    tim3.TIM_CounterMode = TIM_CounterMode_Up;      //������ģʽ
    tim3.TIM_Period = 2000;                         //��װ��ֵ
    tim3.TIM_Prescaler = 84 - 1;                   //��Ƶϵ��
    TIM_TimeBaseInit(TIM3, &tim3);  
    TIM_TimeBaseInit(TIM4, &tim3);  

    TIM_OCInitTypeDef TIM4_oc1_Init;                            //��ʱ��4Ƶ��1PWM��ʼ��
    TIM4_oc1_Init.TIM_OCMode = TIM_OCMode_PWM1;                 //PWMģʽѡ��
    TIM4_oc1_Init.TIM_OCPolarity = TIM_OCPolarity_High;         //PWM��Ч��ƽ����
    TIM4_oc1_Init.TIM_OutputState = TIM_OutputState_Enable;     //����Ƚ�״̬
    TIM_OC1Init(TIM4, &TIM4_oc1_Init);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_SetCompare1(TIM4, 1000);
    

    TIM_OCInitTypeDef TIM3_oc2_Init;                            //��ʱ��3Ƶ��2PWM��ʼ��
    TIM3_oc2_Init.TIM_OCMode = TIM_OCMode_PWM1;                 //PWMģʽѡ��
    TIM3_oc2_Init.TIM_OCPolarity = TIM_OCPolarity_High;         //PWM��Ч��ƽ����
    TIM3_oc2_Init.TIM_OutputState = TIM_OutputState_Enable;     //����Ƚ�״̬
    TIM_OC2Init(TIM3, &TIM3_oc2_Init);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_SetCompare2(TIM3, 1000);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef tim3_NvicInit;
    tim3_NvicInit.NVIC_IRQChannel = TIM3_IRQn;                       //�ж�Ƶ��
    tim3_NvicInit.NVIC_IRQChannelCmd = ENABLE;
    tim3_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //��ռ���ȼ��������ȼ�
    tim3_NvicInit.NVIC_IRQChannelSubPriority = 0;            //�ύ���ȼ��������ȼ�
    NVIC_Init(&tim3_NvicInit);

    NVIC_InitTypeDef tim4_NvicInit;
    tim4_NvicInit.NVIC_IRQChannel = TIM4_IRQn;                       //�ж�Ƶ��
    tim4_NvicInit.NVIC_IRQChannelCmd = ENABLE;
    tim4_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //��ռ���ȼ��������ȼ�
    tim4_NvicInit.NVIC_IRQChannelSubPriority = 0;            //�ύ���ȼ��������ȼ�
    NVIC_Init(&tim4_NvicInit);
}



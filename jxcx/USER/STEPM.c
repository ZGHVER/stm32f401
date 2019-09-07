#include "STEM.h"


void STEPM_Init(){
    GPIO_InitTypeDef ioB;
    ioB.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    ioB.GPIO_Mode = GPIO_Mode_AF;
    ioB.GPIO_OType = GPIO_OType_PP;
    ioB.GPIO_PuPd = GPIO_PuPd_UP;
    ioB.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_Init(GPIOB, &ioB);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef tim3;
    tim3.TIM_ClockDivision = TIM_CKD_DIV1;
    tim3.TIM_CounterMode = TIM_CounterMode_Up;      //计数器模式
    tim3.TIM_Period = 1000;                         //重装载值
    tim3.TIM_Prescaler = 84 - 1;                   //分频系数
    TIM_TimeBaseInit(TIM3, &tim3);  
    TIM_TimeBaseInit(TIM4, &tim3);  

    TIM_OCInitTypeDef TIM4_oc1_Init;                            //定时器4频道1PWM初始化
    TIM4_oc1_Init.TIM_OCMode = TIM_OCMode_PWM1;                 //PWM模式选择
    TIM4_oc1_Init.TIM_OCPolarity = TIM_OCPolarity_High;         //PWM有效电平设置
    TIM4_oc1_Init.TIM_OutputState = TIM_OutputState_Enable;     //输出比较状态
    TIM_OC1Init(TIM4, &TIM4_oc1_Init);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_SetCompare1(TIM4, 1000);
    

    TIM_OCInitTypeDef TIM3_oc2_Init;                            //定时器3频道2PWM初始化
    TIM3_oc2_Init.TIM_OCMode = TIM_OCMode_PWM1;                 //PWM模式选择
    TIM3_oc2_Init.TIM_OCPolarity = TIM_OCPolarity_High;         //PWM有效电平设置
    TIM3_oc2_Init.TIM_OutputState = TIM_OutputState_Enable;     //输出比较状态
    TIM_OC2Init(TIM3, &TIM3_oc2_Init);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_SetCompare2(TIM3, 1000);
}

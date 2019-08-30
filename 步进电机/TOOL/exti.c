#include"exti.h"
#include"delay.h"
void f(void);
void exit_init(void){//中断初始化函数
    //设置中断分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    //使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFGEN, ENABLE);
    //把接受中断的引脚初始化并设置为输入模式
    Key_Init();
    //设置中断的IO组和引脚号
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource1);
    //初始化中断线路，触发方式
    //中断对IO口的正常使用无影响
    EXTI_InitTypeDef exit_init;
    exit_init.EXTI_Line = EXTI_Line1;               //设置中断线路
    exit_init.EXTI_LineCmd = ENABLE;                //使能中断
    exit_init.EXTI_Mode = EXTI_Mode_Interrupt;      //外部中断模式
    exit_init.EXTI_Trigger = EXTI_Trigger_Rising;   //触发方式设置为上升沿触发
    EXTI_Init(&exit_init);
    //设置中断优先级
    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = EXTI1_IRQn;              //设置中断线路
    nvic.NVIC_IRQChannelCmd = ENABLE;               //使能纵断管理
    nvic.NVIC_IRQChannelPreemptionPriority = 0;     //设置抢占优先级
    nvic.NVIC_IRQChannelSubPriority = 0;            //设置响应优先级
    NVIC_Init(&nvic);
    f();
  
}
void f(){
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource3);
    EXTI_InitTypeDef def;
    def.EXTI_Line = EXTI_Line3;
    def.EXTI_LineCmd = ENABLE;
    def.EXTI_Mode = EXTI_Mode_Interrupt;
    def.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&def);

    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = EXTI3_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic);

}
void EXTI1_IRQHandler(){   //中断线1服务函数
    delay_ms(10);
    if((GPIOD->IDR & GPIO_Pin_1) == GPIO_Pin_1)
        GPIOA->ODR ^= GPIO_Pin_All;
    EXTI_ClearITPendingBit(EXTI_Line1);     //清除中断标志位
}

void EXTI3_IRQHandler(){  //中断线2服务函数
     delay_ms(10);
    if((GPIOD->IDR & GPIO_Pin_3) == GPIO_Pin_3)
        GPIOA->ODR ^= GPIO_Pin_All;   
    EXTI_ClearITPendingBit(EXTI_Line3);
}

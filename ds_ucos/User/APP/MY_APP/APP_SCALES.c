#include"app.h"
#include"./SCALES/SCALES.h"
#include"./OLED/oled.h"
#include "os_cfg_app.h"

extern volatile uint16_t ADCval[100];

#define CAL_TSize 128
#define CAL_PRIO 11
OS_TCB  CAL_TCB;
CPU_STK CAL_STK[CAL_TSize];
void SCALES_CAL_Task(void* p_args);
OS_SEM CAL_sem;


OS_TMR SENSOR_Tmr;
void SENSOR_TIM_CallBack(void *p_tmr, void *p_arg);

__STATIC_INLINE void SW_Init(){
    GPIO_InitTypeDef ioA;
    ioA.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    ioA.GPIO_Mode = GPIO_Mode_IN;
    ioA.GPIO_PuPd = GPIO_PuPd_UP;
    ioA.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &ioA);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFGEN, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);//GPIOA pin4 外部中断初始化
    EXTI_InitTypeDef IOAPIN4;
    IOAPIN4.EXTI_Line = EXTI_Line4;
    IOAPIN4.EXTI_LineCmd = ENABLE;
    IOAPIN4.EXTI_Mode = EXTI_Mode_Interrupt;
    IOAPIN4.EXTI_Trigger = EXTI_Trigger_Falling;  //触发方式
    EXTI_Init(&IOAPIN4);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);//GPIOA pin5 外部中断初始化
    EXTI_InitTypeDef IOAPIN5;
    IOAPIN5.EXTI_Line = EXTI_Line5;
    IOAPIN5.EXTI_LineCmd = ENABLE;
    IOAPIN5.EXTI_Mode = EXTI_Mode_Interrupt;
    IOAPIN5.EXTI_Trigger = EXTI_Trigger_Falling;  //触发方式
    EXTI_Init(&IOAPIN5);
}

void SCALES_Start(){
    OS_ERR err;
    SCALES_Init();
    SW_Init();
    OSTaskCreate(
        (  OS_TCB*   )&CAL_TCB,                                   //任务控制块
        ( CPU_CHAR*  )"SCALES_CAL_Task",                          //任务名
        (OS_TASK_PTR )SCALES_CAL_Task,                            //任务函数指针
        (   void*    )0,                                          //首次运行时传递的参数
        (  OS_PRIO   )CAL_PRIO,                                   //任务优先级
        (  CPU_STK*  )&CAL_STK[0],                                //任务堆栈基地址
        (CPU_STK_SIZE)CAL_TSize / 10,                             //可用最大堆栈空间
        (CPU_STK_SIZE)CAL_TSize,                                  //任务堆栈大小
        ( OS_MSG_QTY )5,                                          //任务可接收的最大消息数
        (  OS_TICK   )0,                                          //在任务之间循环时的时间片的时间量（以刻度表示）指定0以使用默认值
        (   void*    )0,                                          //TCB扩展指针
        (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //包含有关任务行为的其他信息（或选项）
        (  OS_ERR*   )&err                                        //错误变量
    );

    OSTmrCreate (
        (OS_TMR*            )  &SENSOR_Tmr,
        (CPU_CHAR*          )  "sensor",
        (OS_TICK            )  50,
        (OS_TICK            )  50,
        (OS_OPT             )  OS_OPT_TMR_PERIODIC,
        (OS_TMR_CALLBACK_PTR)  SENSOR_TIM_CallBack,
        (void*              )  0,
        (OS_ERR*            )  &err
    );
    osError_hander(err);

    OSSemCreate(&CAL_sem, "ok", 0, &err);
    osError_hander(err);
}

void SENSOR_TIM_CallBack(void *p_tmr, void *p_arg){
    OS_ERR err;
    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
    osError_hander(err);

}

void SCALES_CAL_Task(void* p_args){  
    OS_ERR err; 
    OSTmrStart(&SENSOR_Tmr, &err);
    while(1){
        OSSemPend(&CAL_sem, 0, OS_OPT_PEND_BLOCKING, (CPU_TS*)0, &err);
        osError_hander(err);
        OLED_ShowNum(0, 12, ADCval[0], 5, 12);
        OLED_ShowNum(50, 12, ADCval[1], 5, 12);
        OLED_ShowNum(0, 24, ADCval[2], 5, 12);
        OLED_ShowNum(50, 24, ADCval[3], 5, 12);
        OLED_ShowNum(0, 36, ADCval[4], 5, 12);
        OLED_ShowNum(50, 36, ADCval[5], 5, 12);
        ADC_Cmd(ADC1, ENABLE);
        ADC_SoftwareStartConv(ADC1);
    }
}

void DMA2_Stream0_IRQHandler(){
    OS_ERR err;
    OSIntEnter();
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) == SET){
        OSSemPost(&CAL_sem, OS_OPT_POST_ALL, &err);
        ADC_Cmd(ADC1, DISABLE);
    }
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
    OSIntExit();
}

void EXTI4_IRQHandler(){
    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line4)){

    }
    EXTI_ClearITPendingBit(EXTI_Line4);
    OSIntExit();
}

void EXTI5_IRQHandler(){
    OSIntEnter(); 
    if(EXTI_GetITStatus(EXTI_Line5)){

    }
    EXTI_ClearITPendingBit(EXTI_Line5);
    OSIntExit();
}

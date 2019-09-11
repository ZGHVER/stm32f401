#include"app.h"
#include"./SCALES/SCALES.h"
#include"./OLED/oled.h"

OS_ERR err;
extern volatile uint16_t ADCval[100];
OS_SEM sems;

#define CAL_TSize 128
#define CAL_PRIO 11
OS_TCB CAL_TCB;
CPU_STK CAL_STK[CAL_TSize];
void SCALES_CAL_Task(void* p_args);

void SCALES_Start(){
    SCALES_Init();

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
    osError_hander(err);

    OSSemCreate(&sems, "ok", 0, &err);
    
    osError_hander(err);
}


void SCALES_CAL_Task(void* p_args){    
    while(1){
        OSSemPend(&sems, 0, OS_OPT_PEND_BLOCKING, (CPU_TS*)0, &err);
        osError_hander(err);
        OLED_ShowNum(0, 12, ADCval[0], 5, 12);
        OLED_ShowNum(50, 12, ADCval[0], 5, 12);
        ADC_Cmd(ADC1, ENABLE);
        ADC_SoftwareStartConv(ADC1);
    }
}

void DMA2_Stream0_IRQHandler(){
    OSIntEnter();
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) == SET){
        ADC_Cmd(ADC1, DISABLE);
        OSSemPost(&sems, OS_OPT_POST_ALL, &err);
        osError_hander(err);
    }
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
    OSIntExit();
}

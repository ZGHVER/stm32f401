#include"app.h"
#include"./STEPM/STEPM.h"

#define MOTA_TSize 64
#define MOTA_PRIO  12
OS_TCB  MOTA_TCB;
CPU_STK MOTA_STK[MOTA_TSize];
void MOTA_Task(void* p_args);
OS_SEM MOTA_S;

#define MOTB_TSize 64
#define MOTB_PRIO  12
OS_TCB  MOTB_TCB;
CPU_STK MOTB_STK[MOTB_TSize];
void MOTB_Task(void* p_args);
OS_SEM MOTB_S;

extern __IO uint16_t tim3_counte;
extern __IO uint16_t tim3_t;
extern __IO uint16_t tim4_counte;
extern __IO uint16_t tim4_t;

OS_ERR err;
void STEPM_Start(void){
    STEPM_Init();
    OSTaskCreate(
        (  OS_TCB*   )&MOTA_TCB,                                  //任务控制块
        ( CPU_CHAR*  )"MOTA_Task",                                //任务名
        (OS_TASK_PTR )MOTA_Task,                                  //任务函数指针
        (   void*    )0,                                          //首次运行时传递的参数
        (  OS_PRIO   )MOTA_PRIO,                                  //任务优先级
        (  CPU_STK*  )&MOTA_STK[0],                               //任务堆栈基地址
        (CPU_STK_SIZE)MOTA_TSize / 10,                            //可用最大堆栈空间
        (CPU_STK_SIZE)MOTA_TSize,                                 //任务堆栈大小
        ( OS_MSG_QTY )10,                                         //任务可接收的最大消息数
        (  OS_TICK   )0,                                          //在任务之间循环时的时间片的时间量（以刻度表示）指定0以使用默认值
        (   void*    )0,                                          //TCB扩展指针
        (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //包含有关任务行为的其他信息（或选项）
        (  OS_ERR*   )&err                                        //错误变量
    );
    osError_hander(err);
    
    OSTaskCreate(
        (  OS_TCB*   )&MOTB_TCB,                                  //任务控制块
        ( CPU_CHAR*  )"MOTB_Task",                                //任务名
        (OS_TASK_PTR )MOTB_Task,                                  //任务函数指针
        (   void*    )0,                                          //首次运行时传递的参数
        (  OS_PRIO   )MOTB_PRIO,                                  //任务优先级
        (  CPU_STK*  )&MOTB_STK[0],                               //任务堆栈基地址
        (CPU_STK_SIZE)MOTB_TSize / 10,                            //可用最大堆栈空间
        (CPU_STK_SIZE)MOTB_TSize,                                 //任务堆栈大小
        ( OS_MSG_QTY )10,                                         //任务可接收的最大消息数
        (  OS_TICK   )0,                                          //在任务之间循环时的时间片的时间量（以刻度表示）指定0以使用默认值
        (   void*    )0,                                          //TCB扩展指针
        (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //包含有关任务行为的其他信息（或选项）
        (  OS_ERR*   )&err                                        //错误变量
    );
    osError_hander(err);

    OSSemCreate(&MOTA_S, "MOTA_S", 0, &err);
    OSSemCreate(&MOTB_S, "MOTB_S", 0, &err);
    osError_hander(err);

    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

void MOTA_Task(void* p_args){
    while(1){
        OSSemPend(&MOTA_S, 0, OS_OPT_PEND_BLOCKING, (CPU_TS*)0, &err);
        tim3_counte ++;
        if(tim3_counte == tim3_t){
            TIM_Cmd(TIM3, DISABLE);
            tim3_counte = 0;
        }
    }
}

void MOTB_Task(void* p_args){
    while(1){
        OSSemPend(&MOTB_S, 0, OS_OPT_PEND_BLOCKING, (CPU_TS*)0, &err);
        osError_hander(err);
        tim4_counte ++;
        if(tim4_counte == tim4_t){
            TIM_Cmd(TIM4, DISABLE);
            tim4_counte = 0;
        }
    }
}

void TIM3_IRQHandler(){
    OSIntEnter();
    if(TIM_GetITStatus(TIM3, TIM_IT_Update)){
       OSSemPost(&MOTA_S, OS_OPT_POST_ALL, &err);
    }
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    OSIntExit();
}

void TIM4_IRQHandler(){
    OSIntEnter();
    if(TIM_GetITStatus(TIM4, TIM_IT_Update)){
        OSSemPost(&MOTB_S, OS_OPT_POST_ALL, &err);
    }
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    OSIntExit();
}



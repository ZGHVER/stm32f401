#include "app.h"
#include"./BSP/OLED/oled.h"

#define REFRE_TSize 64
#define REFRE_PRIO 10
CPU_STK REFRE_STK[REFRE_TSize];
OS_TCB REFRE_TCB;
void OLED_REFRE(void* p_args);

void OLED_Start(){
    OS_ERR err;

    OLED_Init();

    OSTaskCreate(
        (  OS_TCB*   )&REFRE_TCB,                                 //任务控制块
        ( CPU_CHAR*  )"OLED_REFRE",                               //任务名
        (OS_TASK_PTR )OLED_REFRE,                                 //任务函数指针
        (   void*    )0,                                          //首次运行时传递的参数
        (  OS_PRIO   )REFRE_PRIO,                                 //任务优先级
        (  CPU_STK*  )&REFRE_STK[0],                              //任务堆栈基地址
        (CPU_STK_SIZE)REFRE_TSize / 10,                           //可用最大堆栈空间
        (CPU_STK_SIZE)REFRE_TSize,                                //任务堆栈大小
        ( OS_MSG_QTY )10,                                         //任务可接收的最大消息数
        (  OS_TICK   )0,                                          //在任务之间循环时的时间片的时间量（以刻度表示）指定0以使用默认值
        (   void*    )0,                                          //TCB扩展指针
        (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //包含有关任务行为的其他信息（或选项）
        (  OS_ERR*   )&err                                        //错误变量
    );
    osError_hander(err);

    osError_hander(err);
}

void OLED_REFRE(void* p_args){
    OS_ERR err;
    while(1){
        OLED_Refresh_Gram();
        OSTimeDly(250, OS_OPT_TIME_DLY, &err);
        osError_hander(err);
    }
}




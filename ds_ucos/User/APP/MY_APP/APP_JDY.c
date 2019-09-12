#include"os.h"
#include"./JDY/JDY.h"
#include"app.h"

OS_Q JDY_SEND_Q;

#define SEND_TSize 64
#define SEND_PRIO  13
OS_TCB  SEND_TCB;
CPU_STK SEND_STK[SEND_TSize];
void JDY_SEND_Task(void* p_args);

    OS_ERR JDY_err;

void JDY_Start(){
    JDY_Init(); 

    OSTaskCreate(
        (  OS_TCB*   )&SEND_TCB,                                  //任务控制块
        ( CPU_CHAR*  )"SEND_TASK_JDY",                            //任务名
        (OS_TASK_PTR )JDY_SEND_Task,                              //任务函数指针
        (   void*    )0,                                          //首次运行时传递的参数
        (  OS_PRIO   )SEND_PRIO,                                  //任务优先级
        (  CPU_STK*  )&SEND_STK[0],                               //任务堆栈基地址
        (CPU_STK_SIZE)SEND_TSize / 10,                            //可用最大堆栈空间
        (CPU_STK_SIZE)SEND_TSize,                                 //任务堆栈大小
        ( OS_MSG_QTY )10,                                         //任务可接收的最大消息数
        (  OS_TICK   )0,                                          //在任务之间循环时的时间片的时间量（以刻度表示）指定0以使用默认值
        (   void*    )0,                                          //TCB扩展指针
        (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //包含有关任务行为的其他信息（或选项）
        (  OS_ERR*   )&JDY_err                                        //错误变量
    );

    OSQCreate(&JDY_SEND_Q, "JDY_SEND_Q", 20, &JDY_err);
    osError_hander(JDY_err);
}

void JDY_SendData(uint8_t* Strings, uint8_t size){
    OSQPost(&JDY_SEND_Q, (void*)Strings, (OS_MSG_SIZE)size, OS_OPT_POST_FIFO | OS_OPT_POST_ALL, &JDY_err);
    osError_hander(JDY_err);
}

void JDY_SEND_Task(void* p_args){
    uint8_t size;
    uint8_t* strings;
    while(1){
        strings = (uint8_t*)OSQPend(
            &JDY_SEND_Q, 
            (OS_TICK)0, 
            OS_OPT_PEND_BLOCKING, 
            (OS_MSG_SIZE*)&size, 
            (CPU_TS*)0, 
            &JDY_err);
        osError_hander(JDY_err);
        for(uint8_t i = 0; i < size; i++){
            USART_SendData(JDY_UART, *strings++);
            while(USART_GetFlagStatus(JDY_UART, USART_FLAG_TC) != SET);
        }
    }

}
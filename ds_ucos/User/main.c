
  
#include "stm32f4xx.h"
#include "bsp.h"
#include "os.h"

#define init_TSIZE 128
CPU_STK init_task_STK[init_TSIZE];
OS_TCB TCB_init_task;
void init_task(void* args);

#define LED_TSIZE 128
CPU_STK LED_task_STK[LED_TSIZE];
OS_TCB TCB_LED_task;
void LED_task(void* args);



int main(void)
{
  OS_ERR P;
  CPU_Init();
  BSP_Tick_Init();
  OSInit(&P);
  OSTaskCreate(
      (  OS_TCB*   )&TCB_init_task,                                   //任务控制块
      ( CPU_CHAR*  )"inittask",                                  //任务名
      (OS_TASK_PTR )init_task,                                     //任务函数指针
      (   void*    )0,                                          //首次运行时传递的参数
      (  OS_PRIO   )10,                                         //任务优先级
      (  CPU_STK*  )&init_task_STK[0],                                  //任务堆栈基地址
      (CPU_STK_SIZE)init_TSIZE / 10,                            //可用最大堆栈空间
      (CPU_STK_SIZE)init_TSIZE,                                 //任务堆栈大小
      ( OS_MSG_QTY )10,                                         //任务可接收的最大消息数
      (  OS_TICK   )0,                                          //在任务之间循环时的时间片的时间量（以刻度表示）指定0以使用默认值
      (   void*    )0,                                          //TCB扩展指针
      (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //包含有关任务行为的其他信息（或选项）
      (  OS_ERR*   )&P                                      //错误变量
  );
			P = P;
  OSStart(&P);
}

void init_task(void* args){
  OS_ERR P;
  //BSP_Init();
  GPIO_InitTypeDef ioC;
  ioC.GPIO_Pin = GPIO_Pin_13;
  ioC.GPIO_Mode = GPIO_Mode_OUT;
  ioC.GPIO_OType = GPIO_OType_PP;
  ioC.GPIO_PuPd = GPIO_PuPd_UP;
  ioC.GPIO_Speed = GPIO_Speed_50MHz;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  GPIO_Init(GPIOC, &ioC);

  OSTaskCreate(
      (  OS_TCB*   )&TCB_LED_task,                                   //任务控制块
      ( CPU_CHAR*  )"task_name",                                  //任务名
      (OS_TASK_PTR )LED_task,                                        //任务函数指针
      (   void*    )0,                                          //首次运行时传递的参数
      (  OS_PRIO   )10,                                         //任务优先级
      (  CPU_STK*  )&LED_task_STK[0],                                  //任务堆栈基地址
      (CPU_STK_SIZE)LED_TSIZE / 10,                            //可用最大堆栈空间
      (CPU_STK_SIZE)LED_TSIZE,                                 //任务堆栈大小
      ( OS_MSG_QTY )5,                                         //任务可接收的最大消息数
      (  OS_TICK   )0,                                          //在任务之间循环时的时间片的时间量（以刻度表示）指定0以使用默认值
      (   void*    )0,                                          //TCB扩展指针
      (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //包含有关任务行为的其他信息（或选项）
      (  OS_ERR*   )&P                                      //错误变量
  );

  OSTaskDel(0, &P);
}

void LED_task(void* args){
  OS_ERR P;
  while(1){
    OSTimeDly(1000, OS_OPT_TIME_DLY, &P);
    GPIOC->ODR ^= GPIO_Pin_13;
  }
}





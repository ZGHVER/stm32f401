/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ���ڽӷ����ԣ����ڽ��յ����ݺ����ϻش���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "bsp.h"
#include "os.h"
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
	void init_task(void* args);
	OS_ERR P = OS_ERR_NONE;
int main(void)
{
  
  CPU_SR_ALLOC();
  CPU_Init();
  OSInit(&P);

  OS_TCB init_tasktcb;
  int stk_size = 128;
  CPU_STK stk[stk_size];
  OSTaskCreate(
      (  OS_TCB*   )&init_tasktcb,      //任务控制块
      ( CPU_CHAR*  )"task_name",    //任务名
      (OS_TASK_PTR )init_task,      //任务函数指针
      (   void*    )0,           //首次运行时传递的参数
      (  OS_PRIO   )10,          //任务优先级
      (  CPU_STK*  )&stk[0],          //任务堆栈基地址
      (CPU_STK_SIZE)stk_size / 10,     //可用最大堆栈空间
      (CPU_STK_SIZE)stk_size,     //任务堆栈大小
      ( OS_MSG_QTY )0,       //任务可接收的最大消息数
      (  OS_TICK   )0,          //在任务之间循环时的时间片的时间量（以刻度表示）指定0以使用默认值
      (   void*    )0,            //TCB扩展指针
      (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, //包含有关任务行为的其他信息（或选项）
      (  OS_ERR*   )P          //错误变量
  );

  OSStart(&P);
}

void init_task(void* args){
	while(1);
}





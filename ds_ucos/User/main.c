
  
#include "stm32f4xx.h"
#include "bsp.h"
#include "os.h"
#include "app.h"
#include "./OLED/oled.h"

#define init_TSIZE 128
CPU_STK init_task_STK[init_TSIZE];
OS_TCB TCB_init_task;
void init_task(void* args);

#define LED_TSIZE 128
#define LED_task_PRIO 15
CPU_STK LED_task_STK[LED_TSIZE];
OS_TCB TCB_LED_task;
void LED_task(void* args);

int main(void){
  OS_ERR P;
  CPU_Init();
  BSP_Tick_Init();

  OSInit(&P);
  osError_hander(P);
  
  OSTaskCreate(
      (  OS_TCB*   )&TCB_init_task,                             //������ƿ�
      ( CPU_CHAR*  )"inittask",                                 //������
      (OS_TASK_PTR )init_task,                                  //������ָ��
      (   void*    )0,                                          //�״�����ʱ���ݵĲ���
      (  OS_PRIO   )10,                                         //�������ȼ�
      (  CPU_STK*  )&init_task_STK[0],                          //�����ջ����ַ
      (CPU_STK_SIZE)init_TSIZE / 10,                            //��������ջ�ռ�
      (CPU_STK_SIZE)init_TSIZE,                                 //�����ջ��С
      ( OS_MSG_QTY )10,                                         //����ɽ��յ������Ϣ��
      (  OS_TICK   )0,                                          //������֮��ѭ��ʱ��ʱ��Ƭ��ʱ�������Կ̶ȱ�ʾ��ָ��0��ʹ��Ĭ��ֵ
      (   void*    )0,                                          //TCB��չָ��
      (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //�����й�������Ϊ��������Ϣ����ѡ�
      (  OS_ERR*   )&P                                          //�������
  );
  osError_hander(P);

  OSStart(&P);
  osError_hander(P);

}

__STATIC_INLINE void LED_Start(){
   OSTaskCreate(
      (  OS_TCB*   )&TCB_LED_task,                              //������ƿ�
      ( CPU_CHAR*  )"task_name",                                //������
      (OS_TASK_PTR )LED_task,                                   //������ָ��
      (   void*    )0,                                          //�״�����ʱ���ݵĲ���
      (  OS_PRIO   )LED_task_PRIO,                              //�������ȼ�
      (  CPU_STK*  )&LED_task_STK[0],                           //�����ջ����ַ
      (CPU_STK_SIZE)LED_TSIZE / 10,                             //��������ջ�ռ�
      (CPU_STK_SIZE)LED_TSIZE,                                  //�����ջ��С
      ( OS_MSG_QTY )5,                                          //����ɽ��յ������Ϣ��
      (  OS_TICK   )0,                                          //������֮��ѭ��ʱ��ʱ��Ƭ��ʱ�������Կ̶ȱ�ʾ��ָ��0��ʹ��Ĭ��ֵ
      (   void*    )0,                                          //TCB��չָ��
      (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //�����й�������Ϊ��������Ϣ����ѡ�
      (  OS_ERR*   )&P                                          //�������
  );
  osError_hander(P);
}

void init_task(void* args){
  OS_ERR P;
    
  GPIO_InitTypeDef ioC;
  ioC.GPIO_Pin = GPIO_Pin_13;
  ioC.GPIO_Mode = GPIO_Mode_OUT;
  ioC.GPIO_OType = GPIO_OType_PP;
  ioC.GPIO_PuPd = GPIO_PuPd_UP;
  ioC.GPIO_Speed = GPIO_Speed_50MHz;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  GPIO_Init(GPIOC, &ioC);

  OSSchedLock(&P);
  osError_hander(P);              //��ʼ����������������

  OLED_Start();
  SCALES_Start();
  LED_Start();
  ADC_SoftwareStartConv(ADC1);
 
  OSSchedUnlock(&P);              //��ʼ������������������
  osError_hander(P);

  OSTaskDel(0, &P);
  osError_hander(P);
}

void LED_task(void* args){
  OS_ERR err;
  uint16_t i = 0;

  while(1){
    OSTimeDly(250, OS_OPT_TIME_DLY, &err);
    osError_hander(err);
    GPIOC->ODR ^= GPIO_Pin_13;
    OLED_ShowNum(0, 0, i++, 12, 12);
  }  
}





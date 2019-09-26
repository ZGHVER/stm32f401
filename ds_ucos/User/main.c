
  
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
	 OS_ERR P;;
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

  __disable_irq();//��ʼ��ʼ�� ���ж�

  OLED_Start();
  SCALES_Start();
  LED_Start();
  STEPM_Start();
  ADC_SoftwareStartConv(ADC1);
 
  __enable_irq();//������ʼ�� ���ж�

  OSTaskDel(0, &P);
  osError_hander(P);

  GPIO_InitTypeDef ioB;
  ioB.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
  ioB.GPIO_Mode = GPIO_Mode_AF;
  ioB.GPIO_OType = GPIO_OType_PP;
  ioB.GPIO_PuPd = GPIO_PuPd_UP;
  ioB.GPIO_Speed = GPIO_Speed_2MHz;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_Init(GPIOB, &ioB);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);


  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  I2C_InitTypeDef i2c1;
  i2c1.I2C_Mode = I2C_Mode_I2C;
  i2c1.I2C_Ack = I2C_Ack_Enable;
  i2c1.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  i2c1.I2C_ClockSpeed = 200000;
  i2c1.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c1.I2C_OwnAddress1 = 0x0A;
  I2C_Init(I2C1, &i2c1);
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  I2C_Cmd(I2C1, ENABLE);

  I2C_SendData();
  I2C_GetFlagStatus();
  I2C_FLAG_BTF
}

void LED_task(void* args){
  OS_ERR err;
  uint16_t i = 0;

  while(1){

    OSTimeDly(100, OS_OPT_TIME_DLY, &err);
    osError_hander(err);
    GPIOC->ODR ^= GPIO_Pin_13;
    OLED_ShowNum(0, 0, i++, 12, 12);
  }  
}





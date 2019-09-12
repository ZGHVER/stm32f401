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
        (  OS_TCB*   )&SEND_TCB,                                  //������ƿ�
        ( CPU_CHAR*  )"SEND_TASK_JDY",                            //������
        (OS_TASK_PTR )JDY_SEND_Task,                              //������ָ��
        (   void*    )0,                                          //�״�����ʱ���ݵĲ���
        (  OS_PRIO   )SEND_PRIO,                                  //�������ȼ�
        (  CPU_STK*  )&SEND_STK[0],                               //�����ջ����ַ
        (CPU_STK_SIZE)SEND_TSize / 10,                            //��������ջ�ռ�
        (CPU_STK_SIZE)SEND_TSize,                                 //�����ջ��С
        ( OS_MSG_QTY )10,                                         //����ɽ��յ������Ϣ��
        (  OS_TICK   )0,                                          //������֮��ѭ��ʱ��ʱ��Ƭ��ʱ�������Կ̶ȱ�ʾ��ָ��0��ʹ��Ĭ��ֵ
        (   void*    )0,                                          //TCB��չָ��
        (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //�����й�������Ϊ��������Ϣ����ѡ�
        (  OS_ERR*   )&JDY_err                                        //�������
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
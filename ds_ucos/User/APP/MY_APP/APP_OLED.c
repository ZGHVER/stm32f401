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
        (  OS_TCB*   )&REFRE_TCB,                                 //������ƿ�
        ( CPU_CHAR*  )"OLED_REFRE",                               //������
        (OS_TASK_PTR )OLED_REFRE,                                 //������ָ��
        (   void*    )0,                                          //�״�����ʱ���ݵĲ���
        (  OS_PRIO   )REFRE_PRIO,                                 //�������ȼ�
        (  CPU_STK*  )&REFRE_STK[0],                              //�����ջ����ַ
        (CPU_STK_SIZE)REFRE_TSize / 10,                           //��������ջ�ռ�
        (CPU_STK_SIZE)REFRE_TSize,                                //�����ջ��С
        ( OS_MSG_QTY )10,                                         //����ɽ��յ������Ϣ��
        (  OS_TICK   )0,                                          //������֮��ѭ��ʱ��ʱ��Ƭ��ʱ�������Կ̶ȱ�ʾ��ָ��0��ʹ��Ĭ��ֵ
        (   void*    )0,                                          //TCB��չָ��
        (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //�����й�������Ϊ��������Ϣ����ѡ�
        (  OS_ERR*   )&err                                        //�������
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




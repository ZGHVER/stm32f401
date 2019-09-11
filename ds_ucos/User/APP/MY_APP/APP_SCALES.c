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
        (  OS_TCB*   )&CAL_TCB,                                   //������ƿ�
        ( CPU_CHAR*  )"SCALES_CAL_Task",                          //������
        (OS_TASK_PTR )SCALES_CAL_Task,                            //������ָ��
        (   void*    )0,                                          //�״�����ʱ���ݵĲ���
        (  OS_PRIO   )CAL_PRIO,                                   //�������ȼ�
        (  CPU_STK*  )&CAL_STK[0],                                //�����ջ����ַ
        (CPU_STK_SIZE)CAL_TSize / 10,                             //��������ջ�ռ�
        (CPU_STK_SIZE)CAL_TSize,                                  //�����ջ��С
        ( OS_MSG_QTY )5,                                          //����ɽ��յ������Ϣ��
        (  OS_TICK   )0,                                          //������֮��ѭ��ʱ��ʱ��Ƭ��ʱ�������Կ̶ȱ�ʾ��ָ��0��ʹ��Ĭ��ֵ
        (   void*    )0,                                          //TCB��չָ��
        (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //�����й�������Ϊ��������Ϣ����ѡ�
        (  OS_ERR*   )&err                                        //�������
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

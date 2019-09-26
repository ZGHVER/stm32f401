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
        (  OS_TCB*   )&MOTA_TCB,                                  //������ƿ�
        ( CPU_CHAR*  )"MOTA_Task",                                //������
        (OS_TASK_PTR )MOTA_Task,                                  //������ָ��
        (   void*    )0,                                          //�״�����ʱ���ݵĲ���
        (  OS_PRIO   )MOTA_PRIO,                                  //�������ȼ�
        (  CPU_STK*  )&MOTA_STK[0],                               //�����ջ����ַ
        (CPU_STK_SIZE)MOTA_TSize / 10,                            //��������ջ�ռ�
        (CPU_STK_SIZE)MOTA_TSize,                                 //�����ջ��С
        ( OS_MSG_QTY )10,                                         //����ɽ��յ������Ϣ��
        (  OS_TICK   )0,                                          //������֮��ѭ��ʱ��ʱ��Ƭ��ʱ�������Կ̶ȱ�ʾ��ָ��0��ʹ��Ĭ��ֵ
        (   void*    )0,                                          //TCB��չָ��
        (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //�����й�������Ϊ��������Ϣ����ѡ�
        (  OS_ERR*   )&err                                        //�������
    );
    osError_hander(err);
    
    OSTaskCreate(
        (  OS_TCB*   )&MOTB_TCB,                                  //������ƿ�
        ( CPU_CHAR*  )"MOTB_Task",                                //������
        (OS_TASK_PTR )MOTB_Task,                                  //������ָ��
        (   void*    )0,                                          //�״�����ʱ���ݵĲ���
        (  OS_PRIO   )MOTB_PRIO,                                  //�������ȼ�
        (  CPU_STK*  )&MOTB_STK[0],                               //�����ջ����ַ
        (CPU_STK_SIZE)MOTB_TSize / 10,                            //��������ջ�ռ�
        (CPU_STK_SIZE)MOTB_TSize,                                 //�����ջ��С
        ( OS_MSG_QTY )10,                                         //����ɽ��յ������Ϣ��
        (  OS_TICK   )0,                                          //������֮��ѭ��ʱ��ʱ��Ƭ��ʱ�������Կ̶ȱ�ʾ��ָ��0��ʹ��Ĭ��ֵ
        (   void*    )0,                                          //TCB��չָ��
        (  OS_OPT    )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,  //�����й�������Ϊ��������Ϣ����ѡ�
        (  OS_ERR*   )&err                                        //�������
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



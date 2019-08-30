#include"MOTc.h"
static uint16_t PID_SEQ;
static uint32_t EC_MAXVAL;
static q15_t PWM_maxVAL;
arm_pid_instance_q15 PIDS_L;
arm_pid_instance_q15 PIDS_R;
volatile q15_t targetSPEED_L = 0;
volatile q15_t targetSPEED_R = 0;

void MOTC_init(uint32_t EcodiingMAXLAL, 
                q15_t PWM_preLoad, 
                uint16_t PWMPSC, 
                q15_t* pidL, 
                q15_t* pidR,
                uint16_t PIDControl_sequence){
    PID_SEQ = PIDControl_sequence;
    PWM_maxVAL = PWM_preLoad;
    EC_MAXVAL = EcodiingMAXLAL;
    MOTC_IO_init();
    MOTC_EcodingTimer_init(EcodiingMAXLAL);
    MOTC_PWMCon_init(PWM_preLoad, PWMPSC);
   MOTC_PID_init(pidL, pidR);
   
    TIM_Cmd(TIM10, ENABLE);
}

void MOTC_SETDIR(MOT mot, DIR dir){
    if(mot == MOTR){
        switch(dir){
            case POSI: 
            GPIOA->ODR |= MOTR_CA;
            GPIOA->ODR &= ~MOTR_CB;             
            break;
            case NEGA:
            GPIOA->ODR &= ~MOTR_CA;
            GPIOA->ODR |= MOTR_CB;  
            
                  
            break;
            case STOP: 
            GPIOA->ODR &= ~MOTR_CA;
            GPIOA->ODR &= ~MOTR_CB;
            break;
            default :break;
        }
    }else if(mot == MOTL){
        switch(dir){
            case POSI: 
            GPIOA->ODR |= MOTL_CA;
            GPIOA->ODR &= ~MOTL_CB;
            break;
            case NEGA:
            GPIOA->ODR &= ~MOTL_CA;
            GPIOA->ODR |= MOTL_CB;
            break;
            case STOP: 
            GPIOA->ODR &= ~MOTL_CA;
            GPIOA->ODR &= ~MOTL_CB;
            break;
            default :break;
	    }
    }
}

static void MOTC_IO_init(){
    GPIO_InitTypeDef ioInitType;                            //定时器1，编码器IO初始化  
    ioInitType.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    ioInitType.GPIO_Mode = GPIO_Mode_AF;
    ioInitType.GPIO_OType = GPIO_OType_PP;
    ioInitType.GPIO_PuPd = GPIO_PuPd_DOWN;
    ioInitType.GPIO_Speed = GPIO_Speed_100MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &ioInitType);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);

    ioInitType.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;          //定时器2，编码器IO初始化
    GPIO_Init(GPIOA, &ioInitType);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);  

    ioInitType.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
    ioInitType.GPIO_OType = GPIO_OType_PP;
    ioInitType.GPIO_PuPd = GPIO_PuPd_UP;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_Init(GPIOB, &ioInitType);                          //定时器3，PWM控制IO初始化
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);

    ioInitType.GPIO_Pin = MOTL_CA | MOTL_CB | MOTR_CA | MOTR_CB;    //方向控制IO初始化
    ioInitType.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &ioInitType);    

    
}

static void MOTC_EcodingTimer_init(uint32_t countermax){
    EC_MAXVAL = countermax;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_TimeBaseInitTypeDef Tim_ECO_Init;
    Tim_ECO_Init.TIM_ClockDivision = TIM_CKD_DIV1;
    Tim_ECO_Init.TIM_CounterMode = TIM_CounterMode_Up;      //计数器模式
    Tim_ECO_Init.TIM_Period = countermax - 1;               //重装载值
    Tim_ECO_Init.TIM_Prescaler = 0;                         //分频系数
    TIM_TimeBaseInit(TIM1, &Tim_ECO_Init);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseInit(TIM2, &Tim_ECO_Init);

    TIM_EncoderInterfaceConfig(TIM1, 
                                TIM_EncoderMode_TI12, 
                                TIM_ICPolarity_Rising, 
                                TIM_ICPolarity_Rising 
                                );

    TIM_EncoderInterfaceConfig(TIM2, 
                                TIM_EncoderMode_TI12, 
                                TIM_ICPolarity_Rising, 
                                TIM_ICPolarity_Rising
                                );
    TIM_ICInitTypeDef icint;
    TIM_ICStructInit(&icint);
    icint.TIM_ICFilter = 20;
    TIM_ICInit(TIM1, &icint);
    TIM_ICInit(TIM2, &icint);

    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

static void MOTC_PWMCon_init(uint32_t preLoad, uint16_t PSC){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef tim3;
    tim3.TIM_ClockDivision = TIM_CKD_DIV1;
    tim3.TIM_CounterMode = TIM_CounterMode_Down;      //计数器模式
    tim3.TIM_Period = 4000 - 1;                   //重装载值
    tim3.TIM_Prescaler =  84 - 1;                   //分频系数
    TIM_TimeBaseInit(TIM3, &tim3);

    TIM_OCInitTypeDef TIM3_oc3_Init;                            //定时器3频道3PWM初始化
    TIM3_oc3_Init.TIM_OCMode = TIM_OCMode_PWM1;                 //PWM模式选择
    TIM3_oc3_Init.TIM_OCPolarity = TIM_OCPolarity_High;         //PWM有效电平设置
    TIM3_oc3_Init.TIM_OutputState = TIM_OutputState_Enable;     //输出比较状态
    TIM_OC3Init(TIM3, &TIM3_oc3_Init);
    TIM_OC4Init(TIM3, &TIM3_oc3_Init);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_SetCompare3(TIM3, 1000);
    TIM_SetCompare4(TIM3, 1000);
    TIM_Cmd(TIM3, ENABLE);
}

static void MOTC_PID_init(q15_t* pidL, q15_t* pidR){
    MOTC_PIDHard_init();
    arm_pid_reset_q15(&PIDS_L);
    arm_pid_reset_q15(&PIDS_R);
    PIDS_L.Kp = pidL[0];
    PIDS_L.Ki = pidL[1];
    PIDS_L.Kd = pidL[2];
    PIDS_R.Kp = pidR[0];
    PIDS_R.Ki = pidR[1];    
    PIDS_R.Kd = pidR[2];
    arm_pid_init_q15(&PIDS_L, 1);
    arm_pid_init_q15(&PIDS_R, 1);
}

static void MOTC_PIDHard_init(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
    TIM_TimeBaseInitTypeDef tim10_BaseInit;
    tim10_BaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
    tim10_BaseInit.TIM_CounterMode = TIM_CounterMode_Up;                       //计数器模式
    tim10_BaseInit.TIM_Period = 1000000 / PID_SEQ - 1;                         //重装载值
    tim10_BaseInit.TIM_Prescaler = 83;                                         //分频系数
    TIM_TimeBaseInit(TIM10, &tim10_BaseInit); 

    TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);
    NVIC_InitTypeDef TIM2_NvicInit;
    TIM2_NvicInit.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;                       //中断频道
    TIM2_NvicInit.NVIC_IRQChannelCmd = ENABLE;
    TIM2_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //抢占优先级，主优先级
    TIM2_NvicInit.NVIC_IRQChannelSubPriority = 1;            //提交优先级，次优先级
    NVIC_Init(&TIM2_NvicInit);
}
char s[10];
static void PIDC_MOTL(){
    q15_t curSPEED = MOTC_GetSpeed(MOTL);

    q15_t out = arm_pid_q15(&PIDS_L, targetSPEED_L - curSPEED);
    sprintf(s, "%d", out);
    OLED_ShowString(0,20,s,12);

    if(out > PWM_maxVAL)
        out = PWM_maxVAL;
    else if(out < -PWM_maxVAL)
        out = -PWM_maxVAL;
    if(out < 0){
        out = -out;
        MOTC_SETDIR(MOTL,NEGA );
    }
    else 
        MOTC_SETDIR(MOTL,POSI );

    TIM_SetCompare3(TIM3, out);
}

static void PIDC_MOTR(){
    q15_t curSPEED = MOTC_GetSpeed(MOTR);
  //  OLED_ADDval(curSPEED / 8);
   // OLED_Refresh_Gram();
	q15_t out = arm_pid_q15(&PIDS_R,targetSPEED_R - curSPEED);
    sprintf(s, "%d", out);
    OLED_ShowString(0,20,s,12);

    if(out > PWM_maxVAL)
        out = PWM_maxVAL;
    else if(out < -PWM_maxVAL)
        out = -PWM_maxVAL;
    if(out < 0){
        out = -out;
        MOTC_SETDIR(MOTR, NEGA);
    }else{
        MOTC_SETDIR(MOTR, POSI );
    }
    TIM_SetCompare4(TIM3, out);
}

void MOTC_SetSpeed(MOT mot,q15_t speed){
    if(mot == MOTL){
        if(speed < 0)
            MOTC_SETDIR(MOTL, NEGA);
        else if(speed > 0)
            MOTC_SETDIR(MOTL, POSI);
        else 
            MOTC_SETDIR(MOTL, STOP);
        targetSPEED_L = speed;
    }else if(mot == MOTR){
         if(speed < 0)
            MOTC_SETDIR(MOTR, NEGA);
        else if(speed > 0)
            MOTC_SETDIR(MOTR, POSI);
        else 
            MOTC_SETDIR(MOTL, STOP);
        targetSPEED_R = speed;
    } 
}

q15_t MOTC_GetSpeed(MOT mot){
    static u32 lastCNT_L = 0;
    static u32 lastCNT_R = 0;
    q15_t speed = 0;
    if(mot == MOTL){
       speed  =  CAL_speed(TIM1, lastCNT_L);
        lastCNT_L = TIM1->CNT;
    }else if(mot == MOTR){
        speed =  CAL_speed(TIM2, lastCNT_R);
        lastCNT_R = TIM2->CNT;
    }
    return speed;
}

static q15_t CAL_speed(TIM_TypeDef* TIMS, u32 lastCNT){
    q15_t sig = 1;
    u32 CNT = TIMS->CNT;

    float32_t detVAL = 0;
    if ((TIMS->CR1 >> 4 & 0x01) == 0){ 
        sig = -1;
        if (CNT < lastCNT)
            detVAL = EC_MAXVAL - lastCNT + CNT;
        else
            detVAL = CNT - lastCNT;
    }
    else if ((TIMS->CR1 >> 4 & 0x01) == 1){
        if (CNT > lastCNT)
            detVAL = EC_MAXVAL - CNT + lastCNT;
        else
            detVAL = lastCNT - CNT;
    }
    detVAL = (detVAL / PLUS_PRE_ROW) * PID_SEQ;
    detVAL *= sig;
    return (q15_t)detVAL;
}

void TIM1_UP_TIM10_IRQHandler(){
    if(TIM_GetITStatus(TIM10, TIM_IT_Update) == SET){
     //   PIDC_MOTL();
       // PIDC_MOTR();
        TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
    }else if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET){
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }

   
}

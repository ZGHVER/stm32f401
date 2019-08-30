#include"ECG.h"
#include"delay.h"
#include"Servo.h"

float MAXPower = 1.9;
uint8_t ischarging = 0;
uint32_t chargingTIM = 2000;
uint32_t targetTIM = 6000;

__STATIC_INLINE void ECG_ADCInit(){

  	GPIO_InitTypeDef ioC;
	ioC.GPIO_Pin = GPIO_Pin_13;
	ioC.GPIO_Mode = GPIO_Mode_OUT;
	ioC.GPIO_OType = GPIO_OType_PP;
	ioC.GPIO_PuPd = GPIO_PuPd_UP;
	ioC.GPIO_Speed = GPIO_Speed_50MHz;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_Init(GPIOC, &ioC);


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    GPIO_InitTypeDef ioA;
    ioA.GPIO_Pin = GPIO_Pin_0;
    ioA.GPIO_Mode = GPIO_Mode_AIN;
    ioA.GPIO_PuPd = GPIO_PuPd_NOPULL;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &ioA);


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);
    ADC_CommonInitTypeDef adcc;
    adcc.ADC_Mode = ADC_Mode_Independent;
    adcc.ADC_Prescaler = ADC_Prescaler_Div2;
    adcc.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    adcc.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;
    ADC_CommonInit(&adcc);


    ADC_InitTypeDef adci;
    ADC_StructInit(&adci);
    adci.ADC_Resolution = ADC_Resolution_12b;
    adci.ADC_ScanConvMode = DISABLE;
    adci.ADC_ContinuousConvMode = ENABLE;
    adci.ADC_DataAlign = ADC_DataAlign_Right;
    adci.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &adci);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_56Cycles);
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

    NVIC_InitTypeDef ADC1_NvicInit;
    ADC1_NvicInit.NVIC_IRQChannel = ADC_IRQn;                       //中断频道
    ADC1_NvicInit.NVIC_IRQChannelCmd = ENABLE;
    ADC1_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //抢占优先级，主优先级
    ADC1_NvicInit.NVIC_IRQChannelSubPriority = 0;            //提交优先级，次优先级
    NVIC_Init(&ADC1_NvicInit);
    
    ADC_Cmd(ADC1, ENABLE);
}

__STATIC_INLINE void TIM_init(){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef tim3;
    tim3.TIM_ClockDivision = TIM_CKD_DIV1;
    tim3.TIM_CounterMode = TIM_CounterMode_Up;      //计数器模式
    tim3.TIM_Period = 1000 - 1;                         //重装载值
    tim3.TIM_Prescaler = 84 - 1;                      //分频系数
    TIM_TimeBaseInit(TIM3, &tim3);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);



    NVIC_InitTypeDef tim3_NvicInit;
    tim3_NvicInit.NVIC_IRQChannel = TIM3_IRQn;                       //中断频道
    tim3_NvicInit.NVIC_IRQChannelCmd = ENABLE;
    tim3_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //抢占优先级，主优先级
    tim3_NvicInit.NVIC_IRQChannelSubPriority = 0;            //提交优先级，次优先级
    NVIC_Init(&tim3_NvicInit);
    
}

__STATIC_INLINE void ECG_ControlInit(){
    GPIO_InitTypeDef ioA;
    ioA.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    ioA.GPIO_Mode = GPIO_Mode_OUT;
    ioA.GPIO_OType = GPIO_OType_PP;
    ioA.GPIO_PuPd = GPIO_PuPd_UP;
    ioA.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &ioA);
}

void ECG_Init(){
    ECG_ControlInit();
    //ECG_ADCInit();
    TIM_init();
}

void ECG_Fire(){
    GPIOA->ODR |= GPIO_Pin_5;
    delay_ms(2000);
    GPIOA->ODR &= ~GPIO_Pin_5;
}

void ECG_SetAngle(uint8_t angle){
    Servo_SetAngle(servo2, angle);
}

void ECG_SetLength(uint16_t length){

}

void ECG_setCharValue(uint32_t maxvalue){
    targetTIM = maxvalue;
}

uint8_t ECG_IsCharging(){
    return ischarging;
}

void ECG_Charge(){
    ischarging = 1;
    chargingTIM = targetTIM;
    delay_us(200);
    TIM_Cmd(TIM3, ENABLE);
    GPIOA->ODR |= GPIO_Pin_4;
}

void TIM3_IRQHandler(){
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
	
        chargingTIM--;
        if(chargingTIM == 0){
            ischarging = 0;
            GPIOA->ODR &= ~GPIO_Pin_4;
        }
    }
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

}

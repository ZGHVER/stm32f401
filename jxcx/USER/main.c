
#include "oled.h"
#include "delay.h"
#include "JDY.h"
#include "STEM.h"
#include "PinDef.h"
#include "SCALES.h"
#include "rtc.h"

#define SENSOR_FEED     PAin(6)
#define SENSOR_WATER    PAin(7)
#define WATER_PUMP      PAout(4)

__STATIC_INLINE void ALL_init(void);
void SENSOR_init(void);

extern volatile uint16_t ADCval[100];
int main(){ 
    ALL_init();
    
    GPIO_InitTypeDef ioC;
    ioC.GPIO_Pin = GPIO_Pin_13;
    ioC.GPIO_Mode = GPIO_Mode_OUT;
    ioC.GPIO_OType = GPIO_OType_PP;
    ioC.GPIO_PuPd = GPIO_PuPd_UP;
    ioC.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_Init(GPIOC, &ioC);
    ADC_SoftwareStartConv(ADC1);
    uint8_t tim[7];
    uint8_t s[40];
    int i = 0;
    while(1){
     /*   OLED_Clear();
        OLED_ShowNum(0,0,ADCval[i++],12,12);
        if(i == 100)
            i = 0;
        RTCm_getTim(tim);
        sprintf(s,"%d,%d,%d,%d,%d,%d,%d",tim[0], tim[1], tim[2], tim[3], tim[4], tim[5], tim[6]);
        OLED_ShowString(0, 12, s, 12);
        OLED_Refresh_Gram();
        delay_ms(200);*/
        TIM_Cmd(TIM3, ENABLE);
        delay_ms(1000);
        delay_ms(1000);
        delay_ms(1000);
        delay_ms(1000);
        delay_ms(1000);
        i++;
        OLED_ShowNum(0,0,i,4,24);
        OLED_Refresh_Gram();
        if(i == 100)break;
    }
    while(1);
}

__STATIC_INLINE void ALL_init(){
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    delay_init(84);
    OLED_Init();
    //JDY_Init();
    STEPM_Init();
   // SENSOR_init();
    SCALES_Init();
    RTCm_Init();

    //TIM_Cmd(TIM3, ENABLE);
   // TIM_Cmd(TIM4, ENABLE);

    if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 12){
        RTC_TimeTypeDef tim;
        tim.RTC_Hours = 20;
        tim.RTC_Minutes = 49;
        tim.RTC_Seconds = 0;
        RTC_DateTypeDef date;
        date.RTC_Year = 19;
        date.RTC_Month = 4;
        date.RTC_Date = 18;
        date.RTC_WeekDay = 5;
        RTCm_SetTIM(&tim, &date, 12);
    }
}

void SENSOR_init(){
    
} 
/*
void ADC_IRQHandler(){
    GPIOC->ODR ^= GPIO_Pin_13;
    ADCval[0] = ADC_GetConversionValue(ADC1);
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}
*/

void DMA2_Stream0_IRQHandler(){
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0)){
        ADC_Cmd(ADC1, DISABLE);
    }
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
}

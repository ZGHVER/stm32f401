
#include "oled.h"
#include "delay.h"
#include "JDY.h"
#include "STEM.h"
#include "PinDef.h"
#include "SCALES.h"

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

    while(1){
        OLED_ShowNum(0,0,ADCval[1],12,12);
        OLED_ShowNum(0,12,ADCval[2],12,12);
        OLED_ShowNum(0,24,ADCval[3],12,12);
        OLED_Refresh_Gram();
    }
}

__STATIC_INLINE void ALL_init(){
    delay_init(84);
    OLED_Init();
    //JDY_Init();
   // STEPM_Init();
   // SENSOR_init();
    SCALES_Init();
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
      //  DMA_Cmd(DMA2_Stream0, DISABLE);
        ADC_Cmd(ADC1, DISABLE);
    }
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
}

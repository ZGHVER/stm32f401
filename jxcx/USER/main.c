
#include"stm32f4xx_gpio.h"
#include"stm32f4xx_conf.h"
#include"oled.h"
#include"stm32f4xx_flash.h"
#include"delay.h"
#include"JDY.h"

void ALL_init();
void ADC_init();
int main(){ 
    ALL_init();
    OLED_Clear();

    while(1){
         
    }
}

void ALL_init(){
    delay_init(84);
    OLED_Init();
    ADC_init();
    STEP_Init();
}

#include"stm32f4xx_gpio.h"
#include"stm32f4xx_conf.h"
#include"oled.h"
#include"delay.h"

void ALL_init();

int main(){ 
    ALL_init();
    while(1){
    OLED_ShowString(0, 0, (uint8_t*)"asdasd", 12);
    OLED_Refresh_Gram();
    }
    
}

void ALL_init(){
    delay_init(84);
    OLED_Init();

}

void ADC_init(){

}

void RTC_Init_(){
}
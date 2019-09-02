
#include"stm32f4xx_gpio.h"
#include"stm32f4xx_conf.h"
#include"oled.h"
#include"stm32f4xx_flash.h"
#include"delay.h"
#include"JDY.h"

void ALL_init();
int main(){ 
    ALL_init();
    OLED_Clear();


    GPIO_InitTypeDef ioC;
    ioC.GPIO_Pin = GPIO_Pin_13;
    ioC.GPIO_Mode = GPIO_Mode_OUT;
    ioC.GPIO_OType = GPIO_OType_PP;
    ioC.GPIO_PuPd = GPIO_PuPd_UP;
    ioC.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_Init(GPIOC, &ioC);
    delay_ms(1000);
    JDY_Set_BoundRate(JDY_BoundRate_19200);
    while(1){
        OLED_Refresh_Gram();
    }
}

void ALL_init(){
    delay_init(84);
    OLED_Init();
    JDY_Init();
}
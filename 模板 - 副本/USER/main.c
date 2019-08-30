
#include"stm32f4xx_gpio.h"
#include"delay.h"
#include"arm_math.h"
int main()
{ 
	delay_init(84);
	
	GPIO_InitTypeDef ioC;
	ioC.GPIO_Pin = GPIO_Pin_All;
	ioC.GPIO_Mode = GPIO_Mode_OUT;
	ioC.GPIO_OType = GPIO_OType_PP;
	ioC.GPIO_PuPd = GPIO_PuPd_UP;
	ioC.GPIO_Speed = GPIO_Speed_2MHz;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_Init(GPIOC, &ioC);
	

		while(1){
			arm_sin_q15(1233);
			GPIOC->ODR ^= GPIO_Pin_13;
			delay_ms(1000);
		};
}


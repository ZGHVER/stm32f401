
#include"stm32f4xx_gpio.h"
#include"delay.h"
#include"arm_math.h"
#include"serial_key.h"
#include"usart.h"
#define A 			GPIO_Pin_4					
#define _A			GPIO_Pin_5		
#define B			GPIO_Pin_6		
#define _B			GPIO_Pin_7		
uint16_t T_eight_Path[8] = {
	_B, B, A, _A, B,_B, _A, A,
};
void IOInit();
void TIMinit();
void key1(){
}

void key2(){
	 
}
int main()
{ 	static u8 counter = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(84);
	SE_KeyMap_init();
	IOInit();
	TIMinit();
	SE_KeyMap_setFun(key1, 1);
	SE_KeyMap_setFun(key2, 2);
	GPIOA->ODR = 0;
	GPIOA->ODR |= A;
	delay_us(800);
	GPIOA->ODR = 0;
	GPIOA->ODR |= GPIO_Pin_4;
	GPIOA->ODR |= GPIO_Pin_7;
	u16 delays = 1000;
	u8 k = 4;
		GPIOA->ODR = 0;
		GPIOA->ODR |= A;
	
	while(1){
	/* 	GPIOA->ODR ^=T_eight_Path[k++];
		delay_us(delays);
		if(k == 8)
			k = 0;*/
		GPIOA->ODR &= ~_B;
		GPIOA->ODR |= B;
		delay_us(delays);
		GPIOA->ODR &= ~A;
		GPIOA->ODR |= _A;
		delay_us(delays);
		GPIOA->ODR &= ~B;
		GPIOA->ODR |= _B;
		delay_us(delays);
		GPIOA->ODR &= ~_A;
		GPIOA->ODR |= A;
		delay_us(delays);

	}
}

void IOInit(){
	GPIO_InitTypeDef ioA;
	ioA.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	ioA.GPIO_Mode = GPIO_Mode_OUT;
	ioA.GPIO_OType = GPIO_OType_PP;
	ioA.GPIO_PuPd = GPIO_PuPd_UP;
	ioA.GPIO_Speed = GPIO_Speed_50MHz;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_Init(GPIOA, &ioA);
}

void TIMinit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	TIM_TimeBaseInitTypeDef tim9_BaseInit;
	tim9_BaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
	tim9_BaseInit.TIM_CounterMode = TIM_CounterMode_Up;      //计数器模�?
	tim9_BaseInit.TIM_Period = 400 - 1;                         //重装载�?
	tim9_BaseInit.TIM_Prescaler = 83;                   //分频系数
	TIM_TimeBaseInit(TIM9, &tim9_BaseInit);
	TIM_ARRPreloadConfig(TIM9, ENABLE);

	TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
	NVIC_InitTypeDef TIM9_NvicInit;
	TIM9_NvicInit.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;                       //中断频道
	TIM9_NvicInit.NVIC_IRQChannelCmd = ENABLE;
	TIM9_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //抢占优先级，主优先级
	TIM9_NvicInit.NVIC_IRQChannelSubPriority = 0;            //提交优先级，次优先级
	NVIC_Init(&TIM9_NvicInit);
	
	TIM_Cmd(TIM9, ENABLE);
}

void TIM1_BRK_TIM9_IRQHandler(){
	static u8 counter = 0;
	if(TIM_GetITStatus(TIM9, TIM_IT_Update) != SET)
		return;
	//GPIOA->ODR ^= T_eight_Path[counter++];
	if(counter == 8)
		counter = 0;	
	TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
}


#include"stm32f4xx_gpio.h"
#include"delay.h"
#include"arm_math.h"
#include"MOTc.h"
#include"oled.h"


#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include"dmpKey.h"
#include"dmpmap.h"
#include"mpu6050.h"
#include"myiic.h"

q15_t pidc[3], PIDR[3];
void timinit();

int main()
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  	short x, y, z;
	delay_init(84);


	GPIO_InitTypeDef ioC;
	ioC.GPIO_Pin = GPIO_Pin_13;
	ioC.GPIO_Mode = GPIO_Mode_OUT;
	ioC.GPIO_OType = GPIO_OType_PP;
	ioC.GPIO_PuPd = GPIO_PuPd_UP;
	ioC.GPIO_Speed = GPIO_Speed_50MHz;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_Init(GPIOC, &ioC);

	GPIO_InitTypeDef ioA;
	ioA.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	ioA.GPIO_Mode = GPIO_Mode_AF;
	ioA.GPIO_OType = GPIO_OType_PP;
	ioA.GPIO_PuPd = GPIO_PuPd_UP;
	ioA.GPIO_Speed = GPIO_Speed_50MHz;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_Init(GPIOA, &ioA);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM2);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef tim2;
	tim2.TIM_ClockDivision = TIM_CKD_DIV1;
	tim2.TIM_CounterMode = TIM_CounterMode_Up;      //计数器模式
	tim2.TIM_Period = 2000 - 1;                    //重装载值
	tim2.TIM_Prescaler = 84 - 1;                   //分频系数
	TIM_TimeBaseInit(TIM2, &tim2);

	TIM_OCInitTypeDef TIM2_oc_Init;                            //定时器2频道PWM初始化
	TIM2_oc_Init.TIM_OCMode = TIM_OCMode_PWM1;                 //PWM模式选择
	TIM2_oc_Init.TIM_OCPolarity = TIM_OCPolarity_High;         //PWM有效电平设置
	TIM2_oc_Init.TIM_OutputState = TIM_OutputState_Enable;     //输出比较状态
	TIM_OC1Init(TIM2, &TIM2_oc_Init);
	TIM_OC2Init(TIM2, &TIM2_oc_Init);
	TIM_OC3Init(TIM2, &TIM2_oc_Init);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);

	TIM_SetCompare1(TIM2, 500);
	TIM_SetCompare2(TIM2, 1000);
	TIM_SetCompare3(TIM2, 1500);
	


	TIM_Cmd(TIM2, ENABLE);

/* 

	pidc[0] = 300;
	pidc[1] = 3000;
	pidc[2] = 1000;

	PIDR[0] = 300;
	PIDR[1] = 3000;
	PIDR[2] = 1000;
	MOTC_init(2000, 2000, 42, pidc, PIDR, 500);
	MOTC_SetSpeed(MOTL,500);
	MOTC_SetSpeed(MOTR,500);

	q15_t out;*/
	while(1){
		GPIOC->ODR ^= GPIO_Pin_13;
		delay_ms(1000);
	}
}

void timinit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
	TIM_TimeBaseInitTypeDef tim11_BaseInit;
	tim11_BaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
	tim11_BaseInit.TIM_CounterMode = TIM_CounterMode_Up;      //计数器模式
	tim11_BaseInit.TIM_Period = 1000000 / 100 - 1;            //重装载值
	tim11_BaseInit.TIM_Prescaler = 84 - 1;                   //分频系数
	TIM_TimeBaseInit(TIM11, &tim11_BaseInit);

	TIM_ITConfig(TIM11, TIM_IT_Update, ENABLE);
	NVIC_InitTypeDef tim11_NvicInit;
	tim11_NvicInit.NVIC_IRQChannel = TIM1_TRG_COM_TIM11_IRQn;                       //中断频道
	tim11_NvicInit.NVIC_IRQChannelCmd = ENABLE;
	tim11_NvicInit.NVIC_IRQChannelPreemptionPriority = 1;     //抢占优先级，主优先级
	tim11_NvicInit.NVIC_IRQChannelSubPriority = 0;            //提交优先级，次优先级
	NVIC_Init(&tim11_NvicInit);

	TIM_Cmd(TIM11 ,ENABLE);



	


	
}
void TIM1_TRG_COM_TIM11_IRQHandler(){
	static float32_t Kp = 175;    // 0-200
	static float32_t Kd = 0;
	char s[20];
	float32_t pitch ,roll, yaw; 		//欧拉角
		static float32_t lastERR = 0;
		mpu_dmp_get_data(&pitch, &roll, &yaw);
		pitch += 0;
		q15_t out = Kp * pitch + Kd * ( pitch - lastERR );
		lastERR = pitch;
		if(out > 2000)
			out = 2000;
		else if(out < -2000)	
			out = -2000;
		sprintf(s, "%4.3f, %d", pitch, out);
		OLED_ShowString(0,0,s,12);
		if(out < 0){
			out = -out;
			MOTC_SETDIR(MOTL, NEGA);
			MOTC_SETDIR(MOTR, NEGA);
		}else{
			MOTC_SETDIR(MOTL, POSI);
			MOTC_SETDIR(MOTR, POSI);
		}
		TIM_SetCompare3(TIM3, out);
		TIM_SetCompare4(TIM3, out);
		TIM_ClearITPendingBit(TIM11, TIM_IT_Update);
}




#include"Servo.h"
#include"arm_math.h"

uint16_t curAngle1 = 0;
uint16_t curAngle2 = 0;
static void Servo_PinInit(){
    GPIO_InitTypeDef ioA;
    ioA.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_1;
    ioA.GPIO_Mode = GPIO_Mode_AF;
    ioA.GPIO_OType = GPIO_OType_PP;
    ioA.GPIO_PuPd = GPIO_PuPd_UP;
    ioA.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &ioA);
    
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_TIM2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
}

static void Servo_TimInit(){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //50Hz
    TIM_TimeBaseInitTypeDef tim2;
    tim2.TIM_ClockDivision = TIM_CKD_DIV1;
    tim2.TIM_CounterMode = TIM_CounterMode_Up;      //计数器模式
    tim2.TIM_Period = 20000 - 1;                         //重装载值
    tim2.TIM_Prescaler = 84 - 1;                   //分频系数
    TIM_TimeBaseInit(TIM2, &tim2);

    TIM_OCInitTypeDef TIM2_oc1_Init;                            //定时器2频道1PWM初始化
    TIM2_oc1_Init.TIM_OCMode = TIM_OCMode_PWM1;                 //PWM模式选择
    TIM2_oc1_Init.TIM_OCPolarity = TIM_OCPolarity_High;         //PWM有效电平设置
    TIM2_oc1_Init.TIM_OutputState = TIM_OutputState_Enable;     //输出比较状态
    TIM_OC1Init(TIM2, &TIM2_oc1_Init);
    TIM_OC2Init(TIM2, &TIM2_oc1_Init);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_SetCompare1(TIM2, 1500);
    TIM_SetCompare2(TIM2, 500);   
}

void Servo_init(){
    Servo_PinInit();
    Servo_TimInit();
    TIM_Cmd(TIM2, ENABLE);
}

__STATIC_INLINE uint32_t Servo_getValue(uint16_t angle){
    return 500 + (1800 * angle / 180.0);
}

uint16_t Servo_getCurAngle(Servo servo){
    if(servo == servo1)
        return curAngle1;
    else if(servo == servo2)
        return curAngle2;
    else
        return 0; 
}

void Servo_SetAngle(Servo servo, uint16_t angle){
    if(servo == servo1) {
        TIM_SetCompare1(TIM2, Servo_getValue(angle)); 
        curAngle1 = angle;
    }else if(servo == servo2){
        TIM_SetCompare2(TIM2, Servo_getValue(angle));
        curAngle2 = angle;
    }
}

uint16_t Servo_SeachTarget(){

    uint16_t posidata[4];
	uint8_t	iss = 0;
    uint8_t TrueTimes = 0;
    int err;

    arm_pid_instance_q15 PIDS;
	arm_pid_reset_q15(&PIDS);
	PIDS.Kp = 6000;
	PIDS.Ki = 4000;
	PIDS.Kd = 0;
	arm_pid_init_q15(&PIDS, 1);
	while(1){
		iss = CAMERA_ResData(posidata);
		if(iss == 1){

            GPIOC->ODR ^= GPIO_Pin_13;
            err = 160;
            err -= posidata[2];
            if(err < 10)
                TrueTimes ++;
            if(TrueTimes == 10)
                return (posidata[0] + posidata[1]) / 2;
			q15_t out = arm_pid_q15(&PIDS, err);
			if(out > 180)
				out = 180;
			else if(out < 0)
				out = 0;
			Servo_SetAngle(servo1, out);
            GPIOC->ODR ^= GPIO_Pin_13;

        }		
		while(CAMERA_ResData(posidata) == 0){
			if(Servo_getCurAngle(servo1) == 180){
				Servo_SetAngle(servo1, 0);
				delay_ms(800);
			}else
				Servo_SetAngle(servo1, Servo_getCurAngle(servo1) + 20);
			delay_ms(200);
		}
	    
    }
}
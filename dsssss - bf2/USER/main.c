#include"main.h"
#include"stm32f4xx_conf.h"
#include"oled.h"
#include"Servo.h"
#include"ECG.h"
#include"serial_key.h"
#include"SE_KeyFun.h"
#include"CRMERA.h"
#include"arm_math.h"

extern volatile uint8_t CurModeCode;
extern volatile uint8_t LengthORAngle;
extern volatile uint8_t Buffer_ptr;
extern volatile uint16_t GEtValue;
extern volatile uint8_t isNewValue;
extern volatile char Key_Buffer[10];


extern volatile uint16_t res;
void ALL_init();
uint8_t SelMode();
void Mode0();
void Mode1();
void Mode2();


int main(){
	ALL_init();
 	CurModeCode = 0;
	Servo_SetAngle(servo1, 90);
	Servo_SetAngle(servo2, 90);
	while(1){
		OLED_Refresh_Gram();
  		switch(CurModeCode){
			case 0: Mode0();break;
			case 1: Mode1();break;
			case 2: Mode2();break;
		}      
	}
}

void ALL_init(){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	GPIO_InitTypeDef ioC;
	ioC.GPIO_Pin = GPIO_Pin_13;
	ioC.GPIO_Mode = GPIO_Mode_OUT;
	ioC.GPIO_OType = GPIO_OType_PP;
	ioC.GPIO_PuPd = GPIO_PuPd_UP;
	ioC.GPIO_Speed = GPIO_Speed_50MHz;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_Init(GPIOC, &ioC);



	delay_init(84);
	OLED_Init();
	Servo_init();
	SE_KeyMap_init();
	CAMERA_Init();
	ECG_Init();
	SE_KeyMap_setFun(key1_Fun, 1);
	SE_KeyMap_setFun(key2_Fun, 2);
	SE_KeyMap_setFun(key3_Fun, 3);
	SE_KeyMap_setFun(key4_Fun, 4);
	SE_KeyMap_setFun(key5_Fun, 5);
	SE_KeyMap_setFun(key6_Fun, 6);
	SE_KeyMap_setFun(key7_Fun, 7);
	SE_KeyMap_setFun(key8_Fun, 8);
	SE_KeyMap_setFun(key9_Fun, 9);
	SE_KeyMap_setFun(key10_Fun, 10);
	SE_KeyMap_setFun(key11_Fun, 11);
	SE_KeyMap_setFun(key12_Fun, 12);
	SE_KeyMap_setFun(key13_Fun, 13);
	SE_KeyMap_setFun(key14_Fun, 14);
	SE_KeyMap_setFun(key15_Fun, 15);
	SE_KeyMap_setFun(key16_Fun, 16);
}

const char showstring[5][20] = {
		"Charging",
		"Charging .",
		"Charging ..",
		"Charging ...",
		"Charging ...."
	};
char buffer[10];
const char mode1String[] = {"curMode : MODE1"};
void Mode0(){
	OLED_Clear();
	char ts[20];
	char temang[20], temleng[20];
	static uint16_t curLength = 0;
	static uint8_t curAngle = 0;
	OLED_ShowString(20, 0, mode1String, 12);
	sprintf(temang, "%s%dSEC","curAngle:", curAngle);
	OLED_ShowString(0, 30, temang, 12);

	while(isNewValue == 0){
		if(CurModeCode != 0)
			return;
		OLED_Clear();
		OLED_ShowString(20, 0, mode1String, 12);
		OLED_ShowString(0, 30, temang, 12);

		for(int i = 0; i < Buffer_ptr; i++){
			buffer[i] = Key_Buffer[i] + 48;
		}
		
		sprintf(ts, "%s%scm","curLength:", buffer);

		for(int i = 0; i < 10; i++)
			buffer[i] = 0;

		OLED_ShowString(0, 15, ts, 12);
		OLED_ShowString(120, 15, "*", 12);
		OLED_Refresh_Gram();
	}

	curLength = GEtValue;
	//Servo_SetAngle(servo1, curLength);
	ECG_setCharValue(curLength);
	isNewValue = 0;
	sprintf(temleng, "%s%dcm", "curLength:", curLength);

	while(isNewValue == 0){
		if(CurModeCode != 0)
			return;
		OLED_Clear();
		OLED_ShowString(20, 0, mode1String, 12);
		OLED_ShowString(0, 15, temleng, 12);

		for(int i = 0; i < Buffer_ptr; i++){
			buffer[i] = Key_Buffer[i] + 48;
		}
		
		sprintf(ts, "%s%sSEC","curAngle:", buffer);

		for(int i = 0; i < 10; i++)
			buffer[i] = 0;

		OLED_ShowString(0, 30, ts, 12);
		OLED_ShowString(120, 30, "*", 12);
		OLED_Refresh_Gram();
	}
	
	for(int i = 0; i < 10; i++)
			buffer[i] = 0;

	curAngle = GEtValue;
	ECG_SetAngle((uint8_t)curAngle);
	isNewValue = 0;
	delay_ms(500);
	int ks = 0;
	ECG_Charge();
	while(ECG_IsCharging()){
		OLED_Clear();
		OLED_ShowString(20, 0, mode1String, 12);
		OLED_ShowString(0, 24, showstring[ks++], 16);
		ks = ks > 4 ? 0 : ks;
		OLED_Refresh_Gram();
		delay_ms(500);
	}
	OLED_Clear();
	OLED_ShowString(28, 20, "Fire!!", 24);
	OLED_Refresh_Gram();
	ECG_Fire();
	delay_ms(1000);
//	ECG_Charge();
}

const char mode2String[] = {"curMode : MODE2"};
void Mode1(){
		uint16_t Length = 0;
		OLED_Clear();
		OLED_ShowString(20,0,mode2String,12 );
		OLED_ShowString(0, 15,"waiting...", 12);
		OLED_Refresh_Gram();
		while(isNewValue == 0){
			if(CurModeCode != 1)
				return;
			delay_ms(100);
		} 
		isNewValue = 0;
		OLED_Clear();
		OLED_ShowString(20, 0, mode2String, 12);
		OLED_ShowString(0, 15, "searching ...", 12);
		OLED_Refresh_Gram();
		GPIOC->ODR ^= GPIO_Pin_13;
		Length = Servo_SeachTarget();
		GPIOC->ODR ^= GPIO_Pin_13;
		ECG_SetLength(Length);
		uint8_t ks = 0;
		while(ECG_IsCharging()){
			OLED_Clear();
			OLED_ShowString(20, 0, mode1String, 12);
			OLED_ShowString(0, 24, showstring[ks++], 16);
			ks = ks > 4 ? 0 : ks;
			OLED_Refresh_Gram();
			delay_ms(500);
		}
		OLED_Clear();
		OLED_ShowString(28, 20, "Fire!!", 24);
		OLED_Refresh_Gram();
		ECG_Fire();
		delay_ms(1000);
		ECG_Charge();
		return;
}


void timIinit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_TimeBaseInitTypeDef tim1_BaseInit;
	tim1_BaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
	tim1_BaseInit.TIM_CounterMode = TIM_CounterMode_Up;      //计数器模式
	tim1_BaseInit.TIM_Period = 20000;                         //重装载值
	tim1_BaseInit.TIM_Prescaler = 8400 - 1;                   //分频系数
	TIM_TimeBaseInit(TIM1, &tim1_BaseInit);

	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	NVIC_InitTypeDef tim1_NvicInit;
	tim1_NvicInit.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;                       //中断频道
	tim1_NvicInit.NVIC_IRQChannelCmd = ENABLE;
	tim1_NvicInit.NVIC_IRQChannelPreemptionPriority = 1;     //抢占优先级，主优先级
	tim1_NvicInit.NVIC_IRQChannelSubPriority = 0;            //提交优先级，次优先级
	NVIC_Init(&tim1_NvicInit);
	TIM_Cmd(TIM1, ENABLE);
}

volatile isArrived = 0;
const char mode3String[] = {"curMode : MODE3"};
void Mode2(){
	uint16_t resData[4];
	uint16_t datalList[10];
	uint16_t Timlist[10];
	uint8_t ptr = 0;
		OLED_Clear();
		OLED_ShowString(20,0,mode3String,12 );
		OLED_Refresh_Gram();
		Servo_SetAngle(servo1, 90);
		Servo_SetAngle(servo2, 60);

		ECG_SetLength(250);
		ECG_SetAngle(0);

		delay_ms(1000);

		Servo_SetAngle(servo1, 120);
	while(isArrived == 0){
		if(CAMERA_ResData(&resData) == 1 && ptr < 10){
			int sr = 160 - resData[2];
			if(sr < 30){
				datalList[ptr] = resData[2];
				Timlist[ptr++] = TIM1->CNT;
			}
		}
	}
	Servo_SetAngle(servo1, 60);
	isArrived = 0;
	while(isArrived == 0){

	}

}

void TIM1_UP_TIM10_IRQHandler(){
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET);
}

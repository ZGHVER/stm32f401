
#include"stm32f4xx_gpio.h"
#include"delay.h"
#include"oled.h"
#include"oled1.h"
#include"arm_common_tables.h"
#include"stm32f4xx_rng.h"

#define TS_LENGTH 128

typedef void(*WAVE_FUN)(u8, u8, u8, float32_t*, uint16_t); 
void All_Init();
void OLED_show(float32_t* ser, uint16_t length, uint16_t A, uint16_t B);
void OLED1_show(float32_t* ser, uint16_t length);
void RFFT_Fun_f32(float32_t* input, float32_t* output);
void Cos_wave(u8 A, u8 w, u8 v, float32_t* out, uint16_t length);
void Sqrt_wave(u8 A, u8 w, u8 v, float32_t* out, uint16_t length);
void Tria_wave(u8 A, u8 w, u8 v, float32_t* out, uint16_t length);
void Rand_wave(u8 A, u8 w, u8 v, float32_t* out, uint16_t length);
void ADC_wave(u8 A, u8 w, u8 v, float32_t* out, uint16_t length);

float32_t Input_f32[TS_LENGTH * 2] = { 0 };
float32_t OutPut_f32[TS_LENGTH] = { 0 };
q15_t twiddleCoef_128_q15_s[192];

q15_t Input_q15[TS_LENGTH * 2] = { 0 };
q15_t OutPut_q15[TS_LENGTH] = { 0 };

const arm_cfft_instance_f32 arm_cfft_sR_f32_len1024 = {
  TS_LENGTH, twiddleCoef_128, armBitRevIndexTable128, ARMBITREVINDEXTABLE_128_TABLE_LENGTH
};
arm_cfft_instance_q15 cfft_q15;
const char wave_name[5][5] = {
"cos", "sqrt", "tria", "rand", "ADC",
};
const WAVE_FUN FUN_list[5] = {
  Cos_wave, Sqrt_wave, Tria_wave, Rand_wave, ADC_wave,
};
volatile u8 curWaveIndex = 0;
volatile uint16_t Cur_W = 20, Cur_v = 50;

int main()
{ 
	All_Init();
	while(1){
  uint16_t A = Cur_W;
  uint16_t B = Cur_v;
  WAVE_FUN fun = FUN_list[curWaveIndex];
   if(curWaveIndex != 4){
      ADC_Cmd(ADC1, DISABLE);
      OLED1_Clear();
      OLED_Clear();
      fun(10, A, B, Input_f32, TS_LENGTH << 1);
      OLED1_show(Input_f32, TS_LENGTH << 1);
      RFFT_Fun_f32(Input_f32, OutPut_f32);
      OLED_show(OutPut_f32, TS_LENGTH, A, B);
   }else{
      ADC_Cmd(ADC1, ENABLE);
      fun(10,A,B,Input_f32, 128);
   }
	}
}

void EXTI_init(){
   GPIO_InitTypeDef ioA;
  ioA.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
  ioA.GPIO_Mode = GPIO_Mode_IN;
  ioA.GPIO_OType = GPIO_OType_PP;
  ioA.GPIO_PuPd = GPIO_PuPd_UP;
  ioA.GPIO_Speed = GPIO_Speed_50MHz;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_Init(GPIOA, &ioA);

  RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFGEN, ENABLE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource9);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource10);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource11);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource12);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
  EXTI_InitTypeDef IOAPIN9;
  IOAPIN9.EXTI_Line =  EXTI_Line9 | EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line15;
  IOAPIN9.EXTI_LineCmd = ENABLE;
  IOAPIN9.EXTI_Mode = EXTI_Mode_Interrupt;
  IOAPIN9.EXTI_Trigger = EXTI_Trigger_Rising;  //触发方式
  EXTI_Init(&IOAPIN9);
  
  NVIC_InitTypeDef EXTI9_5_NvicInit;
  EXTI9_5_NvicInit.NVIC_IRQChannel = EXTI9_5_IRQn;                       //中断频道
  EXTI9_5_NvicInit.NVIC_IRQChannelCmd = ENABLE;
  EXTI9_5_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //抢占优先级，主优先级
  EXTI9_5_NvicInit.NVIC_IRQChannelSubPriority = 0;            //提交优先级，次优先级
  NVIC_Init(&EXTI9_5_NvicInit);

  NVIC_InitTypeDef EXTI15_10_NvicInit;
  EXTI15_10_NvicInit.NVIC_IRQChannel = EXTI15_10_IRQn;                       //中断频道
  EXTI15_10_NvicInit.NVIC_IRQChannelCmd = ENABLE;
  EXTI15_10_NvicInit.NVIC_IRQChannelPreemptionPriority = 2;     //抢占优先级，主优先级
  EXTI15_10_NvicInit.NVIC_IRQChannelSubPriority = 1;            //提交优先级，次优先级
  NVIC_Init(&EXTI15_10_NvicInit);

  GPIO_InitTypeDef ioC;
  ioC.GPIO_Pin = GPIO_Pin_13;
  ioC.GPIO_Mode = GPIO_Mode_OUT;
  ioC.GPIO_OType = GPIO_OType_PP;
  ioC.GPIO_PuPd = GPIO_PuPd_UP;
  ioC.GPIO_Speed = GPIO_Speed_50MHz;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  GPIO_Init(GPIOC, &ioC);
}

void ADC_init(){
  GPIO_InitTypeDef ioA;
  ioA.GPIO_Pin = GPIO_Pin_7;
  ioA.GPIO_Mode = GPIO_Mode_AN;
  ioA.GPIO_PuPd = GPIO_PuPd_NOPULL;
  ioA.GPIO_Speed = GPIO_Speed_50MHz;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_Init(GPIOA, &ioA);


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_CommonInitTypeDef cominit;
	cominit.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	cominit.ADC_Mode = ADC_Mode_Independent;
	cominit.ADC_Prescaler = ADC_Prescaler_Div2;
	cominit.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
	ADC_CommonInit(&cominit);

  ADC_InitTypeDef adc1type;
	ADC_StructInit(&adc1type);
	adc1type.ADC_Resolution = ADC_Resolution_8b;
	adc1type.ADC_ScanConvMode = DISABLE;
	adc1type.ADC_ContinuousConvMode = ENABLE;
	adc1type.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	adc1type.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	adc1type.ADC_DataAlign = ADC_DataAlign_Right;
	adc1type.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &adc1type);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_56Cycles);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

  NVIC_InitTypeDef ADC_NvicInit;
  ADC_NvicInit.NVIC_IRQChannel = ADC_IRQn;                       //中断频道
  ADC_NvicInit.NVIC_IRQChannelCmd = ENABLE;
  ADC_NvicInit.NVIC_IRQChannelPreemptionPriority = 1;     //抢占优先级，主优先级
  ADC_NvicInit.NVIC_IRQChannelSubPriority = 2;            //提交优先级，次优先级
  NVIC_Init(&ADC_NvicInit);
  
  ADC_Cmd(ADC1, ENABLE);
}

void All_Init(){
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(84);
	OLED_Init();
  ADC_init();
  OLED1_Init();
  EXTI_init();
	OLED1_Clear();
  OLED1_Refresh_Gram();
  OLED_Clear();
  OLED_Refresh_Gram();

  for(int i = 0; i < 192; i ++)
    twiddleCoef_128_q15_s[i] = twiddleCoef_128[i] * 32768;
  cfft_q15.fftLen = TS_LENGTH;
  cfft_q15.pTwiddle = twiddleCoef_128_q15_s;
  cfft_q15.bitRevLength = ARMBITREVINDEXTABLE_128_TABLE_LENGTH;
  cfft_q15.pBitRevTable = armBitRevIndexTable128;
}

void OLED_show(float32_t* ser, uint16_t length, uint16_t A, uint16_t B){
  float32_t maxval;
	uint32_t maxindex;
    char s[30];
    const char* sp;
  sp = wave_name[curWaveIndex];
  sprintf(s, "Name:%s W:%d V:%d", sp, A, B);
  arm_max_f32(ser, TS_LENGTH, &maxval, &maxindex);
  //float32_t par = 63.0f / maxval;
  float32_t par = 63.0f / (maxval / 4);
  for(uint16_t i = 0; i < length; i ++){
    OLED_ADDval((u8)(ser[i] * par));
    OLED_ShowString(0, 0, s, 12);
  }
    OLED_Refresh_Gram();
  
}

void OLED1_show(float32_t* ser, uint16_t length){
  float32_t maxval;
	uint32_t maxindex;
  arm_max_f32(ser, TS_LENGTH * 2, &maxval, &maxindex);
  float32_t par = 63.0f / maxval;
  for(uint16_t i = 0; i < length; i += 2){
    OLED1_ADDval((u8)(ser[i] * par));
  }
    OLED1_Refresh_Gram();
}

void RFFT_Fun_f32(float32_t* input, float32_t* output){

  arm_cfft_f32(&arm_cfft_sR_f32_len1024, input, 0, 1);
  arm_cmplx_mag_f32(input, output, TS_LENGTH);
}

void Cos_wave(u8 A, u8 w, u8 v, float32_t* out, uint16_t length){
  uint16_t fai = 0;
  for (uint16_t i = 0; i < length; i++)
  {
    if(i % 2 == 1){
      out[i] = 0;
      continue;
    }
    fai = w * i + v;
    out[i] = A * arm_cos_f32(((float32_t)fai) * 0.01745329f) + A;
  }
}

void Sqrt_wave(u8 A, u8 w, u8 v, float32_t* out, uint16_t length){
  float32_t f = v/100.0f;
  f *= w;
  u8 temp = 0;
  for (uint16_t i = 0; i < length; i++)
  {
    if(i % 2 == 1){
      out[i] = 0;
      continue;
    }
    if(temp < f){
      out[i] = A;
      temp ++;
    }else{
      out[i] = 0;
      temp ++;
      if(temp == w)
        temp = 0;
    }
  }
}

void Tria_wave(u8 A, u8 w, u8 v, float32_t* out, uint16_t length){
  float pup = w * ( v / 100.0f);
  float detU = A / pup;
  float detD = A / ((float)w - pup);
  u8 dir = 1;
	float pr = 0;
  for(int i = 0; i < length ; i++){
    if(i % 2 == 1){
      out[i] = 0;
      continue;
    }
    if(dir == 1){
      out[i] = pr;
      pr += detU;
      if(pr > A){
        dir = 0;
        pr -= detD; 
      }
    }else {
      out[i] = pr;
      pr -= detD;
      if(pr < 0){
        dir = 1;
        pr += detU;
      }
    }
  }
}

void Rand_wave(u8 A, u8 w, u8 v, float32_t* out, uint16_t length){
  srand(34);
  for(int i = 0; i < length; i ++){
    if(i % 2 == 1){
      out[i] = 0;
      continue;
    }
    out[i] = rand() % 63;
  }
}

void ADC_wave(u8 A, u8 w, u8 v, float32_t* out, uint16_t length){
  ADC_SoftwareStartConv(ADC1);
  OLED_Clear();
  OLED_show(OutPut_f32, 128, 0, 0);
  OLED_Refresh_Gram();
  while(1){
    if(curWaveIndex != 4)
    break;
  }
  ADC_Cmd(ADC1, DISABLE);
}

void key1_ACT(){
  if(curWaveIndex == 4)
    curWaveIndex = 0;
  else 
    curWaveIndex ++;
}

void key2_ACT(){
  Cur_W ++;
}     

void key3_ACT(){
  if(Cur_W == 0)
    return;
  else
    Cur_W --;
}

void key4_ACT(){
  Cur_v += 10;
}

void key5_ACT(){
  if(Cur_v == 0)
    return;
  else
    Cur_v-= 10;
}

void EXTI9_5_IRQHandler(){
  GPIOC->ODR ^= GPIO_Pin_13;
  if(EXTI_GetITStatus(EXTI_Line9)){
    delay_ms(10);
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_PinSource9))
    key1_ACT();
  }
  delay_ms(10);
  EXTI_ClearITPendingBit(EXTI_Line9);
}

void EXTI15_10_IRQHandler(){
  if(EXTI_GetFlagStatus(EXTI_Line10)){
    delay_ms(10);
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_PinSource10))
    key2_ACT();
  }
  else if(EXTI_GetITStatus(EXTI_Line11)){
    delay_ms(10);
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_PinSource11))
    key3_ACT();
  }
  else if(EXTI_GetITStatus(EXTI_Line12)){
    delay_ms(10);
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_PinSource12))
    key4_ACT();
  }
  else if(EXTI_GetITStatus(EXTI_Line15)){
    delay_ms(10);
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_PinSource15))
    key5_ACT();
  }
  delay_ms(10);
  EXTI_ClearITPendingBit(EXTI_Line10);
  EXTI_ClearITPendingBit(EXTI_Line11);
  EXTI_ClearITPendingBit(EXTI_Line12);
  EXTI_ClearITPendingBit(EXTI_Line15);
}

void ADC_IRQHandler(void){
  static u8 a = 0;
	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET){
    uint16_t value = ADC_GetConversionValue(ADC1);
    Input_q15[a] = value * 1.289062f;
    OLED1_ADDval(value / 4);
    OLED1_Refresh_Gram();
    a += 2;
    if(a > TS_LENGTH){
      a = 0;
      arm_cfft_q15(&cfft_q15, Input_q15, 0, 1);
      arm_cmplx_mag_q15(Input_q15, OutPut_q15, TS_LENGTH);
      OLED1_show(OutPut_q15, TS_LENGTH);
    }
	}
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);



}



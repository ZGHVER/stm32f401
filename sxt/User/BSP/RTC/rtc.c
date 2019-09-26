#include"rtc.h"

void RTCm_Init(){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

   RCC_LSICmd(ENABLE);
   while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
   RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();

    RTC_InitTypeDef RTC_InitStructure;
    RTC_InitStructure.RTC_AsynchPrediv = 0x7f;
	RTC_InitStructure.RTC_SynchPrediv = 0xff;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
    RTC_Init(&RTC_InitStructure);
}

void RTCm_SetTIM(RTC_TimeTypeDef* RTC_TimeStructure, 
                RTC_DateTypeDef* RTC_DateStructure, 
                uint32_t TIM_Code){
    if(RTC_TimeStructure != 0){
        RTC_SetTime(RTC_Format_BIN, RTC_TimeStructure);
    }
	if(RTC_DateStructure != 0){
	    RTC_SetDate(RTC_Format_BIN, RTC_DateStructure);
    }
	RTC_WriteBackupRegister(RTC_BKP_DR0, TIM_Code);
}

void RTCm_SetAlarm(uint32_t RTC_Alarm, 
                RTC_AlarmTypeDef* RTC_AlarmStructure,
                ACTION_FUN action){
  
  if(RTC_Alarm == RTC_Alarm_A)
    RTC_WriteBackupRegister(RTC_BKP_DR1, (uint32_t)action);
  else if(RTC_Alarm == RTC_Alarm_B)
    RTC_WriteBackupRegister(RTC_BKP_DR2, (uint32_t)action);
  EXTI_InitTypeDef  EXTI_InitStructure;
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  RTC_AlarmCmd(RTC_Alarm, DISABLE);
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm, RTC_AlarmStructure);
  
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
  
  RTC_AlarmCmd(RTC_Alarm, ENABLE);
  RTC_ClearFlag(RTC_FLAG_ALRAF);
  EXTI_ClearITPendingBit(EXTI_Line17);	
}

void RTCm_getTim(uint8_t* timList){
    RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
    timList[0] = RTC_DateStructure.RTC_Year;
    timList[1] = RTC_DateStructure.RTC_Month;
    timList[2] = RTC_DateStructure.RTC_Date;
    timList[3] = RTC_TimeStructure.RTC_H12;
    timList[4] = RTC_TimeStructure.RTC_Hours;
    timList[5] = RTC_TimeStructure.RTC_Minutes;
    timList[6] = RTC_TimeStructure.RTC_Seconds;
}

void RTC_Alarm_IRQHandler(){
    if(RTC_GetITStatus(RTC_IT_ALRA)){
        ACTION_FUN action =  (ACTION_FUN)RTC_ReadBackupRegister(RTC_BKP_DR1);
        action();
        RTC_ClearITPendingBit(RTC_IT_ALRA);
    }else if(RTC_GetITStatus(RTC_IT_ALRB)){
        ACTION_FUN action =  (ACTION_FUN)RTC_ReadBackupRegister(RTC_BKP_DR2);
        action();
        RTC_ClearITPendingBit(RTC_IT_ALRA);
    }
    EXTI_ClearITPendingBit(EXTI_Line17);
}

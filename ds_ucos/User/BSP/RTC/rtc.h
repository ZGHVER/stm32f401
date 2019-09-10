#ifndef __RTC_H_
#define __RTC_H_

#include"stm32f4xx.h"
typedef void(*ACTION_FUN)(void);
void RTCm_Init(void);
void RTCm_SetTIM(RTC_TimeTypeDef* RTC_TimeStructure, 
                RTC_DateTypeDef* RTC_DateStructure, 
                uint32_t TIM_Code);
void RTCm_SetAlarm(uint32_t RTC_Alarm, 
                RTC_AlarmTypeDef* RTC_AlarmStructure,
                ACTION_FUN action);
void RTCm_getTim(uint8_t* timList);              
#endif

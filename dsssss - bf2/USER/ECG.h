#ifndef __ECG_H_
#define __ECG_H_

#include"stm32f4xx_conf.h"

void ECG_Init();
void ECG_Fire();
void ECG_SetAngle(uint8_t angle);
void ECG_SetLength(uint16_t length);
uint8_t ECG_IsCharging();
void ECG_Charge();
void ECG_setCharValue(uint32_t maxvalue);

#endif
#ifndef __CAMERA_H_
#define __CAMERA_H_

#include"stm32f4xx_conf.h"

void CAMERA_Init();
uint8_t CAMERA_ResData(uint16_t*);
#endif
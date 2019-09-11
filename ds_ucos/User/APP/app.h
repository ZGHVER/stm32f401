#ifndef __APP_H_
#define __APP_H_

#include "os.h"
#include "stm32f4xx.h"

#define osError_hander(err) ((err) == OS_ERR_NONE ? (void)0 : ERR_H((uint8_t *)__FILE__, __LINE__, (OS_ERR)err))

void ERR_H(uint8_t* file, uint32_t line, OS_ERR err);



void OLED_Start(void);
void SCALES_Start(void);

#endif


#ifndef SERIAL_KEY_H_
#define SERIAL_KEY_H_

#include"stm32f4xx_gpio.h"
#include"stm32f4xx_rcc.h"

#define PRESS_GATE  9
typedef void(*ACTION_FUN)(void);

void SE_KeyMap_init(void);
void VOID_FUN(void);
void SE_KeyMap_setFun(ACTION_FUN fun, u8 key_number);

#endif 

#include"stm32f4xx_gpio.h"


#ifndef INIT_H_
#define INIT_H_

#define keyPressed(A) (GPIOD->IDR & (A)) == (A)

    void GPIOA_Init(void);
    void Key_Init(void);

#endif

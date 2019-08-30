#include"init.h"

void GPIOA_Init(void){
    GPIO_InitTypeDef ioA;
    ioA.GPIO_Pin = GPIO_Pin_All;
    ioA.GPIO_Mode = GPIO_Mode_OUT;
    ioA.GPIO_OType = GPIO_OType_PP; 
    ioA.GPIO_PuPd = GPIO_PuPd_UP;
    ioA.GPIO_Speed = GPIO_Speed_2MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &ioA);
}

void Key_Init(void){
    GPIO_InitTypeDef ioD;
    ioD.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_7;
    ioD.GPIO_Mode = GPIO_Mode_IN;
    ioD.GPIO_PuPd = GPIO_PuPd_DOWN;
    ioD.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_Init(GPIOD, &ioD);
}



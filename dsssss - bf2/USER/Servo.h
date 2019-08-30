#ifndef __SERVO_H_
#define __SERVO_H_

#include"stm32f4xx_conf.h"

typedef enum servo{
    servo1,
    servo2,
}Servo;

void Servo_init();
void Servo_SetAngle(Servo, uint16_t);
uint16_t Servo_getCurAngle(Servo servo);
uint16_t Servo_SeachTarget();
#endif
#ifndef __MOTC_H_
#define __MOTC_H_

#include"stm32f4xx.h"
#include"arm_math.h"
#define PLUS_PRE_ROW 13
#define MOTL_CA GPIO_Pin_11
#define MOTL_CB GPIO_Pin_10
#define MOTR_CA GPIO_Pin_6
#define MOTR_CB GPIO_Pin_7
typedef enum mot
{
    MOTL,MOTR,    
}MOT;

typedef enum dir{
    POSI, NEGA, STOP 
}DIR;

static void MOTC_IO_init(void);
static void MOTC_EcodingTimer_init(uint32_t countermax);
static void MOTC_PWMCon_init(uint32_t preLoad, uint16_t PSC);
static void MOTC_PID_init(q15_t* pidL, q15_t* pidR);
static void MOTC_PIDHard_init(void);
 void MOTC_SETDIR(MOT mot, DIR dir);
static void PIDC_MOTL(void);
static void PIDC_MOTR(void);
void MOTC_SetSpeed(MOT mot,q15_t speed);
q15_t MOTC_GetSpeed(MOT mot);
static q15_t CAL_speed(TIM_TypeDef* TIMS, u32 lastCNT);
void MOTC_init(uint32_t EcodiingMAXLAL, 
                q15_t PWM_preLoad, 
                uint16_t PWMPSC, 
                q15_t* pidL, 
                q15_t* pidR,
                uint16_t PIDControl_sequence);
#endif

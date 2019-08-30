#ifndef EXTI_H_
#define EXTI_H_
    #include"stm32f4xx_rcc.h"
    #include"init.h"
    #include"stm32f4xx_syscfg.h"
    #include"stm32f4xx_exti.h"
    #include"core_cm4.h"
    #include"misc.h"

    void exit_init(void);
    void EXTI1_IRQHandler(void);

#endif

#ifndef __OLED1_H
#define __OLED1_H			  	 
#include "stdlib.h"	  
#include "PinDef.h"  
#include"stm32f4xx.h" 	
#include"delay.h"

#define OLED1_MODE 	0 
		    						  
#define OLED1_CS 	PBout(12)
#define OLED1_WR 	PBout(12)		  
#define OLED1_RD 	PBout(12)
 
#define OLED1_SCLK 	PAout(3)
#define OLED1_SDIN 	PAout(2)
#define OLED1_RST   PAout(1)	    
#define OLED1_RS 	PAout(0)
		     
#define OLED1_CMD  	0		
#define OLED1_DATA 	1		

void OLED1_WR_Byte(u8 dat,u8 cmd);	    
void OLED1_Display_On(void);
void OLED1_Display_Off(void);
void OLED1_Refresh_Gram(void);		   
							   		    
void OLED1_Init(void);
void OLED1_Clear(void);
void OLED1_DrawPoint(u8 x,u8 y,u8 t);
void OLED1_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED1_ADDval(u8 value);
#endif  
	 




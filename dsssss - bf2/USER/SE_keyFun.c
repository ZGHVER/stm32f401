#include"SE_KeyFun.h"


/* 
   1（1）   2（2）   3（3）   确认（4）   
   4（5）   5（6）   6（7）   取消（8）
   7（9）   8（10）  9（11）  模式（12）
   0（13）  void（14） 距离（15） 角度（16）
 */

/*
    模式0 ：手动输入数据，启动发射（基础题）
    模式1 ： 半自动（发挥1）
    模式2 ： 全自动（发挥2）
 */

volatile uint8_t CurModeCode = 0;
volatile uint8_t LengthORAngle = 0;


volatile char Key_Buffer[10];
volatile uint8_t Buffer_ptr = 0;
volatile uint16_t GEtValue = 0;
volatile uint8_t isNewValue = 0;


void key1_Fun(){ //1
    if(Buffer_ptr < 10  | CurModeCode == 0)
        Key_Buffer[Buffer_ptr++] = 1;
}

void key2_Fun(){//2
    if(Buffer_ptr < 10 | CurModeCode == 0)
        Key_Buffer[Buffer_ptr++] = 2;
}

void key3_Fun(){//3
    if(Buffer_ptr < 10 | CurModeCode == 0)
        Key_Buffer[Buffer_ptr++] = 3;
}

float getValue(){
    uint8_t point;
    uint16_t int_part = 0;
    uint16_t k = 1;
    for(int i = Buffer_ptr - 1 ; i  > -1; i--){
        int_part += Key_Buffer[i] * k;
        k *= 10;
    }
    return int_part;
}

void key4_Fun(){//确认
    GEtValue = getValue();
    if(LengthORAngle == 1)
        LengthORAngle = 0;
    for(int i = 0; i < 10; i++){
        Key_Buffer[i] = 0;
    }
    Buffer_ptr = 0;
    isNewValue = 1;
}

void key5_Fun(){//4
    if(Buffer_ptr < 10 | CurModeCode == 0)
        Key_Buffer[Buffer_ptr++] = 4;
}

void key6_Fun(){//5
    if(Buffer_ptr < 10 | CurModeCode == 0)
        Key_Buffer[Buffer_ptr++] = 5;
}

void key7_Fun(){//6
    if(Buffer_ptr < 10 | CurModeCode == 0)
        Key_Buffer[Buffer_ptr++] = 6;
}

void key8_Fun(){//取消
    for(int i = 0; i < 10; i++){
        Key_Buffer[i] = 0;
    }
    Buffer_ptr = 0;
}

void key9_Fun(){//7
    if(Buffer_ptr < 10 | CurModeCode == 0)
        Key_Buffer[Buffer_ptr++] = 7;
}

void key10_Fun(){//8
    if(Buffer_ptr < 10 | CurModeCode == 0)
        Key_Buffer[Buffer_ptr++] = 8;
}

void key11_Fun(){//9
    if(Buffer_ptr < 10 | CurModeCode == 0)
        Key_Buffer[Buffer_ptr++] = 9;
}

void key12_Fun(){//模式

    CurModeCode ++;
    if(CurModeCode > 2)
        CurModeCode = 0;
}

void key13_Fun(){//0
    if(Buffer_ptr < 10 | CurModeCode == 0)
        Key_Buffer[Buffer_ptr++] = 0;
}

void key14_Fun(){//.

}

void key15_Fun(){//
    key8_Fun();//先取消输入
    LengthORAngle = 0;
}

void key16_Fun(){
    key8_Fun();
    LengthORAngle = 1;
}

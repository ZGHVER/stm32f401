#include "OLED1.h"
#include "stdlib.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "delay.h"

u8 OLED1_GRAM[128][8];
void OLED1_Refresh_Gram(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		OLED1_WR_Byte(0xb0 + i, OLED1_CMD);
		OLED1_WR_Byte(0x00, OLED1_CMD);
		OLED1_WR_Byte(0x10, OLED1_CMD);
		for (n = 0; n < 128; n++)
			OLED1_WR_Byte(OLED1_GRAM[n][i], OLED1_DATA);
	}
}

void OLED1_WR_Byte(u8 dat, u8 cmd)
{
	u8 i;
	OLED1_RS = cmd; //д����
	OLED1_CS = 0;
	for (i = 0; i < 8; i++)
	{
		OLED1_SCLK = 0;
		if (dat & 0x80)
			OLED1_SDIN = 1;
		else
			OLED1_SDIN = 0;
		OLED1_SCLK = 1;
		dat <<= 1;
	}
	OLED1_CS = 1;
	OLED1_RS = 1;
}

void OLED1_Display_On(void)
{
	OLED1_WR_Byte(0X8D, OLED1_CMD);
	OLED1_WR_Byte(0X14, OLED1_CMD);
	OLED1_WR_Byte(0XAF, OLED1_CMD);
}

void OLED1_Display_Off(void)
{
	OLED1_WR_Byte(0X8D, OLED1_CMD);
	OLED1_WR_Byte(0X10, OLED1_CMD);
	OLED1_WR_Byte(0XAE, OLED1_CMD);
}

void OLED1_Clear(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
		for (n = 0; n < 128; n++)
			OLED1_GRAM[n][i] = 0X00;
}

void OLED1_DrawPoint(u8 x, u8 y, u8 t)
{
	u8 pos, bx, temp = 0;
	if (x > 127 || y > 63)
		return;
	pos = 7 - y / 8;
	bx = y % 8;
	temp = 1 << (7 - bx);
	if (t)
		OLED1_GRAM[x][pos] |= temp;
	else
		OLED1_GRAM[x][pos] &= ~temp;
}

void OLED1_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot)
{
	u8 x, y;
	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)
			OLED1_DrawPoint(x, y, dot);
	}
	OLED1_Refresh_Gram();
}

void OLED1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_2 | GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	OLED1_SDIN = 1;
	OLED1_SCLK = 1;
	OLED1_CS = 1;
	OLED1_RS = 1;

	OLED1_RST = 0;
	delay_ms(100);
	OLED1_RST = 1;

	OLED1_WR_Byte(0xAE, OLED1_CMD); //�ر���ʾ
	OLED1_WR_Byte(0xD5, OLED1_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED1_WR_Byte(0x80, OLED1_CMD); //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED1_WR_Byte(0xA8, OLED1_CMD); //��������·��
	OLED1_WR_Byte(0X3F, OLED1_CMD); //Ĭ��0X3F(1/64)
	OLED1_WR_Byte(0xD3, OLED1_CMD); //������ʾƫ��
	OLED1_WR_Byte(0X00, OLED1_CMD); //Ĭ��Ϊ0

	OLED1_WR_Byte(0x40, OLED1_CMD); //������ʾ��ʼ�� [5:0],����.

	OLED1_WR_Byte(0x8D, OLED1_CMD); //��ɱ�����?
	OLED1_WR_Byte(0x14, OLED1_CMD); //bit2������/�ر�
	OLED1_WR_Byte(0x20, OLED1_CMD); //�����ڴ��ַģ�?
	OLED1_WR_Byte(0x02, OLED1_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED1_WR_Byte(0xA1, OLED1_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED1_WR_Byte(0xC0, OLED1_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED1_WR_Byte(0xDA, OLED1_CMD); //����COMӲ����������
	OLED1_WR_Byte(0x12, OLED1_CMD); //[5:4]����

	OLED1_WR_Byte(0x81, OLED1_CMD); //�Աȶ�����
	OLED1_WR_Byte(0xEF, OLED1_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED1_WR_Byte(0xD9, OLED1_CMD); //����Ԥ�������?
	OLED1_WR_Byte(0xf1, OLED1_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED1_WR_Byte(0xDB, OLED1_CMD); //����VCOMH ��ѹ����
	OLED1_WR_Byte(0x30, OLED1_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED1_WR_Byte(0xA4, OLED1_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED1_WR_Byte(0xA6, OLED1_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ
	OLED1_WR_Byte(0xAF, OLED1_CMD); //������ʾ
	OLED1_Clear();
}

void OLED1_ADDval(u8 x)
{
    static u8 max = 0;
    u8 i = 0, j = 0;
    if (x > 64)
        return;
    if (max < 128)
    {
        OLED1_DrawPoint(max, 63 - x, 1);
        max++;
    }
    else
    {
        for (i = 0; i < 8; i++)
            OLED1_GRAM[0][i] = OLED1_GRAM[1][i];
        for (i = 1; i < 127; i++)
        {
            for (j = 0; j < 8; j++)
                OLED1_GRAM[i][j] = OLED1_GRAM[i + 1][j];
        }
        for (j = 0; j < 8; j++)
            OLED1_GRAM[127][j] = 0;
        OLED1_DrawPoint(127, 63 - x, 1);
    }
}


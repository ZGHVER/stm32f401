#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "delay.h"

u8 OLED_GRAM[128][8];

void OLED_Refresh_Gram(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD);
		OLED_WR_Byte(0x00, OLED_CMD);
		OLED_WR_Byte(0x10, OLED_CMD);
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
	}
}
#if OLED_MODE == 1 //8080����
//ͨ��ƴ�յķ�����OLED���һ��?8λ����
//data:Ҫ���������?
void OLED_Data_Out(u8 data)
{
	u16 dat = data & 0X0F;
	GPIOC->ODR &= ~(0XF << 6); //���?6~9
	GPIOC->ODR |= dat << 6;		 //D[3:0]-->PC[9:6]
	GPIO_Write(GPIOC, dat << 6);
	PCout(11) = (data >> 4) & 0X01; //D4
	PBout(6) = (data >> 5) & 0X01;	//D5
	PEout(5) = (data >> 6) & 0X01;	//D6
	PEout(6) = (data >> 7) & 0X01;	//D7
}
//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������?/����
//cmd:����/������? 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat, u8 cmd)
{
	OLED_Data_Out(dat);
	OLED_RS = cmd;
	OLED_CS = 0;
	OLED_WR = 0;
	OLED_WR = 1;
	OLED_CS = 1;
	OLED_RS = 1;
}
#else

void OLED_WR_Byte(u8 dat, u8 cmd)
{
	u8 i;
	OLED_RS = cmd; //д����
	for (i = 0; i < 8; i++)
	{
		OLED_SCLK = 0;
		if (dat & 0x80)
			OLED_SDIN = 1;
		else
			OLED_SDIN = 0;
		OLED_SCLK = 1;
		dat <<= 1;
	}
	OLED_RS = 1;
}
#endif

void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);
	OLED_WR_Byte(0X14, OLED_CMD);
	OLED_WR_Byte(0XAF, OLED_CMD);
}

void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);
	OLED_WR_Byte(0X10, OLED_CMD);
	OLED_WR_Byte(0XAE, OLED_CMD);
}

void OLED_Clear(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
		for (n = 0; n < 128; n++)
			OLED_GRAM[n][i] = 0X00;
}

void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
	u8 pos, bx, temp = 0;
	if (x > 127 || y > 63)
		return;
	pos = 7 - y / 8;
	bx = y % 8;
	temp = 1 << (7 - bx);
	if (t)
		OLED_GRAM[x][pos] |= temp;
	else
		OLED_GRAM[x][pos] &= ~temp;
}

void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot)
{
	u8 x, y;
	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)
			OLED_DrawPoint(x, y, dot);
	}
	OLED_Refresh_Gram();
}

void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
	u8 temp, t, t1;
	u8 y0 = y;
	u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
	chr = chr - ' ';
	for (t = 0; t < csize; t++)
	{
		if (size == 12)
			temp = asc2_1206[chr][t];
		else if (size == 16)
			temp = asc2_1608[chr][t];
		else if (size == 24)
			temp = asc2_2412[chr][t];
		else
			return;
		for (t1 = 0; t1 < 8; t1++)
		{
			if (temp & 0x80)
				OLED_DrawPoint(x, y, mode);
			else
				OLED_DrawPoint(x, y, !mode);
			temp <<= 1;
			y++;
			if ((y - y0) == size)
			{
				y = y0;
				x++;
				break;
			}
		}
	}
}
u32 mypow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size)
{
	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size / 2) * t, y, ' ', size, 1);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size, 1);
	}
}

void OLED_ShowString(u8 x, u8 y, const u8 *p, u8 size)
{
	while ((*p <= '~') && (*p >= ' ')) //�ж��ǲ��ǷǷ��ַ�!
	{
		if (x > (128 - (size / 2)))
		{
			x = 0;
			y += size;
		}
		if (y > (64 - size))
		{
			y = x = 0;
			OLED_Clear();
		}
		OLED_ShowChar(x, y, *p, size, 1);
		x += size / 2;
		p++;
	}
}
void OLED_Init(void)
{
	GPIO_InitTypeDef ioA;
	ioA.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	ioA.GPIO_Mode = GPIO_Mode_OUT;
	ioA.GPIO_OType = GPIO_OType_PP;
	ioA.GPIO_PuPd = GPIO_PuPd_UP;
	ioA.GPIO_Speed = GPIO_Speed_50MHz;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_Init(GPIOA, &ioA);
	OLED_SDIN = 1;
	OLED_SCLK = 1;
	OLED_RS = 1;

	OLED_RST = 0;

	delay_ms(100);
	OLED_RST = 1;

	OLED_WR_Byte(0xAE, OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0xD5, OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(0x80, OLED_CMD); //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8, OLED_CMD); //��������·��
	OLED_WR_Byte(0X3F, OLED_CMD); //Ĭ��0X3F(1/64)
	OLED_WR_Byte(0xD3, OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00, OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40, OLED_CMD); //������ʾ��ʼ�� [5:0],����.

	OLED_WR_Byte(0x8D, OLED_CMD); //��ɱ�����?
	OLED_WR_Byte(0x14, OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20, OLED_CMD); //�����ڴ��ַģ�?
	OLED_WR_Byte(0x02, OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1, OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0, OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA, OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12, OLED_CMD); //[5:4]����

	OLED_WR_Byte(0x81, OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xEF, OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9, OLED_CMD); //����Ԥ�������?
	OLED_WR_Byte(0xf1, OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB, OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30, OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4, OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6, OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ
	OLED_WR_Byte(0xAF, OLED_CMD); //������ʾ
	OLED_Clear();
}

void OLED_ADDval(u8 x)
{
    static u8 max = 0;
    u8 i = 0, j = 0;
    if (x > 64)
        return;
    if (max < 128)
    {
        OLED_DrawPoint(max, 63 - x, 1);
        max++;
    }
    else
    {
        for (i = 0; i < 8; i++)
            OLED_GRAM[0][i] = OLED_GRAM[1][i];
        for (i = 1; i < 127; i++)
        {
            for (j = 0; j < 8; j++)
                OLED_GRAM[i][j] = OLED_GRAM[i + 1][j];
        }
        for (j = 0; j < 8; j++)
            OLED_GRAM[127][j] = 0;
        OLED_DrawPoint(127, 63 - x, 1);
    }
}


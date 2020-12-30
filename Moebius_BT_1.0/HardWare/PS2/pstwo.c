#include "pstwo.h"

#define DELAY_TIME  delay_us(5);
u16 Handkey;
u8 Comd[2] = { 0x01, 0x42 };
u8 Data[9] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
u16 MASK[] = {
	PSB_SELECT,
	PSB_L3,
	PSB_R3,
	PSB_START,
	PSB_PAD_UP,
	PSB_PAD_RIGHT,
	PSB_PAD_DOWN,
	PSB_PAD_LEFT,
	PSB_L2,
	PSB_R2,
	PSB_L1,
	PSB_R1,
	PSB_GREEN,
	PSB_RED,
	PSB_BLUE,
	PSB_PINK
};

void PS2_Init(void)
{
	RCC->APB2ENR |= 1 << 2;
	RCC->APB2ENR |= 1 << 4;
	GPIOC->CRL &= 0XFFFFF0FF;
	GPIOC->CRL |= 0X00000800;

	RCC->APB2ENR |= 1 << 4;
	GPIOC->CRL &= 0XFFFF0F0F;
	GPIOC->CRL |= 0X00003030;

	GPIOA->CRL &= 0XFFF0FFFF;
	GPIOA->CRL |= 0X00030000;
}

void PS2_Cmd(u8 CMD)
{
	volatile u16 ref = 0x01;

	Data[1] = 0;
	for (ref = 0x01; ref < 0x0100; ref <<= 1) {
		if (ref & CMD) {
			DO_H;
		} else   {
			DO_L;
		}

		CLK_H;
		DELAY_TIME;
		CLK_L;
		DELAY_TIME;
		CLK_H;
		if (DI) {
			Data[1] = ref | Data[1];
		}
	}
	delay_us(16);
}

u8 PS2_RedLight(void)
{
	CS_L;
	PS2_Cmd(Comd[0]);
	PS2_Cmd(Comd[1]);
	CS_H;
	if (Data[1] == 0X73) {
		return 0;
	} else {
		return 1;
	}

}

void PS2_ReadData(void)
{
	volatile u8 byte = 0;
	volatile u16 ref = 0x01;

	CS_L;
	PS2_Cmd(Comd[0]);
	PS2_Cmd(Comd[1]);
	for (byte = 2; byte < 9; byte++) {
		for (ref = 0x01; ref < 0x100; ref <<= 1) {
			CLK_H;
			DELAY_TIME;
			CLK_L;
			DELAY_TIME;
			CLK_H;
			if (DI) {
				Data[byte] = ref | Data[byte];
			}
		}
		delay_us(16);
	}
	CS_H;
}

u8 PS2_DataKey()
{
	u8 index;

	PS2_ClearData();
	PS2_ReadData();

	Handkey = (Data[4] << 8) | Data[3];
	for (index = 0; index < 16; index++) {
		if ((Handkey & (1 << (MASK[index] - 1))) == 0) {
			return index + 1;
		}
	}
	return 0;
}

u8 PS2_AnologData(u8 button)
{
	return Data[button];
}

void PS2_ClearData()
{
	u8 a;

	for (a = 0; a < 9; a++)
		Data[a] = 0x00;
}

void PS2_Vibration(u8 motor1, u8 motor2)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0X00);
	PS2_Cmd(motor1);
	PS2_Cmd(motor2);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS_H;
	delay_us(16);
}

// short poll
void PS2_ShortPoll(void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	delay_us(16);
}

void PS2_EnterConfing(void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0X00);
	PS2_Cmd(0x01);
	PS2_Cmd(0x00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS_H;
	delay_us(16);
}

void PS2_TurnOnAnalogMode(void)
{
	CS_L;
	PS2_Cmd(0x01);
	PS2_Cmd(0x44);
	PS2_Cmd(0X00);
	PS2_Cmd(0x01);
	PS2_Cmd(0x03);

	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS_H;
	delay_us(16);
}

void PS2_VibrationMode(void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x4D);
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0X01);
	CS_H;
	delay_us(16);
}

void PS2_ExitConfing(void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	CS_H;
	delay_us(16);
}

void PS2_SetInit(void)
{
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfing();
	PS2_TurnOnAnalogMode();

	PS2_ExitConfing();
}

void PS2_Receive(void)
{
	PS2_LX = PS2_AnologData(PSS_LX);
	PS2_LY = PS2_AnologData(PSS_LY);
	PS2_RX = PS2_AnologData(PSS_RX);
	PS2_RY = PS2_AnologData(PSS_RY);
	PS2_KEY = PS2_DataKey();
}

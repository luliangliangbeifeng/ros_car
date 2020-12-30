#include "show.h"
#include "sys.h"

unsigned char i;
unsigned char Send_Count;
float Vol;

void oled_show(void)
{
	OLED_ShowString(0, 0, (u8 *)"X:");
	if (Pitch < 0) {
		OLED_ShowNumber(15, 0, Pitch + 360, 3, 12);
	} else {
		OLED_ShowNumber(15, 0, Pitch, 3, 12);
	}

	OLED_ShowString(40, 0, (u8 *)"Y:");
	if (Roll < 0) {
		OLED_ShowNumber(55, 0, Roll + 360, 3, 12);
	} else {
		OLED_ShowNumber(55, 0, Roll, 3, 12);
	}

	OLED_ShowString(80, 0, (u8 *)"Z:");
	if (Yaw < 0) {
		OLED_ShowNumber(95, 0, Yaw + 360, 3, 12);
	} else {
		OLED_ShowNumber(95, 0, Yaw, 3, 12);
	}

	if (Position_X < 0) {
		OLED_ShowString(00, 10, (u8 *)"-"),
		OLED_ShowNumber(15, 10, -Position_X, 5, 12);
	} else {
		OLED_ShowString(0, 10, (u8 *)"+"),
		OLED_ShowNumber(15, 10, Position_X, 5, 12);
	}
	if (Encoder_A < 0) {
		OLED_ShowString(80, 10, (u8 *)"-"),
		OLED_ShowNumber(95, 10, -Encoder_A, 5, 12);
	} else {
		OLED_ShowString(80, 10, (u8 *)"+"),
		OLED_ShowNumber(95, 10, Encoder_A, 5, 12);
	}

	if (Position_Y < 0) {
		OLED_ShowString(00, 20, (u8 *)"-"),
		OLED_ShowNumber(15, 20, -Position_Y, 5, 12);
	} else {
		OLED_ShowString(0, 20, (u8 *)"+"),
		OLED_ShowNumber(15, 20, Position_Y, 5, 12);
	}
	if (Encoder_B < 0) {
		OLED_ShowString(80, 20, (u8 *)"-"),
		OLED_ShowNumber(95, 20, -Encoder_B, 5, 12);
	} else {
		OLED_ShowString(80, 20, (u8 *)"+"),
		OLED_ShowNumber(95, 20, Encoder_B, 5, 12);
	}

	if (V_X < 0) {
		OLED_ShowString(00, 30, (u8 *)"-"),
		OLED_ShowNumber(15, 30, -V_X, 5, 12);
	} else {
		OLED_ShowString(0, 30, (u8 *)"+"),
		OLED_ShowNumber(15, 30, V_X, 5, 12);
	}
	if (Encoder_C < 0) {
		OLED_ShowString(80, 30, (u8 *)"-"),
		OLED_ShowNumber(95, 30, -Encoder_C, 5, 12);
	} else {
		OLED_ShowString(80, 30, (u8 *)"+"),
		OLED_ShowNumber(95, 30, Encoder_C, 5, 12);
	}

	if (V_Y < 0) {
		OLED_ShowString(00, 40, (u8 *)"-"),
		OLED_ShowNumber(15, 40, -V_Y, 5, 12);
	} else {
		OLED_ShowString(0, 40, (u8 *)"+"),
		OLED_ShowNumber(15, 40, V_Y, 5, 12);
	}

	if (Encoder_D < 0) {
		OLED_ShowString(80, 40, (u8 *)"-"),
		OLED_ShowNumber(95, 40, -Encoder_D, 5, 12);
	} else {
		OLED_ShowString(80, 40, (u8 *)"+"),
		OLED_ShowNumber(95, 40, Encoder_D, 5, 12);
	}
 
	OLED_ShowString(00, 50, (u8 *)"VELOCITY");
	OLED_ShowString(88, 50, (u8 *)".");
	OLED_ShowString(110, 50, (u8 *)"V");
	OLED_ShowNumber(75, 50, Voltage / 100, 2, 12);
	OLED_ShowNumber(98, 50, Voltage % 100, 2, 12);
	if (Voltage % 100 < 10) {
		OLED_ShowNumber(92, 50, 0, 2, 12);
	}
	OLED_Refresh_Gram();
}

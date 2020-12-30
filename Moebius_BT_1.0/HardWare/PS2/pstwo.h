#ifndef __PSTWO_H
#define __PSTWO_H
#include "sys.h"


#define DI   PCin(2)

#define DO_H PCout(1) = 1
#define DO_L PCout(1) = 0

#define CS_H PCout(3) = 1
#define CS_L PCout(3) = 0

#define CLK_H PAout(4) = 1
#define CLK_L PAout(4) = 0


// These are our button constants
#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2          9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13
#define PSB_RED         14
#define PSB_BLUE        15
#define PSB_PINK        16

#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      16

// #define WHAMMY_BAR   8

// These are stick values
#define PSS_RX 5
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8

extern u8 Data[9];
extern u16 MASK[16];
extern u16 Handkey;

void PS2_Init(void);
u8 PS2_RedLight(void);
void PS2_ReadData(void);
void PS2_Cmd(u8 CMD);
u8 PS2_DataKey(void);
u8 PS2_AnologData(u8 button);
void PS2_ClearData(void);
void PS2_Vibration(u8 motor1, u8 motor2);

void PS2_EnterConfing(void);
void PS2_TurnOnAnalogMode(void);
void PS2_VibrationMode(void);
void PS2_ExitConfing(void);
void PS2_SetInit(void);
void PS2_Receive(void);
#endif

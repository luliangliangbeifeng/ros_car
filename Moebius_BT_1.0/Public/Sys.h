#ifndef _Sys_H
#define _Sys_H

// ========================================================================
// ͷ�ļ�����

#include "stm32f10x.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "Systick.h"
#include "define.h"
#include "usart.h"
#include "LED.h"
#include "KEY.h"
#include "oled.h"
#include "show.h"
#include "motor.h"
#include "ioi2c.h"
#include "BlueTooth.h"
#include "control.h"
#include "exti.h"
#include "encoder.h"
#include "adc.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "mpu6050.h"
#include "dmpKey.h"
#include "dmpmap.h"
#include "pstwo.h"

extern int Encoder_A, Encoder_B, Encoder_C, Encoder_D;                                  // ���������������
extern long int Motor_A, Motor_B, Motor_C, Motor_D;                                     // ���PWM����
extern float Target_A, Target_B, Target_C, Target_D;    // ���Ŀ���ٶ�
extern int Temperature;
extern u32 Distance;                                                                    // ���������
extern int RC_Velocity;                                                                 // RC_Position;
extern float Pitch, Roll, Yaw, Move_X, Move_Y, Move_Z;
extern int PS2_LX, PS2_LY, PS2_RX, PS2_RY, PS2_KEY;
extern float Gryo_Z, acc_X, acc_Y, acc_Z;
extern float q0, q1, q2, q3;            //////////////////////////////////
extern float Position_X, Position_Y, Position_Z, angle_Z;
extern float RC_Position;
extern int Voltage;
extern float V_X, V_Y;
extern int PS2_LX, PS2_LY, PS2_RX, PS2_RY, PS2_KEY;
extern u8 PS2_BLU;
extern u8 flag_control;
extern u8 Run_Flag;
extern float KP, KI;
// Ex_NVIC_Configר�ö���
#define GPIO_A 0
#define GPIO_B 1
#define GPIO_C 2
#define GPIO_D 3
#define GPIO_E 4
#define GPIO_F 5
#define GPIO_G 6

#define FTIR   1                                                                                        //�½��ش���
#define RTIR   2                                                                                        //�����ش���

void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset);                                              // ����ƫ�Ƶ�ַ
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);                                                        // ����NVIC����
void MY_NVIC_Init(u8 NVIC_PreemptionPriority, u8 NVIC_SubPriority, u8 NVIC_Channel, u8 NVIC_Group);     // �����ж�
void Ex_NVIC_Config(u8 GPIOx, u8 BITx, u8 TRIM);                                                        // �ⲿ�ж����ú���(ֻ��GPIOA~G)

#endif


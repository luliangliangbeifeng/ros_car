#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"

#define PI 3.14159265
#define ZHONGZHI 0
#define DIFFERENCE 100
void Set_Pwm(int motor_a, int motor_b, int motor_c, int motor_d);
void Kinematic_Analysis(float Vx, float Vy, float Vz);
void Xianfu_Pwm(int amplitude);
void Xianfu_Velocity(int amplitude_A, int amplitude_B, int amplitude_C, int amplitude_D);
u32 myabs(long int a);
int Position_PID_A(int Encoder, float Target);
int Position_PID_B(int Encoder, float Target);
int Position_PID_C(int Encoder, float Target);
int Position_PID_D(int Encoder, float Target);
void PS_control(void);
void Encoder_collect(void);
void distance_Y(float distance);        // Y轴定距离移动
void speedrun(float speed, float dir);  //按照固定方向移动
void dir_Z(float angle);                // 自转
void TIM7_IRQHandler(void);
void TIM7_Init(void);
#endif

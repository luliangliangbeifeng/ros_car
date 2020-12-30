#include "control.h"
#include "filter.h"

int64_t flag_enconder = 0;
int64_t flag_push = 0;
int LX, LY, RX;
int Yuzhi = 20;
float move_x , move_y, move_z;   // m/s
float Voltage_Count, Voltage_All;
#define a_PARAMETER          (0.311f)
#define b_PARAMETER          (0.3075f)


void TIM7_Init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	TIM_TimeBaseInitStruct.TIM_Period = 49;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7199;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStruct.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_Init(&NVIC_InitStruct);
	TIM_Cmd(TIM7, ENABLE);
}

/**************************************************************************
   main control
**************************************************************************/
void TIM7_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) == 1) {
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	}
	if (flag_control) {
		move_x = Move_X * 825 / PI;
		move_y = Move_Y * 825 / PI;
		move_z = Move_Z * 104;
	} else {
		move_x = -LX * RC_Velocity / 80;
		move_y = -LY * RC_Velocity / 80;
		move_z = -RX * RC_Velocity / 80;
	}
	PS_control();
	flag_enconder++;                                        // 50ms per time
	if (flag_enconder % 10 == 1) {
		Encoder_collect();                              // collect Encoder and Position
		
		Kinematic_Analysis(move_x, move_y, move_z);     // The control target value is obtained and kinematic analysis is carried out
		
		Motor_A = Position_PID_A(Encoder_A, Target_A);  // front-right
		Motor_B = Position_PID_B(Encoder_B, -Target_B); // front-left
		Motor_C = Position_PID_C(Encoder_C, Target_C);  // back-left
		Motor_D = Position_PID_D(Encoder_D, -Target_D); // back-right
		
		Set_Pwm(Motor_A, Motor_B, Motor_C, Motor_D);
		Xianfu_Pwm(7200);
	}
	Voltage_All += Get_battery_volt();
	if (++Voltage_Count == 100) {
		Voltage = Voltage_All / 100,
		Voltage_All = 0,
		Voltage_Count = 0;
	}
	flag_push++; // 100ms per time
	if (flag_push % 20 == 0) {
		push(0, q0);
		push(1, q1);
		push(2, q2);
		push(3, q3);            // quaternion
		push(4, Position_X);    // distance X
		push(5, Position_Y);    // distance Y
		push(6, V_Y);    // distance Z
		push(7, acc_X);         // imu acc X
		push(8, acc_Y);         // imu acc Y
		push(9, V_X);         // imu acc Z
		push(10, Gryo_Z);       // imu gryo Z
		push(11, Yaw / 57.3);   // angel Z

		sendDataToScope();
	}
}

void Set_Pwm(int motor_a, int motor_b, int motor_c, int motor_d)   // Set PWM register
{
	if (motor_a < 0) {
		INA2 = 1,       INA1 = 0;
	} else {
		INA2 = 0,       INA1 = 1;
	}
	PWMA = myabs(motor_a);
	if (motor_b < 0) {
		INB2 = 1,       INB1 = 0;
	} else {
		INB2 = 0,       INB1 = 1;
	}
	PWMB = myabs(motor_b);
	if (motor_c > 0) {
		INC2 = 1,       INC1 = 0;
	} else {
		INC2 = 0,       INC1 = 1;
	}
	PWMC = myabs(motor_c);
	if (motor_d > 0) {
		IND2 = 1,       IND1 = 0;
	} else {
		IND2 = 0,       IND1 = 1;
	}
	PWMD = myabs(motor_d);
}

void Xianfu_Pwm(int amplitude)   // Limit PWM
{
	if (Motor_A < -amplitude) {
		Motor_A = -amplitude;
	}
	if (Motor_A > amplitude) {
		Motor_A = amplitude;
	}
	if (Motor_B < -amplitude) {
		Motor_B = -amplitude;
	}
	if (Motor_B > amplitude) {
		Motor_B = amplitude;
	}
	if (Motor_C < -amplitude) {
		Motor_C = -amplitude;
	}
	if (Motor_C > amplitude) {
		Motor_C = amplitude;
	}
	if (Motor_D < -amplitude) {
		Motor_D = -amplitude;
	}
	if (Motor_D > amplitude) {
		Motor_D = amplitude;
	}
}

u32 myabs(long int a)
{
	u32 temp;

	if (a < 0) {
		temp = -a;
	} else {
		temp = a;
	}
	return temp;
}

void Kinematic_Analysis(float Vx, float Vy, float Vz) //	Mathematical model of car motion
{
	Target_A = +Vx + Vy + Vz * (a_PARAMETER + b_PARAMETER);
	Target_B = -Vx + Vy - Vz * (a_PARAMETER + b_PARAMETER);
	Target_C = +Vx + Vy - Vz * (a_PARAMETER + b_PARAMETER);
	Target_D = -Vx + Vy + Vz * (a_PARAMETER + b_PARAMETER);
}

int Position_PID_A(int Encoder, float Target)   // PID controll
{
	static float Bias, Pwm, Integral_bias;

	Bias = Encoder - Target;
	Integral_bias += Bias;
	if (Integral_bias > 100000) {
		Integral_bias = 10000;
	}
	if (Integral_bias < -100000) {
		Integral_bias = -10000;
	}
	Pwm = KP * Bias + KI * Integral_bias;
	return Pwm;
}

int Position_PID_B(int Encoder, float Target)    // PID controll
{
	static float Bias, Pwm, Integral_bias;

	Bias = Encoder - Target;
	Integral_bias += Bias;
	if (Integral_bias > 100000) {
		Integral_bias = 10000;
	}
	if (Integral_bias < -100000) {
		Integral_bias = -10000;
	}
	Pwm = KP * Bias + KI * Integral_bias;
	return Pwm;
}

int Position_PID_C(int Encoder, float Target)    // PID controll
{
	static float Bias, Pwm, Integral_bias;

	Bias = Encoder - Target;
	Integral_bias += Bias;
	if (Integral_bias > 100000) {
		Integral_bias = 10000;
	}
	if (Integral_bias < -100000) {
		Integral_bias = -10000;
	}
	Pwm = KP* Bias + KI * Integral_bias;
	return Pwm;
}

int Position_PID_D(int Encoder, float Target)    // PID controll
{
	static float Bias, Pwm, Integral_bias;

	Bias = Encoder - Target;
	Integral_bias += Bias;
	if (Integral_bias > 100000) {
		Integral_bias = 10000;
	}
	if (Integral_bias < -100000) {
		Integral_bias = -10000;
	}
	Pwm = KP * Bias + KI * Integral_bias;
	return Pwm;
}

void PS_control() // PS control
{
	if ((PS2_LX > 250 && PS2_LY > 250 && PS2_RX > 250 && PS2_RY > 250) || (PS2_LX == 0 && PS2_LY == 0 && PS2_RX == 0 && PS2_RY == 0)) {
		PS2_LX = 128;
		PS2_LY = 128;
		PS2_RX = 128;
		PS2_RY = 128;
	}
	LX = PS2_LX - 128;
	LY = PS2_LY - 128;
	RX = PS2_RX - 128;
	if (LX > -Yuzhi && LX < Yuzhi) {
		LX = 0;
	}
	if (LY > -Yuzhi && LY < Yuzhi) {
		LY = 0;
	}
	if (RX > -Yuzhi && RX < Yuzhi) {
		RX = 0;
	}
}

void Encoder_collect()
{
	Encoder_A = -Read_Encoder(3);
	Encoder_B = -Read_Encoder(2);
	Encoder_C = Read_Encoder(5);
	Encoder_D = Read_Encoder(4);
	
	V_Y = - (-Encoder_A - Encoder_B - Encoder_C - Encoder_D) / ( 4 * 825 / PI ) ;// 210.08 ;
	V_X = (Encoder_A - Encoder_B + Encoder_C - Encoder_D) / ( 4 * 825 / PI );// 210.08;
	Position_X += (Encoder_A - Encoder_B + Encoder_C - Encoder_D) / 210.08 * cos(Yaw / 57.3) - (-Encoder_A - Encoder_B - Encoder_C - Encoder_D) / 210.08 * sin(-Yaw / 57.3);
	Position_Y += (Encoder_A - Encoder_B + Encoder_C - Encoder_D) / 210.08 * sin(Yaw / 57.3) - (-Encoder_A - Encoder_B - Encoder_C - Encoder_D) / 210.08 * cos(-Yaw / 57.3);
	Position_Z = 0;
}


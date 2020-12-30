#include "sys.h"

int Encoder_A, Encoder_B, Encoder_C, Encoder_D;	// encoder pulse counting
float Position_X, Position_Y, Position_Z, angle_Z; //
long int Motor_A, Motor_B, Motor_C, Motor_D; // motor pwm variable
float Target_A , Target_B , Target_C , Target_D ; // motor target value
int Voltage; //	Battery voltage sampling related variables
float Pitch, Roll, Yaw, Move_X = 0, Move_Y = 0, Move_Z = 0;	// Triaxial Angle and x, y, z target velocity(speed)
int RC_Velocity = 25;
float KP = 15, KI = 15;
float Gryo_Z, acc_X, acc_Y, acc_Z;
float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
float V_X,V_Y;
int PS2_LX, PS2_LY, PS2_RX, PS2_RY, PS2_KEY;
u8 flag_control = 1;
u8 PS2_BLU;
u8 Run_Flag;

void Peripheral_Init() 
{
	LED_Init();
	KEY_Init();
	USART1_Init(9600);
	USART2_Init(115200);
	IIC_Init();
	MiniBalance_PWM_Init(7199, 0);
	MPU6050_initialize();
	DMP_Init();
	OLED_Init();
	Adc_Init();
	PS2_Init();
	Encoder_Init_TIM2();
	Encoder_Init_TIM3();
	Encoder_Init_TIM4();
	Encoder_Init_TIM5();
	TIM7_Init();
}

int main(void) //Main function entry
{
	uint8_t PS2_Hold;
	
	Peripheral_Init();
	OLED_ShowString(0, 0, (u8*)"OLED Test");
	OLED_Refresh_Gram();
	while (1)
	{	
		if(PS2_KEY)
		{
			do
			{
				if(PS2_Hold)	break;
				printf("PS2 Key = %d",PS2_KEY);
				switch(PS2_KEY)	
				{
					case 1:	PS2_Hold = 1;	break; 
					case 2:	PS2_Hold = 1;	break; 
					case 3:	PS2_Hold = 1;	break; 
					case 4:	PS2_BLU = 0;	PS2_Hold = 1;	break; //start
					case 5:	PS2_Hold = 1;	break; 
					case 6:	PS2_Hold = 1;	break; 
					case 7:	PS2_Hold = 1;	break; 
					case 8:	PS2_Hold = 1;	break; 
					case 9:	PS2_Hold = 1;	break; 
					case 10: PS2_Hold = 1;	break; 
					case 11: PS2_Hold = 1;	break; 
					case 12: PS2_Hold = 1;	break; 
					case 13: if( PS2_Hold && RC_Velocity < 25 ) RC_Velocity += 5;	PS2_Hold = 1;	break; //RÉÏ
					case 14: PS2_Hold = 1;	break; 
					case 15: if( PS2_Hold && RC_Velocity > 5 ) RC_Velocity -= 5;	PS2_Hold = 1;	break; //RÏÂ
					case 16: PS2_Hold = 1;	break; 
				}
			}while(0);
		}
		else	PS2_Hold = 0;
		PS2_Receive();
		oled_show();		
		Read_DMP();
	}
}

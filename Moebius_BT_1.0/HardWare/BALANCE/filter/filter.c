#include "filter.h"

float K1 = 0.02;
float angle, angle_dot;
float Q_angle = 0.001;
float Q_gyro = 0.003;
float R_angle = 0.5;
float dt = 0.005;
char C_0 = 1;
float Q_bias, Angle_err;
float PCt_0, PCt_1, E;
float K_0, K_1, t_0, t_1;
float Pdot[4] = { 0, 0, 0, 0 };
float PP[2][2] = { { 1, 0 }, { 0, 1 } };


void Kalman_Filter(float Accel, float Gyro)
{
	angle += (Gyro - Q_bias) * dt;
	Pdot[0] = Q_angle - PP[0][1] - PP[1][0];

	Pdot[1] = -PP[1][1];
	Pdot[2] = -PP[1][1];
	Pdot[3] = Q_gyro;
	PP[0][0] += Pdot[0] * dt;
	PP[0][1] += Pdot[1] * dt;
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;

	Angle_err = Accel - angle;

	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];

	E = R_angle + C_0 * PCt_0;

	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;

	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;

	angle += K_0 * Angle_err;
	Q_bias += K_1 * Angle_err;
	angle_dot = Gyro - Q_bias;
}

void Yijielvbo(float angle_m, float gyro_m)
{
	angle = K1 * angle_m + (1 - K1) * (angle + gyro_m * 0.005);
}

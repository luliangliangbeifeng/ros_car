#include "ioi2c.h"
#include "sys.h"

void IIC_Init(void)
{
	RCC->APB2ENR |= 1 << 3;
	GPIOB->CRH &= 0XFFFF00FF;
	GPIOB->CRH |= 0X00003300;
}

int IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA = 1;
	if (!READ_SDA) {
		return 0;
	}
	IIC_SCL = 1;
	delay_us(1);
	IIC_SDA = 0;
	if (READ_SDA) {
		return 0;
	}
	delay_us(1);
	IIC_SCL = 0;
	return 1;
}

void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL = 0;
	IIC_SDA = 0;
	delay_us(1);
	IIC_SCL = 1;
	IIC_SDA = 1;
	delay_us(1);
}

int IIC_Wait_Ack(void)
{
	u8 ucErrTime = 0;

	SDA_IN();
	IIC_SDA = 1;
	delay_us(1);
	IIC_SCL = 1;
	delay_us(1);
	while (READ_SDA) {
		ucErrTime++;
		if (ucErrTime > 50) {
			IIC_Stop();
			return 0;
		}
		delay_us(1);
	}
	IIC_SCL = 0;
	return 1;
}

void IIC_Ack(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 0;
	delay_us(1);
	IIC_SCL = 1;
	delay_us(1);
	IIC_SCL = 0;
}

void IIC_NAck(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 1;
	delay_us(1);
	IIC_SCL = 1;
	delay_us(1);
	IIC_SCL = 0;
}

void IIC_Send_Byte(u8 txd)
{
	u8 t;

	SDA_OUT();
	IIC_SCL = 0;
	for (t = 0; t < 8; t++) {
		IIC_SDA = (txd & 0x80) >> 7;
		txd <<= 1;
		delay_us(1);
		IIC_SCL = 1;
		delay_us(1);
		IIC_SCL = 0;
		delay_us(1);
	}
}

int i2cWrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
	int i;

	if (!IIC_Start()) {
		return 1;
	}
	IIC_Send_Byte(addr << 1);
	if (!IIC_Wait_Ack()) {
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	for (i = 0; i < len; i++) {
		IIC_Send_Byte(data[i]);
		if (!IIC_Wait_Ack()) {
			IIC_Stop();
			return 0;
		}
	}
	IIC_Stop();
	return 0;
}

int i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	if (!IIC_Start()) {
		return 1;
	}
	IIC_Send_Byte(addr << 1);
	if (!IIC_Wait_Ack()) {
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte((addr << 1) + 1);
	IIC_Wait_Ack();
	while (len) {
		if (len == 1) {
			*buf = IIC_Read_Byte(0);
		} else {
			*buf = IIC_Read_Byte(1);
		}
		buf++;
		len--;
	}
	IIC_Stop();
	return 0;
}

u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;

	SDA_IN();
	for (i = 0; i < 8; i++) {
		IIC_SCL = 0;
		delay_us(2);
		IIC_SCL = 1;
		receive <<= 1;
		if (READ_SDA) {
			receive++;
		}
		delay_us(2);
	}
	if (ack) {
		IIC_Ack();
	} else {
		IIC_NAck();
	}
	return receive;
}

unsigned char I2C_ReadOneByte(unsigned char I2C_Addr, unsigned char addr)
{
	unsigned char res = 0;

	IIC_Start();
	IIC_Send_Byte(I2C_Addr);
	res++;
	IIC_Wait_Ack();
	IIC_Send_Byte(addr); res++;
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(I2C_Addr + 1); res++;
	IIC_Wait_Ack();
	res = IIC_Read_Byte(0);
	IIC_Stop();

	return res;
}

u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data)
{
	u8 count = 0;

	IIC_Start();
	IIC_Send_Byte(dev);
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(dev + 1);
	IIC_Wait_Ack();

	for (count = 0; count < length; count++) {

		if (count != length - 1) {
			data[count] = IIC_Read_Byte(1);
		} else {
			data[count] = IIC_Read_Byte(0);
		}
	}
	IIC_Stop();
	return count;
}

u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8 *data)
{

	u8 count = 0;

	IIC_Start();
	IIC_Send_Byte(dev);
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	for (count = 0; count < length; count++) {
		IIC_Send_Byte(data[count]);
		IIC_Wait_Ack();
	}
	IIC_Stop();

	return 1;
}

u8 IICreadByte(u8 dev, u8 reg, u8 *data)
{
	*data = I2C_ReadOneByte(dev, reg);
	return 1;
}

unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
{
	return IICwriteBytes(dev, reg, 1, &data);
}

u8 IICwriteBits(u8 dev, u8 reg, u8 bitStart, u8 length, u8 data)
{

	u8 b;

	if (IICreadByte(dev, reg, &b) != 0) {
		u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
		data <<= (8 - length);
		data >>= (7 - bitStart);
		b &= mask;
		b |= data;
		return IICwriteByte(dev, reg, b);
	} else {
		return 0;
	}
}

u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
{
	u8 b;

	IICreadByte(dev, reg, &b);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	return IICwriteByte(dev, reg, b);
}

#ifndef __IOI2C_H
#define __IOI2C_H
#include "stm32f10x.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))


#define GPIOA_ODR_Addr    (GPIOA_BASE + 12)
#define GPIOB_ODR_Addr    (GPIOB_BASE + 12)
#define GPIOC_ODR_Addr    (GPIOC_BASE + 12)
#define GPIOD_ODR_Addr    (GPIOD_BASE + 12)
#define GPIOE_ODR_Addr    (GPIOE_BASE + 12)
#define GPIOF_ODR_Addr    (GPIOF_BASE + 12)
#define GPIOG_ODR_Addr    (GPIOG_BASE + 12)

#define GPIOA_IDR_Addr    (GPIOA_BASE + 8)
#define GPIOB_IDR_Addr    (GPIOB_BASE + 8)
#define GPIOC_IDR_Addr    (GPIOC_BASE + 8)
#define GPIOD_IDR_Addr    (GPIOD_BASE + 8)
#define GPIOE_IDR_Addr    (GPIOE_BASE + 8)
#define GPIOF_IDR_Addr    (GPIOF_BASE + 8)
#define GPIOG_IDR_Addr    (GPIOG_BASE + 8)

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr, n)
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr, n)

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr, n)
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr, n)

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr, n)
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr, n)

#define SDA_IN()  { GPIOB->CRH &= 0XFFFF0FFF; GPIOB->CRH |= 8 << 12; }
#define SDA_OUT() { GPIOB->CRH &= 0XFFFF0FFF; GPIOB->CRH |= 3 << 12; }

#define IIC_SCL    PBout(10)
#define IIC_SDA    PBout(11)
#define READ_SDA   PBin(11)

void IIC_Init(void);
int IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(unsigned char ack);
int IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);

void IIC_Write_One_Byte(u8 daddr, u8 addr, u8 data);
u8 IIC_Read_One_Byte(u8 daddr, u8 addr);
unsigned char I2C_Readkey(unsigned char I2C_Addr);

unsigned char I2C_ReadOneByte(unsigned char I2C_Addr, unsigned char addr);
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data);
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8 *data);
u8 IICwriteBits(u8 dev, u8 reg, u8 bitStart, u8 length, u8 data);
u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data);
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data);

int i2cWrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data);
int i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);

#endif

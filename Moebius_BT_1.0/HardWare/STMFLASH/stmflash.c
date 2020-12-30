#include "stmflash.h"
#include "delay.h"
#include "stmflash.h"
#define FLASH_SAVE_ADDR  0X0800E000

void STMFLASH_Unlock(void)
{
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
}

void STMFLASH_Lock(void)
{
	FLASH->CR |= 1 << 7;
}

u8 STMFLASH_GetStatus(void)
{
	u32 res;

	res = FLASH->SR;
	if (res & (1 << 0)) {
		return 1;
	} else if (res & (1 << 2)) {
		return 2;
	} else if (res & (1 << 4)) {
		return 3;
	}
	return 0;
}

u8 STMFLASH_WaitDone(u16 time)
{
	u8 res;

	do {
		res = STMFLASH_GetStatus();
		if (res != 1) {
			break;
		}
		delay_us(1);
		time--;
	} while (time);
	if (time == 0) {
		res = 0xff;       // TIMEOUT
	}
	return res;
}

u8 STMFLASH_ErasePage(u32 paddr)
{
	u8 res = 0;

	res = STMFLASH_WaitDone(0X5FFF);
	if (res == 0) {
		FLASH->CR |= 1 << 1;
		FLASH->AR = paddr;
		FLASH->CR |= 1 << 6;
		res = STMFLASH_WaitDone(0X5FFF);
		if (res != 1) {
			FLASH->CR &= ~(1 << 1);
		}
	}
	return res;
}

u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat)
{
	u8 res;

	res = STMFLASH_WaitDone(0XFF);
	if (res == 0) {
		FLASH->CR |= 1 << 0;
		*(vu16 *)faddr = dat;
		res = STMFLASH_WaitDone(0XFF);
		if (res != 1) {
			FLASH->CR &= ~(1 << 0);
		}
	}
	return res;
}

u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16 *)faddr;
}
#if STM32_FLASH_WREN

void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
	u16 i;

	for (i = 0; i < NumToWrite; i++) {
		STMFLASH_WriteHalfWord(WriteAddr, pBuffer[i]);
		WriteAddr += 2;
	}
}

#if STM32_FLASH_SIZE < 256
#define STM_SECTOR_SIZE 1024
#else
#define STM_SECTOR_SIZE 2048
#endif
u16 STMFLASH_BUF[STM_SECTOR_SIZE / 2];
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
	u32 secpos;
	u16 secoff;
	u16 secremain;
	u16 i;
	u32 offaddr;

	if (WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE))) {
		return;
	}
	STMFLASH_Unlock();
	offaddr = WriteAddr - STM32_FLASH_BASE;
	secpos = offaddr / STM_SECTOR_SIZE;
	secoff = (offaddr % STM_SECTOR_SIZE) / 2;
	secremain = STM_SECTOR_SIZE / 2 - secoff;
	if (NumToWrite <= secremain) {
		secremain = NumToWrite;
	}
	while (1) {
		STMFLASH_Read(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);
		for (i = 0; i < secremain; i++) {
			if (STMFLASH_BUF[secoff + i] != 0XFFFF) {
				break;
			}
		}
		if (i < secremain) {
			STMFLASH_ErasePage(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE);
			for (i = 0; i < secremain; i++) {
				STMFLASH_BUF[i + secoff] = pBuffer[i];
			}
			STMFLASH_Write_NoCheck(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);
		} else {
			STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);
		}
		if (NumToWrite == secremain) {
			break;
		} else {
			secpos++;
			secoff = 0;
			pBuffer += secremain;
			WriteAddr += secremain * 2;
			NumToWrite -= secremain;
			if (NumToWrite > (STM_SECTOR_SIZE / 2)) {
				secremain = STM_SECTOR_SIZE / 2;
			} else {
				secremain = NumToWrite;
			}
		}
	}
	;
	STMFLASH_Lock();
}
#endif

void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)
{
	u16 i;

	for (i = 0; i < NumToRead; i++) {
		pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);
		ReadAddr += 2;
	}
}

void Test_Write(u32 WriteAddr, u16 WriteData)
{
	STMFLASH_Write(WriteAddr, &WriteData, 1);
}

void Flash_Read(void)
{
	STMFLASH_Read(FLASH_SAVE_ADDR, (u16 *)PID_Parameter, 10);
	if (PID_Parameter[0] == 65535 && PID_Parameter[1] == 65535 && PID_Parameter[2] == 65535 && PID_Parameter[3] == 65535 && PID_Parameter[4] == 65535 && PID_Parameter[5] == 65535 && PID_Parameter[6] == 65535) {
		RC_Velocity = 30;
		RC_Position = 3000;
		Position_KP = 20;
		Position_KI = 0;
		Position_KD = 200;
		Velocity_KP = 12;
		Velocity_KI = 12;
	} else   {
		RC_Velocity = PID_Parameter[0];
		RC_Position = PID_Parameter[1];
		Position_KP = PID_Parameter[2];
		Position_KI = PID_Parameter[3];
		Position_KD = PID_Parameter[4];
		Velocity_KP = PID_Parameter[5];
		Velocity_KI = PID_Parameter[6];
	}
}

void Flash_Write(void)
{
	Flash_Parameter[0] = RC_Velocity;
	Flash_Parameter[1] = RC_Position;
	Flash_Parameter[2] = Position_KP;
	Flash_Parameter[3] = Position_KI;
	Flash_Parameter[4] = Position_KD;
	Flash_Parameter[5] = Velocity_KP;
	Flash_Parameter[6] = Velocity_KI;
	STMFLASH_Write(FLASH_SAVE_ADDR, (u16 *)Flash_Parameter, 10);
}

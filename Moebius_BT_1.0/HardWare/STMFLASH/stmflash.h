#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"

#define STM32_FLASH_SIZE        64
#define STM32_FLASH_WREN        1

#define STM32_FLASH_BASE 0x08000000

#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB
void STMFLASH_Unlock(void);
void STMFLASH_Lock(void);
u8 STMFLASH_GetStatus(void);
u8 STMFLASH_WaitDone(u16 time);
u8 STMFLASH_ErasePage(u32 paddr);
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat);
u16 STMFLASH_ReadHalfWord(u32 faddr);
void STMFLASH_WriteLenByte(u32 WriteAddr, u32 DataToWrite, u16 Len);
u32 STMFLASH_ReadLenByte(u32 ReadAddr, u16 Len);
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead);

void Test_Write(u32 WriteAddr, u16 WriteData);

void Flash_Read(void);
void Flash_Write(void);
#endif

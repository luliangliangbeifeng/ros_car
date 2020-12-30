#include "CAN.h"
#include "led.h"
#include "delay.h"
#include "usart.h"

u8 CAN1_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)
{
	u16 i = 0;

	if (tsjw == 0 || tbs2 == 0 || tbs1 == 0 || brp == 0) {
		return 1;
	}
	tsjw -= 1;
	tbs2 -= 1;
	tbs1 -= 1;
	brp -= 1;

	RCC->APB2ENR |= 1 << 0;
	RCC->APB2ENR |= 1 << 3;
	RCC->APB1ENR |= 1 << 25;

	GPIOB->CRH &= 0XFFFFFF00;
	GPIOB->CRH |= 0X000000B8;
	GPIOB->ODR |= 3 << 8;
	AFIO->MAPR |= 2 << 13;

	CAN1->MCR = 0x0000;
	CAN1->MCR |= 1 << 0;
	while ((CAN1->MSR & 1 << 0) == 0) {
		i++;
		if (i > 100) {
			return 2;
		}
	}
	CAN1->MCR |= 0 << 7;
	CAN1->MCR |= 0 << 6;
	CAN1->MCR |= 0 << 5;
	CAN1->MCR |= 1 << 4;
	CAN1->MCR |= 0 << 3;
	CAN1->MCR |= 0 << 2;
	CAN1->BTR = 0x00000000;
	CAN1->BTR |= mode << 30;
	CAN1->BTR |= tsjw << 24;
	CAN1->BTR |= tbs2 << 20;
	CAN1->BTR |= tbs1 << 16;
	CAN1->BTR |= brp << 0;
	CAN1->MCR &= ~(1 << 0);
	while ((CAN1->MSR & 1 << 0) == 1) {
		i++;
		if (i > 0XFFF0) {
			return 3;
		}
	}

	CAN1->FMR |= 1 << 0;
	CAN1->FA1R &= ~(1 << 0);
	CAN1->FS1R |= 1 << 0;
	CAN1->FM1R |= 0 << 0;
	CAN1->FFA1R |= 0 << 0;
	CAN1->sFilterRegister[0].FR1 = 0X00000000;
	CAN1->sFilterRegister[0].FR2 = 0X00000000;
	CAN1->FA1R |= 1 << 0;
	CAN1->FMR &= 0 << 0;

#if CAN1_RX0_INT_ENABLE
	CAN1->IER |= 1 << 1;
	MY_NVIC_Init(1, 0, USB_LP_CAN1_RX0_IRQn, 2);
#endif
	return 0;
}

u8 CAN1_Tx_Msg(u32 id, u8 ide, u8 rtr, u8 len, u8 *dat)
{
	u8 mbox;

	if (CAN1->TSR & (1 << 26)) {
		mbox = 0;
	} else if (CAN1->TSR & (1 << 27)) {
		mbox = 1;
	} else if (CAN1->TSR & (1 << 28)) {
		mbox = 2;
	} else {
		return 0XFF;
	}
	CAN1->sTxMailBox[mbox].TIR = 0;
	if (ide == 0) {
		id &= 0x7ff;
		id <<= 21;
	} else {
		id &= 0X1FFFFFFF;
		id <<= 3;
	}
	CAN1->sTxMailBox[mbox].TIR |= id;
	CAN1->sTxMailBox[mbox].TIR |= ide << 2;
	CAN1->sTxMailBox[mbox].TIR |= rtr << 1;
	len &= 0X0F;
	CAN1->sTxMailBox[mbox].TDTR &= ~(0X0000000F);
	CAN1->sTxMailBox[mbox].TDTR |= len;
	CAN1->sTxMailBox[mbox].TDHR = (((u32)dat[7] << 24) |
				       ((u32)dat[6] << 16) |
				       ((u32)dat[5] << 8) |
				       ((u32)dat[4]));
	CAN1->sTxMailBox[mbox].TDLR = (((u32)dat[3] << 24) |
				       ((u32)dat[2] << 16) |
				       ((u32)dat[1] << 8) |
				       ((u32)dat[0]));
	CAN1->sTxMailBox[mbox].TIR |= 1 << 0;
	return mbox;
}

u8 CAN1_Tx_Staus(u8 mbox)
{
	u8 sta = 0;

	switch (mbox) {
	case 0:
		sta |= CAN1->TSR & (1 << 0);            // RQCP0
		sta |= CAN1->TSR & (1 << 1);            // TXOK0
		sta |= ((CAN1->TSR & (1 << 26)) >> 24); // TME0
		break;
	case 1:
		sta |= CAN1->TSR & (1 << 8) >> 8;       // RQCP1
		sta |= CAN1->TSR & (1 << 9) >> 8;       // TXOK1
		sta |= ((CAN1->TSR & (1 << 27)) >> 25); // TME1
		break;
	case 2:
		sta |= CAN1->TSR & (1 << 16) >> 16;     // RQCP2
		sta |= CAN1->TSR & (1 << 17) >> 16;     // TXOK2
		sta |= ((CAN1->TSR & (1 << 28)) >> 26); // TME2
		break;
	default:
		sta = 0X05; // ÓÊÏäºÅ²»¶Ô,¿Ï¶¨Ê§°Ü.
		break;
	}
	return sta;
}

u8 CAN1_Msg_Pend(u8 fifox)
{
	if (fifox == 0) {
		return CAN1->RF0R & 0x03;
	} else if (fifox == 1) {
		return CAN1->RF1R & 0x03;
	} else {
		return 0;
	}
}

void CAN1_Rx_Msg(u8 fifox, u32 *id, u8 *ide, u8 *rtr, u8 *len, u8 *dat)
{
	*ide = CAN1->sFIFOMailBox[fifox].RIR & 0x04;
	if (*ide == 0) {
		*id = CAN1->sFIFOMailBox[fifox].RIR >> 21;
	} else {
		*id = CAN1->sFIFOMailBox[fifox].RIR >> 3;
	}
	*rtr = CAN1->sFIFOMailBox[fifox].RIR & 0x02;
	*len = CAN1->sFIFOMailBox[fifox].RDTR & 0x0F;

	dat[0] = CAN1->sFIFOMailBox[fifox].RDLR & 0XFF;
	dat[1] = (CAN1->sFIFOMailBox[fifox].RDLR >> 8) & 0XFF;
	dat[2] = (CAN1->sFIFOMailBox[fifox].RDLR >> 16) & 0XFF;
	dat[3] = (CAN1->sFIFOMailBox[fifox].RDLR >> 24) & 0XFF;
	dat[4] = CAN1->sFIFOMailBox[fifox].RDHR & 0XFF;
	dat[5] = (CAN1->sFIFOMailBox[fifox].RDHR >> 8) & 0XFF;
	dat[6] = (CAN1->sFIFOMailBox[fifox].RDHR >> 16) & 0XFF;
	dat[7] = (CAN1->sFIFOMailBox[fifox].RDHR >> 24) & 0XFF;
	if (fifox == 0) {
		CAN1->RF0R |= 0X20;
	} else if (fifox == 1) {
		CAN1->RF1R |= 0X20;
	}
}

#if CAN1_RX0_INT_ENABLE

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	u8 i;
	u32 id;
	u8 ide, rtr, len;
	u8 ON_rxbuf[8] = { 10, 12, 15, 19, 24, 30, 37, 0 };

	u8 temp_rxbuf[8];

	CAN1_Rx_Msg(0, &id, &ide, &rtr, &len, temp_rxbuf);
	for (i = 0; i <= 7; i++) {
		if (i == 7) {
			CAN_ON_Flag = temp_rxbuf[i];
			break;
		}
		if (temp_rxbuf[i] != ON_rxbuf[i]) {
			break;
		}
	}
	if (id == 0x121 && CAN_ON_Flag) {
		memcpy(rxbuf, temp_rxbuf, 8 * sizeof(u8));
	}
}
#endif

u8 CAN1_Send_Msg(u8 *msg, u8 len)
{
	u8 mbox;
	u16 i = 0;

	mbox = CAN1_Tx_Msg(0X601, 0, 0, len, msg);
	while ((CAN1_Tx_Staus(mbox) != 0X07) && (i < 0XFFF)) i++;
	if (i >= 0XFFF) {
		return 1;
	}
	return 0;
}

u8 CAN1_Receive_Msg(u8 *buf)
{
	u32 id;
	u8 ide, rtr, len;

	if (CAN1_Msg_Pend(0) == 0) {
		return 0;
	}
	CAN1_Rx_Msg(0, &id, &ide, &rtr, &len, buf);
	if (id != 0x12 || ide != 0 || rtr != 0) {
		len = 0;
	}
	return len;
}


u8 CAN1_Send_MsgTEST(u8 *msg, u8 len)
{
	u8 mbox;
	u16 i = 0;

	mbox = CAN1_Tx_Msg(0X701, 0, 0, len, msg);
	while ((CAN1_Tx_Staus(mbox) != 0X07) && (i < 0XFFF)) i++;
	if (i >= 0XFFF) {
		return 1;
	}
	return 0;
}

u8 CAN1_Send_Num(u32 id, u8 *msg)
{
	u8 mbox;
	u16 i = 0;

	mbox = CAN1_Tx_Msg(id, 0, 0, 8, msg);
	while ((CAN1_Tx_Staus(mbox) != 0X07) && (i < 0XFFF)) i++;
	if (i >= 0XFFF) {
		return 1;
	}
	return 0;
}

void CAN1_SEND(void)
{
	u8 Direction_A, Direction_B, Direction_C, Direction_D;
	u16 Temp_Pitch, Temp_Roll, Temp_Yaw, Temp_GZ;
	static u8 Flag_Send;

	if (Encoder_A > 0) {
		Direction_A = 0;
	} else if (Encoder_A < 0) {
		Direction_A = 2;
	} else {
		Direction_A = 1;
	}

	if (Encoder_B > 0) {
		Direction_B = 0;
	} else if (Encoder_B < 0) {
		Direction_B = 2;
	} else {
		Direction_B = 1;
	}

	if (Encoder_C > 0) {
		Direction_C = 0;
	} else if (Encoder_C < 0) {
		Direction_C = 2;
	} else {
		Direction_C = 1;
	}

	if (Encoder_D > 0) {
		Direction_D = 0;
	} else if (Encoder_D < 0) {
		Direction_D = 2;
	} else {
		Direction_D = 1;
	}

	Temp_Pitch = Pitch * 100 + 30000;
	Temp_Roll =  Roll * 100 + 30000;
	Temp_Yaw =    Yaw * 100 + 30000;
	Temp_GZ = gyro[2] + 32768;
	Flag_Send = !Flag_Send;
	if (Flag_Send == 0) {
		txbuf[0] = Temp_Pitch >> 8;
		txbuf[1] = Temp_Pitch & 0x00ff;
		txbuf[2] = Temp_Roll >> 8;
		txbuf[3] = Temp_Roll & 0x00ff;
		txbuf[4] = Temp_Yaw >> 8;
		txbuf[5] = Temp_Yaw & 0x00ff;
		txbuf[6] = Temp_GZ >> 8;
		txbuf[7] = Temp_GZ & 0x00ff;
		CAN1_Send_Num(0x100, txbuf);
	} else {
		txbuf[0] = abs(Encoder_A);
		txbuf[1] = Direction_A;
		txbuf[2] = abs(Encoder_B);
		txbuf[3] = Direction_B;
		txbuf[4] = abs(Encoder_C);
		txbuf[5] = Direction_C;
		txbuf[6] = abs(Encoder_D);
		txbuf[7] = Direction_D;
		CAN1_Send_Num(0x101, txbuf);
	}
}

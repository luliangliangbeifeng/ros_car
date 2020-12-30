#ifndef __CAN_H
#define __CAN_H
#include "sys.h"

#define CAN1_RX0_INT_ENABLE     1

u8 CAN1_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode);
u8 CAN1_Tx_Msg(u32 id, u8 ide, u8 rtr, u8 len, u8 *dat);
u8 CAN1_Msg_Pend(u8 fifox);
void CAN1_Rx_Msg(u8 fifox, u32 *id, u8 *ide, u8 *rtr, u8 *len, u8 *dat);
u8 CAN1_Tx_Staus(u8 mbox);
u8 CAN1_Send_Msg(u8 *msg, u8 len);
u8 CAN1_Receive_Msg(u8 *buf);

u8 CAN1_Send_MsgTEST(u8 *msg, u8 len);
u8 CAN1_Send_Num(u32 id, u8 *msg);
void CAN1_SEND(void);
#endif

#ifndef __EXTERN_CAN_H
#define __EXTERN_CAN_H

#include "sys.h"
#include "faultalarm.h"


//外部变量声明
extern uint8_t MODE;  //用于声明本系统在对外CAN通信中的运行模式(0:BMS/电动汽车模式,1:充电机模式)



//函数声明
void BMS_Enable(void);
void Charge_Enable(void);
void Extern_CAN_Init(void);
void Judge_Msg();
uint8_t GetExternCanFlag(void);
void SetExternCanFlag(uint8_t flag);

void CAN2_RX1_IRQHandler(void);  //CAN2中断服务函数	

uint8_t Extern_CAN_SendMsg(uint32_t stdId, uint32_t extId, uint8_t *buf, uint8_t len);
uint8_t Extern_CAN_ReceiveMsg(uint32_t *stdId, uint32_t *extId, uint8_t *buf);





#endif

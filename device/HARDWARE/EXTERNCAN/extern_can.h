#ifndef __EXTERN_CAN_H
#define __EXTERN_CAN_H

#include "sys.h"
#include "faultalarm.h"


//�ⲿ��������
extern uint8_t MODE;  //����������ϵͳ�ڶ���CANͨ���е�����ģʽ(0:BMS/�綯����ģʽ,1:����ģʽ)



//��������
void BMS_Enable(void);
void Charge_Enable(void);
void Extern_CAN_Init(void);
void Judge_Msg();
uint8_t GetExternCanFlag(void);
void SetExternCanFlag(uint8_t flag);

void CAN2_RX1_IRQHandler(void);  //CAN2�жϷ�����	

uint8_t Extern_CAN_SendMsg(uint32_t stdId, uint32_t extId, uint8_t *buf, uint8_t len);
uint8_t Extern_CAN_ReceiveMsg(uint32_t *stdId, uint32_t *extId, uint8_t *buf);





#endif

#ifndef __RS485_INSULATION_H
#define __RS485_INSULATION_H

#include "stm32f4xx.h"

/*
��Ҫʵ�ֹ���:
(1)���Ե���ģ�鷢�Ϳ����������֡,���ú����з���������֡����6s�Ķ�ʱ���ж�;
(2)���жϷ�������,��ģ�鷢�Ͷ���������֡,ʹ�ô����жϽ�������,ʶ���Ѿ���������ݺ�, ����ȡ���ݱ�����3��������,���á�ĳ����������;���û�м���������ʱ100ms�ظ�����;
(3)

*/


//485ת��ģ���������CON����ָ��,ONΪ����ģʽ��OFFΪ����ģʽ
#define RS485_RE3_ON  GPIO_SetBits(GPIOD,GPIO_Pin_1)  //BCU�ľ�Ե���ģ��ͨ��485ģ���������CONΪPD6
#define RS485_RE3_OFF GPIO_ResetBits(GPIOD,GPIO_Pin_1)

#define RS4853_REC_LEN 20  //RS485_Insulation���ջ������10�ֽ�(ÿ�������������֡��ֻ��10�ֽ�)

extern uint8_t RS4853_RX_BUF[RS4853_REC_LEN];  //RS485_Insulation���ջ�����
extern uint8_t RS4853_RX_STA;  //RS485_Insulation����״̬���
extern uint16_t current_data;
extern uint16_t res_pos_data;
extern uint16_t res_neg_data;



//��������
void RS485_Insulation_Init(u32 bound);  //��Ե���ģ��485ͨ�ų�ʼ��
void RS485_Insulation_OpenRead(void);
void RS485_Insulation_ReadData(void);






























#endif
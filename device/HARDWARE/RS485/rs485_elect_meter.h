#ifndef __RS485_ELECT_METER_H
#define __RS485_ELECT_METER_H

#include "stm32f4xx.h"

/*
	˵��:
	BMS����ϵͳ�У������BCUͨ��RS485���п��ƣ�
	�����ϣ�RS485�ź�ͨ��STM32F413��USART2���ھ�ת��ģ��õ���
	��ˣ�����ϵͳ����Ľ������̼�����ʵ�ֶ���ͨ������USART2ʵ�֣�
*/

//485ת��ģ���������CON����ָ��,ONΪ����ģʽ��OFFΪ����ģʽ
#define RS485_RE2_ON  GPIO_SetBits(GPIOD,GPIO_Pin_6)  //BCU�ĵ��ͨ��485ģ���������CONΪPD6
#define RS485_RE2_OFF GPIO_ResetBits(GPIOD,GPIO_Pin_6)

#define RS4852_REC_LEN 100  //RS485_ElectMeter��������ֽ���

//���ͨ�Ź�����,��Ϣ֡�п������ʽ����
#define MASTER_SEND_FLAG       0x00
#define SLAVE_RESP_FLAG        0x80
#define SLAVE_CRT_FLAG         0x00
//#define SLAVE_CRT_FLAG         0x40
#define NO_SUBSEQ_DATA_FLAG    0x00
#define SUBSEQ_DATA_FLAG       0x20
//�������е�5λ������Ӧ������
#define READ_DATA_FUNC         0x11
#define READ_SUBSEQ_DATA_FUNC  0x12
#define READ_ADDR_FUNC         0x13
#define WRITE_DATA_FUNC        0x14
#define WRITE_ADDR_FUNC        0x15
//������ʱ�ò���


//�������ݱ�ʶ(DI3 DI2 DI1 DI0)
//static const u8 POWER_4BYTES[4] = {0x02,0x00,0x00,0x00};      //4�ֽ����� xxxx.xxxx,��λkW,����,���λΪ����λ
static const u8 CURRENT_4BYTES[4] = {0x00,0x01,0x00,0x02};    //4�ֽ����� xxxx.xxxx,��λA,����,���λΪ����λ
//static const u8 VOLTAGE_4BYTES[4] = {0x02,0x00,0x02,0x00};    //4�ֽ����� xxxx.xxxx,��λV,��ѹ,���λΪ����λ
//static const u8 VOLTAGE_2BYTES[4] = {0x02,0x01,0x01,0x00};    //2�ֽ����� xxx.x,��λV,��ѹ
//static const u8 VOLTAGE_BLOCK[4] = {0x02,0x01,0xFF,0x00};     //6�ֽ�����, ��λV, ��ѹ���ݿ�
//static const u8 CURRENT_3BYTES[4] = {0x02,0x02,0x01,0x00};    //3�ֽ����� xxx.xxx, ��λA, ����
//static const u8 CURRENT_BLOCK[4] = {0x02,0x02,0xFF,0x00};     //9�ֽ�����, ��λA, �������ݿ�
//������ʱ�ò���


//��������
void RS485_ElectMeter_Init(u32 bound);
void RS485_ElectMeter_SendData(const uint8_t *data, uint8_t len);
void RS485_ElectMeter_SendFrame(u8 *addr,u8 ctrlcode,u8 len,u8 *data);
void RS485_ElectMeter_UpdateAddr(void);
u8 RS485_ElectMeter_ReadCurrentData(void);
void RS485_ElectMeter_ReadData(void);
void RS485_ElectMeter_ReadSubseqData(void);

























#endif


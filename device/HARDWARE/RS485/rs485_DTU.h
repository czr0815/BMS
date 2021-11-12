#ifndef __RS485_DTU_H
#define __RS485_DTU_H
#include "stm32f4xx.h"
#include "malloc.h"
#include "bms_state.h"

/*
	˵����
	BMS����ϵͳ�У�DTUģ����BCU/BMUͨ��RS485���п��ƣ�
	�����ϣ�RS485�ź�ͨ��STM32F413��USART1���ھ�ת��ģ��õ���
	��ˣ�����ϵͳ����ʾ���Ľ������̼�����ʵ�ֶ���ͨ������USART1ʵ�֣�
*/

//485ת��ģ���������CON����ָ��,ONΪ����ģʽ��OFFΪ����ģʽ
#define RS485_RE1_ON  GPIO_SetBits(GPIOG,GPIO_Pin_10)  //BCU��DTU485ͨ��ģ����շ���������ΪPG10
#define RS485_RE1_OFF GPIO_ResetBits(GPIOG,GPIO_Pin_10)
#define DTU_SEND_LEN 244 //���ɷ����ֽ���
#define DTU_TEMP_LEN 32
#define RS4851_REC_LEN 256  //RS485_DTU��������ֽ���

//����json��ʽ����ʱ���ַ�������
extern const char DEVICEBCU_15[15];
extern const char DEVICEBCU_15[15];
extern const char BMUID_9[9];
extern const char BMUID_9[9];
extern const char CELL_8[8];
extern const char ID_6[6];
extern const char VOL_6[6];
extern const char SOC_6[6];
extern const char TEMP_8[8];
extern const char VAL_6[6];
extern const char SMOKE_9[9];
extern const char CURR_7[7];
extern const char BCUID_9[9];
extern const char VOLT_8[8];
extern const char INSU_7[7];
extern const char CC_6[6];



void RS485_DTU_Init(u32 bound);  //DTU 485���ڳ�ʼ������
void RS485_DTU_SendData(const uint8_t *data, uint8_t len);  //DTU 485���ڷ��ͺ���


//��BCU�У��ϴ���������BMU���������
//��1���ϴ���������BMU��cell(��ѹ+SOC)
//��2���ϴ���������BMU���¶�����
//��3���ϴ���������BMU���������ݣ�����BMU�ĵ���������
void DTU_UploadBMUCell_InBCU(uint8_t bmu_id, uint8_t pos_s, uint8_t pos_e);
void DTU_UploadBMUTemp_InBCU(uint8_t bmu_id, uint8_t pos_s, uint8_t pos_e);
void DTU_UploadBMUData_InBCU(uint8_t bmu_id, uint8_t curr_pos, uint8_t smoke_pos_s, uint8_t smoke_pos_e);

//�ϴ�BCU�ռ���������,�����ܵ�ѹ����SOC���ܵ�������Ե��ϵͳ״̬��һ�����ϴ�
void DTU_UploadBCUData();

//ͨ��Json��ʽ�ϴ������ܹ���3�����ͣ�
//  ��1����оƬΪ��λ�ϴ���ص�ѹ����,һ��(���)һ���ϴ����ϴ�12����ص�ѹ��12��SOC״̬����(���ڵ�ش�,��ѹ��SOC״̬��)��
//  ��2����BMUΪ��λ�ϴ��¶ȴ������������,���һ�λ��ϴ�25���¶�����(����һ��BMU,�ж���һ�ξ��ϴ�����)��
//  ��3����BCUΪ��λ�ϴ�����BMU�ĵ�������(ͨ��BMU�����ͨ�ŵõ�)����Ե״̬��Ϣ(BCU)����ѹ�����(*3)(BCU)�������SOC״̬(BCU)������BMU��CC�ź�״̬��
u8 RS485_DTU_UploadVolData(u8 bmucount, u8 volnumbegin,u8 volnumend);    //ͨ��DTU�ϴ������ش���ѹ����
u8 RS485_DTU_UploadTempData(u8 bmucount, u8 tempnumbegin, u8 tempnumend);           //ͨ��DTU�ϴ��¶ȼ������
u8 RS485_DTU_UploadCurrentCCSmokeData(u8 bmucount, u8 currnum, u8 ccnum, u8 smokenum);    //ͨ��DTU�ϴ�����͵�������
#if IS_BCU_ROLE
u8 RS485_DTU_UploadBCUData(u8 currnum, u8 soctotalnum, u8 insunum, u8 voltnum);  //BCUͨ��DTU�ϴ�BCU���ȡ������(��ѹ������SOC״̬����Ե״̬)
#endif










#endif

#ifndef __RS485_SCREEN_H
#define __RS485_SCREEN_H
#include "stm32f4xx.h"

/*
	˵����
	BMS����ϵͳ�У���ʾ����BCUͨ��RS485���п��ƣ�
	�����ϣ�RS485�ź�ͨ��STM32F413��USART6���ھ�ת��ģ��õ���
	��ˣ�����ϵͳ����ʾ���Ľ������̼�����ʵ�ֶ���ͨ������USART6ʵ�֣�
*/

//485ת��ģ���������CON����ָ��,ONΪ����ģʽ��OFFΪ����ģʽ
//#define RS485_RE4_ON  GPIO_SetBits(GPIOG,GPIO_Pin_7)  //BCU����ʾ��485ͨ��ģ����շ���������ΪPG7
//#define RS485_RE4_OFF GPIO_ResetBits(GPIOG,GPIO_Pin_7)
#define RS485_RE4_ON  GPIO_SetBits(GPIOG,GPIO_Pin_10)  //BCU����ʾ��485ͨ��ģ����շ���������ΪPG7
#define RS485_RE4_OFF GPIO_ResetBits(GPIOG,GPIO_Pin_10)

#define RS4856_REC_LEN 64  //RS485_Screen��������ֽ���

//�����������洢�����ü���ַ
//����ʱ��: 2021-09-14 19:00
//BMU������ʾ
//BCU��ҳ��Ϣ
#define  TOTAL_SOC_ADDR        0x1000  //����SOC�ٷֱ�(2�ֽ�����)
#define  TOTAL_VOL_ADDR        0x1002  //�ܵ�ѹ(2�ֽ�����)
#define  HIGHVOL_CELL_ADDR     0x1004  //��ߵ����ѹ(2�ֽ�����)
#define  HIGHVOL_CELL_ID_ADDR  0x1006  //��ߵ����ѹ��Ӧ������(2�ֽ�����)
#define  LOWVOL_CELL_ADDR      0x1008  //��͵����ѹ(2�ֽ�����)
#define  LOWVOL_CELL_ID_ADDR   0x100A  //��͵����ѹ��Ӧ������(2�ֽ�����)
#define  AVEG_VOL_ADDR         0x100C  //ƽ�������ѹ(2�ֽ�����)
#define  MAX_VOL_DIFF_ADDR     0x100E  //����֮������ѹ��(2�ֽ�����)
#define  TOTAL_CURR_ADDR       0x1010  //�ܵ���(2�ֽ�����)
#define  HIGHTEMP_ADDR         0x1012  //����¶�(2�ֽ�����)
#define  HIGHTEMP_ID_ADDR      0x1014  //����¶ȼ�����(2�ֽ�����)
#define  LOWTEMP_ADDR          0x1016  //����¶�(2�ֽ�����)
#define  LOWTEMP_ID_ADDR       0x1018  //����¶ȼ�����(2�ֽ�����)
#define  AVEG_TEMP_ADDR        0x101A  //ƽ���¶�(2�ֽ�����)
#define  MAX_TEMP_DIFF_ADDR    0x101C  //����²�(2�ֽ�����)
#define  POWER_VOL_ADDR        0x101E  //�����ѹ(2�ֽ�����)
#define  CHARGE_STATE_ADDR     0x1028  //���״̬(Ԥ������8�ֽ�)

//BMU��ҳ��Ϣ
#define  INSUSTATE_POS_ADDR    0x1030  //��Ե״̬1(Positive)(2�ֽ�����)
#define  INSUSTATE_NEG_ADDR    0x1032  //��Ե״̬2(Negative)(2�ֽ�����)
#define  CHARGER_OUTVOL_ADDR   0x1034  //���������ѹ(2�ֽ�����)
#define  CHARGER_OUTCURR_ADDR  0x1036  //�����������(2�ֽ�����)
#define  VOL_B+_ADDR           0x1038  //��ѹ: B+(2�ֽ�����)
#define  VOL_HV1_ADDR          0x103A  //��ѹ: HV1(2�ֽ�����)
#define  VOL_HV2_ADDR          0x103C  //��ѹ: HV2(2�ֽ�����)
#define  CC_STATE_ADDR         0x1040  //CC״̬(Ԥ������8�ֽ�)
#define  HEAT_COOL_STATE_ADDR  0x1048  //��������(Ԥ������8�ֽ�)
#define  ONLINE_STATE_ADDR     0x1040  //����״̬(Ԥ������8�ֽ�)
#define  RELAY1_STATE_ADDR     0x1058  //�̵���1״̬(Ԥ������8�ֽ�)
#define  RELAY2_STATE_ADDR     0x1060  //�̵���2״̬(Ԥ������8�ֽ�)
#define  RELAY3_STATE_ADDR     0x1068  //�̵���2״̬(Ԥ������8�ֽ�)

//��������
#define  SCREEN_SOCLOW           0x10A0
#define  SCREEN_CVOLTOTALHIGH    0x10A2
#define  SCREEN_DSCVOLTOTALLOW   0x10A4
#define  SCREEN_CVOLHIGH         0x10A6
#define  SCREEN_CVOLLOW          0x10A8
#define  SCREEN_DSCVOLHIGH       0x10AA
#define  SCREEN_DSCVOLLOW        0x10AC
#define  SCREEN_CVOLDIFF         0x10AE
#define  SCREEN_DSCVOLDIFF       0x10B0
#define  SCREEN_VOLWIRE          0x10B2
#define  SCREEN_CTEMPHIGH        0x10B4
#define  SCREEN_DSCTEMPHIGH      0x10B6
#define  SCREEN_CTEMPLOW         0x10B8
#define  SCREEN_DSCTEMPLOW       0x10BA
#define  SCREEN_CTEMPDIFF        0x10BC
#define  SCREEN_DSCTEMPDIFF      0x10BE
#define  SCREEN_COVERCURR        0x10C0
#define  SCREEN_DSCOVERCURR      0x10C2
#define  SCREEN_INSULEAK         0x10C4
#define  SCREEN_TEMPRISE         0x10C6

#define  SCREEN_ALARM_GREEN      0
#define  SCREEN_ALARM_YELLOW     1
#define  SCREEN_ALARM_ORANGE     2
#define  SCREEN_ALARM_RED        3
//......

//��ش������ѹ���¶ȼ�������
#define  VOLCARD1_VOL1_ADDR    0x1100  //��ѹ�ɼ���1  ��ѹ����1 (��24��)
#define  VOLCARD1_TEMP1_ADDR   0x1130  //��ѹ�ɼ���1  �¶�����1 (��10��)
#define  VOLCARD2_VOL1_ADDR    0x1150  //��ѹ�ɼ���2  ��ѹ����1 (��24��)
#define  VOLCARD2_TEMP1_ADDR   0x1180  //��ѹ�ɼ���2  �¶�����1 (��10��)
#define  VOLCARD3_VOL1_ADDR    0x11A0  //��ѹ�ɼ���3  ��ѹ����1 (��24��)
#define  VOLCARD3_TEMP1_ADDR   0x11D0  //��ѹ�ɼ���3  �¶�����1 (��10��)
#define  VOLCARD4_VOL1_ADDR    0x1200  //��ѹ�ɼ���4  ��ѹ����1 (��24��)
#define  VOLCARD4_TEMP1_ADDR   0x1230  //��ѹ�ɼ���4  �¶�����1 (��10��)
#define  VOLCARD5_VOL1_ADDR    0x1250  //��ѹ�ɼ���5  ��ѹ����1 (��24��)
#define  VOLCARD5_TEMP1_ADDR   0x1280  //��ѹ�ɼ���5  �¶�����1 (��10��)
#define  VOLCARD6_VOL1_ADDR    0x12A0  //��ѹ�ɼ���6  ��ѹ����1 (��24��)
#define  VOLCARD6_TEMP1_ADDR   0x12D0  //��ѹ�ɼ���6  �¶�����1 (��10��)
#define  VOLCARD7_VOL1_ADDR    0x1300  //��ѹ�ɼ���7  ��ѹ����1 (��24��)
#define  VOLCARD7_TEMP1_ADDR   0x1330  //��ѹ�ɼ���7  �¶�����1 (��10��)
#define  VOLCARD8_VOL1_ADDR    0x1350  //��ѹ�ɼ���8  ��ѹ����1 (��24��)
#define  VOLCARD8_TEMP1_ADDR   0x1380  //��ѹ�ɼ���8  �¶�����1 (��10��)
#define  VOLCARD9_VOL1_ADDR    0x13A0  //��ѹ�ɼ���9  ��ѹ����1 (��24��)
#define  VOLCARD9_TEMP1_ADDR   0x13D0  //��ѹ�ɼ���9  �¶�����1 (��10��)
#define  VOLCARD10_VOL1_ADDR   0x1400  //��ѹ�ɼ���10 ��ѹ����1 (��24��)
#define  VOLCARD10_TEMP1_ADDR  0x1430  //��ѹ�ɼ���10 �¶�����1 (��10��)



extern uint8_t RS4856_RX_BUF[RS4856_REC_LEN];  //RS485_Screen���ջ�����
extern uint8_t RS4856_RX_LEN;  
extern uint8_t RS4856_RX_STA;  //RS485_Screen����״̬���

void RS485_Screen_Init(uint32_t bound);
void RS485_Screen_SendData(const uint8_t *data, uint8_t len);
void RS485_Screen_ReadData(uint8_t *data, uint8_t *len);

//˵��: DGUS���ļĴ��������ֽ�(Byte)Ѱַ, �����洢��������(Word)(Ѱַ
//��ָ���Ĵ�����ַд������
//void RS485_Screen_WriteReg(uint8_t addr, uint8_t data);
//��ָ���洢����ַд���������
void RS485_Screen_WriteMem(uint16_t addr, uint16_t data);
//��ȡָ���Ĵ�����ַ��ʼ��ָ���ֽڳ��ȵļĴ�������
//void RS485_Screen_ReadReg(uint8_t addr, uint8_t len);
//��ȡָ���Ĵ�����ַ��ʼ��ָ���ֽڳ��ȵı����洢������
void RS485_Screen_ReadMem(uint16_t addr, uint8_t len);

//��ȡָ����ش���ѹ���ݵ���ʾ����ַ
u16 get_celladdr(u16 cellnum);
//��ȡָ���¶ȼ��������
u16 get_tempaddr(u16 tempnum);


#endif





















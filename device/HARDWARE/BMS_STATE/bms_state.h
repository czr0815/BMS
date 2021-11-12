#ifndef __BMS_STATE_H
#define __BMS_STATE_H
/*
	˵����
	���ڶ���������������״̬������;
	�������ļ���ʹ�ø�����ʱ,��ʹ��extern�ؼ��ֽ����ⲿ��������
*/

#include <stdint.h>
#include "sys.h"

//�Ƿ�ʹ��ʵʱϵͳ: 1Ϊʹ��ϵͳ,0Ϊ��ʹ��ϵͳ
#define IS_USE_RTOS 1


//�����豸��BMU or BCU,    ע�⡾��ѡһ��
#define IS_BMU_ROLE 0    //ע�⡾��ѡһ��
#define IS_BCU_ROLE 1    //ע�⡾��ѡһ��
#define IS_BSU_ROLE 0    //ע�⡾��ѡһ��

#define MY_ID     0x10  //�趨��ÿ�����ص�8λID
//����BMU,ͨ��My_Pack_Number������My_Id��������һ��,Ҫ���0x01��ʼ,�����������
//�ݶ�:5��BMU�ĵ�ַ�ֱ�Ϊ0x01-0x05,BCU�ĵ�ַΪ0x10(1BCU+5BMU��������)
#define MY_DID    0x10

#define BMU_PRINT_P  1   //BMU�����й������Ƿ��ͨ�����ڵ��Կ����������Ϣ
#define BMU_SCREEN_P 0   //�Ƿ���BMU�������ѹ���¶���������ʾ������ʾ
#define BCU_PRINT_P  0   //BCU�����й������Ƿ��ͨ�����ڵ��Կ����������Ϣ

//��ز���
#define BATTERY_TYPE 0x03  //������﮵��
#define BATTERY_VOL_ST 35 //������ر�׼��ߵ�ѹ3.5V*10
#define BATTERY_VOL_BEAR 40  //������ؿɳ��ܵ���ߵ�ѹ4V*10
#define BATTERY_CUR_ST 100  //������ض����10A*10
#define BATTERY_CUR_BEAR 120  //������ؿɳ��ܵ�������12A*10
#define BATTERY_CAP_ST  100  //������ض����10Ah*10
#define BATTERY_ENE_ST 350  //������ض����35kWh*10
#define BATTERY_TEMP_BEAR 45  //�������¶�45��

//BCU�������
#define CELL_NUM 240  //���Ի�����237�����
#define BMU_VOLCHIP_NUM  4  //ÿһ��BMU�������ӵĵ�ѹ�ɼ�оƬ�����ֵ
#define BCU_TEMP_NUM 50  //ÿһ��BCU(/BMU)���ܲɼ����¶����ݵ��������
//BMU�������
#define BMU_VOL_NUM   48  //����BMU�����ͬʱ�����ĵ�ش���ѹ����
#define BMU_TEMP_NUM  12  //����BMU�����ͬʱ�������¶���������
#define BMU_SMOKE_NUM 2  //����BMU�����ͬʱ������������������

//CAN1�ڲ�ͨ����غ궨��
#define BMU_LIST_NUM     5     //�뱾BCU�����ӵ�BMU����
#define CAN1_CONNECT_TRYTIMES  100  //CAN1�ڲ�ͨ�����ӹ����г������Ӵ���
//CAN1�ڲ�ͨ����ر�������
extern u8 My_BMS_Role;             //���ڱ�ʶ�����BMS��ɫ(BMU/BCU)
extern u8 My_Id;                   //���ڱ�ʶ����ID�������ڲ�ͨ��
extern u32 My_Did;                //��ʾ�����豸ID�������ƶ��ϴ���ϵͳ����
extern u8 Is_Connected;            //���ڱ�ʶ���ϼ���ɫ�����ӹ�ϵ(����BMU��ʶ�Ƿ�������BCU,����BCU��ʶ�Ƿ�������BSU(����еĻ�))
extern u8 My_Superior_Id;	         //(�����ϼ���ɫ��������֮��)���ڱ�ʶ�ϼ���ID(����BMU��˵��BCU,����BCU��˵��BSU)
extern CanRxMsg CAN1RxMessage;              //���ڱ��洦��CAN1��ǰ���յ�����Ϣ֡
//BMU�ڲ�ͨ�Ŷ���
#if IS_BMU_ROLE
extern u8 My_Pack_Number;           //���ڱ�ʶBMU�������ĵ������,�ϴ�����ʱʹ�øñ���(��Χ:1��ʼ������)
extern volatile u8 bmu_uploaddata;  //���ڱ�ʶBMU�Ƿ��ϴ�����
#endif
#if IS_BCU_ROLE
extern volatile u16 bmucontroltask_delay;     //BCU�ڿ��������ϴ��ȹ��̵������е�ѭ����ʱʱ��
extern volatile u8 is_bmu_uploading;          //���ڱ�ʶĿǰBMU�Ƿ����ϴ�����
extern volatile u8 is_bmu_acked;              //���ڱ�ʶBMU�Ƿ���ϴ�����ظ�ȷ��
#endif

extern u8 BMU_List_Id[BMU_LIST_NUM];       //BMU�б�:BMU ID,����ϵͳ�ڲ�����
extern u32 BMU_List_Did[BMU_LIST_NUM];    //BMU�б��豸ID�������ϴ����ƶˣ���ϵͳ��ʹ��
extern u8 BMU_List_PackNum[BMU_LIST_NUM];  //BMU�б�:BMU����ĵ������
extern u8 BMU_List_State[BMU_LIST_NUM];    //BMU�б�:BMU״̬
extern u8 BMU_Connect_Num;         //��ʶBMU�б��е�BMU������


/* BMU��BCU��Ҫ���ֵ��������� */
//�����ѹ���¶�����,�Լ�SOC״̬����������
#if IS_BMU_ROLE
extern u16 dataacq_time;      //BMU��ȡ���ݵ�ʱ����(��λΪms)
extern u16 heartbeat_time;    //������ʱ����(��λΪms)
extern u16 dataupdate_time;   //BMU�����ϴ���ʱ����(��λΪus)
extern volatile u16 tmpdataupdate_time;//��ʱ��BMU�����ϴ�ʱ����(��λΪus)
extern u16 bmucellnum;        //��ǰBMU�µĵ�ش���(1-48,���48��)
extern u16 cellvol[BMU_VOL_NUM];       //��ǰ���زɼ�оƬBMU���:1��BMU�����ʹ��4��LTC6804оƬ���е�ѹ����
extern float soc[BMU_VOL_NUM];              //��ǰBMU�µ����е�ش���SOC״̬(float����,����0С��1��С��),���ش���ѹ��
extern u16 bmutempnum;        //��ǰBMU�µ��¶Ȳ�������
extern u16 temp[BMU_TEMP_NUM];         //��ǰ���:ÿ��BMU����ܲ���12���¶�����
extern float tempf[BMU_TEMP_NUM];           //ת��Ϊfloat��ʽ�ġ浥λ���¶�����
#endif
#if IS_BCU_ROLE
typedef struct _Cell {
  uint16_t id, vol;
} Cell;
extern Cell cell[BMU_LIST_NUM * BMU_VOL_NUM];
extern u16 bcucellnum;                    //��ǰBCU�µĵ�ش���
extern u16 cellvol[BMU_LIST_NUM * BMU_VOL_NUM]; //��Ҫ���ڲ�ͨ�Ź����н���ѹ�������������Ӧλ��
extern volatile u16 max_cellvol;                   //��ش�������ѹֵ
extern volatile u16 min_cellvol;                   //��ش�����С��ѹֵ
extern volatile u16 max_cellvolid;                 //��ش�������ѹֵ��Ӧ��ش�ID
extern volatile u16 min_cellvolid;                 //��ش�����С��ѹֵ��Ӧ��ش�ID
extern volatile u16 max_diff_cellvol;              //��ش����ѹ��
extern volatile u32 cellvoltotal;                  //����ܵ�ѹ(u32)
extern volatile float cellvolftotal;               //����ܵ�ѹ(float)
extern volatile u16 cellvolaverage;                //ƽ�������ѹ
extern float soc[BMU_LIST_NUM * BMU_VOL_NUM];        //��ǰBCU�µ����е�ش���SOC״̬(float����,����0С��1��С��),���ش���ѹ��
extern u16 bcutempnum;                    //��ǰBCU�µ��¶Ȳ�������
extern u16 temp[BMU_LIST_NUM * BMU_TEMP_NUM];   //��Ҫ���ڲ�ͨ�Ź����н��¶��������������Ӧλ��
extern float tempf[BMU_LIST_NUM * BMU_TEMP_NUM];     //ת��Ϊfloat��ʽ�ġ�Ϊ��λ���¶�����
extern volatile float max_tempf;                   //�¶Ȳɼ���������¶�ֵ
extern volatile float min_tempf;                   //�¶Ȳɼ�������С�¶�ֵ
extern volatile u16 max_tempfid;                 //�¶Ȳɼ���������¶�ֵ��Ӧ�ɼ�����
extern volatile u16 min_tempfid;                 //�¶Ȳɼ�������С�¶�ֵ��Ӧ�ɼ�����
extern volatile float max_diff_tempf;              //�¶Ȳɼ�������²�
extern volatile float tempfaverage;                //�¶Ȳɼ���ƽ����?
#endif
//��������ֵ
#if IS_BMU_ROLE
extern u8 bmusmokenum;                 //BMU�������ݸ���
extern u16 smoke[BMU_SMOKE_NUM];       //BCU��������
extern float smokef[BMU_SMOKE_NUM];                     //BMU��������
#endif
#if IS_BCU_ROLE
extern u8 bcusmokenum;                 //BCU�������ݸ���
extern u16 smoke[BMU_LIST_NUM * BMU_SMOKE_NUM];    //BCU��������
extern float smokef[BMU_LIST_NUM * BMU_SMOKE_NUM];    //BCU��������
#endif
/* BMU��BCU��Ҫ���ֵ������������ */

//��ʾ����ʾ������ر���
extern volatile u8 bmu_displaydata;  //���ڱ�ʶBMU�Ƿ��ϴ�����


//SOC״̬��������

//��������
extern const u16 SOC_OCV[11];  //�����ṩ�ĵ����SOC-OCV���
extern float xk,Pk;            //���ڴ洢Vmin-EKF�㷨��������е��м����(״̬���� Э����)

#define STANDTIME_LINE  600            //����SOC�궨�ľ���ʱ����ֵ(���趨Ϊ10min)
#define STANDCURRENT_LINE  0.5            //��ʾ�ж�Ϊ�Ƿ��õĵ���ֵ(���趨Ϊ0.5A)
extern volatile float soctotal;        //����SOC״̬����(������BCU���ǵ�����BMU,������Ҫ���������SOC״̬��)
extern u32 standtime;                  //ϵͳ����ʱ��,����Ϊ��λ


//ϵͳ���ϵ��������ʱ��
extern u32 runtime;  //����Ϊ��λ(�ɼ�¼>100���ʱ��)


//��Ե���ģ���ȡ����
extern u16 dcvol_data;    //ϵͳֱ����ѹ(��λV)
extern u16 res_pos_data;  //�����ӵص���(��λk��)
extern u16 res_neg_data;  //�����ӵص���(��λk��)
extern volatile float insuval_pos;      //������Ե������ݼ�����
extern volatile float insuval_neg;      //������Ե������ݼ�����

//������غ궨��
#define CURRENT_STATE_CHARGE      0x00  //����״̬:���
#define CURRENT_STATE_DISCHARGE   0x01  //����״̬:�ŵ�
//���ģ���ȡ����,��BMU��������ֵ
extern u8 em_addr[6];  //��λ��ǰ��λ�ں�,��[A0 A1 A2 A3 A4 A5],������֡˳����ͬ
extern u8 current[4];  //BCD����ʾ,��λ��ǰ��λ�ں�,������֡˳���෴,ǰ2�ֽ��Ǳ�ʾ��������,��2�ֽڱ�ʾС������
extern volatile float currentf;      //�ܵ�������(float����,4�ֽڴ洢)(�����ݵĶ�ȡ��Ӧ�����Ƿ����Ӧ�ķ�������ƥ��)(�˴�������,��Ϊ���,��Ϊ�ŵ�)
extern u8 is_charging;               //���ڱ�ʶ��ǰ���ڳ�绹���ڷŵ�(1��ʶ�ڸ��������,0��ʶ�ڸ������ŵ�)
extern float bmucurrentf[BMU_LIST_NUM];


//��ѹ����
extern float voltotal[3];

//CC״̬��غ궨��
#define CC_STATE_CONNECTED   1  //���ǹ�ͳ������������(�������ⲿ1k������,CC2�˵�ѹΪ6V)
#define CC_STATE_DISCONNECT  0  //���ǹ�ͳ����δ����(δ�����ⲿ1k������,CC2�˵�ѹΪ12V)
//CC״̬����
extern volatile u8 ccstate;                //BMU��ŵ�ӿ�����״̬(ֵΪCC_STATE_CONNECTED��CC_STATE_DISCONNECT)
extern u8 ccstatelist[BMU_LIST_NUM];    //BCU�µ�BMU��ŵ�ӿ�����״̬(���������BMU������Ӧ,��BMU_Connect_Numָʾ)

//BMS�����ѹ
extern volatile u16 powervolu16;         //16���ƹ����ѹ(ADCת��+�������)
extern volatile float powervolf;       //float���͹����ѹ(��ѹ��ʵֵ)

//MCU��ID��ȡ��ַ
#ifdef __STM32F00x_H
#define  ID_ADDR1  0x1FFFF7AC      /*STM32F0ΨһID��ʼ��ַ*/
#endif
#ifdef __STM32F10x_H
#define  ID_ADDR1  0x1FFFF7E8     /*STM32F1ΨһID��ʼ��ַ*/
#endif
#ifdef __STM32F2xx_H
#define  ID_ADDR1  0x1FFF7A10         /*STM32F2ΨһID��ʼ��ַ*/
#endif
#ifdef __STM32F3xx_H
#define  ID_ADDR1  0x1FFFF7AC        /*STM32F3ΨһID��ʼ��ַ*/
#endif
#ifdef __STM32F4xx_H
#define  ID_ADDR1  0x1FFF7A10        /*STM32F4ΨһID��ʼ��ַ*/
#endif
#ifdef __STM32F7xx_H
#define  ID_ADDR1  0x1FF0F420        /*STM32F7ΨһID��ʼ��ַ*/
#endif
#ifdef  __STM32L0XX_H
#define  ID_ADDR1  0x1FF80050        /*STM32L0ΨһID��ʼ��ַ*/
#endif
#ifdef __STM32L1XX_H
#define  ID_ADDR1  0x1FF80050        /*STM32L1ΨһID��ʼ��ַ*/
#endif
#ifdef __STM32L4xx_H
#define  ID_ADDR1  0x1FFF7590        /*STM32L4ΨһID��ʼ��ַ*/
#endif
#ifdef __STM32H7xx_H
#define  ID_ADDR1  0x1FF0F420        /*STM32H7ΨһID��ʼ��ַ*/
#endif

#endif

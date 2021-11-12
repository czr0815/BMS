#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	   


//****************************************CAN1���**************************************
//BMS�ڲ�CAN1ͨ���������:

//Ŀǰ��ID���÷���������:
//    (1)BMS��ÿ��CANͨ�Žڵ㶼��ID�洢���ڴ濨��,����ʱ���ڴ濨�ж�ȡBMS�豸���ͺ��豸ID
//    (2)BMS��BMU��BSU(�������)������������,��������ͬһָ��(SD��������д����ʱֱ�Ӷ���),Ȼ��BMU�ں�BCUͨ�Ź����б�ָ��ID
//    	(1)���������(2)���ܸ������ձ���
//    (3)��32/64λIDΨһ��ʶ��˾������һ��CAN2ͨ��ģ��,����ΨһID�洢��SD�ڴ濨��,�豸�忨����ʱ,����BMSϵͳ��8λ��ʱID��
//       ��ΨһID�Ķ�Ӧ��ϵ(��ͬ�����ƶ�),BMSϵͳ��ͨ��ֻʹ��8λ��ʱID

//���岻ͬ֡���͵�PF (�ڱ�BMS�ڲ�Э����,ʹ��PDU1��ʽ,��ʱPSΪĿ���ַDA,ͬʱR��DP����Ϊ0,29λ��ʶ������ֽ�ΪԴ��ַ;
//                    PDU1��ʽ��PGN=PF*256,��˴˴�ֱ��ʹ��PF��Ϊ֡���͵�Ψһ��ʶ,����Ϊ1���ֽ�)

// Э�����ݵ�Ԫ(PDU)��ʽ��
// |  ����  |   P   |   R  |  DP  |     PF    |    PS    |    SA    |    DATA    |  
// | 3bits  | 3bits | 1bit | 1bit |   8bits   |   8bits  |   8bits  |  0~64bits  |

//��ͬ������Ϣ֡��PF
#define PF_COMCONTROL     0x01    //ͨ�ſ���֡
#define PF_DATA           0x02    //����֡
#define PF_CONTROLCMD     0x04    //��������֡
#define PF_ALARMINFO      0x08    //�澯��Ϣ֡
//������ͬ֡���͵����ȼ�P
#define PF_COMCONTROL_P   0x01    //ͨ�ſ���֡
#define PF_DATA_P         0x04    //����֡
#define PF_CONTROLCMD_P   0x02    //��������֡
#define PF_ALARMINFO_P    0x03    //�澯��Ϣ֡
//��ͬ������Ϣ֡�µķ���
#define FT_COMCONTROL_HEARTBEAT       0x01    //ͨ�ſ��ƣ�������
#define FT_COMCONTROL_ACK             0x02    //ͨ�ſ��ƣ�ȷ��֡
#define FT_DATA_CELL                  0x03    //���ݣ���ص�ѹ��SOC״̬
#define FT_DATA_TEMP                  0x04    //���ݣ��¶�
#define FT_DATA_SMOKE                 0x05    //���ݣ�����
#define FT_DATA_CCSTATE               0x06    //���ݣ�CC�ź�״̬
#define FT_DATA_CURRENT               0x07    //����: ����
#define FT_CONTROLCMD_UPLOADDATA      0x08    //����ָ��: �ϴ�����(ָ��ĳ��BMU��ʼ�ϴ�����)
#define FT_CONTROLCMD_CELLBALANCE     0x09    //����ָ���ؾ���(����/ֹͣ��ؾ���)
#define FT_CONTROLCMD_TEMPCONTROL     0x0A    //����ָ��¶ȿ���
#define FT_CONTROLCMD_DANGERDEAL      0x0B    //����ָ����鴦��
#define FT_CONTROLCMD_SWITCH          0x0C    //����ָ���Դ���ؿ���
#define FT_CONTROLCMD_UPLOADPATTERN   0x0D    //����ָ��ı�BMU�ϴ�����֡����Ϊģʽ
#define FT_CONTROLCMD_ACK             0x0E    //����ָ�BMU��BCU�ظ�ȷ����Ϣ

//��������֡�����в�ͬ����ľ������֡
#define CONTROLCMD_CELLBALANCE_ON     0x19    //�����������:��������
#define CONTROLCMD_CELLBALANCE_OFF    0x29    //�����������:�رվ���


//��������֡�����в�ͬ����Ļظ�ȷ����Ϣ
#define CONTROLCMD_ACK_UPLOAD          0x1E   //��������ȷ�ϣ��ϴ���������ȷ��
#define CONTROLCMD_ACK_UPLOAD_COMPLETE 0x2E   //��������ȷ�ϣ��ϴ��������ȷ��


//��غ궨��
#define BMS_ROLE_BMU        0x01    //BMU��ɫ�ı�ʶ��
#define BMS_ROLE_BCU        0x02    //BCU��ɫ�ı�ʶ��
#define BMS_ROLE_BSU        0x04    //BSU��ɫ�ı�ʶ��

//CAN1����RX0�ж�ʹ��
#define CAN1_RX0_INT_ENABLE 1

//CAN1��ʼ��
void CAN1_Mode_Init();

#if CAN1_RX0_INT_ENABLE  //ʹ��CAN1��FIFO0�ж�		    
void CAN1_RX0_IRQHandler(void);  //CAN1�жϷ�����	
#endif

u8 CAN1_Send_Msg(u32 msgid, u8* msg, u8 len);  //CAN1��������
u8 CAN1_ComControl(u8 dst_id, u8 FT);  //CAN1�ڲ�ͨ�ţ�����ͨ�ſ���֡
u8 CAN1_ControlCmd(u8 dst_id, u8 FT, u8 cmd1, u8 cmd2, u16 timecmd);  //CAN1�ڲ�ͨ��: ���Ϳ�������֡
u8 CAN1_Send_Data(u8 dst_id, u8 FT, u8 chip, u8 num, u16 data1u16, float data2f);  //CAN1�ڲ�ͨ��:��������֡

void start_can1_connect();
u8 CAN1_RxMsg_Process();





#endif



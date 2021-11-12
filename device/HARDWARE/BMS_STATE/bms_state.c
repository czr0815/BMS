
/* 
  ����BMSϵͳ״̬���ݵĶ���
*/

#include "bms_state.h"
#include <stdint.h>
#include "sys.h"

//CAN1�ڲ�ͨ����ر�������
u8 My_BMS_Role;             //���ڱ�ʶ�����BMS��ɫ(BMU/BCU)
u8 My_Id;                   //���ڱ�ʶ����ID�������ڲ�ͨ��
u32 My_Did;                 //��ʾ�����豸ID�������ƶ��ϴ���ϵͳ����
u8 Is_Connected;            //���ڱ�ʶ���ϼ���ɫ�����ӹ�ϵ(����BMU��ʶ�Ƿ�������BCU,����BCU��ʶ�Ƿ�������BSU(����еĻ�))
u8 My_Superior_Id;	         //(�����ϼ���ɫ��������֮��)���ڱ�ʶ�ϼ���ID(����BMU��˵��BCU,����BCU��˵��BSU)
CanRxMsg CAN1RxMessage;              //���ڱ��洦��CAN1��ǰ���յ�����Ϣ֡
//BMU�ڲ�ͨ�Ŷ���
#if IS_BMU_ROLE
u8 My_Pack_Number;           //���ڱ�ʶBMU�������ĵ������,�ϴ�����ʱʹ�øñ���
volatile u8 bmu_uploaddata;  //���ڱ�ʶBMU�Ƿ��ϴ�����
#endif
#if IS_BCU_ROLE
volatile u16 bmucontroltask_delay;     //BCU�ڿ��������ϴ��ȹ��̵������е�ѭ����ʱʱ��
volatile u8 is_bmu_uploading;          //���ڱ�ʶĿǰBMU�Ƿ����ϴ�����
volatile u8 is_bmu_acked;              //���ڱ�ʶBMU�Ƿ���ϴ�����ظ�ȷ��
#endif

u8 BMU_List_Id[BMU_LIST_NUM];       //BMU�б�:BMU ID,����ϵͳ�ڲ�����
u32 BMU_List_Did[BMU_LIST_NUM];    //BMU�б��豸ID�������ϴ����ƶˣ���ϵͳ��ʹ��
u8 BMU_List_PackNum[BMU_LIST_NUM];  //BMU�б�:BMU����ĵ������
u8 BMU_List_State[BMU_LIST_NUM] = {0};    //BMU�б�:BMU״̬
u8 BMU_Connect_Num;         //��ʶBMU�б��е�BMU������



/* BMU��BCU��Ҫ���ֵĲ����������� */
//�����ѹ���¶�����,�Լ�SOC״̬����������
#if IS_BMU_ROLE
u16 dataacq_time;      //BMU��ȡ���ݵ�ʱ����(��λΪms)
u16 heartbeat_time;    //������ʱ����(��λΪms)
u16 dataupdate_time;   //BMU�����ϴ���ʱ����(��λΪus)
volatile u16 tmpdataupdate_time;//��ʱ��BMU�����ϴ�ʱ����(��λΪus)
u16 bmucellnum;        //��ǰBMU�µĵ�ش���(1-48,���48��)
u16 cellvol[BMU_VOL_NUM];       //��ǰ���زɼ�оƬBMU���:1��BMU�����ʹ��4��LTC6804оƬ���е�ѹ����
float soc[BMU_VOL_NUM];              //��ǰBMU�µ����е�ش���SOC״̬(float����,����0С��1��С��),���ش���ѹ��
u16 bmutempnum;        //��ǰBMU�µ��¶Ȳ�������
u16 temp[BMU_TEMP_NUM];         //��ǰ���:ÿ��BMU����ܲ���12���¶�����
float tempf[BMU_TEMP_NUM];           //ת��Ϊfloat��ʽ�ġ浥λ���¶�����
#endif
#if IS_BCU_ROLE
Cell cell[BMU_LIST_NUM * BMU_VOL_NUM];
u16 bcucellnum;                    //��ǰBCU�µĵ�ش���
u16 cellvol[BMU_LIST_NUM * BMU_VOL_NUM]; //��Ҫ���ڲ�ͨ�Ź����н���ѹ�������������Ӧλ��
volatile u16 max_cellvol;                   //��ش�������ѹֵ
volatile u16 min_cellvol;                   //��ش�����С��ѹֵ
volatile u16 max_cellvolid;                 //��ش�������ѹֵ��Ӧ��ش�ID
volatile u16 min_cellvolid;                 //��ش�����С��ѹֵ��Ӧ��ش�ID
volatile u16 max_diff_cellvol;              //��ش����ѹ��
volatile u32 cellvoltotal;                  //����ܵ�ѹ(u32)
volatile float cellvolftotal;               //����ܵ�ѹ(float)
volatile u16 cellvolaverage;                //ƽ�������ѹ
float soc[BMU_LIST_NUM * BMU_VOL_NUM];      //��ǰBCU�µ����е�ش���SOC״̬(float����,����0С��1��С��),���ش���ѹ��
u16 bcutempnum;                    //��ǰBCU�µ��¶Ȳ�������
u16 temp[BMU_LIST_NUM * BMU_TEMP_NUM];   //��Ҫ���ڲ�ͨ�Ź����н��¶��������������Ӧλ��
float tempf[BMU_LIST_NUM * BMU_TEMP_NUM];   //ת��Ϊfloat��ʽ�ġ�Ϊ��λ���¶�����
volatile float max_tempf;                   //�¶Ȳɼ���������¶�ֵ
volatile float min_tempf;                   //�¶Ȳɼ�������С�¶�ֵ
volatile u16 max_tempfid;                 //�¶Ȳɼ���������¶�ֵ��Ӧ�ɼ�����
volatile u16 min_tempfid;                 //�¶Ȳɼ�������С�¶�ֵ��Ӧ�ɼ�����
volatile float max_diff_tempf;              //�¶Ȳɼ�������²�
volatile float tempfaverage;                //�¶Ȳɼ���ƽ���¶�
#endif
//��������ֵ
#if IS_BMU_ROLE
u8 bmusmokenum;                 //BMU�������ݸ���
u16 smoke[BMU_SMOKE_NUM];                   //�ɼ�����������������ѹֵ
float smokef[BMU_SMOKE_NUM];                     //BMU��������
#endif
#if IS_BCU_ROLE
u8 bcusmokenum;                 //BCU�������ݸ���
u16 smoke[BMU_LIST_NUM * BMU_SMOKE_NUM];    //BCU��������
float smokef[BMU_LIST_NUM * BMU_SMOKE_NUM];    //BCU��������
#endif  
/* BMU��BCU��Ҫ���ֵ������������ */

//��ʾ����ʾ������ر���
volatile u8 bmu_displaydata;  //���ڱ�ʶBMU�Ƿ��ϴ�����


//SOC״̬��������

//��ر�������
//�����ṩ��SOC-OCV���:  0%    10%   20%   30%   40%   50%   60%   70%   80%   90%  100%
const u16 SOC_OCV[11] = {2723, 3227, 3259, 3280, 3289, 3292, 3301, 3316, 3327, 3333, 3400};
float xk,Pk;           //���ڴ洢Vmin-EKF�㷨��������е��м����(״̬���� Э����)

volatile float soctotal;                         //����SOC״̬����(������BCU���ǵ�����BMU,������Ҫ���������SOC״̬)
u32 standtime;                                   //ϵͳ����ʱ��,����Ϊ��λ




//ϵͳ���ϵ��������ʱ��
u32 runtime;  //����Ϊ��λ(�ɼ�¼>100���ʱ��)

//��Ե���ģ���ȡ����
u16 dcvol_data;    //ϵͳֱ����ѹ(��λV)
u16 res_pos_data;  //�����ӵص���(��λk��)
u16 res_neg_data;  //�����ӵص���(��λk��)
volatile float insuval_pos;      //������Ե������ݼ�����
volatile float insuval_neg;      //������Ե������ݼ�����

//���ģ���ȡ����,��BMU��������ֵ
u8 em_addr[6] = {0x01,0x00,0x00,0x00,0x00,0x00};    //��λ��ǰ��λ�ں�,��[A0 A1 A2 A3 A4 A5],������֡˳����ͬ
u8 current[4];                                      //BCD����ʾ,��λ��ǰ��λ�ں�,������֡˳���෴,ǰ2�ֽ��Ǳ�ʾ��������,��2�ֽڱ�ʾС������
volatile float currentf;                                          //�ܵ�������(float����,4�ֽڴ洢)
u8 is_charging;               //���ڱ�ʶ��ǰ���ڳ�绹���ڷŵ�(1��ʶ�ڸ��������,0��ʶ�ڸ������ŵ�)
float bmucurrentf[BMU_LIST_NUM];                      //BCU�¸���BMU�ĵ�������(���������BMU������Ӧ,��BMU_Connect_Numָʾ)


//��ѹ����
float voltotal[3];

//CC״̬
volatile u8 ccstate;                //BCU��ŵ�ӿ�����״̬
u8 ccstatelist[BMU_LIST_NUM];    //BCU�µ�BMU��ŵ�ӿ�����״̬(���������BMU������Ӧ,��BMU_Connect_Numָʾ)

//BMS�����ѹ
volatile u16 powervolu16;         //16���ƹ����ѹ(ADCת��+�������)
volatile float powervolf;       //float���͹����ѹ(��ѹ��ʵֵ)





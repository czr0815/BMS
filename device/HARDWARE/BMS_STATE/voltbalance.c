
/* ��ѹ����˵��


*/
#include "voltbalance.h"
#include "bms_state.h"

//������ؿ��Ʊ�־
u8 VoltLimitFlag;           //�����ѹ������(1Ϊ��������,0Ϊδ��������)
u8 ModeValidFlag;           //�����ϵͳ����״̬����(1Ϊ��������,0Ϊδ��������)
u8 CurrentLimitFlag;        //����ĵ�����С����(1Ϊ��������,0Ϊδ��������)
u8 BalanceEnableFlag;       //����Ŀ�����־(1Ϊ��������,0Ϊδ��������)
u8 BalancePrevEnableFlag;   //ǰһʱ�̵ľ��⿪����־
#if IS_BCU_ROLE
u8 BalanceFlag[BMU_LIST_NUM * BMU_VOL_NUM] = {0};  //�����ؾ���״̬��־(1Ϊ��������,0Ϊδ��������)
#endif
#if IS_BMU_ROLE
u8 BalanceFlag[BMU_VOL_NUM];  //�����ؾ���״̬��־(1Ϊ��������,0Ϊδ��������)
#endif
u16 balancetargetvol;       //����Ŀ���ѹ(�������ֹͣ��Ŀ���ѹ)

u8 celltobalance_previd[BALANCE_NUM];    //�ϴεľ�����id
u16 celltobalance_prevvol[BALANCE_NUM];  //�ϴεľ����ص�ѹ
u8 celltobalance_id[BALANCE_NUM];       //���εľ�����id
u16 celltobalance_vol[BALANCE_NUM];     //���εľ����ص�ѹ

























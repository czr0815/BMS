
#ifndef __VOLTBALANCE
#define __VOLTBALANCE

#include "sys.h"
#include "bms_state.h"

//���õ���غ궨��
#define BALANCE_NUM 8    //����ͬʱ����·��
#define RUN_BALANCE 1    //��BMS��������״̬��(��ֻ�ǳ��)�Ƿ���о������

//����ı궨��
#define VoltLimitCalb 5000         //�����ѹ�������궨
#define CurrentLimitCalb  10.0     //����ĵ�����С�����궨
#define MinCellVolLimit   28000        //�����ѹ�������


//������ؿ��Ʊ�־
extern u8 VoltLimitFlag;           //�����ѹ������(1Ϊ��������,0Ϊδ��������)
extern u8 ModeValidFlag;           //�����ϵͳ����״̬����(1Ϊ��������,0Ϊδ��������)
extern u8 CurrentLimitFlag;        //����ĵ�����С����(1Ϊ��������,0Ϊδ��������)
extern u8 BalanceEnableFlag;       //����Ŀ�����־(1Ϊ��������,0Ϊδ��������)
extern u8 BalancePrevEnableFlag;   //ǰһʱ�̵ľ��⿪����־
#if IS_BCU_ROLE
extern u8 BalanceFlag[BMU_LIST_NUM * BMU_VOL_NUM];  //�����ؾ���״̬��־(1Ϊ��������,0Ϊδ��������)
#endif
#if IS_BMU_ROLE
extern u8 BalanceFlag[BMU_VOL_NUM];  //�����ؾ���״̬��־(1Ϊ��������,0Ϊδ��������)
#endif
extern u16 balancetargetvol;       //����Ŀ���ѹ(�������ֹͣ��Ŀ���ѹ)

extern u8 celltobalance_previd[BALANCE_NUM];    //�ϴεľ�����id
extern u16 celltobalance_prevvol[BALANCE_NUM];  //�ϴεľ����ص�ѹ
extern u8 celltobalance_id[BALANCE_NUM];       //���εľ�����id
extern u16 celltobalance_vol[BALANCE_NUM];     //���εľ����ص�ѹ


































#endif

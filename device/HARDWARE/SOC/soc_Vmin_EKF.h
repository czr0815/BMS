#ifndef __SOC_VMIN_EKF_H
#define __SOC_VMIN_EKF_H

/* SOC�������˵��
- ���ļ������˹�������������SOC״̬�ķ�������;
- �����SOC״̬�Ĺ��㷽����Ҫ��:������С��ѹ����չ�������˲�����;
*/
#include "sys.h"
#include "stmflash.h"

//�궨��
#define eta 0.999      //����Ч��:��طŵ�������ͬѭ�������г������֮��
#define Cn  727200     //����������:202Ah,���ڼ���SOC״̬����(727200 = 202Ah*3600s)
#define V0  3.4
#define R   0.0003    //�������:���ڼ���۲ⷽ��,��λ��
#define K0  1     //�۲ⷽ�̲���
#define K1  1     //�۲ⷽ�̲���
#define K2  1     //�۲ⷽ�̲���
#define K3  1     //�۲ⷽ�̲���
#define I   1    //��λ����  


////��������
//extern const u16 SOC_OCV[11];  //�����ṩ�ĵ����SOC-OCV���
//extern float xk,Pk;            //���ڴ洢Vmin-EKF�㷨��������е��м����(״̬���� Э����)





//��������
void soc_write(float socnow);  //����SOC����
float soc_read(void);   //��ȡ֮ǰʱ�̱����SOC����
float soc_calibrate(u16 cellvoltocalb);  //SOC�궨
float soc_calculate(float ik_1, u16 yku16, float t);  //����SOC״ֵ̬
float ln(float x);



















#endif



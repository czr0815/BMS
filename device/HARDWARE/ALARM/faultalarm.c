
/* ���ϱ������˵��
- ���ϱ������Ͱ���:
  - ��01��SOC��
  - ��02�������ѹ����
	- ��03���ŵ���ѹ����
	- ��04����絥���ѹ
	- ��05����絥���ѹ
	- ��06���ŵ絥���ѹ
	- ��07���ŵ絥���ѹ
	- ��08�����ѹ��
	- ��09���ŵ�ѹ��
	- ��10����ѹ����
	- ��11��������
	- ��12���ŵ����
	- ��13��������
	- ��14���ŵ����
	- ��15������²�
	- ��16���ŵ��²�
	- ��17��������
	- ��18���ŵ����
	- ��19����Ե©��
	- ��20����������
- Ӧʵ�ֹ���/�������̰���:
	- ÿһ�εı�������,���ǰ��������жϱ��������Ƿ���,��������ͽ��ж�Ӧ�ļ��,���õ����������ȼ�,���������ͺͱ����ȼ�ȷ�����ս���ʲô���Ĳ���;
  - ���õ��ı����ȼ�,Ӧ���Ǽ�����ʱ֮���жϵó���
*/

#include "faultalarm.h"
#include "bms_state.h"
#include "BMS.h"

//�����͹��ϼ���Ƿ���(1Ϊ����,0Ϊ�ر�)
u8 alarmfun_soclow,alarmfun_voltotalhigh,alarmfun_voltotallow,alarmfun_cvolhigh,alarmfun_cvollow,alarmfun_dscvolhigh,alarmfun_dscvollow;
u8 alarmfun_cvoldiff,alarmfun_dscvoldiff,alarmfun_volwire,alarmfun_ctemphigh,alarmfun_dsctemphigh,alarmfun_ctemplow,alarmfun_dsctemplow;
u8 alarmfun_ctempdiff,alarmfun_dsctempdiff,alarmfun_covercurr,alarmfun_dscovercurr,alarmfun_insuleak,alarmfun_temprise;

//ǰһʱ�̵ļ��״̬
uint8_t alarmcheckstate_soclow, alarmcheckstate_voltotalhigh, alarmcheckstate_voltotallow, alarmcheckstate_cvolhigh, alarmcheckstate_cvollow, alarmcheckstate_dscvolhigh, alarmcheckstate_dscvollow;
uint8_t alarmcheckstate_cvoldiff,alarmcheckstate_dscvoldiff,alarmcheckstate_volwire,alarmcheckstate_ctemphigh,alarmcheckstate_dsctemphigh,alarmcheckstate_ctemplow,alarmcheckstate_dsctemplow;
uint8_t alarmcheckstate_ctempdiff,alarmcheckstate_dsctempdiff,alarmcheckstate_covercurr,alarmcheckstate_dscovercurr,alarmcheckstate_insuleak,alarmcheckstate_temprise;

//��ʱ�̺�ǰһʱ�̱���״̬(1Ϊ����,0Ϊδ����)(�Ƚ�����ʱ�̵ı���״̬,��ͬ������ʾ�����Ϳ�����Ϣ������ʾ���ݸ���)
u8 alarmstate_soclow,alarmstate_voltotalhigh,alarmstate_voltotallow,alarmstate_cvolhigh,alarmstate_cvollow,alarmstate_dscvolhigh,alarmstate_dscvollow;
u8 alarmstate_cvoldiff,alarmstate_dscvoldiff,alarmstate_volwire,alarmstate_ctemphigh,alarmstate_dsctemphigh,alarmstate_ctemplow,alarmstate_dsctemplow;
u8 alarmstate_ctempdiff,alarmstate_dsctempdiff,alarmstate_covercurr,alarmstate_dscovercurr,alarmstate_insuleak,alarmstate_temprise;
u8 alarmprevstate_soclow,alarmprevstate_voltotalhigh,alarmprevstate_voltotallow,alarmprevstate_cvolhigh,alarmprevstate_cvollow,alarmprevstate_dscvolhigh,alarmprevstate_dscvollow;
u8 alarmprevstate_cvoldiff,alarmprevstate_dscvoldiff,alarmprevstate_volwire,alarmprevstate_ctemphigh,alarmprevstate_dsctemphigh,alarmprevstate_ctemplow,alarmprevstate_dsctemplow;
u8 alarmprevstate_ctempdiff,alarmprevstate_dsctempdiff,alarmprevstate_covercurr,alarmprevstate_dscovercurr,alarmprevstate_insuleak,alarmprevstate_temprise;

//����ʱ��ʱʱ��(���Ϸ����˶೤ʱ��)
u8 alarmdelay_soclow=0,alarmdelay_voltotalhigh=0,alarmdelay_voltotallow=0,alarmdelay_cvolhigh=0,alarmdelay_cvollow=0,alarmdelay_dscvolhigh=0,alarmdelay_dscvollow=0;
u8 alarmdelay_cvoldiff=0,alarmdelay_dscvoldiff=0,alarmdelay_volwire=0,alarmdelay_ctemphigh=0,alarmdelay_dsctemphigh=0,alarmdelay_ctemplow=0,alarmdelay_dsctemplow=0;
u8 alarmdelay_ctempdiff=0,alarmdelay_dsctempdiff=0,alarmdelay_covercurr=0,alarmdelay_dscovercurr=0,alarmdelay_insuleak=0,alarmdelay_temprise=0;

//�ͷ�ʱ��ʱʱ��(���ϻָ��˶೤ʱ��)
u8 releasedelay_soclow=0,releasedelay_voltotalhigh=0,releasedelay_voltotallow=0,releasedelay_cvolhigh=0,releasedelay_cvollow=0,releasedelay_dscvolhigh=0,releasedelay_dscvollow=0;
u8 releasedelay_cvoldiff=0,releasedelay_dscvoldiff=0,releasedelay_volwire=0,releasedelay_ctemphigh=0,releasedelay_dsctemphigh=0,releasedelay_ctemplow=0,releasedelay_dsctemplow=0;
u8 releasedelay_ctempdiff=0,releasedelay_dsctempdiff=0,releasedelay_covercurr=0,releasedelay_dscovercurr=0,releasedelay_insuleak=0,releasedelay_temprise=0;

//������ֵ(�±�0:���ϵȼ�1,�±�1:���ϵȼ�2,�±�2:���ϵȼ�3)
float alarmtrshold_soclow[3],alarmtrshold_voltotalhigh[3],alarmtrshold_voltotallow[3],alarmtrshold_cvolhigh[3],alarmtrshold_cvollow[3],alarmtrshold_dscvolhigh[3],alarmtrshold_dscvollow[3];
float alarmtrshold_cvoldiff[3],alarmtrshold_dscvoldiff[3],alarmtrshold_volwire[3],alarmtrshold_ctemphigh[3],alarmtrshold_dsctemphigh[3],alarmtrshold_ctemplow[3],alarmtrshold_dsctemplow[3];
float alarmtrshold_ctempdiff[3],alarmtrshold_dsctempdiff[3],alarmtrshold_covercurr[3],alarmtrshold_dscovercurr[3],alarmtrshold_insuleak[3],alarmtrshold_temprise[3];

//������ʱ��ֵ
u8 alarmdelaytrshold_soclow[3],alarmdelaytrshold_voltotalhigh[3],alarmdelaytrshold_voltotallow[3],alarmdelaytrshold_cvolhigh[3];
u8 alarmdelaytrshold_cvollow[3],alarmdelaytrshold_dscvolhigh[3],alarmdelaytrshold_dscvollow[3];
u8 alarmdelaytrshold_cvoldiff[3],alarmdelaytrshold_dscvoldiff[3],alarmdelaytrshold_volwire[3],alarmdelaytrshold_ctemphigh[3];
u8 alarmdelaytrshold_dsctemphigh[3],alarmdelaytrshold_ctemplow[3],alarmdelaytrshold_dsctemplow[3];
u8 alarmdelaytrshold_ctempdiff[3],alarmdelaytrshold_dsctempdiff[3],alarmdelaytrshold_covercurr[3],alarmdelaytrshold_dscovercurr[3],alarmdelaytrshold_insuleak[3],alarmdelaytrshold_temprise[3];

//�ͷ���ֵ
float releasetrshold_soclow[3],releasetrshold_voltotalhigh[3],releasetrshold_voltotallow[3],releasetrshold_cvolhigh[3],releasetrshold_cvollow[3],releasetrshold_dscvolhigh[3],releasetrshold_dscvollow[3];
float releasetrshold_cvoldiff[3],releasetrshold_dscvoldiff[3],releasetrshold_volwire[3],releasetrshold_ctemphigh[3],releasetrshold_dsctemphigh[3],releasetrshold_ctemplow[3],releasetrshold_dsctemplow[3];
float releasetrshold_ctempdiff[3],releasetrshold_dsctempdiff[3],releasetrshold_covercurr[3],releasetrshold_dscovercurr[3],releasetrshold_insuleak[3],releasetrshold_temprise[3];

//�ͷ���ʱ��ֵ
u8 releasedelaytrshold_soclow[3],releasedelaytrshold_voltotalhigh[3],releasedelaytrshold_voltotallow[3];
u8 releasedelaytrshold_cvolhigh[3],releasedelaytrshold_cvollow[3],releasedelaytrshold_dscvolhigh[3],releasedelaytrshold_dscvollow[3];
u8 releasedelaytrshold_cvoldiff[3],releasedelaytrshold_dscvoldiff[3],releasedelaytrshold_volwire[3],releasedelaytrshold_ctemphigh[3];
u8 releasedelaytrshold_dsctemphigh[3],releasedelaytrshold_ctemplow[3],releasedelaytrshold_dsctemplow[3];
u8 releasedelaytrshold_ctempdiff[3],releasedelaytrshold_dsctempdiff[3],releasedelaytrshold_covercurr[3],releasedelaytrshold_dscovercurr[3];
u8 releasedelaytrshold_insuleak[3],releasedelaytrshold_temprise[3];




//����ʵ��
#if IS_BCU_ROLE
//�趨���ϼ�⹦�ܵ�Ĭ�Ͽ�����Ĭ��ֵ
void faultalarm_setdefault(void)
{
	bcucellnum = 237;  //�����ֳ���237�����
	
	//���ù��ϼ�⹦���Ƿ���(1Ϊ����,0Ϊ�ر�)
	alarmfun_soclow = 1;           //SOC��
	alarmfun_voltotalhigh = 1;     //�����ѹ����
	alarmfun_voltotallow = 1;      //�����ѹ����
	alarmfun_cvolhigh = 1;         //��絥���ѹ
	alarmfun_cvollow = 1;          //��絥���ѹ
	alarmfun_dscvolhigh = 1;       //�ŵ絥���ѹ
	alarmfun_dscvollow = 1;        //�ŵ絥���ѹ
	alarmfun_cvoldiff = 1;         //���ѹ��
	alarmfun_dscvoldiff = 1;       //�ŵ�ѹ��
	alarmfun_volwire = 0;          //��ѹ����
	alarmfun_ctemphigh = 1;        //������
	alarmfun_dsctemphigh = 1;      //�ŵ����
	alarmfun_ctemplow = 1;         //������
	alarmfun_dsctemplow = 1;       //�ŵ����
	alarmfun_ctempdiff = 1;        //����²�
	alarmfun_dsctempdiff = 1;      //�ŵ��²�
	alarmfun_covercurr = 1;        //������
	alarmfun_dscovercurr = 1;      //�ŵ����
	alarmfun_insuleak = 1;         //��Ե©��
	alarmfun_temprise = 1;         //��������
	
	//����ǰһʱ�̵Ĺ���״̬�ͼ��״̬Ϊδ����(0)
	//���õ�ǰʱ�̵Ĺ���״̬Ϊδ����
	alarmprevstate_soclow = alarmcheckstate_soclow = alarmstate_soclow = 0;
	alarmprevstate_voltotalhigh = alarmcheckstate_voltotalhigh = alarmstate_voltotalhigh = 0;
	alarmprevstate_voltotallow = alarmcheckstate_voltotallow = alarmstate_voltotallow = 0;
	alarmprevstate_cvolhigh = alarmcheckstate_cvolhigh = alarmstate_cvolhigh = 0;
	alarmprevstate_cvollow = alarmcheckstate_cvollow = alarmstate_cvollow = 0;
	alarmprevstate_dscvolhigh = alarmcheckstate_dscvolhigh = alarmstate_dscvolhigh = 0;
	alarmprevstate_dscvollow = alarmcheckstate_dscvollow = alarmstate_dscvollow = 0;
	alarmprevstate_cvoldiff = alarmcheckstate_cvoldiff = alarmstate_cvoldiff = 0;
	alarmprevstate_dscvoldiff = alarmcheckstate_dscvoldiff = alarmstate_dscvoldiff = 0;
	alarmprevstate_volwire = alarmcheckstate_volwire = alarmstate_volwire = 0;
	alarmprevstate_ctemphigh = alarmcheckstate_ctemphigh = alarmstate_ctemphigh = 0;
	alarmprevstate_dsctemphigh = alarmcheckstate_dsctemphigh = alarmstate_dsctemphigh = 0;
	alarmprevstate_ctemplow = alarmcheckstate_ctemplow = alarmstate_ctemplow = 0;
	alarmprevstate_dsctemplow = alarmcheckstate_dsctemplow = alarmstate_dsctemplow =0;
	alarmprevstate_ctempdiff = alarmcheckstate_ctempdiff = alarmstate_ctempdiff = 0;
	alarmprevstate_dsctempdiff = alarmcheckstate_dsctempdiff = alarmstate_dsctempdiff = 0;
	alarmprevstate_covercurr = alarmcheckstate_covercurr = alarmstate_covercurr = 0;
	alarmprevstate_dscovercurr = alarmcheckstate_dscovercurr = alarmstate_dscovercurr = 0;
	alarmprevstate_insuleak = alarmcheckstate_insuleak = alarmstate_insuleak = 0;
	alarmprevstate_temprise = alarmcheckstate_insuleak = alarmstate_insuleak = 0;
	
	//������ֵ�趨
	alarmtrshold_soclow[0] = 0.3;  //��ѹ�ȽϺ�����Ҫת��Ϊһ�µĵ�λ
	alarmtrshold_soclow[1] = 0.2;
	alarmtrshold_soclow[2] = 0.1;
	alarmtrshold_voltotalhigh[0] = 3.5;  //�����ѹ�ߵĵ����ѹ,�Ƚ�ʱ����ܵ�ش���
	alarmtrshold_voltotalhigh[1] = 3.55;
	alarmtrshold_voltotalhigh[2] = 3.65;
	alarmtrshold_voltotallow[0] = 3.0;  //�ŵ���ѹ�͵ĵ����ѹ,�Ƚ�ʱ����ܵ�ش���
	alarmtrshold_voltotallow[1] = 2.8;
	alarmtrshold_voltotallow[2] = 2.5;
	alarmtrshold_cvolhigh[0] = 3.5;
	alarmtrshold_cvolhigh[1] = 3.55;
	alarmtrshold_cvolhigh[2] = 3.65;
	alarmtrshold_cvollow[0] = 3.0;
	alarmtrshold_cvollow[1] = 2.8;
	alarmtrshold_cvollow[2] = 2.5;
	alarmtrshold_dscvolhigh[0] = 3.5;
	alarmtrshold_dscvolhigh[1] = 3.55;
	alarmtrshold_dscvolhigh[2] = 3.65;
	alarmtrshold_dscvollow[0] = 3.0;
	alarmtrshold_dscvollow[1] = 2.8;
	alarmtrshold_dscvollow[2] = 2.5;
	alarmtrshold_cvoldiff[0] = 0.3;  //��������û�ᵽ��
	alarmtrshold_cvoldiff[1] = 0.4;
	alarmtrshold_cvoldiff[2] = 0.8;
	alarmtrshold_dscvoldiff[0] = 0.4;  //��������û�ᵽ��
	alarmtrshold_dscvoldiff[1] = 0.8;
	alarmtrshold_dscvoldiff[2] = 1.0;
	//alarmtrshold_volwire[0] = ?  //��������û�ᵽ��
	alarmtrshold_ctemphigh[0] = 38.0;
	alarmtrshold_ctemphigh[1] = 42.0;
	alarmtrshold_ctemphigh[2] = 45.0;
	alarmtrshold_dsctemphigh[0] = 45.0;
	alarmtrshold_dsctemphigh[1] = 50.0;
	alarmtrshold_dsctemphigh[2] = 55.0;
	alarmtrshold_ctemplow[0] = 5.0;  //��������û�ᵽ��
	alarmtrshold_ctemplow[1] = 2.0;  //��������û�ᵽ��
	alarmtrshold_ctemplow[2] = 0.0;
	alarmtrshold_dsctemplow[0] = 0.0;
	alarmtrshold_dsctemplow[1] = -10.0;
	alarmtrshold_dsctemplow[2] = -20.0;
	alarmtrshold_ctempdiff[0] = 3.0;  //��������û�ᵽ��
	alarmtrshold_ctempdiff[1] = 5.0;
	alarmtrshold_ctempdiff[2] = 10.0;
	alarmtrshold_dsctempdiff[0] = 3.0;  //��������û�ᵽ��
	alarmtrshold_dsctempdiff[1] = 5.0;
	alarmtrshold_dsctempdiff[2] = 10.0;
	alarmtrshold_covercurr[0] = 70.0;  //��������û�ᵽ��
	alarmtrshold_covercurr[1] = 80.0;
	alarmtrshold_covercurr[2] = 100.0;
	alarmtrshold_dscovercurr[0] = 250.0;  //��������û�ᵽ��
	alarmtrshold_dscovercurr[1] = 300.0;
	alarmtrshold_dscovercurr[2] = 350.0;
	alarmtrshold_insuleak[0] = 500.0;
	alarmtrshold_insuleak[1] = 200.0;
	alarmtrshold_insuleak[2] = 100.0;
	alarmtrshold_temprise[0] = 5.0;
	alarmtrshold_temprise[1] = 10.0;
	alarmtrshold_temprise[2] = 15.0;
	
	//�ͷ���ֵ�趨
	releasetrshold_soclow[0] = 0.31;
	releasetrshold_soclow[1] = 0.21;
	releasetrshold_soclow[2] = 0.11;
	releasetrshold_voltotalhigh[0] = 3.45;
	releasetrshold_voltotalhigh[1] = 3.5;
	releasetrshold_voltotalhigh[2] = 3.6;  //��������û�ᵽ��
	releasetrshold_voltotallow[0] = 3.1;
	releasetrshold_voltotallow[1] = 3.0;
	releasetrshold_voltotallow[2] = 2.9;  //��������û�ᵽ��
	releasetrshold_cvolhigh[0] = 3.45;
	releasetrshold_cvolhigh[1] = 3.5;
	releasetrshold_cvolhigh[2] = 3.55;  //��������û�ᵽ��
	releasetrshold_cvollow[0] = 3.1;
	releasetrshold_cvollow[1] = 3.0;
	releasetrshold_cvollow[2] =	2.9;  //��������û�ᵽ��
	releasetrshold_dscvolhigh[0] = 3.45;
	releasetrshold_dscvolhigh[1] = 3.5;
	releasetrshold_dscvolhigh[2] = 3.55;  //��������û�ᵽ��
	releasetrshold_dscvollow[0] = 3.1;
	releasetrshold_dscvollow[1] = 3.0;
	releasetrshold_dscvollow[2] =	2.9;  //��������û�ᵽ��
	releasetrshold_cvoldiff[0] = 0.15;  //��������û�ᵽ��
	releasetrshold_cvoldiff[1] = 0.2;
	releasetrshold_cvoldiff[2] = 0.4;
	releasetrshold_dscvoldiff[0] = 0.5;  //��������û�ᵽ��
	releasetrshold_dscvoldiff[1] = 0.6;
	releasetrshold_dscvoldiff[2] = 0.8;
	//releasetrshold_volwire[0] =
	//releasetrshold_volwire[1] =
	//releasetrshold_volwire[2] =	
	releasetrshold_ctemphigh[0] = 36.0;
	releasetrshold_ctemphigh[1] = 40.0;
	releasetrshold_ctemphigh[2] =	42.0;
	releasetrshold_dsctemphigh[0] = 40.0;
	releasetrshold_dsctemphigh[1] = 45.0;
	releasetrshold_dsctemphigh[2] =	50.0;
	releasetrshold_ctemplow[0] = 8.0;  //��������û�ᵽ��
	releasetrshold_ctemplow[1] = 7.0;  //��������û�ᵽ��
	releasetrshold_ctemplow[2] = 5.0;
	releasetrshold_dsctemplow[0] = 5.0;
	releasetrshold_dsctemplow[1] = 0.0;
	releasetrshold_dsctemplow[2] = -5.0;
	releasetrshold_ctempdiff[0] = 3.0;  //��������û�ᵽ��
	releasetrshold_ctempdiff[1] = 4.0;
	releasetrshold_ctempdiff[2] =	8.0;
	releasetrshold_dsctempdiff[0] = 3.0;  //��������û�ᵽ��
	releasetrshold_dsctempdiff[1] = 4.0;
	releasetrshold_dsctempdiff[2] =	8.0;
	releasetrshold_covercurr[0] = 70.0;  //��������û�ᵽ��
	releasetrshold_covercurr[1] = 75.0;
	releasetrshold_covercurr[2] =	78.0;  //��������û�ᵽ��
	releasetrshold_dscovercurr[0] = 285.0;  //��������û�ᵽ��
	releasetrshold_dscovercurr[1] =  290.0;
	releasetrshold_dscovercurr[2] = 295.0;  //��������û�ᵽ��
	releasetrshold_insuleak[0] = 500.0;  //��������û�ᵽ��
	releasetrshold_insuleak[1] = 500.0;  //��������û�ᵽ��
	releasetrshold_insuleak[2] = 500.0;  //��������û�ᵽ��
	releasetrshold_temprise[0] = 0.0;  //��������û�ᵽ�� 
	releasetrshold_temprise[1] = 0.0;  //��������û�ᵽ�� 
	releasetrshold_temprise[2] = 0.0;  //��������û�ᵽ�� 
	
	//������ʱ��ֵ
//	alarmdelaytrshold_soclow[3]
//	alarmdelaytrshold_voltotalhigh[3]
//	alarmdelaytrshold_voltotallow[3]
//	alarmdelaytrshold_cvolhigh[3]
//	alarmdelaytrshold_cvollow[3]
//	alarmdelaytrshold_dscvolhigh[3]
//	alarmdelaytrshold_dscvollow[3]
//	alarmdelaytrshold_cvoldiff[3]
//	alarmdelaytrshold_dscvoldiff[3]
//	alarmdelaytrshold_volwire[3]
//	alarmdelaytrshold_ctemphigh[3]
//	alarmdelaytrshold_dsctemphigh[3]
//	alarmdelaytrshold_ctemplow[3]
//	alarmdelaytrshold_dsctemplow[3]
//	alarmdelaytrshold_ctempdiff[3]
//	alarmdelaytrshold_dsctempdiff[3]
//	alarmdelaytrshold_covercurr[3]
//	alarmdelaytrshold_dscovercurr[3]
//	alarmdelaytrshold_insuleak[3]
//	alarmdelaytrshold_temprise[3]
}

uint8_t faultalarm_soclow(float soc){
	alarmprevstate_soclow = alarmstate_soclow;

	if(soc<alarmtrshold_soclow[1]&&alarmstate_soclow<2){
		if(alarmcheckstate_soclow==2&&runtime-alarmdelay_soclow>alarmdelaytrshold_soclow[1]){
			return alarmstate_soclow=2;
		}else if(alarmcheckstate_soclow!=2){
			alarmcheckstate_soclow=2;
			alarmdelay_soclow = runtime;
		}
	}else if(soc<alarmtrshold_soclow[0]&&alarmstate_soclow<1){
		if(alarmcheckstate_soclow==1&&runtime-alarmdelay_soclow>alarmdelaytrshold_soclow[0]){
			return alarmstate_soclow=1;
		}else if(alarmcheckstate_soclow!=1){
			alarmcheckstate_soclow=1;
			alarmdelay_soclow = runtime;
		}
	}else if(soc>alarmtrshold_soclow[0]&&alarmstate_soclow==1){
		if(alarmcheckstate_soclow==1+3&&runtime-alarmdelay_soclow>alarmdelaytrshold_soclow[0]){
			return alarmstate_soclow=0;
		}else if(alarmcheckstate_soclow!=1+3){
			alarmcheckstate_soclow=1+3;
			alarmdelay_soclow = runtime;
		}
	}else if(soc>alarmtrshold_soclow[1]&&alarmstate_soclow==2){
		if(alarmcheckstate_soclow==2+3&&runtime-alarmdelay_soclow>alarmdelaytrshold_soclow[1]){
			return alarmstate_soclow=1;
		}else if(alarmcheckstate_soclow!=2+3){
			alarmcheckstate_soclow=2+3;
			alarmdelay_soclow = runtime;
		}
	}
	
	return alarmstate_soclow;
}

uint8_t faultalarm_voltotalhigh(float vol){
	float v = vol * CELL_NUM;
	alarmprevstate_voltotalhigh = alarmstate_voltotalhigh;

	if(v>alarmtrshold_voltotalhigh[2]*CELL_NUM&&alarmprevstate_voltotalhigh<3){
		if(alarmcheckstate_voltotalhigh==3&&runtime-alarmdelay_voltotalhigh>alarmdelaytrshold_voltotalhigh[2]&&alarmprevstate_voltotalhigh!=3){
			return alarmstate_voltotalhigh=3;
		}else if(alarmcheckstate_voltotalhigh!=3){
			alarmcheckstate_voltotalhigh=3;
			alarmdelay_voltotalhigh = runtime;
		}
	}else if(v>alarmtrshold_voltotalhigh[1]*CELL_NUM&&alarmprevstate_voltotalhigh<2){
		if(alarmcheckstate_voltotalhigh==2&&runtime-alarmdelay_voltotalhigh>alarmdelaytrshold_voltotalhigh[1]&&alarmprevstate_voltotalhigh!=2){
			return alarmstate_voltotalhigh=2;
		}else if(alarmcheckstate_voltotalhigh!=2){
			alarmcheckstate_voltotalhigh=2;
			alarmdelay_voltotalhigh = runtime;
		}
	}else if(v>alarmtrshold_voltotalhigh[0]*CELL_NUM&&alarmprevstate_voltotalhigh<1){
		if(alarmcheckstate_voltotalhigh==1&&runtime-alarmdelay_voltotalhigh>alarmdelaytrshold_voltotalhigh[0]&&alarmprevstate_voltotalhigh!=1){
			return alarmstate_voltotalhigh=1;
		}else if(alarmcheckstate_voltotalhigh!=1){
			alarmcheckstate_voltotalhigh=1;
			alarmdelay_voltotalhigh = runtime;
		}
	}else if(v<releasetrshold_voltotalhigh[0]*CELL_NUM&&alarmstate_voltotalhigh>=1&&alarmstate_voltotalhigh<3){
		if(alarmcheckstate_voltotalhigh==1+3&&runtime-releasedelay_voltotalhigh>releasedelaytrshold_voltotalhigh[0]){
			return alarmstate_voltotalhigh=0;
		}else if(alarmcheckstate_voltotalhigh!=1+3){
			alarmcheckstate_voltotalhigh=1+3;
			releasedelay_voltotalhigh = runtime;
		}
	}else if(v<releasetrshold_voltotalhigh[1]*CELL_NUM&&alarmstate_voltotalhigh>=2&&alarmstate_voltotalhigh<3){
		if(alarmcheckstate_voltotalhigh==2+3&&runtime-releasedelay_voltotalhigh>releasedelaytrshold_voltotalhigh[1]){
			return alarmstate_voltotalhigh=1;
		}else if(alarmcheckstate_voltotalhigh!=2+3){
			alarmcheckstate_voltotalhigh=2+3;
			releasedelay_voltotalhigh = runtime;
		}
	}
	return alarmstate_voltotalhigh;
}

uint8_t faultalarm_voltagelow(float vol){
	float v = vol * CELL_NUM;
	alarmprevstate_voltotallow = alarmstate_voltotallow;

	if(v<alarmtrshold_voltotallow[2]*CELL_NUM&&alarmstate_voltotallow<3){
		if(alarmcheckstate_voltotallow==3&&runtime-alarmdelay_voltotallow>alarmdelaytrshold_voltotallow[2]&&alarmprevstate_voltotallow!=3){
			return alarmstate_voltotallow=3;
		}else if(alarmcheckstate_voltotallow!=3){
			alarmcheckstate_voltotallow=3;
			alarmdelay_voltotallow = runtime;
		}
	}else if(v<alarmtrshold_voltotallow[1]*CELL_NUM&&alarmstate_voltotallow<2){
		if(alarmcheckstate_voltotallow==2&&runtime-alarmdelay_voltotallow>alarmdelaytrshold_voltotallow[1]&&alarmprevstate_voltotallow!=2){
			return alarmstate_voltotallow=2;
		}else if(alarmcheckstate_voltotallow!=2){
			alarmcheckstate_voltotallow=2;
			alarmdelay_voltotallow = runtime;
		}
	}else if(v<alarmtrshold_voltotallow[0]*CELL_NUM&&alarmstate_voltotallow<1){
		if(alarmcheckstate_voltotallow==1&&runtime-alarmdelay_voltotallow>alarmdelaytrshold_voltotallow[0]&&alarmprevstate_voltotallow!=1){
			return alarmstate_voltotallow=1;
		}else if(alarmcheckstate_voltotallow!=1){
			alarmcheckstate_voltotallow=1;
			alarmdelay_voltotallow = runtime;
		}
	}else if(v>releasetrshold_voltotallow[0]*CELL_NUM&&alarmstate_voltotallow>=1&&alarmstate_voltotallow<3){
		if(alarmcheckstate_voltotallow==1+3&&runtime-releasedelay_voltotallow>releasedelaytrshold_voltotallow[0]){
			return alarmstate_voltotallow=0;
		}else if(alarmcheckstate_voltotallow!=1+3){
			alarmcheckstate_voltotallow=1+3;
			releasedelay_voltotallow = runtime;
		}
	}else if(v>releasetrshold_voltotallow[1]*CELL_NUM&&alarmstate_voltotallow>=2&&alarmstate_voltotallow<3){
		if(alarmcheckstate_voltotallow==2+3&&runtime-releasedelay_voltotallow>releasedelaytrshold_voltotallow[1]){
			return alarmstate_voltotallow=1;
		}else if(alarmcheckstate_voltotallow!=2+3){
			alarmcheckstate_voltotallow=2+3;
			releasedelay_voltotallow = runtime;
		}
	}
	return alarmstate_voltotallow;
}

uint8_t faultalarm_cvolhigh(float cvol){
	alarmprevstate_cvolhigh = alarmstate_cvolhigh;

	if(cvol>alarmtrshold_cvolhigh[2]&&alarmstate_cvolhigh<3){
		if(alarmcheckstate_cvolhigh==3&&runtime-alarmdelay_cvolhigh>alarmdelaytrshold_cvolhigh[2]&&alarmprevstate_cvolhigh!=3){
			return alarmstate_cvolhigh=3;
		}else if(alarmcheckstate_cvolhigh!=3){
			alarmcheckstate_cvolhigh=3;
			alarmdelay_cvolhigh = runtime;
		}
	}else if(cvol>alarmtrshold_cvolhigh[1]&&alarmstate_cvolhigh<2){
		if(alarmcheckstate_cvolhigh==2&&runtime-alarmdelay_cvolhigh>alarmdelaytrshold_cvolhigh[1]&&alarmprevstate_cvolhigh!=2){
			return alarmstate_cvolhigh=2;
		}else if(alarmcheckstate_cvolhigh!=2){
			alarmcheckstate_cvolhigh=2;
			alarmdelay_cvolhigh = runtime;
		}
	}else if(cvol>alarmtrshold_cvolhigh[0]&&alarmstate_cvolhigh<1){
		if(alarmcheckstate_cvolhigh==1&&runtime-alarmdelay_cvolhigh>alarmdelaytrshold_cvolhigh[0]&&alarmprevstate_cvolhigh!=1){
			return alarmstate_cvolhigh=1;
		}else if(alarmcheckstate_cvolhigh!=1){
			alarmcheckstate_cvolhigh=1;
			alarmdelay_cvolhigh = runtime;
		}
	}else if(cvol<releasetrshold_cvolhigh[0]&&alarmstate_cvolhigh>=1&&alarmstate_cvolhigh<3){
		if(alarmcheckstate_cvolhigh==1+3&&runtime-releasedelay_cvolhigh>releasedelaytrshold_cvolhigh[0]){
			return alarmstate_cvolhigh=0;
		}else if(alarmcheckstate_cvolhigh!=1+3){
			alarmcheckstate_cvolhigh=1+3;
			releasedelay_cvolhigh = runtime;
		}
	}else if(cvol<releasetrshold_cvolhigh[1]&&alarmstate_cvolhigh>=2&&alarmstate_cvolhigh<3){
		if(alarmcheckstate_cvolhigh==2+3&&runtime-releasedelay_cvolhigh>releasedelaytrshold_cvolhigh[1]){
			return alarmstate_cvolhigh=1;
		}else if(alarmcheckstate_cvolhigh!=2+3){
			alarmcheckstate_cvolhigh=2+3;
			releasedelay_cvolhigh = runtime;
		}
	}
	return alarmstate_cvolhigh;
}

uint8_t faultalarm_cvollow(float cvol){
	alarmprevstate_cvollow = alarmstate_cvollow;

	if(cvol<alarmtrshold_cvollow[2]&&alarmstate_cvollow<3){
		if(alarmcheckstate_cvollow==3&&runtime-alarmdelay_cvollow>alarmdelaytrshold_cvollow[2]&&alarmprevstate_cvollow!=3){
			return alarmstate_cvollow=3;
		}else if(alarmcheckstate_cvollow!=3){
			alarmcheckstate_cvollow=3;
			alarmdelay_cvollow = runtime;
		}
	}else if(cvol<alarmtrshold_cvollow[1]&&alarmstate_cvollow<2){
		if(alarmcheckstate_cvollow==2&&runtime-alarmdelay_cvollow>alarmdelaytrshold_cvollow[1]&&alarmprevstate_cvollow!=2){
			return alarmstate_cvollow=2;
		}else if(alarmcheckstate_cvollow!=2){
			alarmcheckstate_cvollow=2;
			alarmdelay_cvollow = runtime;
		}
	}else if(cvol<alarmtrshold_cvollow[0]&&alarmstate_cvollow<1){
		if(alarmcheckstate_cvollow==1&&runtime-alarmdelay_cvollow>alarmdelaytrshold_cvollow[0]&&alarmprevstate_cvollow!=1){
			return alarmstate_cvollow=1;
		}else if(alarmcheckstate_cvollow!=1){
			alarmcheckstate_cvollow=1;
			alarmdelay_cvollow = runtime;
		}
	}else if(cvol>releasetrshold_cvollow[0]&&alarmstate_cvollow>=1&&alarmstate_cvollow<3){
		if(alarmcheckstate_cvollow==1+3&&runtime-releasedelay_cvollow>releasedelaytrshold_cvollow[0]){
			return alarmstate_cvollow=0;
		}else if(alarmcheckstate_cvollow!=1+3){
			alarmcheckstate_cvollow=1+3;
			releasedelay_cvollow = runtime;
		}
	}else if(cvol>releasetrshold_cvollow[1]&&alarmstate_cvollow>=2&&alarmstate_cvollow<3){
		if(alarmcheckstate_cvollow==2+3&&runtime-releasedelay_cvollow>releasedelaytrshold_cvollow[1]){
			return alarmstate_cvollow=1;
		}else if(alarmcheckstate_cvollow!=2+3){
			alarmcheckstate_cvollow=2+3;
			releasedelay_cvollow = runtime;
		}
	}
	return alarmstate_cvollow;
}

uint8_t faultalarm_dscvolhigh(float dscvol){
	alarmprevstate_cvolhigh = alarmstate_cvolhigh;

	if(dscvol>alarmtrshold_dscvolhigh[2]&&alarmstate_dscvolhigh<3){
		if(alarmcheckstate_dscvolhigh==3&&runtime-alarmdelay_dscvolhigh>alarmdelaytrshold_dscvolhigh[2]&&alarmprevstate_dscvolhigh!=3){
			return alarmstate_dscvolhigh=3;
		}else if(alarmcheckstate_dscvolhigh!=3){
			alarmcheckstate_dscvolhigh=3;
			alarmdelay_dscvolhigh = runtime;
		}
	}else if(dscvol>alarmtrshold_dscvolhigh[1]&&alarmstate_dscvolhigh<2){
		if(alarmcheckstate_dscvolhigh==2&&runtime-alarmdelay_dscvolhigh>alarmdelaytrshold_dscvolhigh[1]&&alarmprevstate_dscvolhigh!=2){
			return alarmstate_dscvolhigh=2;
		}else if(alarmcheckstate_dscvolhigh!=2){
			alarmcheckstate_dscvolhigh=2;
			alarmdelay_dscvolhigh = runtime;
		}
	}else if(dscvol>alarmtrshold_dscvolhigh[0]&&alarmstate_dscvolhigh<1){
		if(alarmcheckstate_dscvolhigh==1&&runtime-alarmdelay_dscvolhigh>alarmdelaytrshold_dscvolhigh[0]&&alarmprevstate_dscvolhigh!=1){
			return alarmstate_dscvolhigh=1;
		}else if(alarmcheckstate_dscvolhigh!=1){
			alarmcheckstate_dscvolhigh=1;
			alarmdelay_dscvolhigh = runtime;
		}
	}else if(dscvol<releasetrshold_dscvolhigh[0]&&alarmstate_dscvolhigh>=1&&alarmstate_dscvolhigh<3){
		if(alarmcheckstate_dscvolhigh==1+3&&runtime-releasedelay_dscvolhigh>releasedelaytrshold_dscvolhigh[0]){
			return alarmstate_dscvolhigh=0;
		}else if(alarmcheckstate_dscvolhigh!=1+3){
			alarmcheckstate_dscvolhigh=1+3;
			releasedelay_dscvolhigh = runtime;
		}
	}else if(dscvol<releasetrshold_dscvolhigh[1]&&alarmstate_dscvolhigh>=2&&alarmstate_dscvolhigh<3){
		if(alarmcheckstate_dscvolhigh==2+3&&runtime-releasedelay_dscvolhigh>releasedelaytrshold_dscvolhigh[1]){
			return alarmstate_dscvolhigh=1;
		}else if(alarmcheckstate_dscvolhigh!=2+3){
			alarmcheckstate_dscvolhigh=2+3;
			releasedelay_dscvolhigh = runtime;
		}
	}
	return alarmstate_dscvolhigh;
}

uint8_t faultalarm_dscvollow(float dscvol){
	alarmprevstate_dscvollow = alarmstate_dscvollow;

	if(dscvol<alarmtrshold_dscvollow[2]&&alarmstate_dscvollow<3){
		if(alarmcheckstate_dscvollow==3&&runtime-alarmdelay_dscvollow>alarmdelaytrshold_dscvollow[2]&&alarmprevstate_dscvollow!=3){
			return alarmstate_dscvollow=3;
		}else if(alarmcheckstate_dscvollow!=3){
			alarmcheckstate_dscvollow=3;
			alarmdelay_dscvollow = runtime;
		}
	}else if(dscvol<alarmtrshold_dscvollow[1]&&alarmstate_dscvollow<2){
		if(alarmcheckstate_dscvollow==2&&runtime-alarmdelay_dscvollow>alarmdelaytrshold_dscvollow[1]&&alarmprevstate_dscvollow!=2){
			return alarmstate_dscvollow=2;
		}else if(alarmcheckstate_dscvollow!=2){
			alarmcheckstate_dscvollow=2;
			alarmdelay_dscvollow = runtime;
		}
	}else if(dscvol<alarmtrshold_dscvollow[0]&&alarmstate_dscvollow<1){
		if(alarmcheckstate_dscvollow==1&&runtime-alarmdelay_dscvollow>alarmdelaytrshold_dscvollow[0]&&alarmprevstate_dscvollow!=1){
			return alarmstate_dscvollow=1;
		}else if(alarmcheckstate_dscvollow!=1){
			alarmcheckstate_dscvollow=1;
			alarmdelay_dscvollow = runtime;
		}
	}else if(dscvol>releasetrshold_dscvollow[0]&&alarmstate_dscvollow>=1&&alarmstate_dscvollow<3){
		if(alarmcheckstate_dscvollow==1+3&&runtime-releasedelay_dscvollow>releasedelaytrshold_dscvollow[0]){
			return alarmstate_dscvollow=0;
		}else if(alarmcheckstate_dscvollow!=1+3){
			alarmcheckstate_dscvollow=1+3;
			releasedelay_dscvollow = runtime;
		}
	}else if(dscvol>releasetrshold_dscvollow[1]&&alarmstate_dscvollow>=2&&alarmstate_dscvollow<3){
		if(alarmcheckstate_dscvollow==2+3&&runtime-releasedelay_dscvollow>releasedelaytrshold_dscvollow[1]){
			return alarmstate_dscvollow=1;
		}else if(alarmcheckstate_dscvollow!=2+3){
			alarmcheckstate_dscvollow=2+3;
			releasedelay_dscvollow = runtime;
		}
	}
	return alarmstate_dscvollow;
}

uint8_t faultalarm_cvoldiff(float diff){
	alarmprevstate_cvoldiff = alarmstate_cvoldiff;

	if(diff>alarmtrshold_cvoldiff[2]&&alarmstate_cvoldiff<3){
		if(alarmcheckstate_cvoldiff==3&&runtime-alarmdelay_cvoldiff>alarmdelaytrshold_cvoldiff[2]){
			return alarmstate_cvoldiff=3;
		}else if(alarmcheckstate_cvoldiff!=3){
			alarmcheckstate_cvoldiff=3;
			alarmdelay_cvoldiff = runtime;
		}
	}else if(diff>alarmtrshold_cvoldiff[1]&&alarmstate_cvoldiff<2){
		if(alarmcheckstate_cvoldiff==2&&runtime-alarmdelay_cvoldiff>alarmdelaytrshold_cvoldiff[1]){
			return alarmstate_cvoldiff=2;
		}else if(alarmcheckstate_cvoldiff!=2){
			alarmcheckstate_cvoldiff=2;
			alarmdelay_cvoldiff = runtime;
		}
	}else if(diff<releasetrshold_cvoldiff[1]&&alarmstate_cvoldiff==2){
		if(alarmcheckstate_cvoldiff==2+3&&runtime-releasedelay_cvoldiff>releasedelaytrshold_cvoldiff[1]){
			return alarmstate_cvoldiff=0;
		}else if(alarmcheckstate_cvoldiff!=2+3){
			alarmcheckstate_cvoldiff=2+3;
			releasedelay_cvoldiff = runtime;
		}
	}else if(diff<releasetrshold_cvoldiff[2]&&alarmstate_cvoldiff==3){
		if(alarmcheckstate_cvoldiff==3+3&&runtime-releasedelay_cvoldiff>releasedelaytrshold_cvoldiff[2]){
			return alarmstate_cvoldiff=2;
		}else if(alarmcheckstate_cvoldiff!=3+3){
			alarmcheckstate_cvoldiff=3+3;
			releasedelay_cvoldiff = runtime;
		}
	}
	return alarmstate_cvoldiff;
}

uint8_t faultalarm_dscvoldiff(float diff){
	alarmprevstate_dscvoldiff = alarmstate_dscvoldiff;

	if(diff>alarmtrshold_dscvoldiff[2]&&alarmstate_dscvoldiff<3){
		if(alarmcheckstate_dscvoldiff==3&&runtime-alarmdelay_dscvoldiff>alarmdelaytrshold_dscvoldiff[2]){
			return alarmstate_dscvoldiff=3;
		}else if(alarmcheckstate_dscvoldiff!=3){
			alarmcheckstate_dscvoldiff=3;
			alarmdelay_dscvoldiff = runtime;
		}
	}else if(diff>alarmtrshold_dscvoldiff[1]&&alarmstate_dscvoldiff<2){
		if(alarmcheckstate_dscvoldiff==2&&runtime-alarmdelay_dscvoldiff>alarmdelaytrshold_dscvoldiff[1]){
			return alarmstate_dscvoldiff=2;
		}else if(alarmcheckstate_dscvoldiff!=2){
			alarmcheckstate_dscvoldiff=2;
			alarmdelay_dscvoldiff = runtime;
		}
	}else if(diff<releasetrshold_dscvoldiff[1]&&alarmstate_dscvoldiff==2){
		if(alarmcheckstate_dscvoldiff==2+3&&runtime-releasedelay_dscvoldiff>releasedelaytrshold_dscvoldiff[1]){
			return alarmstate_dscvoldiff=0;
		}else if(alarmcheckstate_dscvoldiff!=2+3){
			alarmcheckstate_dscvoldiff=2+3;
			releasedelay_dscvoldiff = runtime;
		}
	}else if(diff<releasetrshold_dscvoldiff[2]&&alarmstate_dscvoldiff==3){
		if(alarmcheckstate_dscvoldiff==3+3&&runtime-releasedelay_dscvoldiff>releasedelaytrshold_dscvoldiff[2]){
			return alarmstate_dscvoldiff=2;
		}else if(alarmcheckstate_dscvoldiff!=3+3){
			alarmcheckstate_dscvoldiff=3+3;
			releasedelay_dscvoldiff = runtime;
		}
	}
	return alarmstate_dscvoldiff;
}

uint8_t faultalarm_volwire(float volwire){
	return alarmstate_volwire;
}

uint8_t faultalarm_ctemphigh(float ctemp){
	alarmprevstate_ctemphigh = alarmstate_ctemphigh;

	if(ctemp>alarmtrshold_ctemphigh[2]&&alarmstate_ctemphigh<3){
		if(alarmcheckstate_ctemphigh==3&&runtime-alarmdelay_ctemphigh>alarmdelaytrshold_ctemphigh[2]){
			return alarmstate_ctemphigh=3;
		}else if(alarmcheckstate_ctemphigh!=3){
			alarmcheckstate_ctemphigh=3;
			alarmdelay_ctemphigh = runtime;
		}
	}else if(ctemp>alarmtrshold_ctemphigh[1]&&alarmstate_ctemphigh<2){
		if(alarmcheckstate_ctemphigh==2&&runtime-alarmdelay_ctemphigh>alarmdelaytrshold_ctemphigh[1]){
			return alarmstate_ctemphigh=2;
		}else if(alarmcheckstate_ctemphigh!=2){
			alarmcheckstate_ctemphigh=2;
			alarmdelay_ctemphigh = runtime;
		}
	}else if(ctemp>alarmtrshold_ctemphigh[0]&&alarmstate_ctemphigh<1){
		if(alarmcheckstate_ctemphigh==1&&runtime-alarmdelay_ctemphigh>alarmdelaytrshold_ctemphigh[0]){
			return alarmstate_ctemphigh=1;
		}else if(alarmcheckstate_ctemphigh!=1){
			alarmcheckstate_ctemphigh=1;
			alarmdelay_ctemphigh = runtime;
		}
	}else if(ctemp<releasetrshold_ctemphigh[0]&&alarmstate_ctemphigh>=1){
		if(alarmcheckstate_ctemphigh==1+3&&runtime-releasedelay_ctemphigh>releasedelaytrshold_ctemphigh[0]){
			return alarmstate_ctemphigh=0;
		}else if(alarmcheckstate_ctemphigh!=1+3){
			alarmcheckstate_ctemphigh=1+3;
			releasedelay_ctemphigh = runtime;
		}
	}else if(ctemp<releasetrshold_ctemphigh[1]&&alarmstate_ctemphigh>=2){
		if(alarmcheckstate_ctemphigh==2+3&&runtime-releasedelay_ctemphigh>releasedelaytrshold_ctemphigh[1]){
			return alarmstate_ctemphigh=1;
		}else if(alarmcheckstate_ctemphigh!=2+3){
			alarmcheckstate_ctemphigh=2+3;
			releasedelay_ctemphigh = runtime;
		}
	}else if(ctemp<releasetrshold_ctemphigh[2]&&alarmstate_ctemphigh>=3){
		if(alarmcheckstate_ctemphigh==3+3&&runtime-releasedelay_ctemphigh>releasedelaytrshold_ctemphigh[2]){
			return alarmstate_ctemphigh=2;
		}else if(alarmcheckstate_ctemphigh!=3+3){
			alarmcheckstate_ctemphigh=3+3;
			releasedelay_ctemphigh = runtime;
		}
	}
	
	return alarmstate_ctemphigh;
}

uint8_t faultalarm_dsctemphigh(float dsctemp){
	alarmprevstate_dsctemphigh = alarmstate_dsctemphigh;

	if(dsctemp>alarmtrshold_dsctemphigh[2]&&alarmstate_dsctemphigh<3){
		if(alarmcheckstate_dsctemphigh==3&&runtime-alarmdelay_dsctemphigh>alarmdelaytrshold_dsctemphigh[2]){
			return alarmstate_dsctemphigh=3;
		}else if(alarmcheckstate_dsctemphigh!=3){
			alarmcheckstate_dsctemphigh=3;
			alarmdelay_dsctemphigh = runtime;
		}
	}else if(dsctemp>alarmtrshold_dsctemphigh[1]&&alarmstate_dsctemphigh<2){
		if(alarmcheckstate_dsctemphigh==2&&runtime-alarmdelay_dsctemphigh>alarmdelaytrshold_dsctemphigh[1]){
			return alarmstate_dsctemphigh=2;
		}else if(alarmcheckstate_dsctemphigh!=2){
			alarmcheckstate_dsctemphigh=2;
			alarmdelay_dsctemphigh = runtime;
		}
	}else if(dsctemp>alarmtrshold_dsctemphigh[0]&&alarmstate_dsctemphigh<1){
		if(alarmcheckstate_dsctemphigh==1&&runtime-alarmdelay_dsctemphigh>alarmdelaytrshold_dsctemphigh[0]){
			return alarmstate_dsctemphigh=1;
		}else if(alarmcheckstate_dsctemphigh!=1){
			alarmcheckstate_dsctemphigh=1;
			alarmdelay_dsctemphigh = runtime;
		}
	}else if(dsctemp<releasetrshold_dsctemphigh[0]&&alarmstate_dsctemphigh>=1){
		if(alarmcheckstate_dsctemphigh==1+3&&runtime-releasedelay_dsctemphigh>releasedelaytrshold_dsctemphigh[0]){
			return alarmstate_dsctemphigh=0;
		}else if(alarmcheckstate_dsctemphigh!=1+3){
			alarmcheckstate_dsctemphigh=1+3;
			releasedelay_dsctemphigh = runtime;
		}
	}else if(dsctemp<releasetrshold_dsctemphigh[1]&&alarmstate_dsctemphigh>=2){
		if(alarmcheckstate_dsctemphigh==2+3&&runtime-releasedelay_dsctemphigh>releasedelaytrshold_dsctemphigh[1]){
			return alarmstate_dsctemphigh=1;
		}else if(alarmcheckstate_dsctemphigh!=2+3){
			alarmcheckstate_dsctemphigh=2+3;
			releasedelay_dsctemphigh = runtime;
		}
	}else if(dsctemp<releasetrshold_dsctemphigh[2]&&alarmstate_dsctemphigh>=3){
		if(alarmcheckstate_dsctemphigh==3+3&&runtime-releasedelay_dsctemphigh>releasedelaytrshold_dsctemphigh[2]){
			return alarmstate_dsctemphigh=2;
		}else if(alarmcheckstate_dsctemphigh!=3+3){
			alarmcheckstate_dsctemphigh=3+3;
			releasedelay_dsctemphigh = runtime;
		}
	}
	
	return alarmstate_dsctemphigh;	
}

uint8_t faultalarm_ctemplow(float ctemp){
	alarmprevstate_ctemplow = alarmstate_ctemplow;

	if(ctemp<alarmtrshold_ctemplow[2]&&alarmstate_ctemplow<3){
		if(alarmcheckstate_ctemplow==3&&runtime-alarmdelay_ctemplow>alarmdelaytrshold_ctemplow[2]){
			return alarmstate_ctemplow=3;
		}else if(alarmcheckstate_ctemplow!=3){
			alarmcheckstate_ctemplow=3;
			alarmdelay_ctemplow = runtime;
		}
	}else if(ctemp<alarmtrshold_ctemplow[1]&&alarmstate_ctemplow<2){
		if(alarmcheckstate_ctemplow==2&&runtime-alarmdelay_ctemplow>alarmdelaytrshold_ctemplow[1]){
			return alarmstate_ctemplow=2;
		}else if(alarmcheckstate_ctemplow!=2){
			alarmcheckstate_ctemplow=2;
			alarmdelay_ctemplow = runtime;
		}
	}else if(ctemp<alarmtrshold_ctemplow[0]&&alarmstate_ctemplow<1){
		if(alarmcheckstate_ctemplow==1&&runtime-alarmdelay_ctemplow>alarmdelaytrshold_ctemplow[0]){
			return alarmstate_ctemplow=1;
		}else if(alarmcheckstate_ctemplow!=1){
			alarmcheckstate_ctemplow=1;
			alarmdelay_ctemplow = runtime;
		}
	}else if(ctemp>releasetrshold_ctemplow[0]&&alarmstate_ctemplow>=1){
		if(alarmcheckstate_ctemplow==1+3&&runtime-releasedelay_ctemplow>releasedelaytrshold_ctemplow[0]){
			return alarmstate_ctemplow=0;
		}else if(alarmcheckstate_ctemplow!=1+3){
			alarmcheckstate_ctemplow=1+3;
			releasedelay_ctemplow = runtime;
		}
	}else if(ctemp>releasetrshold_ctemplow[1]&&alarmstate_ctemplow>=2){
		if(alarmcheckstate_ctemplow==2+3&&runtime-releasedelay_ctemplow>releasedelaytrshold_ctemplow[1]){
			return alarmstate_ctemplow=1;
		}else if(alarmcheckstate_ctemplow!=2+3){
			alarmcheckstate_ctemplow=2+3;
			releasedelay_ctemplow = runtime;
		}
	}else if(ctemp>releasetrshold_ctemplow[2]&&alarmstate_ctemplow>=3){
		if(alarmcheckstate_ctemplow==3+3&&runtime-releasedelay_ctemplow>releasedelaytrshold_ctemplow[2]){
			return alarmstate_ctemplow=2;
		}else if(alarmcheckstate_ctemplow!=3+3){
			alarmcheckstate_ctemplow=3+3;
			releasedelay_ctemplow = runtime;
		}
	}
	
	return alarmstate_ctemplow;
}

uint8_t faultalarm_dsctemplow(float dsctemp){
	alarmprevstate_dsctemplow = alarmstate_dsctemplow;

	if(dsctemp<alarmtrshold_dsctemplow[2]&&alarmstate_dsctemplow<3){
		if(alarmcheckstate_dsctemplow==3&&runtime-alarmdelay_dsctemplow>alarmdelaytrshold_dsctemplow[2]){
			return alarmstate_dsctemplow=3;
		}else if(alarmcheckstate_dsctemplow!=3){
			alarmcheckstate_dsctemplow=3;
			alarmdelay_dsctemplow = runtime;
		}
	}else if(dsctemp<alarmtrshold_dsctemplow[1]&&alarmstate_dsctemplow<2){
		if(alarmcheckstate_dsctemplow==2&&runtime-alarmdelay_dsctemplow>alarmdelaytrshold_dsctemplow[1]){
			return alarmstate_dsctemplow=2;
		}else if(alarmcheckstate_dsctemplow!=2){
			alarmcheckstate_dsctemplow=2;
			alarmdelay_dsctemplow = runtime;
		}
	}else if(dsctemp<alarmtrshold_dsctemplow[0]&&alarmstate_dsctemplow<1){
		if(alarmcheckstate_dsctemplow==1&&runtime-alarmdelay_dsctemplow>alarmdelaytrshold_dsctemplow[0]){
			return alarmstate_dsctemplow=1;
		}else if(alarmcheckstate_dsctemplow!=1){
			alarmcheckstate_dsctemplow=1;
			alarmdelay_dsctemplow = runtime;
		}
	}else if(dsctemp>releasetrshold_dsctemplow[0]&&alarmstate_dsctemplow>=1){
		if(alarmcheckstate_dsctemplow==1+3&&runtime-releasedelay_dsctemplow>releasedelaytrshold_dsctemplow[0]){
			return alarmstate_dsctemplow=0;
		}else if(alarmcheckstate_dsctemplow!=1+3){
			alarmcheckstate_dsctemplow=1+3;
			releasedelay_dsctemplow = runtime;
		}
	}else if(dsctemp>releasetrshold_dsctemplow[1]&&alarmstate_dsctemplow>=2){
		if(alarmcheckstate_dsctemplow==2+3&&runtime-releasedelay_dsctemplow>releasedelaytrshold_dsctemplow[1]){
			return alarmstate_dsctemplow=1;
		}else if(alarmcheckstate_dsctemplow!=2+3){
			alarmcheckstate_dsctemplow=2+3;
			releasedelay_dsctemplow = runtime;
		}
	}else if(dsctemp>releasetrshold_dsctemplow[2]&&alarmstate_dsctemplow>=3){
		if(alarmcheckstate_dsctemplow==3+3&&runtime-releasedelay_dsctemplow>releasedelaytrshold_dsctemplow[2]){
			return alarmstate_dsctemplow=2;
		}else if(alarmcheckstate_dsctemplow!=3+3){
			alarmcheckstate_dsctemplow=3+3;
			releasedelay_dsctemplow = runtime;
		}
	}
	
	return alarmstate_dsctemplow;
}

uint8_t faultalarm_ctempdiff(float diff){
	alarmprevstate_ctempdiff = alarmstate_ctempdiff;

	if(diff>alarmtrshold_ctempdiff[2]&&alarmstate_ctempdiff<3){
		if(alarmcheckstate_ctempdiff==3&&runtime-alarmdelay_ctempdiff>alarmdelaytrshold_ctempdiff[2]){
			return alarmstate_ctempdiff=3;
		}else if(alarmcheckstate_ctempdiff!=3){
			alarmcheckstate_ctempdiff=3;
			alarmdelay_dsctempdiff = runtime;
		}
	}else if(diff<alarmtrshold_ctempdiff[2]&&alarmstate_ctempdiff==3){
		if(alarmcheckstate_ctempdiff==3+3&&runtime-releasedelay_ctempdiff>releasedelaytrshold_ctempdiff[2]){
			return alarmstate_ctempdiff=2;
		}else if(alarmcheckstate_ctempdiff!=3+3){
			alarmcheckstate_ctempdiff=3+3;
			releasedelay_ctempdiff = runtime;
		}
	}
	
	return alarmstate_dscovercurr;
}

uint8_t faultalarm_dsctempdiff(float diff){
	alarmprevstate_dsctempdiff = alarmstate_dsctempdiff;

	if(diff>alarmtrshold_dsctempdiff[2]&&alarmstate_dsctempdiff<3){
		if(alarmcheckstate_dsctempdiff==3&&runtime-alarmdelay_dsctempdiff>alarmdelaytrshold_dsctempdiff[2]){
			return alarmstate_dsctempdiff=3;
		}else if(alarmcheckstate_dsctempdiff!=3){
			alarmcheckstate_dsctempdiff=3;
			alarmdelay_dsctempdiff = runtime;
		}
	}else if(diff>alarmtrshold_dsctempdiff[1]&&alarmstate_dsctempdiff<2){
		if(alarmcheckstate_dsctempdiff==2&&runtime-alarmdelay_dsctempdiff>alarmdelaytrshold_dsctempdiff[1]){
			return alarmstate_dsctempdiff=2;
		}else if(alarmcheckstate_dsctempdiff!=2){
			alarmcheckstate_dsctempdiff=2;
			alarmdelay_dsctempdiff = runtime;
		}
	}else if(diff<alarmtrshold_dsctempdiff[1]&&alarmstate_dsctempdiff>=2){
		if(alarmcheckstate_dsctempdiff==2+3&&runtime-releasedelay_dsctempdiff>releasedelaytrshold_dsctempdiff[1]){
			return alarmstate_dsctempdiff=0;
		}else if(alarmcheckstate_dsctempdiff!=2+3){
			alarmcheckstate_dsctempdiff=2+3;
			releasedelay_dsctempdiff = runtime;
		}
	}else if(diff<alarmtrshold_dsctempdiff[2]&&alarmstate_dsctempdiff==3){
		if(alarmcheckstate_dsctempdiff==3+3&&runtime-releasedelay_dsctempdiff>releasedelaytrshold_dsctempdiff[2]){
			return alarmstate_dsctempdiff=2;
		}else if(alarmcheckstate_dsctempdiff!=3+3){
			alarmcheckstate_dsctempdiff=3+3;
			releasedelay_dsctempdiff = runtime;
		}
	}
	
	return alarmstate_dscovercurr;
}

uint8_t faultalarm_covercurr(float ccurr){
	alarmprevstate_covercurr = alarmstate_covercurr;

	if(ccurr>alarmtrshold_covercurr[2]&&alarmstate_covercurr<3){
		if(alarmcheckstate_covercurr==3&&runtime-alarmdelay_covercurr>alarmdelaytrshold_covercurr[2]){
			return alarmstate_covercurr=3;
		}else if(alarmcheckstate_covercurr!=3){
			alarmcheckstate_covercurr=3;
			alarmdelay_covercurr = runtime;
		}
	}else if(ccurr>alarmtrshold_covercurr[1]&&alarmstate_covercurr<2){
		if(alarmcheckstate_covercurr==2&&runtime-alarmdelay_covercurr>alarmdelaytrshold_covercurr[1]){
			return alarmstate_covercurr=2;
		}else if(alarmcheckstate_covercurr!=2){
			alarmcheckstate_covercurr=2;
			alarmdelay_covercurr = runtime;
		}
	}else if(ccurr<alarmtrshold_covercurr[1]&&alarmstate_covercurr==2){
		if(alarmcheckstate_covercurr==2+3&&runtime-releasedelay_covercurr>releasedelaytrshold_covercurr[1]){
			return alarmstate_covercurr=0;
		}else if(alarmcheckstate_covercurr!=2+3){
			alarmcheckstate_covercurr=2+3;
			releasedelay_covercurr = runtime;
		}
	}
	
	return alarmstate_covercurr;
}

uint8_t faultalarm_dscovercurr(float dsccurr){
	alarmprevstate_dscovercurr = alarmstate_dscovercurr;

	if(dsccurr>alarmtrshold_dscovercurr[2]&&alarmstate_dscovercurr<3){
		if(alarmcheckstate_dscovercurr==3&&runtime-alarmdelay_dscovercurr>alarmdelaytrshold_dscovercurr[2]){
			return alarmstate_dscovercurr=3;
		}else if(alarmcheckstate_dscovercurr!=3){
			alarmcheckstate_dscovercurr=3;
			alarmdelay_dscovercurr = runtime;
		}
	}else if(dsccurr>alarmtrshold_dscovercurr[1]&&alarmstate_dscovercurr<2){
		if(alarmcheckstate_dscovercurr==2&&runtime-alarmdelay_dscovercurr>alarmdelaytrshold_dscovercurr[1]){
			return alarmstate_dscovercurr=2;
		}else if(alarmcheckstate_dscovercurr!=2){
			alarmcheckstate_dscovercurr=2;
			alarmdelay_dscovercurr = runtime;
		}
	}else if(dsccurr<alarmtrshold_dscovercurr[1]&&alarmstate_dscovercurr==2){
		if(alarmcheckstate_dscovercurr==2+3&&runtime-releasedelay_dscovercurr>releasedelaytrshold_dscovercurr[1]){
			return alarmstate_dscovercurr=0;
		}else if(alarmcheckstate_dscovercurr!=2+3){
			alarmcheckstate_dscovercurr=2+3;
			releasedelay_dscovercurr = runtime;
		}
	}
	
	return alarmstate_dscovercurr;
}

uint8_t faultalarm_insuleak(float insul){
	alarmprevstate_insuleak = alarmstate_insuleak;

	if(insul<alarmtrshold_insuleak[2]&&alarmstate_insuleak<3){
		if(alarmcheckstate_insuleak==3&&runtime-alarmdelay_insuleak>alarmdelaytrshold_insuleak[2]){
			return alarmstate_insuleak=3;
		}else if(alarmcheckstate_insuleak!=3){
			alarmcheckstate_insuleak=3;
			alarmdelay_insuleak = runtime;
		}
	}else if(insul<alarmtrshold_insuleak[1]&&alarmstate_insuleak<2){
		if(alarmcheckstate_insuleak==2&&runtime-alarmdelay_insuleak>alarmdelaytrshold_insuleak[1]){
			return alarmstate_insuleak=2;
		}else if(alarmcheckstate_insuleak!=2){
			alarmcheckstate_insuleak=2;
			alarmdelay_insuleak = runtime;
		}
	}else if(insul<alarmtrshold_insuleak[0]&&alarmstate_insuleak<1){
		if(alarmcheckstate_insuleak==1&&runtime-alarmdelay_insuleak>alarmdelaytrshold_insuleak[0]){
			return alarmstate_insuleak=1;
		}else if(alarmcheckstate_insuleak!=1){
			alarmcheckstate_insuleak=1;
			alarmdelay_insuleak = runtime;
		}
	}else if(insul>releasetrshold_insuleak[0]&&alarmstate_insuleak>=1){
		if(alarmcheckstate_insuleak==1+3&&runtime-releasedelay_insuleak>releasedelaytrshold_insuleak[0]){
			return alarmstate_insuleak=0;
		}else if(alarmcheckstate_insuleak!=1+3){
			alarmcheckstate_insuleak=1+3;
			releasedelay_insuleak = runtime;
		}
	}
	
	return alarmstate_insuleak;
}

uint8_t faultalarm_temprise(float temprise){
	alarmprevstate_temprise = alarmstate_temprise;

	if(temprise>alarmtrshold_temprise[2]&&alarmstate_temprise<3){
		if(alarmcheckstate_temprise==3&&runtime-alarmdelay_temprise>alarmdelaytrshold_temprise[2]){
			return alarmstate_temprise=3;
		}else if(alarmcheckstate_temprise!=3){
			alarmcheckstate_temprise=3;
			alarmdelay_temprise = runtime;
		}
	}else if(temprise>alarmtrshold_temprise[1]&&alarmstate_temprise<2){
		if(alarmcheckstate_temprise==2&&runtime-alarmdelay_temprise>alarmdelaytrshold_temprise[1]){
			return alarmstate_temprise=2;
		}else if(alarmcheckstate_temprise!=2){
			alarmcheckstate_temprise=2;
			alarmdelay_temprise = runtime;
		}
	}else if(temprise>alarmtrshold_temprise[0]&&alarmstate_temprise<1){
		if(alarmcheckstate_temprise==1&&runtime-alarmdelay_temprise>alarmdelaytrshold_temprise[0]){
			return alarmstate_temprise=1;
		}else if(alarmcheckstate_temprise!=1){
			alarmcheckstate_temprise=1;
			alarmdelay_temprise = runtime;
		}
	}else if(temprise<releasetrshold_temprise[0]&&alarmstate_temprise>=1){
		if(alarmcheckstate_temprise==1+3&&runtime-releasedelay_temprise>releasedelaytrshold_temprise[0]){
			return alarmstate_temprise=0;
		}else if(alarmcheckstate_temprise!=1+3){
			alarmcheckstate_temprise=1+3;
			releasedelay_temprise = runtime;
		}
	}else if(temprise<releasetrshold_temprise[1]&&alarmstate_temprise>=2){
		if(alarmcheckstate_temprise==2+3&&runtime-releasedelay_temprise>releasedelaytrshold_temprise[1]){
			return alarmstate_temprise=1;
		}else if(alarmcheckstate_temprise!=2+3){
			alarmcheckstate_temprise=2+3;
			releasedelay_temprise = runtime;
		}
	}else if(temprise<releasetrshold_temprise[2]&&alarmstate_temprise>=3){
		if(alarmcheckstate_temprise==3+3&&runtime-releasedelay_temprise>releasedelaytrshold_temprise[2]){
			return alarmstate_temprise=2;
		}else if(alarmcheckstate_temprise!=3+3){
			alarmcheckstate_temprise=3+3;
			releasedelay_temprise = runtime;
		}
	}
	
	return alarmstate_temprise;
}

void faulthandle_soclow3(){
  //����Ϊ0���µ�
  //data3091 = 0x10;
  //��ΪChargerʱ���д���
}

void faulthandle_soclow2(){
  //data3091 = 0x10;
  //data3073 = base_curr + standard_data3073/5;  //������20%
  RS485_Screen_WriteMem(SCREEN_SOCLOW, SCREEN_ALARM_ORANGE);
}

void faulthandle_soclow1(){
  //data3091 = 0x10;
  //data3073 = base_curr + standard_data3073/2;  //������50%
  RS485_Screen_WriteMem(SCREEN_SOCLOW, SCREEN_ALARM_YELLOW);
}

void faulthandle_soclow0(){
  //data3091 = 0x00;
  //data3073 = base_curr + standard_data3073;  //�ָ�����
  RS485_Screen_WriteMem(SCREEN_SOCLOW, SCREEN_ALARM_GREEN);
}

void faulthandle_voltotalhigh3(){
  //����������ϵ���Ϣ
  data3073 = base_curr;
  data3096 = 0;
  set_BST_data(1<<2, 0, 0);
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler); 
  //�жϼ̵���
  RS485_Screen_WriteMem(SCREEN_CVOLTOTALHIGH, SCREEN_ALARM_RED);  //��Ļ��ɫ
  //����
}

void faulthandle_voltotalhigh2(){
  data3073 = base_curr;   //����Ϊ0����������Ϊ0
  data3096 = 0;
  set_BST_data(1<<2, 0, 0);
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler);
  RS485_Screen_WriteMem(SCREEN_CVOLTOTALHIGH, SCREEN_ALARM_ORANGE);  //�ı���Ļ��ʾ��ɫ
}

void faulthandle_voltotalhigh1(){
  data3073 = base_curr + standard_data3073/2;  //������50%
  data3096 = 0;
  set_BST_data(1<<2, 0, 0);
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler);
  RS485_Screen_WriteMem(SCREEN_CVOLTOTALHIGH, SCREEN_ALARM_YELLOW);  //��Ļ��ɫ
}

void faulthandle_voltotalhigh0(){
  data3073 = base_curr + standard_data3073;  //��������
  data3096 = 1;
  RS485_Screen_WriteMem(SCREEN_CVOLTOTALHIGH, SCREEN_ALARM_GREEN);  //��Ļ��ɫ
}

void faulthandle_voltotallow3(){
  //��BMS�����ϵ���Ϣ
  //�жϼ̵���
  RS485_Screen_WriteMem(SCREEN_DSCVOLTOTALLOW, SCREEN_ALARM_RED);  //��Ļ��ɫ
  //����
}

void faulthandle_voltotallow2(){
  //����Ϊ0
  //��Ļ��ɫ
}

void faulthandle_voltotallow1(){
  //������50%
  //��Ļ��ɫ
}

void faulthandle_voltoallow(){
  //�����ָ�����
  //��Ļ��ɫ
}

void faulthandle_cvolhigh3(){
  //����������ϵ���Ϣ
  data3090 = 0x01;
  data3096 = 0;
  data3073 = base_curr;
  set_BST_data(1<<4, 0, 0);
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler);
  RS485_Screen_WriteMem(SCREEN_CVOLHIGH, SCREEN_ALARM_RED);  //��Ļ��ɫ
  //�жϼ̵���
  //����
}

void faulthandle_cvolhigh2(){
  data3073 = base_curr;   //����Ϊ0����������Ϊ0
  data3090 = 0x01;
  data3096 = 0;
  set_BST_data(1<<4, 0, 0);
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler);
  RS485_Screen_WriteMem(SCREEN_CVOLHIGH, SCREEN_ALARM_ORANGE);  //��Ļ��ɫ
}

void faulthandle_cvolhigh1(){
  data3090 = 0x01;
  data3073 = base_curr + standard_data3073/2;  //������50%
  data3096 = 0;
  set_BST_data(1<<4, 0, 0);
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler);
  RS485_Screen_WriteMem(SCREEN_CVOLHIGH, SCREEN_ALARM_YELLOW);  //��Ļ��ɫ
}

void faulthandle_cvolhigh0(){
  data3090 = 0x00;
  data3073 = base_curr + standard_data3073;  //��������
  data3096 = 1;
  RS485_Screen_WriteMem(SCREEN_CVOLHIGH, SCREEN_ALARM_GREEN);  //��Ļ��ɫ
}

void faulthandle_cvollow3(){
  data3090 = 0x10;
  data3073 = base_curr;   //����Ϊ0����������Ϊ0
  RS485_Screen_WriteMem(SCREEN_CVOLLOW, SCREEN_ALARM_RED);  //��Ļ��ɫ
}

void faulthandle_cvollow2(){
  data3090 = 0x10;
  data3073 = base_curr + standard_data3073/5;   //����Ϊ20%
  RS485_Screen_WriteMem(SCREEN_CVOLLOW, SCREEN_ALARM_ORANGE);  //��Ļ��ɫ
}

void faulthandle_cvollow1(){
  data3090 = 0x10;
  data3073 = base_curr + standard_data3073/2;   //����Ϊ50%
  RS485_Screen_WriteMem(SCREEN_CVOLLOW, SCREEN_ALARM_YELLOW);  //��Ļ��ɫ
}

void faulthandle_cvollow0(){
  data3090 = 0x00;
  data3073 = base_curr + standard_data3073;  //��������
  RS485_Screen_WriteMem(SCREEN_CVOLLOW, SCREEN_ALARM_GREEN);  //��Ļ��ɫ
}

void faulthandle_cvoldiff3(){
  data3073 = base_curr;   //����Ϊ0����������Ϊ0
  data3096 = 0;
  set_BST_data(0, 0, 1<<2);
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler);
  RS485_Screen_WriteMem(SCREEN_CVOLDIFF, SCREEN_ALARM_RED);  //��Ļ��ɫ
}

void faulthandle_cvoldiff2(){
  data3073 = base_curr + standard_data3073/5;   //����Ϊ20%
  data3096 = 0;
  set_BST_data(0, 0, 1<<2);
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler);
  RS485_Screen_WriteMem(SCREEN_CVOLDIFF, SCREEN_ALARM_ORANGE);  //��Ļ��ɫ
}

void faulthandle_cvoldiff1(){
}

void faulthandle_cvoldiff0(){
  data3073 = base_curr + standard_data3073;   //������
  data3096 = 1;
  RS485_Screen_WriteMem(SCREEN_CVOLDIFF, SCREEN_ALARM_GREEN);  //��Ļ��ɫ
}

void faulthandle_covercurr3(){
  //����������ϵ���Ϣ
  data3073 = base_curr;
  data3096 = 0;
  set_BST_data(0, 0, 1);
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler);
  RS485_Screen_WriteMem(SCREEN_COVERCURR, SCREEN_ALARM_RED);  //��Ļ��ɫ
  //�жϼ̵���
  //����
}

void faulthandle_covercurr2(){
  data3073 = base_curr + standard_data3073/5;   //����Ϊ20%
  data3096 = 1;
  RS485_Screen_WriteMem(SCREEN_COVERCURR, SCREEN_ALARM_ORANGE);  //��Ļ��ɫ
}

void faulthandle_covercurr1(){
}

void faulthandle_covercurr0(){
  data3073 = base_curr + standard_data3073;   //�ָ�����
  data3096 = 1;
  RS485_Screen_WriteMem(SCREEN_COVERCURR, SCREEN_ALARM_GREEN);  //��Ļ��ɫ
}

void faulthandle_insuleak3(){
  //����������ϵ���Ϣ
  data3073 = base_curr;
  data3094 = 0x01;
  data3096 = 0;
  set_BST_data(0, 1, 0);
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler);
  RS485_Screen_WriteMem(SCREEN_INSULEAK, SCREEN_ALARM_RED);  //��Ļ��ɫ
  //�жϼ̵���
  //����
}

void faulthandle_insuleak2(){
  data3073 = base_curr;   //����Ϊ0
  data3094 = 0x01;
  data3096 = 0;
  xTaskCreate((TaskFunction_t   )send_BST_task,          
	            (const char*      )"send_BST_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBSTTaskHandler);
  RS485_Screen_WriteMem(SCREEN_INSULEAK, SCREEN_ALARM_ORANGE);  //��Ļ��ɫ
}

void faulthandle_insuleak1(){
  data3073 = base_curr + standard_data3073/2;   //����Ϊ20%
  data3094 = 0x01;
  data3096 = 1;
  RS485_Screen_WriteMem(SCREEN_INSULEAK, SCREEN_ALARM_ORANGE);  //��Ļ��ɫ
}

void faulthandle_insuleak0(){
  data3073 = base_curr + standard_data3073;   //������
  data3094 = 0;
  data3096 = 1;
  RS485_Screen_WriteMem(SCREEN_INSULEAK, SCREEN_ALARM_GREEN);   //��Ļ��ɫ
}

#endif

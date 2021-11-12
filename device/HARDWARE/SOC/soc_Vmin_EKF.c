
/* SOC�������˵��
- ���ļ������˹�������������SOC״̬�ķ���ʵ��;
- �����SOC״̬�Ĺ��㷽����Ҫ��:������С��ѹ����չ�������˲�����;
  - (1)ʵ��ԭ���ǽ������Եĵ����״̬���̺͹۲ⷽ�̽�����ɢ�������Ի�(̩��չ��),Ȼ��Ӧ�ÿ������˲�������SOC״̬���й���;
  - (2)Vmin-EKF�㷨��ʵ�ֲ�����Ҫ��:
	- (3)��ʼ��SOC״̬;
	- (4)����SOC״̬:
	  - (a)״̬ʱ�����;
	  - (b)���Э����ʱ�����;
	  - (c)����Kalman����;
	  - (d)״̬���ƹ۲����;
	  - (e)���Э����۲����;
- ����ĵ����SOC������̻�����:
  - (1)OCV-SOC�궨У׼:
	  - (a)���ϵ�ͳ��״̬��У׼,ʹ��(Ƭ��Flash)�洢�ĵ�ǰ��ϵ�ǰ����������ΪSOC״̬���������;
		- (b)��BMS�ϵ�����龲��(�����ڸö�ʱ������ֵ��С��|1%FSR|)ʱ��ϳ�ʱ(���趨Ϊ0.5h/1h/1.5h/2h��),���ݳ����ṩ��SOC-OCV������б궨;
  - (2)���ĩ��У׼:BMS��⵽"��絥���ѹ3��"ʱ,��ǿ�ƽ�SOCУ׼��100%;δ�ﵽ֮ǰ,�����ʾ99%(�����Ƶ���ؽ���״̬���ϴ�ʱ,���������ܴ�);
  - (3)�ŵ�ĩ��У׼:BMS��⵽"�ŵ絥��Ƿѹ3��"ʱ,��ǿ�ƽ�SOCУ׼��0%;δ�ﵽ֮ǰ,�����ʾ1%(Ŀ��Ƚ�����ʵ��)
*/

#include "sys.h"
#include "soc_Vmin_EKF.h"
#include "math.h"
#include "bms_state.h"

////��ر�������
////�����ṩ��SOC-OCV����:  0%    10%   20%   30%   40%   50%   60%   70%   80%   90%  100%
//const u16 SOC_OCV[11] = {2723, 3227, 3259, 3280, 3289, 3292, 3301, 3316, 3327, 3333, 3400};
//float xk,Pk;           //���ڴ洢Vmin-EKF�㷨��������е��м����(״̬���� Э����)






/* д�뵱ǰSOC��ֵ
����:����ǰ����õ���SOC��ֵд�뵽��Ӧλ��
����:��ǰSOC��ֵ
����ֵ:��
*/
void soc_write(float socnow) {
	//д�뵽ȫ�ֱ�����
	soctotal = socnow;
	
	//д�뵽Ƭ��FLASH��
	STMFlash_Write_SOC(soctotal);

	return;
}

/* ��ȡǰһʱ��SOC��ֵ
����:��ȡ�洢��Ƭ�ϵ�ǰһʱ�̵�SOC��ֵ
����:��
����ֵ:ǰһʱ�̵�SOC��ֵ
*/
float soc_read(void) {
	float socprevious;

  //��Flash�ж�ȡSOC��ֵ
	socprevious = STMFlash_Read_SOC();
  soctotal = socprevious;

	return socprevious;
}


/* SOC�궨
����:������龲�ýϳ�ʱ��ʱ,Ӧ��SOC-OCV����Ե����SOC���б궨,�Ὣ�궨���SOC��ֵͬ����ȫ�ֱ�����Ƭ��Flash��
����:���ڱ궨�ĵ�ص�ѹ(��ʹ����С�����ص�ѹ���б궨),����ֵΪ5λ��,������mVΪ��λ��4λ��;
����ֵ:�궨���SOC��ֵ;
*/
float soc_calibrate(u16 cellvoltocalb)
{
	u8 i;
	float soccalbed;  //����궨�õ���SOC��ֵ
	cellvoltocalb/= 10;  //���㵽��SOC-OCV����ͬ����
	if(cellvoltocalb<SOC_OCV[0] || cellvoltocalb>SOC_OCV[10]){  //��ص�ѹ���뷶Χ����
		return -1;
	}
	
	//�궨����
	for(i=0;i<10;i++) {
		if((cellvoltocalb>=SOC_OCV[i])&&(cellvoltocalb<SOC_OCV[i+1])){  //��ĳһ������
			if((cellvoltocalb-SOC_OCV[i])<(SOC_OCV[i+1]-cellvoltocalb)) {  //���������
				soccalbed = 0.1 * i;
			}
			else {  //�������Ҳ�
				soccalbed = 0.1 * (i+1);
			}
		}
	}
	
	//д��궨�õ���SOC��ֵ
	//soc_write(soccalbed);
	
	
	//���ر궨�õ���SOC��ֵ
	return soccalbed;
}

/*�����µ�SOC״ֵ̬
����:����Vmin-EKF�㷨�����µ�SOC״ֵ̬;
����:���������,ʹ�����������ȫ�ֱ���xk Pk��Ϊ��������,���ؼ���õ���soc��ֵ,��������л����xk��Pk��ֵ
����:
  - ik_1: ��ǰ�����ĵ���,������,��Ϊ���,��Ϊ�ŵ�,��λ����
  - yku16: ȫ�ֱ�����ʽ�����(5λ����ֵ)
  - t:����SOC��ʱ����,��sΪ��λ;
����ֵ:����õ���SOC��ֵ(״̬����x��ΪSOC);
*/
float soc_calculate(float ik_1, u16 yku16, float t) {
	float yk;  //float���͵����ѹֵ
	float xk_1,Pk_1;  //ǰһʱ�̵�״̬������Э����(k_1��ʾk-1,��ǰһʱ��)
	float xk_,Pk_;    //ʱ����·��̼���õ���ֵ,���ֱ��ʾxk-��Pk-
	float Ck,Kk;
	float gk;
	
	/* ����ΪVmin-EKF��SOC�������
	//�������
	yk = (float)yku16 * 0.0001;  //��ȫ�ֱ����б����u16���͵ĵ����ѹֵת��Ϊfloat����
	
	//ʱ����·���
	//[1]״̬����ʱ����·���
	xk_1 = xk;  //ǰһʱ��״̬����ֵ
	xk_ = xk_1 + eta * t * ik_1 / Cn;  //���·���
	
	//[2]���Э����ʱ����·���
	Pk_1 = Pk;  //ǰһʱ��Э�������
	Pk_ = Pk_1;  //+ Q;  //Э������·���,QΪwk��Э�������
	
	//״̬���·���
	//[1]���㿨��������
	Ck = K0/(xk_*xk_) - K1 + K2/xk_ - K3/(1-xk_);
	Kk = 1 / Ck;  //����������(����ֵ�����м����
	//Kk = Pk_ * Ck / (Ck * Pk_ * Ck);  //����������
	//Kk = Pk_ * Ck / (Ck * Pk_ * Ck + R);  //����������,RΪvk��Э�������
	
	//[2]״̬���ƹ۲���·���
	gk = V0 - ik_1*R - K0/xk_ - K1*xk_ + K2*ln(xk_) + K3*ln(1-xk_);  //ln()������Ҫ���
	xk = xk_ + Kk * (yk - gk);
	
	//[3]���Э����۲���·���
	Pk = (I- Kk*Ck);
	
	*/
	
	//����Ϊ�������ַ���SOC�������
	xk_1 = xk;  //ǰһʱ��SOC״̬
	xk = xk_1 + eta*t*ik_1/Cn;  //��������
	
	
	return xk;
}


float ln(float x) {
	
	
	
	return x;
}

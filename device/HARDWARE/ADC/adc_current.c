
#include "adc_current.h"
#include "bms_state.h"
#include "sys.h"
#include "iic_LTC2305.h"

/*
˵��:
- BCU V1.3��,������⹦��������LTC2305ADCоƬ��õ�ѹ����,ʹ��PB14��PB15���Ž��п���,��ѡ�����ģ��I2C6��I2CFMP1����I2Cͨ��,LTC2305оƬ��ַΪ0001000;
- BCU V1.2��,�ù����ɵ��ʵ��,����޷�ʹ��V1.2���е��ԺͲ���,�������õ�����׼ȷ�ȵļ��;
*/

/*
- ��ʹ�� iic_fun.c �ļ����ṩ�����ģ��IICͨ�Ź��ܵ�ʱ��,������⹦�ܵ�LTC2305оƬ,��ͨ������Ϊ PB15_IIC3_SCL + PB14_IIC3_SDA;
- ʹ�� iic_LTC2305.c �ļ��з�װ��оƬ��������ʵ�����ݵĶ�ȡ;
*/


/* �������������ź�ת��
����:����BCU��ӦIICͨ����ADCоƬת��
����:��
����ֵ:�Ƿ�ɹ�,1Ϊ�ɹ�,0Ϊʧ��
*/
u8 IIC_StartCurrentConvert(void){
	u8 ack_flag = 1;
	
	ack_flag = LTC2305_start_convert(3, LT2305_ADDR_CURR);  //����Ĭ�Ϲ���ģʽ�µ�ADCת��
	
	if(ack_flag==0){
		return 1;
	}
	else {
		return 0;
	}
}

/* ͨ��ADCоƬ��ȡ��������
����:��ȡADC�źŵ�ת�����,�������صĽ��ת��Ϊ��Ӧ�ĵ�����ֵ
����:��
����ֵ:��������
*/
float IIC_ReadCurrentData(void) {
	u16 currdata = 0;
	float currdataf;
	
	currdata = LTC2305_read_result(3,LT2305_ADDR_CURR);
	
	//�����ת��Ϊ��Ӧ�ĵ���ֵ(ģ���ѹֵ->��ѹ->��������)
	
	
	
	
	
	
	
	
	
}



































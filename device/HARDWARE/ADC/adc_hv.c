
#include "adc_hv.h"
#include "bms_state.h"
#include "sys.h"
#include "iic_LTC2305.h"
/*
˵��:
- BCU V1.3�ĸ�ѹ��⹦��ʹ��LTC2305ADCоƬ����ʵ��,ʹ��PF0_SDA��PF1_SCL���Ž���I2Cͨ��,������Ϊ���ģ��I2C��Ӳ��I2C2;
- B+(CH0)��HV1(CH1)�ĵ�ѹ���оƬ��ַΪ0001000,HV2(CH0)�ĵ�ѹ���оƬ��ַΪ0011010;
- ʹ�� iic_LTC2305.c �ļ��з�װ��оƬ��������ʵ�����ݵĶ�ȡ;
*/


/* ������ѹ�����ź�ת��
����:���ö�Ӧ�ĸ�ѹ���ͨ��ģʽ,����BCU��ӦIICͨ����ADCоƬת��
����:hvx:ѡȡ��ѹ����
����ֵ:�Ƿ�ɹ�,1Ϊ�ɹ�,0Ϊʧ��
*/
u8 IIC_StartHVConvert(u8 hvx){
	u8 success_flag = 0;
	
	if(hvx>1)  //�����������������Χ��
		return 0;
	
	if(hvx==0) {  //��ѹ����:B+
		success_flag = LTC2305_change_mode(4, LT2305_ADDR_HVCHIP0, DIN_UNIP_CH0_MODE);  //�ı��ѹ���ADCоƬ��ģʽ:CH0
	}
	else if(hvx==1){  //��ѹ����:HV1
		success_flag = LTC2305_change_mode(4, LT2305_ADDR_HVCHIP0, DIN_UNIP_CH0_MODE);  //�ı��ѹ���ADCоƬ��ģʽ:CH1
	}
	return success_flag;
	
}

/* ͨ��ADCоƬ��ȡ��ѹת�����ݽ��
����:��ȡADC�źŵ�ת�����,�������صĽ��ת��Ϊ��Ӧ�ĸ�ѹ����ֵ
����:��
����ֵ:��ѹ�������
*/
float IIC_ReadHVData(u8 hvx) {
	u16 hvdata = 0;
	float hvdataf;
	if(hvx>1)  //�����������������Χ��
		return 0;
	
	if((hvx==0)||(hvx==1)){
		hvdata = LTC2305_read_result(4,LT2305_ADDR_HVCHIP0);
	}
	
	//�����ת��Ϊ��ѹ����(ģ���ѹֵ->��ѹ->��ѹ����ֵ)
	
	
	
	
	
	
	return hvdataf;
	
}







































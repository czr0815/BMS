
#include "adc_cc.h"
#include "bms_state.h"
#include "sys.h"
/*
˵��:
- BCU V1.3��CC�ź�״̬��⹦����MAX11646ADCоƬ��ʵ��(����ΪPG12��PG13ͨ��ģ��I2C������ͨ��),Ҳ����ʱʹ��PB1_ADC1_IN9(CC2)��PC5_ADC1_IN5(CC1)������ͨ��ST�ڲ�ADC���ܽ��е�ѹ���;
- BMU V1.2��CC�ź�״̬��⹦����MAX11646ADCоƬ��ʵ��(����ΪPB14��PB15ͨ��ģ��I2C������ͨ��),Ҳ����ʱʹ��PB1(CC2)��PB0(CC1)������ͨ��ST�ڲ�ADC���ܽ��е�ѹ���;
*/




/*
����:��ȡBMS������ĳ������CC״̬
����:��ȡ��״̬��ADCת������
����ֵ:CC״ֵ̬(CC_STATE_CONNECTED=1,CC_STATE_DISCONNECT=2)
*/
u8 get_cc_state(u8 times)
{
	u16 adcval = 0;
	u16 adcvol;
	u16 ccval;
	float ccvolf;
	
	/*����Ϊʹ��STM32оƬ�ڲ�ADC����ʵ��CC״̬��⹦��*/
	adcval = get_adc_stfun_average(ADC_Channel_CC2,times);
	adcvol = adcval * 3300 / 4096;  //CC2�����ѹ,��λmV,3300Ϊ��������ѹ;
	//�˴�ʹ��20k��100k�����ѹ,���δ����ʱadcvol�ĵ���ֵӦΪ2000mV,������ʱadcvol�ĵ���ֵӦΪ1000mV;
	
	if(adcvol>500 && adcvol<1500){        //ת������ڸ÷�Χ�����ж�Ϊ������(����ֵ1000mV)
		ccval = CC_STATE_CONNECTED;  
		ccstate = CC_STATE_CONNECTED;  
	}
	else if(adcvol>1500 && adcvol<2500){    //��������ڸ÷�Χ�����ж�Ϊδ����(����ֵ2000mV)
		ccval = CC_STATE_DISCONNECT;
		ccstate = CC_STATE_DISCONNECT;
	}
	else{                                   //����������ֵ
		ccval = CC_STATE_DISCONNECT;
		ccstate = CC_STATE_DISCONNECT;
	}
	
	//CC��ѹ�������
//	ccvolf = ((float)adcvol)*0.006;
//	printf("CC volt:%.2f",ccvolf);
	
	return ccval;
	
}
































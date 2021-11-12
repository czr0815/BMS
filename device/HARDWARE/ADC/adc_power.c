
#include "adc_power.h"
#include "bms_state.h"
#include "sys.h"
/*
˵��:
- BCU V1.3��,��Դ��ѹ��⹦���� PB0_ADC1_IN8 ����ʵ��;
- ��BCU V1.2��,�ù��ܵļ������ֻ������LTC2305оƬ��,����޷�ʹ��V1.2���е��Բ��ԣ�
*/

/*
����:STM32F413�ڲ�ADC���ܳ�ʼ��,��ʼ����Ҫ�õ���2��ͨ��(�����ѹ���:PB0_ADC1_IN8,CC2״̬���:PB1_ADC1_IN9)
����:��
����ֵ:��
*/
void ADC_Inside_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	//����ADCʱ�Ӻ�GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);  //ʹ��ADC1ʱ��
	
	//������������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;        //ģ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //PB0|1����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    //������/����;  
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����		
	
	//ADCͨ������(��Ӱ��Ƭ������ADC)
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;  //����DMA
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;  //��������
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;  //�ٶȾ����ܵ�(��Div2 Div4 Div6 Div8��ѡ)
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;  //������ʱ(��5Cycles - 20Cycles��ѡ)
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//ADC1���ó�ʼ��
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //�ر�����ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //�����Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;  //��ʹ���ⲿ����,Ҳ����ֹ�������,ʹ���������
	ADC_InitStructure.ADC_NbrOfConversion = 2;    //����ͨ����ת��ADC������
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;  //12λ����
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;  //��ɨ��ģʽ
	ADC_Init(ADC1,&ADC_InitStructure);
	
	//ADC1,ͨ��8(PB0),��ѡͨ���Ĳ���ʱ��ֵ
	//��������:����ָ��ADC�Ĺ�����ͨ��,�������ǵ�ת��˳��Ͳ���ʱ��(������������,���в�����Ӧ��2);
	//ADC_RegularChannelConfig(ADC1,ADC_Channel_8,1,ADC_SampleTime_144Cycles);
	
	ADC_Cmd(ADC1,ENABLE);
	
	//ADC_SoftwareStartConv(ADC1);  //�ֶ�����ADC����,����Ϊ����ɨ��ֻ�迪��һ�μ���(Ϊ����������ѡ����ADCת��)

}

/*
����:���ָ��ADCͨ����ADCֵ
����:ͨ��ֵ(��ѡ:ADC_Channel_CC2 �� ADC_Channel_Power,��ͨ��9��ͨ��8)
����ֵ:ת�����
*/
u16 get_adc_stfun(u8 ch)
{
	
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
  
	ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������	
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	
	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����	
	
}

/*
����:���ָ��ADCͨ����times��ƽ��ֵ;
����:chΪͨ��ֵ(��ѡ:ADC_Channel_CC2 �� ADC_Channel_Power,��ͨ��9��ͨ��8),timesΪת������;
����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
*/
u16 get_adc_stfun_average(u8 ch, u8 times)
{
	u32 tmp_total = 0;
	u16 tmp_val = 0;
	u8 t;
	for(t=0;t<times;t++){
		tmp_val = get_adc_stfun(ch);
		tmp_total += tmp_val;
		//delay_ms(1);  //�˴���Ҫ���Բ�ͬ��ʱ�µļ�����Ƿ�һ��
	}
	return tmp_total/times;
}


/*
����:��ȡBMS�����ѹֵ(ADCת��+����)
����:��ȡ�õ�ѹֵ��ADCת������
����ֵ:u16���͵Ĺ����ѹֵ
*/
u16 get_power_vol(u16 times)
{
	u16 adcval,adcvol,powervol;
	
	//�˴���ȡ�����ѹֵʹ��STM32оƬ�ڲ�ADC��������ʵ��
	adcval = get_adc_stfun_average(ADC_Channel_Power,times);  //��ȡ���ת��ƽ��ֵ(x/4096),STM32��������ѹ3.3V
	powervol = adcval * 3300 * 8 / 4096;  //�����ѹ,��λmV,3300Ϊ��������ѹ,8Ϊ��ѹϵ��(ԭBCU�Ϸ�ѹϵ��Ϊ6)
	powervolu16 = powervol;  //ȫ�ֱ���
	powervolf = (float)powervol * 0.001;
	return powervol;
	
}







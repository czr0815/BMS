#ifndef __ADC_POWER_H
#define __ADC_POWER_H

#include "sys.h"
/*
˵��:���ļ�����BMS�����ѹ��⹦�ܵ���ض����ʵ��
*/


//�궨��
#define ADC_Channel_CC2    ADC_Channel_9
#define ADC_Channel_Power  ADC_Channel_8





//��������
//��������������ST�ڲ�ADC������ͨ�ú���,�����ڹ����ѹ����CC״̬���
void ADC_Inside_Init();   //����Ŀ��Ҫʹ�õ�ST�ڲ�ADC���ܳ�ʼ��
u16 get_adc_stfun(u8 ch);    //��ȡADC1ĳ��ͨ����ͨ��ֵ
u16 get_adc_stfun_average(u8 ch, u8 times);   //��ȡADC1ĳ��ͨ����������������ƽ��ֵ


u16 get_power_vol(u16 times);    //��ȡBMS�����ѹֵ(ADCת��+����)








































#endif

#ifndef __ADC_POWER_H
#define __ADC_POWER_H

#include "sys.h"
/*
说明:本文件用于BMS供电电压检测功能的相关定义和实现
*/


//宏定义
#define ADC_Channel_CC2    ADC_Channel_9
#define ADC_Channel_Power  ADC_Channel_8





//函数声明
//以下三个函数是ST内部ADC采样的通用函数,可用于供电电压检测和CC状态检测
void ADC_Inside_Init();   //该项目需要使用的ST内部ADC功能初始化
u16 get_adc_stfun(u8 ch);    //获取ADC1某个通道的通道值
u16 get_adc_stfun_average(u8 ch, u8 times);   //获取ADC1某个通道给定次数采样的平均值


u16 get_power_vol(u16 times);    //获取BMS供电电压值(ADC转换+换算)








































#endif

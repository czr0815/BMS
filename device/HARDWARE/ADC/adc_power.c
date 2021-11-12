
#include "adc_power.h"
#include "bms_state.h"
#include "sys.h"
/*
说明:
- BCU V1.3中,电源电压检测功能由 PB0_ADC1_IN8 引脚实现;
- 因BCU V1.2中,该功能的检测引脚只连到了LTC2305芯片上,因此无法使用V1.2进行调试测试：
*/

/*
功能:STM32F413内部ADC功能初始化,初始化需要用到的2个通道(供电电压检测:PB0_ADC1_IN8,CC2状态检测:PB1_ADC1_IN9)
参数:无
返回值:无
*/
void ADC_Inside_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	//开启ADC时钟和GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);  //使能ADC1时钟
	
	//输入引脚配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;        //模拟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //PB0|1引脚
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    //不上拉/下拉;  
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束		
	
	//ADC通用配置(将影响片上所有ADC)
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;  //不用DMA
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;  //独立采样
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;  //速度尽可能低(有Div2 Div4 Div6 Div8可选)
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;  //采样延时(有5Cycles - 20Cycles可选)
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//ADC1配置初始化
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //关闭连续转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //数据右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;  //不使用外部触发,也即禁止触发检测,使用软件触发
	ADC_InitStructure.ADC_NbrOfConversion = 2;    //常规通道组转换ADC的数量
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;  //12位精度
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;  //非扫描模式
	ADC_Init(ADC1,&ADC_InitStructure);
	
	//ADC1,通道8(PB0),所选通道的采样时间值
	//函数功能:设置指定ADC的规则组通道,设置它们的转化顺序和采样时间(如需两个采样,下行参数中应填2);
	//ADC_RegularChannelConfig(ADC1,ADC_Channel_8,1,ADC_SampleTime_144Cycles);
	
	ADC_Cmd(ADC1,ENABLE);
	
	//ADC_SoftwareStartConv(ADC1);  //手动开启ADC采样,配置为连续扫描只需开启一次即可(为常规组启用选定的ADC转换)

}

/*
功能:获得指定ADC通道的ADC值
参数:通道值(可选:ADC_Channel_CC2 或 ADC_Channel_Power,即通道9或通道8)
返回值:转换结果
*/
u16 get_adc_stfun(u8 ch)
{
	
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
	
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果	
	
}

/*
功能:获得指定ADC通道的times次平均值;
参数:ch为通道值(可选:ADC_Channel_CC2 或 ADC_Channel_Power,即通道9或通道8),times为转换次数;
返回值:通道ch的times次转换结果平均值
*/
u16 get_adc_stfun_average(u8 ch, u8 times)
{
	u32 tmp_total = 0;
	u16 tmp_val = 0;
	u8 t;
	for(t=0;t<times;t++){
		tmp_val = get_adc_stfun(ch);
		tmp_total += tmp_val;
		//delay_ms(1);  //此处需要测试不同延时下的检测结果是否一致
	}
	return tmp_total/times;
}


/*
功能:获取BMS供电电压值(ADC转换+换算)
参数:获取该电压值的ADC转换次数
返回值:u16类型的供电电压值
*/
u16 get_power_vol(u16 times)
{
	u16 adcval,adcvol,powervol;
	
	//此处获取供电电压值使用STM32芯片内部ADC采样功能实现
	adcval = get_adc_stfun_average(ADC_Channel_Power,times);  //获取多次转换平均值(x/4096),STM32最大测量电压3.3V
	powervol = adcval * 3300 * 8 / 4096;  //供电电压,单位mV,3300为最大测量电压,8为分压系数(原BCU上分压系数为6)
	powervolu16 = powervol;  //全局变量
	powervolf = (float)powervol * 0.001;
	return powervol;
	
}







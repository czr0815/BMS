
#include "adc_cc.h"
#include "bms_state.h"
#include "sys.h"
/*
说明:
- BCU V1.3的CC信号状态检测功能由MAX11646ADC芯片来实现(引脚为PG12和PG13通过模拟I2C来进行通信),也可暂时使用PB1_ADC1_IN9(CC2)和PC5_ADC1_IN5(CC1)两引脚通过ST内部ADC功能进行电压检测;
- BMU V1.2的CC信号状态检测功能由MAX11646ADC芯片来实现(引脚为PB14和PB15通过模拟I2C来进行通信),也可暂时使用PB1(CC2)和PB0(CC1)两引脚通过ST内部ADC功能进行电压检测;
*/




/*
功能:获取BMS与充电机的充电连接CC状态
参数:获取该状态的ADC转换次数
返回值:CC状态值(CC_STATE_CONNECTED=1,CC_STATE_DISCONNECT=2)
*/
u8 get_cc_state(u8 times)
{
	u16 adcval = 0;
	u16 adcvol;
	u16 ccval;
	float ccvolf;
	
	/*以下为使用STM32芯片内部ADC采样实现CC状态检测功能*/
	adcval = get_adc_stfun_average(ADC_Channel_CC2,times);
	adcvol = adcval * 3300 / 4096;  //CC2检测点电压,单位mV,3300为最大测量电压;
	//此处使用20k和100k电阻分压,因此未连接时adcvol的典型值应为2000mV,已连接时adcvol的典型值应为1000mV;
	
	if(adcvol>500 && adcvol<1500){        //转换结果在该范围内则判断为已连接(典型值1000mV)
		ccval = CC_STATE_CONNECTED;  
		ccstate = CC_STATE_CONNECTED;  
	}
	else if(adcvol>1500 && adcvol<2500){    //采样结果在该范围内则判断为未连接(典型值2000mV)
		ccval = CC_STATE_DISCONNECT;
		ccstate = CC_STATE_DISCONNECT;
	}
	else{                                   //不正常采样值
		ccval = CC_STATE_DISCONNECT;
		ccstate = CC_STATE_DISCONNECT;
	}
	
	//CC电压输出测试
//	ccvolf = ((float)adcvol)*0.006;
//	printf("CC volt:%.2f",ccvolf);
	
	return ccval;
	
}
































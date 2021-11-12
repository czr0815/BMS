
#include "adc_current.h"
#include "bms_state.h"
#include "sys.h"
#include "iic_LTC2305.h"

/*
说明:
- BCU V1.3中,电流检测功能最终由LTC2305ADC芯片获得电压数字,使用PB14和PB15引脚进行控制,可选用软件模拟I2C6或I2CFMP1进行I2C通信,LTC2305芯片地址为0001000;
- BCU V1.2中,该功能由电表实现,因此无法使用V1.2进行调试和测试,但可以用电表进行准确度的检测;
*/

/*
- 在使用 iic_fun.c 文件中提供的软件模拟IIC通信功能的时候,电流检测功能的LTC2305芯片,其通信引脚为 PB15_IIC3_SCL + PB14_IIC3_SDA;
- 使用 iic_LTC2305.c 文件中封装的芯片操作函数实现数据的读取;
*/


/* 开启电流数据信号转换
功能:开启BCU对应IIC通道的ADC芯片转换
参数:无
返回值:是否成功,1为成功,0为失败
*/
u8 IIC_StartCurrentConvert(void){
	u8 ack_flag = 1;
	
	ack_flag = LTC2305_start_convert(3, LT2305_ADDR_CURR);  //开启默认工作模式下的ADC转换
	
	if(ack_flag==0){
		return 1;
	}
	else {
		return 0;
	}
}

/* 通过ADC芯片获取电流数据
功能:读取ADC信号的转换结果,并将返回的结果转换为对应的电流数值
参数:无
返回值:电流数据
*/
float IIC_ReadCurrentData(void) {
	u16 currdata = 0;
	float currdataf;
	
	currdata = LTC2305_read_result(3,LT2305_ADDR_CURR);
	
	//将结果转换为对应的电流值(模拟电压值->分压->电流数据)
	
	
	
	
	
	
	
	
	
}



































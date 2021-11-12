
#include "adc_hv.h"
#include "bms_state.h"
#include "sys.h"
#include "iic_LTC2305.h"
/*
说明:
- BCU V1.3的高压检测功能使用LTC2305ADC芯片进行实现,使用PF0_SDA和PF1_SCL引脚进行I2C通信,可配置为软件模拟I2C或硬件I2C2;
- B+(CH0)和HV1(CH1)的电压检测芯片地址为0001000,HV2(CH0)的电压检测芯片地址为0011010;
- 使用 iic_LTC2305.c 文件中封装的芯片操作函数实现数据的读取;
*/


/* 开启高压数据信号转换
功能:配置对应的高压检测通道模式,开启BCU对应IIC通道的ADC芯片转换
参数:hvx:选取高压检测点
返回值:是否成功,1为成功,0为失败
*/
u8 IIC_StartHVConvert(u8 hvx){
	u8 success_flag = 0;
	
	if(hvx>1)  //输入参数不在正常范围内
		return 0;
	
	if(hvx==0) {  //高压检测点:B+
		success_flag = LTC2305_change_mode(4, LT2305_ADDR_HVCHIP0, DIN_UNIP_CH0_MODE);  //改变高压检测ADC芯片的模式:CH0
	}
	else if(hvx==1){  //高压检测点:HV1
		success_flag = LTC2305_change_mode(4, LT2305_ADDR_HVCHIP0, DIN_UNIP_CH0_MODE);  //改变高压检测ADC芯片的模式:CH1
	}
	return success_flag;
	
}

/* 通过ADC芯片获取高压转换数据结果
功能:读取ADC信号的转换结果,并将返回的结果转换为对应的高压数据值
参数:无
返回值:高压检测数据
*/
float IIC_ReadHVData(u8 hvx) {
	u16 hvdata = 0;
	float hvdataf;
	if(hvx>1)  //输入参数不在正常范围内
		return 0;
	
	if((hvx==0)||(hvx==1)){
		hvdata = LTC2305_read_result(4,LT2305_ADDR_HVCHIP0);
	}
	
	//将结果转换为高压数据(模拟电压值->分压->总压数据值)
	
	
	
	
	
	
	return hvdataf;
	
}







































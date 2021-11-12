
/* 说明
- 本文件包含通过IIC对LTC2305芯片控制与使用的功能实现;
- 关于LTC2305芯片的部分说明
*/


#include "iic_fun.h"
#include "iic_LTC2305.h"
#include "bms_state.h"


/* 开启输入信号转换
功能:向指定地址发送信号,开启输入信号的转换
流程:【开始信号+7位地址+读信号位+获取应答位+停止信号】;
参数:x:选定IIC,addr:芯片地址;
返回值:从机是否应答,有应答为0,无应答为1;
*/
u8 LTC2305_start_convert(u8 x, u8 addr){
	u8 ack_flag = 1;  //无应答信号
	u8 byte = 0;
	byte = x<<1 | 0x01;
	
	IIC_Start(x);  //开始信号
	IIC_Send_Byte(x,byte);  //发送地址+读信号位
	ack_flag = IIC_Wait_Ack(x);  //读取有无应答信号
	IIC_Stop(x);  //停止信号
	
	return ack_flag;
}

/* 改变芯片转换模式
功能:改变指定地址芯片的工作模式(差分输入模式,CH0输入模式,CH1输入模式);
流程:【开始信号+7位地址+写信号位+获取应答位+6位Din+2个无用位+获取应答位+停止信号】;
参数:x:选定IIC,addr:芯片地址,Din:用于指定芯片的工作模式;
返回值:根据从机应答状况判断是否写入命令成功,1为写入成功,0为写入失败;
*/
u8 LTC2305_change_mode(u8 x, u8 addr, u8 din){
	u8 ack_flag1 = 1;
	u8 ack_flag2 = 1;
	u8 byte;
	
	IIC_Start(x);  //开始信号
	byte = x<<1 | 0x00;  //7位地址+写信号位
	IIC_Send_Byte(x,byte);  //发送地址+写信号位
	ack_flag1 = IIC_Wait_Ack(x);  //读取有无应答位(有为0)
	IIC_Send_Byte(x,din);  //写入DIN模式
	ack_flag2 = IIC_Wait_Ack(x);  //读取有无应答位(有为0)
	IIC_Stop(x);
	
	if((ack_flag1+ack_flag2)==0)  //两次从机都有应答
		return 1;  //写入成功
	else
		return 0;  //写入失败
	
}

/* 读取输入转换结果
功能:读取指定芯片地址的转换结果
流程:【开始信号+7位地址+读信号位+获取从机应答位+读入高8位+主机发送ACK+读入低4位+读入4个无用低位+主机发送NACK+停止信号】;
参数:x:选定IIC,addr:芯片地址;
返回值:data:读取结果(16位二进制,根据模式再转换为对应的无符号或有符号数);
*/
u16 LTC2305_read_result(u8 x, u8 addr) {
	u16 result = 0;
	u8 data = 0;
	u8 byte;
	u8 ack_flag = 1;
	
	IIC_Start(x);  //开始信号
	byte = x<<1 | 0x01;  //7位地址+读信号位
	IIC_Send_Byte(x,byte);  // 发送7位地址+读信号位
	ack_flag = IIC_Wait_Ack(x);  // 获取有无应答位(有为0)
	
	data = IIC_Read_Byte(x,1);  //读入高8位,发送Ack
	result= data<<4;
	data = IIC_Read_Byte(x,0);  //读入低4位,发送NAck
	IIC_Stop(x);
	result = result | (data>>4);
	return result;
	
}
























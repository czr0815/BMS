#ifndef __RS485_ELECT_METER_H
#define __RS485_ELECT_METER_H

#include "stm32f4xx.h"

/*
	说明:
	BMS主控系统中，电表由BCU通过RS485进行控制；
	配置上，RS485信号通过STM32F413的USART2串口经转换模块得到；
	因此，主控系统与电表的交互过程及功能实现都是通过串口USART2实现；
*/

//485转换模块控制引脚CON控制指令,ON为发送模式，OFF为接收模式
#define RS485_RE2_ON  GPIO_SetBits(GPIOD,GPIO_Pin_6)  //BCU的电表通信485模块控制引脚CON为PD6
#define RS485_RE2_OFF GPIO_ResetBits(GPIOD,GPIO_Pin_6)

#define RS4852_REC_LEN 100  //RS485_ElectMeter接收最大字节数

//电表通信过程中,信息帧中控制码格式定义
#define MASTER_SEND_FLAG       0x00
#define SLAVE_RESP_FLAG        0x80
#define SLAVE_CRT_FLAG         0x00
//#define SLAVE_CRT_FLAG         0x40
#define NO_SUBSEQ_DATA_FLAG    0x00
#define SUBSEQ_DATA_FLAG       0x20
//控制码中低5位的请求及应答功能码
#define READ_DATA_FUNC         0x11
#define READ_SUBSEQ_DATA_FUNC  0x12
#define READ_ADDR_FUNC         0x13
#define WRITE_DATA_FUNC        0x14
#define WRITE_ADDR_FUNC        0x15
//后续暂时用不到


//变量数据标识(DI3 DI2 DI1 DI0)
//static const u8 POWER_4BYTES[4] = {0x02,0x00,0x00,0x00};      //4字节数据 xxxx.xxxx,单位kW,功率,最高位为符号位
static const u8 CURRENT_4BYTES[4] = {0x00,0x01,0x00,0x02};    //4字节数据 xxxx.xxxx,单位A,电流,最高位为符号位
//static const u8 VOLTAGE_4BYTES[4] = {0x02,0x00,0x02,0x00};    //4字节数据 xxxx.xxxx,单位V,电压,最高位为符号位
//static const u8 VOLTAGE_2BYTES[4] = {0x02,0x01,0x01,0x00};    //2字节数据 xxx.x,单位V,电压
//static const u8 VOLTAGE_BLOCK[4] = {0x02,0x01,0xFF,0x00};     //6字节数据, 单位V, 电压数据块
//static const u8 CURRENT_3BYTES[4] = {0x02,0x02,0x01,0x00};    //3字节数据 xxx.xxx, 单位A, 电流
//static const u8 CURRENT_BLOCK[4] = {0x02,0x02,0xFF,0x00};     //9字节数据, 单位A, 电流数据块
//后续暂时用不到


//函数声明
void RS485_ElectMeter_Init(u32 bound);
void RS485_ElectMeter_SendData(const uint8_t *data, uint8_t len);
void RS485_ElectMeter_SendFrame(u8 *addr,u8 ctrlcode,u8 len,u8 *data);
void RS485_ElectMeter_UpdateAddr(void);
u8 RS485_ElectMeter_ReadCurrentData(void);
void RS485_ElectMeter_ReadData(void);
void RS485_ElectMeter_ReadSubseqData(void);

























#endif


#ifndef __IIC_LTC2305_H
#define __IIC_LTC2305_H

#include "sys.h"

/*Din的模式设定
- Din由6位扩充为8位(1个字节),发送时从高位开始发送;
- Din格式: 
  - 标识|S/D|O/S| X | X |UNI|SLP| X | X |
  - 位数  8   7   6   5   4   3   2   1
- SLP位:应设置为0,即芯片上电后进入NAP模式;
- UNI位:高为单极,低为双极
- S/D和O/S:
- 0 0 : CH0+, CH1- ->即上电默认状态为电流检测的状态;
- 1 0 : CH0+  +单极 ->测量B+高压
- 1 1 : CH1+  +单极 ->测量HV1高压
- 电流测量:CH0+ CH1- 双极 -> 0x00
- 总压检测B+:CH0+ 单极 -> 10001000
- 总压检测HV1:CH1+ 单极 -> 11001000
*/
#define DIN_BIP_MODE       0x00
#define DIN_UNIP_CH0_MODE  0x88
#define DIN_UNIP_CH1_MODE  0xC8


/*使用的几个LTC2305芯片地址
- BCU中:
  - IIC3 电流检测:AD0=LOW,AD1=LOW,Addr=0001000=0x08;
  - IIC4 高压检测:
    - 芯片1:AD0=LOW,AD1=LOW, Addr=0001000=0x08;
	- 芯片2:AD0=LOW,AD1=HIGH,Addr=0011010=0x1A;
- BMU中:
  - IIC4 烟雾传感器(选用):AD0=LOW,AD1=LOW,Addr=0001000=0x08;
*/
#define LT2305_ADDR_CURR  0x08
#define LT2305_ADDR_HVCHIP0   0x08
#define LT2305_ADDR_HVCHIP1   0x1A
#define LT2305_ADDR_SMOKE 0x08




//函数声明
u8 LTC2305_start_convert(u8 x, u8 addr);  //开启信号转换
u8 LTC2305_change_mode(u8 x, u8 addr, u8 din);  //改变芯片工作模式
u16 LTC2305_read_result(u8 x, u8 addr);  //读取输入转换结果






























#endif /* __IIC_LTC2305_H */

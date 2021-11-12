
/*
  描述:
    BMU的单体电池电压采集使用SPI1通信实现,片选功能分别通过不同的引脚实现,
	每个BMU连接的3个电压采集卡其片选引脚分别为PF6 PF7 PF8 PF9 PF10 PC0,
	本文件中分别实现了SPI1的初始化,封装了不同电压采集芯片的电压采集过程,以及单体电池的均衡功能(均衡开启/关闭)
*/

#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
#include "bms_state.h"

#define CHIP1_OPEN  GPIO_ResetBits(GPIOF,GPIO_Pin_6)
#define CHIP1_CLOSE GPIO_SetBits(GPIOF,GPIO_Pin_6)
#define CHIP2_OPEN  GPIO_ResetBits(GPIOF,GPIO_Pin_7)
#define CHIP2_CLOSE GPIO_SetBits(GPIOF,GPIO_Pin_7)
#define CHIP3_OPEN  GPIO_ResetBits(GPIOF,GPIO_Pin_8)
#define CHIP3_CLOSE GPIO_SetBits(GPIOF,GPIO_Pin_8)
#define CHIP4_OPEN  GPIO_ResetBits(GPIOF,GPIO_Pin_9)
#define CHIP4_CLOSE GPIO_SetBits(GPIOF,GPIO_Pin_9)
#define CHIP5_OPEN  GPIO_ResetBits(GPIOF,GPIO_Pin_10)
#define CHIP5_CLOSE GPIO_SetBits(GPIOF,GPIO_Pin_10)
#define CHIP6_OPEN  GPIO_ResetBits(GPIOC,GPIO_Pin_0)
#define CHIP6_CLOSE GPIO_SetBits(GPIOC,GPIO_Pin_0)
#define CHIP7_OPEN  GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define CHIP7_CLOSE GPIO_SetBits(GPIOC,GPIO_Pin_2)
#define CHIP8_OPEN  GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define CHIP8_CLOSE GPIO_SetBits(GPIOC,GPIO_Pin_3)
#define CHIP9_OPEN  GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define CHIP9_CLOSE GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define CHIP10_OPEN  GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define CHIP10_CLOSE GPIO_SetBits(GPIOA,GPIO_Pin_1)

										  
void SPI1_Init(void);			 //初始化SPI1口
void SPI1_Control_Init(void);    //采集芯片控制引脚初始化
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI1速度   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1总线读写一个字节

float temp_data(u16 adcv,u16 vref);  //将LTC6804芯片模拟输入数据转换为温度数据
u8 readvol(uint8_t chip); //,uint16_t* cellvol);  //读取第chip个芯片上的单体电池电压数据,并将其保存到全局变量cellvol[chip][]中		 
u8 readtemp(uint8_t chip); //,uint16_t* temp,float* tempf);  //读取第chip个芯片上的模拟数据,获取对应的温度传感器数据,并将其保存到全局变量temp[chip][]中
u8 set_batteryequ(uint8_t chip,uint8_t celln,uint8_t state);  //开启(s=1)/关闭(s=0)第chip个芯片上的第celln个电池模块的均衡功能

#endif


#ifndef __RS485_INSULATION_H
#define __RS485_INSULATION_H

#include "stm32f4xx.h"

/*
主要实现过程:
(1)向绝缘检测模块发送开启检测命令帧,并该函数中发送完数据帧后开启6s的定时器中断;
(2)在中断服务函数中,向模块发送读数据命令帧,使用串口中断接收数据,识别到已经检测完数据后, 将读取数据保存在3个变量中,调用【某操作】函数;如果没有检测完成则延时100ms重复过程;
(3)

*/


//485转换模块控制引脚CON控制指令,ON为发送模式，OFF为接收模式
#define RS485_RE3_ON  GPIO_SetBits(GPIOD,GPIO_Pin_1)  //BCU的绝缘检测模块通信485模块控制引脚CON为PD6
#define RS485_RE3_OFF GPIO_ResetBits(GPIOD,GPIO_Pin_1)

#define RS4853_REC_LEN 20  //RS485_Insulation接收缓冲仅需10字节(每个读数据命令返回帧都只有10字节)

extern uint8_t RS4853_RX_BUF[RS4853_REC_LEN];  //RS485_Insulation接收缓冲区
extern uint8_t RS4853_RX_STA;  //RS485_Insulation接收状态标记
extern uint16_t current_data;
extern uint16_t res_pos_data;
extern uint16_t res_neg_data;



//函数声明
void RS485_Insulation_Init(u32 bound);  //绝缘检测模块485通信初始化
void RS485_Insulation_OpenRead(void);
void RS485_Insulation_ReadData(void);






























#endif
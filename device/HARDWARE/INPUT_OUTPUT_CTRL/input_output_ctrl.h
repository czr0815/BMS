#ifndef __INPUT_OUTPUT_CTRL_H
#define __INPUT_OUTPUT_CTRL_H

#include "stm32f4xx.h"
#include "sys.h"

#define IO_CLOSE 1        //1为拉高引脚,继电器闭合
#define IO_DISCONNECT  0  //0为拉低引脚,继电器断开


//函数声明
void IOControl_Init(void);  //控制功能初始化
u8 IOControl(u8 pin_num, u8 pin_ctrl);  //闭合/断开控制继电器

void Show_RelayState(void);  //显示继电器1/2/3的状态为0

#endif


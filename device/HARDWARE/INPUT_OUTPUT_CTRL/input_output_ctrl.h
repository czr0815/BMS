#ifndef __INPUT_OUTPUT_CTRL_H
#define __INPUT_OUTPUT_CTRL_H

#include "stm32f4xx.h"
#include "sys.h"

#define IO_CLOSE 1        //1Ϊ��������,�̵����պ�
#define IO_DISCONNECT  0  //0Ϊ��������,�̵����Ͽ�


//��������
void IOControl_Init(void);  //���ƹ��ܳ�ʼ��
u8 IOControl(u8 pin_num, u8 pin_ctrl);  //�պ�/�Ͽ����Ƽ̵���

void Show_RelayState(void);  //��ʾ�̵���1/2/3��״̬Ϊ0

#endif


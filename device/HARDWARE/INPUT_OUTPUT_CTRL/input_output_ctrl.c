
/*
	˵��: ��������ӿڿ���
	ֱ��ͨ��MCU���������������BMU/BCU�ϵļ̵�����ʵ�ֿ��ƹ���;
*/

#include "bms_state.h"
#include "input_output_ctrl.h"
#include "rs485_screen.h"




//���ƹ��ܳ�ʼ��
void IOControl_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
#if IS_BMU_ROLE  //�����BMU	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  //ʹ��GPIOEʱ��
	
	//����BMU��Դ�������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //����
  GPIO_Init(GPIOE, &GPIO_InitStructure);  //��ʼ��
	GPIO_ResetBits(GPIOE,GPIO_Pin_9);
	GPIO_ResetBits(GPIOE,GPIO_Pin_10);
	GPIO_ResetBits(GPIOE,GPIO_Pin_11);
	GPIO_ResetBits(GPIOE,GPIO_Pin_12);
	GPIO_ResetBits(GPIOE,GPIO_Pin_13);
	GPIO_ResetBits(GPIOE,GPIO_Pin_14);
	GPIO_ResetBits(GPIOE,GPIO_Pin_15);
	
#endif
	
#if IS_BCU_ROLE  //�����BCU

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  //ʹ��GPIOE��GPIOGʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//����BCU���ƹ���:ǿ���������ӿڡ�3,��Դ�����8,�ɽӵ������1,(��Դ�������2,��δ��ʼ��)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //����
  GPIO_Init(GPIOE, &GPIO_InitStructure);  //��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //����
  GPIO_Init(GPIOG, &GPIO_InitStructure);  //��ʼ��	
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_9);
	GPIO_ResetBits(GPIOE,GPIO_Pin_10);
	GPIO_ResetBits(GPIOE,GPIO_Pin_11);
	GPIO_ResetBits(GPIOE,GPIO_Pin_12);
	GPIO_ResetBits(GPIOE,GPIO_Pin_13);
	GPIO_ResetBits(GPIOE,GPIO_Pin_14);
	GPIO_ResetBits(GPIOE,GPIO_Pin_15);	
	GPIO_ResetBits(GPIOG,GPIO_Pin_2);
	GPIO_ResetBits(GPIOG,GPIO_Pin_3);
	GPIO_ResetBits(GPIOG,GPIO_Pin_4);
	GPIO_ResetBits(GPIOG,GPIO_Pin_5);
	GPIO_ResetBits(GPIOG,GPIO_Pin_6);		
	
#endif
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	//PD15,���ذ������Ź���ָʾ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //����
	GPIO_Init(GPIOD, &GPIO_InitStructure);  //��ʼ��
	GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	
}

/*
	����:����/�رտ��Ƽ̵���
	����:pin_numΪBMU/BCU�ļ̵������(BMU:1-7,BCU:1-12),pin_ctrlָʾ����/�رոü̵���(1Ϊ��������,CON_OUTPUT_xΪ0,�̵����պ�;0��Ϊ�̵����Ͽ�)
	����ֵ:��������������������Ҫ��,�򷵻�1;���򷵻�0
*/
u8 IOControl(u8 pin_num, u8 pin_ctrl)
{
#if IS_BMU_ROLE
	
	if(!((pin_num>0)&&(pin_num<8)&&((pin_ctrl==0)||(pin_ctrl==1))))
		return 0;
	
	if(pin_ctrl==IO_DISCONNECT) {
		GPIO_ResetBits(GPIOE,GPIO_Pin_8<<pin_num);  //pin_num=1��ӦPE9
	}
	else if(pin_ctrl==IO_CLOSE) {
		GPIO_SetBits(GPIOE,GPIO_Pin_8<<pin_num);
	}
	return 1;
#endif
	
#if IS_BCU_ROLE
	
	if(!((pin_num>0)&&(pin_num<13)&&((pin_ctrl==0)||(pin_ctrl==1))))
		return 0;
	
	if(pin_ctrl==IO_DISCONNECT) {
		if(pin_num<8) {
			GPIO_ResetBits(GPIOE,GPIO_Pin_8<<pin_num);  //pin_num=1��ӦPE9
		}
		else {
			GPIO_ResetBits(GPIOG,GPIO_Pin_2<<(pin_num-8));  //pin_num=8��ӦPG2
		}
	}
	else if(pin_ctrl==IO_CLOSE) {
		if(pin_num<8) {
			GPIO_SetBits(GPIOE,GPIO_Pin_8<<pin_num);  //pin_num=1��ӦPE9
		}
		else {
			GPIO_SetBits(GPIOG,GPIO_Pin_2<<(pin_num-8));  //pin_num=8��ӦPG2
		}		
	}
	return 1;
#endif	

}

/* ����:����ʾ�����ͼ̵���1/2/3�Ͽ�״̬����;
����:��;
����ֵ:��;
*/
void Show_RelayState(void)
{
	RS485_Screen_WriteMem(RELAY1_STATE_ADDR,0);  //����Ļ����ʾ�̵���1״̬Ϊ0(�Ͽ�)
	RS485_Screen_WriteMem(RELAY2_STATE_ADDR,0);  //����Ļ����ʾ�̵���2״̬Ϊ0(�Ͽ�)	
	RS485_Screen_WriteMem(RELAY3_STATE_ADDR,0);  //����Ļ����ʾ�̵���3״̬Ϊ0(�Ͽ�)
}	



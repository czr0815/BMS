
/*
  ����:
    BMU�ĵ����ص�ѹ�ɼ�ʹ��SPI1ͨ��ʵ��,Ƭѡ���ֱܷ�ͨ����ͬ������ʵ��,
	ÿ��BMU���ӵ�3����ѹ�ɼ�����Ƭѡ���ŷֱ�ΪPF6 PF7 PF8 PF9 PF10 PC0,
	���ļ��зֱ�ʵ����SPI1�ĳ�ʼ��,��װ�˲�ͬ��ѹ�ɼ�оƬ�ĵ�ѹ�ɼ�����,�Լ������صľ��⹦��(���⿪��/�ر�)
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

										  
void SPI1_Init(void);			 //��ʼ��SPI1��
void SPI1_Control_Init(void);    //�ɼ�оƬ�������ų�ʼ��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI1�ٶ�   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1���߶�дһ���ֽ�

float temp_data(u16 adcv,u16 vref);  //��LTC6804оƬģ����������ת��Ϊ�¶�����
u8 readvol(uint8_t chip); //,uint16_t* cellvol);  //��ȡ��chip��оƬ�ϵĵ����ص�ѹ����,�����䱣�浽ȫ�ֱ���cellvol[chip][]��		 
u8 readtemp(uint8_t chip); //,uint16_t* temp,float* tempf);  //��ȡ��chip��оƬ�ϵ�ģ������,��ȡ��Ӧ���¶ȴ���������,�����䱣�浽ȫ�ֱ���temp[chip][]��
u8 set_batteryequ(uint8_t chip,uint8_t celln,uint8_t state);  //����(s=1)/�ر�(s=0)��chip��оƬ�ϵĵ�celln�����ģ��ľ��⹦��

#endif


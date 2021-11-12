#ifndef __IIC_FUN_H
#define __IIC_FUN_H

#include "sys.h"

//IICxͨ�Ŷ˿ں����ź궨��
#define IIC1_SCL_Port  GPIOB
#define IIC1_SCL_Pin   GPIO_Pin_8
#define IIC1_SDA_Port  GPIOB
#define IIC1_SDA_Pin   GPIO_Pin_9
#define IIC2_SCL_Port  GPIOB
#define IIC2_SCL_Pin   GPIO_Pin_10
#define IIC2_SDA_Port  GPIOB
#define IIC2_SDA_Pin   GPIO_Pin_11
#define IIC3_SCL_Port  GPIOB
#define IIC3_SCL_Pin   GPIO_Pin_15
#define IIC3_SDA_Port  GPIOB
#define IIC3_SDA_Pin   GPIO_Pin_14
#define IIC4_SCL_Port  GPIOF
#define IIC4_SCL_Pin   GPIO_Pin_1
#define IIC4_SDA_Port  GPIOF
#define IIC4_SDA_Pin   GPIO_Pin_0
#define IIC5_SCL_Port  GPIOG
#define IIC5_SCL_Pin   GPIO_Pin_13
#define IIC5_SDA_Port  GPIOG
#define IIC5_SDA_Pin   GPIO_Pin_12

//IICͨ�Ź�����ʱ�������ʱ����
#define DELAY_US_NUM 50  //ʱ��������2*DELAY_US_NUM,��ʱ������100us->10kHz


//IICx��������/���
#define IIC1_SDA_IN  PBin(9)
#define IIC2_SDA_IN  PBin(11)
#define IIC3_SDA_IN  PBin(14)                                  
#define IIC4_SDA_IN  PFin(0)
#define IIC5_SDA_IN  PGin(12)


//IIC��������
void IIC_Init(void);        //IIC���ų�ʼ��
void IIC_SCL_H(u8 x);
void IIC_SCL_L(u8 x);
void IIC_SDA_H(u8 x);
void IIC_SDA_L(u8 x);
u8 IIC_SCL_IN(u8 x);

void IIC_SDA_Set_Out(u8 x);  //IIC��ȡSDA�����ߵĿ���Ȩ
void IIC_SDA_Set_In(u8 x);   //IIC�ͷ�SDA�����ߵĿ���Ȩ

void IIC_Start(u8 x);
void IIC_Stop(u8 x);
u8 IIC_Wait_Ack(u8 x);
void IIC_Ack(u8 x);
void IIC_NAck(u8 x);
void IIC_Send_Byte(u8 x, u8 txd);
u8 IIC_Read_Byte(u8 x, unsigned char ack);


#endif  /* __IIC_FUN_H */

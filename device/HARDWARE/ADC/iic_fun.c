

/*
- ���ļ��������ģ��I2C��Ӳ��I2Cͨ�Ź��ܵ�ʵ��;
- adc_current.c��adc_hv.c�зֱ�ʵ�ֵĵ���������ѹ���Ĺ��ܿ���ͨ�����·�װ�ĺ�����ʵ��;
- BMS��I2C��Դ�ķ������:
	- �������:PB14_SDA + PB15_SCLͨ��,(�ɸ���Ӳ��IICFMP1,Ҳ��)ͨ�����ģ��ʵ��IIC(3)ͨ��;
	- ��ѹ���:PF0_SDA + PF1_SCLͨ��,(�ɸ���Ӳ��IIC2,Ҳ��)ͨ�����ģ��ʵ��IIC(4)ͨ��;
	- CC״̬���:PG12_SDA + PG13_SCLͨ��,��ͨ�����ģ��ʵ��IIC(5)ͨ��;
	- �ⲿEEPROM:PB8_SCL + PB9_SDA,�ɸ���Ӳ��IIC1,Ҳ��ͨ�����ģ��ʵ��IIC(1)ͨ��;
	- ������ʪ�ȴ�����:PB10_SCL + PB11_SDA,�ɸ���Ӳ��IIC2,Ҳ��ͨ�����ģ��IIC(2)ͨ��;
*/

#include "iic_fun.h"
#include "delay.h"
#include "bms_state.h"

/*
�˲���ͨ����ͨIO�����ģ��IICʱ��,��װI2Cͨ�ŵĹ���(����ԭ����������˵:STM32��Ӳ��IIC�ǳ�����,�Ҳ��ȶ�,�ʲ��Ƽ�ʹ��,��˴˲���ͨ��IO�����������ģ��;
��ʱ�����ַ����ģ��IICΪ:
- �ⲿEEPROM:           PB8_IIC1_SCL  + PB9_IIC1_SDA
- ������ʪ�ȴ�����:     PB10_IIC2_SCL + PB11_IIC2_SDA
- �������:             PB15_IIC3_SCL + PB14_IIC3_SDA
- ��ѹ���/��������:  PF1_IIC4_SCL  + PF0_IIC4_SDA
- CC״̬���:           PG13_IIC5_SCL + PG12_IIC5_SDA
ע:���ļ��е�IICͨ�Ź��ܾ�ʹ�����ģ�ⷽʽʵ��;
*/


/* IIC���ų�ʼ��
����:��ʼ�����ģ��IIC���ܵ�IO��
����:��
����ֵ:��
*/
void IIC_Init(void){	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);  //ʹ��GPIOFʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);  //ʹ��GPIOGʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //����
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_14|GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_SetBits(GPIOF,GPIO_Pin_0|GPIO_Pin_1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_SetBits(GPIOG,GPIO_Pin_12|GPIO_Pin_13);
	
}

//IICx��SCL���������
void IIC_SCL_H(u8 x){
	switch(x){
		case 1:{
			GPIO_SetBits(IIC1_SCL_Port,IIC1_SCL_Pin);
			break;
		}
		case 2:{
			GPIO_SetBits(IIC2_SCL_Port,IIC2_SCL_Pin);						
			break;
		}	
		case 3:{
			GPIO_SetBits(IIC3_SCL_Port,IIC3_SCL_Pin);			
			break;
		}
		case 4:{
			GPIO_SetBits(IIC4_SCL_Port,IIC4_SCL_Pin);
			break;
		}		
		case 5:{
			GPIO_SetBits(IIC5_SCL_Port,IIC5_SCL_Pin);			
			break;
		}
	}		
}

//IICx��SCL���������
void IIC_SCL_L(u8 x){
	switch(x){
		case 1:{
			GPIO_ResetBits(IIC1_SCL_Port,IIC1_SCL_Pin);
			break;
		}
		case 2:{
			GPIO_ResetBits(IIC2_SCL_Port,IIC2_SCL_Pin);						
			break;
		}	
		case 3:{
			GPIO_ResetBits(IIC3_SCL_Port,IIC3_SCL_Pin);			
			break;
		}
		case 4:{
			GPIO_ResetBits(IIC4_SCL_Port,IIC4_SCL_Pin);
			break;
		}		
		case 5:{
			GPIO_ResetBits(IIC5_SCL_Port,IIC5_SCL_Pin);			
			break;
		}		
	}
}

//IICx��SDA���������
void IIC_SDA_H(u8 x){
	switch(x){
		case 1:{
			GPIO_SetBits(IIC1_SDA_Port,IIC1_SDA_Pin);
			break;
		}
		case 2:{
			GPIO_SetBits(IIC2_SDA_Port,IIC2_SDA_Pin);						
			break;
		}	
		case 3:{
			GPIO_SetBits(IIC3_SDA_Port,IIC3_SDA_Pin);			
			break;
		}
		case 4:{
			GPIO_SetBits(IIC4_SDA_Port,IIC4_SDA_Pin);
			break;
		}		
		case 5:{
			GPIO_SetBits(IIC5_SDA_Port,IIC5_SDA_Pin);			
			break;
		}		
	}	
}

//IICx��SDA���������
void IIC_SDA_L(u8 x){
	switch(x){
		case 1:{
			GPIO_ResetBits(IIC1_SDA_Port,IIC1_SDA_Pin);
			break;
		}
		case 2:{
			GPIO_ResetBits(IIC2_SDA_Port,IIC2_SDA_Pin);						
			break;
		}	
		case 3:{
			GPIO_ResetBits(IIC3_SDA_Port,IIC3_SDA_Pin);			
			break;
		}
		case 4:{
			GPIO_ResetBits(IIC4_SDA_Port,IIC4_SDA_Pin);
			break;
		}		
		case 5:{
			GPIO_ResetBits(IIC5_SDA_Port,IIC5_SDA_Pin);			
			break;
		}		
	}		
}

//IICx��SDA����
u8 IIC_SDA_IN(u8 x){
	switch(x){
		case 1:{
			return IIC1_SDA_IN;
		}
		case 2:{
			return IIC2_SDA_IN;						
		}	
		case 3:{
			return IIC3_SDA_IN;
		}
		case 4:{
			return IIC4_SDA_IN;
		}		
		case 5:{
			return IIC5_SDA_IN;
		}		
	}	
}

//����IICx��SDAΪ���ģʽ(������ȡSDA�Ŀ���Ȩ)
void IIC_SDA_Set_Out(u8 x) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //����	
	
	switch(x){
		case 1:{
			//GPIO_DeInit(IIC1_SDA_Port,IIC1_SDA_Pin);  //IIC1_SDA���Ž��ʼ��(����Ϊ�ϵ縴λ״̬��ֵ��
			GPIO_InitStructure.GPIO_Pin = IIC1_SDA_Pin;
			GPIO_Init(IIC1_SDA_Port, &GPIO_InitStructure);				
			break;
		}
		case 2:{
			//GPIO_DeInit(IIC2_SDA_Port,IIC2_SDA_Pin);  //IIC2_SDA���Ž��ʼ��(����Ϊ�ϵ縴λ״̬��ֵ��
			GPIO_InitStructure.GPIO_Pin = IIC2_SDA_Pin;
			GPIO_Init(IIC2_SDA_Port, &GPIO_InitStructure);	
			break;
		}
		case 3:{
			//GPIO_DeInit(IIC3_SDA_Port,IIC3_SDA_Pin);  //IIC3_SDA���Ž��ʼ��(����Ϊ�ϵ縴λ״̬��ֵ��
			GPIO_InitStructure.GPIO_Pin = IIC3_SDA_Pin;
			GPIO_Init(IIC3_SDA_Port, &GPIO_InitStructure);	
			break;
		}
		case 4:{
			//GPIO_DeInit(IIC4_SDA_Port,IIC4_SDA_Pin);  //IIC4_SDA���Ž��ʼ��(����Ϊ�ϵ縴λ״̬��ֵ��
			GPIO_InitStructure.GPIO_Pin = IIC4_SDA_Pin;
			GPIO_Init(IIC4_SDA_Port, &GPIO_InitStructure);				
			break;
		}
		case 5:{
			//GPIO_DeInit(IIC5_SDA_Port,IIC5_SDA_Pin);  //IIC5_SDA���Ž��ʼ��(����Ϊ�ϵ縴λ״̬��ֵ��
			GPIO_InitStructure.GPIO_Pin = IIC5_SDA_Pin;
			GPIO_Init(IIC5_SDA_Port, &GPIO_InitStructure);				
			break;
		}
	}		
}

//����IICx��SDAΪ����ģʽ(�����ͷ�SDA�Ŀ���Ȩ)
void IIC_SDA_Set_In(u8 x) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   //����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //����	

	switch(x){
		case 1:{
			//GPIO_DeInit(IIC1_SDA_Port,IIC1_SDA_Pin);  //IIC1_SDA���Ž��ʼ��(����Ϊ�ϵ縴λ״̬��ֵ��
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
			GPIO_Init(IIC1_SDA_Port, &GPIO_InitStructure);				
			break;
		}
		case 2:{
			//GPIO_DeInit(IIC2_SDA_Port,IIC2_SDA_Pin);  //IIC2_SDA���Ž��ʼ��(����Ϊ�ϵ縴λ״̬��ֵ��
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
			GPIO_Init(IIC2_SDA_Port, &GPIO_InitStructure);	
			break;
		}
		case 3:{
			//GPIO_DeInit(IIC3_SDA_Port,IIC3_SDA_Pin);  //IIC3_SDA���Ž��ʼ��(����Ϊ�ϵ縴λ״̬��ֵ��
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
			GPIO_Init(IIC3_SDA_Port, &GPIO_InitStructure);	
			break;
		}
		case 4:{
			//GPIO_DeInit(IIC4_SDA_Port,IIC4_SDA_Pin);  //IIC4_SDA���Ž��ʼ��(����Ϊ�ϵ縴λ״̬��ֵ��
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
			GPIO_Init(IIC4_SDA_Port, &GPIO_InitStructure);				
			break;
		}
		case 5:{
			//GPIO_DeInit(IIC5_SDA_Port,IIC5_SDA_Pin);  //IIC5_SDA���Ž��ʼ��(����Ϊ�ϵ縴λ״̬��ֵ��
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
			GPIO_Init(IIC5_SDA_Port, &GPIO_InitStructure);				
			break;
		}
	}	
}



/* IICx���Ϳ�ʼ�ź�
����:ͨ��IICx���Ϳ�ʼ�ź�
����:x:����ѡȡIIC,��ֵ��ΧӦ��1~5֮��
����ֵ:��
*/
void IIC_Start(u8 x) {
	if(x>5)
		return;
	
	IIC_SDA_Set_Out(x);  //IICx��SDA�����
	IIC_SDA_H(x);
	IIC_SCL_H(x);
	delay_us(DELAY_US_NUM);  //��ʱ
	IIC_SDA_L(x);    //IICx��SDA��,��ʼ�ź�:SCL��ʱSDA�ɸ߱��
	delay_us(DELAY_US_NUM);  //��ʱ
	IIC_SCL_L(x);    //ǯסI2C���ߣ�׼�����ͻ��������
	
}
	
/* IICx����ֹͣ�ź�
����:ͨ��IICx����ֹͣ�ź�
����:x:����ѡȡIIC,��ֵ��ΧӦ��1~5֮��
����ֵ:��
*/
void IIC_Stop(u8 x) {
	if(x>5)
		return;
	
	IIC_SDA_Set_Out(x);  //IICx��SDA�����
	IIC_SCL_L(x);    
	IIC_SDA_L(x);
	delay_us(DELAY_US_NUM);
	IIC_SCL_H(x);    //ֹͣ�ź�:SCL��ʱSDA�ɵͱ��
	IIC_SDA_H(x);
	delay_us(DELAY_US_NUM);
	IIC_SCL_L(x);  //������IIC�����źź�����SCL
}

/* IICx�ȴ�AckӦ���źŵĵ���
����:IICx�ȴ�AckӦ���źŵĵ���
����:x:����ѡȡIIC,��ֵ��ΧӦ��1~5֮��
����ֵ:1������Ӧ��ʧ��,0:����Ӧ��ɹ�
*/
u8 IIC_Wait_Ack(u8 x) {
	u8 ErrTime=0;
	if(x>5)
		return 1;
	
	IIC_SDA_Set_In(x);      //SDA����Ϊ����  
	IIC_SDA_H(x);
	delay_us(DELAY_US_NUM);	
	IIC_SCL_H(x);
	delay_us(DELAY_US_NUM);
	
	while(IIC_SDA_IN(x)) {
		delay_us(1);
		ErrTime++;
		if(ErrTime>250)
		{
			//IIC_Stop(x);
			return 1;
		}
	}
	
	IIC_SCL_L(x);         //ʱ�����0 	   
	return 0;  
}

/* IICx����AckӦ��
����:ͨ��IICx����Ack�ź� ����AckӦ��
����:x:����ѡȡIIC,��ֵ��ΧӦ��1~5֮��
����ֵ:��
*/
void IIC_Ack(u8 x) {
	if(x>5)
		return;
	
	IIC_SCL_L(x);
	IIC_SDA_Set_Out(x);
	IIC_SDA_L(x);
	delay_us(DELAY_US_NUM);
	IIC_SCL_H(x);
	delay_us(DELAY_US_NUM);
	IIC_SCL_L(x);
	
}
	
/* IICx������AckӦ��
����:IICx������Ack�ź� ������AckӦ��
����:x:����ѡȡIIC,��ֵ��ΧӦ��1~5֮��
����ֵ:��
*/
void IIC_NAck(u8 x) {
	if(x>5)
		return;
	
	IIC_SCL_L(x);
	IIC_SDA_Set_Out(x);
	IIC_SDA_H(x);
	delay_us(DELAY_US_NUM);
	IIC_SCL_H(x);
	delay_us(DELAY_US_NUM);
	IIC_SCL_L(x);
}

/* IICx����һ���ֽ�
����:ͨ��IICx����һ���ֽ�
����:x:����ѡȡIIC,��ֵ��ΧӦ��1~5֮��,txd:Ҫ���͵��ֽ�
����ֵ:�ӻ�����Ӧ��,1:�ӻ���Ӧ��;0:�ӻ���Ӧ��
*/
void IIC_Send_Byte(u8 x, u8 txd) {
	u8 t;
	if(x>5)
		return;
	
	IIC_SDA_Set_Out(x); 	    
	IIC_SCL_L(x);    //����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++) {              
		if((txd&0x80)>>7)
			IIC_SDA_H(x);
		else
			IIC_SDA_L(x);
		txd<<=1;
		delay_us(DELAY_US_NUM);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_H(x);
		delay_us(DELAY_US_NUM); 
		IIC_SCL_L(x);	
		delay_us(DELAY_US_NUM);
	}
	//���شӻ�����Ӧ��
}

/* IICx��ȡһ���ֽ�
����:ͨ��IICx��ȡһ���ֽ�
����:x:����ѡȡIIC,��ֵ��ΧӦ��1~5֮��,ack:1��ʾ����Ack,0��ʾ����nAck
����ֵ:ͨ��IICx��ȡ���ֽ�ֵ
*/
u8 IIC_Read_Byte(u8 x, unsigned char ack) {
	unsigned char i,receive=0;
	IIC_SDA_Set_In(x);//SDA����Ϊ����
  for(i=0;i<8;i++ ) {
    IIC_SCL_L(x); 
    delay_us(DELAY_US_NUM);
		IIC_SCL_H(x);
    receive<<=1;
    if(IIC_SDA_IN(x))
			receive++;   
		delay_us(DELAY_US_NUM); 
  }
	if (!ack)
			IIC_NAck(x);//����NACK
	else
			IIC_Ack(x); //����ACK   
	return receive;
}



//***************����Bվ��Ƶд��**************
/*
ע:����ͨ�ŵķ�װ��,��ʱʱ�����ͨ�������й�,�����޸�

////IICx��ʼ�ź�
//void IIC_Start(u8 x) {
//	
//	IIC_SDA_Out(x);  //IICx���ģʽ
//	
//	IIC_SDA_H(x);
//	IIC_SCL_H(x);
//	delay_us(DELAY_US_NUM);
//	
//	IIC_SDA_L(x);
//	delay_us(DELAY_US_NUM);
//	
//	IIC_SDA_H(x);  //???
//	delay_us(DELAY_US_NUM);	
//	
//}


////IICx�����ź�
////SCL��ʱSDA�ɵ͵���
//void IIC_Stop(u8 x) {
//	
//	IIC_SDA_Out(x);  //IIC3���ģʽ
//	
//	IIC_SDA_L(x);
//	delay_us(DELAY_US_NUM);
//	
//	IIC_SCL_H(x);
//	delay_us(DELAY_US_NUM);
//	
//	IIC_SDA_H(x);
//	delay_us(DELAY_US_NUM);
//	
//	IIC_SCL_L(x);
//	delay_us(DELAY_US_NUM);
//	
//}


////��ȡIICx��Ӧ���ź�(ͨ�����ӻ��Ƿ�������SDA������)
//u8 IIC_GetAck(u8 x){
//	IIC_SDA_Out(x);  //�Ƚ�����������,�ӳ�һ��ʱ��
//	IIC_SDA_H(x);
//	delay_us(DELAY_US_NUM);
//	
//	IIC_SDA_In(x);
//	
//	//����һ��ʱ��
//	IIC_SCL_L(x);
//	delay_us(DELAY_US_NUM);
//	IIC_SCL_H(x);
//	delay_us(DELAY_US_NUM);
//	
//	//��ȡSDA_IN�ĵ�ƽ�Ƿ�Ϊ��
//	

//}


////IIC��������
////����:ʱ��Ϊ��ʱ�任����,Ȼ������ʱ��,���ߵĹ�����SDA�����Ա�֤������Ч
//void IIC_WriteByte(u8 x, u8 data)
//{
//	u8 i=0;
//	IIC_SDA_Out(x);  //IICx���ģʽ
//	
//	for(i=0;i<8;++i){
//		IIC3_SCL_L;
//		delay_us(DELAY_US_NUM);
//		
//		if(data&0x80){
//			IIC_SDA_H(x);
//		}	
//		else{
//			IIC_SDA_L(x);
//		}
//		data = data<<1;
//		
//		IIC_SCL_H(x);
//		delay_us(DELAY_US_NUM);
//	}
//	
//	IIC_SCL_L(x);  //����ʱ����
//	delay_us(DELAY_US_NUM);
//	
//	IIC_GetAck(x);  //��ȡӦ���ź�
//	
//}

////IIC��ȡ����
//u8 IIC_ReadByte(u8 x, u8 ack)
//{
//	u8 i=0;
//	u8 data=0;
//	
//	IIC_SDA_In(x);
//	
//	IIC_SCL_L(x);
//	delay_us(DELAY_US_NUM);
//	
//	for(i=0;i<8;i++){
//		IIC_SCL_H(x);
//		delay_us(DELAY_US_NUM);
//		if(IIC_SDA_IN==1){
//			data += 1;
//		}
//		data = data<<1;
//		IIC_SCL_L(x);
//		delay_us(DELAY_US_NUM);
//	}
//	if(ack){
//		IIC_ACK(x);
//	}
//	else{
//		IIC_NACK(x);
//	}
//	
//	return data;
//}

*/







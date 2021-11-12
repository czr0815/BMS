

/*
- 本文件包含软件模拟I2C、硬件I2C通信功能的实现;
- adc_current.c和adc_hv.c中分别实现的电流检测和总压检测的功能可以通过以下封装的函数来实现;
- BMS中I2C资源的分配情况:
	- 电流检测:PB14_SDA + PB15_SCL通信,(可复用硬件IICFMP1,也可)通过软件模拟实现IIC(3)通信;
	- 高压检测:PF0_SDA + PF1_SCL通信,(可复用硬件IIC2,也可)通过软件模拟实现IIC(4)通信;
	- CC状态检测:PG12_SDA + PG13_SCL通信,需通过软件模拟实现IIC(5)通信;
	- 外部EEPROM:PB8_SCL + PB9_SDA,可复用硬件IIC1,也可通过软件模拟实现IIC(1)通信;
	- 板载温湿度传感器:PB10_SCL + PB11_SDA,可复用硬件IIC2,也可通过软件模拟IIC(2)通信;
*/

#include "iic_fun.h"
#include "delay.h"
#include "bms_state.h"

/*
此部分通过普通IO口软件模拟IIC时序,封装I2C通信的功能(正点原子资料中有说:STM32的硬件IIC非常复杂,且不稳定,故不推荐使用,因此此部分通过IO口来进行软件模拟;
此时各部分分配的模拟IIC为:
- 外部EEPROM:           PB8_IIC1_SCL  + PB9_IIC1_SDA
- 板载温湿度传感器:     PB10_IIC2_SCL + PB11_IIC2_SDA
- 电流检测:             PB15_IIC3_SCL + PB14_IIC3_SDA
- 高压检测/烟雾传感器:  PF1_IIC4_SCL  + PF0_IIC4_SDA
- CC状态检测:           PG13_IIC5_SCL + PG12_IIC5_SDA
注:本文件中的IIC通信功能均使用软件模拟方式实现;
*/


/* IIC引脚初始化
功能:初始化软件模拟IIC功能的IO口
参数:无
返回值:无
*/
void IIC_Init(void){	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);  //使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);  //使能GPIOG时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_14|GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_SetBits(GPIOF,GPIO_Pin_0|GPIO_Pin_1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_SetBits(GPIOG,GPIO_Pin_12|GPIO_Pin_13);
	
}

//IICx的SCL引脚输出高
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

//IICx的SCL引脚输出低
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

//IICx的SDA引脚输出高
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

//IICx的SDA引脚输出低
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

//IICx的SDA输入
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

//设置IICx的SDA为输出模式(主机获取SDA的控制权)
void IIC_SDA_Set_Out(u8 x) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉	
	
	switch(x){
		case 1:{
			//GPIO_DeInit(IIC1_SDA_Port,IIC1_SDA_Pin);  //IIC1_SDA引脚解初始化(设置为上电复位状态的值）
			GPIO_InitStructure.GPIO_Pin = IIC1_SDA_Pin;
			GPIO_Init(IIC1_SDA_Port, &GPIO_InitStructure);				
			break;
		}
		case 2:{
			//GPIO_DeInit(IIC2_SDA_Port,IIC2_SDA_Pin);  //IIC2_SDA引脚解初始化(设置为上电复位状态的值）
			GPIO_InitStructure.GPIO_Pin = IIC2_SDA_Pin;
			GPIO_Init(IIC2_SDA_Port, &GPIO_InitStructure);	
			break;
		}
		case 3:{
			//GPIO_DeInit(IIC3_SDA_Port,IIC3_SDA_Pin);  //IIC3_SDA引脚解初始化(设置为上电复位状态的值）
			GPIO_InitStructure.GPIO_Pin = IIC3_SDA_Pin;
			GPIO_Init(IIC3_SDA_Port, &GPIO_InitStructure);	
			break;
		}
		case 4:{
			//GPIO_DeInit(IIC4_SDA_Port,IIC4_SDA_Pin);  //IIC4_SDA引脚解初始化(设置为上电复位状态的值）
			GPIO_InitStructure.GPIO_Pin = IIC4_SDA_Pin;
			GPIO_Init(IIC4_SDA_Port, &GPIO_InitStructure);				
			break;
		}
		case 5:{
			//GPIO_DeInit(IIC5_SDA_Port,IIC5_SDA_Pin);  //IIC5_SDA引脚解初始化(设置为上电复位状态的值）
			GPIO_InitStructure.GPIO_Pin = IIC5_SDA_Pin;
			GPIO_Init(IIC5_SDA_Port, &GPIO_InitStructure);				
			break;
		}
	}		
}

//设置IICx的SDA为输入模式(主机释放SDA的控制权)
void IIC_SDA_Set_In(u8 x) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   //输入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉	

	switch(x){
		case 1:{
			//GPIO_DeInit(IIC1_SDA_Port,IIC1_SDA_Pin);  //IIC1_SDA引脚解初始化(设置为上电复位状态的值）
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
			GPIO_Init(IIC1_SDA_Port, &GPIO_InitStructure);				
			break;
		}
		case 2:{
			//GPIO_DeInit(IIC2_SDA_Port,IIC2_SDA_Pin);  //IIC2_SDA引脚解初始化(设置为上电复位状态的值）
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
			GPIO_Init(IIC2_SDA_Port, &GPIO_InitStructure);	
			break;
		}
		case 3:{
			//GPIO_DeInit(IIC3_SDA_Port,IIC3_SDA_Pin);  //IIC3_SDA引脚解初始化(设置为上电复位状态的值）
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
			GPIO_Init(IIC3_SDA_Port, &GPIO_InitStructure);	
			break;
		}
		case 4:{
			//GPIO_DeInit(IIC4_SDA_Port,IIC4_SDA_Pin);  //IIC4_SDA引脚解初始化(设置为上电复位状态的值）
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
			GPIO_Init(IIC4_SDA_Port, &GPIO_InitStructure);				
			break;
		}
		case 5:{
			//GPIO_DeInit(IIC5_SDA_Port,IIC5_SDA_Pin);  //IIC5_SDA引脚解初始化(设置为上电复位状态的值）
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
			GPIO_Init(IIC5_SDA_Port, &GPIO_InitStructure);				
			break;
		}
	}	
}



/* IICx发送开始信号
功能:通过IICx发送开始信号
参数:x:用于选取IIC,该值范围应在1~5之间
返回值:无
*/
void IIC_Start(u8 x) {
	if(x>5)
		return;
	
	IIC_SDA_Set_Out(x);  //IICx的SDA线输出
	IIC_SDA_H(x);
	IIC_SCL_H(x);
	delay_us(DELAY_US_NUM);  //延时
	IIC_SDA_L(x);    //IICx的SDA低,开始信号:SCL高时SDA由高变低
	delay_us(DELAY_US_NUM);  //延时
	IIC_SCL_L(x);    //钳住I2C总线，准备发送或接收数据
	
}
	
/* IICx发送停止信号
功能:通过IICx发送停止信号
参数:x:用于选取IIC,该值范围应在1~5之间
返回值:无
*/
void IIC_Stop(u8 x) {
	if(x>5)
		return;
	
	IIC_SDA_Set_Out(x);  //IICx的SDA线输出
	IIC_SCL_L(x);    
	IIC_SDA_L(x);
	delay_us(DELAY_US_NUM);
	IIC_SCL_H(x);    //停止信号:SCL高时SDA由低变高
	IIC_SDA_H(x);
	delay_us(DELAY_US_NUM);
	IIC_SCL_L(x);  //发送完IIC结束信号后拉低SCL
}

/* IICx等待Ack应答信号的到来
功能:IICx等待Ack应答信号的到来
参数:x:用于选取IIC,该值范围应在1~5之间
返回值:1：接收应答失败,0:接收应答成功
*/
u8 IIC_Wait_Ack(u8 x) {
	u8 ErrTime=0;
	if(x>5)
		return 1;
	
	IIC_SDA_Set_In(x);      //SDA设置为输入  
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
	
	IIC_SCL_L(x);         //时钟输出0 	   
	return 0;  
}

/* IICx产生Ack应答
功能:通过IICx发送Ack信号 产生Ack应答
参数:x:用于选取IIC,该值范围应在1~5之间
返回值:无
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
	
/* IICx不产生Ack应答
功能:IICx不发送Ack信号 不产生Ack应答
参数:x:用于选取IIC,该值范围应在1~5之间
返回值:无
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

/* IICx发送一个字节
功能:通过IICx发送一个字节
参数:x:用于选取IIC,该值范围应在1~5之间,txd:要发送的字节
返回值:从机有无应答,1:从机有应答;0:从机无应答
*/
void IIC_Send_Byte(u8 x, u8 txd) {
	u8 t;
	if(x>5)
		return;
	
	IIC_SDA_Set_Out(x); 	    
	IIC_SCL_L(x);    //拉低时钟开始数据传输
	for(t=0;t<8;t++) {              
		if((txd&0x80)>>7)
			IIC_SDA_H(x);
		else
			IIC_SDA_L(x);
		txd<<=1;
		delay_us(DELAY_US_NUM);   //对TEA5767这三个延时都是必须的
		IIC_SCL_H(x);
		delay_us(DELAY_US_NUM); 
		IIC_SCL_L(x);	
		delay_us(DELAY_US_NUM);
	}
	//返回从机有无应答
}

/* IICx读取一个字节
功能:通过IICx读取一个字节
参数:x:用于选取IIC,该值范围应在1~5之间,ack:1表示发送Ack,0表示发送nAck
返回值:通过IICx读取的字节值
*/
u8 IIC_Read_Byte(u8 x, unsigned char ack) {
	unsigned char i,receive=0;
	IIC_SDA_Set_In(x);//SDA设置为输入
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
			IIC_NAck(x);//发送NACK
	else
			IIC_Ack(x); //发送ACK   
	return receive;
}



//***************跟着B站视频写的**************
/*
注:以下通信的封装中,延时时间均和通信速率有关,谨慎修改

////IICx开始信号
//void IIC_Start(u8 x) {
//	
//	IIC_SDA_Out(x);  //IICx输出模式
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


////IICx结束信号
////SCL高时SDA由低到高
//void IIC_Stop(u8 x) {
//	
//	IIC_SDA_Out(x);  //IIC3输出模式
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


////获取IICx的应答信号(通过检测从机是否拉低了SDA数据线)
//u8 IIC_GetAck(u8 x){
//	IIC_SDA_Out(x);  //先将数据线拉高,延迟一段时间
//	IIC_SDA_H(x);
//	delay_us(DELAY_US_NUM);
//	
//	IIC_SDA_In(x);
//	
//	//产生一个时钟
//	IIC_SCL_L(x);
//	delay_us(DELAY_US_NUM);
//	IIC_SCL_H(x);
//	delay_us(DELAY_US_NUM);
//	
//	//读取SDA_IN的电平是否为低
//	

//}


////IIC发送数据
////流程:时钟为低时变换数据,然后拉高时钟,拉高的过程中SDA保持以保证数据有效
//void IIC_WriteByte(u8 x, u8 data)
//{
//	u8 i=0;
//	IIC_SDA_Out(x);  //IICx输出模式
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
//	IIC_SCL_L(x);  //拉低时钟线
//	delay_us(DELAY_US_NUM);
//	
//	IIC_GetAck(x);  //获取应答信号
//	
//}

////IIC读取数据
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







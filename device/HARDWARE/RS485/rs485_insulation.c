
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "rs485_insulation.h"
#include "bms_state.h"

#include "rs485_DTU.h" //用于测试

//变量定义
//给绝缘测量模块发送命令:
const u8 order_insulation_open[8] = {0x10,0x02,0x00,0x02,0x10,0x20,0xd6,0x93};  //第1个字节为地址(10 02 00 02 10 20 d6 93)
const u8 order_insulation_close[8] = {0x10,0x02,0x00,0x02,0x10,0x00,0xd6,0x2b};  //(10 02 00 02 10 00 d6 2b)
const u8 order_insulation_readdata[8] = {0x10,0x01,0x02,0x03,0x04,0x05,0x0c,0x30};  //(10 01 02 03 04 05 0c 30)

const unsigned char CRCHi[256] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
}; 
const unsigned char CRCLo[256] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
	0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
	0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
	0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
	0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
	0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
	0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
}; 

//4853_绝缘检测中断接收数据相关变量定义
uint8_t RS4853_RX_BUF[RS4853_REC_LEN];  //RS485_Insulation接收缓冲区
uint8_t RS4853_RX_STA;  //RS485_Insulation接收状态标记


//获取绝缘检测模块数据帧中的校验码
unsigned short chkcrc(unsigned char *pcData,unsigned char cDataLeng) {
	unsigned int iTemp=0;
	unsigned char cCRCHi = 0xFF ;
	unsigned char cCRCLo = 0xFF ;
	unsigned char cIndex ;
	while(cDataLeng--) {
		cIndex = cCRCHi ^ *pcData++ ;
		cCRCHi = cCRCLo ^ CRCHi[cIndex] ;
		cCRCLo = CRCLo[cIndex] ;
	}
	iTemp+=cCRCHi;
	iTemp<<=8;
	iTemp+=cCRCLo;
	return (iTemp);
} 


//绝缘检测模块初始化
//暂时修改为使用USART2收发数据测试
void RS485_Insulation_Init(u32 bound)
{
	
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
	
	//串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3); //GPIOD8复用为USART3
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); //GPIOD9复用为USART3
	
	//USART3端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //GPIOD8与GPIOD9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PD8,PD9
	
	//USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//绝缘模块默认无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	
	USART_Cmd(USART3, ENABLE);  //使能串口3
	
	//USART_ClearFlag(USART3, USART_FLAG_TC);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断
	
	//Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;  //抢占优先级6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
	//配置PD1(BCU的绝缘检测485通信模块控制引脚)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化
	GPIO_SetBits(GPIOD,GPIO_Pin_1);  //拉高引脚,模块允许输入
	
}

/*
	功能：给绝缘检测模块发送命令帧,开启绝缘检测功能
	参数：1字节地址码,默认参数为0x01(地址开关都关闭时地址为01)
	说明: 绝缘检测模块响应后,会持续约5s的绝缘状态检测过程,5s后绝缘状态检测完成(读数据可获得绝缘检测状态),检测完成后有约15s的读数据时间
*/
void RS485_Insulation_OpenRead(void) {
	
	u8 i=0;
	RS4853_RX_STA = 0;  //开启绝缘检测前先清空绝缘检测数据读取状态标志位
	
	//使用USART3发送帧信息
	RS485_RE3_OFF;  //拉低引脚,模块输出
	for(i=0; i<8; i++){
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		USART_SendData(USART3,order_insulation_open[i]);
	}
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
	RS485_RE3_ON;  //拉高引脚,模块允许输入
	
}

void RS485_Insulation_ReadData(void) {
	
	u8 i=0,ch;
	u8 not_correct_data = 1;
	RS4853_RX_STA = 0;  //开启绝缘检测前先清空绝缘检测数据读取状态标志位
	RS4853_RX_BUF[1] = 0x56;  //测试数据是否每次正常读取
	
		
	//使用USART3发送:读取数据命令帧
	RS485_RE3_OFF;  //拉低引脚,模块输出
	for(i=0; i<8; i++){
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		USART_SendData(USART3,order_insulation_readdata[i]);
	}
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
	RS485_RE3_ON;  //拉高引脚,模块允许输入
	
	
	//处理数据在中断服务函数中执行
//	delay_ms(50);  //延时等待数据接收完成(9600bps,1200byte/s,1ms1.2字节)
//	
//	if(RS4853_RX_BUF[1]==0x30)  //获取到了测试完成后的数据
//		not_correct_data = 0;
//	
//	dcvol_data = (RS4853_RX_BUF[2]<<8)|(RS4853_RX_BUF[3]<<0);
//	res_pos_data = (RS4853_RX_BUF[4]<<8)|(RS4853_RX_BUF[5]<<0);	
//	res_neg_data = (RS4853_RX_BUF[6]<<8)|(RS4853_RX_BUF[7]<<0);
//	
//	ch = RS4853_RX_STA;
//		
//	if(ch==0x8A) {  //如果10个字节的数据接收完毕
//		
//		
//	}
		
}


void USART3_IRQHandler(void) {
	u8 i;
	u8 Res;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){  //接收数据寄存器非空中断
		Res = USART_ReceiveData(USART3);  //接收到的字节
		i = RS4853_RX_STA & 0x7F;
		RS4853_RX_BUF[i++] = Res;
		if(i==10){
			RS4853_RX_STA = i|0x80;  //接收完10个字节
			
			//打印测试
			//printf("Insulation received data.\r\n");
			printf("Insu->...\r\n");
			
			//接收到正确的绝缘状态读取完成的数据后进行处理
			if(RS4853_RX_BUF[1]==0x30){
				
				printf("Insu correct.\r\n");
				
				//数据处理(将数据结果存储至16位无符号整数的全局变量中)
				dcvol_data = (RS4853_RX_BUF[2]<<8)|(RS4853_RX_BUF[3]<<0);    //单位:V
				res_pos_data = (RS4853_RX_BUF[4]<<8)|(RS4853_RX_BUF[5]<<0);  //单位:kΩ
				res_neg_data = (RS4853_RX_BUF[6]<<8)|(RS4853_RX_BUF[7]<<0);  //单位:kΩ			
				
				if(dcvol_data!=0){  //电压数据不为0则更新float类型的绝缘数据
					insuval_pos = ((float)res_pos_data) * 1000 / ((float)dcvol_data);
					insuval_neg = ((float)res_neg_data) * 1000 / ((float)dcvol_data);
				}
			}
			
		}
		else{
			RS4853_RX_STA = i;  //未接收完10个字节
		}
	}
	
}
























#include "bms_state.h"
#include "rs485_screen.h"
#include "usart.h"
#include "stdlib.h"

//相关变量定义
uint8_t RS4856_RX_BUF[RS4856_REC_LEN];  //RS485_Screen接收缓冲区
uint8_t RS4856_RX_LEN;
uint8_t RS4856_RX_STA;  //RS485_Screen接收状态标记




//RS485_Screen初始化函数
//配置串口USART6与485转换模块CON引脚PG7
void RS485_Screen_Init(u32 bound) {
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); //GPIOG9复用为USART6
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //GPIOG14复用为USART6	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; //GPIOG9与GPIOG14
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOG,&GPIO_InitStructure); //初始化PG9|PG14	
	
	//USART串口配置
	USART_InitStructure.USART_BaudRate = bound;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART6, &USART_InitStructure);

	USART_Cmd(USART6, ENABLE);  //使能

	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启中断
	//USART1 NVIC 
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;  //抢占优先级5
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
	
	
	
	//配置PG7(BCU的触控屏通信485模块控制引脚)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);  //初始化
	GPIO_ResetBits(GPIOG,GPIO_Pin_7);

	RS485_RE4_ON;

	RS4856_RX_LEN = 0;
}


void RS485_Screen_SendData(const uint8_t *data, uint8_t len){
  uint8_t i =0;
  RS485_RE4_OFF;
  for(i=0; i<len; i++){
		while(USART_GetFlagStatus(USART6,USART_FLAG_TC)!=SET);
		USART_SendData(USART6,data[i]);
  }
	while(USART_GetFlagStatus(USART6,USART_FLAG_TC)!=SET);
  RS485_RE4_ON;
}
	

/*与迪文显示屏相匹配的字符串接收功能:
	对于读取寄存器和读取变量存储器有不同的应答方式：
		- 读取寄存器: 应答为 寄存器地址(0x00-0xFF)+字节数据长度+读取的寄存器数据, 应答最多允许传输257个字节,1+1+255;
		- 读取变量存储器: 应答为 变量存储器地址(0x0000-0x6FFF)+变量数据字长度+读取的变量数据, 应答最多允许传输257个字节,1*2+1+127*2;
	该中断服务函数,按照变量存储器读函数的应答格式进行编写;
*/
/* USART1中断服务函数
void USART1_IRQHandler(void) {
	u8 i;
	
  if((USART1->SR)&(1<<5)){
    RS485_SCREEN_RX_BUF[RS485_SCREEN_RX_LEN++]=USART1->DR;
  }
  if((USART1->SR)&(1<<4)){
    RS485_SCREEN_RX_BUF[RS485_SCREEN_RX_LEN]=0;  //字符串结束标志
		USART1->SR;
		USART1->DR;
	}
	
	for(i=0;i<RS485_SCREEN_RX_LEN;i++){
		printf("%x ",RS485_SCREEN_RX_BUF[i]);
	}
	
}
*/

void RS485_SCREEN_ReadData(uint8_t *data, uint8_t *len){
	uint8_t i;
	*len = RS4856_RX_LEN;
	for(i=0;i<RS4856_RX_LEN;i++){
		data[i] = RS4856_RX_BUF[i];
	}
	RS4856_RX_LEN = 0;
	RS4856_RX_BUF[RS4856_RX_LEN] = 0;
}	


/*
	迪文DGUS屏的串口数据帧的5个数据块：
	| 数据块 |   1   |     2     |   3   |   4   |   5   |
	|  定义  | 帧头  | 数据长度  | 指令  | 数据  |  CRC  |
	|数据长度|   2   |     1     |   1   |   N   |   2   |
*/


/*
功能:向指定存储器地址写入变量数据(暂假设都是2字节数据)
参数:addr:16位存储器地址,data:要显示的数据;
返回值:无
注:在本文件中不再封装向对应的显示器位置写入数据的函数,所有的更改显示数据的功能都由主函数在采集到相应数据后自行实现(配合头文件中的变量地址信息)
*/
void RS485_Screen_WriteMem(uint16_t addr, uint16_t data) {
	uint8_t frame[10];  //数据帧
	uint8_t len = 8;      //数据帧长度
	
	//帧头
	frame[0] = 0x5A;
	frame[1] = 0xA5;
	//数据长度
	frame[2] = len - 3;  //指令+数据的长度
	//指令
	frame[3] = 0x82;  //变量存储器写指令
	//数据
	frame[4] = 0xFF & (addr>>8);
	frame[5] = 0xFF & (addr>>0);
	
	//高字节在前
	frame[6] = 0xFF & (data>>8);
	frame[7] = 0xFF & (data>>0);	
	
	RS485_Screen_SendData(frame,len);
	
}


/*功能:获取指定电池串电压数据的显示屏地址
参数:电池串的编号(范围应为0~CELL_NUM-1,目前CELL_NUM值为240
返回值:对应的显示屏地址
*/
u16 get_celladdr(u16 cellnum){
	u16 addrbase = 0;

	switch(cellnum/24){  //该电池串电压数据所在的采集卡编号
		case 0: addrbase = VOLCARD1_VOL1_ADDR;
			break;
		case 1: addrbase = VOLCARD2_VOL1_ADDR;
			break;	
		case 2: addrbase = VOLCARD3_VOL1_ADDR;
			break;
		case 3: addrbase = VOLCARD4_VOL1_ADDR;
			break;
		case 4: addrbase = VOLCARD5_VOL1_ADDR;
			break;
		case 5: addrbase = VOLCARD6_VOL1_ADDR;
			break;
		case 6: addrbase = VOLCARD7_VOL1_ADDR;
			break;
		case 7: addrbase = VOLCARD8_VOL1_ADDR;
			break;
		case 8: addrbase = VOLCARD9_VOL1_ADDR;
			break;
		case 9: addrbase = VOLCARD10_VOL1_ADDR;
			break;		
		default:
			return VOLCARD1_VOL1_ADDR;
	}
	return addrbase + 2*(cellnum%24);
	
}


/*功能:获取指定温度检测点数据的显示屏地址
参数:温度检测点的编号
返回值:对应的显示屏地址
*/
u16 get_tempaddr(u16 tempnum){
	
	u16 addrbase = 0;

	switch(tempnum/12){  //该电池串电压数据所在的采集卡编号
		case 0: addrbase = VOLCARD1_TEMP1_ADDR;
			break;
		case 1: addrbase = VOLCARD2_TEMP1_ADDR;
			break;	
		case 2: addrbase = VOLCARD3_TEMP1_ADDR;
			break;
		case 3: addrbase = VOLCARD4_TEMP1_ADDR;
			break;
		case 4: addrbase = VOLCARD5_TEMP1_ADDR;
			break;
		case 5: addrbase = VOLCARD6_TEMP1_ADDR;
			break;
		case 6: addrbase = VOLCARD7_TEMP1_ADDR;
			break;
		case 7: addrbase = VOLCARD8_TEMP1_ADDR;
			break;
		case 8: addrbase = VOLCARD9_TEMP1_ADDR;
			break;
		case 9: addrbase = VOLCARD10_TEMP1_ADDR;
			break;		
		default:
			return VOLCARD1_TEMP1_ADDR;
	}
	return addrbase + 2*(tempnum%12);
	
}








































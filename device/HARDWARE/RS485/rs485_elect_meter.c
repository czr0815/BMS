 
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "rs485_elect_meter.h"
#include "usart.h"
#include "bms_state.h"
#include "delay.h"

//4852_电表中断接收数据相关变量定义
uint8_t RS4852_RX_BUF[RS4852_REC_LEN];  //RS485_ElectMeter接收缓冲区
uint8_t RS4852_RX_LEN;  //RS485_ElectMeter接收数据帧总长度
uint16_t RS4852_RX_STA;  //RS485_ElectMeter接收状态标记

uint8_t RS4852_RX_ADDR[6];  //RS485_ElectMeter接收数据帧的通信地址(数组存储中高字节地址在前)
uint8_t RS4852_RX_CTRLC;  //RS485_ElectMeter接收数据帧的控制码
uint8_t RS4852_RX_DATALEN;  //RS485_ElectMeter接收数据帧的数据长度
uint8_t RS4852_RX_DATA[20];  //RS485_ElectMeter接收数据(不同类型数据帧数据格式不同)
uint8_t RS4852_RX_CS;  //RS485_ElectMeter接收数据的校验码


//RS485_Electricity_Meter初始化函数
//配置串口USART2与485转换模块CON引脚PD6
void RS485_ElectMeter_Init(u32 bound) {
	
  //目前电表配置:波特率9600,无奇偶校验位
	
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	
	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2
	
	//USART2端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA2,PA3
	
   //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//无奇偶校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	
	USART_Cmd(USART2, ENABLE);  //使能串口2
	
	//USART_ClearFlag(USART2, USART_FLAG_TC);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断
	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;  //抢占优先级6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	//配置PD6(BCU的电表485通信模块控制引脚)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化
	GPIO_SetBits(GPIOD,GPIO_Pin_6);
	
	RS4852_RX_STA = 0;  //初始化RS4852串口接收状态变量
}


/*
功能:电表模块485串口输出
问题:经串口调试助手测试,当字符串过长时,每次最多只能输出244字节;
*/
void RS485_ElectMeter_SendData(const uint8_t *data, uint8_t len){
	uint8_t i =0;
	RS485_RE2_OFF;
	//此处直接操作寄存器可能会避免一些莫名其妙的错误
	for(i=0; i<len; i++){
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		USART_SendData(USART2,data[i]);
	}
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET) ;
	RS485_RE2_ON;
}


/*
功能：给电表发送帧信息
参数：6字节地址,1字节控制码,1字节数据长度,len字节数据
			6字节地址传入参数为: 低位字节在前, 高位字节在后, 与发送帧地址格式相同
*/
void RS485_ElectMeter_SendFrame(u8 *addr,u8 ctrlcode,u8 len,u8 *data) {
	u8 frame[215];
	u8 i;
	u8 checkcode;
	
	//帧起始符
	frame[0] = 0x68;
	//地址域(6个字节)
	for(i=0;i<6;i++) {
		frame[1+i] = addr[i];
	}
	//帧起始符
	frame[7] = 0x68;
	//控制码
	frame[8] = ctrlcode;
	//数据长度域
	frame[9] = len;
	//数据域
	for(i=0;i<len;i++) {
		frame[10+i] = data[i] + 0x33;
	}
	//校验码(从第一个帧起始符开始到校验码之前的所有各字节的模256的和)
	// (应该是先得到加33H之后的数据帧,然后计算得到校验码)
	checkcode = 0;
	for(i=0;i<10+len;i++) {
		checkcode += frame[i];
	}
	frame[10+len] = checkcode;
	//结束符
	frame[10+len+1] = 0x16;
	
	//发送帧信息(应该使用USART2)
	RS485_RE2_OFF;
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET) ;
	USART_SendData(USART2,0xFE);
	for(i=0; i<len+12; i++){
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		USART_SendData(USART2,frame[i]);
	}
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET) ;
	RS485_RE2_ON;		
	
}

//读电能表通信地址(仅支持点对点通信)
void RS485_ElectMeter_UpdateAddr(void) {
	//地址域:AA...AA, 控制码C=13H, 数据长度L=00H,
	u8 addr[6] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
	u8 data[1];
	u8 i;
	RS485_ElectMeter_SendFrame(addr,0x13,0,data);
	
	//delay_ms(10); //延时
	while(!(RS4852_RX_STA&0x8000)) {
		delay_us(100);
	}
	
	//更新地址信息到全局变量em_addr[6]中(低位在前高位在后)
	for(i=0;i<6;i++) {
		em_addr[i] = RS4852_RX_BUF[5+i];
	}
	
	RS4852_RX_STA = 0;  //清空4852串口接收标识符
}

/*
功能:485串口通信读电能表电流数据
参数:无,通过RS4852_RX_BUF、RS4852_RX_STA、current、currentf等全局变量进行数据共享
返回值:1,保留功能
*/
u8 RS485_ElectMeter_ReadCurrentData(void)
{
	u8 data[4];
	float tmp;
	u8 timecount;
	
	data[0] = CURRENT_4BYTES[0];
	data[1] = CURRENT_4BYTES[1];
	data[2] = CURRENT_4BYTES[2];
	data[3] = CURRENT_4BYTES[3];
	
	RS485_ElectMeter_SendFrame(em_addr,0x11,0x04,data);  //发送电流数据请求帧
	
//	//delay_ms(10); //延时20ms
//	timecount = 0;
//	while((!(RS4852_RX_STA&0x8000))||timecount>200) {
//		delay_us(100);
//		++timecount;
//	}
//	
//	//更新电流信息到全局变量中(下标0开始4个字节是0xFE)
//	current[0] = RS4852_RX_BUF[21] - 0x33;  //整数部分(BCD码最高位)(说明书上说最高位为符号位,这个符号位是1个字节还是1个比特?)
//	current[1] = RS4852_RX_BUF[20] - 0x33;  //整数部分
//	current[2] = RS4852_RX_BUF[19] - 0x33;  //小数部分
//	current[3] = RS4852_RX_BUF[18] - 0x33;  //小数部分(BCD码最低位)
//	
//	//4字节BCD码转换为float类型数据currentf
//	tmp = 1.0;
//	tmp = 0;
//	tmp += (current[3]&0x0F)*0.0001;
//	tmp += ((current[3]&0xF0)>>4)*0.001;
//	tmp += (current[2]&0x0F)*0.01;
//	tmp += ((current[2]&0xF0)>>4)*0.1;
//	tmp += (current[1]&0x0F);
//	tmp += ((current[1]&0xF0)>>4)*10;
//	tmp += (current[0]&0x0F)*100;
//	tmp += ((current[0]&0x70)>>4)*1000;
//	if(current[0]&0x80){  //此处认为接收到的最高字节的最高位为符号位
//		currentf = -tmp;
//	}	
//	else
//		currentf = tmp;
//	
//	RS4852_RX_STA = 0;  //清空4852串口接收标识符
	
	return 1;
}

/*
功能:与电表相匹配的字符串接收函数,将DL/T645-2007协议的帧保存在RS4852_RX_BUF[]数组中
或许可以直接在中断服务函数中,在识别到是电流数据信息后,直接转换并存储该值
*/
void USART2_IRQHandler(void) {
	u8 i,ii,j;
	u8 checkcode = 0;
	u8 framelen = 0;
	u8 Res;  //接收字符
	float tmpcur;
	
	//判断两个起始标识符,记录从设备的6字节地址,将帧信息记录在RS4852_RX_BUF[]中用于后续处理
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //接收数据寄存器非空中断
		Res = USART_ReceiveData(USART2);  //接收到的字节
		i = RS4852_RX_STA & 0x7FFF;
		RS4852_RX_BUF[i++] = Res;
		
		if(Res==0x16)  //接收到了帧结束符标志(但不一定代表帧结束)
		{
			ii = i;
			if((ii>13)&&(ii>(RS4852_RX_BUF[13]+15))) { 
				//已经至少接收了14个字节数据 && 已经接收完数据帧的所有字节,此时的Res只能在结束符的位置	
				
				/*
				framelen = RS4852_RX_BUF[13]+10;
				for(ii=0;ii<framelen;ii++) {
					checkcode = checkcode + RS4852_RX_BUF[4+ii];
				}
				if(checkcode==RS4852_RX_BUF[framelen+4]) {  //验证校验码  
				
				*/
				RS4852_RX_STA = i|0x8000;  //接收完信息帧
					
				//如果是电流数据信息,直接在中断服务程序中转换并存储该值
				if((RS4852_RX_BUF[12]==0x91)){
					
					//打印测试
//					printf("Received string is:");
//					for(j=0;j<i;++j) {
//						printf("%c",RS4852_RX_BUF[j]);
//					}
//					printf("\r\n");
					
					tmpcur = 0;
					tmpcur += ((RS4852_RX_BUF[18]-0x33)&0x0F)*0.0001;
					tmpcur += (((RS4852_RX_BUF[18]-0x33)&0xF0)>>4)*0.001;
					tmpcur += ((RS4852_RX_BUF[19]-0x33)&0x0F)*0.01;
					tmpcur += (((RS4852_RX_BUF[19]-0x33)&0xF0)>>4)*0.1;
					tmpcur += ((RS4852_RX_BUF[20]-0x33)&0x0F);
					tmpcur += (((RS4852_RX_BUF[20]-0x33)&0xF0)>>4)*10;
					tmpcur += ((RS4852_RX_BUF[21]-0x33)&0x0F)*100;
					tmpcur += (((RS4852_RX_BUF[21]-0x33)&0x70)>>4)*1000;
					if((RS4852_RX_BUF[21]-0x33)&0x80){  //此处认为接收到的最高字节的最高位为符号位
						currentf = -tmpcur;
					}
					else {
						currentf = tmpcur;
					}
					
					//printf("Received curr data: %f.\r\n",currentf);
					
					RS4852_RX_STA = 0;
					
				}
			//}
			}
			
		}
		else{
			RS4852_RX_STA = i;  //未接收完数据帧,已经接收了i个字节的数据,范围为RS4852_RX_BUF[0-(i-1)]
		}
	}
}
	
	





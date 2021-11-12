//#include "can.h"
//#include "bms_state.h"
//#include "delay.h"
//#include "usart.h"



////自定义延时函数声明
//void Delay_ms(uint32_t i);

////CAN初始化
////tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
////tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
////tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
////brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
////波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
////mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
////Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
////则波特率为:42M/((6+7+1)*6)=500Kbps
////返回值:0,初始化OK;
////    其他,初始化失败; 

////CAN波特率 = RCC_APB1PeriphClock/CAN_SJW+CAN_BS1_BS1+CAN_BS2/CAN_Prescaler

////***************** CAN2初始化：BMS对外通信 **********************
////CAN2初始化
//u8 CAN2_Mode_Init()  //u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
//{

//  	GPIO_InitTypeDef GPIO_InitStructure; 
//	CAN_InitTypeDef        CAN_InitStructure;
//  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
//#if CAN2_RX1_INT_ENABLE 
//   	NVIC_InitTypeDef  NVIC_InitStructure;
//#endif
//    //使能相关时钟
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PORTG时钟	                   											 

//  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//使能CAN1时钟	
//	
//    //初始化GPIO
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;  //BCU的CAN2使用PB12和PB13
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PB12和PB13
//	
//	//引脚复用映射配置
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2); //GPIOB12复用为CAN2
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2); //GPIOB13复用为CAN2
//	
//  	//CAN单元设置
//   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
//  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
//  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
//  	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
//  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
//  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
//  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;  ////mode;	 //模式设置 
//  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;  //tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
//  	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq;  //tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
//  	CAN_InitStructure.CAN_BS2=CAN_BS2_6tq;  //tbs2; //Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
//  	CAN_InitStructure.CAN_Prescaler=6;  //brp;  //分频系数(Fdiv)为brp+1	
//  	CAN_Init(CAN2, &CAN_InitStructure);   // 初始化CAN2
//    
//	//配置过滤器
// 	CAN_FilterInitStructure.CAN_FilterNumber=1;	  //过滤器1
//  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
//  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
//  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
//  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
//  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK屏蔽(屏蔽为0则表示都不过滤,若某位为1则表示接收的帧ID该位必须和32位ID中的一致)
//  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
//   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO1;//过滤器1关联到FIFO1
//  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器1
//  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
//	
//#if CAN2_RX1_INT_ENABLE
//	
//	CAN_ITConfig(CAN2,CAN_IT_FMP1,ENABLE);//FIFO1消息挂号中断允许.		    
//		
//  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  	NVIC_Init(&NVIC_InitStructure);
//#endif
//	return 0;
//}   

//#if CAN2_RX1_INT_ENABLE	//使能RX0中断
////中断服务函数			    
//void CAN2_RX1_IRQHandler(void)
//{
//	int i=0;
//	CanRxMsg Rx2Message;
//	if(CAN_GetITStatus(CAN2, CAN_IT_FMP0) == SET){
//		CAN_Receive(CAN1, CAN_FIFO0, &Rx2Message);
//		
//		//中断操作(可能和CAN接收函数有重叠)
//		CAN_Receive(CAN1, 0, &Rx2Message);
//		printf("ID: %x\r\n",Rx2Message.ExtId);
//		for(i=0;i<8;i++)
//			printf("rxbuf[%d]:%d\r\n",i,Rx2Message.Data[i]);
//		
//		//测试
//		//GPIO_ResetBits(GPIOF,GPIO_Pin_10);  //LED1亮
//		//delay_ms(50);
//		//GPIO_SetBits(GPIOF,GPIO_Pin_10);  //LED1灭
//		
//		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
//	}

//}
//#endif

////CAN2发送数据
////发送扩展帧 数据帧
////msgid: 帧ID, 低29位有效
////msg: 数据指针, 最大为8个字节
////len: 数据长度, 最大为8
//u8 CAN2_Send_Msg(u32 msgid, u8* msg, u8 len)
//{
//	u8 mbox;
//	u16 i=0;
//	CanTxMsg Tx2Message;  //CAN2发送帧
//	Tx2Message.StdId = 0;	 // 标准标识符
//	Tx2Message.ExtId = msgid;	 // 扩展标识符
//	Tx2Message.IDE = CAN_Id_Extended;		  // 使用扩展标识符
//	Tx2Message.RTR = CAN_RTR_DATA;		  // 消息类型为数据帧,1帧8位
//	Tx2Message.DLC=len;		// 消息数据长度
//	for(i=0;i<len;i++)
//		Tx2Message.Data[i]=msg[i];      
//	mbox = CAN_Transmit(CAN2, &Tx2Message);   
//	i=0;
//	while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束


//	if(i<0XFFF) {
//		return 1;  //i未到0xFFF已经发送完成,发送成功
//	
//		//测试
//		GPIO_SetBits(GPIOD,GPIO_Pin_15);  //LED亮
//		Delay_ms(0xFFFF);
//		GPIO_ResetBits(GPIOD,GPIO_Pin_15);  //LED灭
//		
//	}
//		
//	return 0;  //发送失败
//	
//}


////CAN1口接收数据查询
////buf:数据缓存区;	 
////返回值:0,无数据被收到;
////		 其他,接收的数据长度;
//u8 CAN2_Receive_Msg(u8 *buf)
//{		   		   
// 	u32 i;
//	CanRxMsg Rx2Message;
//    if( CAN_MessagePending(CAN2,CAN_FIFO1)==0)  //没有接收到数据,直接退出 
//		return 0;
//    CAN_Receive(CAN2, CAN_FIFO1, &Rx2Message);//接收到数据,读取数据	
//    for(i=0;i<Rx2Message.DLC;i++) {
//		buf[i]=Rx2Message.Data[i]; 
//	}		
//	return Rx2Message.DLC;  //返回接收数据的长度
//	
//	//LED闪烁以示接收成功
//	
//	
//}











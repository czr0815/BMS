#include "can.h"
#include "bms_state.h"
#include "delay.h"
#include "usart.h"
#include "rs485_screen.h"
#include "FreeRTOS.h"
#include "task.h"
#include "voltbalance.h"

//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//波特率=APB1时钟/((tbs1+tbs2+1(tsjw))*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//APB1的时钟在初始化的时候设置为25MHz,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_2tq,CAN_BS1_7tq,10,CAN_Mode_LoopBack);
//则波特率为:25M/((7+2+1)*10)=250Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 

//BMU上传数据任务及任务句柄
#if IS_BMU_ROLE
void bmu_updatedata_task(void);          //创建BMU上传数据任务
TaskHandle_t vBMUUpdateDataTaskHandler;  //BMU上传数据任务句柄
#endif


//*****************CAN1初始化：BMS内部通信**********************
/*
功能:CAN1初始化
*/
void CAN1_Mode_Init()
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
		CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN1_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
    //使能相关时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能PORTG时钟	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
		
    //初始化GPIO
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;  //BCU的CAN1使用PG0和PG1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化PG0和PG1
		
		//引脚复用映射配置
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource0,GPIO_AF_CAN1); //GPIOG0复用为CAN1
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource1,GPIO_AF_CAN1); //GPIOG1复用为CAN1
	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal; //CAN_Mode_Normal;  ////mode;	 //模式设置 
  	//CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;   //回环模式
		CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;  //tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq;  //tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;  //tbs2; //Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=10;  //brp;  
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 (波特率为25MHz/(1+7+2)/10 = 250kbps)
    
		//配置过滤器
		CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
	
#if CAN1_RX0_INT_ENABLE
		CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
		
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif

}

#if CAN1_RX0_INT_ENABLE	//使能RX0中断
/*
功能:中断服务函数,CAN1接收消息帧,并调用CAN1_RxMeg_Process()函数对消息帧进行处理
参数:无
返回值:无
*/
void CAN1_RX0_IRQHandler(void)
{
	int i=0;
	
	printf("CAN1 Interrupt.\r\n");
	
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET){
		CAN_Receive(CAN1, CAN_FIFO0, &CAN1RxMessage);  //全局接收信息帧
		
		//测试
//		printf("ID: 0x%x: ",CAN1RxMessage.ExtId);
		for(i=0;i<8;i++)
			printf("0x%x ",CAN1RxMessage.Data[i]);
		printf("\r\n");
		
		//CAN1_RxMsg_Process();  //处理接收到的数据帧
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);  //允许接收CAN1信息帧
	}
	
	//测试
//	if(CAN1RxMessage.Data[0] == FT_COMCONTROL_HEARTBEAT)  //如果收到心跳包,闪亮指示灯
//	{
//		GPIO_SetBits(GPIOD,GPIO_Pin_15);  //LED亮
//		delay_ms(10);
//		GPIO_ResetBits(GPIOD,GPIO_Pin_15);  //LED灭
//	}
}
#endif

/*
功能:通过CAN1发送消息帧
参数: msgid:帧ID,低29位有效; msg:数据指针,最大为8个字节; len:数据长度,最大为8
返回值:1为发送成功,0为发送失败
*/
u8 CAN1_Send_Msg(u32 msgid, u8* msg, u8 len)
{
	u8 mbox;
	u16 i=0;
	CanTxMsg Tx1Message;  //CAN1发送帧
	Tx1Message.StdId = 0;	 // 标准标识符
	Tx1Message.ExtId = msgid;	 // 扩展标识符
	Tx1Message.IDE = CAN_Id_Extended;		  // 使用扩展标识符
	Tx1Message.RTR = CAN_RTR_DATA;		  // 消息类型为数据帧,1帧8位
	Tx1Message.DLC=len;		// 消息数据长度
	for(i=0;i<len;i++)
		Tx1Message.Data[i]=msg[i];      
	mbox = CAN_Transmit(CAN1, &Tx1Message);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	
	if(i<0XFFF) {
		return 1;  //i未到0xFFF已经发送完成,发送成功	
	}
	return 0;  //发送失败
}

/* 通信控制帧发送函数
功能:CAN1内部通信过程中,用于发送心跳包|应答包
参数:FT:通信控制帧的类型：FT_COMCONTROL_HEARTBEAT/FT_COMCONTROL_ACK,dst_id:目的ID,广播时为0xFF
返回值:1:发送成功 / 0:发送失败
*/
u8 CAN1_ComControl(u8 dst_id, u8 FT) {
	
	u8 mbox;
	u16 i=0;
	CanTxMsg Tx1Message;  //CAN1发送帧
	
	Tx1Message.StdId = 0;	 // 标准标识符
	Tx1Message.ExtId = (PF_COMCONTROL_P<<26)|(PF_COMCONTROL<<16)|(dst_id<<8)|(My_Id<<0);
	Tx1Message.IDE = CAN_Id_Extended;		  // 使用扩展标识符
	Tx1Message.RTR = CAN_RTR_DATA;		  // 消息类型为数据帧,1帧8位
	Tx1Message.DLC = 8;		// 消息数据长度
	
	Tx1Message.Data[0] = FT; 
	Tx1Message.Data[1] = My_BMS_Role;
	Tx1Message.Data[2] = My_Id;
#if IS_BMU_ROLE	
	Tx1Message.Data[3] = My_Pack_Number;
#endif
	
	//测试
	Tx1Message.Data[4] = 0x01;
	Tx1Message.Data[5] = 0x00;
	Tx1Message.Data[6] = 0x01;
	Tx1Message.Data[7] = 0x00;
	
	mbox = CAN_Transmit(CAN1, &Tx1Message);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	
	if(i<0XFFF) {	
		return 1;  //i未到0xFFF已经发送完成,发送成功	
	}
	return 0;  //发送失败

}

/* 控制命令帧发送函数
功能:CAN1内部通信过程中,用于发送不同类型的控制命令帧(如上传数据命令|电池均衡命令等)
参数:FT:控制命令帧的类型,dst_id:目的ID,广播时为0xFF,cmd1u8 cmd2u8 cmd3u16均为控制命令,缺省时需在发送处填入0xFF;
返回值:1:发送成功 / 0:发送失败
*/
u8 CAN1_ControlCmd(u8 dst_id, u8 FT, u8 cmd1u8, u8 cmd2u8, u16 cmd3u16) {
	
	u8 mbox;
	u16 i=0;
	CanTxMsg Tx1Message;  //CAN1发送帧
	
	Tx1Message.StdId = 0;	 // 标准标识符
	Tx1Message.ExtId = (PF_CONTROLCMD_P<<26)|(PF_CONTROLCMD<<16)|(dst_id<<8)|(My_Id<<0);
	Tx1Message.IDE = CAN_Id_Extended;		  // 使用扩展标识符
	Tx1Message.RTR = CAN_RTR_DATA;		  // 消息类型为数据帧,1帧8位
	Tx1Message.DLC = 8;		// 消息数据长度
	
	Tx1Message.Data[0] = FT; 
	Tx1Message.Data[1] = My_BMS_Role;
	Tx1Message.Data[2] = My_Id;
#if IS_BMU_ROLE	
	Tx1Message.Data[3] = My_Pack_Number;
#endif
	
	Tx1Message.Data[4] = cmd1u8;
	Tx1Message.Data[5] = cmd2u8;
	Tx1Message.Data[6] = 0xFF & (cmd3u16>>8);
	Tx1Message.Data[7] = 0xFF & (cmd3u16>>0);
	
	mbox = CAN_Transmit(CAN1, &Tx1Message);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	
	if(i<0XFFF) {	
		return 1;  //i未到0xFFF已经发送完成,发送成功	
	}
	return 0;  //发送失败

}

/* 发送数据函数
功能:向指定ID通过CAN1发送单体电池电压及SOC数据
参数:dst_id:目前地址,FT:要发送的数据类型,packnum:数据所在BMU的电池组编号,datanum:数据在该BMU中的位置编号,data1u16:传送数据1,data2f:传送数据2
返回值:0:发送失败或FT类型错误,1:发送成功
说明:BMU与BCU之间内部通信传送数据的CAN通信帧格式如下(不同种类型数据有不同的帧格式)
- 电池数据: |数据类型FT(1byte)|packnum(1byte)|datanum(1byte)|voldata(2bytes)|socdata(the last 3bytes)|
- 温度数据: |数据类型FT(1byte)|packnum(1byte)|datanum(1byte)|tempdata(BCD type xxx.xxx)(the last 3bytes)|
- 烟雾数据: |数据类型FT(1byte)|packnum(1byte)|datanum(1byte)|smokedata(2bytes)|
- 电流数据: |数据类型FT(1byte)|电流方向(1byte)|currentdata(BCD type xxx.xxx)(the last 3bytes)|
注:对于没有用到的参数,对应位置传入0
*/
u8 CAN1_Send_Data(u8 dst_id, u8 FT, u8 packnum, u8 datanum, u16 data1u16, float data2f)
{
	u8 mbox;
	u8 tmpu8,tmpbcd;
	u32 tmpu32;
	float tmpdata2f;
	u16 i=0;
	CanTxMsg Tx1Message;  //CAN1发送帧
	
	Tx1Message.StdId = 0;	 // 标准标识符
	Tx1Message.ExtId = (PF_DATA_P<<26)|(PF_DATA<<16)|(dst_id<<8)|(My_Id<<0);
	Tx1Message.IDE = CAN_Id_Extended;		  // 使用扩展标识符
	Tx1Message.RTR = CAN_RTR_DATA;		  // 消息类型为数据帧,1帧8位
	
	Tx1Message.Data[0] = FT;

/*提前计算float到BCD格式的过程
	将float类型数据转换为3个字节的BCU编码格式进行数据传输,表达xxx.xxx%的含义,传入的float数据也是data2f%的格式,data2f范围0~100
	实现流程:先将其变为整数,然后依次:取个位,除以10,循环6次
*/
	if((FT==FT_DATA_CELL)||(FT==FT_DATA_TEMP)||(FT==FT_DATA_SMOKE))
	{
			tmpu32 = (u32)(data2f*1000);
			
			Tx1Message.Data[7] = 0xff&(tmpu32>>0);
			Tx1Message.Data[6] = 0xff&(tmpu32>>8);		
			Tx1Message.Data[5] = 0xff&(tmpu32>>16);		

			if(data2f>0.1){
				printf("0x%02X%02X%02X\r\n",Tx1Message.Data[5],Tx1Message.Data[6],Tx1Message.Data[7]);
			}
		
	}
	
	switch(FT)
	{
#if IS_BMU_ROLE
		case FT_DATA_CELL: {  //电池数据
			Tx1Message.Data[1] = packnum;
			Tx1Message.Data[2] = datanum;
			Tx1Message.Data[3] = 0xFF&(data1u16>>8);
			Tx1Message.Data[4] = 0xFF&(data1u16>>0);
			
			Tx1Message.DLC = 8;		// 消息数据长度
			break;
		}
		case FT_DATA_TEMP: {	//温度数据
			Tx1Message.Data[1] = packnum;
			Tx1Message.Data[2] = datanum;			
			
			Tx1Message.DLC = 8;		// 消息数据长度
			break;
		}		
		case FT_DATA_SMOKE: {	 //烟雾数据
			Tx1Message.Data[1] = packnum;
			Tx1Message.Data[2] = datanum;
			Tx1Message.Data[3] = 0xFF&(data1u16>>8);
			Tx1Message.Data[4] = 0xFF&(data1u16>>0);
			Tx1Message.DLC = 8;		// 消息数据长度
			break;
		}				
#endif
		
		case FT_DATA_CURRENT: {	//电流数据
			//使用3字节的BCD码对数据进行传输,并使用1字节标识电流的方向
			if(data2f>=0){  //此处方向还不确定
				Tx1Message.Data[1] = CURRENT_STATE_CHARGE;
				tmpu32 = (u32)(data2f*1000);
			}
			else {
				Tx1Message.Data[1] = CURRENT_STATE_DISCHARGE;
				tmpu32 = (u32)(-data2f*1000);
			}
			
			Tx1Message.Data[7] = 0xff&(tmpu32>>0);
			Tx1Message.Data[6] = 0xff&(tmpu32>>8);		
			Tx1Message.Data[5] = 0xff&(tmpu32>>16);					
			
			Tx1Message.DLC = 8;		// 消息数据长度
			break;
		}
		default: {
			return 0;
		}
	}
	
	mbox = CAN_Transmit(CAN1, &Tx1Message);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	
	if(i<0XFFF) {
		return 1;  //i未到0xFFF已经发送完成,发送成功	
	}
	return 0;  //发送失败
}


/*
功能:处理当前CAN1接收到的消息帧,针对不同的消息类型进行对应的处理
参数:无(通过全局变量传递数据)
返回值:无
*/
u8 CAN1_RxMsg_Process(void)
{
	u8 i;
	u8 packnum,datanum,datau8;
	u8 dataarray[8]; //用于处理数据
	u8 bmu_i,cellnum,tempnum,smokenum;
	u16 datau16;  //用于处理数据帧
	u32 datau32;  //用于处理数据帧
	float dataf;  //用于处理数据帧
	u8 PF_Type = (0xFF)&(CAN1RxMessage.ExtId>>16);  //接收到的信息帧的帧类型
	u8 DstId = (0xFF)&(CAN1RxMessage.ExtId>>8);   //接收到的信息帧的目的地址
	u8 SrcId = (0xFF)&(CAN1RxMessage.ExtId>>0);   //接收到的信息帧的源地址
	u8 RxMsgData[8];
	for(i=0;i<8;i++){
		RxMsgData[i] = CAN1RxMessage.Data[i];  //接收到的信息帧数据
	}
	if((DstId!=0xFF)&&(DstId!=My_Id)){  //收到的信息帧的目的地址不是0xFF广播地址并且也不是接收方ID
		return 0;                        //【该过滤功能后续应由CAN1内置的过滤器实现,接收效率会更高】
	}
	
	switch(PF_Type) {
		case PF_COMCONTROL:  //帧类型:通信控制帧
		{
			switch(RxMsgData[0]) {				
				case FT_COMCONTROL_HEARTBEAT:  //通信控制帧类型:心跳帧
				{
					//接收到来自该ID的心跳包则将该ID的BMU填到连接列表中
					if((My_BMS_Role==BMS_ROLE_BCU)&&(RxMsgData[1]==BMS_ROLE_BMU)){ //如果自身是BCU且接收到的帧发送方是BMU
						
						//将该BMU填入到列表中
						datau8 = RxMsgData[3]-1;  //电池组编号(ID可能有很大变化,但电池组编号必须是1开始且连续)
						BMU_List_Id[datau8] = RxMsgData[2];
						BMU_List_PackNum[datau8] = RxMsgData[3];
						BMU_List_State[datau8] = 1;
						
						//返回确认帧
						CAN1_ComControl(SrcId,FT_COMCONTROL_ACK);
						
						//测试输出
						printf("HB:0x%x.\r\n",SrcId);				
					}
					break;
				}
				case FT_COMCONTROL_ACK:  //通信控制帧类型:确认帧
				{
					if((My_BMS_Role==BMS_ROLE_BMU)&&(RxMsgData[1]==BMS_ROLE_BCU)){ //如果自身是BMU且接收到的帧发送方是BCU
						Is_Connected = 1;
						My_Superior_Id = SrcId;
#if BMU_PRINT_P
						printf("ACK:0x%x.\r\n",SrcId);	
#endif						
					}
					break;
				}				
			}
			break;
		}
		case PF_DATA:      //帧类型:数据帧
		{
			//提前将3字节BCD数据转换为float形式
			if((RxMsgData[0]==FT_DATA_CELL)||(RxMsgData[0]==FT_DATA_TEMP)||(RxMsgData[0]==FT_DATA_CURRENT)) {
				datau32 = 0x00000000|(RxMsgData[5]<<16)|(RxMsgData[6]<<8)|(RxMsgData[7]<<0);
				//printf("***%d***\r\n",datau32);
				dataf = (float)datau32 * 0.001;
			}
			
			switch(RxMsgData[0]) {					
#if IS_BCU_ROLE
				case FT_DATA_CELL:  //数据帧类型:电池数据
				{
					packnum = RxMsgData[1];
					datanum = RxMsgData[2];
					datau16 = (RxMsgData[3]<<8)|(RxMsgData[4]<<0);										
					cellnum = (packnum-1) * BMU_VOL_NUM + datanum;  //电池数据整体编号
					
					cellvol[cellnum] = datau16;
					soc[cellnum] = dataf;  //float类型
					
//					if(cellnum==96){
//						printf("CAN1 data: %d, Addr: 0x%04X\r\n",datau16,get_celladdr(cellnum));  //打印测试CAN内网是否能够接收到数据
//					}
					printf("V%d:%.3f\r\n",cellnum,((float)datau16)*0.0001);
					//printf("V%d ",cellnum);
					//printf("c%d ",cellnum);
					
					
					/*数据比较过程
					//在显示屏显示该电池串电压(和电池串SOC数据)
					RS485_Screen_WriteMem(get_celladdr(cellnum),datau16);
					//检测是否是最大电压值
					if(datau16>max_cellvol){
						max_cellvol = datau16;
						max_cellvolid = cellnum;
						RS485_Screen_WriteMem(HIGHVOL_CELL_ADDR,max_cellvol);  //更新最大电压值
						RS485_Screen_WriteMem(HIGHVOL_CELL_ID_ADDR,max_cellvolid);  //更新最大电压值对应电池串ID
						max_diff_cellvol = max_cellvol - min_cellvol;
						RS485_Screen_WriteMem(MAX_VOL_DIFF_ADDR,max_diff_cellvol);  //更新最大压差
					}
					//检测是否是最小电压值
					if(datau16<min_cellvol){
						min_cellvol = datau16;
						min_cellvolid = cellnum;
						RS485_Screen_WriteMem(LOWVOL_CELL_ADDR,min_cellvol);
						RS485_Screen_WriteMem(LOWVOL_CELL_ID_ADDR,min_cellvolid);
					}
					*/
					
					//打印测试
					//printf("CAN1 received cell data from chip%d num%d.\r\n",chip,num);	
					break;
				}
				
				case FT_DATA_TEMP:  //数据帧类型:温度数据
				{
					packnum = RxMsgData[1];
					datanum = RxMsgData[2];
					
					tempnum = (packnum-1) * BMU_TEMP_NUM + datanum;    //温度数据整体编号
					
					tempf[tempnum] = dataf;  //生成的float类型数据
					
					printf("T%d:%.2f\r\n",tempnum,dataf);
					//printf("T%d ",tempnum);
					
					//printf("t%d ",tempnum);
					
					//在显示屏显示该温度数据
					//RS485_Screen_WriteMem(get_tempaddr(tempnum),datau16);
					
					
					//打印测试
					//printf("CAN1 received temp data from chip%d num%d.\r\n",chip,num);					
					break;
				}
				
				case FT_DATA_SMOKE:  //数据帧类型:烟雾数据
				{
					packnum = RxMsgData[1];
					datanum = RxMsgData[2];
					datau16 = (RxMsgData[3]<<8)|(RxMsgData[4]<<0);
					smokenum = (packnum-1) * BMU_SMOKE_NUM + datanum;
					
					smoke[smokenum] = datau16;
					smokef[smokenum] = (float)datau16;
					
					//打印测试
					//printf("CAN1 received smoke data from  num%d.\r\n",num);					
					break;
				}
#endif
				case FT_DATA_CURRENT:  //数据帧类型:电流数据
				{
					if(RxMsgData[1]==CURRENT_STATE_DISCHARGE){
						dataf = -dataf;
					}
					currentf = dataf;
					
					break;
				}
			break;
			}
		}
		case PF_CONTROLCMD:  //帧类型:控制命令帧
		{
			switch(RxMsgData[0]) {
				case FT_CONTROLCMD_UPLOADDATA:  //控制命令帧类型:上传数据
				{
					
#if IS_BMU_ROLE					
					CAN1_ControlCmd(My_Superior_Id,FT_CONTROLCMD_ACK,CONTROLCMD_ACK_UPLOAD,0xFF,0xFFFF);  //回复上传命令确认
					
					//printf("Upload data task.\r\n");
					
					//开启BMU的数据上传任务
					bmu_uploaddata = 1;
					
#endif					
					break;
				}
				case FT_CONTROLCMD_ACK:  //控制命令帧类型:控制命令确认
				{
#if IS_BCU_ROLE					
					switch(RxMsgData[4]) {
						case CONTROLCMD_ACK_UPLOAD:  //收到上传数据的确认帧
						{
							is_bmu_acked = 1;  //标识为接收到BMU确认
							
							printf("UploadCmd Ack.\r\n");
							
							break;
						}
						case CONTROLCMD_ACK_UPLOAD_COMPLETE:  //收到上传数据完成的确认帧
						{
							is_bmu_uploading = 0;  //标识为BMU停止上传数据
							
							break;
						}
					}
#endif
					break;
				}
				case FT_CONTROLCMD_CELLBALANCE:  //控制命令帧类型:均衡控制命令
				{				
#if IS_BMU_ROLE
					switch(RxMsgData[4]) {
						case CONTROLCMD_CELLBALANCE_ON:  //直接更新BalanceFlag[BMU_VOL_NUM]向量,具体的开启/关闭均衡操作由其他均衡相关任务根据向量操作
						{
							cellnum = RxMsgData[5]; //开启均衡的单体电池编号
							if(cellnum<BMU_VOL_NUM){
								BalanceEnableFlag = 1;  //均衡开启标志(可能在BMU的均衡控制过程中不需要,而选择直接将向量状态写入)
								balancetargetvol = (RxMsgData[6]<<8)|(RxMsgData[7]<<0);  //均衡目标电压
								BalanceFlag[cellnum] = 1;  //开启标志位
							}
							break;
						}
						case CONTROLCMD_CELLBALANCE_OFF:  //收到关闭均衡命令:如果目前均衡数大于1,则关闭该电池均衡后在均衡任务中减少一个电池;如果目前均衡数等于1,则关闭该电池均衡后关闭均衡任务
						{
							cellnum = RxMsgData[5]; //关闭均衡的单体电池编号
							if(cellnum<BMU_VOL_NUM){
								BalanceFlag[cellnum] = 0;	 //关闭标志位
							}
							break;
						}
					}					
#endif				
				}			
				
			}
			break;
		}
		case PF_ALARMINFO:{

			break;
		}
	}
	
	return 1;

}



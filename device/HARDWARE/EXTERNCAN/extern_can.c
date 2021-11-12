#include "extern_can.h"
#include <stdio.h>
#include "bms_state.h"
#include "BMS.h"
#include "Charger.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tick.h"

//定义系统对外通信角色的全局变量
uint8_t MODE;
uint8_t externcan_flag;
CanRxMsg externcan_msg;

/*
功能:设置对外CAN2口为接收充电模式(即BMS/电动汽车模式)
说明:BMS模式下,系统通过CAN2接收充电机的帧信息从而进入各种CAN2对外通信的各种任务
*/
void BMS_Enable(){
	//设置BMS接收充电过程中的相关初始化参数
	data2601 = CELL_NUM * BATTERY_VOL_BEAR;  //BHM 最高允许充电总电压
	
	data2565 = (1<<16)|(1<<8)|0;   //BRM BMS通信协议版本号
	data2566 = BATTERY_TYPE;       //BRM 电池类型
	data2567 = CELL_NUM * BATTERY_CAP_ST;      //BRM 整体蓄电池额定容量/Ah 读数为500*0.1
	data2568 = CELL_NUM * BATTERY_VOL_ST;     //BRM 整体蓄电池额定总电压/V  读数为7200*0.1
	
	data2816 = BATTERY_VOL_BEAR * 10;     //BCP 单体动力蓄电池最高允许电压 350*0.01
	data2817 = BATTERY_CUR_BEAR;     //BCP 最高允许充电电流 100*0.1
	data2818 = CELL_NUM * BATTERY_ENE_ST;    //BCP 动力蓄电池标称总能量kW*h  5000*0.1
	data2819 = CELL_NUM * BATTERY_VOL_ST;    //BCP 最高允许充电电压 1200*0.1
	data2820 = 50 + BATTERY_TEMP_BEAR;      //BCP 最高允许温度
	data2821 = 0;       //BCP 整车动力及蓄电池荷电状态 0*0.1%      soc数据待定-----------
	data2822 = 31;       //BCP 整车动力蓄电池当前电池电压 0*0.1          --------------
  //data2822 = voltotal[0] * 10;
	
	standard_data3072 = data3072 = CELL_NUM * BATTERY_VOL_ST;    //BCL 电压需求 7200*0.1
	standard_data3073 = BATTERY_CUR_ST;     
  data3073 = base_curr + standard_data3073;//BCL 电流需求 (-400+400+100)*0.1
	data3074 = 0x01;    //BCL 要求恒压模式
  //data3074 = 0x02;    //恒流模式
	
	//data3075 = voltotal[1] * 10;    //BCS 充电电压测量值 1200*0.1
	data3075 = 0;
  //data3076 = base_curr + (u16)currentf * 10;     //BCS 充电电流测量值 100*0.1
  data3076 = 0;
  data3081 = data3082 = 0;  //充电机的电压电流输出值（测量到的电压电流输入值）
	data3077 = (max_cellvol<<4)|(max_cellvolid&0xF);       //BCS 最高单体电压及其组号(0<<4)|1
	data3078 = 0;       //BCS 当前荷电状态SOC（%)  ---------------
	data3079 = 5;       //BCS 预估剩余充电时间   ---------------
	
	data3085 = max_cellvolid;       //BSM 最高单体电压所在编号
	data3086 = base_temp + (u16)max_tempf;      //BSM 最高温度  -50+80
	data3087 = max_tempfid;       //BSM 最高温度检测点
	data3088 = base_temp + (u16)min_tempf;      //BSM 最低温度  -50+78
	data3089 = min_tempfid;       //BSM 最低温度检测点
	data3090 = data3091 = data3092 = data3093 = data3094 = data3095 = 0;       //BSM 正常
	data3096 = 1;       //BSM 允许充电
	
	data3511 = (1<<6)|(1<<4)|(1<<2)|1;    //BST 正常结束
	data3512 = 0;                         //BST 无故障结束
	data3513 = 0;                         //BST 无故障结束
	
	data3601 = 100;     //BSD 中止荷电状态SOC   ------------------
	data3602 = (u16)(min_cellvol*100);     //单体最低电压 320*0.01
	data3603 = (u16)(max_cellvol*100);     //单体最高电压 330*0.1
	data3604 = base_temp + (u16)min_tempf;      //最低温度 -50+70
	data3605 = base_temp + (u16)max_tempf;      //最高温度 -50+70
	
	data2829 = 0xAA;    //BRO BMS已准备好
	
	MODE = 0;
	BHM_Flag = 0; 
	BHM_Tick = 0;
}

/*
功能:设置对外CAN口为对外放电模式
*/
void Charge_Enable(){
	MODE = 1;
	CHM_Flag = 0;
	BMS_Total_Voltage_Flag = BMS_Total_Current_Flag = 0;
	xTaskCreate((TaskFunction_t   )send_CHM_task,          
              (const char*      )"send_CHM_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendCHMTaskHandler); 
}

uint8_t GetExternCanFlag(){
	return externcan_flag;
}

void SetExternCanFlag(uint8_t flag){
	externcan_flag = flag;
}

/*
功能:配置执行国标的对外通信CAN2接口;
说明:
	- 使用CAN2,PB12:RX,PB13:TX;
	- 系统时钟配置为100MHz,APB1时钟配置为25MHz,CAN2波特率配置为250kbps
*/
void Extern_CAN_Init(){
	//CAN2
	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef CAN_InitStructure;
  CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  //使能相关时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PORTB时钟	                   											 
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//使能CAN2时钟	
	
  //初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PB12,PB13复用为CAN2
	
	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2); //GPIOB12复用为CAN2
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2); //GPIOB13复用为CAN2
	
  //CAN单元设置
  CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
	//设置模式
 	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;    //普通模式
	//CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;   //回环模式
	//设置波特率为250kbps
 	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
 	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
 	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  CAN_InitStructure.CAN_Prescaler=10;
  CAN_Init(CAN2, &CAN_InitStructure);   // 初始化CAN2(波特率为25MHz/(1+7+2)/10 = 250kbps) 
  
	//配置过滤器
 	CAN_FilterInitStructure.CAN_FilterNumber=14;	  //过滤器14
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO1;//过滤器14关联到FIFO1
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器14
  CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
	
	//配置CAN2接收中断
	CAN_ITConfig(CAN2,CAN_IT_FMP1,ENABLE);//FIFO1消息挂号中断允许.		    
  NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;     // 主优先级为3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
功能:CAN2中断接收函数
*/
void CAN2_RX1_IRQHandler(void)
{
//	u8 RxMsgData[8];
//	u8 i;
	CanRxMsg RxMessage;
	
//	printf("CAN2 Interrupt.\r\n");
	
	if(CAN_GetITStatus(CAN2, CAN_IT_FMP1) == SET){	
		CAN_Receive(CAN2, CAN_FIFO1, &externcan_msg);
		externcan_flag = 1;
		//printf("收到CAN2数据   param:%x\r\n", externcan_msg.Data);
		//CAN_Receive(CAN2, CAN_FIFO1, &RxMessage);
		//Judge_Msg();
		
		//测试CAN2通信
//		printf("CAN2 received:");
//		for(i=0;i<8;i++){
//			RxMsgData[i] = RxMessage.Data[i];  //接收到的信息帧数据
//			printf("0x%02X ",RxMsgData[i]);
//		}		
//		printf("\r\n");
		
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);  //允许接收CAN2信息帧
	}
}

/*
功能:发送扩展数据帧
*/
uint8_t Extern_CAN_SendMsg(uint32_t stdId, uint32_t extId, uint8_t *buf, uint8_t len){
	uint8_t mbox;
	uint16_t i;
	CanTxMsg TxMessage;
	TxMessage.StdId=stdId;	 // 标准标识符为0
  TxMessage.ExtId=extId;	 // 设置扩展标示符（29位）
	//IDE标识标准帧和扩展帧，显性标准帧，隐性扩展帧
  TxMessage.IDE = CAN_Id_Extended;		  // 使用扩展标识符
	//RTR表示数据帧和远程帧，显性数据帧，隐性远程帧。显性->0,隐性->1
  TxMessage.RTR = CAN_RTR_DATA;		  // 消息类型为数据帧，一帧8位
  TxMessage.DLC=len;
  for(i=0;i<len;i++)
	  TxMessage.Data[i]=buf[i];         
  mbox = CAN_Transmit(CAN2, &TxMessage);
  i = 0;
  while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF)) i++;	//等待发送结束
  if(i>=0XFFF) return 1;
  return 0;		
}


/*
功能:接收扩展数据帧
*/
uint8_t Extern_CAN_ReceiveMsg(uint32_t *stdId, uint32_t *extId, uint8_t *buf){
	u32 i;
	CanRxMsg RxMessage;
  if( CAN_MessagePending(CAN2,CAN_FIFO1)==0)  return 0;		//没有接收到数据,直接退出 
  CAN_Receive(CAN2, CAN_FIFO1, &RxMessage);//读取数据	
	*stdId = RxMessage.StdId;
	*extId = RxMessage.ExtId;
  for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}

/*
功能:对收到的CAN2帧信息进行判断和分析
*/
void Judge_Msg(){
	uint32_t ti = 0;
	
	CanRxMsg RxMsg = externcan_msg;
	uint32_t identifier, pgn;
	uint8_t parm;
	identifier = RxMsg.ExtId;
	printf("收到PGN:%x\r\n", identifier);
	pgn = (identifier>>8) & 0x3FFFF;
	parm = (pgn>>8) & 0xFF;
	//printf("参数： %x\r\n", parm);
	if(parm==TP_Param){
		printf("收到多帧 order-%d cnt-%d num-%d param-%d", RxMsg.Data[0], RxMsg.Data[1], RxMsg.Data[2], RxMsg.Data[6]);
		if(RxMsg.Data[0]==17){
			uint8_t i, cnt, num, param;
			cnt = RxMsg.Data[1];
			num = RxMsg.Data[2];
			param = RxMsg.Data[6];
			if(cnt==0){
				return;
			} else {
				/*
				if(!MODE&&param==BRM_Param){
					for(i=0;i<cnt;i++){
						send_BRM_data(num+i);
						vTaskDelay(10);
						send_BRM_data(num+i);
						vTaskDelay(10);
					}
				}
				*/
				if(!MODE&&param==BCP_Param){
					for(i=0;i<cnt;i++){
						send_BCP_data(num+i);
						vTaskDelay(10);
						send_BCP_data(num+i);
						vTaskDelay(10);
					}
			  }else if(!MODE&&param==BCS_Param){
					for(i=0;i<cnt;i++){
						send_BCS_data(num+i);
						vTaskDelay(10);
						send_BCS_data(num+i);
						vTaskDelay(10);
					}
				  //在发送完最后一个帧之后，应当开始计时，判断CCS的接收是否超时
			  }
			}
		}
	}else if(!MODE&&parm==CHM_Param){
		//收到CHM,充电机握手；发送BHM
		printf("收到CHM\r\n");
		//data2565 = (RxMsg.Data[0]|(RxMsg.Data[1]<<8)|(RxMsg.Data[2]<<16)|0);
		//printf("data2565:%d  %d  %d\r\n", RxMsg.Data[0], RxMsg.Data[1], RxMsg.Data[2]);
		if(runtime-BHM_Tick>5){
    //if(millis()-BHM_Tick>5000){
			BHM_Flag = BRM_Flag = BCP_Flag = BRO_Flag = BCL_Flag = BCS_Flag = Check_CCS_Flag = BSM_Flag = BST_Flag = BSD_Flag = 0;
		}
		if(!BHM_Flag){
			printf("创建BHM任务!!!\r\n");
			//BHM_Tick = millis();
      BHM_Tick = runtime;
			xTaskCreate((TaskFunction_t   )send_BHM_task,          //任务函数
                  (const char*      )"send_BHM_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendBHMTaskHandler);  //任务句柄
		}
	}else if(!MODE&&parm==CRM_Param){
		//收到CRM,充电机识别；发送BRM或者BCP
		printf("收到CRM！！！！！！！！！！！！【%x】\r\n", RxMsg.Data[0]);
		//set_CRM_data(RxMsg.Data[0], RxMsg.Data[1]|(RxMsg.Data[2]<<8)|(RxMsg.Data[3]<<16)|(RxMsg.Data[4]<<24));
		if(BHM_Flag){
			BHM_Flag = 0;
			vTaskDelete(SendBHMTaskHandler);
		}
		if(RxMsg.Data[0]==0x00&&!BRM_Flag){
      if(runtime-BHM_Tick>5){
				printf("等待CRM-0x00超时  %d  %d", runtime, BHM_Tick);
				set_BEM_data(1, 0, 0, 0, 0, 0, 0);
	      xTaskCreate((TaskFunction_t   )send_BEM_task,          
	                  (const char*      )"send_BEM_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&SendBEMTaskHandler); 
			}
			printf("创建BRM任务\r\n");
			//BRM_Tick = millis();
      BRM_Tick = runtime;
			xTaskCreate((TaskFunction_t   )send_BRM_task,          
                  (const char*      )"send_BRM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendBRMTaskHandler);  
		}else if(RxMsg.Data[0]==0xAA){
			printf("结束握手识别阶段，应当发送BCP\r\n");
			if(runtime-BRM_Tick>5){
				printf("等待CRM-0xAA超时  %d  %d", runtime, BRM_Tick);
				set_BEM_data(0, 1, 0, 0, 0, 0, 0);
	      xTaskCreate((TaskFunction_t   )send_BEM_task,          
	                  (const char*      )"send_BEM_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&SendBEMTaskHandler); 
			}
			if(BRM_Flag){
				BRM_Flag = 0;
			  vTaskDelete(SendBRMTaskHandler);
			}
			if(!BCP_Flag){
				printf("创建BCP任务\r\n");
				BCP_Tick = runtime;
				xTaskCreate((TaskFunction_t   )send_BCP_task,          
                    (const char*      )"send_BCP_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&SendBCPTaskHandler); 
			}
		}
	}else if(!MODE&&parm==CTS_Param){
		//收到CTS,充电机时间同步信息；目前不做处理
		printf("收到CTS\r\n");
	}else if(!MODE&&parm==CML_Param){
		//收到CML,充电机最大输出能力，便于预估充电时间;进行检测，做出回应，是否做好充电准备
		printf("收到CML\r\n");
		//set_CML_data(RxMsg.Data[0]|(RxMsg.Data[1]<<8), RxMsg.Data[2]|(RxMsg.Data[3]<<8), RxMsg.Data[4]|(RxMsg.Data[5]<<8), RxMsg.Data[6]|(RxMsg.Data[7]<<8));
		if(BCP_Flag&&runtime-BCP_Tick>5){
			printf("等待CML超时   %d  %d\r\n", runtime, BCP_Tick);
			set_BEM_data(0, 0, 1, 0, 0, 0, 0);
	    xTaskCreate((TaskFunction_t   )send_BEM_task,          
	                (const char*      )"send_BEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendBEMTaskHandler); 
		}
		if(BCP_Flag){
			BCP_Flag = 0;
			vTaskDelete(SendBCPTaskHandler);
		}
		if(!BRO_Flag){
			BRO_Tick = runtime;
			printf("创建BRO任务");
			xTaskCreate((TaskFunction_t   )send_BRO_task,          
                  (const char*      )"send_BRO_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendBROTaskHandler);  
		}
	}else if(!MODE&&parm==CRO_Param){
		//收到CRO，充电机已做好充电准备；开始进入充电阶段
    //应当连接充电继电器
		printf("收到CRO:%x\r\n", RxMsg.Data[0]);
		set_CRO_data(RxMsg.Data[0]);
		if(BRO_Flag&&runtime-BRO_Tick>5){
			set_BEM_data(0, 0, 0, 1, 0, 0, 0);
	    xTaskCreate((TaskFunction_t   )send_BEM_task,          
	                (const char*      )"send_BEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendBEMTaskHandler); 
		}
		if(BRO_Flag){
			BRO_Flag = 0;
			vTaskDelete(SendBROTaskHandler);
		}
		if(data2830==0xAA){
      printf("更新充电过程中参数\r\n");
      if(!BCL_Flag){
        ti = 0;
        data3075 = data3076 = data3078 = 0;
        data3079 = 5;
        data3077 = (320<<4)|(1&0xF);

        data3085 = 1;
        data3086 = base_temp + 26;
        data3087 = 1;
        data3088 = base_temp + 15;
        data3089 = 2;
        data3090 = data3091 = data3092 = data3093 = data3094 = data3095 = 0; 
      }else{
        ti += runtime-Check_CCS_Tick;
        data3075 = data3081;
        data3076 = data3082;
        data3078 = (ti/3)&0xFF;
        data3079 = ((600-ti)/60)&0xFFFF;
      }

      //data3074 = 0x02;  //恒流充电

      //data3075 = voltotal[1] * 10;
      //data3076 = base_curr + (u16)currentf * 10;
      //data3077 = (max_cellvol<<4)|(max_cellvolid&0xF);
      //data3078 = (u16)soctotal;
      //data3079       //估算剩余充电时间

      // data3085 = max_cellvolid;       //BSM 最高单体电压所在编号
	    // data3086 = base_temp + (u16)max_tempf;      //BSM 最高温度  -50+80
	    // data3087 = max_tempfid;       //BSM 最高温度检测点
	    // data3088 = base_temp + (u16)min_tempf;      //BSM 最低温度  -50+78
	    // data3089 = min_tempfid;       //BSM 最低温度检测点
	    // data3096 = 1;       //BSM 允许充电

			Check_CCS_Tick = runtime;
			if(!BCL_Flag){
				printf("创建BCL任务\r\n");
			  xTaskCreate((TaskFunction_t   )send_BCL_task,          
                    (const char*      )"send_BCL_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&SendBCLTaskHandler); 
		  }
		  if(!BCS_Flag){
				printf("创建BCS任务\r\n");
			  xTaskCreate((TaskFunction_t   )send_BCS_task,          
                    (const char*      )"send_BCS_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&SendBCSTaskHandler); 
	    }
			if(!Check_CCS_Flag){
				printf("创建CCS监听任务\r\n");
				xTaskCreate((TaskFunction_t   )check_CCS_task,          
                    (const char*      )"check_CCS_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&CheckCCSTaskHandler); 
			}
			if(!BSM_Flag){
				printf("创建BSM任务\r\n");
				xTaskCreate((TaskFunction_t   )send_BSM_task,          
                    (const char*      )"send_BSM_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&SendBSMTaskHandler); 
			}
		}
	}else if(!MODE&&parm==CCS_Param){
		//确认充电过程中充电机状态
		printf("收到CCS，确认充电机状态\r\n");
		Check_CCS_Tick = runtime;
		set_CCS_data(RxMsg.Data[0]|(RxMsg.Data[1]<<8), RxMsg.Data[2]|(RxMsg.Data[3]<<8), RxMsg.Data[4]|(RxMsg.Data[5]<<8), RxMsg.Data[6]);
		printf("电压输出%d 电流输出%d 累计充电%d\r\n", data3081, data3082, data3083);
		/*
		if(!check_CCS_data(RxMsg.Data)){
			//发生3级错误，结束放电
			set_BEM_data(0, 0, 0, 0, 2, 0, 0);
	    xTaskCreate((TaskFunction_t   )send_BEM_task,          
	                (const char*      )"send_BEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendBEMTaskHandler); 

      
			return;
		}
		*/
	}else if(!MODE&&parm==CST_Param){
		printf("收到CST\r\n");
		set_CST_data(RxMsg.Data[0], RxMsg.Data[1]|(RxMsg.Data[2]<<8), RxMsg.Data[3]);
    printf("收到CST，中止充电：%x %x %x\r\n", data3521, data3522, data3523);
		if(BCL_Flag){
			BCL_Flag = 0;
			vTaskDelete(SendBCLTaskHandler);
		}
		if(BCS_Flag){
			BCS_Flag = 0;
			vTaskDelete(SendBCSTaskHandler);
		}
		if(Check_CCS_Flag){
			Check_CCS_Flag = 0;
			vTaskDelete(CheckCCSTaskHandler);
		}
		if(BSM_Flag){
			BSM_Flag = 0;
			vTaskDelete(SendBSMTaskHandler);
		}
		if(!BST_Flag){
			set_BST_data(1<<6, 0, 0);
	    xTaskCreate((TaskFunction_t   )send_BST_task,          
	                (const char*      )"send_BST_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendBSTTaskHandler); 
		}
		//结束充电，开始结尾统计阶段
		if(!BSD_Flag){
			xTaskCreate((TaskFunction_t   )send_BSD_task,          //任务函数
                  (const char*      )"send_BSD_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendBSDTaskHandler);  //任务句柄
		}
	}else if(!MODE&&parm==CSD_Param){
		printf("收到CSD\r\n");
    //采取操作，断开充电继电器
		set_CSD_data(RxMsg.Data[0]|(RxMsg.Data[1]<<8), RxMsg.Data[2]|(RxMsg.Data[3]<<8), RxMsg.Data[4]|(RxMsg.Data[5]<<8)|(RxMsg.Data[6]<<16)|(RxMsg.Data[7]<<24));
		/*
		if(BST_Flag&&millis()-BST_Tick>5000){
			set_BEM_data(0, 0, 0, 0, 0, 1, 0);
			xTaskCreate((TaskFunction_t   )send_BEM_task,          
	                (const char*      )"send_BEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendBEMTaskHandler); 
		}
		if(BSD_Flag&&millis()-BSD_Tick>5000){
			set_BEM_data(0, 0, 0, 0, 0, 0, 1);
			xTaskCreate((TaskFunction_t   )send_BEM_task,          
	                (const char*      )"send_BEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendBEMTaskHandler); 
		}
		if(BST_Flag){
			BST_Flag = 0;
			vTaskDelete(SendBSTTaskHandler);
		}
		if(BSD_Flag){
			BSD_Flag = 0;
			vTaskDelete(SendBSDTaskHandler);
		}
		//结束充电，确认完毕
		if(!BSD_Flag){
			xTaskCreate((TaskFunction_t   )send_BSD_task,          //任务函数
                  (const char*      )"send_BSD_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendBSDTaskHandler);  //任务句柄
		}
		*/
	}else if(MODE&&parm==CEM_Param){
    printf("收到来自充电机的错误信息！\r\n");
    printf("辨识过程超时:data3921, %x\r\n", RxMsg.Data[0]);
    printf("接收充电参数报文超时:data3922, %x\r\n", RxMsg.Data[1]&0x3);
    printf("接收完成充电准备报文超时:data3923, %x\r\n", (RxMsg.Data[1]>>2)&0x3);
    printf("接收充电总状态报文超时:data3924, %x\r\n", RxMsg.Data[2]&0x3);
    printf("接收充电要求报文超时:data3925, %x\r\n", (RxMsg.Data[2]>>2)&0x3);
    printf("接收中止充电报文超时:data3926, %x\r\n", (RxMsg.Data[2]>>4)&0x3);
    printf("接收充电统计报文超时:data3927, %x\r\n", RxMsg.Data[3]&0x3);
  }else if(MODE&&parm==BHM_Param){
		if(CHM_Flag){
			CHM_Flag =0;
			vTaskDelete(SendCHMTaskHandler);
		}
		//进行初步对接
		if(!CRM_Flag){
			CRM_Tick = millis();
			set_CRM_data(0, 0);
			xTaskCreate((TaskFunction_t   )send_CRM_task,          //任务函数
                  (const char*      )"send_CRM_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendCRMTaskHandler);  //任务句柄
		}
	}else if(MODE&&parm==BRM_Param){
		if(CRM_Flag&&millis()-CRM_Tick>5000){
			set_CEM_data(1, 0, 0, 0, 0, 0, 0);
	    xTaskCreate((TaskFunction_t   )send_CEM_task,          
	                (const char*      )"send_CEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendCEMTaskHandler); 
		}
		if(CRM_Flag){
			CRM_Flag =0;
			vTaskDelete(SendCRMTaskHandler);
		}
		//对BRM传输的数据进行处理
		if(!CRM_Flag){
			CRM_Tick = millis();
			set_CRM_data(0xAA, 0);
			xTaskCreate((TaskFunction_t   )send_CRM_task,          //任务函数
                  (const char*      )"send_CRM_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendCRMTaskHandler);  //任务句柄
		}
	}else if(MODE&&parm==BCP_Param){
		if(CRM_Flag&&millis()-CRM_Tick>5000){
			set_CEM_data(0, 1, 0, 0, 0, 0, 0);
	    xTaskCreate((TaskFunction_t   )send_CEM_task,          
	                (const char*      )"send_CEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendCEMTaskHandler); 
		}
		if(CRM_Flag){
			CRM_Flag = 0;
			vTaskDelete(SendCRMTaskHandler);
		}
		//发送最大充电能力
		if(!CML_Flag){
			CML_Tick = millis();
			xTaskCreate((TaskFunction_t   )send_CML_task,          //任务函数
                  (const char*      )"send_CML_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendCMLTaskHandler);  //任务句柄
		}
	}else if(MODE&&parm==BRO_Param){
		if(CML_Flag&&millis()-CML_Tick>5000){
			set_CEM_data(0, 0, 1, 0, 0, 0, 0);
			xTaskCreate((TaskFunction_t   )send_CEM_task,          
	                (const char*      )"send_CEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendCEMTaskHandler); 
		}
		if(CML_Flag){
			CML_Flag = 0;
			vTaskDelete(SendCMLTaskHandler);
		}
		//确认做好充电准备
		if(!CRO_Flag){
			CRO_Tick = millis();
			xTaskCreate((TaskFunction_t   )send_CRO_task,          //任务函数
                  (const char*      )"send_CRO_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendCROTaskHandler);  //任务句柄
		}
	}else if(MODE&&parm==BCL_Param){
		if(CRO_Flag&&millis()-CRO_Tick>5000){
			set_CEM_data(0, 0, 0, 0, 1, 0, 0);
			xTaskCreate((TaskFunction_t   )send_CEM_task,          
	                (const char*      )"send_CEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendCEMTaskHandler); 
		}
		//分析电池充电需求，充电机针对性地调整电压电流
	}else if(MODE&&parm==BCS_Param){
		if(CRO_Flag&&millis()-CRO_Tick>5000){
			set_CEM_data(0, 0, 0, 1, 0, 0, 0);
			xTaskCreate((TaskFunction_t   )send_CEM_task,          
	                (const char*      )"send_CEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendCEMTaskHandler); 
		}
		if(CCS_Flag&&millis()-CCS_Tick>1000){
			set_CEM_data(0, 0, 0, 1, 0, 0, 0);
			xTaskCreate((TaskFunction_t   )send_CEM_task,          
	                (const char*      )"send_CEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendCEMTaskHandler); 
		}
		if(CRO_Flag){
			CRO_Flag = 0;
			vTaskDelete(SendCROTaskHandler);
		}
		if(CCS_Flag){
			CCS_Flag = 0;
			vTaskDelete(SendCCSTaskHandler);
		}
		//对充电前电充总状态进行检测
		if(!CCS_Flag){
			CCS_Tick = millis();
			xTaskCreate((TaskFunction_t   )send_CCS_task,          //任务函数
                  (const char*      )"send_CCS_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendCCSTaskHandler);  //任务句柄
		}
	}else if(MODE&&parm==BSM_Param){
		//分析充电过程的电池状态，暂时不做处理
	}else if(MODE&&parm==BST_Param){
		if(CCS_Flag){
			CCS_Flag = 0;
			vTaskDelete(SendCCSTaskHandler);
		}
		if(CST_Flag&&millis()-CST_Tick>5000){
			set_CEM_data(0, 0, 0, 0, 0, 1, 0);
			xTaskCreate((TaskFunction_t   )send_CEM_task,          
	                (const char*      )"send_CEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendCEMTaskHandler); 
		}
		//结束充电，开始统计
		if(!CST_Flag){
			CST_Tick = millis();
			xTaskCreate((TaskFunction_t   )send_CST_task,          //任务函数
                  (const char*      )"send_CST_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendCSTTaskHandler);  //任务句柄
		}else if(CST_Flag&&!CSD_Flag){
			CST_Flag = 0;
			vTaskDelete(SendCSTTaskHandler);
			CSD_Tick = millis();
			xTaskCreate((TaskFunction_t   )send_CSD_task,          //任务函数
                  (const char*      )"send_CSD_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendCSDTaskHandler);  //任务句柄
		}
	}else if(MODE&&parm==BSD_Param){
		if(CST_Flag&&millis()-CST_Tick>5000){
			set_CEM_data(0, 0, 0, 0, 0, 1, 0);
			xTaskCreate((TaskFunction_t   )send_CEM_task,          
	                (const char*      )"send_CEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendCEMTaskHandler); 
		}
		if(CSD_Flag&&millis()-CSD_Tick>5000){
			set_CEM_data(0, 0, 0, 0, 0, 0, 1);
			xTaskCreate((TaskFunction_t   )send_CEM_task,          
	                (const char*      )"send_CEM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendCEMTaskHandler); 
		}
		if(CST_Flag){
			CST_Flag = 0;
			vTaskDelete(SendCSTTaskHandler);
		}
		if(CSD_Flag){
			CSD_Flag = 0;
			vTaskDelete(SendCSDTaskHandler);
		}
		//结束充电，确认完毕
		if(!CSD_Flag){
			xTaskCreate((TaskFunction_t   )send_CSD_task,          //任务函数
                  (const char*      )"send_CSD_task",        //任务名称
                  (uint16_t         )128,                 //任务堆栈大小
                  (void*            )NULL,               //传递给任务函数的参数
                  (UBaseType_t      )2,                  //任务优先级
                  (TaskHandle_t*    )&SendCSDTaskHandler);  //任务句柄
		}
	}
}

#include "extern_can.h"
#include <stdio.h>
#include "bms_state.h"
#include "BMS.h"
#include "Charger.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tick.h"

//����ϵͳ����ͨ�Ž�ɫ��ȫ�ֱ���
uint8_t MODE;
uint8_t externcan_flag;
CanRxMsg externcan_msg;

/*
����:���ö���CAN2��Ϊ���ճ��ģʽ(��BMS/�綯����ģʽ)
˵��:BMSģʽ��,ϵͳͨ��CAN2���ճ�����֡��Ϣ�Ӷ��������CAN2����ͨ�ŵĸ�������
*/
void BMS_Enable(){
	//����BMS���ճ������е���س�ʼ������
	data2601 = CELL_NUM * BATTERY_VOL_BEAR;  //BHM ����������ܵ�ѹ
	
	data2565 = (1<<16)|(1<<8)|0;   //BRM BMSͨ��Э��汾��
	data2566 = BATTERY_TYPE;       //BRM �������
	data2567 = CELL_NUM * BATTERY_CAP_ST;      //BRM �������ض����/Ah ����Ϊ500*0.1
	data2568 = CELL_NUM * BATTERY_VOL_ST;     //BRM �������ض�ܵ�ѹ/V  ����Ϊ7200*0.1
	
	data2816 = BATTERY_VOL_BEAR * 10;     //BCP ���嶯��������������ѹ 350*0.01
	data2817 = BATTERY_CUR_BEAR;     //BCP ������������ 100*0.1
	data2818 = CELL_NUM * BATTERY_ENE_ST;    //BCP �������ر��������kW*h  5000*0.1
	data2819 = CELL_NUM * BATTERY_VOL_ST;    //BCP ����������ѹ 1200*0.1
	data2820 = 50 + BATTERY_TEMP_BEAR;      //BCP ��������¶�
	data2821 = 0;       //BCP �������������غɵ�״̬ 0*0.1%      soc���ݴ���-----------
	data2822 = 31;       //BCP �����������ص�ǰ��ص�ѹ 0*0.1          --------------
  //data2822 = voltotal[0] * 10;
	
	standard_data3072 = data3072 = CELL_NUM * BATTERY_VOL_ST;    //BCL ��ѹ���� 7200*0.1
	standard_data3073 = BATTERY_CUR_ST;     
  data3073 = base_curr + standard_data3073;//BCL �������� (-400+400+100)*0.1
	data3074 = 0x01;    //BCL Ҫ���ѹģʽ
  //data3074 = 0x02;    //����ģʽ
	
	//data3075 = voltotal[1] * 10;    //BCS ����ѹ����ֵ 1200*0.1
	data3075 = 0;
  //data3076 = base_curr + (u16)currentf * 10;     //BCS ����������ֵ 100*0.1
  data3076 = 0;
  data3081 = data3082 = 0;  //�����ĵ�ѹ�������ֵ���������ĵ�ѹ��������ֵ��
	data3077 = (max_cellvol<<4)|(max_cellvolid&0xF);       //BCS ��ߵ����ѹ�������(0<<4)|1
	data3078 = 0;       //BCS ��ǰ�ɵ�״̬SOC��%)  ---------------
	data3079 = 5;       //BCS Ԥ��ʣ����ʱ��   ---------------
	
	data3085 = max_cellvolid;       //BSM ��ߵ����ѹ���ڱ��
	data3086 = base_temp + (u16)max_tempf;      //BSM ����¶�  -50+80
	data3087 = max_tempfid;       //BSM ����¶ȼ���
	data3088 = base_temp + (u16)min_tempf;      //BSM ����¶�  -50+78
	data3089 = min_tempfid;       //BSM ����¶ȼ���
	data3090 = data3091 = data3092 = data3093 = data3094 = data3095 = 0;       //BSM ����
	data3096 = 1;       //BSM ������
	
	data3511 = (1<<6)|(1<<4)|(1<<2)|1;    //BST ��������
	data3512 = 0;                         //BST �޹��Ͻ���
	data3513 = 0;                         //BST �޹��Ͻ���
	
	data3601 = 100;     //BSD ��ֹ�ɵ�״̬SOC   ------------------
	data3602 = (u16)(min_cellvol*100);     //������͵�ѹ 320*0.01
	data3603 = (u16)(max_cellvol*100);     //������ߵ�ѹ 330*0.1
	data3604 = base_temp + (u16)min_tempf;      //����¶� -50+70
	data3605 = base_temp + (u16)max_tempf;      //����¶� -50+70
	
	data2829 = 0xAA;    //BRO BMS��׼����
	
	MODE = 0;
	BHM_Flag = 0; 
	BHM_Tick = 0;
}

/*
����:���ö���CAN��Ϊ����ŵ�ģʽ
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
����:����ִ�й���Ķ���ͨ��CAN2�ӿ�;
˵��:
	- ʹ��CAN2,PB12:RX,PB13:TX;
	- ϵͳʱ������Ϊ100MHz,APB1ʱ������Ϊ25MHz,CAN2����������Ϊ250kbps
*/
void Extern_CAN_Init(){
	//CAN2
	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef CAN_InitStructure;
  CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  //ʹ�����ʱ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��PORTBʱ��	                   											 
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//ʹ��CAN2ʱ��	
	
  //��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��PB12,PB13����ΪCAN2
	
	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2); //GPIOB12����ΪCAN2
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2); //GPIOB13����ΪCAN2
	
  //CAN��Ԫ����
  CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  CAN_InitStructure.CAN_ABOM=DISABLE;	//����Զ����߹���	  
  CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
	//����ģʽ
 	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;    //��ͨģʽ
	//CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;   //�ػ�ģʽ
	//���ò�����Ϊ250kbps
 	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
 	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
 	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  CAN_InitStructure.CAN_Prescaler=10;
  CAN_Init(CAN2, &CAN_InitStructure);   // ��ʼ��CAN2(������Ϊ25MHz/(1+7+2)/10 = 250kbps) 
  
	//���ù�����
 	CAN_FilterInitStructure.CAN_FilterNumber=14;	  //������14
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO1;//������14������FIFO1
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������14
  CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	
	//����CAN2�����ж�
	CAN_ITConfig(CAN2,CAN_IT_FMP1,ENABLE);//FIFO1��Ϣ�Һ��ж�����.		    
  NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;     // �����ȼ�Ϊ3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
����:CAN2�жϽ��պ���
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
		//printf("�յ�CAN2����   param:%x\r\n", externcan_msg.Data);
		//CAN_Receive(CAN2, CAN_FIFO1, &RxMessage);
		//Judge_Msg();
		
		//����CAN2ͨ��
//		printf("CAN2 received:");
//		for(i=0;i<8;i++){
//			RxMsgData[i] = RxMessage.Data[i];  //���յ�����Ϣ֡����
//			printf("0x%02X ",RxMsgData[i]);
//		}		
//		printf("\r\n");
		
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);  //�������CAN2��Ϣ֡
	}
}

/*
����:������չ����֡
*/
uint8_t Extern_CAN_SendMsg(uint32_t stdId, uint32_t extId, uint8_t *buf, uint8_t len){
	uint8_t mbox;
	uint16_t i;
	CanTxMsg TxMessage;
	TxMessage.StdId=stdId;	 // ��׼��ʶ��Ϊ0
  TxMessage.ExtId=extId;	 // ������չ��ʾ����29λ��
	//IDE��ʶ��׼֡����չ֡�����Ա�׼֡��������չ֡
  TxMessage.IDE = CAN_Id_Extended;		  // ʹ����չ��ʶ��
	//RTR��ʾ����֡��Զ��֡����������֡������Զ��֡������->0,����->1
  TxMessage.RTR = CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC=len;
  for(i=0;i<len;i++)
	  TxMessage.Data[i]=buf[i];         
  mbox = CAN_Transmit(CAN2, &TxMessage);
  i = 0;
  while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF)) i++;	//�ȴ����ͽ���
  if(i>=0XFFF) return 1;
  return 0;		
}


/*
����:������չ����֡
*/
uint8_t Extern_CAN_ReceiveMsg(uint32_t *stdId, uint32_t *extId, uint8_t *buf){
	u32 i;
	CanRxMsg RxMessage;
  if( CAN_MessagePending(CAN2,CAN_FIFO1)==0)  return 0;		//û�н��յ�����,ֱ���˳� 
  CAN_Receive(CAN2, CAN_FIFO1, &RxMessage);//��ȡ����	
	*stdId = RxMessage.StdId;
	*extId = RxMessage.ExtId;
  for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}

/*
����:���յ���CAN2֡��Ϣ�����жϺͷ���
*/
void Judge_Msg(){
	uint32_t ti = 0;
	
	CanRxMsg RxMsg = externcan_msg;
	uint32_t identifier, pgn;
	uint8_t parm;
	identifier = RxMsg.ExtId;
	printf("�յ�PGN:%x\r\n", identifier);
	pgn = (identifier>>8) & 0x3FFFF;
	parm = (pgn>>8) & 0xFF;
	//printf("������ %x\r\n", parm);
	if(parm==TP_Param){
		printf("�յ���֡ order-%d cnt-%d num-%d param-%d", RxMsg.Data[0], RxMsg.Data[1], RxMsg.Data[2], RxMsg.Data[6]);
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
				  //�ڷ��������һ��֮֡��Ӧ����ʼ��ʱ���ж�CCS�Ľ����Ƿ�ʱ
			  }
			}
		}
	}else if(!MODE&&parm==CHM_Param){
		//�յ�CHM,�������֣�����BHM
		printf("�յ�CHM\r\n");
		//data2565 = (RxMsg.Data[0]|(RxMsg.Data[1]<<8)|(RxMsg.Data[2]<<16)|0);
		//printf("data2565:%d  %d  %d\r\n", RxMsg.Data[0], RxMsg.Data[1], RxMsg.Data[2]);
		if(runtime-BHM_Tick>5){
    //if(millis()-BHM_Tick>5000){
			BHM_Flag = BRM_Flag = BCP_Flag = BRO_Flag = BCL_Flag = BCS_Flag = Check_CCS_Flag = BSM_Flag = BST_Flag = BSD_Flag = 0;
		}
		if(!BHM_Flag){
			printf("����BHM����!!!\r\n");
			//BHM_Tick = millis();
      BHM_Tick = runtime;
			xTaskCreate((TaskFunction_t   )send_BHM_task,          //������
                  (const char*      )"send_BHM_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendBHMTaskHandler);  //������
		}
	}else if(!MODE&&parm==CRM_Param){
		//�յ�CRM,����ʶ�𣻷���BRM����BCP
		printf("�յ�CRM��������������������������%x��\r\n", RxMsg.Data[0]);
		//set_CRM_data(RxMsg.Data[0], RxMsg.Data[1]|(RxMsg.Data[2]<<8)|(RxMsg.Data[3]<<16)|(RxMsg.Data[4]<<24));
		if(BHM_Flag){
			BHM_Flag = 0;
			vTaskDelete(SendBHMTaskHandler);
		}
		if(RxMsg.Data[0]==0x00&&!BRM_Flag){
      if(runtime-BHM_Tick>5){
				printf("�ȴ�CRM-0x00��ʱ  %d  %d", runtime, BHM_Tick);
				set_BEM_data(1, 0, 0, 0, 0, 0, 0);
	      xTaskCreate((TaskFunction_t   )send_BEM_task,          
	                  (const char*      )"send_BEM_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&SendBEMTaskHandler); 
			}
			printf("����BRM����\r\n");
			//BRM_Tick = millis();
      BRM_Tick = runtime;
			xTaskCreate((TaskFunction_t   )send_BRM_task,          
                  (const char*      )"send_BRM_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendBRMTaskHandler);  
		}else if(RxMsg.Data[0]==0xAA){
			printf("��������ʶ��׶Σ�Ӧ������BCP\r\n");
			if(runtime-BRM_Tick>5){
				printf("�ȴ�CRM-0xAA��ʱ  %d  %d", runtime, BRM_Tick);
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
				printf("����BCP����\r\n");
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
		//�յ�CTS,����ʱ��ͬ����Ϣ��Ŀǰ��������
		printf("�յ�CTS\r\n");
	}else if(!MODE&&parm==CML_Param){
		//�յ�CML,��������������������Ԥ�����ʱ��;���м�⣬������Ӧ���Ƿ����ó��׼��
		printf("�յ�CML\r\n");
		//set_CML_data(RxMsg.Data[0]|(RxMsg.Data[1]<<8), RxMsg.Data[2]|(RxMsg.Data[3]<<8), RxMsg.Data[4]|(RxMsg.Data[5]<<8), RxMsg.Data[6]|(RxMsg.Data[7]<<8));
		if(BCP_Flag&&runtime-BCP_Tick>5){
			printf("�ȴ�CML��ʱ   %d  %d\r\n", runtime, BCP_Tick);
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
			printf("����BRO����");
			xTaskCreate((TaskFunction_t   )send_BRO_task,          
                  (const char*      )"send_BRO_task",        
                  (uint16_t         )128,                 
                  (void*            )NULL,               
                  (UBaseType_t      )2,                  
                  (TaskHandle_t*    )&SendBROTaskHandler);  
		}
	}else if(!MODE&&parm==CRO_Param){
		//�յ�CRO�����������ó��׼������ʼ������׶�
    //Ӧ�����ӳ��̵���
		printf("�յ�CRO:%x\r\n", RxMsg.Data[0]);
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
      printf("���³������в���\r\n");
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

      //data3074 = 0x02;  //�������

      //data3075 = voltotal[1] * 10;
      //data3076 = base_curr + (u16)currentf * 10;
      //data3077 = (max_cellvol<<4)|(max_cellvolid&0xF);
      //data3078 = (u16)soctotal;
      //data3079       //����ʣ����ʱ��

      // data3085 = max_cellvolid;       //BSM ��ߵ����ѹ���ڱ��
	    // data3086 = base_temp + (u16)max_tempf;      //BSM ����¶�  -50+80
	    // data3087 = max_tempfid;       //BSM ����¶ȼ���
	    // data3088 = base_temp + (u16)min_tempf;      //BSM ����¶�  -50+78
	    // data3089 = min_tempfid;       //BSM ����¶ȼ���
	    // data3096 = 1;       //BSM ������

			Check_CCS_Tick = runtime;
			if(!BCL_Flag){
				printf("����BCL����\r\n");
			  xTaskCreate((TaskFunction_t   )send_BCL_task,          
                    (const char*      )"send_BCL_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&SendBCLTaskHandler); 
		  }
		  if(!BCS_Flag){
				printf("����BCS����\r\n");
			  xTaskCreate((TaskFunction_t   )send_BCS_task,          
                    (const char*      )"send_BCS_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&SendBCSTaskHandler); 
	    }
			if(!Check_CCS_Flag){
				printf("����CCS��������\r\n");
				xTaskCreate((TaskFunction_t   )check_CCS_task,          
                    (const char*      )"check_CCS_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&CheckCCSTaskHandler); 
			}
			if(!BSM_Flag){
				printf("����BSM����\r\n");
				xTaskCreate((TaskFunction_t   )send_BSM_task,          
                    (const char*      )"send_BSM_task",        
                    (uint16_t         )128,                 
                    (void*            )NULL,               
                    (UBaseType_t      )2,                  
                    (TaskHandle_t*    )&SendBSMTaskHandler); 
			}
		}
	}else if(!MODE&&parm==CCS_Param){
		//ȷ�ϳ������г���״̬
		printf("�յ�CCS��ȷ�ϳ���״̬\r\n");
		Check_CCS_Tick = runtime;
		set_CCS_data(RxMsg.Data[0]|(RxMsg.Data[1]<<8), RxMsg.Data[2]|(RxMsg.Data[3]<<8), RxMsg.Data[4]|(RxMsg.Data[5]<<8), RxMsg.Data[6]);
		printf("��ѹ���%d �������%d �ۼƳ��%d\r\n", data3081, data3082, data3083);
		/*
		if(!check_CCS_data(RxMsg.Data)){
			//����3�����󣬽����ŵ�
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
		printf("�յ�CST\r\n");
		set_CST_data(RxMsg.Data[0], RxMsg.Data[1]|(RxMsg.Data[2]<<8), RxMsg.Data[3]);
    printf("�յ�CST����ֹ��磺%x %x %x\r\n", data3521, data3522, data3523);
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
		//������磬��ʼ��βͳ�ƽ׶�
		if(!BSD_Flag){
			xTaskCreate((TaskFunction_t   )send_BSD_task,          //������
                  (const char*      )"send_BSD_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendBSDTaskHandler);  //������
		}
	}else if(!MODE&&parm==CSD_Param){
		printf("�յ�CSD\r\n");
    //��ȡ�������Ͽ����̵���
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
		//������磬ȷ�����
		if(!BSD_Flag){
			xTaskCreate((TaskFunction_t   )send_BSD_task,          //������
                  (const char*      )"send_BSD_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendBSDTaskHandler);  //������
		}
		*/
	}else if(MODE&&parm==CEM_Param){
    printf("�յ����Գ����Ĵ�����Ϣ��\r\n");
    printf("��ʶ���̳�ʱ:data3921, %x\r\n", RxMsg.Data[0]);
    printf("���ճ��������ĳ�ʱ:data3922, %x\r\n", RxMsg.Data[1]&0x3);
    printf("������ɳ��׼�����ĳ�ʱ:data3923, %x\r\n", (RxMsg.Data[1]>>2)&0x3);
    printf("���ճ����״̬���ĳ�ʱ:data3924, %x\r\n", RxMsg.Data[2]&0x3);
    printf("���ճ��Ҫ���ĳ�ʱ:data3925, %x\r\n", (RxMsg.Data[2]>>2)&0x3);
    printf("������ֹ��籨�ĳ�ʱ:data3926, %x\r\n", (RxMsg.Data[2]>>4)&0x3);
    printf("���ճ��ͳ�Ʊ��ĳ�ʱ:data3927, %x\r\n", RxMsg.Data[3]&0x3);
  }else if(MODE&&parm==BHM_Param){
		if(CHM_Flag){
			CHM_Flag =0;
			vTaskDelete(SendCHMTaskHandler);
		}
		//���г����Խ�
		if(!CRM_Flag){
			CRM_Tick = millis();
			set_CRM_data(0, 0);
			xTaskCreate((TaskFunction_t   )send_CRM_task,          //������
                  (const char*      )"send_CRM_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendCRMTaskHandler);  //������
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
		//��BRM��������ݽ��д���
		if(!CRM_Flag){
			CRM_Tick = millis();
			set_CRM_data(0xAA, 0);
			xTaskCreate((TaskFunction_t   )send_CRM_task,          //������
                  (const char*      )"send_CRM_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendCRMTaskHandler);  //������
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
		//�������������
		if(!CML_Flag){
			CML_Tick = millis();
			xTaskCreate((TaskFunction_t   )send_CML_task,          //������
                  (const char*      )"send_CML_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendCMLTaskHandler);  //������
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
		//ȷ�����ó��׼��
		if(!CRO_Flag){
			CRO_Tick = millis();
			xTaskCreate((TaskFunction_t   )send_CRO_task,          //������
                  (const char*      )"send_CRO_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendCROTaskHandler);  //������
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
		//������س�����󣬳�������Եص�����ѹ����
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
		//�Գ��ǰ�����״̬���м��
		if(!CCS_Flag){
			CCS_Tick = millis();
			xTaskCreate((TaskFunction_t   )send_CCS_task,          //������
                  (const char*      )"send_CCS_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendCCSTaskHandler);  //������
		}
	}else if(MODE&&parm==BSM_Param){
		//���������̵ĵ��״̬����ʱ��������
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
		//������磬��ʼͳ��
		if(!CST_Flag){
			CST_Tick = millis();
			xTaskCreate((TaskFunction_t   )send_CST_task,          //������
                  (const char*      )"send_CST_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendCSTTaskHandler);  //������
		}else if(CST_Flag&&!CSD_Flag){
			CST_Flag = 0;
			vTaskDelete(SendCSTTaskHandler);
			CSD_Tick = millis();
			xTaskCreate((TaskFunction_t   )send_CSD_task,          //������
                  (const char*      )"send_CSD_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendCSDTaskHandler);  //������
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
		//������磬ȷ�����
		if(!CSD_Flag){
			xTaskCreate((TaskFunction_t   )send_CSD_task,          //������
                  (const char*      )"send_CSD_task",        //��������
                  (uint16_t         )128,                 //�����ջ��С
                  (void*            )NULL,               //���ݸ��������Ĳ���
                  (UBaseType_t      )2,                  //�������ȼ�
                  (TaskHandle_t*    )&SendCSDTaskHandler);  //������
		}
	}
}

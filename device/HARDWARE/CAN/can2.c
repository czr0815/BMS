//#include "can.h"
//#include "bms_state.h"
//#include "delay.h"
//#include "usart.h"



////�Զ�����ʱ��������
//void Delay_ms(uint32_t i);

////CAN��ʼ��
////tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
////tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
////tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
////brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
////������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
////mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
////Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
////������Ϊ:42M/((6+7+1)*6)=500Kbps
////����ֵ:0,��ʼ��OK;
////    ����,��ʼ��ʧ��; 

////CAN������ = RCC_APB1PeriphClock/CAN_SJW+CAN_BS1_BS1+CAN_BS2/CAN_Prescaler

////***************** CAN2��ʼ����BMS����ͨ�� **********************
////CAN2��ʼ��
//u8 CAN2_Mode_Init()  //u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
//{

//  	GPIO_InitTypeDef GPIO_InitStructure; 
//	CAN_InitTypeDef        CAN_InitStructure;
//  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
//#if CAN2_RX1_INT_ENABLE 
//   	NVIC_InitTypeDef  NVIC_InitStructure;
//#endif
//    //ʹ�����ʱ��
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��PORTGʱ��	                   											 

//  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//ʹ��CAN1ʱ��	
//	
//    //��ʼ��GPIO
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;  //BCU��CAN2ʹ��PB12��PB13
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��PB12��PB13
//	
//	//���Ÿ���ӳ������
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2); //GPIOB12����ΪCAN2
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2); //GPIOB13����ΪCAN2
//	
//  	//CAN��Ԫ����
//   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
//  	CAN_InitStructure.CAN_ABOM=DISABLE;	//����Զ����߹���	  
//  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
//  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
//  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
//  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
//  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;  ////mode;	 //ģʽ���� 
//  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;  //tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
//  	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq;  //tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
//  	CAN_InitStructure.CAN_BS2=CAN_BS2_6tq;  //tbs2; //Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
//  	CAN_InitStructure.CAN_Prescaler=6;  //brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
//  	CAN_Init(CAN2, &CAN_InitStructure);   // ��ʼ��CAN2
//    
//	//���ù�����
// 	CAN_FilterInitStructure.CAN_FilterNumber=1;	  //������1
//  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
//  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
//  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
//  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
//  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK����(����Ϊ0���ʾ��������,��ĳλΪ1���ʾ���յ�֡ID��λ�����32λID�е�һ��)
//  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
//   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO1;//������1������FIFO1
//  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������1
//  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
//	
//#if CAN2_RX1_INT_ENABLE
//	
//	CAN_ITConfig(CAN2,CAN_IT_FMP1,ENABLE);//FIFO1��Ϣ�Һ��ж�����.		    
//		
//  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  	NVIC_Init(&NVIC_InitStructure);
//#endif
//	return 0;
//}   

//#if CAN2_RX1_INT_ENABLE	//ʹ��RX0�ж�
////�жϷ�����			    
//void CAN2_RX1_IRQHandler(void)
//{
//	int i=0;
//	CanRxMsg Rx2Message;
//	if(CAN_GetITStatus(CAN2, CAN_IT_FMP0) == SET){
//		CAN_Receive(CAN1, CAN_FIFO0, &Rx2Message);
//		
//		//�жϲ���(���ܺ�CAN���պ������ص�)
//		CAN_Receive(CAN1, 0, &Rx2Message);
//		printf("ID: %x\r\n",Rx2Message.ExtId);
//		for(i=0;i<8;i++)
//			printf("rxbuf[%d]:%d\r\n",i,Rx2Message.Data[i]);
//		
//		//����
//		//GPIO_ResetBits(GPIOF,GPIO_Pin_10);  //LED1��
//		//delay_ms(50);
//		//GPIO_SetBits(GPIOF,GPIO_Pin_10);  //LED1��
//		
//		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
//	}

//}
//#endif

////CAN2��������
////������չ֡ ����֡
////msgid: ֡ID, ��29λ��Ч
////msg: ����ָ��, ���Ϊ8���ֽ�
////len: ���ݳ���, ���Ϊ8
//u8 CAN2_Send_Msg(u32 msgid, u8* msg, u8 len)
//{
//	u8 mbox;
//	u16 i=0;
//	CanTxMsg Tx2Message;  //CAN2����֡
//	Tx2Message.StdId = 0;	 // ��׼��ʶ��
//	Tx2Message.ExtId = msgid;	 // ��չ��ʶ��
//	Tx2Message.IDE = CAN_Id_Extended;		  // ʹ����չ��ʶ��
//	Tx2Message.RTR = CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡,1֡8λ
//	Tx2Message.DLC=len;		// ��Ϣ���ݳ���
//	for(i=0;i<len;i++)
//		Tx2Message.Data[i]=msg[i];      
//	mbox = CAN_Transmit(CAN2, &Tx2Message);   
//	i=0;
//	while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���


//	if(i<0XFFF) {
//		return 1;  //iδ��0xFFF�Ѿ��������,���ͳɹ�
//	
//		//����
//		GPIO_SetBits(GPIOD,GPIO_Pin_15);  //LED��
//		Delay_ms(0xFFFF);
//		GPIO_ResetBits(GPIOD,GPIO_Pin_15);  //LED��
//		
//	}
//		
//	return 0;  //����ʧ��
//	
//}


////CAN1�ڽ������ݲ�ѯ
////buf:���ݻ�����;	 
////����ֵ:0,�����ݱ��յ�;
////		 ����,���յ����ݳ���;
//u8 CAN2_Receive_Msg(u8 *buf)
//{		   		   
// 	u32 i;
//	CanRxMsg Rx2Message;
//    if( CAN_MessagePending(CAN2,CAN_FIFO1)==0)  //û�н��յ�����,ֱ���˳� 
//		return 0;
//    CAN_Receive(CAN2, CAN_FIFO1, &Rx2Message);//���յ�����,��ȡ����	
//    for(i=0;i<Rx2Message.DLC;i++) {
//		buf[i]=Rx2Message.Data[i]; 
//	}		
//	return Rx2Message.DLC;  //���ؽ������ݵĳ���
//	
//	//LED��˸��ʾ���ճɹ�
//	
//	
//}











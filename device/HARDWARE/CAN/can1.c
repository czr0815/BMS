#include "can.h"
#include "bms_state.h"
#include "delay.h"
#include "usart.h"
#include "rs485_screen.h"
#include "FreeRTOS.h"
#include "task.h"
#include "voltbalance.h"

//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=APB1ʱ��/((tbs1+tbs2+1(tsjw))*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//APB1��ʱ���ڳ�ʼ����ʱ������Ϊ25MHz,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_2tq,CAN_BS1_7tq,10,CAN_Mode_LoopBack);
//������Ϊ:25M/((7+2+1)*10)=250Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 

//BMU�ϴ���������������
#if IS_BMU_ROLE
void bmu_updatedata_task(void);          //����BMU�ϴ���������
TaskHandle_t vBMUUpdateDataTaskHandler;  //BMU�ϴ�����������
#endif


//*****************CAN1��ʼ����BMS�ڲ�ͨ��**********************
/*
����:CAN1��ʼ��
*/
void CAN1_Mode_Init()
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
		CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN1_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
    //ʹ�����ʱ��
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��PORTGʱ��	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
		
    //��ʼ��GPIO
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;  //BCU��CAN1ʹ��PG0��PG1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��PG0��PG1
		
		//���Ÿ���ӳ������
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource0,GPIO_AF_CAN1); //GPIOG0����ΪCAN1
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource1,GPIO_AF_CAN1); //GPIOG1����ΪCAN1
	  
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal; //CAN_Mode_Normal;  ////mode;	 //ģʽ���� 
  	//CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;   //�ػ�ģʽ
		CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;  //tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq;  //tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;  //tbs2; //Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=10;  //brp;  
  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 (������Ϊ25MHz/(1+7+2)/10 = 250kbps)
    
		//���ù�����
		CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	
#if CAN1_RX0_INT_ENABLE
		CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
		
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif

}

#if CAN1_RX0_INT_ENABLE	//ʹ��RX0�ж�
/*
����:�жϷ�����,CAN1������Ϣ֡,������CAN1_RxMeg_Process()��������Ϣ֡���д���
����:��
����ֵ:��
*/
void CAN1_RX0_IRQHandler(void)
{
	int i=0;
	
	printf("CAN1 Interrupt.\r\n");
	
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET){
		CAN_Receive(CAN1, CAN_FIFO0, &CAN1RxMessage);  //ȫ�ֽ�����Ϣ֡
		
		//����
//		printf("ID: 0x%x: ",CAN1RxMessage.ExtId);
		for(i=0;i<8;i++)
			printf("0x%x ",CAN1RxMessage.Data[i]);
		printf("\r\n");
		
		//CAN1_RxMsg_Process();  //������յ�������֡
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);  //�������CAN1��Ϣ֡
	}
	
	//����
//	if(CAN1RxMessage.Data[0] == FT_COMCONTROL_HEARTBEAT)  //����յ�������,����ָʾ��
//	{
//		GPIO_SetBits(GPIOD,GPIO_Pin_15);  //LED��
//		delay_ms(10);
//		GPIO_ResetBits(GPIOD,GPIO_Pin_15);  //LED��
//	}
}
#endif

/*
����:ͨ��CAN1������Ϣ֡
����: msgid:֡ID,��29λ��Ч; msg:����ָ��,���Ϊ8���ֽ�; len:���ݳ���,���Ϊ8
����ֵ:1Ϊ���ͳɹ�,0Ϊ����ʧ��
*/
u8 CAN1_Send_Msg(u32 msgid, u8* msg, u8 len)
{
	u8 mbox;
	u16 i=0;
	CanTxMsg Tx1Message;  //CAN1����֡
	Tx1Message.StdId = 0;	 // ��׼��ʶ��
	Tx1Message.ExtId = msgid;	 // ��չ��ʶ��
	Tx1Message.IDE = CAN_Id_Extended;		  // ʹ����չ��ʶ��
	Tx1Message.RTR = CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡,1֡8λ
	Tx1Message.DLC=len;		// ��Ϣ���ݳ���
	for(i=0;i<len;i++)
		Tx1Message.Data[i]=msg[i];      
	mbox = CAN_Transmit(CAN1, &Tx1Message);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	
	if(i<0XFFF) {
		return 1;  //iδ��0xFFF�Ѿ��������,���ͳɹ�	
	}
	return 0;  //����ʧ��
}

/* ͨ�ſ���֡���ͺ���
����:CAN1�ڲ�ͨ�Ź�����,���ڷ���������|Ӧ���
����:FT:ͨ�ſ���֡�����ͣ�FT_COMCONTROL_HEARTBEAT/FT_COMCONTROL_ACK,dst_id:Ŀ��ID,�㲥ʱΪ0xFF
����ֵ:1:���ͳɹ� / 0:����ʧ��
*/
u8 CAN1_ComControl(u8 dst_id, u8 FT) {
	
	u8 mbox;
	u16 i=0;
	CanTxMsg Tx1Message;  //CAN1����֡
	
	Tx1Message.StdId = 0;	 // ��׼��ʶ��
	Tx1Message.ExtId = (PF_COMCONTROL_P<<26)|(PF_COMCONTROL<<16)|(dst_id<<8)|(My_Id<<0);
	Tx1Message.IDE = CAN_Id_Extended;		  // ʹ����չ��ʶ��
	Tx1Message.RTR = CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡,1֡8λ
	Tx1Message.DLC = 8;		// ��Ϣ���ݳ���
	
	Tx1Message.Data[0] = FT; 
	Tx1Message.Data[1] = My_BMS_Role;
	Tx1Message.Data[2] = My_Id;
#if IS_BMU_ROLE	
	Tx1Message.Data[3] = My_Pack_Number;
#endif
	
	//����
	Tx1Message.Data[4] = 0x01;
	Tx1Message.Data[5] = 0x00;
	Tx1Message.Data[6] = 0x01;
	Tx1Message.Data[7] = 0x00;
	
	mbox = CAN_Transmit(CAN1, &Tx1Message);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	
	if(i<0XFFF) {	
		return 1;  //iδ��0xFFF�Ѿ��������,���ͳɹ�	
	}
	return 0;  //����ʧ��

}

/* ��������֡���ͺ���
����:CAN1�ڲ�ͨ�Ź�����,���ڷ��Ͳ�ͬ���͵Ŀ�������֡(���ϴ���������|��ؾ��������)
����:FT:��������֡������,dst_id:Ŀ��ID,�㲥ʱΪ0xFF,cmd1u8 cmd2u8 cmd3u16��Ϊ��������,ȱʡʱ���ڷ��ʹ�����0xFF;
����ֵ:1:���ͳɹ� / 0:����ʧ��
*/
u8 CAN1_ControlCmd(u8 dst_id, u8 FT, u8 cmd1u8, u8 cmd2u8, u16 cmd3u16) {
	
	u8 mbox;
	u16 i=0;
	CanTxMsg Tx1Message;  //CAN1����֡
	
	Tx1Message.StdId = 0;	 // ��׼��ʶ��
	Tx1Message.ExtId = (PF_CONTROLCMD_P<<26)|(PF_CONTROLCMD<<16)|(dst_id<<8)|(My_Id<<0);
	Tx1Message.IDE = CAN_Id_Extended;		  // ʹ����չ��ʶ��
	Tx1Message.RTR = CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡,1֡8λ
	Tx1Message.DLC = 8;		// ��Ϣ���ݳ���
	
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
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	
	if(i<0XFFF) {	
		return 1;  //iδ��0xFFF�Ѿ��������,���ͳɹ�	
	}
	return 0;  //����ʧ��

}

/* �������ݺ���
����:��ָ��IDͨ��CAN1���͵����ص�ѹ��SOC����
����:dst_id:Ŀǰ��ַ,FT:Ҫ���͵���������,packnum:��������BMU�ĵ������,datanum:�����ڸ�BMU�е�λ�ñ��,data1u16:��������1,data2f:��������2
����ֵ:0:����ʧ�ܻ�FT���ʹ���,1:���ͳɹ�
˵��:BMU��BCU֮���ڲ�ͨ�Ŵ������ݵ�CANͨ��֡��ʽ����(��ͬ�����������в�ͬ��֡��ʽ)
- �������: |��������FT(1byte)|packnum(1byte)|datanum(1byte)|voldata(2bytes)|socdata(the last 3bytes)|
- �¶�����: |��������FT(1byte)|packnum(1byte)|datanum(1byte)|tempdata(BCD type xxx.xxx)(the last 3bytes)|
- ��������: |��������FT(1byte)|packnum(1byte)|datanum(1byte)|smokedata(2bytes)|
- ��������: |��������FT(1byte)|��������(1byte)|currentdata(BCD type xxx.xxx)(the last 3bytes)|
ע:����û���õ��Ĳ���,��Ӧλ�ô���0
*/
u8 CAN1_Send_Data(u8 dst_id, u8 FT, u8 packnum, u8 datanum, u16 data1u16, float data2f)
{
	u8 mbox;
	u8 tmpu8,tmpbcd;
	u32 tmpu32;
	float tmpdata2f;
	u16 i=0;
	CanTxMsg Tx1Message;  //CAN1����֡
	
	Tx1Message.StdId = 0;	 // ��׼��ʶ��
	Tx1Message.ExtId = (PF_DATA_P<<26)|(PF_DATA<<16)|(dst_id<<8)|(My_Id<<0);
	Tx1Message.IDE = CAN_Id_Extended;		  // ʹ����չ��ʶ��
	Tx1Message.RTR = CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡,1֡8λ
	
	Tx1Message.Data[0] = FT;

/*��ǰ����float��BCD��ʽ�Ĺ���
	��float��������ת��Ϊ3���ֽڵ�BCU�����ʽ�������ݴ���,���xxx.xxx%�ĺ���,�����float����Ҳ��data2f%�ĸ�ʽ,data2f��Χ0~100
	ʵ������:�Ƚ����Ϊ����,Ȼ������:ȡ��λ,����10,ѭ��6��
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
		case FT_DATA_CELL: {  //�������
			Tx1Message.Data[1] = packnum;
			Tx1Message.Data[2] = datanum;
			Tx1Message.Data[3] = 0xFF&(data1u16>>8);
			Tx1Message.Data[4] = 0xFF&(data1u16>>0);
			
			Tx1Message.DLC = 8;		// ��Ϣ���ݳ���
			break;
		}
		case FT_DATA_TEMP: {	//�¶�����
			Tx1Message.Data[1] = packnum;
			Tx1Message.Data[2] = datanum;			
			
			Tx1Message.DLC = 8;		// ��Ϣ���ݳ���
			break;
		}		
		case FT_DATA_SMOKE: {	 //��������
			Tx1Message.Data[1] = packnum;
			Tx1Message.Data[2] = datanum;
			Tx1Message.Data[3] = 0xFF&(data1u16>>8);
			Tx1Message.Data[4] = 0xFF&(data1u16>>0);
			Tx1Message.DLC = 8;		// ��Ϣ���ݳ���
			break;
		}				
#endif
		
		case FT_DATA_CURRENT: {	//��������
			//ʹ��3�ֽڵ�BCD������ݽ��д���,��ʹ��1�ֽڱ�ʶ�����ķ���
			if(data2f>=0){  //�˴����򻹲�ȷ��
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
			
			Tx1Message.DLC = 8;		// ��Ϣ���ݳ���
			break;
		}
		default: {
			return 0;
		}
	}
	
	mbox = CAN_Transmit(CAN1, &Tx1Message);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	
	if(i<0XFFF) {
		return 1;  //iδ��0xFFF�Ѿ��������,���ͳɹ�	
	}
	return 0;  //����ʧ��
}


/*
����:����ǰCAN1���յ�����Ϣ֡,��Բ�ͬ����Ϣ���ͽ��ж�Ӧ�Ĵ���
����:��(ͨ��ȫ�ֱ�����������)
����ֵ:��
*/
u8 CAN1_RxMsg_Process(void)
{
	u8 i;
	u8 packnum,datanum,datau8;
	u8 dataarray[8]; //���ڴ�������
	u8 bmu_i,cellnum,tempnum,smokenum;
	u16 datau16;  //���ڴ�������֡
	u32 datau32;  //���ڴ�������֡
	float dataf;  //���ڴ�������֡
	u8 PF_Type = (0xFF)&(CAN1RxMessage.ExtId>>16);  //���յ�����Ϣ֡��֡����
	u8 DstId = (0xFF)&(CAN1RxMessage.ExtId>>8);   //���յ�����Ϣ֡��Ŀ�ĵ�ַ
	u8 SrcId = (0xFF)&(CAN1RxMessage.ExtId>>0);   //���յ�����Ϣ֡��Դ��ַ
	u8 RxMsgData[8];
	for(i=0;i<8;i++){
		RxMsgData[i] = CAN1RxMessage.Data[i];  //���յ�����Ϣ֡����
	}
	if((DstId!=0xFF)&&(DstId!=My_Id)){  //�յ�����Ϣ֡��Ŀ�ĵ�ַ����0xFF�㲥��ַ����Ҳ���ǽ��շ�ID
		return 0;                        //���ù��˹��ܺ���Ӧ��CAN1���õĹ�����ʵ��,����Ч�ʻ���ߡ�
	}
	
	switch(PF_Type) {
		case PF_COMCONTROL:  //֡����:ͨ�ſ���֡
		{
			switch(RxMsgData[0]) {				
				case FT_COMCONTROL_HEARTBEAT:  //ͨ�ſ���֡����:����֡
				{
					//���յ����Ը�ID���������򽫸�ID��BMU������б���
					if((My_BMS_Role==BMS_ROLE_BCU)&&(RxMsgData[1]==BMS_ROLE_BMU)){ //���������BCU�ҽ��յ���֡���ͷ���BMU
						
						//����BMU���뵽�б���
						datau8 = RxMsgData[3]-1;  //�������(ID�����кܴ�仯,��������ű�����1��ʼ������)
						BMU_List_Id[datau8] = RxMsgData[2];
						BMU_List_PackNum[datau8] = RxMsgData[3];
						BMU_List_State[datau8] = 1;
						
						//����ȷ��֡
						CAN1_ComControl(SrcId,FT_COMCONTROL_ACK);
						
						//�������
						printf("HB:0x%x.\r\n",SrcId);				
					}
					break;
				}
				case FT_COMCONTROL_ACK:  //ͨ�ſ���֡����:ȷ��֡
				{
					if((My_BMS_Role==BMS_ROLE_BMU)&&(RxMsgData[1]==BMS_ROLE_BCU)){ //���������BMU�ҽ��յ���֡���ͷ���BCU
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
		case PF_DATA:      //֡����:����֡
		{
			//��ǰ��3�ֽ�BCD����ת��Ϊfloat��ʽ
			if((RxMsgData[0]==FT_DATA_CELL)||(RxMsgData[0]==FT_DATA_TEMP)||(RxMsgData[0]==FT_DATA_CURRENT)) {
				datau32 = 0x00000000|(RxMsgData[5]<<16)|(RxMsgData[6]<<8)|(RxMsgData[7]<<0);
				//printf("***%d***\r\n",datau32);
				dataf = (float)datau32 * 0.001;
			}
			
			switch(RxMsgData[0]) {					
#if IS_BCU_ROLE
				case FT_DATA_CELL:  //����֡����:�������
				{
					packnum = RxMsgData[1];
					datanum = RxMsgData[2];
					datau16 = (RxMsgData[3]<<8)|(RxMsgData[4]<<0);										
					cellnum = (packnum-1) * BMU_VOL_NUM + datanum;  //�������������
					
					cellvol[cellnum] = datau16;
					soc[cellnum] = dataf;  //float����
					
//					if(cellnum==96){
//						printf("CAN1 data: %d, Addr: 0x%04X\r\n",datau16,get_celladdr(cellnum));  //��ӡ����CAN�����Ƿ��ܹ����յ�����
//					}
					printf("V%d:%.3f\r\n",cellnum,((float)datau16)*0.0001);
					//printf("V%d ",cellnum);
					//printf("c%d ",cellnum);
					
					
					/*���ݱȽϹ���
					//����ʾ����ʾ�õ�ش���ѹ(�͵�ش�SOC����)
					RS485_Screen_WriteMem(get_celladdr(cellnum),datau16);
					//����Ƿ�������ѹֵ
					if(datau16>max_cellvol){
						max_cellvol = datau16;
						max_cellvolid = cellnum;
						RS485_Screen_WriteMem(HIGHVOL_CELL_ADDR,max_cellvol);  //��������ѹֵ
						RS485_Screen_WriteMem(HIGHVOL_CELL_ID_ADDR,max_cellvolid);  //��������ѹֵ��Ӧ��ش�ID
						max_diff_cellvol = max_cellvol - min_cellvol;
						RS485_Screen_WriteMem(MAX_VOL_DIFF_ADDR,max_diff_cellvol);  //�������ѹ��
					}
					//����Ƿ�����С��ѹֵ
					if(datau16<min_cellvol){
						min_cellvol = datau16;
						min_cellvolid = cellnum;
						RS485_Screen_WriteMem(LOWVOL_CELL_ADDR,min_cellvol);
						RS485_Screen_WriteMem(LOWVOL_CELL_ID_ADDR,min_cellvolid);
					}
					*/
					
					//��ӡ����
					//printf("CAN1 received cell data from chip%d num%d.\r\n",chip,num);	
					break;
				}
				
				case FT_DATA_TEMP:  //����֡����:�¶�����
				{
					packnum = RxMsgData[1];
					datanum = RxMsgData[2];
					
					tempnum = (packnum-1) * BMU_TEMP_NUM + datanum;    //�¶�����������
					
					tempf[tempnum] = dataf;  //���ɵ�float��������
					
					printf("T%d:%.2f\r\n",tempnum,dataf);
					//printf("T%d ",tempnum);
					
					//printf("t%d ",tempnum);
					
					//����ʾ����ʾ���¶�����
					//RS485_Screen_WriteMem(get_tempaddr(tempnum),datau16);
					
					
					//��ӡ����
					//printf("CAN1 received temp data from chip%d num%d.\r\n",chip,num);					
					break;
				}
				
				case FT_DATA_SMOKE:  //����֡����:��������
				{
					packnum = RxMsgData[1];
					datanum = RxMsgData[2];
					datau16 = (RxMsgData[3]<<8)|(RxMsgData[4]<<0);
					smokenum = (packnum-1) * BMU_SMOKE_NUM + datanum;
					
					smoke[smokenum] = datau16;
					smokef[smokenum] = (float)datau16;
					
					//��ӡ����
					//printf("CAN1 received smoke data from  num%d.\r\n",num);					
					break;
				}
#endif
				case FT_DATA_CURRENT:  //����֡����:��������
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
		case PF_CONTROLCMD:  //֡����:��������֡
		{
			switch(RxMsgData[0]) {
				case FT_CONTROLCMD_UPLOADDATA:  //��������֡����:�ϴ�����
				{
					
#if IS_BMU_ROLE					
					CAN1_ControlCmd(My_Superior_Id,FT_CONTROLCMD_ACK,CONTROLCMD_ACK_UPLOAD,0xFF,0xFFFF);  //�ظ��ϴ�����ȷ��
					
					//printf("Upload data task.\r\n");
					
					//����BMU�������ϴ�����
					bmu_uploaddata = 1;
					
#endif					
					break;
				}
				case FT_CONTROLCMD_ACK:  //��������֡����:��������ȷ��
				{
#if IS_BCU_ROLE					
					switch(RxMsgData[4]) {
						case CONTROLCMD_ACK_UPLOAD:  //�յ��ϴ����ݵ�ȷ��֡
						{
							is_bmu_acked = 1;  //��ʶΪ���յ�BMUȷ��
							
							printf("UploadCmd Ack.\r\n");
							
							break;
						}
						case CONTROLCMD_ACK_UPLOAD_COMPLETE:  //�յ��ϴ�������ɵ�ȷ��֡
						{
							is_bmu_uploading = 0;  //��ʶΪBMUֹͣ�ϴ�����
							
							break;
						}
					}
#endif
					break;
				}
				case FT_CONTROLCMD_CELLBALANCE:  //��������֡����:�����������
				{				
#if IS_BMU_ROLE
					switch(RxMsgData[4]) {
						case CONTROLCMD_CELLBALANCE_ON:  //ֱ�Ӹ���BalanceFlag[BMU_VOL_NUM]����,����Ŀ���/�رվ����������������������������������
						{
							cellnum = RxMsgData[5]; //��������ĵ����ر��
							if(cellnum<BMU_VOL_NUM){
								BalanceEnableFlag = 1;  //���⿪����־(������BMU�ľ�����ƹ����в���Ҫ,��ѡ��ֱ�ӽ�����״̬д��)
								balancetargetvol = (RxMsgData[6]<<8)|(RxMsgData[7]<<0);  //����Ŀ���ѹ
								BalanceFlag[cellnum] = 1;  //������־λ
							}
							break;
						}
						case CONTROLCMD_CELLBALANCE_OFF:  //�յ��رվ�������:���Ŀǰ����������1,��رոõ�ؾ�����ھ��������м���һ�����;���Ŀǰ����������1,��رոõ�ؾ����رվ�������
						{
							cellnum = RxMsgData[5]; //�رվ���ĵ����ر��
							if(cellnum<BMU_VOL_NUM){
								BalanceFlag[cellnum] = 0;	 //�رձ�־λ
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



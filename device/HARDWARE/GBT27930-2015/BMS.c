#include "BMS.h"
#include "Charger.h"
#include "extern_can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "tick.h"

/*
  BHM
	data2601,����������ܵ�ѹ
*/
uint16_t data2601;
/*
  BRM
	data2565,BMSͨ��Э��汾��
	data2566,�������
	data2567,������������ϵͳ�����
	data2568,�����������ض�ܵ�ѹ
*/
uint32_t data2565;
uint8_t data2566;
uint16_t data2567, data2568;
/*
  BCP
	data2816,���嶯��������������ѹ
	data2817,������������
	data2818,�������ر��������
	data2819,����������ѹ
	data2820,��������¶�
	data2821,�������������غɵ�״̬
	data2822,�����������ص�ǰ��ص�ѹ
*/
uint16_t data2816, data2817, data2818, data2819, data2821, data2822;
uint8_t data2820;
/*
  BRO
	data2829,BMS�Ƿ�׼����
*/
uint8_t data2829;
/*
  BCL
	data3072,��ѹ����
	data3073,��������
	data3074,���ģʽ
*/
uint16_t data3072, data3073;
uint8_t data3074;
/*
  BCS
	data3075,����ѹ����ֵ
	data3076,����������ֵ
	data3077,��ߵ��嶯�����ص�ѹ�������
	data3078,��ǰ�ɵ�״̬SOC(%)
	data3079,����ʣ����ʱ��(min_
*/
uint16_t data3075, data3076, data3077, data3079;
uint8_t data3078;
/*
  BSM
	data3085,��ߵ��嶯�����ص�ѹ���ڱ��
	data3086,��߶��������¶�
	data3087,����¶ȼ�����
	data3088,��Ͷ��������¶�
	data3089,��Ͷ��������¶ȼ�����
	data3090,���嶯�����ص�ѹ����/����
	data3091,�����������غɵ�״̬����/����
	data3092,�������س�������
	data3093,���������¶ȹ���
	data3094,�������ؾ�Ե״̬
	data3095,�����������������������״̬
	data3096,�������
*/
uint8_t data3085, data3086, data3087, data3088, data3089, data3090, data3091, data3092, data3093, data3094, data3095, data3096;
/*
  BST
	data3511,BMS��ֹ���ԭ��
	data3512,BMS��ֹ������ԭ��
	data3513,BMS��ֹ������ԭ��
*/
uint8_t data3511, data3513;
uint16_t data3512;
/*
  BSD
	data3601,��ֹ�ɵ�״̬SOC(%)
	data3602,�������ص�����͵�ѹ
	data3603,�������ص�����ߵ�ѹ
	data3604,������������¶�
	data3605,������������¶�
*/
uint8_t data3601, data3604, data3605;
uint16_t data3602, data3603;
/*
  BEM
	data3921,����BMS�ͳ����ı��ĳ�ʱ
	data3922,���յ�س��������ĳ�ʱ
	data3923,����BMS��ɳ��׼�����ĳ�ʱ
	data3924,���յ�س����װ���ᱨ�ĳ�ʱ
	data3925,���յ��Ҫ���ĳ�ʱ
	data3926,����BMS��ֹ��籨�ĳ�ʱ
	data3927,����BMS���ͳ�Ʊ��ĳ�ʱ
*/
uint8_t data3901, data3902, data3903, data3904, data3905, data3906, data3907;

uint16_t standard_data3072, standard_data3073;

u8 BMS_Total_Voltage_Flag, BMS_Total_Current_Flag;

//����ʱ��
uint32_t BHM_Tick, BRM_Tick, BCP_Tick, BRO_Tick, BCL_Tick, BCS_Tick, Check_CCS_Tick, BSM_Tick, BST_Tick, BSD_Tick;

//Flag
uint8_t BHM_Flag, BRM_Flag, BCP_Flag, BRO_Flag, BCL_Flag, BCS_Flag, Check_CCS_Flag, BSM_Flag, BST_Flag, BSD_Flag;

TaskHandle_t SendBHMTaskHandler;
TaskHandle_t SendBRMTaskHandler;
TaskHandle_t SendBCPTaskHandler;
TaskHandle_t SendBROTaskHandler;
TaskHandle_t SendBCLTaskHandler;
TaskHandle_t SendBCSTaskHandler;
TaskHandle_t CheckCCSTaskHandler;
TaskHandle_t SendBSMTaskHandler;
TaskHandle_t SendBSTTaskHandler;
TaskHandle_t SendBSDTaskHandler;
TaskHandle_t SendBEMTaskHandler;

/*
  ���ݲ������ź�Ŀ���ַ������չ֡��29λ��ʶ��
*/
void Get_Identifier(uint32_t *extId, uint8_t pri, uint8_t parm, uint8_t des, uint8_t dep){
	uint32_t pgn = 0;
	pgn |= dep;
	pgn |= (des<<8);
	pgn |= (parm<<16);
	pgn |= (pri<<26);
	printf("����PGN:%x\r\n", pgn);
	//*stdId = (pgn>>11) & 0x3FFFF;
	//*extId = pgn & 0x7FF;
	//*stdId = (pgn>>18) & 0x7FF;
	*extId = pgn & 0xFFFFFFFF;
}

void send_BHM_task(){
  uint16_t interval = 0;
	BHM_Flag = 1;
	BRM_Flag = 0;
	while(interval<=5000){
		uint32_t extId;
		uint8_t priority = 6,buf[2],len=2;
		buf[0]=data2601&0xFF;  buf[1]=(data2601>>8)&0xFF;
    printf("����BHM\r\n");
		Get_Identifier(&extId, priority, BHM_Param, Charger_Address, BMS_Address);
		Extern_CAN_SendMsg(0, extId, buf, len);
		vTaskDelay(250);
	}
  BHM_Flag = 0;
	set_BEM_data(1, 0, 0, 0, 0, 0, 0);
	xTaskCreate((TaskFunction_t   )send_BEM_task,          
	            (const char*      )"send_BEM_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBEMTaskHandler); 
	vTaskDelete(SendBHMTaskHandler);
}

void send_BRM_task(){
	uint16_t interval = 0, i;
	BRM_Flag = 1;
	BCP_Flag = 0;
	while(interval<=5000){
		uint32_t extId;
		uint8_t priority = 7, buf[8],len=8;
		
		/*
		buf[0]=16;
		buf[1]=0; buf[2]=41;
		buf[3]=6;
		buf[4]=0xFF;
		buf[5]=0; buf[6]=BRM_Param; buf[7]=0;
		Get_Identifier(&stdId, &extId, priority, TP_Param, Charger_Address, BMS_Address);
		Extern_CAN_SendMsg(stdId, extId, buf, len);
		*/
		
		buf[0]=1; buf[1]=1; buf[2]=0;
		buf[3]=data2566&0xFF;
		buf[4]=data2567&0xFF;  buf[5]=(data2567>>8)&0xFF;
		buf[6]=data2568&0xFF;  buf[7]=(data2568>>8)&0xFF;
		printf("����BRM\r\n");
		Get_Identifier(&extId, priority, BRM_Param, Charger_Address, BMS_Address);
		Extern_CAN_SendMsg(0, extId, buf, len);
		
		/*/
		for(i=1;i<=6;i++){
			send_BRM_data(i);
			vTaskDelay(10);
			send_BRM_data(i);
			vTaskDelay(10);
		}
		*/
		
		//printf("����BRM\r\n");
		interval += 250;
		vTaskDelay(250);
	}
	BRM_Flag = 0;
	set_BEM_data(0, 1, 0, 0, 0, 0, 0);
	xTaskCreate((TaskFunction_t   )send_BEM_task,          
	            (const char*      )"send_BEM_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBEMTaskHandler); 
	vTaskDelete(SendBRMTaskHandler);
}

void send_BRM_data(uint8_t num){
  uint8_t data[41];
	uint32_t extId;
  uint8_t	size=41, priority = 7, buf[8], len=8, i, index;
	data[0]=1; data[1]=1; data[2]=0;
	data[3]=data2566&0xFF;
	data[4]=data2567&0xFF;  data[5]=(data2567>>8)&0xFF;
	data[6]=data2568&0xFF;  data[7]=(data2568>>8)&0xFF;
	for(i=8;i<41;i++){
		data[i]=0;
	}
	
	buf[0]=num;
	for(i=1;i<8;i++){
		index = (num-1)*7+i-1;
		if(index<size){
			buf[i] = data[index];
		}else{
			buf[i] = 0xFF;
		}
	}
	Get_Identifier(&extId, priority, TP_Param, Charger_Address, BMS_Address);
	printf("����BRM���� - %d\r\n", num);
	Extern_CAN_SendMsg(0, extId, buf, len);	
}

void send_BCP_task(){
	uint16_t interval = 0;
	BCP_Flag = 1;
	BRO_Flag = 0;
	while(interval<=5000){
		uint32_t extId;
		uint8_t priority = 7, buf[8], len = 8;
		buf[0]=16;
		buf[1]=0; buf[2]=13;
		buf[3]=2;
		buf[4]=0xFF;
		buf[5]=0; buf[6]=BCP_Param; buf[7]=0;
		Get_Identifier(&extId, priority, TP_Param, Charger_Address, BMS_Address);
		Extern_CAN_SendMsg(0, extId, buf, len);
		printf("����BCP\r\n");
		interval += 250;
		vTaskDelay(250);
	}
	//���ͳ�ʱ����֡
	BCP_Flag = 0;
	set_BEM_data(0, 0, 1, 0, 0, 0, 0);
	xTaskCreate((TaskFunction_t   )send_BEM_task,          
	            (const char*      )"send_BEM_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBEMTaskHandler); 
	vTaskDelete(SendBCPTaskHandler);
}

void send_BCP_data(uint8_t num){
	uint8_t data[13];
	uint32_t extId;
  uint8_t	size=13, priority = 6, buf[8], len=8, i, index;
	data[0] = data2816&0xFF;  data[1] = (data2816>>8)&0xFF;
	data[2] = data2817&0xFF;  data[3] = (data2817>>8)&0xFF;
	data[4] =	data2818&0xFF;  data[5] = (data2818>>8)&0xFF;
	data[6] = data2819&0xFF;  data[7] = (data2819>>8)&0xFF;
  data[8] = data2820&0xFF;
  data[9] = data2821&0xFF;  data[10] = (data2821>>8)&0xFF;
  data[11] = data2822&0xFF;  data[12] = (data2822>>8)&0xFF;
	buf[0]=num;
	for(i=1;i<8;i++){
		index = (num-1)*7+i-1;
		if(index<size){
			buf[i] = data[index];
		}else{
			buf[i] = 0xFF;
		}
	}
	Get_Identifier(&extId, priority, TP_Param, Charger_Address, BMS_Address);
	printf("����BCP���� - %d\r\n", num);
	Extern_CAN_SendMsg(0, extId, buf, len);
}

void send_BRO_task(){
	uint16_t interval = 0;
	BRO_Flag = 1;
	BCL_Flag = 0;
	BCS_Flag = 0;
	Check_CCS_Flag = 0;
	while(interval<=5000){
		uint32_t extId;
		uint8_t priority = 4, buf[1], len;
		len = 1;
		buf[0]=0xAA;
		Get_Identifier(&extId, priority, BRO_Param, Charger_Address, BMS_Address);
		Extern_CAN_SendMsg(0, extId, buf, len);
		printf("����BRO\r\n");
		interval += 250;
		vTaskDelay(250);
	}
	//���ͳ�ʱ����֡
	BRO_Flag = 0;
	set_BEM_data(0, 0, 0, 1, 0, 0, 0);
	xTaskCreate((TaskFunction_t   )send_BEM_task,          
	            (const char*      )"send_BEM_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBEMTaskHandler); 
	vTaskDelete(SendBROTaskHandler);
}

void send_BCL_task(){
	BCL_Flag = 1;
	while(1){
		uint32_t extId;
		uint8_t priority = 6, buf[5], len;
		len = 5;
		buf[0]=data3072&0xFF;  buf[1]=(data3072>>8)&0xFF;
		buf[2]=data3073&0xFF;  buf[3]=(data3073>>8)&0xFF;
		buf[4]=data3074&0xFF;
		Get_Identifier(&extId, priority, BCL_Param, Charger_Address, BMS_Address);
		Extern_CAN_SendMsg(0, extId, buf, len);
		printf("����BCL\r\n");
		vTaskDelay(250);
	}
	BCL_Flag = 0 ;
	vTaskDelete(SendBCLTaskHandler);
}

void send_BCS_task(){
	BCS_Flag = 1;
	BST_Flag = 0;
	while(1){
		uint32_t extId;
		uint8_t priority = 7, buf[8], len;
		len = 8;
		//���Ͷ�֡����TP.CM_RTS
		buf[0]=16;
		buf[1]=0; buf[1]=9;
		buf[3]=2;
		buf[4]=0xFF;
		buf[5]=0; buf[6]=BCS_Param; buf[7]=0;
		Get_Identifier(&extId, priority, TP_Param, Charger_Address, BMS_Address);
		Extern_CAN_SendMsg(0, extId, buf, len);
		printf("����BCS\r\n");
		vTaskDelay(250);
	}
	BCS_Flag = 0;
	vTaskDelete(SendBCSTaskHandler);
}

void send_BCS_data(uint8_t num){
	uint32_t extId;
	uint8_t	priority = 6, size=9, buf[8], len=8, i, index;
	uint8_t data[9];
	data[0] = data3075&0xFF;  data[1] = (data3075>>8)&0xFF;
  data[2] = data3076&0xFF;  data[3] = (data3076>>8)&0xFF;
  data[4] = data3077&0xFF;  data[5] = (data3077>>8)&0xFF;
  data[6] = data3078;
  data[7] = data3079&0xFF;  data[8] = (data3079>>8)&0xFF;
	buf[0]=num;
	for(i=1;i<8;i++){
		index = (num-1)*7+i-1;
		if(index<size){
			buf[i] = data[index];
		}else{
			buf[i] = 0xFF;
		}
	}
	Get_Identifier(&extId, priority, TP_Param, Charger_Address, BMS_Address);
	printf("����BCS���� - %d\r\n", num);
	Extern_CAN_SendMsg(0, extId, buf, len);
}

void check_CCS_task(){
	Check_CCS_Flag = 1;
	while(runtime-Check_CCS_Tick<=1){
    vTaskDelay(100);
	}
	printf("CCS��ʱ\r\n");
	Check_CCS_Flag = 0;
	set_BEM_data(0, 0, 0, 0, 1, 0, 0);
	xTaskCreate((TaskFunction_t   )send_BEM_task,          
	            (const char*      )"send_BEM_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendBEMTaskHandler);
	vTaskDelete(CheckCCSTaskHandler);
	if(BCL_Flag){
		BCL_Flag = 0;
		vTaskDelete(SendBCLTaskHandler);
	}
	if(BCS_Flag){
		BCS_Flag = 0;
		vTaskDelete(SendBCSTaskHandler);
	}
	if(BSM_Flag){
		BSM_Flag = 0;
		vTaskDelete(SendBSMTaskHandler);
	}
}

void send_BSM_task(){
	BSM_Flag = 1;
	//û�������ظ����ͺͳ�ʱ�ж�
	while(1){
		uint32_t extId;
		uint8_t priority = 6, buf[7], len = 7;
		buf[0] = data3085&0xFF;
		buf[1] = data3086&0xFF;
		buf[2] = data3087&0xFF;
		buf[3] = data3088&0xFF;
		buf[4] = data3089&0xFF;
		buf[5] = (data3090|(data3091<<2)|(data3092<<4)|(data3093<<6))&0xFF;
		buf[6] = (data3094|(data3094<<2)|(data3096<<4))&0xFF;
		Get_Identifier(&extId, priority, BSM_Param, Charger_Address, BMS_Address);
		Extern_CAN_SendMsg(0, extId, buf, len);
		printf("����BSM\r\n");
		vTaskDelay(250);
	}
	BSM_Flag = 0;
	vTaskDelete(SendBSMTaskHandler);
}

void send_BST_task(){
	uint8_t i = 0;
	BST_Flag = 1;
	BSD_Flag = 0;
	for(i=0;i<6;i++){
		uint32_t extId;
		uint8_t priority = 4, buf[4], len = 4;
		buf[0] = data3511&0xFF;
		buf[1] = data3512&0xFF;  buf[2] = (data3512>>8)&0xFF;
		buf[3] = data3513&0xFF;
		Get_Identifier(&extId, priority, BST_Param, Charger_Address, BMS_Address);
		Extern_CAN_SendMsg(0, extId, buf, len);
		printf("����BST\r\n");
    vTaskDelay(250);
	}
	BST_Flag = 0;
	vTaskDelete(SendBSTTaskHandler);
}

void send_BSD_task(){
  //�ѽ���ͳ�ƽ׶Σ��ڴ˶Ͽ����̵���
	uint8_t i;
	BSD_Flag = 1;
	for(i=0;i<6;i++){
		uint32_t extId;
		uint8_t priority = 6, buf[7], len = 7;
		buf[0] = data3601&0xFF;
		buf[1] = data3602&0xFF;  buf[2] = (data3602>>8)&0xFF;
		buf[3] = data3603&0xFF;  buf[4] = (data3603>>8)&0xFF;
		buf[5] = data3604&0xFF;
		buf[6] = data3605&0xFF;
		Get_Identifier(&extId, priority, BSD_Param, Charger_Address, BMS_Address);
		Extern_CAN_SendMsg(0, extId, buf, len);
		printf("����BSD\r\n");
    vTaskDelay(250);
	}
	BSD_Flag = 0;
	vTaskDelete(SendBSDTaskHandler);
}

void send_BEM_task(){
	u8 i, priority = 2, buf[4], len = 4;
	u32 extId;
	buf[0] = (data3901&0x3)|((data3902&0x3)<<2);
	buf[1] = (data3903&0x3)|((data3904&0x3)<<2);
	buf[2] = (data3905&0x3)|((data3906&0x3)<<2);
	buf[3] = data3907&0x3;
	Get_Identifier(&extId, priority, BEM_Param, Charger_Address, BMS_Address);
	for(i=0;i<3;i++){
		Extern_CAN_SendMsg(0, extId, buf, len);
		printf("����BEM\r\n");
		vTaskDelay(250);
	}
	vTaskDelete(SendBEMTaskHandler);
}

void set_BHM_data(u16 d2601){
	data3601 = d2601;
}

void set_BRM_data(u32 d2565, u8 d2566, u16 d2567, u16 d2568){
	data2565 = d2565;
	data2566 = d2566;
	data2567 = d2567;
	data2568 = d2568;
}

void set_BCP_data(u16 d2816, u16 d2817, u16 d2818, u16 d2819, u8 d2820, u16 d2821, u16 d2822){
	data2816 = d2816;
	data2817 = d2817;
	data2818 = d2818;
	data2819 = d2819;
	data2820 = d2820;
	data2821 = d2821;
	data2822 = d2822;
}

void set_BRO_data(u8 d2829){
	data2829 = d2829;
}

void set_BCL_data(u16 d3072, u16 d3073, u8 d3074){
	data3072 = d3072;
	data3073 = d3073;
	data3074 = d3074;
}

void set_BCS_data(u16 d3075, u16 d3076, u16 d3077, u8 d3078, u16 d3079){
	data3075 = d3075;
	data3076 = d3076;
	data3077 = d3077;
	data3078 = d3078;
	data3079 = d3079;
}

void set_BSM_data(u8 d3085, u8 d3086, u8 d3087, u8 d3088, u8 d3089, u8 d3090, u8 d3091, u8 d3092, u8 d3093, u8 d3094, u8 d3095, u8 d3096){
	data3085 = d3085;
	data3086 = d3086;
	data3087 = d3087;
	data3088 = d3088;
	data3089 = d3089;
	data3090 = d3090;
	data3091 = d3091;
	data3092 = d3092;
	data3093 = d3093;
	data3094 = d3094;
	data3095 = d3095;
	data3096 = d3096;
}

void set_BST_data(u8 d3511, u16 d3512, u8 d3513){
	data3511 = d3511;
	data3512 = d3512;
	data3513 = d3513;
}

void set_BSD_data(u8 d3601, u16 d3602, u16 d3603, u8 d3604, u8 d3605){
	data3601 = d3601;
	data3602 = d3602;
	data3603 = d3603;
	data3604 = d3604;
	data3605 = d3605;
}

void set_BEM_data(u8 d3901, u8 d3902, u8 d3903, u8 d3904, u8 d3905, u8 d3906, u8 d3907){
	data3901 = d3901;
	data3902 = d3902;
	data3903 = d3903;
	data3904 = d3904;
	data3905 = d3905;
	data3906 = d3906;
	data3907 = d3907;
}

u8 check_CCS_data(u8 *data){
	float data3081 = ((data[0]<<8)|data[1])*0.1;
	float data3082 = ((data[2]<<8)|data[3])*0.1;
	if(data3081>3.65){
		//3.65*�ܴ���
		BMS_Total_Voltage_Flag = 3;
		//����
		return 0;
	}else if(BMS_Total_Voltage_Flag==2){
		if(data3081<3.45){
			//������0��
			BMS_Total_Voltage_Flag = 0;
			//�ظ�����
		}else if(data3081<3.5){
			//������1��
			BMS_Total_Voltage_Flag = 1;
			//50
		}
	}else if(BMS_Total_Voltage_Flag==1){
		if(data3081>3.55){
			//������2��
			BMS_Total_Voltage_Flag = 2;
			//0
		}else if(data3081<3.45){
			//������0��
			BMS_Total_Voltage_Flag = 0;
			//�ظ�����
		}
	}else if(BMS_Total_Voltage_Flag==0){
		if(data3081>3.55){
			//������2��
			BMS_Total_Voltage_Flag = 2;
			//0
		}else if(data3081>3.5){
			//������1��
			BMS_Total_Voltage_Flag = 1;
			//50
		}
	}
	
	if(data3082>100){
		//3.65*�ܴ���
		BMS_Total_Current_Flag = 3;
		//����
		return 0;
	}else if(BMS_Total_Current_Flag==2){
		if(data3082<75){
			//������0��
			BMS_Total_Current_Flag = 0;
			//����
		}else if(data3082<80){
			//������1��
			BMS_Total_Current_Flag = 1;
			//50
		}
	}else if(BMS_Total_Current_Flag==1){
		if(data3082>80){
			//������2��
			BMS_Total_Current_Flag = 2;
			//0
		}else if(data3082<75){
			//������0��
			BMS_Total_Current_Flag = 0;
			//�ظ�����
		}
	}else if(BMS_Total_Current_Flag==0){
		if(data3082>80){
			//������2��
			BMS_Total_Current_Flag = 2;
			//0
		}else if(data3082>3.5){
			//������1��
			BMS_Total_Current_Flag = 1;
			//50
		}
	}
}

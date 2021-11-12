
#include "bms_state.h"
#include "rs485_DTU.h"
//#include "usart.h"
#include "stdlib.h"
#include "string.h"  //C��׼���ַ���������,Ԥ��ʹ��strncat����
#include "stdio.h"
#include "rs485_elect_meter.h"

//��ر�������
uint8_t RS4851_RX_BUF[RS4851_REC_LEN];  //RS485_Screen���ջ�����
uint8_t RS4851_RX_LEN;
uint16_t RS4851_RX_STA;  //RS485_Screen����״̬���

//����ƴ��JSON
const char DEVICEBCU_15[15] = {'"','d','e','v','i','c','e','"',':','"','b','c','u','"',','};
const char DEVICEBMU_15[15] = {'"','d','e','v','i','c','e','"',':','"','b','m','u','"',','};
const char BMUID_9[9] = {'"','b','m','u','I','d','"',':','"'};
const char CELL_8[8] = {'"','c','e','l','l','"',':','['};
const char ID_6[6] = {'"','i','d','"',':','"'};
const char VOL_6[6] = {'"','v','o','l','"',':'};
const char SOC_6[6] = {'"','s','o','c','"',':'};
const char TEMP_8[8] = {'"','t','e','m','p','"',':','['};
const char VAL_6[6] = {'"','v','a','l','"',':'};
const char SMOKE_9[9] = {'"','s','m','o','k','e','"',':','['};
const char CURR_7[7] = {'"','c','u','r','r','"',':'};
const char BCUID_9[9] = {'"','b','c','u','I','d','"',':','"'};
const char VOLT_8[8] = {'"','v','o','l','t','"',':','['};
const char INSU_8[8] = {'"','i','n','s','u','"',':','['};
const char CC_5[5] = {'"','c','c','"',':'};

//���ݷ���ģ��
const char bmu_head[] = "{\"device\":\"bmu\",\"bmuId\":\"%d\"%s}smartiot\r\n";
const char bcu_head[] = "{\"device\":\"bcu\",\"bcuId\":\"%d\"%s}smartiot\r\n";
const char cell_template[] = "{\"id\":\"%d\",\"vol\":%.4f,\"soc\":%.3f}";
const char common2f_template[] = "{\"id\":\"%d\",\"val\":%.2f}";
const char commond_template[] = "{\"id\":\"%d\",\"val\":%d}";
const char cell_head[] = ",\"cell\":[%s]";
const char temp_head[] = ",\"temp\":[%s]";
const char smoke_head[] = ",\"smoke\":[%s]";
const char volt_head[] = ",\"volt\":[%s]";
const char soc_head[] = ",\"soc\":%.3f";
const char curr_head[] = ",\"curr\":%.3f";
const char insu_head[] = ",\"insu\":[%s]";
const char system_head[] = ",\"system\":{\"online\":%d,\"charge\":%d,\"cc\":%d,\"heat\":%d,\"system\":%d,\"interior\":%d,\"voltage\":%f,\"current\":%f}";

//char upload_data[DTU_SEND_LEN], part_data[DTU_SEND_LEN], temp_complete[DTU_SEND_LEN], temp_single[DTU_SEND_LEN];

//RS485_Screen��ʼ������
//���ô���USART1��485ת��ģ��CON����PG10
void RS485_DTU_Init(u32 bound) {
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10	
	
	
	//USART��������
	USART_InitStructure.USART_BaudRate = bound;//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);  //ʹ��

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	//USART1 NVIC 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;  //��ռ���ȼ�6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	
	
	
	//����PG10(BMU/BCU��DTUͨ��485ģ���������)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //����
	GPIO_Init(GPIOG, &GPIO_InitStructure);  //��ʼ��
	GPIO_SetBits(GPIOG,GPIO_Pin_10);

	RS4851_RX_LEN = 0;
}

/*
����:DTUģ��485�������
����:�����ڵ������ֲ���,���ַ�������ʱ,ÿ�����ֻ�����244�ֽ�;
*/
void RS485_DTU_SendData(const uint8_t *data, uint8_t len){
	uint8_t i =0;
	RS485_RE1_OFF;
	//�˴�ֱ�Ӳ����Ĵ������ܻ����һЩĪ������Ĵ���
	for(i=0; i<len; i++){
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
		USART_SendData(USART1,data[i]);
	}
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET) ;
	RS485_RE1_ON;
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		USART_SendData(USART1,Res);	//����
		
		if((RS4851_RX_STA&0x8000)==0)//����δ���
		{
			if(RS4851_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)
					RS4851_RX_STA=0;//���մ���,���¿�ʼ
				else {
					RS4851_RX_STA|=0x8000;	//��������� 
					
					//����
					for(Res=0;Res<(RS4851_RX_STA&0X3FFF);Res++)
						USART_SendData(USART1,RS4851_RX_BUF[Res]);
					
				}
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)RS4851_RX_STA|=0x4000;
				else
				{
					RS4851_RX_BUF[RS4851_RX_STA&0X3FFF]=Res ;
					RS4851_RX_STA++;
					if(RS4851_RX_STA>(RS4851_REC_LEN-1))RS4851_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		} 

  } 
  
  //GPIO_ToggleBits(GPIOD,GPIO_Pin_15);
  
}

void DTU_UploadBMUCell_InBCU(uint8_t bmu_id, uint8_t pos_s, uint8_t pos_e){
  char *upload_data = NULL, *part_data = NULL, *temp_complete = NULL, *temp_single = NULL;
  uint8_t up_num = 0;
  uint16_t i;
  //char up_data[DTU_SEND_LEN], cell_comp[DTU_SEND_LEN], cell_data[DTU_SEND_LEN], temp_data[DTU_SEND_LEN];

  //�����ڴ�ռ�
  upload_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  part_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_complete = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_single = mymalloc(SRAMIN, DTU_SEND_LEN);
  if(upload_data == NULL || part_data == NULL || temp_complete == NULL || temp_single == NULL){
    printf("�����ڴ�ʧ��");
    return;
  }
  memset(upload_data, 0, DTU_SEND_LEN);
  memset(part_data, 0, DTU_SEND_LEN);
  memset(temp_complete, 0, DTU_SEND_LEN);

  //����cell����ģ��
  for(i=pos_s;i<pos_e;i++){
    memset(temp_single, 0, DTU_SEND_LEN);
    cellvol[i] = 3.3;
    soc[i] = 23.5;
    sprintf(temp_single, cell_template, i+1, cellvol[i], soc[i]);
    if(i!=pos_s){
      strcat(temp_complete, ",");
    }
    strcat(temp_complete, temp_single);
  }

  //�����ݽ��л���
  sprintf(part_data, cell_head, temp_complete);
  //sprintf(upload_data, bmu_head, BMU_List_Did[bmu_id], part_data);
	sprintf(upload_data, bmu_head, bmu_id, part_data);
  up_num = strlen(upload_data);
  //printf("%d\r\n", up_num);
  printf(upload_data);
  RS485_DTU_SendData(upload_data, up_num);
  myfree(SRAMIN, upload_data);
  myfree(SRAMIN, part_data);
  myfree(SRAMIN, temp_complete);
  myfree(SRAMIN, temp_single);
}

void DTU_UploadBMUTemp_InBCU(uint8_t bmu_id, uint8_t pos_s, uint8_t pos_e){
  char *upload_data = NULL, *part_data = NULL, *temp_complete = NULL, *temp_single = NULL;
  uint8_t up_num = 0;
  uint16_t i;

  //�����ڴ�ռ�
  upload_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  part_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_complete = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_single = mymalloc(SRAMIN, DTU_SEND_LEN);
  if(upload_data == NULL || part_data == NULL || temp_complete == NULL || temp_single == NULL){
    printf("�����ڴ�ʧ��");
    return;
  }
  memset(upload_data, 0, DTU_SEND_LEN);
  memset(part_data, 0, DTU_SEND_LEN);
  memset(temp_complete, 0, DTU_SEND_LEN);

  //����cell����ģ��
  for(i=pos_s;i<pos_e;i++){
    memset(temp_single, 0, DTU_SEND_LEN);
    tempf[i] = 18.3;
    sprintf(temp_single, common2f_template, i+1, tempf[i]);
    if(i!=pos_s){
      strcat(temp_complete, ",");
    }
    strcat(temp_complete, temp_single);
  }

  //�����ݽ��л���
  sprintf(part_data, temp_head, temp_complete);
  //sprintf(upload_data, bmu_head, BMU_List_Did[bmu_id], part_data);
	sprintf(upload_data, bmu_head, bmu_id, part_data);
  up_num = strlen(upload_data);
  //printf("%d\r\n", up_num);
  printf(upload_data);
  RS485_DTU_SendData(upload_data, up_num);
  myfree(SRAMIN, upload_data);
  myfree(SRAMIN, part_data);
  myfree(SRAMIN, temp_complete);
  myfree(SRAMIN, temp_single);
}

void DTU_UploadBCUData(){
  char *upload_data=NULL,*part_data=NULL,*temp_complete=NULL,*temp_single=NULL, *volt_data=NULL, *soc_data=NULL, *curr_data=NULL, *insu_data=NULL, *system_data=NULL;
  uint8_t up_num = 0;
  uint16_t i;

  //�����ڴ�ռ�
  upload_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  part_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  volt_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  soc_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  curr_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  insu_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  system_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_complete = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_single = mymalloc(SRAMIN, DTU_SEND_LEN);
  if(upload_data==NULL||part_data==NULL||volt_data==NULL||soc_data==NULL||curr_data==NULL||insu_data==NULL||system_data==NULL||temp_complete==NULL||temp_single==NULL){
    printf("�����ڴ�ʧ��");
    return;
  }
  memset(upload_data, 0, DTU_SEND_LEN);
  memset(part_data, 0, DTU_SEND_LEN);
  memset(volt_data, 0, DTU_SEND_LEN);
  memset(soc_data, 0, DTU_SEND_LEN);
  memset(curr_data, 0, DTU_SEND_LEN);
  memset(insu_data, 0, DTU_SEND_LEN);
  memset(system_data, 0, DTU_SEND_LEN);
  memset(temp_complete, 0, DTU_SEND_LEN);

  //�����ѹ����ģ��
  for(i=0;i<3;i++){
    memset(temp_single, 0, DTU_SEND_LEN);
    sprintf(temp_single, commond_template, i+1, voltotal[i]);
    if(i!=0){
      strcat(temp_complete, ",");
    }
    strcat(temp_complete, temp_single);
  }
  sprintf(volt_data, volt_head, temp_complete);

  sprintf(soc_data, soc_head, soctotal);    //����socģ��
  sprintf(curr_data, curr_head, currentf);   //�������ģ��

  //�����Եģ��
  memset(temp_single, 0, DTU_SEND_LEN);
  memset(temp_complete, 0, DTU_SEND_LEN);
  sprintf(temp_single, commond_template, 1, insuval_pos);
  strcat(temp_complete, temp_single);
  memset(temp_single, 0, DTU_SEND_LEN);
  sprintf(temp_single, commond_template, 2, insuval_neg);
  strcat(temp_complete, ",");
  strcat(temp_complete, temp_single);
  sprintf(insu_data, insu_head, temp_complete);

  //����ϵͳת̬ģ��
  //sprintf(system_data, system_data, ...);

  //�����ݽ��л���????????????
  strcat(part_data, volt_data);
  strcat(part_data, soc_data);
  strcat(part_data, curr_data);
  strcat(part_data, insu_data);
  //strcat(part_data, system_data);
  sprintf(upload_data, bcu_head, My_Did, part_data);
  up_num = strlen(upload_data);
  //printf("%d\r\n", up_num);
  printf(upload_data);
  RS485_DTU_SendData(upload_data, up_num);
  myfree(SRAMIN, upload_data);
  myfree(SRAMIN, part_data);
  myfree(SRAMIN, temp_complete);
  myfree(SRAMIN, temp_single);
  myfree(SRAMIN, volt_data);
  myfree(SRAMIN, soc_data);
  myfree(SRAMIN, curr_data);
  myfree(SRAMIN, insu_data);
  myfree(SRAMIN, system_data);
}

//�ϴ���������BMU���������ݣ�����BMU�ĵ���������,�����ֱ���bmuID,��������λ�á�����������ʼλ�á���ֹλ��
void DTU_UploadBMUData_InBCU(uint8_t bmu_id, uint8_t curr_pos, uint8_t smoke_pos_s, uint8_t smoke_pos_e){
  char *upload_data = NULL, *part_data = NULL, *temp_complete = NULL, *temp_single = NULL;
  uint8_t up_num = 0;
  uint16_t i;

  //�����ڴ�ռ�
  upload_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  part_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_complete = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_single = mymalloc(SRAMIN, DTU_SEND_LEN);
  if(upload_data == NULL || part_data == NULL || temp_complete == NULL || temp_single == NULL){
    printf("�����ڴ�ʧ��");
    return;
  }
  memset(upload_data, 0, DTU_SEND_LEN);
  memset(part_data, 0, DTU_SEND_LEN);
  memset(temp_complete, 0, DTU_SEND_LEN);

  //����smoke����ģ��
  for(i=smoke_pos_s;i<smoke_pos_e;i++){
    memset(temp_single, 0, DTU_SEND_LEN);
    sprintf(temp_single, commond_template, i+1, smoke[i]);
    if(i!=smoke_pos_s){
      strcat(temp_complete, ",");
    }
    strcat(temp_complete, temp_single);
  }
  //���������ݽ��л���
  sprintf(part_data, smoke_head, temp_complete);

  memset(temp_complete, 0, DTU_SEND_LEN);
  sprintf(temp_complete, curr_head, bmucurrentf[curr_pos]);
  strcat(part_data, temp_complete);

  sprintf(upload_data, bmu_head, bmu_id, part_data);
  up_num = strlen(upload_data);
  //printf("%d\r\n", up_num);
  printf(upload_data);
  RS485_DTU_SendData(upload_data, up_num);
  myfree(SRAMIN, upload_data);
  myfree(SRAMIN, part_data);
  myfree(SRAMIN, temp_complete);
  myfree(SRAMIN, temp_single);
}

/*
����:ͨ��DTU�ϴ������ش��ĵ�ѹֵ��ע:���е�ش��ķ�Χ����Ϊ����ϵͳ�еĵ�ش���š�
����:
- bmucount:BCU��BMU�ı��(0~4),�����BMU�Ļ�,�˴�Ӧ����4,����ֵ0xff
- chip:Ҫ�ϴ���BMU�µĵ�chip��оƬ�ϴβɼ������ĵ�ѹ��ֵ(0~BMU_VOLCHIP_NUM-1)
- volnumbegin:Ҫ�ϴ�ϵͳ�е�����ݵ���ʼλ��(����)(0~11)
- volnumend:Ҫ�ϴ�ϵͳ�е�����ݵ���ֹλ��(������)(1~12)
- ע:ÿ������ϴ�5����ش�����,�Ƽ�4��(12������Ҫ�ֱ��ϴ�3��)
����ֵ:1:�������������ȷ��Χ��,0:���������Χ����ȷ
*/
u8 RS485_DTU_UploadVolData(u8 bmucount, u8 volnumbegin,u8 volnumend)
{
	char jsondata[300];  //Ҫ���͵�json��ʽ����
	uint16_t jsoncount = 0;  //Ҫ���Ͷ��ٸ��ֽ�,��ǰjsondata����洢����Ч�ֽ���,Ҳ����һ���ֽڴ�ŵ��±��ַ
	char numstr[12];     //����������С��ת�����ַ���֮��Ľ��
	uint8_t numstrcount;    //����ת���ɵ��ַ������ֽ���
	u8 loop_i,loop_c,loop_n; //����ѭ���������
	float fdata;             //����������ʵ��ѹֵ|���soc״ֵ̬,��ת��Ϊ�ַ���
	u8 bmuid;
	u16 volid;
	
//	if(volnumbegin>(bcucellnum-1)||volnumend>bcucellnum||volnumbegin>=volnumend||(volnumend-volnumbegin)>6)  //�������������Ҫ��
//		return 0;
	
	memset(jsondata,0,300);
	jsondata[0] = '{';
	++jsoncount;
	
	strncat(jsondata,DEVICEBMU_15,15);        //"device"ΪBMU,�ϴ�����BMU���������� 
	jsoncount = jsoncount + 15;               
	strncat(jsondata,BMUID_9,9);              //"bmuId"
	jsoncount = jsoncount + 9;
	//bmuid = BMU_List_Id[bmucount];
	numstrcount = sprintf(numstr,"%d",bmuid);
	strncat(jsondata,numstr,numstrcount);  //��bmuId��ֵ���Ƶ�json��
	jsoncount = jsoncount + numstrcount;
	jsondata[jsoncount] = '"';
	++jsoncount;
	jsondata[jsoncount] = ',';              
	++jsoncount;                              //���bmuid��д��
	strncat(jsondata,CELL_8,8);               //"cell"
	jsoncount = jsoncount + 8;
	
	for(loop_i=volnumbegin;loop_i<volnumend;++loop_i){     //����ϵͳ��ָ��������n����ѹ����д��jsondata��
		jsondata[jsoncount] = '{';
		++jsoncount;
		
		strncat(jsondata,ID_6,6);              //"id"(��idΪ����������еĵ�ش����)
		jsoncount = jsoncount + 6;
		
		numstrcount = sprintf(numstr,"%d",loop_i+1);  // ��Ӧ��ش�id
		strncat(jsondata,numstr,numstrcount);  //��id��ֵ���Ƶ�json��
		jsoncount = jsoncount + numstrcount;		
		jsondata[jsoncount] = '"';              
		++jsoncount; 
		jsondata[jsoncount] = ',';              
		++jsoncount; 	                         //���"id"��д��
		
		strncat(jsondata,VOL_6,6);              //"vol"
		jsoncount = jsoncount + 6;		
		
#if IS_BCU_ROLE
		loop_c = (loop_i%(BMU_VOLCHIP_NUM*12)) / 12;
		loop_n = (loop_i%(BMU_VOLCHIP_NUM*12)) % 12;
		//fdata = (float)cellvol[(loop_i/(BMU_VOLCHIP_NUM*12))][loop_c][loop_n];  //fdata����ά���б�
#endif
#if IS_BMU_ROLE
		loop_c = loop_i / 12;
		loop_n = loop_i % 12;		
		//fdata = (float)cellvol[loop_c][loop_n];      //fdata����ά���б�
#endif	
		numstrcount = sprintf(numstr,"%.4f",fdata*0.0001);  //��ص�ѹ���ֱ���4λС����
		strncat(jsondata,numstr,numstrcount);  //��vol��ֵ���Ƶ�json��
		jsoncount = jsoncount + numstrcount;		
		jsondata[jsoncount] = ',';              
		++jsoncount; 	                         //���"vol"��д��	
		
		strncat(jsondata,SOC_6,6);             //"soc"
		jsoncount = jsoncount + 6;
		
#if IS_BCU_ROLE
		loop_c = (loop_i%(BMU_VOLCHIP_NUM*12)) / 12;
		loop_n = (loop_i%(BMU_VOLCHIP_NUM*12)) % 12;
		//fdata = soc[(loop_i/(BMU_VOLCHIP_NUM*12))][loop_c][loop_n];     //fdata����ά���б�
#endif
#if IS_BMU_ROLE
		loop_c = loop_i / 12;
		loop_n = loop_i % 12;
		//fdata = soc[loop_c][loop_n];
#endif	
		numstrcount = sprintf(numstr,"%.3f",fdata);  //���soc״̬����3λС����(����4��5��)
		strncat(jsondata,numstr,numstrcount);  //��soc��ֵ���Ƶ�json��
		jsoncount = jsoncount + numstrcount;   //���"soc"��д��			
	
		jsondata[jsoncount] = '}';              
		++jsoncount;

		if(loop_i<volnumend-1) {
			jsondata[jsoncount] = ',';              
			++jsoncount;
		}
	}
	
	jsondata[jsoncount] = ']';              
	++jsoncount;
	jsondata[jsoncount] = '}';              
	++jsoncount;                   //���bmu��chipƬ��ѹ�ɼ�оƬ��ǰvolnum����ش��ĵ�ѹ��soc���ݵ�json��ʽת��
	
	//ͨ��485���ڷ��͸�json��ʽ����
	RS485_DTU_SendData(jsondata,jsoncount);
	
	return 1;
}

/*
����:ͨ��DTU�ϴ��¶�����(Ŀǰʵ�ֵ���BCU�������ϴ�,�����Ĳ������úͽ�ɫ�й�,������BMU�Ľ�ɫ�Ļ�,���������޸�)
����:
- bmucount:BCU��BMU�ı��(0~BMU_LIST_NUM-1),�����BMU�Ļ�,�˴�Ӧ����BMU_LIST_NUM-1,����ֵ0xff
- tempnumbegin:Ҫ�ϴ����¶����ݵ���ʼλ��(����)(0~BCU_TEMP_NUM-1)
- tempnumend:Ҫ�ϴ����¶����ݵ���ֹλ��(������)(1~BCU_TEMP_NUM)
����ֵ:1:�������������ȷ��Χ��,0:���������Χ����ȷ
*/
u8 RS485_DTU_UploadTempData(u8 bmucount, u8 tempnumbegin, u8 tempnumend)
{
	char jsondata[300];      //Ҫ���͵�json��ʽ����
	uint16_t jsoncount = 0;  //Ҫ���Ͷ��ٸ��ֽ�,��ǰjsondata����洢����Ч�ֽ���,Ҳ����һ���ֽڴ�ŵ��±��ַ
	char numstr[12];         //����������С��ת�����ַ���֮��Ľ��
	uint8_t numstrcount;     //����ת���ɵ��ַ������ֽ���
	u8 loop_i;               //����ѭ���������	
	float fdata;             //����ת���¶�ֵ
	
	if(tempnumbegin>(BCU_TEMP_NUM-1)||tempnumend>BCU_TEMP_NUM||tempnumbegin>=tempnumend)  //�������������Ҫ��
		return 0;
	
	memset(jsondata,0,300);
	jsondata[0] = '{';
	++jsoncount;	
	
	strncat(jsondata,DEVICEBMU_15,15);        //"device"ΪBMU,�ϴ�����BMU���������� 
	jsoncount = jsoncount + 15;               
	strncat(jsondata,BMUID_9,9);              //"bmuId"
	jsoncount = jsoncount + 9;
	//numstrcount = sprintf(numstr,"%d",BMU_List_Id[bmucount]);
	strncat(jsondata,numstr,numstrcount);     //��bmuId��ֵ���Ƶ�json��
	jsoncount = jsoncount + numstrcount;
	jsondata[jsoncount] = '"';
	++jsoncount;
	jsondata[jsoncount] = ',';              
	++jsoncount;                              //���bmuid��д��	
	strncat(jsondata,TEMP_8,8);               //"temp"
	jsoncount = jsoncount + 8;	
	
	for(loop_i=tempnumbegin;loop_i<tempnumend;++loop_i){     //����BMU��(��ǰ����ȷ�������25��BMU����BCU)��25���¶�ֵ�е�tempnumbegin~tempnumend��ŷ�Χ�ڵ��¶�ֵת����json��ʽ֡��
		jsondata[jsoncount] = '{';              
		++jsoncount; 	
	
		strncat(jsondata,ID_6,6);              //"id"
		jsoncount = jsoncount + 6;
		numstrcount = sprintf(numstr,"%d",loop_i+1);  //��ȫϵͳ�е��¶�λ�ñ��
		strncat(jsondata,numstr,numstrcount);  //��id��ֵ���Ƶ�json��
		jsoncount = jsoncount + numstrcount;		
		jsondata[jsoncount] = '"';              
		++jsoncount; 
		jsondata[jsoncount] = ',';              
		++jsoncount; 	                         //���"id"��д��		
		
		strncat(jsondata,VAL_6,6);             //"val"
		jsoncount = jsoncount + 6;		
#if IS_BCU_ROLE
		//fdata = tempf[loop_i/5][loop_i%5];     //ӳ�䵽2ά����
#endif
#if IS_BMU_ROLE
		//fdata = tempf[loop_i/5][loop_i%5];     //ӳ�䵽2ά����
#endif
		numstrcount = sprintf(numstr,"%.2f",fdata);  //�¶�����ֵ������λС��
		strncat(jsondata,numstr,numstrcount);  //��tempf��ֵ���Ƶ�json��
		jsoncount = jsoncount + numstrcount;				
		jsondata[jsoncount] = '}';              
		++jsoncount;                           //���"val"��д��      
		
		if(loop_i<tempnumend-1) {
			jsondata[jsoncount] = ',';              
			++jsoncount;		
		}
	}
	
	jsondata[jsoncount] = ']';              
	++jsoncount;
	jsondata[jsoncount] = '}';              
	++jsoncount;                   //���bmu��tempnumbegin~tempnumend��Χ���¶������ϴ�(���485���ڴ��䳤�ȵ����ƻ�û����Ļ�,�˴�����ÿ����෢��5~7���¶�����)
	
	//ͨ��485���ڷ��͸�json��ʽ����
	RS485_DTU_SendData(jsondata,jsoncount);
	
	return 1;
}

/*
����:ͨ��DTU�ϴ���BCU�µĵ�bumcount��BMU�ĵ�������������(Ŀǰʵ�ֵ���BCU�������ϴ�,�����Ĳ������úͽ�ɫ�й�)
����:
- bmucount:BCU��BMU�ı��(0~BMU_LIST_NUM-1),�����BMU�Ļ�,�˴�Ӧ����BMU_LIST_NUM-1,����ֵ0xff
- currnum:Ҫ�ϴ��ĵ������ݸ���,��Ҫ�ϴ�Ϊ1(ֻ��1������ֵ),����Ҫ�ϴ�Ϊ0
- ccnum:Ҫ�ϴ���CC״̬���ݸ���,��Ҫ�ϴ�Ϊ1(ֻ��1��CC״ֵ̬),����Ҫ�ϴ�Ϊ0
- smokenum:Ҫ�ϴ����������ݸ���,��Ҫ�ϴ������0,����Ҫ�ϴ���Ϊ0
����ֵ:1:�������������ȷ��Χ��,0:���������Χ����ȷ
*/
u8 RS485_DTU_UploadCurrentCCSmokeData(u8 bmucount, u8 currnum, u8 ccnum, u8 smokenum)
{
	char jsondata[300];      //Ҫ���͵�json��ʽ����
	uint16_t jsoncount = 0;  //Ҫ���Ͷ��ٸ��ֽ�,��ǰjsondata����洢����Ч�ֽ���,Ҳ����һ���ֽڴ�ŵ��±��ַ
	char numstr[12];         //����������С��ת�����ַ���֮��Ľ��
	uint8_t numstrcount;     //����ת���ɵ��ַ������ֽ���
	u8 loop_i;               //����ѭ���������	
	float fdata;             //����ת��float������
	u8 uccstate;              //����ת��CC״̬(�˴����趨��ǹΪ"1",����Ϊ"2",���ӳɹ�Ϊ"3"
	
	if(currnum>1||ccnum>1||smokenum>BMU_SMOKE_NUM)  //�������������Ҫ��
		return 0;
	
	memset(jsondata,0,300);
	jsondata[0] = '{';
	++jsoncount;		
	
	strncat(jsondata,DEVICEBMU_15,15);        //"device"ΪBMU,�ϴ�����BMU���������� 
	jsoncount = jsoncount + 15;               
	strncat(jsondata,BMUID_9,9);              //"bmuId"
	jsoncount = jsoncount + 9;
	//numstrcount = sprintf(numstr,"%d",BMU_List_Id[bmucount]);
	strncat(jsondata,numstr,numstrcount);     //��bmuId��ֵ���Ƶ�json��
	jsoncount = jsoncount + numstrcount;
	jsondata[jsoncount] = '"';
	++jsoncount;
	jsondata[jsoncount] = ',';              
	++jsoncount;                              //���bmuid��д��	

	if(currnum>0) {
		strncat(jsondata,CURR_7,7);             //"curr"
		jsoncount = jsoncount + 7;

#if IS_BCU_ROLE
		fdata = bmucurrentf[bmucount];          //��Ӧ���BMU��������ֵ
#endif
#if IS_BMU_ROLE
		fdata = currentf;                       //��BMU��������ֵ
#endif
		numstrcount = sprintf(numstr,"%.3f",fdata);  //��������ֵ����3λС��
		strncat(jsondata,numstr,numstrcount);  //���������ݵ�ֵ���Ƶ�json��
		jsoncount = jsoncount + numstrcount;		

		if(smokenum+ccnum>0){
			jsondata[jsoncount] = ',';              
			++jsoncount;                           //���"curr"(��ʹ�ñ��)��д��
		}
	}

	if(ccnum>0) {
		strncat(jsondata,CC_5,5);             //"cc"
		jsoncount = jsoncount + 5;

#if IS_BCU_ROLE		
		uccstate = ccstatelist[bmucount];       //��Ӧ���BMU��CC״̬
#endif
#if IS_BMU_ROLE
		uccstate = ccstate;                 //��BMU��CC״̬
#endif
		
		numstrcount = sprintf(numstr,"%d",uccstate);
		strncat(jsondata,numstr,numstrcount);     //��cc״̬��ֵ���Ƶ�json��
		jsoncount = jsoncount + numstrcount;				
		
		if(smokenum>0){
			jsondata[jsoncount] = ',';              
			++jsoncount;                           //���"cc"��д��
		}
	}
	
	if(smokenum>0) {                         //"smoke"(�˴���smokenum��ʾ���͸�BMU�µĶ��ٸ���������)
			
		strncat(jsondata,SMOKE_9,9);
		jsoncount = jsoncount + 9;		

		for(loop_i=0;loop_i<smokenum;++loop_i) {  //д��smokenum����������(0~BMU_SMOKEDATA_NUM)
			jsondata[jsoncount] = '{';              
			++jsoncount; 				
			
			strncat(jsondata,ID_6,6);              //"id"(BCUϵͳ�е�����ID)
			jsoncount = jsoncount + 6;
			numstrcount = sprintf(numstr,"%d",bmucount*BMU_SMOKE_NUM+loop_i+1);
			strncat(jsondata,numstr,numstrcount);  //��id��ֵ���Ƶ�json��
			jsoncount = jsoncount + numstrcount;		
			jsondata[jsoncount] = '"';              
			++jsoncount; 
			jsondata[jsoncount] = ',';              
			++jsoncount; 	                         //���"id"��д��		
			
			strncat(jsondata,VAL_6,6);             //"val"
			jsoncount = jsoncount + 6;		
#if IS_BCU_ROLE
			//fdata = smokef[bmucount][loop_i];  //BCU�¶�ӦBMU����������
#endif
#if IS_BMU_ROLE
			fdata = smokef[loop_i];  //��BMU����������
#endif
			numstrcount = sprintf(numstr,"%.2f",fdata);  //��������ֵ������λС��
			strncat(jsondata,numstr,numstrcount);  //���������ݵ�ֵ���Ƶ�json��
			jsoncount = jsoncount + numstrcount;				
			jsondata[jsoncount] = '}';              
			++jsoncount;                           //���"val"��д��   			
			
			if(loop_i<smokenum-1) {                 //ǰsmokenum-1����������Ҫ��','
				jsondata[jsoncount] = ',';              
				++jsoncount;		
			}			
		}
		jsondata[jsoncount] = ']';              
		++jsoncount;	
	}
	jsondata[jsoncount] = '}';              
	++jsoncount;   	
	
	//ͨ��485���ڷ��͸�json��ʽ����
	RS485_DTU_SendData(jsondata,jsoncount);	
	
	return 1;
}

/*
����:ͨ��DTU�ϴ��ڸ�BCU�²ɼ���������
����:
- currnum:Ҫ�ϴ���BCU��������,��Ҫ�ϴ�Ϊ1(ֻ��Ҫ�ϴ�1������ֵ),����Ҫ�ϴ�Ϊ0
- soctotalnum:Ҫ�ϴ�������SOC״̬����,��Ҫ�ϴ�Ϊ1(ֻ��Ҫ�ϴ�1��SOCֵ),����Ҫ�ϴ�Ϊ0
- insunum:Ҫ�ϴ��ľ�Ե״̬����,��Ҫ�ϴ�Ϊ1(ֻ��Ҫ�ϴ�1����Ե����),����Ҫ�ϴ�Ϊ0
- voltnum:Ҫ�ϴ�����ѹ����,��Ҫ�ϴ�Ϊ1~3(ÿ��BCU���3����ѹ����),����Ҫ�ϴ�Ϊ0
����ֵ:1:�������������ȷ��Χ��,0:���������Χ����ȷ
*/
#if IS_BCU_ROLE
u8 RS485_DTU_UploadBCUData(u8 currnum, u8 soctotalnum, u8 insunum, u8 voltnum)
{
	char jsondata[300];      //Ҫ���͵�json��ʽ����
	uint16_t jsoncount = 0;  //Ҫ���Ͷ��ٸ��ֽ�,��ǰjsondata����洢����Ч�ֽ���,Ҳ����һ���ֽڴ�ŵ��±��ַ
	char numstr[12];         //����������С��ת�����ַ���֮��Ľ��
	uint8_t numstrcount;     //����ת���ɵ��ַ������ֽ���
	u8 loop_i;               //����ѭ���������	
	float fdata;             //����ת���¶�ֵ
	
	if(currnum>1||soctotalnum>1||insunum>2||voltnum>3||(currnum+soctotalnum+insunum+voltnum)==0)  //�������������Ҫ��
		return 0;
	
	memset(jsondata,0,300);
	jsondata[0] = '{';
	++jsoncount;		
	
	strncat(jsondata,DEVICEBCU_15,15);        //"device"ΪBCU,�ϴ�����BCU���������� 
	jsoncount = jsoncount + 15;               
	strncat(jsondata,BCUID_9,9);              //"bcuId"
	jsoncount = jsoncount + 9;
	numstrcount = sprintf(numstr,"%d",My_Id);
	strncat(jsondata,numstr,numstrcount);     //������bcuId��ֵ���Ƶ�json��
	jsoncount = jsoncount + numstrcount;
	jsondata[jsoncount] = '"';
	++jsoncount;
	jsondata[jsoncount] = ',';              
	++jsoncount;                              //���bcuid��д��	

	if(currnum>0) {                           //"curr"
		strncat(jsondata,CURR_7,7);             
		jsoncount = jsoncount + 7;
		fdata = currentf;    //����BCU�ĵ���ֵ,ȫ�ֱ���,ͨ������ȡ
		numstrcount = sprintf(numstr,"%.3f",fdata);  //�������ݴ���3λС��
		strncat(jsondata,numstr,numstrcount);  //����ֵ���Ƶ�json��
		jsoncount = jsoncount + numstrcount;
		if(soctotalnum+insunum+voltnum>0){          //��������������������
			jsondata[jsoncount] = ',';              	
			++jsoncount;                         //���"curr"��д��
		}
	}
	
	if(soctotalnum>0) {                      //"soctotal"
		strncat(jsondata,SOC_6,6);
		jsoncount = jsoncount + 6;	
		fdata = soctotal;    //����BCU�µ����������SOC״̬,ȫ�ֱ���,����õ�
		numstrcount = sprintf(numstr,"%.3f",fdata);  //SOC״̬���ݴ���3λС��
		strncat(jsondata,numstr,numstrcount);  //����ֵ���Ƶ�json��
		jsoncount = jsoncount + numstrcount;
		if(insunum+voltnum>0){                 //��������������������
			jsondata[jsoncount] = ',';              	
			++jsoncount;                         //���"soctotal"��д��
		}
	}
	
	if(insunum>0) {                          //"insu"
		strncat(jsondata,INSU_8,8);
		jsoncount = jsoncount + 8;

		for(loop_i=0;loop_i<insunum;++loop_i){ //д��voltnum����ѹ����(1~3)
			jsondata[jsoncount] = '{';              
			++jsoncount; 

			strncat(jsondata,ID_6,6);            //"id"
			jsoncount = jsoncount + 6;
			numstrcount = sprintf(numstr,"%d",loop_i+1);
			strncat(jsondata,numstr,numstrcount);  //��id��ֵ���Ƶ�json��
			jsoncount = jsoncount + numstrcount;		
			jsondata[jsoncount] = '"';              
			++jsoncount; 
			jsondata[jsoncount] = ',';              
			++jsoncount; 	                       //���"id"��д��				
			
			strncat(jsondata,VAL_6,6);           //"val"
			jsoncount = jsoncount + 6;		
			
			if(loop_i==0){
				fdata = insuval_pos;            //BCU�µ�������Ե����
			}
			if(loop_i==1){
				fdata = insuval_neg;            //BCU�µĸ�����Ե����
			}
			
			numstrcount = sprintf(numstr,"%.2f",fdata); //��Ե״̬����ֵ������λС��
			strncat(jsondata,numstr,numstrcount);  //����Ե״̬���ݵ�ֵ���Ƶ�json��
			jsoncount = jsoncount + numstrcount;				
			jsondata[jsoncount] = '}';              
			++jsoncount;                           //���"val"��д��   			
			
			if(loop_i<insunum-1) {                 //ǰvoltnum-1����������Ҫ��','
				jsondata[jsoncount] = ',';              
				++jsoncount;		
			}
		}			
		jsondata[jsoncount] = ']';              
		++jsoncount;				
		
		if(voltnum>0){                         //��������������������
			jsondata[jsoncount] = ',';              	
			++jsoncount;                         //���"insu"��д��
		}
	}
	
	if(voltnum>0) {                          //"volt"
		strncat(jsondata,VOLT_8,8);
		jsoncount = jsoncount + 8;			
		
		for(loop_i=0;loop_i<voltnum;++loop_i){ //д��voltnum����ѹ����(1~3)
			jsondata[jsoncount] = '{';              
			++jsoncount; 				
			
			strncat(jsondata,ID_6,6);            //"id"
			jsoncount = jsoncount + 6;
			numstrcount = sprintf(numstr,"%d",loop_i+1);
			strncat(jsondata,numstr,numstrcount);  //��id��ֵ���Ƶ�json��
			jsoncount = jsoncount + numstrcount;		
			jsondata[jsoncount] = '"';              
			++jsoncount; 
			jsondata[jsoncount] = ',';              
			++jsoncount; 	                       //���"id"��д��		
			
			strncat(jsondata,VAL_6,6);           //"val"
			jsoncount = jsoncount + 6;		

			fdata = voltotal[loop_i];  //BCU�µ���ѹ����

			numstrcount = sprintf(numstr,"%.2f",fdata); //��ѹ����ֵ������λС��
			strncat(jsondata,numstr,numstrcount);  //����ѹ���ݵ�ֵ���Ƶ�json��
			jsoncount = jsoncount + numstrcount;				
			jsondata[jsoncount] = '}';              
			++jsoncount;                           //���"val"��д��   			
			
			if(loop_i<voltnum-1) {                 //ǰvoltnum-1����������Ҫ��','
				jsondata[jsoncount] = ',';              
				++jsoncount;		
			}			
		}
		jsondata[jsoncount] = ']';              
		++jsoncount;		
	}

	jsondata[jsoncount] = '}';              
	++jsoncount;   	
	
	//ͨ��485���ڷ��͸�json��ʽ����
	RS485_DTU_SendData(jsondata,jsoncount);	
	
	return 1;
	
	
	
}
#endif





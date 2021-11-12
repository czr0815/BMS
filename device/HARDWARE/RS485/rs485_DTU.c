
#include "bms_state.h"
#include "rs485_DTU.h"
//#include "usart.h"
#include "stdlib.h"
#include "string.h"  //C标准库字符串处理函数,预计使用strncat函数
#include "stdio.h"
#include "rs485_elect_meter.h"

//相关变量定义
uint8_t RS4851_RX_BUF[RS4851_REC_LEN];  //RS485_Screen接收缓冲区
uint8_t RS4851_RX_LEN;
uint16_t RS4851_RX_STA;  //RS485_Screen接收状态标记

//用于拼接JSON
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

//数据发送模板
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

//RS485_Screen初始化函数
//配置串口USART1与485转换模块CON引脚PG10
void RS485_DTU_Init(u32 bound) {
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10	
	
	
	//USART串口配置
	USART_InitStructure.USART_BaudRate = bound;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);  //使能

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	//USART1 NVIC 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;  //抢占优先级6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
	
	
	//配置PG10(BMU/BCU的DTU通信485模块控制引脚)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);  //初始化
	GPIO_SetBits(GPIOG,GPIO_Pin_10);

	RS4851_RX_LEN = 0;
}

/*
功能:DTU模块485串口输出
问题:经串口调试助手测试,当字符串过长时,每次最多只能输出244字节;
*/
void RS485_DTU_SendData(const uint8_t *data, uint8_t len){
	uint8_t i =0;
	RS485_RE1_OFF;
	//此处直接操作寄存器可能会避免一些莫名其妙的错误
	for(i=0; i<len; i++){
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
		USART_SendData(USART1,data[i]);
	}
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET) ;
	RS485_RE1_ON;
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		USART_SendData(USART1,Res);	//测试
		
		if((RS4851_RX_STA&0x8000)==0)//接收未完成
		{
			if(RS4851_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)
					RS4851_RX_STA=0;//接收错误,重新开始
				else {
					RS4851_RX_STA|=0x8000;	//接收完成了 
					
					//测试
					for(Res=0;Res<(RS4851_RX_STA&0X3FFF);Res++)
						USART_SendData(USART1,RS4851_RX_BUF[Res]);
					
				}
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)RS4851_RX_STA|=0x4000;
				else
				{
					RS4851_RX_BUF[RS4851_RX_STA&0X3FFF]=Res ;
					RS4851_RX_STA++;
					if(RS4851_RX_STA>(RS4851_REC_LEN-1))RS4851_RX_STA=0;//接收数据错误,重新开始接收	  
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

  //申请内存空间
  upload_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  part_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_complete = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_single = mymalloc(SRAMIN, DTU_SEND_LEN);
  if(upload_data == NULL || part_data == NULL || temp_complete == NULL || temp_single == NULL){
    printf("申请内存失败");
    return;
  }
  memset(upload_data, 0, DTU_SEND_LEN);
  memset(part_data, 0, DTU_SEND_LEN);
  memset(temp_complete, 0, DTU_SEND_LEN);

  //填入cell数据模板
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

  //将数据进行汇总
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

  //申请内存空间
  upload_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  part_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_complete = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_single = mymalloc(SRAMIN, DTU_SEND_LEN);
  if(upload_data == NULL || part_data == NULL || temp_complete == NULL || temp_single == NULL){
    printf("申请内存失败");
    return;
  }
  memset(upload_data, 0, DTU_SEND_LEN);
  memset(part_data, 0, DTU_SEND_LEN);
  memset(temp_complete, 0, DTU_SEND_LEN);

  //填入cell数据模板
  for(i=pos_s;i<pos_e;i++){
    memset(temp_single, 0, DTU_SEND_LEN);
    tempf[i] = 18.3;
    sprintf(temp_single, common2f_template, i+1, tempf[i]);
    if(i!=pos_s){
      strcat(temp_complete, ",");
    }
    strcat(temp_complete, temp_single);
  }

  //将数据进行汇总
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

  //申请内存空间
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
    printf("申请内存失败");
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

  //填入电压数据模板
  for(i=0;i<3;i++){
    memset(temp_single, 0, DTU_SEND_LEN);
    sprintf(temp_single, commond_template, i+1, voltotal[i]);
    if(i!=0){
      strcat(temp_complete, ",");
    }
    strcat(temp_complete, temp_single);
  }
  sprintf(volt_data, volt_head, temp_complete);

  sprintf(soc_data, soc_head, soctotal);    //填入soc模板
  sprintf(curr_data, curr_head, currentf);   //填入电流模板

  //填入绝缘模板
  memset(temp_single, 0, DTU_SEND_LEN);
  memset(temp_complete, 0, DTU_SEND_LEN);
  sprintf(temp_single, commond_template, 1, insuval_pos);
  strcat(temp_complete, temp_single);
  memset(temp_single, 0, DTU_SEND_LEN);
  sprintf(temp_single, commond_template, 2, insuval_neg);
  strcat(temp_complete, ",");
  strcat(temp_complete, temp_single);
  sprintf(insu_data, insu_head, temp_complete);

  //填入系统转态模板
  //sprintf(system_data, system_data, ...);

  //将数据进行汇总????????????
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

//上传来自下属BMU的其他数据，包括BMU的电流和烟雾,参数分别是bmuID,电流数据位置、烟雾数据起始位置、中止位置
void DTU_UploadBMUData_InBCU(uint8_t bmu_id, uint8_t curr_pos, uint8_t smoke_pos_s, uint8_t smoke_pos_e){
  char *upload_data = NULL, *part_data = NULL, *temp_complete = NULL, *temp_single = NULL;
  uint8_t up_num = 0;
  uint16_t i;

  //申请内存空间
  upload_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  part_data = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_complete = mymalloc(SRAMIN, DTU_SEND_LEN);
  temp_single = mymalloc(SRAMIN, DTU_SEND_LEN);
  if(upload_data == NULL || part_data == NULL || temp_complete == NULL || temp_single == NULL){
    printf("申请内存失败");
    return;
  }
  memset(upload_data, 0, DTU_SEND_LEN);
  memset(part_data, 0, DTU_SEND_LEN);
  memset(temp_complete, 0, DTU_SEND_LEN);

  //填入smoke数据模板
  for(i=smoke_pos_s;i<smoke_pos_e;i++){
    memset(temp_single, 0, DTU_SEND_LEN);
    sprintf(temp_single, commond_template, i+1, smoke[i]);
    if(i!=smoke_pos_s){
      strcat(temp_complete, ",");
    }
    strcat(temp_complete, temp_single);
  }
  //将烟雾数据进行汇总
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
功能:通过DTU上传单体电池串的电压值【注:其中电池串的范围参数为整个系统中的电池串编号】
参数:
- bmucount:BCU下BMU的编号(0~4),如果是BMU的话,此处应大于4,典型值0xff
- chip:要上传该BMU下的第chip个芯片上次采集上来的电压数值(0~BMU_VOLCHIP_NUM-1)
- volnumbegin:要上传系统中电池数据的起始位置(包括)(0~11)
- volnumend:要上传系统中电池数据的终止位置(不包括)(1~12)
- 注:每条最多上传5个电池串数据,推荐4个(12个就需要分别上传3次)
返回值:1:传入参数均在正确范围内,0:传入参数范围不正确
*/
u8 RS485_DTU_UploadVolData(u8 bmucount, u8 volnumbegin,u8 volnumend)
{
	char jsondata[300];  //要发送的json格式数据
	uint16_t jsoncount = 0;  //要发送多少个字节,当前jsondata数组存储的有效字节数,也即下一个字节存放的下标地址
	char numstr[12];     //保存整数或小数转换成字符串之后的结果
	uint8_t numstrcount;    //保存转换成的字符串的字节数
	u8 loop_i,loop_c,loop_n; //用于循环处理计数
	float fdata;             //用于生成真实电压值|存放soc状态值,并转化为字符串
	u8 bmuid;
	u16 volid;
	
//	if(volnumbegin>(bcucellnum-1)||volnumend>bcucellnum||volnumbegin>=volnumend||(volnumend-volnumbegin)>6)  //传入参数不符合要求
//		return 0;
	
	memset(jsondata,0,300);
	jsondata[0] = '{';
	++jsoncount;
	
	strncat(jsondata,DEVICEBMU_15,15);        //"device"为BMU,上传的是BMU产生的数据 
	jsoncount = jsoncount + 15;               
	strncat(jsondata,BMUID_9,9);              //"bmuId"
	jsoncount = jsoncount + 9;
	//bmuid = BMU_List_Id[bmucount];
	numstrcount = sprintf(numstr,"%d",bmuid);
	strncat(jsondata,numstr,numstrcount);  //将bmuId的值复制到json中
	jsoncount = jsoncount + numstrcount;
	jsondata[jsoncount] = '"';
	++jsoncount;
	jsondata[jsoncount] = ',';              
	++jsoncount;                              //完成bmuid的写入
	strncat(jsondata,CELL_8,8);               //"cell"
	jsoncount = jsoncount + 8;
	
	for(loop_i=volnumbegin;loop_i<volnumend;++loop_i){     //将该系统中指定的连续n个电压数据写入jsondata中
		jsondata[jsoncount] = '{';
		++jsoncount;
		
		strncat(jsondata,ID_6,6);              //"id"(该id为整个电池组中的电池串编号)
		jsoncount = jsoncount + 6;
		
		numstrcount = sprintf(numstr,"%d",loop_i+1);  // 对应电池串id
		strncat(jsondata,numstr,numstrcount);  //将id的值复制到json中
		jsoncount = jsoncount + numstrcount;		
		jsondata[jsoncount] = '"';              
		++jsoncount; 
		jsondata[jsoncount] = ',';              
		++jsoncount; 	                         //完成"id"的写入
		
		strncat(jsondata,VOL_6,6);              //"vol"
		jsoncount = jsoncount + 6;		
		
#if IS_BCU_ROLE
		loop_c = (loop_i%(BMU_VOLCHIP_NUM*12)) / 12;
		loop_n = (loop_i%(BMU_VOLCHIP_NUM*12)) % 12;
		//fdata = (float)cellvol[(loop_i/(BMU_VOLCHIP_NUM*12))][loop_c][loop_n];  //fdata数组维度有变
#endif
#if IS_BMU_ROLE
		loop_c = loop_i / 12;
		loop_n = loop_i % 12;		
		//fdata = (float)cellvol[loop_c][loop_n];      //fdata数组维度有变
#endif	
		numstrcount = sprintf(numstr,"%.4f",fdata*0.0001);  //电池电压数字保存4位小数点
		strncat(jsondata,numstr,numstrcount);  //将vol的值复制到json中
		jsoncount = jsoncount + numstrcount;		
		jsondata[jsoncount] = ',';              
		++jsoncount; 	                         //完成"vol"的写入	
		
		strncat(jsondata,SOC_6,6);             //"soc"
		jsoncount = jsoncount + 6;
		
#if IS_BCU_ROLE
		loop_c = (loop_i%(BMU_VOLCHIP_NUM*12)) / 12;
		loop_n = (loop_i%(BMU_VOLCHIP_NUM*12)) % 12;
		//fdata = soc[(loop_i/(BMU_VOLCHIP_NUM*12))][loop_c][loop_n];     //fdata数组维度有变
#endif
#if IS_BMU_ROLE
		loop_c = loop_i / 12;
		loop_n = loop_i % 12;
		//fdata = soc[loop_c][loop_n];
#endif	
		numstrcount = sprintf(numstr,"%.3f",fdata);  //电池soc状态保存3位小数点(还会4舍5入)
		strncat(jsondata,numstr,numstrcount);  //将soc的值复制到json中
		jsoncount = jsoncount + numstrcount;   //完成"soc"的写入			
	
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
	++jsoncount;                   //完成bmu第chip片电压采集芯片下前volnum个电池串的电压和soc数据的json格式转换
	
	//通过485串口发送该json格式数据
	RS485_DTU_SendData(jsondata,jsoncount);
	
	return 1;
}

/*
功能:通过DTU上传温度数据(目前实现的是BCU的数据上传,函数的参数设置和角色有关,若加入BMU的角色的话,参数还需修改)
参数:
- bmucount:BCU下BMU的编号(0~BMU_LIST_NUM-1),如果是BMU的话,此处应大于BMU_LIST_NUM-1,典型值0xff
- tempnumbegin:要上传的温度数据的起始位置(包括)(0~BCU_TEMP_NUM-1)
- tempnumend:要上传的温度数据的终止位置(不包括)(1~BCU_TEMP_NUM)
返回值:1:传入参数均在正确范围内,0:传入参数范围不正确
*/
u8 RS485_DTU_UploadTempData(u8 bmucount, u8 tempnumbegin, u8 tempnumend)
{
	char jsondata[300];      //要发送的json格式数据
	uint16_t jsoncount = 0;  //要发送多少个字节,当前jsondata数组存储的有效字节数,也即下一个字节存放的下标地址
	char numstr[12];         //保存整数或小数转换成字符串之后的结果
	uint8_t numstrcount;     //保存转换成的字符串的字节数
	u8 loop_i;               //用于循环处理计数	
	float fdata;             //用于转存温度值
	
	if(tempnumbegin>(BCU_TEMP_NUM-1)||tempnumend>BCU_TEMP_NUM||tempnumbegin>=tempnumend)  //传入参数不符合要求
		return 0;
	
	memset(jsondata,0,300);
	jsondata[0] = '{';
	++jsoncount;	
	
	strncat(jsondata,DEVICEBMU_15,15);        //"device"为BMU,上传的是BMU产生的数据 
	jsoncount = jsoncount + 15;               
	strncat(jsondata,BMUID_9,9);              //"bmuId"
	jsoncount = jsoncount + 9;
	//numstrcount = sprintf(numstr,"%d",BMU_List_Id[bmucount]);
	strncat(jsondata,numstr,numstrcount);     //将bmuId的值复制到json中
	jsoncount = jsoncount + numstrcount;
	jsondata[jsoncount] = '"';
	++jsoncount;
	jsondata[jsoncount] = ',';              
	++jsoncount;                              //完成bmuid的写入	
	strncat(jsondata,TEMP_8,8);               //"temp"
	jsoncount = jsoncount + 8;	
	
	for(loop_i=tempnumbegin;loop_i<tempnumend;++loop_i){     //将该BMU下(当前还不确定是最多25是BMU还是BCU)的25串温度值中的tempnumbegin~tempnumend编号范围内的温度值转换到json格式帧中
		jsondata[jsoncount] = '{';              
		++jsoncount; 	
	
		strncat(jsondata,ID_6,6);              //"id"
		jsoncount = jsoncount + 6;
		numstrcount = sprintf(numstr,"%d",loop_i+1);  //在全系统中的温度位置编号
		strncat(jsondata,numstr,numstrcount);  //将id的值复制到json中
		jsoncount = jsoncount + numstrcount;		
		jsondata[jsoncount] = '"';              
		++jsoncount; 
		jsondata[jsoncount] = ',';              
		++jsoncount; 	                         //完成"id"的写入		
		
		strncat(jsondata,VAL_6,6);             //"val"
		jsoncount = jsoncount + 6;		
#if IS_BCU_ROLE
		//fdata = tempf[loop_i/5][loop_i%5];     //映射到2维数组
#endif
#if IS_BMU_ROLE
		//fdata = tempf[loop_i/5][loop_i%5];     //映射到2维数组
#endif
		numstrcount = sprintf(numstr,"%.2f",fdata);  //温度数据值传送两位小数
		strncat(jsondata,numstr,numstrcount);  //将tempf的值复制到json中
		jsoncount = jsoncount + numstrcount;				
		jsondata[jsoncount] = '}';              
		++jsoncount;                           //完成"val"的写入      
		
		if(loop_i<tempnumend-1) {
			jsondata[jsoncount] = ',';              
			++jsoncount;		
		}
	}
	
	jsondata[jsoncount] = ']';              
	++jsoncount;
	jsondata[jsoncount] = '}';              
	++jsoncount;                   //完成bmu的tempnumbegin~tempnumend范围的温度数据上传(如果485串口传输长度的限制还没解决的话,此处建议每次最多发送5~7个温度数据)
	
	//通过485串口发送该json格式数据
	RS485_DTU_SendData(jsondata,jsoncount);
	
	return 1;
}

/*
功能:通过DTU上传该BCU下的第bumcount个BMU的电流和烟雾数据(目前实现的是BCU的数据上传,函数的参数设置和角色有关)
参数:
- bmucount:BCU下BMU的编号(0~BMU_LIST_NUM-1),如果是BMU的话,此处应大于BMU_LIST_NUM-1,典型值0xff
- currnum:要上传的电流数据个数,需要上传为1(只有1个电流值),不需要上传为0
- ccnum:要上传的CC状态数据个数,需要上传为1(只有1个CC状态值),不需要上传为0
- smokenum:要上传的烟雾数据个数,需要上传则大于0,不需要上传则为0
返回值:1:传入参数均在正确范围内,0:传入参数范围不正确
*/
u8 RS485_DTU_UploadCurrentCCSmokeData(u8 bmucount, u8 currnum, u8 ccnum, u8 smokenum)
{
	char jsondata[300];      //要发送的json格式数据
	uint16_t jsoncount = 0;  //要发送多少个字节,当前jsondata数组存储的有效字节数,也即下一个字节存放的下标地址
	char numstr[12];         //保存整数或小数转换成字符串之后的结果
	uint8_t numstrcount;     //保存转换成的字符串的字节数
	u8 loop_i;               //用于循环处理计数	
	float fdata;             //用于转存float型数据
	u8 uccstate;              //用于转存CC状态(此处暂设定空枪为"1",按下为"2",连接成功为"3"
	
	if(currnum>1||ccnum>1||smokenum>BMU_SMOKE_NUM)  //传入参数不符合要求
		return 0;
	
	memset(jsondata,0,300);
	jsondata[0] = '{';
	++jsoncount;		
	
	strncat(jsondata,DEVICEBMU_15,15);        //"device"为BMU,上传的是BMU产生的数据 
	jsoncount = jsoncount + 15;               
	strncat(jsondata,BMUID_9,9);              //"bmuId"
	jsoncount = jsoncount + 9;
	//numstrcount = sprintf(numstr,"%d",BMU_List_Id[bmucount]);
	strncat(jsondata,numstr,numstrcount);     //将bmuId的值复制到json中
	jsoncount = jsoncount + numstrcount;
	jsondata[jsoncount] = '"';
	++jsoncount;
	jsondata[jsoncount] = ',';              
	++jsoncount;                              //完成bmuid的写入	

	if(currnum>0) {
		strncat(jsondata,CURR_7,7);             //"curr"
		jsoncount = jsoncount + 7;

#if IS_BCU_ROLE
		fdata = bmucurrentf[bmucount];          //对应编号BMU电流数据值
#endif
#if IS_BMU_ROLE
		fdata = currentf;                       //本BMU电流数据值
#endif
		numstrcount = sprintf(numstr,"%.3f",fdata);  //电流数据值传送3位小数
		strncat(jsondata,numstr,numstrcount);  //将电流数据的值复制到json中
		jsoncount = jsoncount + numstrcount;		

		if(smokenum+ccnum>0){
			jsondata[jsoncount] = ',';              
			++jsoncount;                           //完成"curr"(不使用编号)的写入
		}
	}

	if(ccnum>0) {
		strncat(jsondata,CC_5,5);             //"cc"
		jsoncount = jsoncount + 5;

#if IS_BCU_ROLE		
		uccstate = ccstatelist[bmucount];       //对应编号BMU的CC状态
#endif
#if IS_BMU_ROLE
		uccstate = ccstate;                 //本BMU的CC状态
#endif
		
		numstrcount = sprintf(numstr,"%d",uccstate);
		strncat(jsondata,numstr,numstrcount);     //将cc状态的值复制到json中
		jsoncount = jsoncount + numstrcount;				
		
		if(smokenum>0){
			jsondata[jsoncount] = ',';              
			++jsoncount;                           //完成"cc"的写入
		}
	}
	
	if(smokenum>0) {                         //"smoke"(此处的smokenum表示发送该BMU下的多少个烟雾数据)
			
		strncat(jsondata,SMOKE_9,9);
		jsoncount = jsoncount + 9;		

		for(loop_i=0;loop_i<smokenum;++loop_i) {  //写入smokenum个烟雾数据(0~BMU_SMOKEDATA_NUM)
			jsondata[jsoncount] = '{';              
			++jsoncount; 				
			
			strncat(jsondata,ID_6,6);              //"id"(BCU系统中的烟雾ID)
			jsoncount = jsoncount + 6;
			numstrcount = sprintf(numstr,"%d",bmucount*BMU_SMOKE_NUM+loop_i+1);
			strncat(jsondata,numstr,numstrcount);  //将id的值复制到json中
			jsoncount = jsoncount + numstrcount;		
			jsondata[jsoncount] = '"';              
			++jsoncount; 
			jsondata[jsoncount] = ',';              
			++jsoncount; 	                         //完成"id"的写入		
			
			strncat(jsondata,VAL_6,6);             //"val"
			jsoncount = jsoncount + 6;		
#if IS_BCU_ROLE
			//fdata = smokef[bmucount][loop_i];  //BCU下对应BMU的烟雾数据
#endif
#if IS_BMU_ROLE
			fdata = smokef[loop_i];  //本BMU的烟雾数据
#endif
			numstrcount = sprintf(numstr,"%.2f",fdata);  //烟雾数据值传送两位小数
			strncat(jsondata,numstr,numstrcount);  //将烟雾数据的值复制到json中
			jsoncount = jsoncount + numstrcount;				
			jsondata[jsoncount] = '}';              
			++jsoncount;                           //完成"val"的写入   			
			
			if(loop_i<smokenum-1) {                 //前smokenum-1个结束后需要跟','
				jsondata[jsoncount] = ',';              
				++jsoncount;		
			}			
		}
		jsondata[jsoncount] = ']';              
		++jsoncount;	
	}
	jsondata[jsoncount] = '}';              
	++jsoncount;   	
	
	//通过485串口发送该json格式数据
	RS485_DTU_SendData(jsondata,jsoncount);	
	
	return 1;
}

/*
功能:通过DTU上传在该BCU下采集到的数据
参数:
- currnum:要上传的BCU电流数据,需要上传为1(只需要上传1个电流值),不需要上传为0
- soctotalnum:要上传的总体SOC状态数据,需要上传为1(只需要上传1个SOC值),不需要上传为0
- insunum:要上传的绝缘状态数据,需要上传为1(只需要上传1个绝缘数据),不需要上传为0
- voltnum:要上传的总压数据,需要上传为1~3(每个BCU最多3个总压数据),不需要上传为0
返回值:1:传入参数均在正确范围内,0:传入参数范围不正确
*/
#if IS_BCU_ROLE
u8 RS485_DTU_UploadBCUData(u8 currnum, u8 soctotalnum, u8 insunum, u8 voltnum)
{
	char jsondata[300];      //要发送的json格式数据
	uint16_t jsoncount = 0;  //要发送多少个字节,当前jsondata数组存储的有效字节数,也即下一个字节存放的下标地址
	char numstr[12];         //保存整数或小数转换成字符串之后的结果
	uint8_t numstrcount;     //保存转换成的字符串的字节数
	u8 loop_i;               //用于循环处理计数	
	float fdata;             //用于转存温度值
	
	if(currnum>1||soctotalnum>1||insunum>2||voltnum>3||(currnum+soctotalnum+insunum+voltnum)==0)  //传入参数不符合要求
		return 0;
	
	memset(jsondata,0,300);
	jsondata[0] = '{';
	++jsoncount;		
	
	strncat(jsondata,DEVICEBCU_15,15);        //"device"为BCU,上传的是BCU产生的数据 
	jsoncount = jsoncount + 15;               
	strncat(jsondata,BCUID_9,9);              //"bcuId"
	jsoncount = jsoncount + 9;
	numstrcount = sprintf(numstr,"%d",My_Id);
	strncat(jsondata,numstr,numstrcount);     //将自身bcuId的值复制到json中
	jsoncount = jsoncount + numstrcount;
	jsondata[jsoncount] = '"';
	++jsoncount;
	jsondata[jsoncount] = ',';              
	++jsoncount;                              //完成bcuid的写入	

	if(currnum>0) {                           //"curr"
		strncat(jsondata,CURR_7,7);             
		jsoncount = jsoncount + 7;
		fdata = currentf;    //自身BCU的电流值,全局变量,通过电表获取
		numstrcount = sprintf(numstr,"%.3f",fdata);  //电流数据传送3位小数
		strncat(jsondata,numstr,numstrcount);  //将的值复制到json中
		jsoncount = jsoncount + numstrcount;
		if(soctotalnum+insunum+voltnum>0){          //后续还有其他类型数据
			jsondata[jsoncount] = ',';              	
			++jsoncount;                         //完成"curr"的写入
		}
	}
	
	if(soctotalnum>0) {                      //"soctotal"
		strncat(jsondata,SOC_6,6);
		jsoncount = jsoncount + 6;	
		fdata = soctotal;    //自身BCU下的整体电池组的SOC状态,全局变量,计算得到
		numstrcount = sprintf(numstr,"%.3f",fdata);  //SOC状态数据传送3位小数
		strncat(jsondata,numstr,numstrcount);  //将的值复制到json中
		jsoncount = jsoncount + numstrcount;
		if(insunum+voltnum>0){                 //后续还有其他类型数据
			jsondata[jsoncount] = ',';              	
			++jsoncount;                         //完成"soctotal"的写入
		}
	}
	
	if(insunum>0) {                          //"insu"
		strncat(jsondata,INSU_8,8);
		jsoncount = jsoncount + 8;

		for(loop_i=0;loop_i<insunum;++loop_i){ //写入voltnum个总压数据(1~3)
			jsondata[jsoncount] = '{';              
			++jsoncount; 

			strncat(jsondata,ID_6,6);            //"id"
			jsoncount = jsoncount + 6;
			numstrcount = sprintf(numstr,"%d",loop_i+1);
			strncat(jsondata,numstr,numstrcount);  //将id的值复制到json中
			jsoncount = jsoncount + numstrcount;		
			jsondata[jsoncount] = '"';              
			++jsoncount; 
			jsondata[jsoncount] = ',';              
			++jsoncount; 	                       //完成"id"的写入				
			
			strncat(jsondata,VAL_6,6);           //"val"
			jsoncount = jsoncount + 6;		
			
			if(loop_i==0){
				fdata = insuval_pos;            //BCU下的正极绝缘数据
			}
			if(loop_i==1){
				fdata = insuval_neg;            //BCU下的负极绝缘数据
			}
			
			numstrcount = sprintf(numstr,"%.2f",fdata); //绝缘状态数据值传送两位小数
			strncat(jsondata,numstr,numstrcount);  //将绝缘状态数据的值复制到json中
			jsoncount = jsoncount + numstrcount;				
			jsondata[jsoncount] = '}';              
			++jsoncount;                           //完成"val"的写入   			
			
			if(loop_i<insunum-1) {                 //前voltnum-1个结束后需要跟','
				jsondata[jsoncount] = ',';              
				++jsoncount;		
			}
		}			
		jsondata[jsoncount] = ']';              
		++jsoncount;				
		
		if(voltnum>0){                         //后续还有其他类型数据
			jsondata[jsoncount] = ',';              	
			++jsoncount;                         //完成"insu"的写入
		}
	}
	
	if(voltnum>0) {                          //"volt"
		strncat(jsondata,VOLT_8,8);
		jsoncount = jsoncount + 8;			
		
		for(loop_i=0;loop_i<voltnum;++loop_i){ //写入voltnum个总压数据(1~3)
			jsondata[jsoncount] = '{';              
			++jsoncount; 				
			
			strncat(jsondata,ID_6,6);            //"id"
			jsoncount = jsoncount + 6;
			numstrcount = sprintf(numstr,"%d",loop_i+1);
			strncat(jsondata,numstr,numstrcount);  //将id的值复制到json中
			jsoncount = jsoncount + numstrcount;		
			jsondata[jsoncount] = '"';              
			++jsoncount; 
			jsondata[jsoncount] = ',';              
			++jsoncount; 	                       //完成"id"的写入		
			
			strncat(jsondata,VAL_6,6);           //"val"
			jsoncount = jsoncount + 6;		

			fdata = voltotal[loop_i];  //BCU下的总压数据

			numstrcount = sprintf(numstr,"%.2f",fdata); //总压数据值传送两位小数
			strncat(jsondata,numstr,numstrcount);  //将总压数据的值复制到json中
			jsoncount = jsoncount + numstrcount;				
			jsondata[jsoncount] = '}';              
			++jsoncount;                           //完成"val"的写入   			
			
			if(loop_i<voltnum-1) {                 //前voltnum-1个结束后需要跟','
				jsondata[jsoncount] = ',';              
				++jsoncount;		
			}			
		}
		jsondata[jsoncount] = ']';              
		++jsoncount;		
	}

	jsondata[jsoncount] = '}';              
	++jsoncount;   	
	
	//通过485串口发送该json格式数据
	RS485_DTU_SendData(jsondata,jsoncount);	
	
	return 1;
	
	
	
}
#endif





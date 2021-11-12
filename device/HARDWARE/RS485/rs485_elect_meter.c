 
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "rs485_elect_meter.h"
#include "usart.h"
#include "bms_state.h"
#include "delay.h"

//4852_����жϽ���������ر�������
uint8_t RS4852_RX_BUF[RS4852_REC_LEN];  //RS485_ElectMeter���ջ�����
uint8_t RS4852_RX_LEN;  //RS485_ElectMeter��������֡�ܳ���
uint16_t RS4852_RX_STA;  //RS485_ElectMeter����״̬���

uint8_t RS4852_RX_ADDR[6];  //RS485_ElectMeter��������֡��ͨ�ŵ�ַ(����洢�и��ֽڵ�ַ��ǰ)
uint8_t RS4852_RX_CTRLC;  //RS485_ElectMeter��������֡�Ŀ�����
uint8_t RS4852_RX_DATALEN;  //RS485_ElectMeter��������֡�����ݳ���
uint8_t RS4852_RX_DATA[20];  //RS485_ElectMeter��������(��ͬ��������֡���ݸ�ʽ��ͬ)
uint8_t RS4852_RX_CS;  //RS485_ElectMeter�������ݵ�У����


//RS485_Electricity_Meter��ʼ������
//���ô���USART2��485ת��ģ��CON����PD6
void RS485_ElectMeter_Init(u32 bound) {
	
  //Ŀǰ�������:������9600,����żУ��λ
	
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
	
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2
	
	//USART2�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2,PA3
	
   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//����żУ��λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
	USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
	
	//USART_ClearFlag(USART2, USART_FLAG_TC);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�
	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;  //��ռ���ȼ�6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	//����PD6(BCU�ĵ��485ͨ��ģ���������)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //����
	GPIO_Init(GPIOD, &GPIO_InitStructure);  //��ʼ��
	GPIO_SetBits(GPIOD,GPIO_Pin_6);
	
	RS4852_RX_STA = 0;  //��ʼ��RS4852���ڽ���״̬����
}


/*
����:���ģ��485�������
����:�����ڵ������ֲ���,���ַ�������ʱ,ÿ�����ֻ�����244�ֽ�;
*/
void RS485_ElectMeter_SendData(const uint8_t *data, uint8_t len){
	uint8_t i =0;
	RS485_RE2_OFF;
	//�˴�ֱ�Ӳ����Ĵ������ܻ����һЩĪ������Ĵ���
	for(i=0; i<len; i++){
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		USART_SendData(USART2,data[i]);
	}
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET) ;
	RS485_RE2_ON;
}


/*
���ܣ��������֡��Ϣ
������6�ֽڵ�ַ,1�ֽڿ�����,1�ֽ����ݳ���,len�ֽ�����
			6�ֽڵ�ַ�������Ϊ: ��λ�ֽ���ǰ, ��λ�ֽ��ں�, �뷢��֡��ַ��ʽ��ͬ
*/
void RS485_ElectMeter_SendFrame(u8 *addr,u8 ctrlcode,u8 len,u8 *data) {
	u8 frame[215];
	u8 i;
	u8 checkcode;
	
	//֡��ʼ��
	frame[0] = 0x68;
	//��ַ��(6���ֽ�)
	for(i=0;i<6;i++) {
		frame[1+i] = addr[i];
	}
	//֡��ʼ��
	frame[7] = 0x68;
	//������
	frame[8] = ctrlcode;
	//���ݳ�����
	frame[9] = len;
	//������
	for(i=0;i<len;i++) {
		frame[10+i] = data[i] + 0x33;
	}
	//У����(�ӵ�һ��֡��ʼ����ʼ��У����֮ǰ�����и��ֽڵ�ģ256�ĺ�)
	// (Ӧ�����ȵõ���33H֮�������֡,Ȼ�����õ�У����)
	checkcode = 0;
	for(i=0;i<10+len;i++) {
		checkcode += frame[i];
	}
	frame[10+len] = checkcode;
	//������
	frame[10+len+1] = 0x16;
	
	//����֡��Ϣ(Ӧ��ʹ��USART2)
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

//�����ܱ�ͨ�ŵ�ַ(��֧�ֵ�Ե�ͨ��)
void RS485_ElectMeter_UpdateAddr(void) {
	//��ַ��:AA...AA, ������C=13H, ���ݳ���L=00H,
	u8 addr[6] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
	u8 data[1];
	u8 i;
	RS485_ElectMeter_SendFrame(addr,0x13,0,data);
	
	//delay_ms(10); //��ʱ
	while(!(RS4852_RX_STA&0x8000)) {
		delay_us(100);
	}
	
	//���µ�ַ��Ϣ��ȫ�ֱ���em_addr[6]��(��λ��ǰ��λ�ں�)
	for(i=0;i<6;i++) {
		em_addr[i] = RS4852_RX_BUF[5+i];
	}
	
	RS4852_RX_STA = 0;  //���4852���ڽ��ձ�ʶ��
}

/*
����:485����ͨ�Ŷ����ܱ��������
����:��,ͨ��RS4852_RX_BUF��RS4852_RX_STA��current��currentf��ȫ�ֱ����������ݹ���
����ֵ:1,��������
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
	
	RS485_ElectMeter_SendFrame(em_addr,0x11,0x04,data);  //���͵�����������֡
	
//	//delay_ms(10); //��ʱ20ms
//	timecount = 0;
//	while((!(RS4852_RX_STA&0x8000))||timecount>200) {
//		delay_us(100);
//		++timecount;
//	}
//	
//	//���µ�����Ϣ��ȫ�ֱ�����(�±�0��ʼ4���ֽ���0xFE)
//	current[0] = RS4852_RX_BUF[21] - 0x33;  //��������(BCD�����λ)(˵������˵���λΪ����λ,�������λ��1���ֽڻ���1������?)
//	current[1] = RS4852_RX_BUF[20] - 0x33;  //��������
//	current[2] = RS4852_RX_BUF[19] - 0x33;  //С������
//	current[3] = RS4852_RX_BUF[18] - 0x33;  //С������(BCD�����λ)
//	
//	//4�ֽ�BCD��ת��Ϊfloat��������currentf
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
//	if(current[0]&0x80){  //�˴���Ϊ���յ�������ֽڵ����λΪ����λ
//		currentf = -tmp;
//	}	
//	else
//		currentf = tmp;
//	
//	RS4852_RX_STA = 0;  //���4852���ڽ��ձ�ʶ��
	
	return 1;
}

/*
����:������ƥ����ַ������պ���,��DL/T645-2007Э���֡������RS4852_RX_BUF[]������
�������ֱ�����жϷ�������,��ʶ���ǵ���������Ϣ��,ֱ��ת�����洢��ֵ
*/
void USART2_IRQHandler(void) {
	u8 i,ii,j;
	u8 checkcode = 0;
	u8 framelen = 0;
	u8 Res;  //�����ַ�
	float tmpcur;
	
	//�ж�������ʼ��ʶ��,��¼���豸��6�ֽڵ�ַ,��֡��Ϣ��¼��RS4852_RX_BUF[]�����ں�������
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //�������ݼĴ����ǿ��ж�
		Res = USART_ReceiveData(USART2);  //���յ����ֽ�
		i = RS4852_RX_STA & 0x7FFF;
		RS4852_RX_BUF[i++] = Res;
		
		if(Res==0x16)  //���յ���֡��������־(����һ������֡����)
		{
			ii = i;
			if((ii>13)&&(ii>(RS4852_RX_BUF[13]+15))) { 
				//�Ѿ����ٽ�����14���ֽ����� && �Ѿ�����������֡�������ֽ�,��ʱ��Resֻ���ڽ�������λ��	
				
				/*
				framelen = RS4852_RX_BUF[13]+10;
				for(ii=0;ii<framelen;ii++) {
					checkcode = checkcode + RS4852_RX_BUF[4+ii];
				}
				if(checkcode==RS4852_RX_BUF[framelen+4]) {  //��֤У����  
				
				*/
				RS4852_RX_STA = i|0x8000;  //��������Ϣ֡
					
				//����ǵ���������Ϣ,ֱ�����жϷ��������ת�����洢��ֵ
				if((RS4852_RX_BUF[12]==0x91)){
					
					//��ӡ����
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
					if((RS4852_RX_BUF[21]-0x33)&0x80){  //�˴���Ϊ���յ�������ֽڵ����λΪ����λ
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
			RS4852_RX_STA = i;  //δ����������֡,�Ѿ�������i���ֽڵ�����,��ΧΪRS4852_RX_BUF[0-(i-1)]
		}
	}
}
	
	





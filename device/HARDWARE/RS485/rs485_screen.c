
#include "bms_state.h"
#include "rs485_screen.h"
#include "usart.h"
#include "stdlib.h"

//��ر�������
uint8_t RS4856_RX_BUF[RS4856_REC_LEN];  //RS485_Screen���ջ�����
uint8_t RS4856_RX_LEN;
uint8_t RS4856_RX_STA;  //RS485_Screen����״̬���




//RS485_Screen��ʼ������
//���ô���USART6��485ת��ģ��CON����PG7
void RS485_Screen_Init(u32 bound) {
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); //GPIOG9����ΪUSART6
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //GPIOG14����ΪUSART6	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; //GPIOG9��GPIOG14
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG,&GPIO_InitStructure); //��ʼ��PG9|PG14	
	
	//USART��������
	USART_InitStructure.USART_BaudRate = bound;//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART6, &USART_InitStructure);

	USART_Cmd(USART6, ENABLE);  //ʹ��

	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//�����ж�
	//USART1 NVIC 
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;  //��ռ���ȼ�5
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	
	
	
	
	//����PG7(BCU�Ĵ�����ͨ��485ģ���������)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //����
	GPIO_Init(GPIOG, &GPIO_InitStructure);  //��ʼ��
	GPIO_ResetBits(GPIOG,GPIO_Pin_7);

	RS485_RE4_ON;

	RS4856_RX_LEN = 0;
}


void RS485_Screen_SendData(const uint8_t *data, uint8_t len){
  uint8_t i =0;
  RS485_RE4_OFF;
  for(i=0; i<len; i++){
		while(USART_GetFlagStatus(USART6,USART_FLAG_TC)!=SET);
		USART_SendData(USART6,data[i]);
  }
	while(USART_GetFlagStatus(USART6,USART_FLAG_TC)!=SET);
  RS485_RE4_ON;
}
	

/*�������ʾ����ƥ����ַ������չ���:
	���ڶ�ȡ�Ĵ����Ͷ�ȡ�����洢���в�ͬ��Ӧ��ʽ��
		- ��ȡ�Ĵ���: Ӧ��Ϊ �Ĵ�����ַ(0x00-0xFF)+�ֽ����ݳ���+��ȡ�ļĴ�������, Ӧ�����������257���ֽ�,1+1+255;
		- ��ȡ�����洢��: Ӧ��Ϊ �����洢����ַ(0x0000-0x6FFF)+���������ֳ���+��ȡ�ı�������, Ӧ�����������257���ֽ�,1*2+1+127*2;
	���жϷ�����,���ձ����洢����������Ӧ���ʽ���б�д;
*/
/* USART1�жϷ�����
void USART1_IRQHandler(void) {
	u8 i;
	
  if((USART1->SR)&(1<<5)){
    RS485_SCREEN_RX_BUF[RS485_SCREEN_RX_LEN++]=USART1->DR;
  }
  if((USART1->SR)&(1<<4)){
    RS485_SCREEN_RX_BUF[RS485_SCREEN_RX_LEN]=0;  //�ַ���������־
		USART1->SR;
		USART1->DR;
	}
	
	for(i=0;i<RS485_SCREEN_RX_LEN;i++){
		printf("%x ",RS485_SCREEN_RX_BUF[i]);
	}
	
}
*/

void RS485_SCREEN_ReadData(uint8_t *data, uint8_t *len){
	uint8_t i;
	*len = RS4856_RX_LEN;
	for(i=0;i<RS4856_RX_LEN;i++){
		data[i] = RS4856_RX_BUF[i];
	}
	RS4856_RX_LEN = 0;
	RS4856_RX_BUF[RS4856_RX_LEN] = 0;
}	


/*
	����DGUS���Ĵ�������֡��5�����ݿ飺
	| ���ݿ� |   1   |     2     |   3   |   4   |   5   |
	|  ����  | ֡ͷ  | ���ݳ���  | ָ��  | ����  |  CRC  |
	|���ݳ���|   2   |     1     |   1   |   N   |   2   |
*/


/*
����:��ָ���洢����ַд���������(�ݼ��趼��2�ֽ�����)
����:addr:16λ�洢����ַ,data:Ҫ��ʾ������;
����ֵ:��
ע:�ڱ��ļ��в��ٷ�װ���Ӧ����ʾ��λ��д�����ݵĺ���,���еĸ�����ʾ���ݵĹ��ܶ����������ڲɼ�����Ӧ���ݺ�����ʵ��(���ͷ�ļ��еı�����ַ��Ϣ)
*/
void RS485_Screen_WriteMem(uint16_t addr, uint16_t data) {
	uint8_t frame[10];  //����֡
	uint8_t len = 8;      //����֡����
	
	//֡ͷ
	frame[0] = 0x5A;
	frame[1] = 0xA5;
	//���ݳ���
	frame[2] = len - 3;  //ָ��+���ݵĳ���
	//ָ��
	frame[3] = 0x82;  //�����洢��дָ��
	//����
	frame[4] = 0xFF & (addr>>8);
	frame[5] = 0xFF & (addr>>0);
	
	//���ֽ���ǰ
	frame[6] = 0xFF & (data>>8);
	frame[7] = 0xFF & (data>>0);	
	
	RS485_Screen_SendData(frame,len);
	
}


/*����:��ȡָ����ش���ѹ���ݵ���ʾ����ַ
����:��ش��ı��(��ΧӦΪ0~CELL_NUM-1,ĿǰCELL_NUMֵΪ240
����ֵ:��Ӧ����ʾ����ַ
*/
u16 get_celladdr(u16 cellnum){
	u16 addrbase = 0;

	switch(cellnum/24){  //�õ�ش���ѹ�������ڵĲɼ������
		case 0: addrbase = VOLCARD1_VOL1_ADDR;
			break;
		case 1: addrbase = VOLCARD2_VOL1_ADDR;
			break;	
		case 2: addrbase = VOLCARD3_VOL1_ADDR;
			break;
		case 3: addrbase = VOLCARD4_VOL1_ADDR;
			break;
		case 4: addrbase = VOLCARD5_VOL1_ADDR;
			break;
		case 5: addrbase = VOLCARD6_VOL1_ADDR;
			break;
		case 6: addrbase = VOLCARD7_VOL1_ADDR;
			break;
		case 7: addrbase = VOLCARD8_VOL1_ADDR;
			break;
		case 8: addrbase = VOLCARD9_VOL1_ADDR;
			break;
		case 9: addrbase = VOLCARD10_VOL1_ADDR;
			break;		
		default:
			return VOLCARD1_VOL1_ADDR;
	}
	return addrbase + 2*(cellnum%24);
	
}


/*����:��ȡָ���¶ȼ������ݵ���ʾ����ַ
����:�¶ȼ���ı��
����ֵ:��Ӧ����ʾ����ַ
*/
u16 get_tempaddr(u16 tempnum){
	
	u16 addrbase = 0;

	switch(tempnum/12){  //�õ�ش���ѹ�������ڵĲɼ������
		case 0: addrbase = VOLCARD1_TEMP1_ADDR;
			break;
		case 1: addrbase = VOLCARD2_TEMP1_ADDR;
			break;	
		case 2: addrbase = VOLCARD3_TEMP1_ADDR;
			break;
		case 3: addrbase = VOLCARD4_TEMP1_ADDR;
			break;
		case 4: addrbase = VOLCARD5_TEMP1_ADDR;
			break;
		case 5: addrbase = VOLCARD6_TEMP1_ADDR;
			break;
		case 6: addrbase = VOLCARD7_TEMP1_ADDR;
			break;
		case 7: addrbase = VOLCARD8_TEMP1_ADDR;
			break;
		case 8: addrbase = VOLCARD9_TEMP1_ADDR;
			break;
		case 9: addrbase = VOLCARD10_TEMP1_ADDR;
			break;		
		default:
			return VOLCARD1_TEMP1_ADDR;
	}
	return addrbase + 2*(tempnum%12);
	
}








































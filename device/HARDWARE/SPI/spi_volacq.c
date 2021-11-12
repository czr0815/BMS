#include "spi_volacq.h"
#include "math.h"
#include "LTC6804.h"
#include "delay.h"


//��SPI1���г�ʼ������,����Ϊ����ģʽ
void SPI1_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	SPI1_Control_Init();  //LTC6804оƬ�Ŀ������ų�ʼ������
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//ʹ��SPI1ʱ��

	//GPIOFA5,6,7��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//PA5~7���ù������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); //PA5����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); //PA6����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); //PA7����Ϊ SPI1
 
	//����ֻ���SPI�ڳ�ʼ��
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//��λSPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//ֹͣ��λSPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����

	SPI1_ReadWriteByte(0xff);//��������		 
}   

//LTC6804оƬ��SPI1�������ų�ʼ������
void SPI1_Control_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;  //|GPIO_Pin_10;//PF6-10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��GPIOF6-9
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3;  //PC0,PC2-3
//	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC0
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;  //PA0-1
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOC0	
	
	//�ر�4��Ƭѡ
	GPIO_SetBits(GPIOF,GPIO_Pin_6);
	GPIO_SetBits(GPIOF,GPIO_Pin_7);
	GPIO_SetBits(GPIOF,GPIO_Pin_8);
	GPIO_SetBits(GPIOF,GPIO_Pin_9);
//	GPIO_SetBits(GPIOF,GPIO_Pin_10);
//	GPIO_SetBits(GPIOC,GPIO_Pin_0);
//	GPIO_SetBits(GPIOC,GPIO_Pin_2);
//	GPIO_SetBits(GPIOC,GPIO_Pin_3);
//	GPIO_SetBits(GPIOA,GPIO_Pin_0);
//	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	
}

//SPI1�ٶ����ú���
//SPI�ٶ�=fAPB2/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2ʱ��һ��Ϊ84Mhz��
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
	SPI1->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
	SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	SPI_Cmd(SPI1,ENABLE); //ʹ��SPI1
} 

//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ��byte  ����
	
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
 
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	
 		    
}


/*
��ȡ��ص�ѹ���ݡ��¶����ݡ����þ���״̬,����BMU����ʵ�ֵĹ���
*/
/*
����:��LTC6804оƬģ����������ת��Ϊ�¶�����
����:ģ���ѹֵ,��ѹ�ο�ֵ
����ֵ:float���͵������϶ȡ�Ϊ��λ���¶�ֵ
*/
float temp_data(u16 adcv,u16 vref)
{
  //���ù̶�����10k,��������25���϶�ʱ����Ϊ100k
   float Rt=0;
   float Rp=100000;   //��������25���϶�ʱ����Ϊ100k
   float T2=273.15+25;
   float Bx=3950;     //Rp=100kʱ
   //float Bx=3435;   //Rp=10kʱ
   float Ka=273.15;
   float vol=0;
	 float temp,vreff;
	 vreff=(float)(vref);
   vol=(float)(adcv);
   Rt=(vreff-vol)*10000/vol;   //�̶�������ֵ10k
	// printf("%f %f %f %f \r\n",vreff,vol,Rt,log((double)(Rt/Rp)));
   temp=1/(1/T2+log((double)(Rt/Rp))/Bx)-Ka;
	 return temp;
}
#if IS_BMU_ROLE
/*
����:��ȡ��chip��оƬ�ϵĵ����i�ص�ѹ����,�����䱣�浽BMU��ȫ�ֱ���cellvol[][]��	
����:BMU�����ӵ�LTC6804оƬ���(��Χ0~3)
����ֵ:0Ϊ���뷶Χ����,1Ϊ�ɹ�
*/
u8 readvol(uint8_t chip)
{
	uint16_t tmp_cellvol[1][12];  //��ʱ�����ȡ���ĵ�ѹֵ
	u8 i;
	
	if(chip>=BMU_VOLCHIP_NUM) //����4����ѹ�ɼ�оƬʱchip��ȷ��ΧΪ0-3
		return 0;
	
	LTC6804_adcv(chip);  //������ѹת��
	LTC6804_rdcv(chip, 0, 1, tmp_cellvol);   //��ȡ������ѹ�Ĵ���
	
	for(i=0;i<12;i++){
		cellvol[chip*12+i] = tmp_cellvol[0][i];  //�ֲ�������ֵ��ֵ��ȫ�ֱ���
	}
	
	return 1;
}

/*
����:��ȡ��chip��оƬ�ϵ�GPIO�ڵ�ģ������,��ȡ��Ӧ���¶ȴ���������,�����䱣�浽ȫ�ֱ���temp[0][][]��
����:BMU�����ӵ�LTC6804оƬ���(��Χ0~4)
����ֵ:0Ϊ���뷶Χ����,1Ϊ�ɹ�
*/
u8 readtemp(uint8_t chip)
{
	uint16_t tmp_temp[1][6];  //��ʱ�����ȡ����GPIOģ������ֵ(6��ֵ,ǰ5��ΪGPIO�˿ڵ�ѹֵ)
	u8 i;
	
	if(chip>=BMU_VOLCHIP_NUM) //����4����ѹ�ɼ�оƬʱchip��ȷ��ΧΪ0-3
		return 0;
	
	LTC6804_adax(chip);
	LTC6804_rdaux(chip, 0, 1, tmp_temp);
	//delay_ms(5);
	
	for(i=0;i<4;i++){  //��6��Ϊ4,��ÿ��оƬ�ɼ�4���¶�����(��1-4���¶�)
		temp[chip*4+i] = tmp_temp[0][i];
		tempf[chip*4+i] = temp_data(tmp_temp[0][i],tmp_temp[0][5]);
	}
	
	return 1;
}

//����(s=1)/�ر�(s=0)��chip��оƬ�ϵĵ�celln�����ģ��ľ��⹦��
u8 set_batteryequ(uint8_t chip,uint8_t celln,uint8_t state)
{
	

	
	return 0;
}
	
#endif

#include "spi_volacq.h"
#include "math.h"
#include "LTC6804.h"
#include "delay.h"


//对SPI1进行初始化设置,配置为主机模式
void SPI1_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	SPI1_Control_Init();  //LTC6804芯片的控制引脚初始化配置
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI1时钟

	//GPIOFA5,6,7初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//PA5~7复用功能输出	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); //PA5复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); //PA6复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); //PA7复用为 SPI1
 
	//这里只针对SPI口初始化
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设

	SPI1_ReadWriteByte(0xff);//启动传输		 
}   

//LTC6804芯片的SPI1控制引脚初始化配置
void SPI1_Control_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;  //|GPIO_Pin_10;//PF6-10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIOF6-9
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3;  //PC0,PC2-3
//	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC0
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;  //PA0-1
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOC0	
	
	//关闭4个片选
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

//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); //使能SPI1
} 

//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据
	
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
 		    
}


/*
读取电池电压数据、温度数据、设置均衡状态,均是BMU可以实现的功能
*/
/*
功能:将LTC6804芯片模拟输入数据转换为温度数据
参数:模拟电压值,电压参考值
返回值:float类型的以摄氏度℃为单位的温度值
*/
float temp_data(u16 adcv,u16 vref)
{
  //采用固定电阻10k,热敏电阻25摄氏度时电阻为100k
   float Rt=0;
   float Rp=100000;   //热敏电阻25摄氏度时电阻为100k
   float T2=273.15+25;
   float Bx=3950;     //Rp=100k时
   //float Bx=3435;   //Rp=10k时
   float Ka=273.15;
   float vol=0;
	 float temp,vreff;
	 vreff=(float)(vref);
   vol=(float)(adcv);
   Rt=(vreff-vol)*10000/vol;   //固定电阻阻值10k
	// printf("%f %f %f %f \r\n",vreff,vol,Rt,log((double)(Rt/Rp)));
   temp=1/(1/T2+log((double)(Rt/Rp))/Bx)-Ka;
	 return temp;
}
#if IS_BMU_ROLE
/*
功能:读取第chip个芯片上的单体电i池电压数据,并将其保存到BMU的全局变量cellvol[][]中	
参数:BMU上连接的LTC6804芯片编号(范围0~3)
返回值:0为输入范围错误,1为成功
*/
u8 readvol(uint8_t chip)
{
	uint16_t tmp_cellvol[1][12];  //暂时保存读取到的电压值
	u8 i;
	
	if(chip>=BMU_VOLCHIP_NUM) //板载4个电压采集芯片时chip正确范围为0-3
		return 0;
	
	LTC6804_adcv(chip);  //开启电压转换
	LTC6804_rdcv(chip, 0, 1, tmp_cellvol);   //读取解析电压寄存器
	
	for(i=0;i<12;i++){
		cellvol[chip*12+i] = tmp_cellvol[0][i];  //局部变量数值赋值给全局变量
	}
	
	return 1;
}

/*
功能:读取第chip个芯片上的GPIO口的模拟数据,获取对应的温度传感器数据,并将其保存到全局变量temp[0][][]中
参数:BMU上连接的LTC6804芯片编号(范围0~4)
返回值:0为输入范围错误,1为成功
*/
u8 readtemp(uint8_t chip)
{
	uint16_t tmp_temp[1][6];  //暂时保存读取到的GPIO模拟数据值(6个值,前5个为GPIO端口电压值)
	u8 i;
	
	if(chip>=BMU_VOLCHIP_NUM) //板载4个电压采集芯片时chip正确范围为0-3
		return 0;
	
	LTC6804_adax(chip);
	LTC6804_rdaux(chip, 0, 1, tmp_temp);
	//delay_ms(5);
	
	for(i=0;i<4;i++){  //由6改为4,即每个芯片采集4个温度数据(第1-4个温度)
		temp[chip*4+i] = tmp_temp[0][i];
		tempf[chip*4+i] = temp_data(tmp_temp[0][i],tmp_temp[0][5]);
	}
	
	return 1;
}

//开启(s=1)/关闭(s=0)第chip个芯片上的第celln个电池模块的均衡功能
u8 set_batteryequ(uint8_t chip,uint8_t celln,uint8_t state)
{
	

	
	return 0;
}
	
#endif

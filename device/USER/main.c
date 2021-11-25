#include "stm32f4xx.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sys.h"
#include "stm32f4xx_conf.h"

#include "bms_state.h"
#include "LTC6804.h"
#include "timer.h"
#include "delay.h"
#include "usart.h"
#include "can.h"
#include "extern_can.h"
#include "BMS.h"
#include "Charger.h"
#include "rs485_DTU.h"
#include "rs485_screen.h"
#include "rs485_elect_meter.h"
#include "rs485_insulation.h"
#include "input_output_ctrl.h"
#include "spi_volacq.h"
#include "stdlib.h"
#include "math.h"
#include "adc_power.h"
#include "adc_cc.h"
#include "iic_fun.h"
#include "adc_current.h"
#include "adc_hv.h"
#include "soc_Vmin_EKF.h"
#include "faultalarm.h"
#include "voltbalance.h"
#include "stmflash.h"
#include "sdio_sdcard.h"
#include "quick_sort.h"

//在【使用系统】的情况下,任务创建函数声明
//BMU和BCU共用任务及任务句柄
void start_task(void);            //创建开启任务
void timerecord_task(void);       //【0.1】创建记录运行时间&闪烁指示灯任务
void pcprint_task(void);          //【0.2】创建串口打印任务
void DTU_task(void);              //【0.3】创建DTU数据上传任务(485通信1)
TaskHandle_t vStartTaskHandler;              //开启任务句柄
TaskHandle_t vTimeRecordTaskHandler;         //记录运行时间&闪烁指示灯任务句柄
TaskHandle_t vPcPrintTaskHandler;            //串口与PC上位机交互控制任务句柄
TaskHandle_t vDTUTaskHandler;                //DTU数据上传任务句柄

//BMU任务及任务句柄
#if IS_BMU_ROLE
void bmu_dataacq_task(void);      //【1.1】创建BMU数据采集任务
void bmu_heartbeat_task(void);         //【1.2】创建BMU内部CAN通信发送心跳包任务
void bmu_uploaddata_task(void);           //【1.3】创建BMU上传数据任务
void bmu_balance_task(void);          //【1.4】创建BMU电池单元的均衡控制任务
void bmu_screen_task(void);           //【1.5】创建BMU显示数据任务
TaskHandle_t vBMUDataAcqTaskHandler;   //BMU数据采集任务句柄
TaskHandle_t vBMUHeartBeatTaskHandler;      //BMU内部CAN通信发送心跳包任务句柄
TaskHandle_t vBMUUploadDataTaskHandler;      //BMU上传数据任务句柄
TaskHandle_t vBMUBalanceTaskHandler;       //BMU电池单元的均衡控制任务句柄
TaskHandle_t vBMUScreenTaskHandler;       //BMU电池单元的均衡控制任务句柄
#endif

//BCU任务及任务句柄
#if IS_BCU_ROLE
void ccacq_task(void);            //【2.1】创建CC信号检测任务
void insulation_task(void);       //【2.2】创建绝缘状态检测任务
void hvacq_task(void);            //【2.3】创建高压状态检测任务
void poweracq_task(void);         //【2.4】创建供电电压检测任务
void currentacq_task(void);       //【2.5】创建电流数据检测任务
void bcucontrol_task(void);       //【2.6】创建BCU控制功能任务
void externcan_task(void);        //【2.7】监听外部CAN任务
TaskHandle_t vCCAcqTaskHandler;              //CC信号检测任务句柄
TaskHandle_t vInsulationTaskHandler;         //绝缘检测模块数据采集任务句柄
TaskHandle_t vHVAcqTaskHandler;              //高压状态检测任务句柄
TaskHandle_t vPowerAcqTaskHandler;           //供电电压检测任务句柄
TaskHandle_t vCurrentAcqTaskHandler;         //电流数据检测任务句柄
TaskHandle_t vBCUControlTaskHandler;         //控制功能任务句柄
TaskHandle_t vExternCanTaskHandler;          //监听外部CAN任务句柄

//BCU控制功能任务的各部分函数声明
void bcu_can1_ctrlandacq(void);    //【2.6.1】控制各个从控,获取对应单体数据(控制上传数据,若一段时间没有回应则跳过,并标记为失联,节省时间且检测了状态)
void datacompare(void);            //【2.6.2】比较单体数据,获得最大/最小数据
void soccalculate(void);           //【2.6.3】估算当前总体SOC状态
void faultalarm(void);             //【2.6.4】故障检测(各个故障类型依次检测一遍)
void historydatastore(void);       //【2.6.5】历史数据记录(以一轮的一次单体数据和二次数据为准进行记录)
void screen_clear(void);           //清空显示屏内容
#endif

//暂时没用的任务及句柄
void BMSelectcontrol_task(void);  //创建BMS系统上|下电任务
void electmeter_task(void);       //创建电表数据采集任务(485通信2)
TaskHandle_t vBMSElectControlTaskHandler;    //BMS系统上|下电任务句柄
TaskHandle_t vElectMeterTaskHandler;         //电表电流数据采集任务句柄


/*
功能:工程入口
*/
int main(void)
{ 
	u8 testdata1[10] = "123456789";
	//char testdata1[500];
	uint16_t i=0;
  u16 u16data;  //测试IIC数据转换
	short int sidata;  //测试IIC数据转换
	float dataf;  //测试IIC数据转换
	u8 iic_flag;
	u8 chip = 0;
	u8 j;
	u8 bmu_loop_i,bmu_loop_j,bcu_loop_i,bcu_loop_j;  //主函数中循环
	u8 bmucelln,bmutempn,bmucount_i;
	u8 loop_i,loop_j;  //"先赋个值"循环
	float tshow;
	u16 tmp_cell_codes[1][12];
	u16 tmp_aux_codes[1][6];
	u16 tmppower;
	
	//*****************************************状态变量初始化********************************************
	My_Id = MY_ID;
  My_Did = MY_DID;
	runtime = 0;  //初始化系统运行时间
#if IS_BMU_ROLE
	My_BMS_Role = BMS_ROLE_BMU;
	My_Superior_Id = 0xFF;        //初始化为广播地址
	Is_Connected = 0;             //没有连接到上一级设备
	My_Pack_Number = My_Id;       //此处保持电池组编号和ID一致
	bmu_uploaddata = 0;           //默认不上传数据
	bmu_displaydata = 0;          //默认不显示数据
	
	//各个时间间隔变量
	dataacq_time = 2000;          //数据获取任务时间间隔(单位ms)
	heartbeat_time = 8000;    //心跳包时间间隔(单位为ms)
	dataupdate_time = 5000;   //BMU数据上传的时间间隔(单位为us)
	
#endif	
#if IS_BCU_ROLE
	My_BMS_Role = BMS_ROLE_BCU;
	bmucontroltask_delay = 5000;
	//bcucellnum = CELL_NUM;
	//bcutempnum = 25;
	
#endif
	//***************************************状态变量初始化完毕********************************************
	
	
	//*****************************************各模块功能初始化********************************************
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);        //中断优先级分组4(主优先级0~15,子优先级0)
	delay_init(100);                                       //自定义延时功能初始化
	IOControl_Init();                                      //IO控制功能初始化(包括BMS系统上下电和LED灯)
	RS485_DTU_Init(115200);                                //DTU数据上传功能初始化
	//SOC状态测算功能初始化
	CAN1_Mode_Init();                                      //BMS内部通信功能初始化
	Extern_CAN_Init();                                     //BMS对外通信功能初始化
	//故障报警功能初始化
	//历史运行数据存储功能初始化
	uart7_init(115200);                                    //串口与PC上位机交互控制功能初始化
	ADC_Inside_Init();                                     //初始化ST芯片内部ADC采样功能(暂时用于CC状态检测和供电电压测量)
	IIC_Init();                                            //IIC通信功能初始化
	SDCard_Init();
#if IS_BMU_ROLE
	LTC6804_initialize();                                  //单体电压数据、温度数据采集功能初始化
	//烟雾数据采集功能初始化
#endif
#if IS_BCU_ROLE
	RS485_Screen_Init(9600);                               //显示屏交互功能初始化
	RS485_ElectMeter_Init(9600);                           //电表电流数据采集功能初始化
	RS485_Insulation_Init(9600);                           //绝缘检测模块数据采集功能初始化
	//Show_RelayState();                                     //将继电器1/2/3的状态显示为0
#endif
	
	//BCU控制高压继电器(×3)闭合
#if IS_BCU_ROLE
	IOControl(5,IO_CLOSE);  //点亮运行指示灯
	
	
//	IOControl(3,IO_CLOSE);  //总负继电器闭合
//	IOControl(2,IO_CLOSE);  //预充继电器闭合
//	delay_ms(3000);
//	IOControl(1,IO_CLOSE);  //总正继电器闭合
	
	/*
	delay_ms(1000);
	IOControl(2,IO_DISCONNECT);  //预充继电器断开
	*/
#endif
	
  //***************************************各模块功能初始化完毕********************************************
	
	
//打印提示语句
#if IS_BMU_ROLE
	printf("BMU 0x%02X initialize completed.\r\n",My_Id);
#endif
#if IS_BCU_ROLE
	printf("BCU 0x%02X initialize completed.\r\n",My_Id);
#endif
	printf("The device is working.\r\n");
	/*
	SDCard_Write_SOC(52.36);
	printf("soc:%f\r\n", STMFlash_Read_SOC());
	STMFlash_Write_SOC(63.36);
	printf("soc:%f\r\n", STMFlash_Read_SOC());
	SDCard_Write_SOC(63.36);
	*/
	
/*	在【不使用系统】的情况下,仅作为功能模块调试使用;
*/	
#if !IS_USE_RTOS  // 不使用系统的情况
while(1) {
			
/* 不使用系统的情况下,此处仅用作BMU调试功能模块使用 */
#if IS_BMU_ROLE
			
			CAN1_ComControl(0x10,FT_COMCONTROL_HEARTBEAT);
			printf("Working...\r\n");
			GPIO_ToggleBits(GPIOD,GPIO_Pin_15);
			delay_ms(1000);
						
#endif

/* 不使用系统的情况下,此处仅用作BCU调试功能模块使用 */			
#if IS_BCU_ROLE
			
	//测试IIC通信功能
	//【注意:电流检测电路连线有问题,因此现在可能需要将电流检测的极性反转】
  //【反转:要么在代码中重设双极数据转换模式,要么在检测电流时针对这种情况反接】
	//【电流数据:浮空状态下,CH1-CH0大概在0.15xV的电压,这应该是分流器经放大25.7倍后的电压值】

	iic_flag = 0;	
			
	
	//测量电流数据
	iic_flag = LTC2305_start_convert(3, LT2305_ADDR_CURR);  //开启ADC转换
	delay_ms(1);
	u16data = LTC2305_read_result(3,LT2305_ADDR_CURR);  //读取电流数据转换结果
	sidata = (short int)u16data;  //将无符号数转换为有符号数
	//输出电流转换结果
	if(iic_flag == 0){
		printf("Current data:%d\r\n",sidata);
	}
	else{
		printf("Current data acq failed.\r\n");
	}		
			
			
			
			
			
			
			
/*	    //部分测试过程代码		
//			//通过DTU上传温度数据(25个,即BMU0的前5个芯片的数据)
//			for(bcu_loop_i=0;bcu_loop_i<5;++bcu_loop_i) {
//				RS485_DTU_UploadTempData(0, bcu_loop_i*5, (bcu_loop_i+1)*5);
//				delay_ms(20000);
//			}
//			//通过DTU上传电池数据(237个,目前240个)
//			for(bcu_loop_i=0;bcu_loop_i<237;bcu_loop_i+=5) {
//				
//				RS485_DTU_UploadVolData(bcu_loop_i/120, bcu_loop_i,bcu_loop_i+5);  //上传电池数据,5个为1组
//				
//				delay_ms(20000);
//			}
//			//通过DTU上传烟雾电流数据
//			for(bcu_loop_i=0;bcu_loop_i<2;++bcu_loop_i){
//				RS485_DTU_UploadCurrentCCSmokeData(bcu_loop_i, 1, 1, 2);
//				delay_ms(20000);
//			}
//			//通过DTU上传BCU端产生的数据
//			RS485_DTU_UploadBCUData(1, 1, 2, 3);
			
			
			//获取电流数据
			//RS485_ElectMeter_ReadCurrentData();
			//delay_ms(2);
			
//			//将电流数据发送给BMU(广播)
//			CAN1_Send_Data(0xff, FT_DATA_CURRENT, 0, 0, 0, currentf);

			//获取供电电压值
//			tmppower = get_power_vol(5);
//			printf("Supply power is %fV (tmppower).\r\n",((float)tmppower)*0.001);
//			printf("Supply power is %fV (powervolu16).\r\n",((float)powervolu16)*0.001);
*/			

			//*********************先赋个值,用于测试DTU模块***********************
/*
//			for(loop_i=0;loop_i<10;loop_i++){  //电池假数据
//				for(loop_j=0;loop_j<12;loop_j++){
//					cellvol[0][loop_i][loop_j] = 36000 + (rand()%1000);  //
//					soc[0][loop_i][loop_j] = 85.135 + (rand()%1000)*0.001;
//					cellvol[1][loop_i][loop_j] = 36000 + (rand()%1000);  //
//					soc[1][loop_i][loop_j] = 85.238 + (rand()%1000)*0.001;
//				}
//			}
//			soctotal = 85.197 + (rand()%1000)*0.001;;
//			for(loop_i=0;loop_i<20;loop_i++){
//				for(loop_j=0;loop_j<5;loop_j++){
//					tempf[loop_i][loop_j] = 28.13 + (rand()%1000)*0.001;
//				}
//			}
//			insuval_pos = 564.354 + (rand()%1000)*0.001;
//			insuval_neg = 557.963 + (rand()%1000)*0.001;
//			bmucurrentf[0] = 43.947 + (rand()%1000)*0.001;
//			bmucurrentf[1] = bmucurrentf[0];
//			currentf = bmucurrentf[0];
//			smokef[0][0] = 32.34 + (rand()%1000)*0.001;
//			smokef[0][1] = 33.69 + (rand()%1000)*0.001;
//			smokef[1][0] = 33.27 + (rand()%1000)*0.001;
//			smokef[1][1] = 31.53 + (rand()%1000)*0.001;
//			voltotal[0] = 864.2374 + (rand()%1000)*0.001;
//			voltotal[1] = 859.2918 + (rand()%1000)*0.001;
//			voltotal[2] = 870.3921 + (rand()%1000)*0.001;;
//			ccstate[1] = 2;
*/
			//**********************假数赋值完毕*********************************			
			
			printf("Working %ds.\r\n",i);
			i++;
			GPIO_ToggleBits(GPIOD,GPIO_Pin_15);
			delay_ms(1000);			
#endif
		}
#endif  //不使用系统的情况结束		


/* 在【使用系统】的情况下:
	 - 初始化后创建开始任务,用以创建需要运行的多个任务
*/
#if IS_USE_RTOS  //使用系统的情况
		xTaskCreate((TaskFunction_t   )start_task,          //任务函数
				  (const char*      )"start_task",        //任务名称
				  (uint16_t         )256,                 //任务堆栈大小
				  (void*            )NULL,                //传递给任务函数的参数
				  (UBaseType_t      )1,                   //任务优先级
				  (TaskHandle_t*    )&vStartTaskHandler);  //任务句柄
  vTaskStartScheduler();
#endif  /* IS_USE_RTOS */
}

/*
在【使用系统】情况下,各个任务的具体执行过程
*/
//**************************************开始任务:创建多个任务***************************************
void start_task(void)
{
	taskENTER_CRITICAL(); // 进入临界区
	
	//【0.1】创建记录运行时间&闪烁指示灯任务
	xTaskCreate((TaskFunction_t   )timerecord_task,          //任务函数
							(const char*      )"timerecord_task",        //任务名称
							(uint16_t         )50,                 //任务堆栈大小
							(void*            )NULL,               //传递给任务函数的参数
							(UBaseType_t      )1,                  //任务优先级
							(TaskHandle_t*    )&vTimeRecordTaskHandler);  //任务句柄	
	
	//【0.2】创建主板串口打印任务(如打印运行时间等)
	xTaskCreate((TaskFunction_t   )pcprint_task,          //任务函数
							(const char*      )"pcprint_task",        //任务名称
							(uint16_t         )50,                 //任务堆栈大小
							(void*            )NULL,               //传递给任务函数的参数
							(UBaseType_t      )2,                  //任务优先级
							(TaskHandle_t*    )&vPcPrintTaskHandler);  //任务句柄									
	
	//【0.3】创建DTU数据上传任务(485通信1)
	xTaskCreate((TaskFunction_t   )DTU_task,          //任务函数
							(const char*      )"DTU_task",        //任务名称
							(uint16_t         )512,                 //任务堆栈大小
							(void*            )NULL,               //传递给任务函数的参数
							(UBaseType_t      )3,                  //任务优先级
							(TaskHandle_t*    )&vDTUTaskHandler);  //任务句柄
	
#if IS_BMU_ROLE  //BMU模块任务
/* BMU自身任务模块:
  - 【1.1】定时采集管理范围内的温度数据和单体电池电压数据;
  - 【1.2】内部CAN通信定时发送心跳包,正常连接到BCU后,应该会收到BUC的确认包;
	- 【1.3】在BMU内部CAN中断收到上传数据命令后,将对应标志变量置为1,在该任务任务中,当标志变量为1时,开始上传数据,上传结束后发送上传结束帧,并将该变量值为0;
  - 【1.4】BMU均衡任务,当某个电池单元开启均衡后,判断该电池当前状态是否达到当前停止均衡条件,达到则关闭;在BMU内部CAN中断收到均衡命令后,开启/关闭对应电池单元的均衡功能;
  - 【1.5】BMU显示数据任务,在内部CAN中断中接收到显示数据命令后,将对应标志变量置为1,在该任务中,当标志变量为1时,开始显示数据,显示结束后,发送显示结束帧,并将该变量置为0;
*/

	//【1.1】创建BMU数据采集任务(温度|单体电压)
	xTaskCreate((TaskFunction_t   )bmu_dataacq_task,          //任务函数
							(const char*      )"bmu_dataacq_task",        //任务名称
							(uint16_t         )1024,                 //任务堆栈大小
							(void*            )NULL,               //传递给任务函数的参数
							(UBaseType_t      )6,                  //任务优先级
							(TaskHandle_t*    )&vBMUDataAcqTaskHandler);  //任务句柄								
	
	//【1.2】创建内部CAN通信发送心跳包任务
	xTaskCreate((TaskFunction_t   )bmu_heartbeat_task,          //任务函数
							(const char*      )"bmu_heartbeat_task",        //任务名称
							(uint16_t         )50,                 //任务堆栈大小
							(void*            )NULL,               //传递给任务函数的参数
							(UBaseType_t      )3,                  //任务优先级
							(TaskHandle_t*    )&vBMUHeartBeatTaskHandler);  //任务句柄							
	
	//【1.3】创建BMU上传数据任务
	xTaskCreate((TaskFunction_t   )bmu_uploaddata_task,          //任务函数
							(const char*      )"bmu_uploaddata_task",        //任务名称
							(uint16_t         )256,                 //任务堆栈大小
							(void*            )NULL,               //传递给任务函数的参数
							(UBaseType_t      )2,                  //任务优先级
							(TaskHandle_t*    )&vBMUUploadDataTaskHandler);  //任务句柄													
							
	//【1.4】创建BMU均衡控制任务
	xTaskCreate((TaskFunction_t   )bmu_balance_task,          //任务函数
							(const char*      )"bmu_balance_task",        //任务名称
							(uint16_t         )256,                 //任务堆栈大小
							(void*            )NULL,               //传递给任务函数的参数
							(UBaseType_t      )5,                  //任务优先级
							(TaskHandle_t*    )&vBMUBalanceTaskHandler);  //任务句柄							
	
	//【1.5】创建BMU显示数据任务
//	xTaskCreate((TaskFunction_t   )bmu_screen_task,          //任务函数
//							(const char*      )"bmu_screen_task",        //任务名称
//							(uint16_t         )256,                 //任务堆栈大小
//							(void*            )NULL,               //传递给任务函数的参数
//							(UBaseType_t      )2,                  //任务优先级
//							(TaskHandle_t*    )&vBMUScreenTaskHandler);  //任务句柄
	
#endif

#if IS_BCU_ROLE  //BCU模块任务						
/* BCU自身任务模块:
	- 【2.1】检测CC状态(对应国标通信功能);
  - 【2.2】检测绝缘状态;
	- 【2.3】检测高压状态;
	- 【2.4】检测供电电压;
	- 【2.5】检测电流值;
	- 【2.6】控制功能(控制各BMU依次上传数据,比较得到最大/最小数据,根据当前数据估算SOC,执行均衡策略,故障检查及报警,历史数据记录(以单体数据的1轮为依据));
  - 【2.7】BMU显示数据控制;
  - 【2.8】监测外来CAN;
*/
						
	//【2.1】创建CC状态检测任务
	// xTaskCreate((TaskFunction_t   )ccacq_task,          //任务函数
	// 						(const char*      )"ccacq_task",        //任务名称
	// 						(uint16_t         )128,                 //任务堆栈大小
	// 						(void*            )NULL,               //传递给任务函数的参数
	// 						(UBaseType_t      )2,                  //任务优先级
	// 						(TaskHandle_t*    )&vCCAcqTaskHandler);  //任务句柄	

//	//【2.2】创建绝缘状态检测任务
//	xTaskCreate((TaskFunction_t   )insulation_task,          //任务函数
//							(const char*      )"insulation_task",        //任务名称
//							(uint16_t         )128,                 //任务堆栈大小
//							(void*            )NULL,               //传递给任务函数的参数
//							(UBaseType_t      )3,                  //任务优先级
//							(TaskHandle_t*    )&vInsulationTaskHandler);  //任务句柄
	
	//【2.3】创建高压状态检测任务
//	xTaskCreate((TaskFunction_t   )hvacq_task,          //任务函数
//							(const char*      )"hvacq_task",        //任务名称
//							(uint16_t         )128,                 //任务堆栈大小
//							(void*            )NULL,               //传递给任务函数的参数
//							(UBaseType_t      )3,                  //任务优先级
//							(TaskHandle_t*    )&vHVAcqTaskHandler);  //任务句柄

//	//【2.4】创建供电电压检测任务
//	xTaskCreate((TaskFunction_t   )poweracq_task,          //任务函数
//							(const char*      )"poweracq_task",        //任务名称
//							(uint16_t         )128,                 //任务堆栈大小
//							(void*            )NULL,               //传递给任务函数的参数
//							(UBaseType_t      )4,                  //任务优先级
//							(TaskHandle_t*    )&vPowerAcqTaskHandler);  //任务句柄								
//	
//	//【2.5】创建电流数据采集任务
//	xTaskCreate((TaskFunction_t   )currentacq_task,          //任务函数
//							(const char*      )"currentacq_task",        //任务名称
//							(uint16_t         )128,                 //任务堆栈大小
//							(void*            )NULL,               //传递给任务函数的参数
//							(UBaseType_t      )3,                  //任务优先级
//							(TaskHandle_t*    )&vCurrentAcqTaskHandler);  //任务句柄		
	
	//【2.6】创建控制功能任务
//	xTaskCreate((TaskFunction_t   )bcucontrol_task,          //任务函数
//							(const char*      )"bcucontrol_task",        //任务名称
//							(uint16_t         )128,                 //任务堆栈大小
//							(void*            )NULL,               //传递给任务函数的参数
//							(UBaseType_t      )2,                  //任务优先级
//							(TaskHandle_t*    )&vBCUControlTaskHandler);  //任务句柄		

//【2.7】创建BMU显示数据控制任务
//	xTaskCreate((TaskFunction_t   )bcu_screen_task,          //任务函数
//							(const char*      )"bcu_screen_task",        //任务名称
//							(uint16_t         )128,                 //任务堆栈大小
//							(void*            )NULL,               //传递给任务函数的参数
//							(UBaseType_t      )2,                  //任务优先级
//							(TaskHandle_t*    )&vBCUScreenTaskHandler);  //任务句柄	

// 【2.8】监听外部CAN							
	xTaskCreate((TaskFunction_t   )externcan_task,          //任务函数
							(const char*      )"externcan_task",        //任务名称
							(uint16_t         )256,                 //任务堆栈大小
							(void*            )NULL,               //传递给任务函数的参数
							(UBaseType_t      )4,                  //任务优先级
							(TaskHandle_t*    )&vExternCanTaskHandler);  //任务句柄	
	
#endif													
	
	//删除开始任务
	vTaskDelete(vStartTaskHandler);
	taskEXIT_CRITICAL();  //退出临界区
	
} //************************************开始任务结束***********************************************



//【0.1】LED指示灯任务:每隔1s改变指示灯状态,以示系统正常运行
void timerecord_task(void)
{
	while(1) {
		GPIO_ToggleBits(GPIOD,GPIO_Pin_15);  //BMS主控板引脚控制指示灯
		++runtime;
		++standtime;
		vTaskDelay(1000);
	}
}


//【0.3】DTU数据上传任务
void DTU_task(void)
{
	u8 i;
	cellvol[0] = cellvol[1] = 3;
  soc[0] = soc[1] = 46.7;
	while(1) {
    for(i=0;i<BMU_Connect_Num;i++){
      DTU_UploadBMUCell_InBCU(BMU_List_Id[i], i*BMU_VOL_NUM, (i+1)*BMU_VOL_NUM);
    } 

  DTU_UploadBMUCell_InBCU(1, 0 , 2);
		vTaskDelay(3000);
  DTU_UploadBMUTemp_InBCU(1, 0, 2);
		vTaskDelay(3000);
  DTU_UploadBMUData_InBCU(1, 0, 0, 2);
		vTaskDelay(3000);
  DTU_UploadBCUData();
	
//		RS485_DTU_UploadTempData(0,0,6);
//		vTaskDelay(pdMS_TO_TICKS(50));
//		RS485_DTU_UploadTempData(0,6,12);
//		vTaskDelay(pdMS_TO_TICKS(50));
//		RS485_DTU_UploadTempData(0,12,18);
//		vTaskDelay(pdMS_TO_TICKS(50));
//		RS485_DTU_UploadTempData(0,18,25);
//		vTaskDelay(pdMS_TO_TICKS(200));
//		
//		RS485_DTU_UploadSmokeCurrentData(0,1,0);
//		vTaskDelay(pdMS_TO_TICKS(200));
//		
//		RS485_DTU_UploadBCUData(1,0,0,0);
//		vTaskDelay(pdMS_TO_TICKS(200));

		//RS485_DTU_UploadVolData(0,0,8,12);
		
		//GPIO_ToggleBits(GPIOD,GPIO_Pin_15);
		vTaskDelay(10000);           //10s更新一轮数据
	}
}

//【0.2】
//通过UART7串口向PC上位机打印日志,并进行交互
//暂时:每隔5s打印运行时间
void pcprint_task(void)
{
	//u32 i=0;
	while(1) {
		printf("Time: %ds.\r\n",runtime);
		//i = i + 5;
		vTaskDelay(5000);
	}
}


#if IS_BMU_ROLE /* BMU自身任务模块实现 */

//【1.1】创建BMU数据采集任务(温度|单体电压)
void bmu_dataacq_task(void)
{
	u8 chip,loop_i,loop_j,i;
	uint16_t tmp_cellvol[1][12];  //暂时保存读取到的电压值
	uint16_t tmp_temp[1][6];  //暂时保存读取到的GPIO模拟数据值(6个值,前5个为GPIO端口电压值)	
	
	while(1){
		
#if BMU_PRINT_P  //开启该权限,则在该任务中输出温度转换结果
		printf("BMU0x%02X dataacq.\r\n",My_Id);
#endif		
		
		taskENTER_CRITICAL(); // 进入临界区
		//获取3个芯片的温度数据(每个芯片4个温度数据,共3个芯片,12个数据)
		for(chip=0;chip<3;++chip) {
			readtemp(chip);
		}
		taskEXIT_CRITICAL();  //退出临界区		
		
//#if BMU_PRINT_P  //开启该权限,则在该任务中输出温度转换结果
//		printf("Temp 0x%02X\r\n",My_Id);
//#endif
//		for(loop_i=0;loop_i<BMU_TEMP_NUM;++loop_i){
//			//if((temp[loop_i]>0) && (temp[loop_i]<65535))
//			{
////#if BMU_SCREEN_P
////				//485发送数据至显示屏
////#endif				
////#if BMU_PRINT_P
////				printf("Temp %d: %d   %f\r\n",loop_i,temp[loop_i],tempf[loop_i]);
////#endif				
//			}
//		}

    vTaskDelay(dataacq_time/2);
		
		//获取4个芯片的单体电压数据
		taskENTER_CRITICAL(); // 进入临界区
		for(chip=0;chip<4;chip++) {
			readvol(chip);  //读取第chip片采集芯片的电压,并将读取到的数据保存到全局变量cellvol[chip][]中
		}
		taskEXIT_CRITICAL();  //退出临界区
		
//#if BMU_PRINT_P  //开启该权限,则在该任务中输出电压转换结果
//		printf("Volt 0x%02X\r\n",My_Id);
//#endif		
//		for(loop_i=0;loop_i<BMU_VOL_NUM;++loop_i){
//			//if((cellvol[loop_i]>0) && (cellvol[loop_i]<65535))
//			{
////#if BMU_SCREEN_P
////        //485发送数据至显示屏
////#endif				
////#if BMU_PRINT_P
////				printf("Vol %d: %f\r\n",loop_i,((float)cellvol[loop_i])*0.0001);
////#endif
//			}
//		}
		
		vTaskDelay(dataacq_time/2);
		
	}
}

//【1.2】创建内部CAN通信发送心跳包任务
void bmu_heartbeat_task(void)
{
	while(1){
		CAN1_ComControl(My_Superior_Id,FT_COMCONTROL_HEARTBEAT);
		
		vTaskDelay(heartbeat_time);
	}
}

//【1.3】创建BMU上传数据任务
void bmu_uploaddata_task(void){
		
	u8 chip,loop_i;
	
	while(1){
		
		if(bmu_uploaddata==1)
		{
			vTaskDelay(1);
			
			#if BMU_PRINT_P			
			  printf("Upload data.\r\n");	
      #endif
			
			//上传温度数据值
			for(loop_i=0;loop_i<BMU_TEMP_NUM;++loop_i){
				//if((temp[loop_i]>0)&&(temp[loop_i]<65536))  //在上传数据之前先筛选出"正常"的数据
				{
					//数据帧中应包含电池组编号
					CAN1_Send_Data(My_Superior_Id, FT_DATA_TEMP, My_Pack_Number, loop_i, 0, tempf[loop_i]);
					vTaskDelay(1);
				}
			}
			
			//上传单体电池数据
			for(loop_i=0;loop_i<BMU_VOL_NUM;++loop_i){
				//if((cellvol[loop_i]>4000)&&(cellvol[loop_i]<65535))
				{
					//数据帧中应包含电池组编号
					CAN1_Send_Data(My_Superior_Id, FT_DATA_CELL, My_Pack_Number, loop_i, cellvol[loop_i], 0);  //单体电压
					//CAN1_Send_Data(My_Superior_Id, FT_DATA_CELL, My_Pack_Number, loop_i, cellvol[loop_i], soc[loop_i]);  //单体电压和单体SOC数据
					vTaskDelay(1);
				}
			}
			
			//上传传输数据完毕信息
			CAN1_ControlCmd(My_Superior_Id, FT_CONTROLCMD_ACK, CONTROLCMD_ACK_UPLOAD_COMPLETE, 0xFF, 0xFFFF);
			
			//printf("Upload complete.\r\n");
			
			bmu_uploaddata = 0;
			
		}
		vTaskDelay(1);
	}
}

/*【1.4】创建BMU均衡控制任务
- 流程:
  - 判断是否关闭均衡:根据当前的balancetargetvol(均衡目标电压)和当前实际电压值,判断BalanceFlag中为1(开启均衡)的电池,当前电压是否已达到均衡目标电压,达到了则将对应的1置为0;
  - 保持一致:依次判断BalanceFlag和BalanceState中的每一个是否一致,不一致则向LT6804芯片发送命令,将需要关闭的关闭(达到均衡目标电压/中断中命令关闭),将需要开启的开启(中断中命令开启);
*/
void bmu_balance_task(void)
{
	u8 loop_i,resultflag;
	
	while(1){
		
		//判断是否关闭均衡
		for(loop_i=0;loop_i<BMU_VOL_NUM;++loop_i){
			if(BalanceFlag[loop_i]==1){   //如果已开启均衡
				if(cellvol[loop_i]<balancetargetvol){  //达到均衡目标电压
					BalanceFlag[loop_i] = 0;  //标记为需要关闭
				}
			}
		}
		
		//保持一致
		for(loop_i=0;loop_i<BMU_VOL_NUM;++loop_i){
			if(BalanceFlag[loop_i] != BalanceState[loop_i]){

				resultflag = set_cell_discha
				rge(loop_i,BalanceFlag[loop_i]);  //直接使用BalanceFlag状态向量开启/关闭均衡
				
				if(resultflag==1){  //如果更改成功
					BalanceState[loop_i] = BalanceFlag[loop_i];  //修改当前均衡状态向量
				}
			}
		}
		
		vTaskDelay(10);  //延时10ms
	}

//【1.5】创建BMU显示数据任务
void bmu_screen_task(void)
{
	u8 loop_i;
	
	while(1){
	
		if(bmu_displaydata==1){
			#if BMU_PRINT_P			
			  printf("Display data.\r\n");
      #endif
			
			//显示温度数据值
			for(loop_i=0;loop_i<BMU_TEMP_NUM;++loop_i){
				//if((temp[loop_i]>0)&&(temp[loop_i]<65536))  //在上传数据之前先筛选出"正常"的数据
				{
					//在显示屏显示该温度数据
					RS485_Screen_WriteMem(get_tempaddr(My_Pack_Number,loop_i),(u16)(tempf[loop_i]*10));					
					vTaskDelay(1);					
				}
			}
			
			//显示单体电池数据
			for(loop_i=0;loop_i<BMU_VOL_NUM;++loop_i){
				//if((cellvol[loop_i]>4000)&&(cellvol[loop_i]<65535))
				{
					//在显示屏显示该电池电压数据
					RS485_Screen_WriteMem(get_celladdr(My_Pack_Number,loop_i),cellvol[loop_i]);		
					
					vTaskDelay(1);
				}
			}

			//上传传输数据完毕信息
			CAN1_ControlCmd(My_Superior_Id, FT_CONTROLCMD_ACK, CONTROLCMD_ACK_DISPLAY_COMPLETE, 0xFF, 0xFFFF);
			
			//printf("Display complete.\r\n");			
			
			bmu_displaydata = 0;
		}
		vTaskDelay(1);
	}
}

#endif

#if IS_BCU_ROLE /* BCU自身任务模块实现 */


/*【2.1】创建CC信号检测任务
通过芯片内部的ADC功能实现CC状态检测的功能,BMS仅需检测CC2的状态即可;
CC状态: 12V:未连接, 6V:已连接;
此处焊接的是20k和100k的分压,即如果未连接,则检测到的电压应为12V*2/12=2V,如果已连接,则检测到的电压应为6V*2/12=1V;
*/
void ccacq_task(void)
{
	u8 cc;
	u32 i=0;
	u8 flag;
	
	BMS_Enable();  //配置为BMS模式
	MODE = 1;  //还没有检测到CC状态,先设置为充电机模式
	
	while(1){
		cc = get_cc_state(10);
		
		//根据CC目前CC状态配置对外通信模式
		if(cc==CC_STATE_CONNECTED){
			MODE = 0;  //BMS状态
			RS485_Screen_WriteMem(CC_STATE_ADDR,1);  //显示CC状态为1(已连接)
			
			//系统上电(继电器闭合)
			
		}
		else{
			MODE = 1;  //充电机状态
			RS485_Screen_WriteMem(CC_STATE_ADDR,0);  //显示CC状态为0(未连接)			
			
			//系统下电(继电器断开)
			
		}
		
		//串口显示CC2电压状态
		if(i++%50==0){  //每5秒显示一次
			if(cc==CC_STATE_CONNECTED)
				printf("CC2 connected.\r\n");
			else{
//				//测试				
//				flag = Extern_CAN_SendMsg(0, 0, "12345678", 8);
//				if(flag==1)
//					printf("CAN2 trans failed.");
				printf("CC2 disconnect.\r\n");
			}
		}
		
		vTaskDelay(100);
		
	}	
}

/*【2.2】创建绝缘状态检测任务
- 总体描述:通过检测电池正负极分别与壳体之间绝缘度是否大于100欧姆/V,来判断电池与壳体是否漏电
- 电压:绝缘检测电压范围:200-1500V
- 周期:正常检测周期10s,异常检测周期20ms,漏电电阻检测精度
- 绝缘漏电的1级故障:500ohm/V,2级故障200ohm/V,3级故障100ohm/V
*/
void insulation_task(void)
{
	u8 insuflag = 0;
	
	//通过设置达到每隔11s获取一次绝缘数据的目的
	while(1) {
		if(insuflag==0){
			insuflag = 1;
			RS485_Insulation_OpenRead();  //发送命令开启数据读取
		}
		else{
			insuflag = 0;
		}
		
		vTaskDelay(8000);;  //应延时约6秒以让模块获取到正确数据
		
		RS485_Insulation_ReadData();  //发送读数据命令,并使用中断接收模块数据
		vTaskDelay(5);
		
		//向显示屏发送绝缘状态数据(欧姆/伏)
//		RS485_Screen_WriteMem(INSUSTATE_POS_ADDR,(u16)insuval_pos);
//		RS485_Screen_WriteMem(INSUSTATE_NEG_ADDR,(u16)insuval_neg);	
		
		//向显示屏发送绝缘状态数据(千欧)(连个电阻测试一下绝缘模块的正确性)
		RS485_Screen_WriteMem(INSUSTATE_POS_ADDR,res_pos_data);
		RS485_Screen_WriteMem(INSUSTATE_NEG_ADDR,res_neg_data);	
		
		vTaskDelay(2995);  //让模块恢复到开启测量前状态
	}		
}

//【2.3】创建高压状态检测任务
void hvacq_task(void)
{
	
	IIC_StartHVConvert(0);  //开启B+(CH0)单极模式
		vTaskDelay(1);
		IIC_ReadHVData(0);  //读取B+(CH0)转换结果数据
		vTaskDelay(1);
		IIC_StartHVConvert(1);  //开启HV1(CH1)单极模式
		vTaskDelay(1);
		IIC_ReadHVData(1);  //读取HV1(CH1)转换结果数据	
		
		vTaskDelay(1000);
}

/*【2.4】创建供电电压检测任务
通过芯片内部 ADC功能实现供电电压的检测,在getpower_vol函数中采样了5次;
并且在BCU V1.3中使用的是8分压转换24V->3V,即如果在BCU V1.3中是正常的,如果在BCU V1.2中使用12V供电的话,会显示是16V供电,12V->2V(6分压)->16V(8分压);
*/
void poweracq_task(void)
{
	u32 i=0;
	u16 powertmpu16 = 0;
	
	while(1) {
		get_power_vol(10);  //获取5次采样供电电压的平均值
		
		//将该数据在显示屏显示
		powertmpu16 = (u16)(powervolf*100);  //截取xx.xx的有效位数
		RS485_Screen_WriteMem(POWER_VOL_ADDR,powertmpu16);  //发送到显示屏
		
		//串口显示供电电压数据
		if(i++%50==0){
			printf("Power: %f\r\n",powervolf);
		}
		vTaskDelay(1000);
		
	}	
}

//【2.5】创建电流数据检测任务
void currentacq_task(void)
{
	//获取的电流数据currentf有正负
	//通过电表获取电流数据
	u32 i=0;
	u16 currenttmpu16;
	
	while(1) {
		//读电能表,并将读取结果更新到current[]和currentf全局变量中
		RS485_ElectMeter_ReadCurrentData();	
		vTaskDelay(5);
		
		
		//通过ADC芯片读取电流数据
		IIC_StartCurrentConvert(1);  //代码中设定CH0为正,实际目前板子接反了,CH0是负
		vTaskDelay(1);
		IIC_ReadCurrentData();
		
		
		
		
		
		
		
		
		currenttmpu16 = (u16)(currentf*100);
		RS485_Screen_WriteMem(TOTAL_CURR_ADDR,currenttmpu16);
		
		if(i++%50==0){  //x秒显示一次
			printf("Current: %f.\r\n",currentf);
		}
		
		//根据电流大小判定是否为静置状态
		if(currentf>=0){
			if(currentf>STANDCURRENT_LINE){  //电流值大于阈值
				standtime = 0;
			}
		}
		else{
			if((-currentf)>STANDCURRENT_LINE){
				standtime = 0;
			}
		}
		
		//vTaskDelay(495);
		vTaskDelay(1995);
	}
}

//【2.6】创建BCU控制功能任务
void bcucontrol_task(void)
{
	while(1) {
		//【2.6.1】获取BMU数据
//		bcu_can1_ctrlandacq();
		
		//【2.6.2】比较单体数据,获得最大/最小数据
//		datacompare();
		
		//【2.6.3】估算当前总体SOC状态
//		soccalculate();	
		
		//【2.6.4】执行均衡策略
//		bcu_balance();
		
		//【2.6.5】故障检测(各个故障类型依次检测一遍)
//		faultalarm();	
		
		//【2.6.6】历史数据记录(以一轮的一次单体数据和二次数据为准进行记录)
//		historydatastore();	
		
		vTaskDelay(bmucontroltask_delay);
	}
}

//【2.6.1】控制各个从控,获取对应单体数据(控制上传数据,若一段时间没有回应则跳过,并标记为失联,节省时间且检测了状态)
void bcu_can1_ctrlandacq(void)
{
	u8 loop_i,loop_j;
	u16 timecount;
	
	printf("Upload ctrl fun.\r\n");
	
	for(loop_i=0;loop_i<BMU_LIST_NUM;++loop_i){  //遍历BMU列表

	  is_bmu_uploading = 0;          //清除数据上传标志
		
		if(BMU_List_State[loop_i]==1){  //若节点存在
			
			printf("BMU 0x%02X uploading...\r\n",BMU_List_Id[loop_i]);
			
			CAN1_ControlCmd(BMU_List_Id[loop_i],FT_CONTROLCMD_UPLOADDATA,0xFF,0xFF,0xFFFF);		//给BMU发送上传数据命令
			
			is_bmu_uploading = 1;  //标识为正在上传数据
			timecount = 0;         //超时也认为传输完成
			
			//延时等待接收数据
			while((is_bmu_uploading==1)&&(timecount<100)){  //没上传完成且没超时
				vTaskDelay(1);
				++timecount;
			}
			if(timecount==100){
				printf("Timeout.\r\n");
			}
			else{
				printf("0x%02X upload complete %d.\r\n",BMU_List_Id[loop_i],timecount);
			}
			
		}
	}

  //生成总电压
  //data2822，整车动力蓄电池当前电压
}

//【2.6.2】比较单体数据,获得最大/最小数据
void datacompare(void)
{
	u16 tmpmaxvol,tmpmaxvolid=0,tmpminvol,tmpminvolid=0;
	float tmpmaxtempf,tmpmintempf;
  u16 tmpmaxtempfid=0,tmpmintempfid=0;
	float tmpvolftotal=0,tmptempftotal=0;
	u32 tmpvoltotal = 0;
	u16 tmpvolnum=0,tmptempfnum=0,loop_i,loop_j;
	
	tmpmaxvol = 0;
	tmpminvol = 40000;
	//比较单体电压数据
	for(loop_i=0;loop_i<BMU_LIST_NUM * BMU_VOL_NUM;++loop_i){  //循环单体电压数据
		if((cellvol[loop_i]>4000)&&(cellvol[loop_i]<39000)){  //数据属于"正常"数据
			if(cellvol[loop_i]>tmpmaxvol){  //最大单体电压
				tmpmaxvol = cellvol[loop_i];
				tmpmaxvolid = loop_i;
			}
			if(cellvol[loop_i]<tmpminvol){  //最小单体电压
				tmpminvol = cellvol[loop_i];
				tmpminvolid = loop_i;
			}
			++tmpvolnum;
			tmpvoltotal += cellvol[loop_i];
		}
	}
	
	tmpmaxtempf = -15.0;
	tmpmintempf = 65.0;
	//比较温度数据
	for(loop_i=0;loop_i<BMU_LIST_NUM * BMU_TEMP_NUM;++loop_i){
		if((tempf[loop_i]>(-10.0))&&(tempf[loop_i]<60.0)){  //数据属于"正常"数据
			if(tempf[loop_i]>tmpmaxtempf){  //最大温度数据
				tmpmaxtempf = tempf[loop_i];
				tmpmaxtempfid = loop_i;
			}
			if(tempf[loop_i]<tmpmintempf){  //最小温度数据
				tmpmintempf = tempf[loop_i];
				tmpmintempfid = loop_i;
			}
			++tmptempfnum;
			tmptempftotal += tempf[loop_i];
		}		
	}
	
	//保存单体电压比较结果
	cellvoltotal = tmpvoltotal;
	max_cellvol = tmpmaxvol;
	max_cellvolid = tmpmaxvolid;
	min_cellvol = tmpminvol;
	min_cellvolid = tmpminvolid;
	cellvolaverage = tmpvoltotal / tmpvolnum;
	max_diff_cellvol = tmpmaxvol - tmpminvol;
	
	//保存温度比较结果
	max_tempf = tmpmaxtempf;
	max_tempfid = tmpmaxtempfid;
	min_tempf = tmpmintempf;
	min_tempfid = tmpmintempfid;
	tempfaverage = tmptempftotal / tmptempfnum;
	max_diff_tempf = tmpmaxtempf - tmpmintempf;

  //将结果更新到对外CAN
  data3077 = (max_cellvol<<4)|(max_cellvolid&0xF);
  data3085 = max_cellvolid;
  data3086 = base_temp + (u16)max_tempf;
  data3087 = max_tempfid;
  data3088 = base_temp + (u16)min_tempf;
  data3089 = min_tempfid;
  data3602 = min_cellvol*100;
  data3603 = max_cellvol*100;
  data3604 = base_temp + (u16)min_tempf;
  data3605 = base_temp + (u16)max_tempf;
}

//【2.6.3】估算当前总体SOC状态
void soccalculate(void)
{
	if(standtime>STANDTIME_LINE){  //电池组静置时间大于设定的阈值,该时间变量在电流大于阈值时归零
		soctotal = soc_calibrate(min_cellvol);  //对最小电压值进行标定
		xk = soctotal;  //对SOC估算过程初始化
		Pk = 0;
	}
	else{
		soc_calculate(currentf, min_cellvol,((float)bmucontroltask_delay)*0.001);
	}

  //将结果同步到对外CAN
  //data2821
  //data3078
  //data3079
  //data3601
}

/*【2.6.4】执行均衡策略
  - 根据当前状态,控制均衡是否开启;
  - 计算均衡目标电压(三者最大值)
    - 单体最低电压+期望(或允许的)最大单体电压差值;
    - (单体最高电压+单体最低电压)/2;
    - 单体电压最低限值;
*/
void bcu_balance(void)
{
	u16 tmpbalancetargetvol;
	u8 loop_i,pknum,pknum_1,cellid;

	u8 tmp_celltobalance_id[BALANCE_NUM];       //本次均衡电池id(目前开启均衡的节点ID,在bcu_balance()函数中会更新)
	u16 tmp_celltobalance_vol[BALANCE_NUM];     //本次均衡电池电压	
	u8 tmp_balanceenableflag = 0;               //本次是否开启均衡(默认不开启)
	
	VoltLimitFlag = 0;           //均衡的压差条件
	ModeValidFlag = 0;           //均衡的系统工作状态条件
	CurrentLimitFlag = 0;        //均衡的电流大小条件

	balancetargetvol = 50000;    //默认不开启均衡时的均衡截止电压
	
	for(loop_i=0;loop_i<BALANCE_NUM;++loop_i){  //清空本次的均衡列表
		tmp_celltobalance_id[loop_i] = 0xFF;
		tmp_celltobalance_vol[loop_i] = 0;
	}
	
	//判断均衡是否开启
	if((max_cellvol-min_cellvol)>VoltLimitCalb){  //压差大于标定值
		VoltLimitFlag = 1;
	}
	if(currentf>0){  //电流大于0,处于充电状态
		ModeValidFlag = 1;
	}
#if RUN_BALANCE  // BMS运行即可开启均衡,不必一定处于充电状态
	ModeValidFlag = 1;
#endif
	if(abs(currentf)<CurrentLimitCalb){  //均衡的电流大小条件限定
		CurrentLimitFlag = 1;
	}
	
	//如果本次均衡开启
	if((VoltLimitFlag+ModeValidFlag+CurrentLimitFlag)==3) {
		tmp_balanceenableflag = 1;  //标记本次均衡开启
		
		//计算均衡目标电压(三者最大值)
		balancetargetvol = min_cellvol + VoltLimitCalb;
		if((max_cellvol+min_cellvol)>(2*balancetargetvol)){
			balancetargetvol = (max_cellvol+min_cellvol)/2;  //(单体最高电压+单体最低电压)/2
		}
		if(balancetargetvol<MinCellVolLimit){
			balancetargetvol = MinCellVolLimit;
		}

    //生成单体电压结构体
    for(loop_i=0;loop_i<bcucellnum;++loop_i){
      cell[loop_i].id = loop_i;
      cell[loop_i].vol = cellvol[loop_i];
    }

		//找出需要开启均衡的BALANCE_NUM个电池ID,保存在celltobalance_id[]中(可能需要去除其中已经达到均衡目标电压的电池)
		quick_sort(cell, bcucellnum);
    for(loop_i=0;loop_i<BALANCE_NUM;++loop_i){
      celltobalance_id[loop_i] = cell[loop_i].id;
    }
		
		//将开启均衡的命令和均衡目标电压发送给本次找到的BALANCE_NUM个电池所在BMU
		// for(loop_i=0;loop_i<BALANCE_NUM;++loop_i) {
		// 	pknum_1 = celltobalance_id[loop_i]/BALANCE_NUM;  //packnum-1(packnum从1开始)
		// 	cellid = celltobalance_id[loop_i]%BALANCE_NUM;
		// 	CAN1_ControlCmd(BMU_List_Id[pknum_1],FT_CONTROLCMD_CELLBALANCE,CONTROLCMD_CELLBALANCE_ON,cellid,balancetargetvol);  //发送开启均衡命令
		// }
	}		
	
  //如果前一时刻开启了均衡,则关闭之前开启的BALANCE_NUM个电池的均衡
	if(BalanceEnableFlag==1){
		for(loop_i=0;loop_i<BALANCE_NUM;++loop_i) {  //将关闭均衡的命令发送给上次找到的BALANCE_NUM个电池所在的BMU
			pknum_1 = celltobalance_id[loop_i]/BMU_VOL_NUM;  //id范围:0-BMU_LIST_NUM*BMU_VOL_NUM
			cellid = celltobalance_id[loop_i]%BMU_VOL_NUM;
			CAN1_ControlCmd(BMU_List_Id[pknum_1],FT_CONTROLCMD_CELLBALANCE,CONTROLCMD_CELLBALANCE_OFF,cellid,0xFFFF);  //发送关闭均衡命令
			vTaskDelay(1);
		}		
	}
		
	//如果本次均衡开启,则开启新找到的BALANCE_NUM个电池的均衡
	if(tmp_balanceenableflag==1){
		for(loop_i=0;loop_i<BALANCE_NUM;++loop_i) {  //将开启均衡的命令和均衡目标电压发送给本次找到的BALANCE_NUM个电池所在BMU
			pknum_1 = tmp_celltobalance_id[loop_i]/BMU_VOL_NUM;
			cellid = tmp_celltobalance_id[loop_i]%BMU_VOL_NUM;
			CAN1_ControlCmd(BMU_List_Id[pknum_1],FT_CONTROLCMD_CELLBALANCE,CONTROLCMD_CELLBALANCE_ON,cellid,balancetargetvol);  //发送开启均衡命令
			vTaskDelay(1);
		}
	}
	
	//更新BCU中存储的单体均衡标志,并更新上一时刻的均衡控制标志
	BalancePrevEnableFlag = BalanceEnableFlag;
	for(loop_i=0;loop_i<BALANCE_NUM;++loop_i){
		celltobalance_previd[loop_i] = celltobalance_id[loop_i];
		celltobalance_prevvol[loop_i] = celltobalance_vol[loop_i];
	}			
}


/*【2.6.5】故障检测(各个故障类型依次检测一遍)
  - 先实现:充电总压过高,放电总压过低,充电单体高压,充电单体低压,放电单体高压,放电单体低压,充电压差,放电压差,SOC低,绝缘漏电 故障判断
*/
void faultalarm(void)
{
  u8 state;
	if(alarmfun_soclow==FAULTALARM_OPEN){  //故障检测:SOC低
    /*
    state = faultalarm_soclow();
    if(state!=alarmprevstate_soclow){
      if(state==0){
        data3091 = 0x00;
      }else{
        data3091=0x10;
      }

      if(state==3){
        faulthandle_soclow3();
      }else if(state==2){
        faulthandle_soclow2();
      }else if(state==1){
        faulthandle_soclow1();
      }else if(state==0){
        faulthandle_soclow0();
      }
    }
    */
	}
	if(alarmfun_voltotalhigh==FAULTALARM_OPEN){  //故障检测:充电总压过高
  /*
    //充电过程中的外部提供的电压
    state = faultalarm_voltotalhigh(voltotal[1]);
    if(state!=alarmprevstate_voltotalhigh){
      if(state==3){
        faulthandle_voltotalhigh3();
      }else if(state==2){
        faulthandle_voltotalhigh2();
      }else if(state==1){
        faulthandle_voltotalhigh1();
      }else if(state==0){
        faulthandle_voltotalhigh0();
      }
    }
    */
	}	
	if(alarmfun_voltotallow==FAULTALARM_OPEN){  //故障检测:放电总压过低
	/*
    //放电总压值是cellvoltotal？voltotal[2]?
    state = faultalarm_voltotallow();
    if(state!=alarmprevstate_voltotallow){
      if(state==3){
        faulthandle_voltotallow3();
      }else if(state==2){
        faulthandle_voltotallow2();
      }else if(state==1){
        faulthandle_voltotallow1();
      }else if(state==0){
        faulthandle_voltotallow0();
      }
    }
    */
	}	
	if(alarmfun_cvolhigh==FAULTALARM_OPEN){  //故障检测:充电单体高压
	/*
    state = faultalarm_cvolhigh(max_cellvol);
    if(state!=alarmprevstate_cvolhigh){
      if(state!=0){
        data3090 = 0x00;
      }else{
        data3090 = 0x01;
      }

      if(state==3){
        faulthandle_cvolhigh3();
      }else if(state==2){
        faulthandle_cvolhigh2();
      }else if(state==1){
        faulthandle_cvolhigh1();
      }else if(state==0){
        faulthandle_cvolhigh0();
      }
    }
    */
	}	
	if(alarmfun_cvollow==FAULTALARM_OPEN){  //故障检测:充电单体低压
		/*
    state = faultalarm_cvollow(min_cellvol);
    if(state!=alarmprevstate_cvollow){
      if(state!=0){
        data3090 = 0x00;
      }else{
        data3090 = 0x10;
      }

      if(state==3){
        faulthandle_cvollow3();
      }else if(state==2){
        faulthandle_cvollow2();
      }else if(state==1){
        faulthandle_cvollow1();
      }else if(state==0){
        faulthandle_cvolow0();
      }
    }
    */
	}	
	if(alarmfun_cvoldiff==FAULTALARM_OPEN){  //故障检测:充电压差
		/*
    state = faultalarm_cvoldiff(max_diff_cellvol);
    if(state!=alarmprevstate_cvoldiff){
      if(state==3){
        faulthandle_cvoldiff3();
      }else if(state==2){
        faulthandle_cvoldiff2();
      }else if(state==1){
        faulthandle_cvoldiff1();
      }else if(state==0){
        faulthandle_cvoldiff0();
      }
    }
    */
	}	
  if(alarmfun_ctemphigh==FAULTALARM_OPEN){  //故障检测:充电温度过高
		/*
    state = faultalarm_ctemphigh(max_tempf);
    if(state!=alarmprevstate_ctemphigh){
      if(state!=0){
        data3093 = 0;
      }else{
        data3093 = 0x01;
      }

      if(state==3){
        faulthandle_ctemphigh3();
      }else if(state==2){
        faulthandle_ctemphigh2();
      }else if(state==1){
        faulthandle_ctemphigh1();
      }else if(state==0){
        faulthandle_ctemphigh0();
      }
    }
    */
	}	
	if(alarmfun_covercurr==FAULTALARM_OPEN){  //故障检测:充电过流
	  /*
    //充电过程中电流值？
    state = faultalarm_covercurr();
    if(state!=alarmprevstate_covercurr){
      if(state!=0){
        data3092 = 0x00;
      }else{
        data3092 = 0x01;
      }

      if(state==3){
        faulthandle_covercurr3();
      }else if(state==2){
        faulthandle_covercurr2();
      }else if(state==1){
        faulthandle_covercurr1();
      }else if(state==0){
        faulthandle_covercurr0();
      }
    }
    */
	}	
	if(alarmfun_insuleak==FAULTALARM_OPEN){  //故障检测:绝缘
  	/*
    //绝缘检测对应的数据值？
    state = faultalarm_insuleak();
    if(state!=alarmprevstate_insuleak){
      if(state!=0){
        data3094 = 0x00;
      }else{
        data3094 = 0x01;
      }

      if(state==3){
        faulthandle_insuleak3();
      }else if(state==2){
        faulthandle_insuleak2();
      }else if(state==1){
        faulthandle_insuleak1();
      }else if(state==0){
        faulthandle_insuleak0();
      }
    }
    */
	
	}	
}

//【2.6.6】历史数据记录(以一轮的一次单体数据和二次数据为准进行记录)(把每轮的电压和温度进行写入sd卡)
void historydatastore(void)
{
	uint8_t i;
  for(i=0;i<bcucellnum;i++){
    SDCard_Write_SingleVoltage(cellvol[i], i);
  }
	
  for(i=0;i<bcutempnum;i++){
    SDCard_Write_Temperature(tempf[i], i);
  }
}

//【2.7】监听externcan
void externcan_task(void) {
	while(1){
		if(GetExternCanFlag()){
			SetExternCanFlag(0);
			Judge_Msg();
		}
		vTaskDelay(10);
	}
}

#endif


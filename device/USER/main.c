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

//�ڡ�ʹ��ϵͳ���������,���񴴽���������
//BMU��BCU��������������
void start_task(void);            //������������
void timerecord_task(void);       //��0.1��������¼����ʱ��&��˸ָʾ������
void pcprint_task(void);          //��0.2���������ڴ�ӡ����
void DTU_task(void);              //��0.3������DTU�����ϴ�����(485ͨ��1)
TaskHandle_t vStartTaskHandler;              //����������
TaskHandle_t vTimeRecordTaskHandler;         //��¼����ʱ��&��˸ָʾ��������
TaskHandle_t vPcPrintTaskHandler;            //������PC��λ����������������
TaskHandle_t vDTUTaskHandler;                //DTU�����ϴ�������

//BMU����������
#if IS_BMU_ROLE
void bmu_dataacq_task(void);      //��1.1������BMU���ݲɼ�����
void bmu_heartbeat_task(void);         //��1.2������BMU�ڲ�CANͨ�ŷ�������������
void bmu_uploaddata_task(void);           //��1.3������BMU�ϴ���������
void bmu_balance_task(void);          //��1.4������BMU��ص�Ԫ�ľ����������
void bmu_screen_task(void);           //��1.5������BMU��ʾ��������
TaskHandle_t vBMUDataAcqTaskHandler;   //BMU���ݲɼ�������
TaskHandle_t vBMUHeartBeatTaskHandler;      //BMU�ڲ�CANͨ�ŷ���������������
TaskHandle_t vBMUUploadDataTaskHandler;      //BMU�ϴ�����������
TaskHandle_t vBMUBalanceTaskHandler;       //BMU��ص�Ԫ�ľ������������
TaskHandle_t vBMUScreenTaskHandler;       //BMU��ص�Ԫ�ľ������������
#endif

//BCU����������
#if IS_BCU_ROLE
void ccacq_task(void);            //��2.1������CC�źż������
void insulation_task(void);       //��2.2��������Ե״̬�������
void hvacq_task(void);            //��2.3��������ѹ״̬�������
void poweracq_task(void);         //��2.4�����������ѹ�������
void currentacq_task(void);       //��2.5�������������ݼ������
void bcucontrol_task(void);       //��2.6������BCU���ƹ�������
void externcan_task(void);        //��2.7�������ⲿCAN����
TaskHandle_t vCCAcqTaskHandler;              //CC�źż��������
TaskHandle_t vInsulationTaskHandler;         //��Ե���ģ�����ݲɼ�������
TaskHandle_t vHVAcqTaskHandler;              //��ѹ״̬���������
TaskHandle_t vPowerAcqTaskHandler;           //�����ѹ���������
TaskHandle_t vCurrentAcqTaskHandler;         //�������ݼ��������
TaskHandle_t vBCUControlTaskHandler;         //���ƹ���������
TaskHandle_t vExternCanTaskHandler;          //�����ⲿCAN������

//BCU���ƹ�������ĸ����ֺ�������
void bcu_can1_ctrlandacq(void);    //��2.6.1�����Ƹ����ӿ�,��ȡ��Ӧ��������(�����ϴ�����,��һ��ʱ��û�л�Ӧ������,�����Ϊʧ��,��ʡʱ���Ҽ����״̬)
void datacompare(void);            //��2.6.2���Ƚϵ�������,������/��С����
void soccalculate(void);           //��2.6.3�����㵱ǰ����SOC״̬
void faultalarm(void);             //��2.6.4�����ϼ��(���������������μ��һ��)
void historydatastore(void);       //��2.6.5����ʷ���ݼ�¼(��һ�ֵ�һ�ε������ݺͶ�������Ϊ׼���м�¼)
void screen_clear(void);           //�����ʾ������
#endif

//��ʱû�õ����񼰾��
void BMSelectcontrol_task(void);  //����BMSϵͳ��|�µ�����
void electmeter_task(void);       //����������ݲɼ�����(485ͨ��2)
TaskHandle_t vBMSElectControlTaskHandler;    //BMSϵͳ��|�µ�������
TaskHandle_t vElectMeterTaskHandler;         //���������ݲɼ�������


/*
����:�������
*/
int main(void)
{ 
	u8 testdata1[10] = "123456789";
	//char testdata1[500];
	uint16_t i=0;
  u16 u16data;  //����IIC����ת��
	short int sidata;  //����IIC����ת��
	float dataf;  //����IIC����ת��
	u8 iic_flag;
	u8 chip = 0;
	u8 j;
	u8 bmu_loop_i,bmu_loop_j,bcu_loop_i,bcu_loop_j;  //��������ѭ��
	u8 bmucelln,bmutempn,bmucount_i;
	u8 loop_i,loop_j;  //"�ȸ���ֵ"ѭ��
	float tshow;
	u16 tmp_cell_codes[1][12];
	u16 tmp_aux_codes[1][6];
	u16 tmppower;
	
	//*****************************************״̬������ʼ��********************************************
	My_Id = MY_ID;
  My_Did = MY_DID;
	runtime = 0;  //��ʼ��ϵͳ����ʱ��
#if IS_BMU_ROLE
	My_BMS_Role = BMS_ROLE_BMU;
	My_Superior_Id = 0xFF;        //��ʼ��Ϊ�㲥��ַ
	Is_Connected = 0;             //û�����ӵ���һ���豸
	My_Pack_Number = My_Id;       //�˴����ֵ�����ź�IDһ��
	bmu_uploaddata = 0;           //Ĭ�ϲ��ϴ�����
	bmu_displaydata = 0;          //Ĭ�ϲ���ʾ����
	
	//����ʱ��������
	dataacq_time = 2000;          //���ݻ�ȡ����ʱ����(��λms)
	heartbeat_time = 8000;    //������ʱ����(��λΪms)
	dataupdate_time = 5000;   //BMU�����ϴ���ʱ����(��λΪus)
	
#endif	
#if IS_BCU_ROLE
	My_BMS_Role = BMS_ROLE_BCU;
	bmucontroltask_delay = 5000;
	//bcucellnum = CELL_NUM;
	//bcutempnum = 25;
	
#endif
	//***************************************״̬������ʼ�����********************************************
	
	
	//*****************************************��ģ�鹦�ܳ�ʼ��********************************************
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);        //�ж����ȼ�����4(�����ȼ�0~15,�����ȼ�0)
	delay_init(100);                                       //�Զ�����ʱ���ܳ�ʼ��
	IOControl_Init();                                      //IO���ƹ��ܳ�ʼ��(����BMSϵͳ���µ��LED��)
	RS485_DTU_Init(115200);                                //DTU�����ϴ����ܳ�ʼ��
	//SOC״̬���㹦�ܳ�ʼ��
	CAN1_Mode_Init();                                      //BMS�ڲ�ͨ�Ź��ܳ�ʼ��
	Extern_CAN_Init();                                     //BMS����ͨ�Ź��ܳ�ʼ��
	//���ϱ������ܳ�ʼ��
	//��ʷ�������ݴ洢���ܳ�ʼ��
	uart7_init(115200);                                    //������PC��λ���������ƹ��ܳ�ʼ��
	ADC_Inside_Init();                                     //��ʼ��STоƬ�ڲ�ADC��������(��ʱ����CC״̬���͹����ѹ����)
	IIC_Init();                                            //IICͨ�Ź��ܳ�ʼ��
	SDCard_Init();
#if IS_BMU_ROLE
	LTC6804_initialize();                                  //�����ѹ���ݡ��¶����ݲɼ����ܳ�ʼ��
	//�������ݲɼ����ܳ�ʼ��
#endif
#if IS_BCU_ROLE
	RS485_Screen_Init(9600);                               //��ʾ���������ܳ�ʼ��
	RS485_ElectMeter_Init(9600);                           //���������ݲɼ����ܳ�ʼ��
	RS485_Insulation_Init(9600);                           //��Ե���ģ�����ݲɼ����ܳ�ʼ��
	//Show_RelayState();                                     //���̵���1/2/3��״̬��ʾΪ0
#endif
	
	//BCU���Ƹ�ѹ�̵���(��3)�պ�
#if IS_BCU_ROLE
	IOControl(5,IO_CLOSE);  //��������ָʾ��
	
	
//	IOControl(3,IO_CLOSE);  //�ܸ��̵����պ�
//	IOControl(2,IO_CLOSE);  //Ԥ��̵����պ�
//	delay_ms(3000);
//	IOControl(1,IO_CLOSE);  //�����̵����պ�
	
	/*
	delay_ms(1000);
	IOControl(2,IO_DISCONNECT);  //Ԥ��̵����Ͽ�
	*/
#endif
	
  //***************************************��ģ�鹦�ܳ�ʼ�����********************************************
	
	
//��ӡ��ʾ���
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
	
/*	�ڡ���ʹ��ϵͳ���������,����Ϊ����ģ�����ʹ��;
*/	
#if !IS_USE_RTOS  // ��ʹ��ϵͳ�����
while(1) {
			
/* ��ʹ��ϵͳ�������,�˴�������BMU���Թ���ģ��ʹ�� */
#if IS_BMU_ROLE
			
			CAN1_ComControl(0x10,FT_COMCONTROL_HEARTBEAT);
			printf("Working...\r\n");
			GPIO_ToggleBits(GPIOD,GPIO_Pin_15);
			delay_ms(1000);
						
#endif

/* ��ʹ��ϵͳ�������,�˴�������BCU���Թ���ģ��ʹ�� */			
#if IS_BCU_ROLE
			
	//����IICͨ�Ź���
	//��ע��:��������·����������,������ڿ�����Ҫ���������ļ��Է�ת��
  //����ת:Ҫô�ڴ���������˫������ת��ģʽ,Ҫô�ڼ�����ʱ�������������ӡ�
	//����������:����״̬��,CH1-CH0�����0.15xV�ĵ�ѹ,��Ӧ���Ƿ��������Ŵ�25.7����ĵ�ѹֵ��

	iic_flag = 0;	
			
	
	//������������
	iic_flag = LTC2305_start_convert(3, LT2305_ADDR_CURR);  //����ADCת��
	delay_ms(1);
	u16data = LTC2305_read_result(3,LT2305_ADDR_CURR);  //��ȡ��������ת�����
	sidata = (short int)u16data;  //���޷�����ת��Ϊ�з�����
	//�������ת�����
	if(iic_flag == 0){
		printf("Current data:%d\r\n",sidata);
	}
	else{
		printf("Current data acq failed.\r\n");
	}		
			
			
			
			
			
			
			
/*	    //���ֲ��Թ��̴���		
//			//ͨ��DTU�ϴ��¶�����(25��,��BMU0��ǰ5��оƬ������)
//			for(bcu_loop_i=0;bcu_loop_i<5;++bcu_loop_i) {
//				RS485_DTU_UploadTempData(0, bcu_loop_i*5, (bcu_loop_i+1)*5);
//				delay_ms(20000);
//			}
//			//ͨ��DTU�ϴ��������(237��,Ŀǰ240��)
//			for(bcu_loop_i=0;bcu_loop_i<237;bcu_loop_i+=5) {
//				
//				RS485_DTU_UploadVolData(bcu_loop_i/120, bcu_loop_i,bcu_loop_i+5);  //�ϴ��������,5��Ϊ1��
//				
//				delay_ms(20000);
//			}
//			//ͨ��DTU�ϴ������������
//			for(bcu_loop_i=0;bcu_loop_i<2;++bcu_loop_i){
//				RS485_DTU_UploadCurrentCCSmokeData(bcu_loop_i, 1, 1, 2);
//				delay_ms(20000);
//			}
//			//ͨ��DTU�ϴ�BCU�˲���������
//			RS485_DTU_UploadBCUData(1, 1, 2, 3);
			
			
			//��ȡ��������
			//RS485_ElectMeter_ReadCurrentData();
			//delay_ms(2);
			
//			//���������ݷ��͸�BMU(�㲥)
//			CAN1_Send_Data(0xff, FT_DATA_CURRENT, 0, 0, 0, currentf);

			//��ȡ�����ѹֵ
//			tmppower = get_power_vol(5);
//			printf("Supply power is %fV (tmppower).\r\n",((float)tmppower)*0.001);
//			printf("Supply power is %fV (powervolu16).\r\n",((float)powervolu16)*0.001);
*/			

			//*********************�ȸ���ֵ,���ڲ���DTUģ��***********************
/*
//			for(loop_i=0;loop_i<10;loop_i++){  //��ؼ�����
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
			//**********************������ֵ���*********************************			
			
			printf("Working %ds.\r\n",i);
			i++;
			GPIO_ToggleBits(GPIOD,GPIO_Pin_15);
			delay_ms(1000);			
#endif
		}
#endif  //��ʹ��ϵͳ���������		


/* �ڡ�ʹ��ϵͳ���������:
	 - ��ʼ���󴴽���ʼ����,���Դ�����Ҫ���еĶ������
*/
#if IS_USE_RTOS  //ʹ��ϵͳ�����
		xTaskCreate((TaskFunction_t   )start_task,          //������
				  (const char*      )"start_task",        //��������
				  (uint16_t         )256,                 //�����ջ��С
				  (void*            )NULL,                //���ݸ��������Ĳ���
				  (UBaseType_t      )1,                   //�������ȼ�
				  (TaskHandle_t*    )&vStartTaskHandler);  //������
  vTaskStartScheduler();
#endif  /* IS_USE_RTOS */
}

/*
�ڡ�ʹ��ϵͳ�������,��������ľ���ִ�й���
*/
//**************************************��ʼ����:�����������***************************************
void start_task(void)
{
	taskENTER_CRITICAL(); // �����ٽ���
	
	//��0.1��������¼����ʱ��&��˸ָʾ������
	xTaskCreate((TaskFunction_t   )timerecord_task,          //������
							(const char*      )"timerecord_task",        //��������
							(uint16_t         )50,                 //�����ջ��С
							(void*            )NULL,               //���ݸ��������Ĳ���
							(UBaseType_t      )1,                  //�������ȼ�
							(TaskHandle_t*    )&vTimeRecordTaskHandler);  //������	
	
	//��0.2���������崮�ڴ�ӡ����(���ӡ����ʱ���)
	xTaskCreate((TaskFunction_t   )pcprint_task,          //������
							(const char*      )"pcprint_task",        //��������
							(uint16_t         )50,                 //�����ջ��С
							(void*            )NULL,               //���ݸ��������Ĳ���
							(UBaseType_t      )2,                  //�������ȼ�
							(TaskHandle_t*    )&vPcPrintTaskHandler);  //������									
	
	//��0.3������DTU�����ϴ�����(485ͨ��1)
	xTaskCreate((TaskFunction_t   )DTU_task,          //������
							(const char*      )"DTU_task",        //��������
							(uint16_t         )512,                 //�����ջ��С
							(void*            )NULL,               //���ݸ��������Ĳ���
							(UBaseType_t      )3,                  //�������ȼ�
							(TaskHandle_t*    )&vDTUTaskHandler);  //������
	
#if IS_BMU_ROLE  //BMUģ������
/* BMU��������ģ��:
  - ��1.1����ʱ�ɼ�����Χ�ڵ��¶����ݺ͵����ص�ѹ����;
  - ��1.2���ڲ�CANͨ�Ŷ�ʱ����������,�������ӵ�BCU��,Ӧ�û��յ�BUC��ȷ�ϰ�;
	- ��1.3����BMU�ڲ�CAN�ж��յ��ϴ����������,����Ӧ��־������Ϊ1,�ڸ�����������,����־����Ϊ1ʱ,��ʼ�ϴ�����,�ϴ����������ϴ�����֡,�����ñ���ֵΪ0;
  - ��1.4��BMU��������,��ĳ����ص�Ԫ���������,�жϸõ�ص�ǰ״̬�Ƿ�ﵽ��ǰֹͣ��������,�ﵽ��ر�;��BMU�ڲ�CAN�ж��յ����������,����/�رն�Ӧ��ص�Ԫ�ľ��⹦��;
  - ��1.5��BMU��ʾ��������,���ڲ�CAN�ж��н��յ���ʾ���������,����Ӧ��־������Ϊ1,�ڸ�������,����־����Ϊ1ʱ,��ʼ��ʾ����,��ʾ������,������ʾ����֡,�����ñ�����Ϊ0;
*/

	//��1.1������BMU���ݲɼ�����(�¶�|�����ѹ)
	xTaskCreate((TaskFunction_t   )bmu_dataacq_task,          //������
							(const char*      )"bmu_dataacq_task",        //��������
							(uint16_t         )1024,                 //�����ջ��С
							(void*            )NULL,               //���ݸ��������Ĳ���
							(UBaseType_t      )6,                  //�������ȼ�
							(TaskHandle_t*    )&vBMUDataAcqTaskHandler);  //������								
	
	//��1.2�������ڲ�CANͨ�ŷ�������������
	xTaskCreate((TaskFunction_t   )bmu_heartbeat_task,          //������
							(const char*      )"bmu_heartbeat_task",        //��������
							(uint16_t         )50,                 //�����ջ��С
							(void*            )NULL,               //���ݸ��������Ĳ���
							(UBaseType_t      )3,                  //�������ȼ�
							(TaskHandle_t*    )&vBMUHeartBeatTaskHandler);  //������							
	
	//��1.3������BMU�ϴ���������
	xTaskCreate((TaskFunction_t   )bmu_uploaddata_task,          //������
							(const char*      )"bmu_uploaddata_task",        //��������
							(uint16_t         )256,                 //�����ջ��С
							(void*            )NULL,               //���ݸ��������Ĳ���
							(UBaseType_t      )2,                  //�������ȼ�
							(TaskHandle_t*    )&vBMUUploadDataTaskHandler);  //������													
							
	//��1.4������BMU�����������
	xTaskCreate((TaskFunction_t   )bmu_balance_task,          //������
							(const char*      )"bmu_balance_task",        //��������
							(uint16_t         )256,                 //�����ջ��С
							(void*            )NULL,               //���ݸ��������Ĳ���
							(UBaseType_t      )5,                  //�������ȼ�
							(TaskHandle_t*    )&vBMUBalanceTaskHandler);  //������							
	
	//��1.5������BMU��ʾ��������
//	xTaskCreate((TaskFunction_t   )bmu_screen_task,          //������
//							(const char*      )"bmu_screen_task",        //��������
//							(uint16_t         )256,                 //�����ջ��С
//							(void*            )NULL,               //���ݸ��������Ĳ���
//							(UBaseType_t      )2,                  //�������ȼ�
//							(TaskHandle_t*    )&vBMUScreenTaskHandler);  //������
	
#endif

#if IS_BCU_ROLE  //BCUģ������						
/* BCU��������ģ��:
	- ��2.1�����CC״̬(��Ӧ����ͨ�Ź���);
  - ��2.2������Ե״̬;
	- ��2.3������ѹ״̬;
	- ��2.4����⹩���ѹ;
	- ��2.5��������ֵ;
	- ��2.6�����ƹ���(���Ƹ�BMU�����ϴ�����,�Ƚϵõ����/��С����,���ݵ�ǰ���ݹ���SOC,ִ�о������,���ϼ�鼰����,��ʷ���ݼ�¼(�Ե������ݵ�1��Ϊ����));
  - ��2.7��BMU��ʾ���ݿ���;
  - ��2.8���������CAN;
*/
						
	//��2.1������CC״̬�������
	// xTaskCreate((TaskFunction_t   )ccacq_task,          //������
	// 						(const char*      )"ccacq_task",        //��������
	// 						(uint16_t         )128,                 //�����ջ��С
	// 						(void*            )NULL,               //���ݸ��������Ĳ���
	// 						(UBaseType_t      )2,                  //�������ȼ�
	// 						(TaskHandle_t*    )&vCCAcqTaskHandler);  //������	

//	//��2.2��������Ե״̬�������
//	xTaskCreate((TaskFunction_t   )insulation_task,          //������
//							(const char*      )"insulation_task",        //��������
//							(uint16_t         )128,                 //�����ջ��С
//							(void*            )NULL,               //���ݸ��������Ĳ���
//							(UBaseType_t      )3,                  //�������ȼ�
//							(TaskHandle_t*    )&vInsulationTaskHandler);  //������
	
	//��2.3��������ѹ״̬�������
//	xTaskCreate((TaskFunction_t   )hvacq_task,          //������
//							(const char*      )"hvacq_task",        //��������
//							(uint16_t         )128,                 //�����ջ��С
//							(void*            )NULL,               //���ݸ��������Ĳ���
//							(UBaseType_t      )3,                  //�������ȼ�
//							(TaskHandle_t*    )&vHVAcqTaskHandler);  //������

//	//��2.4�����������ѹ�������
//	xTaskCreate((TaskFunction_t   )poweracq_task,          //������
//							(const char*      )"poweracq_task",        //��������
//							(uint16_t         )128,                 //�����ջ��С
//							(void*            )NULL,               //���ݸ��������Ĳ���
//							(UBaseType_t      )4,                  //�������ȼ�
//							(TaskHandle_t*    )&vPowerAcqTaskHandler);  //������								
//	
//	//��2.5�������������ݲɼ�����
//	xTaskCreate((TaskFunction_t   )currentacq_task,          //������
//							(const char*      )"currentacq_task",        //��������
//							(uint16_t         )128,                 //�����ջ��С
//							(void*            )NULL,               //���ݸ��������Ĳ���
//							(UBaseType_t      )3,                  //�������ȼ�
//							(TaskHandle_t*    )&vCurrentAcqTaskHandler);  //������		
	
	//��2.6���������ƹ�������
//	xTaskCreate((TaskFunction_t   )bcucontrol_task,          //������
//							(const char*      )"bcucontrol_task",        //��������
//							(uint16_t         )128,                 //�����ջ��С
//							(void*            )NULL,               //���ݸ��������Ĳ���
//							(UBaseType_t      )2,                  //�������ȼ�
//							(TaskHandle_t*    )&vBCUControlTaskHandler);  //������		

//��2.7������BMU��ʾ���ݿ�������
//	xTaskCreate((TaskFunction_t   )bcu_screen_task,          //������
//							(const char*      )"bcu_screen_task",        //��������
//							(uint16_t         )128,                 //�����ջ��С
//							(void*            )NULL,               //���ݸ��������Ĳ���
//							(UBaseType_t      )2,                  //�������ȼ�
//							(TaskHandle_t*    )&vBCUScreenTaskHandler);  //������	

// ��2.8�������ⲿCAN							
	xTaskCreate((TaskFunction_t   )externcan_task,          //������
							(const char*      )"externcan_task",        //��������
							(uint16_t         )256,                 //�����ջ��С
							(void*            )NULL,               //���ݸ��������Ĳ���
							(UBaseType_t      )4,                  //�������ȼ�
							(TaskHandle_t*    )&vExternCanTaskHandler);  //������	
	
#endif													
	
	//ɾ����ʼ����
	vTaskDelete(vStartTaskHandler);
	taskEXIT_CRITICAL();  //�˳��ٽ���
	
} //************************************��ʼ�������***********************************************



//��0.1��LEDָʾ������:ÿ��1s�ı�ָʾ��״̬,��ʾϵͳ��������
void timerecord_task(void)
{
	while(1) {
		GPIO_ToggleBits(GPIOD,GPIO_Pin_15);  //BMS���ذ����ſ���ָʾ��
		++runtime;
		++standtime;
		vTaskDelay(1000);
	}
}


//��0.3��DTU�����ϴ�����
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
		vTaskDelay(10000);           //10s����һ������
	}
}

//��0.2��
//ͨ��UART7������PC��λ����ӡ��־,�����н���
//��ʱ:ÿ��5s��ӡ����ʱ��
void pcprint_task(void)
{
	//u32 i=0;
	while(1) {
		printf("Time: %ds.\r\n",runtime);
		//i = i + 5;
		vTaskDelay(5000);
	}
}


#if IS_BMU_ROLE /* BMU��������ģ��ʵ�� */

//��1.1������BMU���ݲɼ�����(�¶�|�����ѹ)
void bmu_dataacq_task(void)
{
	u8 chip,loop_i,loop_j,i;
	uint16_t tmp_cellvol[1][12];  //��ʱ�����ȡ���ĵ�ѹֵ
	uint16_t tmp_temp[1][6];  //��ʱ�����ȡ����GPIOģ������ֵ(6��ֵ,ǰ5��ΪGPIO�˿ڵ�ѹֵ)	
	
	while(1){
		
#if BMU_PRINT_P  //������Ȩ��,���ڸ�����������¶�ת�����
		printf("BMU0x%02X dataacq.\r\n",My_Id);
#endif		
		
		taskENTER_CRITICAL(); // �����ٽ���
		//��ȡ3��оƬ���¶�����(ÿ��оƬ4���¶�����,��3��оƬ,12������)
		for(chip=0;chip<3;++chip) {
			readtemp(chip);
		}
		taskEXIT_CRITICAL();  //�˳��ٽ���		
		
//#if BMU_PRINT_P  //������Ȩ��,���ڸ�����������¶�ת�����
//		printf("Temp 0x%02X\r\n",My_Id);
//#endif
//		for(loop_i=0;loop_i<BMU_TEMP_NUM;++loop_i){
//			//if((temp[loop_i]>0) && (temp[loop_i]<65535))
//			{
////#if BMU_SCREEN_P
////				//485������������ʾ��
////#endif				
////#if BMU_PRINT_P
////				printf("Temp %d: %d   %f\r\n",loop_i,temp[loop_i],tempf[loop_i]);
////#endif				
//			}
//		}

    vTaskDelay(dataacq_time/2);
		
		//��ȡ4��оƬ�ĵ����ѹ����
		taskENTER_CRITICAL(); // �����ٽ���
		for(chip=0;chip<4;chip++) {
			readvol(chip);  //��ȡ��chipƬ�ɼ�оƬ�ĵ�ѹ,������ȡ�������ݱ��浽ȫ�ֱ���cellvol[chip][]��
		}
		taskEXIT_CRITICAL();  //�˳��ٽ���
		
//#if BMU_PRINT_P  //������Ȩ��,���ڸ������������ѹת�����
//		printf("Volt 0x%02X\r\n",My_Id);
//#endif		
//		for(loop_i=0;loop_i<BMU_VOL_NUM;++loop_i){
//			//if((cellvol[loop_i]>0) && (cellvol[loop_i]<65535))
//			{
////#if BMU_SCREEN_P
////        //485������������ʾ��
////#endif				
////#if BMU_PRINT_P
////				printf("Vol %d: %f\r\n",loop_i,((float)cellvol[loop_i])*0.0001);
////#endif
//			}
//		}
		
		vTaskDelay(dataacq_time/2);
		
	}
}

//��1.2�������ڲ�CANͨ�ŷ�������������
void bmu_heartbeat_task(void)
{
	while(1){
		CAN1_ComControl(My_Superior_Id,FT_COMCONTROL_HEARTBEAT);
		
		vTaskDelay(heartbeat_time);
	}
}

//��1.3������BMU�ϴ���������
void bmu_uploaddata_task(void){
		
	u8 chip,loop_i;
	
	while(1){
		
		if(bmu_uploaddata==1)
		{
			vTaskDelay(1);
			
			#if BMU_PRINT_P			
			  printf("Upload data.\r\n");	
      #endif
			
			//�ϴ��¶�����ֵ
			for(loop_i=0;loop_i<BMU_TEMP_NUM;++loop_i){
				//if((temp[loop_i]>0)&&(temp[loop_i]<65536))  //���ϴ�����֮ǰ��ɸѡ��"����"������
				{
					//����֡��Ӧ�����������
					CAN1_Send_Data(My_Superior_Id, FT_DATA_TEMP, My_Pack_Number, loop_i, 0, tempf[loop_i]);
					vTaskDelay(1);
				}
			}
			
			//�ϴ�����������
			for(loop_i=0;loop_i<BMU_VOL_NUM;++loop_i){
				//if((cellvol[loop_i]>4000)&&(cellvol[loop_i]<65535))
				{
					//����֡��Ӧ�����������
					CAN1_Send_Data(My_Superior_Id, FT_DATA_CELL, My_Pack_Number, loop_i, cellvol[loop_i], 0);  //�����ѹ
					//CAN1_Send_Data(My_Superior_Id, FT_DATA_CELL, My_Pack_Number, loop_i, cellvol[loop_i], soc[loop_i]);  //�����ѹ�͵���SOC����
					vTaskDelay(1);
				}
			}
			
			//�ϴ��������������Ϣ
			CAN1_ControlCmd(My_Superior_Id, FT_CONTROLCMD_ACK, CONTROLCMD_ACK_UPLOAD_COMPLETE, 0xFF, 0xFFFF);
			
			//printf("Upload complete.\r\n");
			
			bmu_uploaddata = 0;
			
		}
		vTaskDelay(1);
	}
}

/*��1.4������BMU�����������
- ����:
  - �ж��Ƿ�رվ���:���ݵ�ǰ��balancetargetvol(����Ŀ���ѹ)�͵�ǰʵ�ʵ�ѹֵ,�ж�BalanceFlag��Ϊ1(��������)�ĵ��,��ǰ��ѹ�Ƿ��Ѵﵽ����Ŀ���ѹ,�ﵽ���򽫶�Ӧ��1��Ϊ0;
  - ����һ��:�����ж�BalanceFlag��BalanceState�е�ÿһ���Ƿ�һ��,��һ������LT6804оƬ��������,����Ҫ�رյĹر�(�ﵽ����Ŀ���ѹ/�ж�������ر�),����Ҫ�����Ŀ���(�ж��������);
*/
void bmu_balance_task(void)
{
	u8 loop_i,resultflag;
	
	while(1){
		
		//�ж��Ƿ�رվ���
		for(loop_i=0;loop_i<BMU_VOL_NUM;++loop_i){
			if(BalanceFlag[loop_i]==1){   //����ѿ�������
				if(cellvol[loop_i]<balancetargetvol){  //�ﵽ����Ŀ���ѹ
					BalanceFlag[loop_i] = 0;  //���Ϊ��Ҫ�ر�
				}
			}
		}
		
		//����һ��
		for(loop_i=0;loop_i<BMU_VOL_NUM;++loop_i){
			if(BalanceFlag[loop_i] != BalanceState[loop_i]){

				resultflag = set_cell_discha
				rge(loop_i,BalanceFlag[loop_i]);  //ֱ��ʹ��BalanceFlag״̬��������/�رվ���
				
				if(resultflag==1){  //������ĳɹ�
					BalanceState[loop_i] = BalanceFlag[loop_i];  //�޸ĵ�ǰ����״̬����
				}
			}
		}
		
		vTaskDelay(10);  //��ʱ10ms
	}

//��1.5������BMU��ʾ��������
void bmu_screen_task(void)
{
	u8 loop_i;
	
	while(1){
	
		if(bmu_displaydata==1){
			#if BMU_PRINT_P			
			  printf("Display data.\r\n");
      #endif
			
			//��ʾ�¶�����ֵ
			for(loop_i=0;loop_i<BMU_TEMP_NUM;++loop_i){
				//if((temp[loop_i]>0)&&(temp[loop_i]<65536))  //���ϴ�����֮ǰ��ɸѡ��"����"������
				{
					//����ʾ����ʾ���¶�����
					RS485_Screen_WriteMem(get_tempaddr(My_Pack_Number,loop_i),(u16)(tempf[loop_i]*10));					
					vTaskDelay(1);					
				}
			}
			
			//��ʾ����������
			for(loop_i=0;loop_i<BMU_VOL_NUM;++loop_i){
				//if((cellvol[loop_i]>4000)&&(cellvol[loop_i]<65535))
				{
					//����ʾ����ʾ�õ�ص�ѹ����
					RS485_Screen_WriteMem(get_celladdr(My_Pack_Number,loop_i),cellvol[loop_i]);		
					
					vTaskDelay(1);
				}
			}

			//�ϴ��������������Ϣ
			CAN1_ControlCmd(My_Superior_Id, FT_CONTROLCMD_ACK, CONTROLCMD_ACK_DISPLAY_COMPLETE, 0xFF, 0xFFFF);
			
			//printf("Display complete.\r\n");			
			
			bmu_displaydata = 0;
		}
		vTaskDelay(1);
	}
}

#endif

#if IS_BCU_ROLE /* BCU��������ģ��ʵ�� */


/*��2.1������CC�źż������
ͨ��оƬ�ڲ���ADC����ʵ��CC״̬���Ĺ���,BMS������CC2��״̬����;
CC״̬: 12V:δ����, 6V:������;
�˴����ӵ���20k��100k�ķ�ѹ,�����δ����,���⵽�ĵ�ѹӦΪ12V*2/12=2V,���������,���⵽�ĵ�ѹӦΪ6V*2/12=1V;
*/
void ccacq_task(void)
{
	u8 cc;
	u32 i=0;
	u8 flag;
	
	BMS_Enable();  //����ΪBMSģʽ
	MODE = 1;  //��û�м�⵽CC״̬,������Ϊ����ģʽ
	
	while(1){
		cc = get_cc_state(10);
		
		//����CCĿǰCC״̬���ö���ͨ��ģʽ
		if(cc==CC_STATE_CONNECTED){
			MODE = 0;  //BMS״̬
			RS485_Screen_WriteMem(CC_STATE_ADDR,1);  //��ʾCC״̬Ϊ1(������)
			
			//ϵͳ�ϵ�(�̵����պ�)
			
		}
		else{
			MODE = 1;  //����״̬
			RS485_Screen_WriteMem(CC_STATE_ADDR,0);  //��ʾCC״̬Ϊ0(δ����)			
			
			//ϵͳ�µ�(�̵����Ͽ�)
			
		}
		
		//������ʾCC2��ѹ״̬
		if(i++%50==0){  //ÿ5����ʾһ��
			if(cc==CC_STATE_CONNECTED)
				printf("CC2 connected.\r\n");
			else{
//				//����				
//				flag = Extern_CAN_SendMsg(0, 0, "12345678", 8);
//				if(flag==1)
//					printf("CAN2 trans failed.");
				printf("CC2 disconnect.\r\n");
			}
		}
		
		vTaskDelay(100);
		
	}	
}

/*��2.2��������Ե״̬�������
- ��������:ͨ��������������ֱ������֮���Ե���Ƿ����100ŷķ/V,���жϵ��������Ƿ�©��
- ��ѹ:��Ե����ѹ��Χ:200-1500V
- ����:�����������10s,�쳣�������20ms,©������⾫��
- ��Ե©���1������:500ohm/V,2������200ohm/V,3������100ohm/V
*/
void insulation_task(void)
{
	u8 insuflag = 0;
	
	//ͨ�����ôﵽÿ��11s��ȡһ�ξ�Ե���ݵ�Ŀ��
	while(1) {
		if(insuflag==0){
			insuflag = 1;
			RS485_Insulation_OpenRead();  //������������ݶ�ȡ
		}
		else{
			insuflag = 0;
		}
		
		vTaskDelay(8000);;  //Ӧ��ʱԼ6������ģ���ȡ����ȷ����
		
		RS485_Insulation_ReadData();  //���Ͷ���������,��ʹ���жϽ���ģ������
		vTaskDelay(5);
		
		//����ʾ�����;�Ե״̬����(ŷķ/��)
//		RS485_Screen_WriteMem(INSUSTATE_POS_ADDR,(u16)insuval_pos);
//		RS485_Screen_WriteMem(INSUSTATE_NEG_ADDR,(u16)insuval_neg);	
		
		//����ʾ�����;�Ե״̬����(ǧŷ)(�����������һ�¾�Եģ�����ȷ��)
		RS485_Screen_WriteMem(INSUSTATE_POS_ADDR,res_pos_data);
		RS485_Screen_WriteMem(INSUSTATE_NEG_ADDR,res_neg_data);	
		
		vTaskDelay(2995);  //��ģ��ָ�����������ǰ״̬
	}		
}

//��2.3��������ѹ״̬�������
void hvacq_task(void)
{
	
	IIC_StartHVConvert(0);  //����B+(CH0)����ģʽ
		vTaskDelay(1);
		IIC_ReadHVData(0);  //��ȡB+(CH0)ת���������
		vTaskDelay(1);
		IIC_StartHVConvert(1);  //����HV1(CH1)����ģʽ
		vTaskDelay(1);
		IIC_ReadHVData(1);  //��ȡHV1(CH1)ת���������	
		
		vTaskDelay(1000);
}

/*��2.4�����������ѹ�������
ͨ��оƬ�ڲ� ADC����ʵ�ֹ����ѹ�ļ��,��getpower_vol�����в�����5��;
������BCU V1.3��ʹ�õ���8��ѹת��24V->3V,�������BCU V1.3����������,�����BCU V1.2��ʹ��12V����Ļ�,����ʾ��16V����,12V->2V(6��ѹ)->16V(8��ѹ);
*/
void poweracq_task(void)
{
	u32 i=0;
	u16 powertmpu16 = 0;
	
	while(1) {
		get_power_vol(10);  //��ȡ5�β��������ѹ��ƽ��ֵ
		
		//������������ʾ����ʾ
		powertmpu16 = (u16)(powervolf*100);  //��ȡxx.xx����Чλ��
		RS485_Screen_WriteMem(POWER_VOL_ADDR,powertmpu16);  //���͵���ʾ��
		
		//������ʾ�����ѹ����
		if(i++%50==0){
			printf("Power: %f\r\n",powervolf);
		}
		vTaskDelay(1000);
		
	}	
}

//��2.5�������������ݼ������
void currentacq_task(void)
{
	//��ȡ�ĵ�������currentf������
	//ͨ������ȡ��������
	u32 i=0;
	u16 currenttmpu16;
	
	while(1) {
		//�����ܱ�,������ȡ������µ�current[]��currentfȫ�ֱ�����
		RS485_ElectMeter_ReadCurrentData();	
		vTaskDelay(5);
		
		
		//ͨ��ADCоƬ��ȡ��������
		IIC_StartCurrentConvert(1);  //�������趨CH0Ϊ��,ʵ��Ŀǰ���ӽӷ���,CH0�Ǹ�
		vTaskDelay(1);
		IIC_ReadCurrentData();
		
		
		
		
		
		
		
		
		currenttmpu16 = (u16)(currentf*100);
		RS485_Screen_WriteMem(TOTAL_CURR_ADDR,currenttmpu16);
		
		if(i++%50==0){  //x����ʾһ��
			printf("Current: %f.\r\n",currentf);
		}
		
		//���ݵ�����С�ж��Ƿ�Ϊ����״̬
		if(currentf>=0){
			if(currentf>STANDCURRENT_LINE){  //����ֵ������ֵ
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

//��2.6������BCU���ƹ�������
void bcucontrol_task(void)
{
	while(1) {
		//��2.6.1����ȡBMU����
//		bcu_can1_ctrlandacq();
		
		//��2.6.2���Ƚϵ�������,������/��С����
//		datacompare();
		
		//��2.6.3�����㵱ǰ����SOC״̬
//		soccalculate();	
		
		//��2.6.4��ִ�о������
//		bcu_balance();
		
		//��2.6.5�����ϼ��(���������������μ��һ��)
//		faultalarm();	
		
		//��2.6.6����ʷ���ݼ�¼(��һ�ֵ�һ�ε������ݺͶ�������Ϊ׼���м�¼)
//		historydatastore();	
		
		vTaskDelay(bmucontroltask_delay);
	}
}

//��2.6.1�����Ƹ����ӿ�,��ȡ��Ӧ��������(�����ϴ�����,��һ��ʱ��û�л�Ӧ������,�����Ϊʧ��,��ʡʱ���Ҽ����״̬)
void bcu_can1_ctrlandacq(void)
{
	u8 loop_i,loop_j;
	u16 timecount;
	
	printf("Upload ctrl fun.\r\n");
	
	for(loop_i=0;loop_i<BMU_LIST_NUM;++loop_i){  //����BMU�б�

	  is_bmu_uploading = 0;          //��������ϴ���־
		
		if(BMU_List_State[loop_i]==1){  //���ڵ����
			
			printf("BMU 0x%02X uploading...\r\n",BMU_List_Id[loop_i]);
			
			CAN1_ControlCmd(BMU_List_Id[loop_i],FT_CONTROLCMD_UPLOADDATA,0xFF,0xFF,0xFFFF);		//��BMU�����ϴ���������
			
			is_bmu_uploading = 1;  //��ʶΪ�����ϴ�����
			timecount = 0;         //��ʱҲ��Ϊ�������
			
			//��ʱ�ȴ���������
			while((is_bmu_uploading==1)&&(timecount<100)){  //û�ϴ������û��ʱ
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

  //�����ܵ�ѹ
  //data2822�������������ص�ǰ��ѹ
}

//��2.6.2���Ƚϵ�������,������/��С����
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
	//�Ƚϵ����ѹ����
	for(loop_i=0;loop_i<BMU_LIST_NUM * BMU_VOL_NUM;++loop_i){  //ѭ�������ѹ����
		if((cellvol[loop_i]>4000)&&(cellvol[loop_i]<39000)){  //��������"����"����
			if(cellvol[loop_i]>tmpmaxvol){  //������ѹ
				tmpmaxvol = cellvol[loop_i];
				tmpmaxvolid = loop_i;
			}
			if(cellvol[loop_i]<tmpminvol){  //��С�����ѹ
				tmpminvol = cellvol[loop_i];
				tmpminvolid = loop_i;
			}
			++tmpvolnum;
			tmpvoltotal += cellvol[loop_i];
		}
	}
	
	tmpmaxtempf = -15.0;
	tmpmintempf = 65.0;
	//�Ƚ��¶�����
	for(loop_i=0;loop_i<BMU_LIST_NUM * BMU_TEMP_NUM;++loop_i){
		if((tempf[loop_i]>(-10.0))&&(tempf[loop_i]<60.0)){  //��������"����"����
			if(tempf[loop_i]>tmpmaxtempf){  //����¶�����
				tmpmaxtempf = tempf[loop_i];
				tmpmaxtempfid = loop_i;
			}
			if(tempf[loop_i]<tmpmintempf){  //��С�¶�����
				tmpmintempf = tempf[loop_i];
				tmpmintempfid = loop_i;
			}
			++tmptempfnum;
			tmptempftotal += tempf[loop_i];
		}		
	}
	
	//���浥���ѹ�ȽϽ��
	cellvoltotal = tmpvoltotal;
	max_cellvol = tmpmaxvol;
	max_cellvolid = tmpmaxvolid;
	min_cellvol = tmpminvol;
	min_cellvolid = tmpminvolid;
	cellvolaverage = tmpvoltotal / tmpvolnum;
	max_diff_cellvol = tmpmaxvol - tmpminvol;
	
	//�����¶ȱȽϽ��
	max_tempf = tmpmaxtempf;
	max_tempfid = tmpmaxtempfid;
	min_tempf = tmpmintempf;
	min_tempfid = tmpmintempfid;
	tempfaverage = tmptempftotal / tmptempfnum;
	max_diff_tempf = tmpmaxtempf - tmpmintempf;

  //��������µ�����CAN
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

//��2.6.3�����㵱ǰ����SOC״̬
void soccalculate(void)
{
	if(standtime>STANDTIME_LINE){  //����龲��ʱ������趨����ֵ,��ʱ������ڵ���������ֵʱ����
		soctotal = soc_calibrate(min_cellvol);  //����С��ѹֵ���б궨
		xk = soctotal;  //��SOC������̳�ʼ��
		Pk = 0;
	}
	else{
		soc_calculate(currentf, min_cellvol,((float)bmucontroltask_delay)*0.001);
	}

  //�����ͬ��������CAN
  //data2821
  //data3078
  //data3079
  //data3601
}

/*��2.6.4��ִ�о������
  - ���ݵ�ǰ״̬,���ƾ����Ƿ���;
  - �������Ŀ���ѹ(�������ֵ)
    - ������͵�ѹ+����(�������)������ѹ��ֵ;
    - (������ߵ�ѹ+������͵�ѹ)/2;
    - �����ѹ�����ֵ;
*/
void bcu_balance(void)
{
	u16 tmpbalancetargetvol;
	u8 loop_i,pknum,pknum_1,cellid;

	u8 tmp_celltobalance_id[BALANCE_NUM];       //���ξ�����id(Ŀǰ��������Ľڵ�ID,��bcu_balance()�����л����)
	u16 tmp_celltobalance_vol[BALANCE_NUM];     //���ξ����ص�ѹ	
	u8 tmp_balanceenableflag = 0;               //�����Ƿ�������(Ĭ�ϲ�����)
	
	VoltLimitFlag = 0;           //�����ѹ������
	ModeValidFlag = 0;           //�����ϵͳ����״̬����
	CurrentLimitFlag = 0;        //����ĵ�����С����

	balancetargetvol = 50000;    //Ĭ�ϲ���������ʱ�ľ����ֹ��ѹ
	
	for(loop_i=0;loop_i<BALANCE_NUM;++loop_i){  //��ձ��εľ����б�
		tmp_celltobalance_id[loop_i] = 0xFF;
		tmp_celltobalance_vol[loop_i] = 0;
	}
	
	//�жϾ����Ƿ���
	if((max_cellvol-min_cellvol)>VoltLimitCalb){  //ѹ����ڱ궨ֵ
		VoltLimitFlag = 1;
	}
	if(currentf>0){  //��������0,���ڳ��״̬
		ModeValidFlag = 1;
	}
#if RUN_BALANCE  // BMS���м��ɿ�������,����һ�����ڳ��״̬
	ModeValidFlag = 1;
#endif
	if(abs(currentf)<CurrentLimitCalb){  //����ĵ�����С�����޶�
		CurrentLimitFlag = 1;
	}
	
	//������ξ��⿪��
	if((VoltLimitFlag+ModeValidFlag+CurrentLimitFlag)==3) {
		tmp_balanceenableflag = 1;  //��Ǳ��ξ��⿪��
		
		//�������Ŀ���ѹ(�������ֵ)
		balancetargetvol = min_cellvol + VoltLimitCalb;
		if((max_cellvol+min_cellvol)>(2*balancetargetvol)){
			balancetargetvol = (max_cellvol+min_cellvol)/2;  //(������ߵ�ѹ+������͵�ѹ)/2
		}
		if(balancetargetvol<MinCellVolLimit){
			balancetargetvol = MinCellVolLimit;
		}

    //���ɵ����ѹ�ṹ��
    for(loop_i=0;loop_i<bcucellnum;++loop_i){
      cell[loop_i].id = loop_i;
      cell[loop_i].vol = cellvol[loop_i];
    }

		//�ҳ���Ҫ���������BALANCE_NUM�����ID,������celltobalance_id[]��(������Ҫȥ�������Ѿ��ﵽ����Ŀ���ѹ�ĵ��)
		quick_sort(cell, bcucellnum);
    for(loop_i=0;loop_i<BALANCE_NUM;++loop_i){
      celltobalance_id[loop_i] = cell[loop_i].id;
    }
		
		//���������������;���Ŀ���ѹ���͸������ҵ���BALANCE_NUM���������BMU
		// for(loop_i=0;loop_i<BALANCE_NUM;++loop_i) {
		// 	pknum_1 = celltobalance_id[loop_i]/BALANCE_NUM;  //packnum-1(packnum��1��ʼ)
		// 	cellid = celltobalance_id[loop_i]%BALANCE_NUM;
		// 	CAN1_ControlCmd(BMU_List_Id[pknum_1],FT_CONTROLCMD_CELLBALANCE,CONTROLCMD_CELLBALANCE_ON,cellid,balancetargetvol);  //���Ϳ�����������
		// }
	}		
	
  //���ǰһʱ�̿����˾���,��ر�֮ǰ������BALANCE_NUM����صľ���
	if(BalanceEnableFlag==1){
		for(loop_i=0;loop_i<BALANCE_NUM;++loop_i) {  //���رվ��������͸��ϴ��ҵ���BALANCE_NUM��������ڵ�BMU
			pknum_1 = celltobalance_id[loop_i]/BMU_VOL_NUM;  //id��Χ:0-BMU_LIST_NUM*BMU_VOL_NUM
			cellid = celltobalance_id[loop_i]%BMU_VOL_NUM;
			CAN1_ControlCmd(BMU_List_Id[pknum_1],FT_CONTROLCMD_CELLBALANCE,CONTROLCMD_CELLBALANCE_OFF,cellid,0xFFFF);  //���͹رվ�������
			vTaskDelay(1);
		}		
	}
		
	//������ξ��⿪��,�������ҵ���BALANCE_NUM����صľ���
	if(tmp_balanceenableflag==1){
		for(loop_i=0;loop_i<BALANCE_NUM;++loop_i) {  //���������������;���Ŀ���ѹ���͸������ҵ���BALANCE_NUM���������BMU
			pknum_1 = tmp_celltobalance_id[loop_i]/BMU_VOL_NUM;
			cellid = tmp_celltobalance_id[loop_i]%BMU_VOL_NUM;
			CAN1_ControlCmd(BMU_List_Id[pknum_1],FT_CONTROLCMD_CELLBALANCE,CONTROLCMD_CELLBALANCE_ON,cellid,balancetargetvol);  //���Ϳ�����������
			vTaskDelay(1);
		}
	}
	
	//����BCU�д洢�ĵ�������־,��������һʱ�̵ľ�����Ʊ�־
	BalancePrevEnableFlag = BalanceEnableFlag;
	for(loop_i=0;loop_i<BALANCE_NUM;++loop_i){
		celltobalance_previd[loop_i] = celltobalance_id[loop_i];
		celltobalance_prevvol[loop_i] = celltobalance_vol[loop_i];
	}			
}


/*��2.6.5�����ϼ��(���������������μ��һ��)
  - ��ʵ��:�����ѹ����,�ŵ���ѹ����,��絥���ѹ,��絥���ѹ,�ŵ絥���ѹ,�ŵ絥���ѹ,���ѹ��,�ŵ�ѹ��,SOC��,��Ե©�� �����ж�
*/
void faultalarm(void)
{
  u8 state;
	if(alarmfun_soclow==FAULTALARM_OPEN){  //���ϼ��:SOC��
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
	if(alarmfun_voltotalhigh==FAULTALARM_OPEN){  //���ϼ��:�����ѹ����
  /*
    //�������е��ⲿ�ṩ�ĵ�ѹ
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
	if(alarmfun_voltotallow==FAULTALARM_OPEN){  //���ϼ��:�ŵ���ѹ����
	/*
    //�ŵ���ѹֵ��cellvoltotal��voltotal[2]?
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
	if(alarmfun_cvolhigh==FAULTALARM_OPEN){  //���ϼ��:��絥���ѹ
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
	if(alarmfun_cvollow==FAULTALARM_OPEN){  //���ϼ��:��絥���ѹ
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
	if(alarmfun_cvoldiff==FAULTALARM_OPEN){  //���ϼ��:���ѹ��
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
  if(alarmfun_ctemphigh==FAULTALARM_OPEN){  //���ϼ��:����¶ȹ���
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
	if(alarmfun_covercurr==FAULTALARM_OPEN){  //���ϼ��:������
	  /*
    //�������е���ֵ��
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
	if(alarmfun_insuleak==FAULTALARM_OPEN){  //���ϼ��:��Ե
  	/*
    //��Ե����Ӧ������ֵ��
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

//��2.6.6����ʷ���ݼ�¼(��һ�ֵ�һ�ε������ݺͶ�������Ϊ׼���м�¼)(��ÿ�ֵĵ�ѹ���¶Ƚ���д��sd��)
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

//��2.7������externcan
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


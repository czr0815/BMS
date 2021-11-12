#include "Charger.h"
#include "BMS.h"
#include "extern_can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"

/*
  CHM
	data2600,充电机通信协议版本号
*/
uint32_t data2600;
/*
  CRM
	data2560,辨识结果
	data2561,充电机编号
*/
uint8_t data2560;
uint32_t data2561;
/*
  CTS,暂时不作设置
	data2823,时间同步信息
*/
uint64_t data2823;
/*
  CML
	data2824,最高输出电压
	data2825,最低输出电压
	data2826,最大输出电流
	data2827,最小输出电流
*/
uint16_t data2824, data2825, data2826, data2827;
/*
  CRO
	data2830,充电机是否准备好
*/
uint8_t data2830;
/*
  CCS
	data3081,电压输出值
	data3082,电流输出值
	data3083,累计充电时间
	data3929，充电允许
*/
uint16_t data3081, data3082, data3083;
uint8_t data3929;
/*
  CST
	data3521,充电机中止充电原因
	data3522,充电机中止充电故障原因
	data3523,充电机中止充电错误原因
*/
uint8_t data3521, data3523;
uint16_t data3522;
/*
  CSD
	data3611,累计充电时间(min)
	data3612,输出能量(kW*h)
	data3613,充电机编号
*/
uint16_t data3611, data3612;
uint32_t data3613;
/*
  CEM
	data3921,接收BMS和车辆的报文超时
	data3922,接收电池充电参数报文超时
	data3923,接收BMS完成充电准备报文超时
	data3924,接收电池充电总装啊提报文超时
	data3925,接收电池要求报文超时
	data3926,接收BMS中止充电报文超时
	data3927,接收BMS充电统计报文超时
*/
u8 data3921, data3922, data3923, data3924, data3925, data3926, data3927;

//任务句柄
TaskHandle_t SendCHMTaskHandler;
TaskHandle_t SendCRMTaskHandler;
TaskHandle_t SendCTSTaskHandler;
TaskHandle_t SendCMLTaskHandler;
TaskHandle_t SendCROTaskHandler;
TaskHandle_t SendCCSTaskHandler;
TaskHandle_t SendCSTTaskHandler;
TaskHandle_t SendCSDTaskHandler;
TaskHandle_t SendCEMTaskHandler;

//发送时间
u32 CHM_Tick, CRM_Tick, CTS_Tick, CML_Tick, CRO_Tick, CCS_Tick, CST_Tick, CSD_Tick;

//Flag
uint8_t CHM_Flag, CRM_Flag, CTS_Flag, CML_Flag, CRO_Flag, CCS_Flag, CST_Flag, CSD_Flag;

/*
  根据参数组编号和目标地址生成扩展帧的29位标识符
*/
static void Get_Identifier(uint32_t *stdId, uint32_t *extId, uint8_t pri, uint8_t parm, uint8_t des, uint8_t dep){
	uint32_t pgn = 0;
	pgn |= dep;
	pgn |= (des<<8);
	pgn |= (parm<<16);
	pgn |= (pri<<26);
	*stdId = (pgn>>11) & 0x3FFFF;
	*extId = pgn & 0x7FF;
}

void send_CHM_task(){
	CHM_Flag = 1;
	CRM_Flag = 0;
	while(1){
		uint32_t stdId, extId;
		uint8_t priority = 6, buf[3], len = 3;
		buf[0]=(data2600>>16)&0xFF;  buf[1]=(data2600>>8)&0xFF;  buf[2]=data2600&0xFF;
		Get_Identifier(&stdId, &extId, priority, CHM_Param, BMS_Address, Charger_Address);
		Extern_CAN_SendMsg(stdId, extId, buf, len);
		vTaskDelay(250);
	}
}

void send_CRM_task(){
	uint16_t interval = 0;
	CRM_Flag = 1;
	CML_Flag = 0;
	while(interval <= 5000){
		uint32_t stdId, extId;
		uint8_t priority = 6, buf[5], len = 5;
		buf[0] = data2560&0xFF;
		buf[1] = (data2561>>24)&0xFF;  buf[2] = (data2561>>16)&0xFF;  buf[3] = (data2561>>8)&0xFF;  buf[4] = data2561&0xFF;
		Get_Identifier(&stdId, &extId, priority, CRM_Param, BMS_Address, Charger_Address);
		Extern_CAN_SendMsg(stdId, extId, buf, len);
		interval += 250;
		vTaskDelay(250);
	}
	CRM_Flag = 0;
	if(data2560==0){
		set_CEM_data(1, 0, 0, 0, 0, 0, 0);
	}else if(data2560==0xAA){
		set_CEM_data(0, 1, 0, 0, 0, 0, 0);
	}
	xTaskCreate((TaskFunction_t   )send_CEM_task,          
	            (const char*      )"send_CEM_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendCEMTaskHandler); 
	vTaskDelete(SendCRMTaskHandler);
}

void send_CML_task(){
	uint16_t interval = 0;
	CML_Flag = 1;
	CRO_Flag = 0;
	while(interval<= 5000){
		uint32_t stdId, extId;
		uint8_t priority = 6, buf[8], len = 8;
		buf[0] = (data2824>>8)&0xFF;  buf[1] = data2824&0xFF;
		buf[2] = (data2825>>8)&0xFF;  buf[3] = data2825&0xFF;
		buf[4] = (data2826>>8)&0xFF;  buf[5] = data2826&0xFF;
		buf[6] = (data2827>>8)&0xFF;  buf[7] = data2827&0xFF;
		Get_Identifier(&stdId, &extId, priority, CML_Param, BMS_Address, Charger_Address);
		Extern_CAN_SendMsg(stdId, extId, buf, len);
		interval += 250;
		vTaskDelay(250);
	}
	CML_Flag = 0;
	set_CEM_data(0, 0, 1, 0, 0, 0, 0);
	xTaskCreate((TaskFunction_t   )send_CEM_task,          
	            (const char*      )"send_CEM_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendCEMTaskHandler); 
	vTaskDelete(SendCMLTaskHandler);
}

void send_CRO_task(){
	uint16_t interval = 0;
	CRO_Flag = 1;
	CCS_Flag = 0;
	while(interval<= 5000){
		uint32_t stdId, extId;
		uint8_t priority = 4, buf[1], len = 1;
		buf[0] = data2830&0xFF;
		Get_Identifier(&stdId, &extId, priority, CML_Param, BMS_Address, Charger_Address);
		Extern_CAN_SendMsg(stdId, extId, buf, len);
		interval += 250;
		vTaskDelay(250);
	}
	CML_Flag = 0;
	vTaskDelete(SendCMLTaskHandler);
}

void send_CCS_task(){
	uint16_t interval = 0;
	CCS_Flag = 1;
	CST_Flag = 0;
	while(interval<= 1000){
		uint32_t stdId, extId;
		uint8_t priority = 6, buf[7], len = 7;
		buf[0] = (data3081>>8)&0xFF;  buf[1] = data3081&0xFF;
		buf[2] = (data3082>>8)&0xFF;  buf[3] = data3082&0xFF;
		buf[4] = (data3083>>8)&0xFF;  buf[5] = data3083&0xFF;
		buf[6] = data3929&0xFF;
		Get_Identifier(&stdId, &extId, priority, CCS_Param, BMS_Address, Charger_Address);
		Extern_CAN_SendMsg(stdId, extId, buf, len);
		interval += 250;
		vTaskDelay(250);
	}
	CCS_Flag = 0;
	set_CEM_data(0, 0, 0, 0, 1, 0, 0);
	xTaskCreate((TaskFunction_t   )send_CEM_task,          
	            (const char*      )"send_CEM_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendCEMTaskHandler); 
	vTaskDelete(SendCCSTaskHandler);
}

void send_CST_task(){
	uint16_t interval = 0;
	CST_Flag = 1;
	CSD_Flag = 0;
	while(interval<= 5000){
		uint32_t stdId, extId;
		uint8_t priority = 4, buf[4], len = 4;
		buf[0] = data3521&0xFF;
		buf[1] = (data3522>>8)&0xFF;  buf[2] = data3522&0xFF;
		buf[3] = data3523&0xFF;
		Get_Identifier(&stdId, &extId, priority, CST_Param, BMS_Address, Charger_Address);
		Extern_CAN_SendMsg(stdId, extId, buf, len);
		interval += 250;
		vTaskDelay(250);
	}
	CST_Flag = 0;
	set_CEM_data(0, 0, 0, 0, 0, 1, 0);
	xTaskCreate((TaskFunction_t   )send_CEM_task,          
	            (const char*      )"send_CEM_task",        
              (uint16_t         )128,                 
              (void*            )NULL,               
              (UBaseType_t      )2,                  
              (TaskHandle_t*    )&SendCEMTaskHandler); 
	vTaskDelete(SendCSTTaskHandler);
}

void send_CSD_task(){
	uint16_t interval = 0;
	CSD_Flag = 1;
	while(interval<= 5000){
		uint32_t stdId, extId;
		uint8_t priority = 6, buf[8], len = 8;
		buf[0] = (data3611>>8)&0xFF;  buf[1] = data3611&0xFF;
		buf[2] = (data3612>>8)&0xFF;  buf[3] = data3612&0xFF;
		buf[4] = (data3613>>24)&0xFF;  buf[5] = (data3613>>16)&0xFF;  buf[6] = (data3613>>8)&0xFF;  buf[7] = data3613&0xFF;
		Get_Identifier(&stdId, &extId, priority, CSD_Param, BMS_Address, Charger_Address);
		Extern_CAN_SendMsg(stdId, extId, buf, len);
		interval += 250;
		vTaskDelay(250);
	}
	CSD_Flag = 0;
	vTaskDelete(SendCSDTaskHandler);
}

void send_CEM_task(){
	u8 i, priority = 2, buf[4], len = 4;
	u32 stdId, extId;
	buf[0] = (data3921<<6)&0xFF;
	buf[1] = ((data3922<<6)&0xFF)|((data3923<<4)&0xFF);
	buf[2] = ((data3924<<6)&0xFF)|((data3925<<4)&0xFF)|((data3926<<2)&0xFF);
	buf[3] = (data3927<<6)&0xFF;
	Get_Identifier(&stdId, &extId, priority, BEM_Param, Charger_Address, BMS_Address);
	for(i=0;i<3;i++){
		Extern_CAN_SendMsg(stdId, extId, buf, len);
		vTaskDelay(250);
	}
	vTaskDelete(SendCEMTaskHandler);
}

void set_CHM_data(u16 d2600){
	data2600 = d2600;
}

void set_CRM_data(u8 d2560, u32 d2561){
	data2560 = d2560;
	data2561 = d2561;
}

void set_CTS_data(u64 d2823){
	data2823 = d2823;
}

void set_CML_data(u16 d2824, u16 d2825, u16 d2826, u16 d2827){
	data2824 = d2824;
	data2825 = d2825;
	data2826 = d2826;
	data2827 = d2827;
}

void set_CRO_data(u8 d2830){
	data2830 = d2830;
}

void set_CCS_data(u16 d3081, u16 d3082, u16 d3083, u8 d3929){
  data3081 = d3081;
  data3082 = d3082;
  data3083 = d3083;
  data3929 = d3929;	
}

void set_CST_data(u8 d3521, u16 d3522, u8 d3523){
  data3521 = d3521;
  data3522 = d3522;
  data3523 = d3523;	
}

void set_CSD_data(u16 d3611, u16 d3612, u32 d3613){
	data3611 = d3611;
	data3612 = d3612;
	data3613 = d3613;
}

void set_CEM_data(u8 d3921, u8 d3922, u8 d3923, u8 d3924, u8 d3925, u8 d3926, u8 d3927){
	data3921 = d3921;
	data3922 = d3922;
	data3923 = d3923;
	data3924 = d3924;
	data3925 = d3925;
	data3926 = d3926;
	data3927 = d3927;
}

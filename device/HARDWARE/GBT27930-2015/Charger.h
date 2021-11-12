#ifndef __CHARGER_H
#define __CHARGER_H

#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define Charger_Address 0x56
#define TP_Param 236
#define CHM_Param 0x26
#define BHM_Param 0x27
#define CRM_Param 0x01
#define BRM_Param 0x02
#define BCP_Param 0x06
#define CTS_Param 0x07
#define CML_Param 0x08
#define BRO_Param 0x09
#define CRO_Param 0x0A
#define BCL_Param 0x10
#define BCS_Param 0x11
#define CCS_Param 0x12
#define BSM_Param 0x13
#define BMV_Param 0x15
#define BMT_Param 0x16
#define BSP_Param 0x17
#define BST_Param 0x19
#define CST_Param 0x1A
#define BSD_Param 0x1C
#define CSD_Param 0x1D
#define BEM_Param 0x1E
#define CEM_Param 0x1F

//BMS.c中定义的国标通信协议使用变量的声明(在引入该文件后,即可在对应文件(如main.c)中使用以下变量)
extern uint32_t data2600;
extern uint8_t data2560;
extern uint32_t data2561;
extern uint64_t data2823;
extern uint16_t data2824, data2825, data2826, data2827;
extern uint8_t data2830;
extern uint16_t data3081, data3082, data3083;
extern uint8_t data3929;
extern uint8_t data3521, data3523;
extern uint16_t data3522;
extern uint16_t data3611, data3612;
extern uint32_t data3613;
extern u8 data3921, data3922, data3923, data3924, data3925, data3926, data3927;


//BMS.c中定义的变量的声明
extern uint32_t CHM_Tick, CRM_Tick, CTS_Tick, CML_Tick, CRO_Tick, CCS_Tick, CST_Tick, CSD_Tick;  //发送时间
extern uint8_t CHM_Flag, CRM_Flag, CTS_Flag, CML_Flag, CRO_Flag, CCS_Flag, CST_Flag, CSD_Flag;   //Flag
extern uint8_t BMS_Total_Voltage_Flag, BMS_Total_Current_Flag;


//BMS.c中定义的任务句柄的声明
extern TaskHandle_t SendCHMTaskHandler;
extern TaskHandle_t SendCRMTaskHandler;
extern TaskHandle_t SendCTSTaskHandler;
extern TaskHandle_t SendCMLTaskHandler;
extern TaskHandle_t SendCROTaskHandler;
extern TaskHandle_t SendCCSTaskHandler;
extern TaskHandle_t SendCSTTaskHandler;
extern TaskHandle_t SendCSDTaskHandler;
extern TaskHandle_t SendCEMTaskHandler;


//函数声明
void set_CHM_data(u16 d2600);
void set_CRM_data(u8 d2560, u32 d2561);
void set_CTS_data(u64 d2823);
void set_CML_data(u16 d2824, u16 d2825, u16 d2826, u16 d2827);
void set_CRO_data(u8 d2830);
void set_CCS_data(u16 d3081, u16 d3082, u16 d3083, u8 d3929);
void set_CST_data(u8 d3521, u16 d3522, u8 d3523);
void set_CSD_data(u16 d3611, u16 d3612, u32 d3613);
void set_CEM_data(u8 d3921, u8 d3922, u8 d3923, u8 d3924, u8 d3925, u8 d3926, u8 d3927);

void send_CHM_task(void);
void send_CRM_task(void);
void send_CTS_task(void);
void send_CML_task(void);
void send_CRO_task(void);
void send_CCS_task(void);
void send_CST_task(void);
void send_CSD_task(void);
void send_CEM_task(void);

u8 check_BCS_data(u8 *data);
u8 check_BSM_data(u8 *data);


#endif

#ifndef __BMS_H
#define __BMS_H

#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bms_state.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define BMS_Address 0xF4
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

#define base_curr 400
#define base_temp 50 

extern uint16_t data2601;
extern uint32_t data2565;
extern uint8_t data2566;
extern uint16_t data2567, data2568;
extern uint16_t data2816, data2817, data2818, data2819, data2821, data2822;
extern uint8_t data2820;
extern uint8_t data2829;
extern uint16_t data3072, data3073;
extern uint8_t data3074;
extern uint16_t data3075, data3076, data3077, data3079;
extern uint8_t data3078;
extern uint8_t data3085, data3086, data3087, data3088, data3089, data3090, data3091, data3092, data3093, data3094, data3095, data3096;
extern uint8_t data3511, data3513;
extern uint16_t data3512;
extern uint8_t data3601, data3604, data3605;
extern uint16_t data3602, data3603;
extern uint8_t data3901, data3902, data3903, data3904, data3905, data3906, data3907;

//用于国标通信的标准变量
extern uint16_t standard_data3072, standard_data3073;

//BMS.c中定义的变量的声明
extern uint32_t BHM_Tick, BRM_Tick, BCP_Tick, BRO_Tick, BCL_Tick, BCS_Tick, Check_CCS_Tick, BSM_Tick, BST_Tick, BSD_Tick;  //发送时间
extern uint8_t BHM_Flag, BRM_Flag, BCP_Flag, BRO_Flag, BCL_Flag, BCS_Flag, Check_CCS_Flag, BSM_Flag, BST_Flag, BSD_Flag;   //Flag
extern uint8_t BMS_Total_Voltage_Flag, BMS_Total_Current_Flag;


//BMS.c中定义的任务句柄的声明
extern TaskHandle_t SendBHMTaskHandler;
extern TaskHandle_t SendBRMTaskHandler;
extern TaskHandle_t SendBCPTaskHandler;
extern TaskHandle_t SendBROTaskHandler;
extern TaskHandle_t SendBCLTaskHandler;
extern TaskHandle_t SendBCSTaskHandler;
extern TaskHandle_t CheckCCSTaskHandler;
extern TaskHandle_t SendBSMTaskHandler;
extern TaskHandle_t SendBSTTaskHandler;
extern TaskHandle_t SendBSDTaskHandler;
extern TaskHandle_t SendBEMTaskHandler;


//函数声明
void set_BHM_data(uint16_t d2601);
void set_BRM_data(uint32_t d2565, uint8_t d2566, uint16_t d2567, uint16_t d2568);
void set_BCP_data(u16 d2816, u16 d2817, u16 d2818, u16 d2819, u8 d2820, u16 d2821, u16 d2822);
void set_BRO_data(u8 d2829);
void set_BCL_data(u16 d3072, u16 d3073, u8 d3074);
void set_BCS_data(u16 d3075, u16 d3076, u16 d3077, u8 d3078, u16 d3079);
void set_BSM_data(u8 d3085, u8 d3086, u8 d3087, u8 d3088, u8 d3089, u8 d3090, u8 d3091, u8 d3092, u8 d3093, u8 d3094, u8 d3095, u8 d3096);
void set_BST_data(u8 d3511, u16 d3512, u8 d3513);
void set_BSD_data(u8 d3601, u16 d3602, u16 d3603, u8 d3604, u8 d3605);
void set_BEM_data(u8 d3901, u8 d3902, u8 d3903, u8 d3904, u8 d3905, u8 d3906, u8 d3907);

void send_BHM_task(void);
void send_BRM_task(void);
void send_BRM_data(uint8_t num);
void send_BCP_task(void);
void send_BCP_data(uint8_t num);
void send_BRO_task(void);
void send_BCL_task(void);
void send_BCS_task(void);
void send_BCS_data(uint8_t num);
void check_CCS_task(void);
void send_BSM_task(void);
void send_BMV_task(void);
void send_BMT_task(void);
void send_BSP_task(void);
void send_BST_task(void);
void send_BSD_task(void);
void send_BEM_task(void);

u8 check_CCS_data(u8 *data);



#endif

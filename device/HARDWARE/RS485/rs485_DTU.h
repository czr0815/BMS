#ifndef __RS485_DTU_H
#define __RS485_DTU_H
#include "stm32f4xx.h"
#include "malloc.h"
#include "bms_state.h"

/*
	说明：
	BMS主控系统中，DTU模块由BCU/BMU通过RS485进行控制；
	配置上，RS485信号通过STM32F413的USART1串口经转换模块得到；
	因此，主控系统与显示屏的交互过程及功能实现都是通过串口USART1实现；
*/

//485转换模块控制引脚CON控制指令,ON为发送模式，OFF为接收模式
#define RS485_RE1_ON  GPIO_SetBits(GPIOG,GPIO_Pin_10)  //BCU的DTU485通信模块的收发控制引脚为PG10
#define RS485_RE1_OFF GPIO_ResetBits(GPIOG,GPIO_Pin_10)
#define DTU_SEND_LEN 244 //最大可发送字节数
#define DTU_TEMP_LEN 32
#define RS4851_REC_LEN 256  //RS485_DTU接收最大字节数

//发送json格式数据时的字符串常量
extern const char DEVICEBCU_15[15];
extern const char DEVICEBCU_15[15];
extern const char BMUID_9[9];
extern const char BMUID_9[9];
extern const char CELL_8[8];
extern const char ID_6[6];
extern const char VOL_6[6];
extern const char SOC_6[6];
extern const char TEMP_8[8];
extern const char VAL_6[6];
extern const char SMOKE_9[9];
extern const char CURR_7[7];
extern const char BCUID_9[9];
extern const char VOLT_8[8];
extern const char INSU_7[7];
extern const char CC_6[6];



void RS485_DTU_Init(u32 bound);  //DTU 485串口初始化函数
void RS485_DTU_SendData(const uint8_t *data, uint8_t len);  //DTU 485串口发送函数


//在BCU中，上传各个下属BMU的相关数据
//【1】上传来自下属BMU的cell(电压+SOC)
//【2】上传来自下属BMU的温度数据
//【3】上传来自下属BMU的其他数据，包括BMU的电流和烟雾
void DTU_UploadBMUCell_InBCU(uint8_t bmu_id, uint8_t pos_s, uint8_t pos_e);
void DTU_UploadBMUTemp_InBCU(uint8_t bmu_id, uint8_t pos_s, uint8_t pos_e);
void DTU_UploadBMUData_InBCU(uint8_t bmu_id, uint8_t curr_pos, uint8_t smoke_pos_s, uint8_t smoke_pos_e);

//上传BCU收集到的数据,包括总电压、总SOC、总电流、绝缘和系统状态，一次性上传
void DTU_UploadBCUData();

//通过Json格式上传数据总共有3种类型：
//  【1】以芯片为单位上传电池电压数据,一般(最多)一次上传会上传12个电池电压和12个SOC状态数据(对于电池串,电压和SOC状态绑定)；
//  【2】以BMU为单位上传温度传感器监测数据,最多一次会上传25个温度数据(对于一个BMU,有多少一次就上传多少)；
//  【3】以BCU为单位上传各个BMU的电流数据(通过BMU与外界通信得到)、绝缘状态信息(BCU)、总压检测结果(*3)(BCU)、电池组SOC状态(BCU)、各个BMU的CC信号状态；
u8 RS485_DTU_UploadVolData(u8 bmucount, u8 volnumbegin,u8 volnumend);    //通过DTU上传单体电池串电压数据
u8 RS485_DTU_UploadTempData(u8 bmucount, u8 tempnumbegin, u8 tempnumend);           //通过DTU上传温度监测数据
u8 RS485_DTU_UploadCurrentCCSmokeData(u8 bmucount, u8 currnum, u8 ccnum, u8 smokenum);    //通过DTU上传烟雾和电流数据
#if IS_BCU_ROLE
u8 RS485_DTU_UploadBCUData(u8 currnum, u8 soctotalnum, u8 insunum, u8 voltnum);  //BCU通过DTU上传BCU侧获取的数据(总压、总体SOC状态、绝缘状态)
#endif










#endif

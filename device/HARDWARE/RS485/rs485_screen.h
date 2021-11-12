#ifndef __RS485_SCREEN_H
#define __RS485_SCREEN_H
#include "stm32f4xx.h"

/*
	说明：
	BMS主控系统中，显示屏由BCU通过RS485进行控制；
	配置上，RS485信号通过STM32F413的USART6串口经转换模块得到；
	因此，主控系统与显示屏的交互过程及功能实现都是通过串口USART6实现；
*/

//485转换模块控制引脚CON控制指令,ON为发送模式，OFF为接收模式
//#define RS485_RE4_ON  GPIO_SetBits(GPIOG,GPIO_Pin_7)  //BCU的显示屏485通信模块的收发控制引脚为PG7
//#define RS485_RE4_OFF GPIO_ResetBits(GPIOG,GPIO_Pin_7)
#define RS485_RE4_ON  GPIO_SetBits(GPIOG,GPIO_Pin_10)  //BCU的显示屏485通信模块的收发控制引脚为PG7
#define RS485_RE4_OFF GPIO_ResetBits(GPIOG,GPIO_Pin_10)

#define RS4856_REC_LEN 64  //RS485_Screen接收最大字节数

//迪文屏变量存储器配置及地址
//更新时间: 2021-09-14 19:00
//BMU数据显示
//BCU主页信息
#define  TOTAL_SOC_ADDR        0x1000  //总体SOC百分比(2字节整数)
#define  TOTAL_VOL_ADDR        0x1002  //总电压(2字节整数)
#define  HIGHVOL_CELL_ADDR     0x1004  //最高单体电压(2字节整数)
#define  HIGHVOL_CELL_ID_ADDR  0x1006  //最高单体电压对应单体编号(2字节整数)
#define  LOWVOL_CELL_ADDR      0x1008  //最低单体电压(2字节整数)
#define  LOWVOL_CELL_ID_ADDR   0x100A  //最低单体电压对应单体编号(2字节整数)
#define  AVEG_VOL_ADDR         0x100C  //平均单体电压(2字节整数)
#define  MAX_VOL_DIFF_ADDR     0x100E  //单体之间的最大压差(2字节整数)
#define  TOTAL_CURR_ADDR       0x1010  //总电流(2字节整数)
#define  HIGHTEMP_ADDR         0x1012  //最高温度(2字节整数)
#define  HIGHTEMP_ID_ADDR      0x1014  //最高温度检测点编号(2字节整数)
#define  LOWTEMP_ADDR          0x1016  //最低温度(2字节整数)
#define  LOWTEMP_ID_ADDR       0x1018  //最低温度检测点编号(2字节整数)
#define  AVEG_TEMP_ADDR        0x101A  //平均温度(2字节整数)
#define  MAX_TEMP_DIFF_ADDR    0x101C  //最大温差(2字节整数)
#define  POWER_VOL_ADDR        0x101E  //供电电压(2字节整数)
#define  CHARGE_STATE_ADDR     0x1028  //充电状态(预留长度8字节)

//BMU主页信息
#define  INSUSTATE_POS_ADDR    0x1030  //绝缘状态1(Positive)(2字节整数)
#define  INSUSTATE_NEG_ADDR    0x1032  //绝缘状态2(Negative)(2字节整数)
#define  CHARGER_OUTVOL_ADDR   0x1034  //充电机输出电压(2字节整数)
#define  CHARGER_OUTCURR_ADDR  0x1036  //充电机输出电流(2字节整数)
#define  VOL_B+_ADDR           0x1038  //高压: B+(2字节整数)
#define  VOL_HV1_ADDR          0x103A  //高压: HV1(2字节整数)
#define  VOL_HV2_ADDR          0x103C  //高压: HV2(2字节整数)
#define  CC_STATE_ADDR         0x1040  //CC状态(预留长度8字节)
#define  HEAT_COOL_STATE_ADDR  0x1048  //加热制冷(预留长度8字节)
#define  ONLINE_STATE_ADDR     0x1040  //上线状态(预留长度8字节)
#define  RELAY1_STATE_ADDR     0x1058  //继电器1状态(预留长度8字节)
#define  RELAY2_STATE_ADDR     0x1060  //继电器2状态(预留长度8字节)
#define  RELAY3_STATE_ADDR     0x1068  //继电器2状态(预留长度8字节)

//报警数据
#define  SCREEN_SOCLOW           0x10A0
#define  SCREEN_CVOLTOTALHIGH    0x10A2
#define  SCREEN_DSCVOLTOTALLOW   0x10A4
#define  SCREEN_CVOLHIGH         0x10A6
#define  SCREEN_CVOLLOW          0x10A8
#define  SCREEN_DSCVOLHIGH       0x10AA
#define  SCREEN_DSCVOLLOW        0x10AC
#define  SCREEN_CVOLDIFF         0x10AE
#define  SCREEN_DSCVOLDIFF       0x10B0
#define  SCREEN_VOLWIRE          0x10B2
#define  SCREEN_CTEMPHIGH        0x10B4
#define  SCREEN_DSCTEMPHIGH      0x10B6
#define  SCREEN_CTEMPLOW         0x10B8
#define  SCREEN_DSCTEMPLOW       0x10BA
#define  SCREEN_CTEMPDIFF        0x10BC
#define  SCREEN_DSCTEMPDIFF      0x10BE
#define  SCREEN_COVERCURR        0x10C0
#define  SCREEN_DSCOVERCURR      0x10C2
#define  SCREEN_INSULEAK         0x10C4
#define  SCREEN_TEMPRISE         0x10C6

#define  SCREEN_ALARM_GREEN      0
#define  SCREEN_ALARM_YELLOW     1
#define  SCREEN_ALARM_ORANGE     2
#define  SCREEN_ALARM_RED        3
//......

//电池串单体电压和温度检测点数据
#define  VOLCARD1_VOL1_ADDR    0x1100  //电压采集卡1  电压数据1 (共24个)
#define  VOLCARD1_TEMP1_ADDR   0x1130  //电压采集卡1  温度数据1 (共10个)
#define  VOLCARD2_VOL1_ADDR    0x1150  //电压采集卡2  电压数据1 (共24个)
#define  VOLCARD2_TEMP1_ADDR   0x1180  //电压采集卡2  温度数据1 (共10个)
#define  VOLCARD3_VOL1_ADDR    0x11A0  //电压采集卡3  电压数据1 (共24个)
#define  VOLCARD3_TEMP1_ADDR   0x11D0  //电压采集卡3  温度数据1 (共10个)
#define  VOLCARD4_VOL1_ADDR    0x1200  //电压采集卡4  电压数据1 (共24个)
#define  VOLCARD4_TEMP1_ADDR   0x1230  //电压采集卡4  温度数据1 (共10个)
#define  VOLCARD5_VOL1_ADDR    0x1250  //电压采集卡5  电压数据1 (共24个)
#define  VOLCARD5_TEMP1_ADDR   0x1280  //电压采集卡5  温度数据1 (共10个)
#define  VOLCARD6_VOL1_ADDR    0x12A0  //电压采集卡6  电压数据1 (共24个)
#define  VOLCARD6_TEMP1_ADDR   0x12D0  //电压采集卡6  温度数据1 (共10个)
#define  VOLCARD7_VOL1_ADDR    0x1300  //电压采集卡7  电压数据1 (共24个)
#define  VOLCARD7_TEMP1_ADDR   0x1330  //电压采集卡7  温度数据1 (共10个)
#define  VOLCARD8_VOL1_ADDR    0x1350  //电压采集卡8  电压数据1 (共24个)
#define  VOLCARD8_TEMP1_ADDR   0x1380  //电压采集卡8  温度数据1 (共10个)
#define  VOLCARD9_VOL1_ADDR    0x13A0  //电压采集卡9  电压数据1 (共24个)
#define  VOLCARD9_TEMP1_ADDR   0x13D0  //电压采集卡9  温度数据1 (共10个)
#define  VOLCARD10_VOL1_ADDR   0x1400  //电压采集卡10 电压数据1 (共24个)
#define  VOLCARD10_TEMP1_ADDR  0x1430  //电压采集卡10 温度数据1 (共10个)



extern uint8_t RS4856_RX_BUF[RS4856_REC_LEN];  //RS485_Screen接收缓冲区
extern uint8_t RS4856_RX_LEN;  
extern uint8_t RS4856_RX_STA;  //RS485_Screen接收状态标记

void RS485_Screen_Init(uint32_t bound);
void RS485_Screen_SendData(const uint8_t *data, uint8_t len);
void RS485_Screen_ReadData(uint8_t *data, uint8_t *len);

//说明: DGUS屏的寄存器按照字节(Byte)寻址, 变量存储器按照字(Word)(寻址
//向指定寄存器地址写入数据
//void RS485_Screen_WriteReg(uint8_t addr, uint8_t data);
//向指定存储器地址写入变量数据
void RS485_Screen_WriteMem(uint16_t addr, uint16_t data);
//读取指定寄存器地址开始的指定字节长度的寄存器数据
//void RS485_Screen_ReadReg(uint8_t addr, uint8_t len);
//读取指定寄存器地址开始的指定字节长度的变量存储区数据
void RS485_Screen_ReadMem(uint16_t addr, uint8_t len);

//获取指定电池串电压数据的显示屏地址
u16 get_celladdr(u16 cellnum);
//获取指定温度检测点的数据
u16 get_tempaddr(u16 tempnum);


#endif





















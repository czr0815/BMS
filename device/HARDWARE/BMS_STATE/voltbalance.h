
#ifndef __VOLTBALANCE
#define __VOLTBALANCE

#include "sys.h"
#include "bms_state.h"

//配置的相关宏定义
#define BALANCE_NUM 8    //均衡同时开启路数
#define RUN_BALANCE 1    //在BMS正常运行状态下(不只是充电)是否进行均衡控制

//均衡的标定量
#define VoltLimitCalb 5000         //均衡的压差条件标定
#define CurrentLimitCalb  10.0     //均衡的电流大小条件标定
#define MinCellVolLimit   28000        //单体电压最低限制


//均衡相关控制标志
extern u8 VoltLimitFlag;           //均衡的压差条件(1为满足条件,0为未满足条件)
extern u8 ModeValidFlag;           //均衡的系统工作状态条件(1为满足条件,0为未满足条件)
extern u8 CurrentLimitFlag;        //均衡的电流大小条件(1为满足条件,0为未满足条件)
extern u8 BalanceEnableFlag;       //均衡的开启标志(1为开启均衡,0为未开启均衡)
extern u8 BalancePrevEnableFlag;   //前一时刻的均衡开启标志
#if IS_BCU_ROLE
extern u8 BalanceFlag[BMU_LIST_NUM * BMU_VOL_NUM];  //单体电池均衡状态标志(1为开启均衡,0为未开启均衡)
#endif
#if IS_BMU_ROLE
extern u8 BalanceFlag[BMU_VOL_NUM];  //单体电池均衡状态标志(1为开启均衡,0为未开启均衡)
#endif
extern u16 balancetargetvol;       //均衡目标电压(单体均衡停止的目标电压)

extern u8 celltobalance_previd[BALANCE_NUM];    //上次的均衡电池id
extern u16 celltobalance_prevvol[BALANCE_NUM];  //上次的均衡电池电压
extern u8 celltobalance_id[BALANCE_NUM];       //本次的均衡电池id
extern u16 celltobalance_vol[BALANCE_NUM];     //本次的均衡电池电压


































#endif

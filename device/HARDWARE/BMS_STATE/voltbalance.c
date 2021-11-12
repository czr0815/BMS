
/* 电压均衡说明


*/
#include "voltbalance.h"
#include "bms_state.h"

//均衡相关控制标志
u8 VoltLimitFlag;           //均衡的压差条件(1为满足条件,0为未满足条件)
u8 ModeValidFlag;           //均衡的系统工作状态条件(1为满足条件,0为未满足条件)
u8 CurrentLimitFlag;        //均衡的电流大小条件(1为满足条件,0为未满足条件)
u8 BalanceEnableFlag;       //均衡的开启标志(1为开启均衡,0为未开启均衡)
u8 BalancePrevEnableFlag;   //前一时刻的均衡开启标志
#if IS_BCU_ROLE
u8 BalanceFlag[BMU_LIST_NUM * BMU_VOL_NUM] = {0};  //单体电池均衡状态标志(1为开启均衡,0为未开启均衡)
#endif
#if IS_BMU_ROLE
u8 BalanceFlag[BMU_VOL_NUM];  //单体电池均衡状态标志(1为开启均衡,0为未开启均衡)
#endif
u16 balancetargetvol;       //均衡目标电压(单体均衡停止的目标电压)

u8 celltobalance_previd[BALANCE_NUM];    //上次的均衡电池id
u16 celltobalance_prevvol[BALANCE_NUM];  //上次的均衡电池电压
u8 celltobalance_id[BALANCE_NUM];       //本次的均衡电池id
u16 celltobalance_vol[BALANCE_NUM];     //本次的均衡电池电压

























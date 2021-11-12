#ifndef __SOC_VMIN_EKF_H
#define __SOC_VMIN_EKF_H

/* SOC估算相关说明
- 本文件给出了估算整个电池组的SOC状态的方法声明;
- 电池组SOC状态的估算方法主要是:基于最小电压的扩展卡尔曼滤波方法;
*/
#include "sys.h"
#include "stmflash.h"

//宏定义
#define eta 0.999      //库伦效率:电池放电容量与同循环过程中充电容量之比
#define Cn  727200     //单体电池容量:202Ah,用于计算SOC状态方程(727200 = 202Ah*3600s)
#define V0  3.4
#define R   0.0003    //电池内阻:用于计算观测方程,单位Ω
#define K0  1     //观测方程参数
#define K1  1     //观测方程参数
#define K2  1     //观测方程参数
#define K3  1     //观测方程参数
#define I   1    //单位矩阵  


////变量声明
//extern const u16 SOC_OCV[11];  //厂方提供的电池组SOC-OCV表格
//extern float xk,Pk;            //用于存储Vmin-EKF算法计算过程中的中间变量(状态变量 协方差)





//函数声明
void soc_write(float socnow);  //保存SOC数据
float soc_read(void);   //读取之前时刻保存的SOC数据
float soc_calibrate(u16 cellvoltocalb);  //SOC标定
float soc_calculate(float ik_1, u16 yku16, float t);  //估算SOC状态值
float ln(float x);



















#endif



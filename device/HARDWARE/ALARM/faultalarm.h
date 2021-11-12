#ifndef __FAULTALARM_H
#define __FAULTALARM_H

#include "sys.h"
#include "BMS.h"
#include "Charger.h"
#include "rs485_screen.h"

#define FAULTALARM_OPEN 1

//各类型故障检测是否开启(1为开启,0为关闭)
extern uint8_t alarmfun_soclow,alarmfun_voltotalhigh,alarmfun_voltotallow,alarmfun_cvolhigh,alarmfun_cvollow,alarmfun_dscvolhigh,alarmfun_dscvollow;
extern uint8_t alarmfun_cvoldiff,alarmfun_dscvoldiff,alarmfun_volwire,alarmfun_ctemphigh,alarmfun_dsctemphigh,alarmfun_ctemplow,alarmfun_dsctemplow;
extern uint8_t alarmfun_ctempdiff,alarmfun_dsctempdiff,alarmfun_covercurr,alarmfun_dscovercurr,alarmfun_insuleak,alarmfun_temprise;

//前一时刻的检测状态
extern uint8_t alarmcheckstate_soclow, alarmcheckstate_voltotalhigh, alarmcheckstate_voltotallow, alarmcheckstate_cvolhigh, alarmcheckstate_cvollow, alarmcheckstate_dscvolhigh, alarmcheckstate_dscvollow;
extern uint8_t alarmcheckstate_cvoldiff,alarmcheckstate_dscvoldiff,alarmcheckstate_volwire,alarmcheckstate_ctemphigh,alarmcheckstate_dsctemphigh,alarmcheckstate_ctemplow,alarmcheckstate_dsctemplow;
extern uint8_t alarmcheckstate_ctempdiff,alarmcheckstate_dsctempdiff,alarmcheckstate_covercurr,alarmcheckstate_dscovercurr,alarmcheckstate_insuleak,alarmcheckstate_temprise;

//本时刻和前一时刻报警状态(1为报警,0为未报警)(比较两个时刻的报警状态,不同才向显示屏发送控制信息进行显示内容更改)
extern uint8_t alarmstate_soclow,alarmstate_voltotalhigh,alarmstate_voltotallow,alarmstate_cvolhigh,alarmstate_cvollow,alarmstate_dscvolhigh,alarmstate_dscvollow;
extern uint8_t alarmstate_cvoldiff,alarmstate_dscvoldiff,alarmstate_volwire,alarmstate_ctemphigh,alarmstate_dsctemphigh,alarmstate_ctemplow,alarmstate_dsctemplow;
extern uint8_t alarmstate_ctempdiff,alarmstate_dsctempdiff,alarmstate_covercurr,alarmstate_dscovercurr,alarmstate_insuleak,alarmstate_temprise;
extern uint8_t alarmprevstate_soclow,alarmprevstate_voltotalhigh,alarmprevstate_voltotallow,alarmprevstate_cvolhigh,alarmprevstate_cvollow,alarmprevstate_dscvolhigh,alarmprevstate_dscvollow;
extern uint8_t alarmprevstate_cvoldiff,alarmprevstate_dscvoldiff,alarmprevstate_volwire,alarmprevstate_ctemphigh,alarmprevstate_dsctemphigh,alarmprevstate_ctemplow,alarmprevstate_dsctemplow;
extern uint8_t alarmprevstate_ctempdiff,alarmprevstate_dsctempdiff,alarmprevstate_covercurr,alarmprevstate_dscovercurr,alarmprevstate_insuleak,alarmprevstate_temprise;

//故障时延时时间(故障发生了多长时间)
extern uint8_t alarmdelay_soclow,alarmdelay_voltotalhigh,alarmdelay_voltotallow,alarmdelay_cvolhigh,alarmdelay_cvollow,alarmdelay_dscvolhigh,alarmdelay_dscvollow;
extern uint8_t alarmdelay_cvoldiff,alarmdelay_dscvoldiff,alarmdelay_volwire,alarmdelay_ctemphigh,alarmdelay_dsctemphigh,alarmdelay_ctemplow,alarmdelay_dsctemplow;
extern uint8_t alarmdelay_ctempdiff,alarmdelay_dsctempdiff,alarmdelay_covercurr,alarmdelay_dscovercurr,alarmdelay_insuleak,alarmdelay_temprise;

//释放时延时时间(故障恢复了多长时间)
extern uint8_t releasedelay_soclow,releasedelay_voltotalhigh,releasedelay_voltotallow,releasedelay_cvolhigh,releasedelay_cvollow,releasedelay_dscvolhigh,releasedelay_dscvollow;
extern uint8_t releasedelay_cvoldiff,releasedelay_dscvoldiff,releasedelay_volwire,releasedelay_ctemphigh,releasedelay_dsctemphigh,releasedelay_ctemplow,releasedelay_dsctemplow;
extern uint8_t releasedelay_ctempdiff,releasedelay_dsctempdelay,releasedelay_covercurr,releasedelay_dscovercurr,releasedelay_insuleak,releasedelay_temprise;

//故障阈值(下标0:故障等级1,下标1:故障等级2,下标2:故障等级3)
extern float alarmtrshold_soclow[3],alarmtrshold_voltotalhigh[3],alarmtrshold_voltotallow[3],alarmtrshold_cvolhigh[3],alarmtrshold_cvollow[3],alarmtrshold_dscvolhigh[3],alarmtrshold_dscvollow[3];
extern float alarmtrshold_cvoldiff[3],alarmtrshold_dscvoldiff[3],alarmtrshold_volwire[3],alarmtrshold_ctemphigh[3],alarmtrshold_dsctemphigh[3],alarmtrshold_ctemplow[3],alarmtrshold_dsctemplow[3];
extern float alarmtrshold_ctempdiff[3],alarmtrshold_dsctempdiff[3],alarmtrshold_covercurr[3],alarmtrshold_dscovercurr[3],alarmtrshold_insuleak[3],alarmtrshold_temprise[3];

//故障延时阈值
extern uint8_t alarmdelaytrshold_soclow[3],alarmdelaytrshold_voltotalhigh[3],alarmdelaytrshold_voltotallow[3],alarmdelaytrshold_cvolhigh[3];
extern uint8_t alarmdelaytrshold_cvollow[3],alarmdelaytrshold_dscvolhigh[3],alarmdelaytrshold_dscvollow[3];
extern uint8_t alarmdelaytrshold_cvoldiff[3],alarmdelaytrshold_dscvoldiff[3],alarmdelaytrshold_volwire[3],alarmdelaytrshold_ctemphigh[3];
extern uint8_t alarmdelaytrshold_dsctemphigh[3],alarmdelaytrshold_ctemplow[3],alarmdelaytrshold_dsctemplow[3];
extern uint8_t alarmdelaytrshold_ctempdiff[3],alarmdelaytrshold_dsctempdiff[3],alarmdelaytrshold_covercurr[3],alarmdelaytrshold_dscovercurr[3],alarmdelaytrshold_insuleak[3],alarmdelaytrshold_temprise[3];

//释放阈值
extern float releasetrshold_soclow[3],releasetrshold_voltotalhigh[3],releasetrshold_voltotallow[3],releasetrshold_cvolhigh[3],releasetrshold_cvollow[3],releasetrshold_dscvolhigh[3],releasetrshold_dscvollow[3];
extern float releasetrshold_cvoldiff[3],releasetrshold_dscvoldiff[3],releasetrshold_volwire[3],releasetrshold_ctemphigh[3],releasetrshold_dsctemphigh[3],releasetrshold_ctemplow[3],releasetrshold_dsctemplow[3];
extern float releasetrshold_ctempdiff[3],releasetrshold_dsctempdiff[3],releasetrshold_covercurr[3],releasetrshold_dscovercurr[3],releasetrshold_insuleak[3],releasetrshold_temprise[3];

//释放延时阈值
extern uint8_t releasedelaytrshold_soclow[3],releasedelaytrshold_voltotalhigh[3],releasedelaytrshold_voltotallow[3];
extern uint8_t releasedelaytrshold_cvolhigh[3],releasedelaytrshold_cvollow[3],releasedelaytrshold_dscvolhigh[3],releasedelaytrshold_dscvollow[3];
extern uint8_t releasedelaytrshold_cvoldiff[3],releasedelaytrshold_dscvoldiff[3],releasedelaytrshold_volwire[3],releasedelaytrshold_ctemphigh[3];
extern uint8_t releasedelaytrshold_dsctemphigh[3],releasedelaytrshold_ctemplow[3],releasedelaytrshold_dsctemplow[3];
extern uint8_t releasedelaytrshold_ctempdiff[3],releasedelaytrshold_dsctempdiff[3],releasedelaytrshold_covercurr[3],releasedelaytrshold_dscovercurr[3];
extern uint8_t releasedelaytrshold_insuleak[3],releasedelaytrshold_temprise[3];


//函数声明
#if IS_BCU_ROLE
//检测故障
uint8_t faultalarm_soclow(float soc);
uint8_t faultalarm_voltotalhigh(float vol);
uint8_t faultalarm_voltotallow(float vol);
uint8_t faultalarm_cvolhigh(float cvol);
uint8_t faultalarm_cvollow(float cvol);
uint8_t faultalarm_dscvolhigh(float dscvol);
uint8_t faultalarm_dscvollow(float dscvol);
uint8_t faultalarm_cvoldiff(float diff);
uint8_t faultalarm_dscvoldiff(float diff);
uint8_t faultalarm_volwire(float volwire);
uint8_t faultalarm_ctemphigh(float ctemp);
uint8_t faultalarm_dsctemphigh(float dsctemp);
uint8_t faultalarm_ctemplow(float ctemp);
uint8_t faultalarm_dsctemplow(float dsctemp);
uint8_t faultalarm_ctempdiff(float diff);
uint8_t faultalarm_dsctempdiff(float diff);
uint8_t faultalarm_covercurr(float ccurr);
uint8_t faultalarm_dscovercurr(float dsccurr);
uint8_t faultalarm_insuleak(float insul);
uint8_t faultalarm_temprise(float temprise);

//故障处理手段
void faulthandle_soclow3(void);
void faulthandle_soclow2(void);
void faulthandle_soclow1(void);
void faulthandle_soclow0(void);
void faulthandle_voltotalhigh3(void);
void faulthandle_voltotalhigh2(void);
void faulthandle_voltotalhigh1(void);
void faulthandle_voltotalhigh0(void);
void faulthandle_voltotallow3(void);
void faulthandle_voltotallow2(void);
void faulthandle_voltotallow1(void);
void faulthandle_voltotallow0(void);
void faulthandle_cvolhigh3(void);
void faulthandle_cvolhigh2(void);
void faulthandle_cvolhigh1(void);
void faulthandle_cvolhigh0(void);
void faulthandle_cvollow3(void);
void faulthandle_cvollow2(void);
void faulthandle_cvollow1(void);
void faulthandle_cvollow0(void);
void faulthandle_cvoldiff3(void);
void faulthandle_cvoldiff2(void);
void faulthandle_cvoldiff1(void);
void faulthandle_cvoldiff0(void);
void faulthandle_covercurr3(void);
void faulthandle_covercurr2(void);
void faulthandle_covercurr1(void);
void faulthandle_covercurr0(void);
void faulthandle_insuleak3(void);
void faulthandle_insuleak2(void);
void faulthandle_insuleak1(void);
void faulthandle_insuleak0(void);
#endif


#endif

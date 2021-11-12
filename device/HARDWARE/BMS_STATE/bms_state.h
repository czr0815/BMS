#ifndef __BMS_STATE_H
#define __BMS_STATE_H
/*
	说明：
	用于定义描述电池组各种状态的数据;
	在其他文件中使用该数据时,均使用extern关键字进行外部变量声明
*/

#include <stdint.h>
#include "sys.h"

//是否使用实时系统: 1为使用系统,0为不使用系统
#define IS_USE_RTOS 1


//声明设备是BMU or BCU,    注意【三选一】
#define IS_BMU_ROLE 0    //注意【三选一】
#define IS_BCU_ROLE 1    //注意【三选一】
#define IS_BSU_ROLE 0    //注意【三选一】

#define MY_ID     0x10  //设定的每个主控的8位ID
//对于BMU,通常My_Pack_Number变量和My_Id变量保持一致,要求从0x01开始,序号连续增加
//暂定:5个BMU的地址分别为0x01-0x05,BCU的地址为0x10(1BCU+5BMU的条件下)
#define MY_DID    0x10

#define BMU_PRINT_P  1   //BMU在运行过程中是否会通过串口调试口输出运行信息
#define BMU_SCREEN_P 0   //是否由BMU将单体电压和温度数据在显示屏上显示
#define BCU_PRINT_P  0   //BCU在运行过程中是否会通过串口调试口输出运行信息

//电池参数
#define BATTERY_TYPE 0x03  //磷酸铁锂电池
#define BATTERY_VOL_ST 35 //单个电池标准最高电压3.5V*10
#define BATTERY_VOL_BEAR 40  //单个电池可承受的最高电压4V*10
#define BATTERY_CUR_ST 100  //单个电池额定电流10A*10
#define BATTERY_CUR_BEAR 120  //单个电池可承受的最大电流12A*10
#define BATTERY_CAP_ST  100  //单个电池额定容量10Ah*10
#define BATTERY_ENE_ST 350  //单个电池额定能量35kWh*10
#define BATTERY_TEMP_BEAR 45  //电池最高温度45℃

//BCU相关配置
#define CELL_NUM 240  //测试环境共237串电池
#define BMU_VOLCHIP_NUM  4  //每一个BMU所能连接的电压采集芯片的最大值
#define BCU_TEMP_NUM 50  //每一个BCU(/BMU)所能采集的温度数据的最大数量
//BMU相关配置
#define BMU_VOL_NUM   48  //单个BMU最多能同时测量的电池串电压数量
#define BMU_TEMP_NUM  12  //单个BMU最多能同时测量的温度数据数量
#define BMU_SMOKE_NUM 2  //单个BMU最多能同时测量的烟雾数据数量

//CAN1内部通信相关宏定义
#define BMU_LIST_NUM     5     //与本BCU所连接的BMU数量
#define CAN1_CONNECT_TRYTIMES  100  //CAN1内部通信连接过程中尝试连接次数
//CAN1内部通信相关变量定义
extern u8 My_BMS_Role;             //用于标识自身的BMS角色(BMU/BCU)
extern u8 My_Id;                   //用于标识自身ID，用于内部通信
extern u32 My_Did;                //表示自身设备ID，用于云端上传和系统运行
extern u8 Is_Connected;            //用于标识与上级角色的连接关系(对于BMU标识是否连接上BCU,对于BCU标识是否连接上BSU(如果有的话))
extern u8 My_Superior_Id;	         //(在与上级角色建立连接之后)用于标识上级的ID(对于BMU来说是BCU,对于BCU来说是BSU)
extern CanRxMsg CAN1RxMessage;              //用于保存处理CAN1当前接收到的信息帧
//BMU内部通信定义
#if IS_BMU_ROLE
extern u8 My_Pack_Number;           //用于标识BMU所测量的电池组编号,上传数据时使用该变量(范围:1开始且连续)
extern volatile u8 bmu_uploaddata;  //用于标识BMU是否上传数据
#endif
#if IS_BCU_ROLE
extern volatile u16 bmucontroltask_delay;     //BCU在控制数据上传等过程的任务中的循环延时时间
extern volatile u8 is_bmu_uploading;          //用于标识目前BMU是否在上传数据
extern volatile u8 is_bmu_acked;              //用于标识BMU是否对上传命令回复确认
#endif

extern u8 BMU_List_Id[BMU_LIST_NUM];       //BMU列表:BMU ID,用于系统内部运行
extern u32 BMU_List_Did[BMU_LIST_NUM];    //BMU列表：设备ID，用于上传到云端，在系统中使用
extern u8 BMU_List_PackNum[BMU_LIST_NUM];  //BMU列表:BMU管理的电池组编号
extern u8 BMU_List_State[BMU_LIST_NUM];    //BMU列表:BMU状态
extern u8 BMU_Connect_Num;         //标识BMU列表中的BMU连接数


/* BMU和BCU需要区分的数据区域 */
//单体电压和温度数据,以及SOC状态测算结果数据
#if IS_BMU_ROLE
extern u16 dataacq_time;      //BMU获取数据的时间间隔(单位为ms)
extern u16 heartbeat_time;    //心跳包时间间隔(单位为ms)
extern u16 dataupdate_time;   //BMU数据上传的时间间隔(单位为us)
extern volatile u16 tmpdataupdate_time;//临时的BMU数据上传时间间隔(单位为us)
extern u16 bmucellnum;        //当前BMU下的电池串数(1-48,最多48串)
extern u16 cellvol[BMU_VOL_NUM];       //当前板载采集芯片BMU设计:1个BMU最多能使用4个LTC6804芯片进行电压测量
extern float soc[BMU_VOL_NUM];              //当前BMU下的所有电池串的SOC状态(float类型,大于0小于1的小数),与电池串电压绑定
extern u16 bmutempnum;        //当前BMU下的温度测量点数
extern u16 temp[BMU_TEMP_NUM];         //当前设计:每个BMU最多能测量12个温度数据
extern float tempf[BMU_TEMP_NUM];           //转化为float格式的℃单位的温度数据
#endif
#if IS_BCU_ROLE
typedef struct _Cell {
  uint16_t id, vol;
} Cell;
extern Cell cell[BMU_LIST_NUM * BMU_VOL_NUM];
extern u16 bcucellnum;                    //当前BCU下的电池串数
extern u16 cellvol[BMU_LIST_NUM * BMU_VOL_NUM]; //需要在内部通信过程中将电压数据填至数组对应位置
extern volatile u16 max_cellvol;                   //电池串中最大电压值
extern volatile u16 min_cellvol;                   //电池串中最小电压值
extern volatile u16 max_cellvolid;                 //电池串中最大电压值对应电池串ID
extern volatile u16 min_cellvolid;                 //电池串中最小电压值对应电池串ID
extern volatile u16 max_diff_cellvol;              //电池串最大压差
extern volatile u32 cellvoltotal;                  //电池总电压(u32)
extern volatile float cellvolftotal;               //电池总电压(float)
extern volatile u16 cellvolaverage;                //平均单体电压
extern float soc[BMU_LIST_NUM * BMU_VOL_NUM];        //当前BCU下的所有电池串的SOC状态(float类型,大于0小于1的小数),与电池串电压绑定
extern u16 bcutempnum;                    //当前BCU下的温度测量点数
extern u16 temp[BMU_LIST_NUM * BMU_TEMP_NUM];   //需要在内部通信过程中将温度数据填至数组对应位置
extern float tempf[BMU_LIST_NUM * BMU_TEMP_NUM];     //转化为float格式的℃为单位的温度数据
extern volatile float max_tempf;                   //温度采集点中最大温度值
extern volatile float min_tempf;                   //温度采集点中最小温度值
extern volatile u16 max_tempfid;                 //温度采集点中最大温度值对应采集点编号
extern volatile u16 min_tempfid;                 //温度采集点中最小温度值对应采集点编号
extern volatile float max_diff_tempf;              //温度采集点最大温差
extern volatile float tempfaverage;                //温度采集点平均温?
#endif
//烟雾数据值
#if IS_BMU_ROLE
extern u8 bmusmokenum;                 //BMU烟雾数据个数
extern u16 smoke[BMU_SMOKE_NUM];       //BCU烟雾数据
extern float smokef[BMU_SMOKE_NUM];                     //BMU烟雾数据
#endif
#if IS_BCU_ROLE
extern u8 bcusmokenum;                 //BCU烟雾数据个数
extern u16 smoke[BMU_LIST_NUM * BMU_SMOKE_NUM];    //BCU烟雾数据
extern float smokef[BMU_LIST_NUM * BMU_SMOKE_NUM];    //BCU烟雾数据
#endif
/* BMU和BCU需要区分的数据区域结束 */

//显示屏显示数据相关变量
extern volatile u8 bmu_displaydata;  //用于标识BMU是否上传数据


//SOC状态计算数据

//变量声明
extern const u16 SOC_OCV[11];  //厂方提供的电池组SOC-OCV表格
extern float xk,Pk;            //用于存储Vmin-EKF算法计算过程中的中间变量(状态变量 协方差)

#define STANDTIME_LINE  600            //进行SOC标定的静置时间阈值(暂设定为10min)
#define STANDCURRENT_LINE  0.5            //表示判定为是否静置的电流值(暂设定为0.5A)
extern volatile float soctotal;        //总体SOC状态数据(不管是BCU还是单独的BMU,都是需要计算整体的SOC状态的)
extern u32 standtime;                  //系统静置时间,以秒为单位


//系统自上电起的运行时间
extern u32 runtime;  //以秒为单位(可记录>100年的时间)


//绝缘检测模块读取数据
extern u16 dcvol_data;    //系统直流电压(单位V)
extern u16 res_pos_data;  //正极接地电阻(单位kΩ)
extern u16 res_neg_data;  //负极接地电阻(单位kΩ)
extern volatile float insuval_pos;      //正极绝缘监测数据计算结果
extern volatile float insuval_neg;      //负极绝缘监测数据计算结果

//电流相关宏定义
#define CURRENT_STATE_CHARGE      0x00  //电流状态:充电
#define CURRENT_STATE_DISCHARGE   0x01  //电流状态:放电
//电表模块读取数据,及BMU电流数据值
extern u8 em_addr[6];  //低位在前高位在后,即[A0 A1 A2 A3 A4 A5],与数据帧顺序相同
extern u8 current[4];  //BCD码显示,高位在前低位在后,与数据帧顺序相反,前2字节是表示整数部分,后2字节表示小数部分
extern volatile float currentf;      //总电流数据(float类型,4字节存储)(该数据的读取还应考虑是否与对应的分流器相匹配)(此处有正负,正为充电,负为放电)
extern u8 is_charging;               //用于标识当前是在充电还是在放电(1标识在给电池组充电,0标识在给电池组放电)
extern float bmucurrentf[BMU_LIST_NUM];


//总压数据
extern float voltotal[3];

//CC状态相关宏定义
#define CC_STATE_CONNECTED   1  //充电枪和充电座建立连接(连接上外部1kΩ电阻,CC2端电压为6V)
#define CC_STATE_DISCONNECT  0  //充电枪和充电座未连接(未连接外部1kΩ电阻,CC2端电压为12V)
//CC状态数据
extern volatile u8 ccstate;                //BMU充放电接口连接状态(值为CC_STATE_CONNECTED或CC_STATE_DISCONNECT)
extern u8 ccstatelist[BMU_LIST_NUM];    //BCU下的BMU充放电接口连接状态(具体个数与BMU个数对应,以BMU_Connect_Num指示)

//BMS供电电压
extern volatile u16 powervolu16;         //16进制供电电压(ADC转换+换算后结果)
extern volatile float powervolf;       //float类型供电电压(电压真实值)

//MCU的ID获取地址
#ifdef __STM32F00x_H
#define  ID_ADDR1  0x1FFFF7AC      /*STM32F0唯一ID起始地址*/
#endif
#ifdef __STM32F10x_H
#define  ID_ADDR1  0x1FFFF7E8     /*STM32F1唯一ID起始地址*/
#endif
#ifdef __STM32F2xx_H
#define  ID_ADDR1  0x1FFF7A10         /*STM32F2唯一ID起始地址*/
#endif
#ifdef __STM32F3xx_H
#define  ID_ADDR1  0x1FFFF7AC        /*STM32F3唯一ID起始地址*/
#endif
#ifdef __STM32F4xx_H
#define  ID_ADDR1  0x1FFF7A10        /*STM32F4唯一ID起始地址*/
#endif
#ifdef __STM32F7xx_H
#define  ID_ADDR1  0x1FF0F420        /*STM32F7唯一ID起始地址*/
#endif
#ifdef  __STM32L0XX_H
#define  ID_ADDR1  0x1FF80050        /*STM32L0唯一ID起始地址*/
#endif
#ifdef __STM32L1XX_H
#define  ID_ADDR1  0x1FF80050        /*STM32L1唯一ID起始地址*/
#endif
#ifdef __STM32L4xx_H
#define  ID_ADDR1  0x1FFF7590        /*STM32L4唯一ID起始地址*/
#endif
#ifdef __STM32H7xx_H
#define  ID_ADDR1  0x1FF0F420        /*STM32H7唯一ID起始地址*/
#endif

#endif

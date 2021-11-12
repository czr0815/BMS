
/* 
  用于BMS系统状态数据的定义
*/

#include "bms_state.h"
#include <stdint.h>
#include "sys.h"

//CAN1内部通信相关变量定义
u8 My_BMS_Role;             //用于标识自身的BMS角色(BMU/BCU)
u8 My_Id;                   //用于标识自身ID，用于内部通信
u32 My_Did;                 //表示自身设备ID，用于云端上传和系统运行
u8 Is_Connected;            //用于标识与上级角色的连接关系(对于BMU标识是否连接上BCU,对于BCU标识是否连接上BSU(如果有的话))
u8 My_Superior_Id;	         //(在与上级角色建立连接之后)用于标识上级的ID(对于BMU来说是BCU,对于BCU来说是BSU)
CanRxMsg CAN1RxMessage;              //用于保存处理CAN1当前接收到的信息帧
//BMU内部通信定义
#if IS_BMU_ROLE
u8 My_Pack_Number;           //用于标识BMU所测量的电池组编号,上传数据时使用该变量
volatile u8 bmu_uploaddata;  //用于标识BMU是否上传数据
#endif
#if IS_BCU_ROLE
volatile u16 bmucontroltask_delay;     //BCU在控制数据上传等过程的任务中的循环延时时间
volatile u8 is_bmu_uploading;          //用于标识目前BMU是否在上传数据
volatile u8 is_bmu_acked;              //用于标识BMU是否对上传命令回复确认
#endif

u8 BMU_List_Id[BMU_LIST_NUM];       //BMU列表:BMU ID,用于系统内部运行
u32 BMU_List_Did[BMU_LIST_NUM];    //BMU列表：设备ID，用于上传到云端，在系统中使用
u8 BMU_List_PackNum[BMU_LIST_NUM];  //BMU列表:BMU管理的电池组编号
u8 BMU_List_State[BMU_LIST_NUM] = {0};    //BMU列表:BMU状态
u8 BMU_Connect_Num;         //标识BMU列表中的BMU连接数



/* BMU和BCU需要区分的测量数据区域 */
//单体电压和温度数据,以及SOC状态测算结果数据
#if IS_BMU_ROLE
u16 dataacq_time;      //BMU获取数据的时间间隔(单位为ms)
u16 heartbeat_time;    //心跳包时间间隔(单位为ms)
u16 dataupdate_time;   //BMU数据上传的时间间隔(单位为us)
volatile u16 tmpdataupdate_time;//临时的BMU数据上传时间间隔(单位为us)
u16 bmucellnum;        //当前BMU下的电池串数(1-48,最多48串)
u16 cellvol[BMU_VOL_NUM];       //当前板载采集芯片BMU设计:1个BMU最多能使用4个LTC6804芯片进行电压测量
float soc[BMU_VOL_NUM];              //当前BMU下的所有电池串的SOC状态(float类型,大于0小于1的小数),与电池串电压绑定
u16 bmutempnum;        //当前BMU下的温度测量点数
u16 temp[BMU_TEMP_NUM];         //当前设计:每个BMU最多能测量12个温度数据
float tempf[BMU_TEMP_NUM];           //转化为float格式的℃单位的温度数据
#endif
#if IS_BCU_ROLE
Cell cell[BMU_LIST_NUM * BMU_VOL_NUM];
u16 bcucellnum;                    //当前BCU下的电池串数
u16 cellvol[BMU_LIST_NUM * BMU_VOL_NUM]; //需要在内部通信过程中将电压数据填至数组对应位置
volatile u16 max_cellvol;                   //电池串中最大电压值
volatile u16 min_cellvol;                   //电池串中最小电压值
volatile u16 max_cellvolid;                 //电池串中最大电压值对应电池串ID
volatile u16 min_cellvolid;                 //电池串中最小电压值对应电池串ID
volatile u16 max_diff_cellvol;              //电池串最大压差
volatile u32 cellvoltotal;                  //电池总电压(u32)
volatile float cellvolftotal;               //电池总电压(float)
volatile u16 cellvolaverage;                //平均单体电压
float soc[BMU_LIST_NUM * BMU_VOL_NUM];      //当前BCU下的所有电池串的SOC状态(float类型,大于0小于1的小数),与电池串电压绑定
u16 bcutempnum;                    //当前BCU下的温度测量点数
u16 temp[BMU_LIST_NUM * BMU_TEMP_NUM];   //需要在内部通信过程中将温度数据填至数组对应位置
float tempf[BMU_LIST_NUM * BMU_TEMP_NUM];   //转化为float格式的℃为单位的温度数据
volatile float max_tempf;                   //温度采集点中最大温度值
volatile float min_tempf;                   //温度采集点中最小温度值
volatile u16 max_tempfid;                 //温度采集点中最大温度值对应采集点编号
volatile u16 min_tempfid;                 //温度采集点中最小温度值对应采集点编号
volatile float max_diff_tempf;              //温度采集点最大温差
volatile float tempfaverage;                //温度采集点平均温度
#endif
//烟雾数据值
#if IS_BMU_ROLE
u8 bmusmokenum;                 //BMU烟雾数据个数
u16 smoke[BMU_SMOKE_NUM];                   //采集上来的烟雾传感器电压值
float smokef[BMU_SMOKE_NUM];                     //BMU烟雾数据
#endif
#if IS_BCU_ROLE
u8 bcusmokenum;                 //BCU烟雾数据个数
u16 smoke[BMU_LIST_NUM * BMU_SMOKE_NUM];    //BCU烟雾数据
float smokef[BMU_LIST_NUM * BMU_SMOKE_NUM];    //BCU烟雾数据
#endif  
/* BMU和BCU需要区分的数据区域结束 */

//显示屏显示数据相关变量
volatile u8 bmu_displaydata;  //用于标识BMU是否上传数据


//SOC状态计算数据

//相关变量定义
//厂方提供的SOC-OCV表格:  0%    10%   20%   30%   40%   50%   60%   70%   80%   90%  100%
const u16 SOC_OCV[11] = {2723, 3227, 3259, 3280, 3289, 3292, 3301, 3316, 3327, 3333, 3400};
float xk,Pk;           //用于存储Vmin-EKF算法计算过程中的中间变量(状态变量 协方差)

volatile float soctotal;                         //总体SOC状态数据(不管是BCU还是单独的BMU,都是需要计算整体的SOC状态)
u32 standtime;                                   //系统静置时间,以秒为单位




//系统自上电起的运行时间
u32 runtime;  //以秒为单位(可记录>100年的时间)

//绝缘检测模块读取数据
u16 dcvol_data;    //系统直流电压(单位V)
u16 res_pos_data;  //正极接地电阻(单位kΩ)
u16 res_neg_data;  //负极接地电阻(单位kΩ)
volatile float insuval_pos;      //正极绝缘监测数据计算结果
volatile float insuval_neg;      //负极绝缘监测数据计算结果

//电表模块读取数据,及BMU电流数据值
u8 em_addr[6] = {0x01,0x00,0x00,0x00,0x00,0x00};    //低位在前高位在后,即[A0 A1 A2 A3 A4 A5],与数据帧顺序相同
u8 current[4];                                      //BCD码显示,高位在前低位在后,与数据帧顺序相反,前2字节是表示整数部分,后2字节表示小数部分
volatile float currentf;                                          //总电流数据(float类型,4字节存储)
u8 is_charging;               //用于标识当前是在充电还是在放电(1标识在给电池组充电,0标识在给电池组放电)
float bmucurrentf[BMU_LIST_NUM];                      //BCU下各个BMU的电流数据(具体个数与BMU个数对应,以BMU_Connect_Num指示)


//总压数据
float voltotal[3];

//CC状态
volatile u8 ccstate;                //BCU充放电接口连接状态
u8 ccstatelist[BMU_LIST_NUM];    //BCU下的BMU充放电接口连接状态(具体个数与BMU个数对应,以BMU_Connect_Num指示)

//BMS供电电压
volatile u16 powervolu16;         //16进制供电电压(ADC转换+换算后结果)
volatile float powervolf;       //float类型供电电压(电压真实值)





#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	   


//****************************************CAN1相关**************************************
//BMS内部CAN1通信相关配置:

//目前的ID设置方法可能有:
//    (1)BMS内每个CAN通信节点都将ID存储在内存卡中,运行时从内存卡中读取BMS设备类型和设备ID
//    (2)BMS内BMU和BSU(如果存在)由于数量较少,可以事先同一指定(SD卡或在烧写程序时直接定义),然后BMU在和BCU通信过程中被指派ID
//    	(1)方法相较于(2)可能更具有普遍性
//    (3)用32/64位ID唯一标识公司生产的一块CAN2通信模块,将该唯一ID存储在SD内存卡中,设备插卡运行时,建立BMS系统内8位临时ID和
//       该唯一ID的对应关系(并同步到云端),BMS系统内通信只使用8位临时ID

//定义不同帧类型的PF (在本BMS内部协议中,使用PDU1格式,此时PS为目标地址DA,同时R和DP都设为0,29位标识符最低字节为源地址;
//                    PDU1格式下PGN=PF*256,因此此处直接使用PF作为帧类型的唯一标识,长度为1个字节)

// 协议数据单元(PDU)格式：
// |  保留  |   P   |   R  |  DP  |     PF    |    PS    |    SA    |    DATA    |  
// | 3bits  | 3bits | 1bit | 1bit |   8bits   |   8bits  |   8bits  |  0~64bits  |

//不同类型信息帧的PF
#define PF_COMCONTROL     0x01    //通信控制帧
#define PF_DATA           0x02    //数据帧
#define PF_CONTROLCMD     0x04    //控制命令帧
#define PF_ALARMINFO      0x08    //告警信息帧
//上述不同帧类型的优先级P
#define PF_COMCONTROL_P   0x01    //通信控制帧
#define PF_DATA_P         0x04    //数据帧
#define PF_CONTROLCMD_P   0x02    //控制命令帧
#define PF_ALARMINFO_P    0x03    //告警信息帧
//不同类型信息帧下的分类
#define FT_COMCONTROL_HEARTBEAT       0x01    //通信控制：心跳包
#define FT_COMCONTROL_ACK             0x02    //通信控制：确认帧
#define FT_DATA_CELL                  0x03    //数据：电池电压及SOC状态
#define FT_DATA_TEMP                  0x04    //数据：温度
#define FT_DATA_SMOKE                 0x05    //数据：烟雾
#define FT_DATA_CCSTATE               0x06    //数据：CC信号状态
#define FT_DATA_CURRENT               0x07    //数据: 电流
#define FT_CONTROLCMD_UPLOADDATA      0x08    //控制指令: 上传数据(指定某个BMU开始上传数据)
#define FT_CONTROLCMD_CELLBALANCE     0x09    //控制指令：电池均衡(开启/停止电池均衡)
#define FT_CONTROLCMD_TEMPCONTROL     0x0A    //控制指令：温度控制
#define FT_CONTROLCMD_DANGERDEAL      0x0B    //控制指令：险情处理
#define FT_CONTROLCMD_SWITCH          0x0C    //控制指令：无源开关控制
#define FT_CONTROLCMD_UPLOADPATTERN   0x0D    //控制指令：改变BMU上传数据帧的行为模式
#define FT_CONTROLCMD_ACK             0x0E    //控制指令：BMU给BCU回复确认信息

//控制命令帧类型中不同种类的均衡控制帧
#define CONTROLCMD_CELLBALANCE_ON     0x19    //均衡控制命令:开启均衡
#define CONTROLCMD_CELLBALANCE_OFF    0x29    //均衡控制命令:关闭均衡


//控制命令帧类型中不同种类的回复确认信息
#define CONTROLCMD_ACK_UPLOAD          0x1E   //控制命令确认：上传数据命令确认
#define CONTROLCMD_ACK_UPLOAD_COMPLETE 0x2E   //控制命令确认：上传数据完成确认


//相关宏定义
#define BMS_ROLE_BMU        0x01    //BMU角色的标识符
#define BMS_ROLE_BCU        0x02    //BCU角色的标识符
#define BMS_ROLE_BSU        0x04    //BSU角色的标识符

//CAN1接收RX0中断使能
#define CAN1_RX0_INT_ENABLE 1

//CAN1初始化
void CAN1_Mode_Init();

#if CAN1_RX0_INT_ENABLE  //使能CAN1的FIFO0中断		    
void CAN1_RX0_IRQHandler(void);  //CAN1中断服务函数	
#endif

u8 CAN1_Send_Msg(u32 msgid, u8* msg, u8 len);  //CAN1发送数据
u8 CAN1_ComControl(u8 dst_id, u8 FT);  //CAN1内部通信：发送通信控制帧
u8 CAN1_ControlCmd(u8 dst_id, u8 FT, u8 cmd1, u8 cmd2, u16 timecmd);  //CAN1内部通信: 发送控制命令帧
u8 CAN1_Send_Data(u8 dst_id, u8 FT, u8 chip, u8 num, u16 data1u16, float data2f);  //CAN1内部通信:发送数据帧

void start_can1_connect();
u8 CAN1_RxMsg_Process();





#endif



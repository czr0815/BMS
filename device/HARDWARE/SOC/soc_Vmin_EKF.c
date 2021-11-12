
/* SOC估算相关说明
- 本文件给出了估算整个电池组的SOC状态的方法实现;
- 电池组SOC状态的估算方法主要是:基于最小电压的扩展卡尔曼滤波方法;
  - (1)实现原理是将非线性的电池组状态方程和观测方程进行离散化和线性化(泰勒展开),然后应用卡尔曼滤波方法对SOC状态进行估算;
  - (2)Vmin-EKF算法的实现步骤主要有:
	- (3)初始化SOC状态;
	- (4)递推SOC状态:
	  - (a)状态时间更新;
	  - (b)误差协方差时间更新;
	  - (c)计算Kalman增益;
	  - (d)状态估计观测更新;
	  - (e)误差协方差观测更新;
- 整体的电池组SOC估算过程还包括:
  - (1)OCV-SOC标定校准:
	  - (a)刚上电和充电状态不校准,使用(片上Flash)存储的当前或断电前的容量来作为SOC状态估算的依据;
		- (b)当BMS上电后电池组静置(电流在该段时间内数值都小于|1%FSR|)时间较长时(可设定为0.5h/1h/1.5h/2h等),根据厂方提供的SOC-OCV表格进行标定;
  - (2)充电末端校准:BMS检测到"充电单体过压3级"时,则强制将SOC校准至100%;未达到之前,最大显示99%(但估计当电池健康状态相差较大时,这个数不会很大);
  - (3)放电末端校准:BMS检测到"放电单体欠压3级"时,则强制将SOC校准至0%;未达到之前,最低显示1%(目测比较贴合实际)
*/

#include "sys.h"
#include "soc_Vmin_EKF.h"
#include "math.h"
#include "bms_state.h"

////相关变量定义
////厂方提供的SOC-OCV表格:  0%    10%   20%   30%   40%   50%   60%   70%   80%   90%  100%
//const u16 SOC_OCV[11] = {2723, 3227, 3259, 3280, 3289, 3292, 3301, 3316, 3327, 3333, 3400};
//float xk,Pk;           //用于存储Vmin-EKF算法计算过程中的中间变量(状态变量 协方差)






/* 写入当前SOC数值
功能:将当前计算得到的SOC数值写入到对应位置
参数:当前SOC数值
返回值:无
*/
void soc_write(float socnow) {
	//写入到全局变量中
	soctotal = socnow;
	
	//写入到片上FLASH中
	STMFlash_Write_SOC(soctotal);

	return;
}

/* 读取前一时刻SOC数值
功能:读取存储在片上的前一时刻的SOC数值
参数:无
返回值:前一时刻的SOC数值
*/
float soc_read(void) {
	float socprevious;

  //从Flash中读取SOC数值
	socprevious = STMFlash_Read_SOC();
  soctotal = socprevious;

	return socprevious;
}


/* SOC标定
功能:当电池组静置较长时间时,应用SOC-OCV表格对电池组SOC进行标定,会将标定后的SOC数值同步到全局变量和片上Flash中
参数:用于标定的电池电压(如使用最小单体电池电压进行标定),该数值为5位数,而非以mV为单位的4位数;
返回值:标定后的SOC数值;
*/
float soc_calibrate(u16 cellvoltocalb)
{
	u8 i;
	float soccalbed;  //保存标定得到的SOC数值
	cellvoltocalb/= 10;  //换算到与SOC-OCV表格同量级
	if(cellvoltocalb<SOC_OCV[0] || cellvoltocalb>SOC_OCV[10]){  //电池电压输入范围错误
		return -1;
	}
	
	//标定过程
	for(i=0;i<10;i++) {
		if((cellvoltocalb>=SOC_OCV[i])&&(cellvoltocalb<SOC_OCV[i+1])){  //在某一区间内
			if((cellvoltocalb-SOC_OCV[i])<(SOC_OCV[i+1]-cellvoltocalb)) {  //更靠近左侧
				soccalbed = 0.1 * i;
			}
			else {  //更靠近右侧
				soccalbed = 0.1 * (i+1);
			}
		}
	}
	
	//写入标定得到的SOC数值
	//soc_write(soccalbed);
	
	
	//返回标定得到的SOC数值
	return soccalbed;
}

/*计算新的SOC状态值
功能:根据Vmin-EKF算法计算新的SOC状态值;
过程:计算过程中,使用输入参数和全局变量xk Pk作为计算依据,返回计算得到的soc数值,计算过程中会更新xk和Pk的值
参数:
  - ik_1: 当前电池组的电流,有正负,正为充电,负为放电,单位安培
  - yku16: 全局变量形式保存的(5位数的值)
  - t:计算SOC的时间间隔,以s为单位;
返回值:计算得到的SOC数值(状态变量x即为SOC);
*/
float soc_calculate(float ik_1, u16 yku16, float t) {
	float yk;  //float类型单体电压值
	float xk_1,Pk_1;  //前一时刻的状态变量和协方差(k_1表示k-1,即前一时刻)
	float xk_,Pk_;    //时间更新方程计算得到的值,即分别表示xk-和Pk-
	float Ck,Kk;
	float gk;
	
	/* 以下为Vmin-EKF的SOC估算过程
	//计算过程
	yk = (float)yku16 * 0.0001;  //将全局变量中保存的u16类型的单体电压值转换为float类型
	
	//时间更新方程
	//[1]状态变量时间更新方程
	xk_1 = xk;  //前一时刻状态变量值
	xk_ = xk_1 + eta * t * ik_1 / Cn;  //更新方程
	
	//[2]误差协方差时间更新方程
	Pk_1 = Pk;  //前一时刻协方差矩阵
	Pk_ = Pk_1;  //+ Q;  //协方差更新方程,Q为wk的协方差矩阵
	
	//状态更新方程
	//[1]计算卡尔曼增益
	Ck = K0/(xk_*xk_) - K1 + K2/xk_ - K3/(1-xk_);
	Kk = 1 / Ck;  //卡尔曼增益(单数值矩阵中计算简化
	//Kk = Pk_ * Ck / (Ck * Pk_ * Ck);  //卡尔曼增益
	//Kk = Pk_ * Ck / (Ck * Pk_ * Ck + R);  //卡尔曼增益,R为vk的协方差矩阵
	
	//[2]状态估计观测更新方程
	gk = V0 - ik_1*R - K0/xk_ - K1*xk_ + K2*ln(xk_) + K3*ln(1-xk_);  //ln()函数需要解决
	xk = xk_ + Kk * (yk - gk);
	
	//[3]误差协方差观测更新方程
	Pk = (I- Kk*Ck);
	
	*/
	
	//以下为电流积分法的SOC估算过程
	xk_1 = xk;  //前一时刻SOC状态
	xk = xk_1 + eta*t*ik_1/Cn;  //电流积分
	
	
	return xk;
}


float ln(float x) {
	
	
	
	return x;
}


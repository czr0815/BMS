
/*
	说明: 输入输出接口控制
	直接通过MCU的引脚输出来控制BMU/BCU上的继电器来实现控制功能;
*/

#include "bms_state.h"
#include "input_output_ctrl.h"
#include "rs485_screen.h"




//控制功能初始化
void IOControl_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
#if IS_BMU_ROLE  //如果是BMU	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  //使能GPIOE时钟
	
	//配置BMU无源开关输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);  //初始化
	GPIO_ResetBits(GPIOE,GPIO_Pin_9);
	GPIO_ResetBits(GPIOE,GPIO_Pin_10);
	GPIO_ResetBits(GPIOE,GPIO_Pin_11);
	GPIO_ResetBits(GPIOE,GPIO_Pin_12);
	GPIO_ResetBits(GPIOE,GPIO_Pin_13);
	GPIO_ResetBits(GPIOE,GPIO_Pin_14);
	GPIO_ResetBits(GPIOE,GPIO_Pin_15);
	
#endif
	
#if IS_BCU_ROLE  //如果是BCU

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  //使能GPIOE和GPIOG时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//配置BCU控制功能:强电控制输出接口×3,有源输出×8,干接点输出×1,(有源量输入×2,还未初始化)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);  //初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
  GPIO_Init(GPIOG, &GPIO_InitStructure);  //初始化	
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_9);
	GPIO_ResetBits(GPIOE,GPIO_Pin_10);
	GPIO_ResetBits(GPIOE,GPIO_Pin_11);
	GPIO_ResetBits(GPIOE,GPIO_Pin_12);
	GPIO_ResetBits(GPIOE,GPIO_Pin_13);
	GPIO_ResetBits(GPIOE,GPIO_Pin_14);
	GPIO_ResetBits(GPIOE,GPIO_Pin_15);	
	GPIO_ResetBits(GPIOG,GPIO_Pin_2);
	GPIO_ResetBits(GPIOG,GPIO_Pin_3);
	GPIO_ResetBits(GPIOG,GPIO_Pin_4);
	GPIO_ResetBits(GPIOG,GPIO_Pin_5);
	GPIO_ResetBits(GPIOG,GPIO_Pin_6);		
	
#endif
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	//PD15,主控板插板引脚功能指示灯
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化
	GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	
}

/*
	功能:开启/关闭控制继电器
	参数:pin_num为BMU/BCU的继电器标号(BMU:1-7,BCU:1-12),pin_ctrl指示开启/关闭该继电器(1为拉高引脚,CON_OUTPUT_x为0,继电器闭合;0则为继电器断开)
	返回值:如果两个输入参数都符合要求,则返回1;否则返回0
*/
u8 IOControl(u8 pin_num, u8 pin_ctrl)
{
#if IS_BMU_ROLE
	
	if(!((pin_num>0)&&(pin_num<8)&&((pin_ctrl==0)||(pin_ctrl==1))))
		return 0;
	
	if(pin_ctrl==IO_DISCONNECT) {
		GPIO_ResetBits(GPIOE,GPIO_Pin_8<<pin_num);  //pin_num=1对应PE9
	}
	else if(pin_ctrl==IO_CLOSE) {
		GPIO_SetBits(GPIOE,GPIO_Pin_8<<pin_num);
	}
	return 1;
#endif
	
#if IS_BCU_ROLE
	
	if(!((pin_num>0)&&(pin_num<13)&&((pin_ctrl==0)||(pin_ctrl==1))))
		return 0;
	
	if(pin_ctrl==IO_DISCONNECT) {
		if(pin_num<8) {
			GPIO_ResetBits(GPIOE,GPIO_Pin_8<<pin_num);  //pin_num=1对应PE9
		}
		else {
			GPIO_ResetBits(GPIOG,GPIO_Pin_2<<(pin_num-8));  //pin_num=8对应PG2
		}
	}
	else if(pin_ctrl==IO_CLOSE) {
		if(pin_num<8) {
			GPIO_SetBits(GPIOE,GPIO_Pin_8<<pin_num);  //pin_num=1对应PE9
		}
		else {
			GPIO_SetBits(GPIOG,GPIO_Pin_2<<(pin_num-8));  //pin_num=8对应PG2
		}		
	}
	return 1;
#endif	

}

/* 功能:向显示屏发送继电器1/2/3断开状态数据;
参数:无;
返回值:无;
*/
void Show_RelayState(void)
{
	RS485_Screen_WriteMem(RELAY1_STATE_ADDR,0);  //在屏幕上显示继电器1状态为0(断开)
	RS485_Screen_WriteMem(RELAY2_STATE_ADDR,0);  //在屏幕上显示继电器2状态为0(断开)	
	RS485_Screen_WriteMem(RELAY3_STATE_ADDR,0);  //在屏幕上显示继电器3状态为0(断开)
}	



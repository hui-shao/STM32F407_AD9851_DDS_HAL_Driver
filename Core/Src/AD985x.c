/***************************************************************************
					   康威电子
功能：stm32f407ZGT6控制AD985x模块，程序兼容AD9850和AD9851模块
接口：控制引脚接口请参照AD985x.h
时间：2023/06/xx
版本：0.7
作者：康威电子 / Hui-Shao
其他：本程序只供学习使用，盗版必究。
说明：本店AD9850模块板载晶振125Mhz，作为参考时钟输入。芯片不支持倍频
			本店AD9851模块板载晶振30Mhz,程序默认配置6倍频，即180Mhz，作为参考时钟输入。

					AD985x			单片机
硬件连接:			 AD985x_rst 	PD4
					AD985x_clk 	    PD0
					AD985x_fq 	    PD5
					D0-07 			PD0-PD7 // 串行模式下，只使用 D7 ，此次连接到单片机 PD3
					5V					--5V
					GND					--GND(0V)

					注意：可以参考CubeMX

更多电子需求，请到淘宝店，康威电子竭诚为您服务 ^_^
https://kvdz.taobao.com/
**************************************************************************/

#include "AD985x.h"

uint8_t AD985x_FD; // 倍频
uint8_t AD985x;	   // 要驱动的模块型号
uint8_t LOAD_MODE; // 串or并行驱动

/***************************************************************************
** 函数名称 ：void AD985x_Init(uint8_t AD985x ,uint8_t Load_Mode)
** 函数功能 ：初始化控制AD985x需要用到的IO口及模式串并行加载模式
** 入口参数 ：cm：要驱动的型号，AD9850;AD9851
**						sp：驱动方式，PARALLEL并行驱动；SERIAL串行驱动
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************************/
void AD985x_Init(uint8_t cm, uint8_t sp)
{
	AD985x = cm;
	LOAD_MODE = sp;

	// AD985x_IO_Init(); // 使用 CubeMX 接管 IO 控制
	AD985x_DataBus = 0xFFFF0000;

	if (AD985x == AD9850)		 // AD9850
		AD985x_FD = AD985x_FD_0; // 不倍频
	else if (AD985x == AD9851)	 // AD9851
		AD985x_FD = AD985x_FD_6; // 使能6倍频

	if (LOAD_MODE == PARALLEL) // 并口
	{
		AD985x_reset_parallel(); // 复位为(并口模式)
	}
	else if (LOAD_MODE == SERIAL) // 串口
	{
		D0 = 1;
		D1 = 1;
		D2 = 0;				   // 当使用串行驱动时，D0、D1、D2脚需输入（110）固定电平。
		AD985x_reset_serial(); // 复位为(串口模式)
	}
}

/***************************************************************************
** 函数名称 ：void AD985x_IO_Init(void)
** 函数功能 ：初始化控制AD985x需要用到的IO口
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无

使用 CubeMX 接管 IO 初始化，无需使用此函数

*****************************************************************************/
// void AD985x_IO_Init(void)
// {
// 	GPIO_InitTypeDef GPIO_InitStructure ;

// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PC端口时钟

// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 ;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
// 	GPIO_Init(GPIOC ,&GPIO_InitStructure) ;

// 	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3|GPIO_Pin_4| GPIO_Pin_6;
// 	GPIO_Init(GPIOA ,&GPIO_InitStructure) ;
// }

/***************************************************************************
** 函数名称 ：void AD985x_reset_parallel()
** 函数功能 ：AD985x复位(并口模式)
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************************/
void AD985x_reset_parallel(void)
{
	AD985x_clk = 0;
	AD985x_fq = 0;
	// rest信号
	AD985x_rst = 0;
	AD985x_rst = 1;
	AD985x_rst = 0;
}

/***************************************************************************
** 函数名称 ：void AD985x_reset_parallel()
** 函数功能 ：AD985x复位(串口模式)
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************************/
void AD985x_reset_serial(void)
{
	AD985x_clk = 0;
	AD985x_fq = 0;
	// rest信号
	AD985x_rst = 0;
	AD985x_rst = 1;
	AD985x_rst = 0;
	// w_clk信号
	AD985x_clk = 0;
	AD985x_clk = 1;
	AD985x_clk = 0;
	// fq_up信号
	AD985x_fq = 0;
	AD985x_fq = 1;
	AD985x_fq = 0;
}

/************************************************************
** 函数名称 ：uint32_t Convert_Freq(double Real_f)
** 函数功能 ：频率数据转换
** 入口参数 ：Real_f，需要转换的频率
** 出口参数 ：频率数据值
** 函数说明 ：
**************************************************************/
uint32_t Convert_Freq(double Real_f)
{
	if (AD985x == AD9850) // AD9850
		return (uint32_t)(AD9850_f_Num * Real_f);
	else if (AD985x == AD9851) // AD9851
		return (uint32_t)(AD9851_f_Num * Real_f);
	return (uint32_t)(AD9851_f_Num * Real_f);
}

/***************************************************************************
** 函数名称 ：void AD985x_wr_parrel(double fre,uint8_t phase,uint8_t mul)
** 函数功能 ： 向AD985x中写命令与数据(并口)
** 入口参数 ：fre: 频率
							phase:相位 0-31 ，一个控制字代表11.25°
							mul:倍频 AD9850芯片不支持倍频
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************************/
void AD985x_wr_parrel(double fre, uint8_t phase, uint8_t mul)
{
	uint32_t w, y;

	y = Convert_Freq(fre);

	w = (phase << 3) | mul;
	AD985x_DataBus = w | (w ^ 0xff) << 16; // 写w0数据
	AD985x_clk = 1;
	AD985x_clk = 0;

	w = (uint8_t)(y >> 24);
	AD985x_DataBus = w | (w ^ 0xff) << 16; // w1
	AD985x_clk = 1;
	AD985x_clk = 0;

	w = (uint8_t)(y >> 16);
	AD985x_DataBus = w | (w ^ 0xff) << 16; // w2
	AD985x_clk = 1;
	AD985x_clk = 0;

	w = (uint8_t)(y >> 8);
	AD985x_DataBus = w | (w ^ 0xff) << 16; // w3
	AD985x_clk = 1;
	AD985x_clk = 0;

	w = (uint8_t)(y);
	AD985x_DataBus = w | (w ^ 0xff) << 16; // w4
	AD985x_clk = 1;
	AD985x_clk = 0;

	// 移入始能
	AD985x_fq = 1;
	AD985x_fq = 0;
}

/***************************************************************************
** 函数名称 ：void AD985x_wr_serial(double fre,uint8_t phase,uint8_t mul)
** 函数功能 ： 向AD985x中写命令与数据(串口)
** 入口参数 ：fre: 频率
							phase:相位 0-31 ，一个控制字代表11.25°
							mul:倍频 AD9850芯片不支持倍频
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************************/
void AD985x_wr_serial(double fre, uint8_t phase, uint8_t mul)
{
	uint8_t i, w;
	uint32_t y;

	y = Convert_Freq(fre);
	// 写w4数据
	w = (y >>= 0);
	for (i = 0; i < 8; i++)
	{
		AD985x_bit_data = (w >> i) & 0x01;
		AD985x_clk = 1;
		AD985x_clk = 0;
	}
	// 写w3数据
	w = (y >> 8);
	for (i = 0; i < 8; i++)
	{
		AD985x_bit_data = (w >> i) & 0x01;
		AD985x_clk = 1;
		AD985x_clk = 0;
	}
	// 写w2数据
	w = (y >> 16);
	for (i = 0; i < 8; i++)
	{
		AD985x_bit_data = (w >> i) & 0x01;
		AD985x_clk = 1;
		AD985x_clk = 0;
	}
	// 写w1数据
	w = (y >> 24);
	for (i = 0; i < 8; i++)
	{
		AD985x_bit_data = (w >> i) & 0x01;
		AD985x_clk = 1;
		AD985x_clk = 0;
	}
	// 写w0数据
	w = (phase << 3) | mul;
	for (i = 0; i < 8; i++)
	{
		AD985x_bit_data = (w >> i) & 0x01;
		AD985x_clk = 1;
		AD985x_clk = 0;
	}
	// 移入始能
	AD985x_fq = 1;
	AD985x_fq = 0;
}

/***************************************************************************
** 函数名称 ：void AD985x_Setfq(double fre,uint8_t phase)
** 函数功能 ：设置AD985x频率及相位
** 入口参数 ：fre: 频率
							phase:相位 0-31 ，一个控制字代表11.25°
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************************/
void AD985x_SetFre_Phase(double fre, uint8_t phase)
{
	if (LOAD_MODE == PARALLEL)
		AD985x_wr_parrel(fre, phase, AD985x_FD);
	else if (LOAD_MODE == SERIAL)
		AD985x_wr_serial(fre, phase, AD985x_FD);
}

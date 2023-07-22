#ifndef __AD985x_H
#define __AD985x_H
#include "sys.h"

#define AD9851 1
#define AD9850 0

#define PARALLEL 0 // 并口
#define SERIAL 1   // 串口

#define AD985x_FD_0 0 // 不倍频
#define AD985x_FD_6 1 // 使能6倍频

#define AD9850_CLK 125000000 // 参考时钟
#define AD9851_CLK 180000000 // 参考时钟

#define AD9850_f_Num 34.359738368 // 频率转换系数：2^32/系统时钟频率
#define AD9851_f_Num 23.860929422 // 频率转换系数：2^32/系统时钟频率

#define AD985x_rst PDout(4)      // 复位
#define AD985x_clk PDout(0)      // 串行时钟输入引脚
#define AD985x_fq PDout(5)       // 数据更新
#define AD985x_bit_data PDout(3) // 串行数据输入引脚

#define AD985x_DataBus GPIOD->BSRR // 并行控制脚PD0-7

#define D0 PDout(3)
#define D1 PDout(1)
#define D2 PDout(2)

extern uint8_t AD985x_FD; // 倍频
extern uint8_t AD985x;    // 要驱动的模块型号
extern uint8_t LOAD_MODE; // 串or并行驱动

void AD985x_IO_Init(void);                // 初始化控制AD985x需要用到的IO口
void AD985x_Init(uint8_t cm, uint8_t sp); // 初始化控制AD985x需要用到的IO口及模式串并行加载模式

void AD985x_reset_parallel(void); // AD985x复位(并口模式)
void AD985x_reset_serial(void);   // AD985x复位(串口模式)

uint32_t Convert_Freq(double Real_f); // 频率数据转换

void AD985x_wr_parrel(double fre, uint8_t phase, uint8_t mul); // 向AD985x中写命令与数据(并口)
void AD985x_wr_serial(double fre, uint8_t phase, uint8_t mul); // 向AD985x中写命令与数据(串口)
void AD985x_SetFre_Phase(double fre, uint8_t phase);           // 设置频率，相位

#endif

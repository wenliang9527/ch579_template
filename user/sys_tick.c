#define sys_tick_c_

#include "sys_tick.h"

uint64_t SysTickTimer=0;

/*获取系统运行时间(ms)*/
uint64_t sys_tick_get_timer(void)
{
	return SysTickTimer;
}

void SysTick_Handler(void)
{
	SysTickTimer++;
}


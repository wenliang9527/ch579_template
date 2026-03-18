#define TIME_C_

#include <stdio.h>
#include "time.h"
#include "uart.h"
#include "net_init.h"
#include "net_dns.h"
int timer0_value;

void time0_init(void)
{
	/*配置定时器0,定时器的时钟是FREQ_SYS*/
	TMR0_TimerInit( 1*(FREQ_SYS/1000) );   //定时1ms
	TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END); //使能定时器周期中断
	NVIC_SetPriority(TMR0_IRQn,6);//设置中断优先级(也可以不设置,默认3)
	NVIC_EnableIRQ( TMR0_IRQn );
}


/*TMR0周期中断*/
void TMR0_IRQHandler(void)
{
	
	if(TcpMybuf.Time100ms)TcpMybuf.Time100ms--;
	if(TcpMybuf.PCIntervaltime)TcpMybuf.PCIntervaltime--;
	if(TcpMybuf.SlaveIntervaltime)TcpMybuf.SlaveIntervaltime--;
	
	
	if( TMR0_GetITFlag( TMR0_3_IT_CYC_END ) )
	{
		TMR0_ClearITFlag( TMR0_3_IT_CYC_END );
		timer0_value++;
		
		CH57xNET_TimeIsr(CH57xNETTIMEPERIOD);/* 定时器中断服务函数 */
		net_dns_loop();
	}
}

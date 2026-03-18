#define net_dhcp_c

#include "net_dhcp.h"

#if 1 //是否打印日志
#define debug_printf printf
#else
#define debug_printf(...)
#endif

UINT8 net_dhcp_ip[4];   /* CH579IP地址 */
UINT8 net_dhcp_gw[4];   /* CH579网关 */
UINT8 net_dhcp_mask[4]; /* CH579子网掩码 */


void (*net_dhcp_callback)();

char net_dhcp_phy_state=0;

/*******************************************************************************
* Function Name  : CH57xNET_DHCPCallBack
* Description    : DHCP回调函数
* Input          : None
* Output         : None
* Return         : 执行状态
*******************************************************************************/
UINT8 CH57xNET_DHCPCallBack(UINT8 status,void *arg)
{
	UINT8 *p;
	if(!status){/* 成功*/
		p = arg;
		debug_printf("DHCP Success\n");
		memcpy(net_dhcp_ip,p,4);//获取分配的IP地址
		memcpy(net_dhcp_gw,&p[4],4);//获取网关
		memcpy(net_dhcp_mask,&p[8],4);//获取子网掩码
		debug_printf("IPAddr = %d.%d.%d.%d \r\n",(UINT16)net_dhcp_ip[0],(UINT16)net_dhcp_ip[1],(UINT16)net_dhcp_ip[2],(UINT16)net_dhcp_ip[3]);
		debug_printf("GWIPAddr = %d.%d.%d.%d \r\n",(UINT16)net_dhcp_gw[0],(UINT16)net_dhcp_gw[1],(UINT16)net_dhcp_gw[2],(UINT16)net_dhcp_gw[3]);
		debug_printf("IPAddr = %d.%d.%d.%d \r\n",(UINT16)net_dhcp_mask[0],(UINT16)net_dhcp_mask[1],(UINT16)net_dhcp_mask[2],(UINT16)net_dhcp_mask[3]);
		debug_printf("DNS1: %d.%d.%d.%d\r\n",p[12],p[13],p[14],p[15]);
		debug_printf("DNS2: %d.%d.%d.%d\r\n",p[16],p[17],p[18],p[19]);
		if(net_dhcp_callback!=NULL){
			net_dhcp_callback();
		}
	}
	else{/* 产生此中断，CH57xNET库内部会将此socket清除，置为关闭*/
		debug_printf("DHCP Fail %02x\n",status);
	}                                                      
	return 0;
}



/*使用说明
void dhcp_callback(void)
{
	每次dhcp成功会进入此函数,如果有需要初始化的在这里面初始化
}

把net_dhcp_while函数放到while(1)循环里面
while(1)
{
	net_dhcp_while(dhcp_callback);//DHCP
}
*/
void net_dhcp_while(void (*callback)())
{
	if(net_dhcp_phy_state != CH57xInf.PHYStat){//网线连接状态改变
		net_dhcp_phy_state = CH57xInf.PHYStat;
		if(net_dhcp_phy_state==2){//连接
			debug_printf("net_phy_connected\r\n");
			net_dhcp_callback = callback;
			CH57xNET_DHCPStart(CH57xNET_DHCPCallBack);/* 启动DHCP */ 
		}
		else{//断开
			debug_printf("net_phy_disconnected\r\n");
			CH57xNET_DHCPStop();//关闭DHCP
		}
	}
}



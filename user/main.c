#define MAIN_C_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "CH57x_common.h"
#include "uart.h"
#include "time.h"
#include "sys_tick.h"

#include "net_init.h"
#include "net_dhcp.h"
#include "net_dns.h"
#include "net_tcp_client.h"

#include "net_tcp_server.h"


Tcpbuf TcpMybuf;

//mydata Tmydata;


__align(4) UINT8  RxBuffer[ MAX_PACKET_SIZE ];  // IN, must even address
__align(4) UINT8  TxBuffer[ MAX_PACKET_SIZE ];  // OUT, must even address
	
 //unsigned char tcpdatabuf[rb_t_usart2_read_buff_len];
	

//缓存数据使用
UINT8 main_buffer[main_buffer_len];//缓存数据,全局通用
uint32_t  main_len;      //全局通用变量
UINT8 *main_str;    //全局通用变量
UINT8 client_ip[4]={160,128,1,100};       //计算机IP

net_tcp_server_struct net_tcp_server;//tcp_server变量

socket_client* socket1=NULL;//记录一个socket客户端
socket_client* socket2=NULL;//记录一个socket客户端
socket_client* socket3=NULL;//记录一个socket客户端
socket_client* socket4=NULL;//记录一个socket客户端






/*******************************************************************************
* Function Name  : CH57xNET_HandleGloableInt
* Description    : 事件处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_HandleGlobalInt(void)
{
	UINT8 initstat;
	UINT8 i;
	UINT8 socketinit;
	initstat = CH57xNET_GetGlobalInt();                                         /* 读全局中断状态并清除 */

	if(initstat & GINT_STAT_IP_CONFLI){/* IP冲突中断 */}
	if(initstat & GINT_STAT_PHY_CHANGE){/* PHY改变中断 */
		i = CH57xNET_GetPHYStatus();/* 获取PHY状态 */
	}
	if(initstat & GINT_STAT_SOCKET)
	{
		/* Socket中断 */
		for(i = 0; i < CH57xNET_MAX_SOCKET_NUM; i ++){
			socketinit = CH57xNET_GetSocketInt(i);/* 读socket中断并清零 */
			if(socketinit)
			{//有事件需要处理
				net_tcp_server_data(&net_tcp_server, i, socketinit);//处理TCP数据
			}
		}
	}
}

/*接收客户端消息回调函数*///网卡来的数据
void tcp_server_recv_back(socket_client * socket_client_value, unsigned char* data, unsigned long length)
	{
		UINT16 sendnum=0;
		TcpMybuf.SLOMIDUP=Internet_IDNumber;
		TcpMybuf.PCInitializationCOM=1;
		for(sendnum=0;sendnum<length;sendnum++)
			{
				uart_send_byte(TcpMybuf.SLOMIDUP,data[sendnum]);
			}
		
	
	}
/*客户端连接上服务器回调函数*/
void tcp_server_connected_back(socket_client * socket_client_value)
	{
		if(socket1 == NULL) socket1 = socket_client_value;
		else if(socket2 == NULL) socket2 = socket_client_value;
		else if(socket3 == NULL) socket3 = socket_client_value;
		else if(socket4 == NULL) socket4 = socket_client_value;
		TcpMybuf.SLOMIDUP=Internet_IDNumber;
}
/*客户端连接断开回调函数*/
void tcp_server_disconnected_back(socket_client * socket_client_value)
	{
		if(socket_client_value->socket_id == socket1->socket_id) socket1 = 0;
		if(socket_client_value->socket_id == socket2->socket_id) socket2 = 0;
		if(socket_client_value->socket_id == socket3->socket_id) socket3 = 0;
		if(socket_client_value->socket_id == socket4->socket_id) socket4 = 0;
 }


void tcp_send(unsigned char ip[4], int port, unsigned char* data, unsigned long length);


/**
* @brief   串口数据处理
* @param   None
* @retval  None
* @warning None
* @example
**/
void uart_data(void)
{           
//	//读取串口接收的数据
//	if(PCIssueBuf.TStatus)
//	{
//		PCIssueBuf.TStatus=0;
//		SlaveIssue_listADD(PCIssueBuf);

//	}
//	if(SlaveUploadBuf.TStatus)
//		{
//			SlaveUploadBuf.TStatus=0;
//			/*获取模组返回的数据*/
//			PCUpload_listADD(SlaveUploadBuf);
//		}
//		if(TcpMybuf.SlaveIntervaltime==0&&SlaveIssueBuf.TStatus)
//		{
//			SlaveIssue_listsend();
//		}
//		if(TcpMybuf.PCIntervaltime==0&&PCUploadBuf.TStatus)
//		{
//			PCUpload_listsend();
//		}
//		
}
/**
* @brief   给指定的客户端发送数据
* @param   客户端的ip地址, 客户端的端口号, 发送的数据, 数据长度 
* @retval  None
* @warning None
* @example
**/
void tcp_send(unsigned char ip[4], int port, unsigned char* data, unsigned long length)
{
	if(socket1->connected_state==1)
	{
		if(memcmp(SocketInf[socket1->socket_id].IPAddr, ip,4)==0 &&  SocketInf[socket1->socket_id].DesPort == port)
		{
			net_tcp_server_send(socket1, data, length);
		}
	}
	else if(socket2->connected_state==1)
	{
		if(memcmp(SocketInf[socket2->socket_id].IPAddr, ip,4)==0 &&  SocketInf[socket2->socket_id].DesPort == port)
		{
			net_tcp_server_send(socket2, data, length);
		}
	}
	else if(socket3->connected_state==1)
	{
		if(memcmp(SocketInf[socket3->socket_id].IPAddr, ip,4)==0 &&  SocketInf[socket3->socket_id].DesPort == port)
		{
			net_tcp_server_send(socket3, data, length);
		}
	}
	else if(socket4->connected_state==1)
	{
		if(memcmp(SocketInf[socket4->socket_id].IPAddr, ip,4)==0 &&  SocketInf[socket4->socket_id].DesPort == port)
		{
			net_tcp_server_send(socket4, data, length);
		}
	}
}
int main()
{
	PWR_UnitModCfg(ENABLE, UNIT_SYS_PLL);//使能PLL(给以太网提供时钟)                               
	DelayMs(3);
	SetSysClock(CLK_SOURCE_HSE_32MHz);                                          /* 外部晶振 PLL 输出32MHz */
	TcpMybuf.InitializationCOM=0;
	TcpMybuf.PCInitializationCOM=0;
	uart_init();//初始化串口
	net_init();//初始化以太网
	time0_init();//初始化定时器
//	printf("start\r\n");
	TcpMybuf.SLOMIDUP=USART_1_IDNumber;   //默认使用网口和TTL通讯
	TcpMybuf.SLOMIDDOWN=USART_3_IDNumber;
	/*******************************TCP服务器********************************************/
	net_tcp_server.port_local = 6666;//设置服务器监听的端口号
	net_tcp_server.socke_id = 255;//初始化该值为255(必须设置下),设置为255就可以
	net_tcp_server_close(&net_tcp_server);//尝试关闭socket
	/*创建TCP socket 服务器*/
	net_tcp_server_creat_socket(&net_tcp_server, tcp_server_connected_back, tcp_server_disconnected_back, tcp_server_recv_back);
//	PCUploadBuf.TStatus=true;
//	SlaveIssueBuf.TStatus=true;

	while(1)
	{
		//uart_data();  //数据接收处理
		CH57xNET_MainTask();/* CH57xNET库主任务函数，需要在主循环中不断调用 */
		/* 查询事件，如果有事件，则调用处理函数 */
		if(CH57xNET_QueryGlobalInt())CH57xNET_HandleGlobalInt();
	}
}





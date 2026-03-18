#define net_tcp_client_c_

#include "net_tcp_server.h"
#include "uart.h"



#if 1 //是否打印日志
#define debug_printf printf
#else
#define debug_printf(...)
#endif

//extern void uart_send_byte_it(char USARTx, unsigned char *c,uint32_t cnt);

socket_client socket_client_t[net_tcp_server_socket_max_len];


/**
* @brief   创建服务器
* @param   socketid
* @param   buf 数据地址
* @param   len 数据长度
* @retval  0:成功; others:失败
* @warning None
* @example 
**/
char net_tcp_server_creat_socket(net_tcp_server_struct *net_tcp_server, 
	net_tcp_server_client_connect_back connected_back, //连接成功回调函数
	net_tcp_server_client_connect_back disconnected_back ,//连接断开回调函数
	net_tcp_server_recv_back received_back)//接收数据回调函数
{
	int i=0;
	unsigned char state;                                                             
	SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */
	memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
	
	TmpSocketInf.SourPort = net_tcp_server->port_local;                          /* 设置源端口 */
	TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socekt类型 */
	TmpSocketInf.RecvStartPoint = (unsigned long)net_tcp_server->socket_recv_buf;                        /* 设置接收缓冲区的接收缓冲区 */
	TmpSocketInf.RecvBufLen = net_tcp_server_socket_recv_buf_len ;                                     /* 设置接收缓冲区的接收长度 */
	
	for(i=0; i<net_tcp_server_socket_max_len; i++)
	{
		socket_client_t[i].connected_state=0;
	}
	
	//创建socket
	state = CH57xNET_SocketCreat(&(net_tcp_server->socke_id),&TmpSocketInf);         /* 创建socket，将返回的socket索引保存在SocketId中 */
	if(state!=CH57xNET_ERR_SUCCESS) /* 检查错误 */
	{
		return state;
	}
	//心跳
	if(net_tcp_server->keeplive_enable==1){
		CH57xNET_SocketSetKeepLive( net_tcp_server->socke_id, 1 ); /* 开启socket的KEEPLIVE功能 */
	}
	
	if(connected_back != NULL) net_tcp_server->connected_back = connected_back;
	if(disconnected_back != NULL) net_tcp_server->disconnected_back = disconnected_back;
	if(received_back != 0) net_tcp_server->received_back = received_back;
	
	//监听
	state = CH57xNET_SocketListen(net_tcp_server->socke_id);
	if(state!=CH57xNET_ERR_SUCCESS) /* 检查错误 */
	{
		debug_printf("CH57xNET_SocketListen ERR: %02X\r\n", (unsigned short)state);
		return state;
	}
	
	
	return 0;
}




/**
* @brief   关闭socket
* @param   socketid
* @retval  0:成功; others:失败
* @warning None
* @example 
**/
char net_tcp_server_close(net_tcp_server_struct *net_tcp_server)
{
	int socke_id = net_tcp_server->socke_id;
	net_tcp_server->socke_id=255;
	return CH57xNET_SocketClose( socke_id, TCP_CLOSE_NORMAL );
}



/**
* @brief   发送数据
* @param   socketid
* @param   buf 数据地址
* @param   len 数据长度
* @retval  0:成功; others:失败
* @warning None
* @example 
**/
char net_tcp_server_send(socket_client * socket_client_value, unsigned char *buf, unsigned long len)
{
	unsigned long length = len;
	unsigned long totallen = length;
	
	if(socket_client_value != NULL && socket_client_value->connected_state == 1)//连接中
	{
		while(1)
		{
			 length = totallen;
			 CH57xNET_SocketSend(socket_client_value->socket_id, buf, &length);         /* 将MyBuf中的数据发送 */
			 totallen -= length;                                                     /* 将总长度减去以及发送完毕的长度 */
			 buf += length;                                                            /* 将缓冲区指针偏移*/
			 if(totallen)continue;                                                /* 如果数据未发送完毕，则继续发送*/
			break;                                                               /* 发送完毕，退出 */
		}
	}
	else
	{
		debug_printf("client no connect %d\r\n",socket_client_value->socket_id);
		return 5;
	}
	
	return 0;
}


/**
* @brief   数据解析函数
* @param   sockeid
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void net_tcp_server_data(net_tcp_server_struct *net_tcp_server,unsigned char sockeid, unsigned char initstat)//pws
{
	unsigned long len;
	
		if(initstat & SINT_STAT_RECV && socket_client_t[sockeid].connected_state == 1)//接收到数据
    {
			socket_client_t[sockeid].connected_state = 1;
			socket_client_t[sockeid].socket_id = sockeid;
			len = CH57xNET_SocketRecvLen(sockeid, NULL);//读取数据个数
			if (len > 0)
			{
				CH57xNET_SocketRecv(sockeid, net_tcp_server->client_recv_buf, &len);//提取数据
				if(net_tcp_server->received_back != NULL)
				{
					net_tcp_server->received_back(&socket_client_t[sockeid], net_tcp_server->client_recv_buf, len);
				}
			}
    }
		//有客户端连接 && 端口号确实是服务器的
		if(initstat & SINT_STAT_CONNECT && SocketInf[sockeid].SourPort == net_tcp_server->port_local)/* TCP连接*/
		{
			CH57xNET_ModifyRecvBuf(sockeid,(UINT32)net_tcp_server->socket_recv_buf[sockeid], net_tcp_server_socket_recv_buf_len);
			
			socket_client_t[sockeid].connected_state = 1;
			socket_client_t[sockeid].socket_id = sockeid;
			
			if(net_tcp_server->connected_back != NULL)
			{
				net_tcp_server->connected_back(&socket_client_t[sockeid]);
			}
		}
		
		if(initstat & SINT_STAT_DISCONNECT && socket_client_t[sockeid].connected_state == 1)/* TCP断开 */
		{/* 产生此中断，CH579库内部会将此socket清除，置为关闭,需要重新创建*/
			socket_client_t[sockeid].connected_state = 0;
			socket_client_t[sockeid].socket_id = sockeid;
			if(net_tcp_server->disconnected_back != NULL)
			{
				net_tcp_server->disconnected_back(&socket_client_t[sockeid]);
			}
		}
		if(initstat & SINT_STAT_TIM_OUT)/* TCP超时中断 */
		{/* 产生此中断，CH579库内部会将此socket清除，置为关闭,需要重新创建*/
			socket_client_t[sockeid].connected_state = 0;
			socket_client_t[sockeid].socket_id = sockeid;
			
			if(net_tcp_server->disconnected_back != NULL)
			{
				net_tcp_server->disconnected_back(&socket_client_t[sockeid]);
			}
		}
}















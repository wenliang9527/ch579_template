#ifndef net_tcp_server_h_
#define net_tcp_server_h_

#ifndef net_tcp_server_c_
#define net_tcp_server_cx_ extern
#else
#define net_tcp_server_cx_
#endif

#include "CH57x_common.h"
#include "CH57xNET.H"
#include "net_init.h"
#include <stdio.h>

#define net_tcp_server_socket_max_len CH57xNET_MAX_SOCKET_NUM //最大支持的客户端连接个数
#define net_tcp_server_socket_recv_buf_len 536


typedef struct socket_client{
	unsigned char connected_state;
	unsigned char socket_id;
}socket_client;

net_tcp_server_cx_ socket_client socket_client_t[net_tcp_server_socket_max_len];


typedef void(*net_tcp_server_client_connect_back)(socket_client * socket_client_value);
typedef void(*net_tcp_server_recv_back)(socket_client * socket_client_value, unsigned char* data, unsigned long length);


typedef struct net_tcp_server_struct
{
	
	unsigned int port_local;//本地端口号
	unsigned char keeplive_enable;//是否开启心跳功能
	
	net_tcp_server_client_connect_back connected_back;//客户端连接成功回调函数
	net_tcp_server_client_connect_back disconnected_back;//客户端连接断开回调函数
	
	
	net_tcp_server_recv_back received_back;//接收数据回调函数
	
	unsigned char socke_id;//记录sockeid
	
	unsigned char socket_recv_buf[net_tcp_server_socket_max_len][net_tcp_server_socket_recv_buf_len];
	
	unsigned char client_recv_buf[net_tcp_server_socket_recv_buf_len];
}net_tcp_server_struct;



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
	net_tcp_server_recv_back received_back);//接收数据回调函数


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
void net_tcp_server_data(net_tcp_server_struct *net_tcp_server,unsigned char sockeid, unsigned char initstat);

/**
* @brief   发送数据
* @param   socketid
* @param   buf 数据地址
* @param   len 数据长度
* @retval  0:成功; others:失败
* @warning None
* @example 
**/
char net_tcp_server_send(socket_client * socket_client_value, unsigned char *buf, unsigned long len);

/**
* @brief   关闭socket
* @param   socketid
* @retval  0:成功; others:失败
* @warning None
* @example 
**/
char net_tcp_server_close(net_tcp_server_struct *net_tcp_server);

#endif


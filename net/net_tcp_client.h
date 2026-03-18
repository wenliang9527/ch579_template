#ifndef net_tcp_client_h_
#define net_tcp_client_h_

#ifndef net_tcp_client_c_
#define net_tcp_client_cx_ extern
#else
#define net_tcp_client_cx_
#endif

#include "CH57x_common.h"
#include "CH57xNET.H"
#include <stdio.h>

typedef void(*net_tcp_client_back)(unsigned char socke_id, int socke_index);
typedef void(*net_tcp_client_recv_back)(unsigned char sockeid, int socke_index, unsigned char* data, unsigned long length);

#define net_tcp_client_socket_recv_buf_len 536
typedef struct net_tcp_client_struct{
	unsigned char  ip_addr_remote[4];//目的地址
	unsigned int port_remote;//目的端口号
	unsigned int port_local;//本地端口号
	unsigned char keeplive_enable;//是否开启心跳功能
	
	net_tcp_client_back connected_back;//连接成功回调函数
	net_tcp_client_back disconnected_back;//连接断开回调函数
	net_tcp_client_recv_back received_back;//接收数据回调函数
	unsigned char socke_id;//记录sockeid
	unsigned char socke_index;
	unsigned char connected_state;//连接状态 0:未连接 1:连接中  2:连接上
	unsigned char socket_recv_buf[net_tcp_client_socket_recv_buf_len];
	unsigned char client_recv_buf[net_tcp_client_socket_recv_buf_len];
}net_tcp_client_struct;




char net_tcp_client_creat_socket(net_tcp_client_struct *net_tcp_client, 
	net_tcp_client_back connected_back, //连接成功回调函数
	net_tcp_client_back disconnected_back ,//连接断开回调函数
	net_tcp_client_recv_back received_back);//接收数据回调函数


void net_tcp_client_connect(net_tcp_client_struct *net_tcp_client);

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
void net_tcp_client_data(net_tcp_client_struct *net_tcp_client,unsigned char sockeid, unsigned char initstat);

/**
* @brief   发送数据
* @param   socketid
* @param   buf 数据地址
* @param   len 数据长度
* @retval  0:成功; others:失败
* @warning None
* @example 
**/
char net_tcp_client_send(net_tcp_client_struct *net_tcp_client, unsigned char *buf, unsigned long len);

/**
* @brief   关闭socket
* @param   socketid
* @param   buf 数据地址
* @param   len 数据长度
* @retval  0:成功; others:失败
* @warning None
* @example 
**/
char net_tcp_client_close(net_tcp_client_struct *net_tcp_client);

/**
* @brief   重新连接TCP
* @param   socketid
* @param   buf 数据地址
* @param   len 数据长度
* @retval  0:成功; others:失败
* @warning None
* @example 
**/
char net_tcp_client_reconnect(net_tcp_client_struct *net_tcp_client);
#endif


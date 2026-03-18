#ifndef __UART_H
#define __UART_H

#ifndef UART_C_//如果没有定义
#define UART_Ex_ extern
#else
#define UART_Ex_
#endif


#include <stdbool.h>
#include "CH57x_common.h"
#include "LoopList.h"
#include "net_init.h"
#include "net_dhcp.h"
#include "net_dns.h"
#include "net_tcp_client.h"

#include "net_tcp_server.h"
#define MAXFRAMELEN    1000    
#define MAXSENTLISTLEN 2    //10  /100
#define SLAVEtimeout    0
typedef enum
{
	USART_0_IDNumber=0,
	USART_1_IDNumber,
	USART_2_IDNumber,
	USART_3_IDNumber,
	Internet_IDNumber
}USARTNumberID;

typedef __packed struct
{
  UINT16 FramLen;
	UINT8 FrameBuf[MAXFRAMELEN];
	UINT16 Point;
  UINT8 UserID;
	UINT8 TStatus;
}TCommDataPacket;

typedef __packed struct
{
  TCommDataPacket Field[MAXSENTLISTLEN];
	UINT16 R_Point;
  UINT16 W_Point;
  UINT16 buflen;
	UINT16 TOUTtime;
}TCommSentList;

typedef struct 
{
	USARTNumberID SLOMIDUP;
	USARTNumberID SLOMIDDOWN;
	UINT8 InitializationCOM;
	UINT8 PCInitializationCOM;
	
	UINT16 Time100ms;
	UINT16 PCIntervaltime;
	UINT16 SlaveIntervaltime;
	
}Tcpbuf;

void uart_init(void);

extern Tcpbuf TcpMybuf;
//extern mydata Tmydata;

extern TCommDataPacket PCIssueBuf;
extern TCommDataPacket PCUploadBuf;
extern TCommDataPacket SlaveIssueBuf;
extern TCommDataPacket SlaveUploadBuf;
extern UINT8 client_ip[4];
extern net_tcp_server_struct net_tcp_server;//tcp_server变量

extern socket_client* socket1;//记录一个socket客户端
extern socket_client* socket2;//记录一个socket客户端
extern socket_client* socket3;//记录一个socket客户端
extern socket_client* socket4;//记录一个socket客户端
/**
* @brief   给指定的客户端发送数据
* @param   客户端的ip地址, 客户端的端口号, 发送的数据, 数据长度 
* @retval  None
* @warning None
* @example
**/
extern void tcp_send(unsigned char ip[4], int port, unsigned char* data, unsigned long length);
/**
* @brief  串口发送字符串数据
* @param  *c:发送的数据指针  cnt:数据个数
* @param  None
* @param  None
* @retval None
* @example 
**/
void uart_send_bytes(char USARTx, unsigned char *c,uint32_t cnt);
/**
* @brief  串口发送一个字节
* @param  *c:发送的数据指针  cnt:数据个数
* @param  None
* @param  None
* @retval None
* @example 
**/
void uart_send_byte(char USARTx, unsigned char c);

/**
* @brief  串口中断方式发送数据
* @param  *c:发送的数据指针  cnt:数据个数
* @param  None
* @param  None
* @retval None
* @example 
**/
extern void uart_send_byte_it(char USARTx, TCommDataPacket Buf);

extern void PCUpload_listADD(TCommDataPacket buf);
extern void PCUpload_listsend(void);
extern void SlaveIssue_listADD(TCommDataPacket buf);
extern void SlaveIssue_listsend(void);

#endif


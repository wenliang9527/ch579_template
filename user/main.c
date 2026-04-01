#define MAIN_C_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "CH57x_common.h"
#include "uart.h"
#include "time.h"
#include "sys_tick.h"
#include "data_forward.h"
#include "net_init.h"
#include "net_dhcp.h"
#include "net_dns.h"
#include "net_tcp_client.h"
#include "net_tcp_server.h"

/*==============================================================================
 * 全局变量定义
 *============================================================================*/

/* CH57xNET 数据缓冲区 */
__align(4) UINT8 RxBuffer[MAX_PACKET_SIZE];  /* 接收缓冲区 */
__align(4) UINT8 TxBuffer[MAX_PACKET_SIZE];  /* 发送缓冲区 */

/* 通用缓存 */
UINT8 main_buffer[main_buffer_len];  /* 通用数据缓冲区 */
uint32_t main_len;                   /* 通用长度变量 */
UINT8 *main_str;                     /* 通用指针变量 */

/* TCP服务器相关 */
net_tcp_server_struct net_tcp_server;           /* TCP服务器结构体 */
socket_client *socket1 = NULL;                  /* 客户端1 */
socket_client *socket2 = NULL;                  /* 客户端2 */
socket_client *socket3 = NULL;                  /* 客户端3 */
socket_client *socket4 = NULL;                  /* 客户端4 */

/* 其他变量定义（保留供扩展） */
Tcpbuf TcpMybuf;
UINT8 client_ip[4] = {160, 128, 1, 100};        /* 默认目标IP */

/*==============================================================================
 * 函数实现
 *============================================================================*/

/**
 * @brief   CH57xNET 全局中断处理函数
 * @param   None
 * @retval  None
 */
void CH57xNET_HandleGlobalInt(void)
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;

    initstat = CH57xNET_GetGlobalInt();  /* 获取网卡全局中断状态 */

    if (initstat & GINT_STAT_IP_CONFLI) {
        /* IP冲突中断处理 */
    }

    if (initstat & GINT_STAT_PHY_CHANGE) {
        /* PHY状态变化中断处理 */
        i = CH57xNET_GetPHYStatus();  /* 获取PHY状态 */
        (void)i;  /* 未使用 */
    }

    if (initstat & GINT_STAT_SOCKET) {
        /* Socket中断处理 */
        for (i = 0; i < CH57xNET_MAX_SOCKET_NUM; i++) {
            socketinit = CH57xNET_GetSocketInt(i);  /* 获取指定socket中断 */
            if (socketinit) {
                net_tcp_server_data(&net_tcp_server, i, socketinit);
            }
        }
    }
}

/**
 * @brief   TCP服务器接收数据回调函数
 * @param   socket_client_value: 客户端句柄
 * @param   data: 数据指针
 * @param   length: 数据长度
 * @retval  None
 * @note    当接收到数据后，转发到串口，自动转发到B口
 */
void tcp_server_recv_back(socket_client *socket_client_value, unsigned char *data, unsigned long length)
{
    (void)socket_client_value;  /* 未使用参数 */
    DataForward_RecvFromEth(data, (uint16_t)length);
}

/**
 * @brief   TCP客户端连接回调函数
 * @param   socket_client_value: 客户端句柄
 * @retval  None
 */
void tcp_server_connected_back(socket_client *socket_client_value)
{
    if (socket1 == NULL) {
        socket1 = socket_client_value;
    } else if (socket2 == NULL) {
        socket2 = socket_client_value;
    } else if (socket3 == NULL) {
        socket3 = socket_client_value;
    } else if (socket4 == NULL) {
        socket4 = socket_client_value;
    }
}

/**
 * @brief   TCP客户端断开回调函数
 * @param   socket_client_value: 客户端句柄
 * @retval  None
 */
void tcp_server_disconnected_back(socket_client *socket_client_value)
{
    if (socket1 != NULL && socket_client_value->socket_id == socket1->socket_id) {
        socket1 = NULL;
    }
    if (socket2 != NULL && socket_client_value->socket_id == socket2->socket_id) {
        socket2 = NULL;
    }
    if (socket3 != NULL && socket_client_value->socket_id == socket3->socket_id) {
        socket3 = NULL;
    }
    if (socket4 != NULL && socket_client_value->socket_id == socket4->socket_id) {
        socket4 = NULL;
    }
}

/**
 * @brief   主函数入口
 * @param   None
 * @retval  None
 */
int main(void)
{
    /* 系统时钟初始化 */
    PWR_UnitModCfg(ENABLE, UNIT_SYS_PLL);  /* 使能PLL */
    DelayMs(3);
    SetSysClock(CLK_SOURCE_HSE_32MHz);      /* 设置32MHz时钟 */

    /* 外设初始化 */
    uart_init();        /* 串口初始化 */
    net_init();         /* 以太网初始化 */
    time0_init();       /* 定时器初始化 */
    DataForward_Init(); /* 数据转发模块初始化 */

    /* TCP服务器初始化 */
    net_tcp_server.port_local = 6666;   /* 监听端口 */
    net_tcp_server.socke_id = 255;      /* 初始化socket ID */
    net_tcp_server_close(&net_tcp_server);
    net_tcp_server_creat_socket(&net_tcp_server, 
                                tcp_server_connected_back, 
                                tcp_server_disconnected_back, 
                                tcp_server_recv_back);

    /* 主循环 */
    while (1) {
        /* CH57xNET库任务处理 */
        CH57xNET_MainTask();

        /* 查询并处理中断事件 */
        if (CH57xNET_QueryGlobalInt()) {
            CH57xNET_HandleGlobalInt();
        }

        /* 数据转发处理（20ms周期运行） */
        DataForward_Process();
    }
}

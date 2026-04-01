#ifndef __DATA_FORWARD_H_
#define __DATA_FORWARD_H_

#ifndef DATA_FORWARD_C_
#define DATA_FORWARD_Ex_ extern
#else
#define DATA_FORWARD_Ex_
#endif

#include <stdint.h>
#include <stdbool.h>
#include "CH57x_common.h"

/* 帧缓冲区大小 - 根据CH579 32KB RAM优化 */
#define FRAME_BUF_SIZE      1024    /* 单帧最大长度 */
#define FRAME_QUEUE_SIZE    3       /* 帧队列深度 - 较小以平衡内存占用 */

/* 发送间隔 20ms */
#define FRAME_SEND_INTERVAL 20

/* 端口类型定义 */
typedef enum {
    PORT_UART0 = 0,     /* UART0 */
    PORT_UART1,         /* UART1 */
    PORT_UART2,         /* UART2 */
    PORT_UART3,         /* UART3 */
    PORT_ETH,           /* 以太网 */
    PORT_NONE           /* 无 */
} PortType_t;

/* 数据帧结构 - 支持多目标转发 */
typedef struct {
    uint8_t  data[FRAME_BUF_SIZE];  /* 帧数据 */
    uint16_t len;                   /* 帧长度 */
    PortType_t srcPort;             /* 源端口 */
    uint8_t  dstMask;               /* 目标端口位掩码 (bit0=UART0, bit1=UART1, bit2=UART2, bit3=UART3, bit4=ETH) */
    uint32_t recvTime;              /* 接收时间戳 */
} DataFrame_t;

/* 帧队列结构 */
typedef struct {
    DataFrame_t frames[FRAME_QUEUE_SIZE];   /* 帧数组 */
    uint8_t writeIdx;                       /* 写索引 */
    uint8_t readIdx;                        /* 读索引 */
    uint8_t count;                          /* 当前帧数 */
} FrameQueue_t;

/* 接收状态结构 */
typedef struct {
    uint8_t  buf[FRAME_BUF_SIZE];   /* 接收缓冲区 */
    uint16_t idx;                   /* 当前写入位置 */
    PortType_t srcPort;             /* 源端口 */
    uint8_t  dstMask;               /* 目标端口位掩码 */
    bool     frameReady;            /* 帧准备就绪标志 */
    uint32_t lastRecvTime;          /* 最后接收时间 */
} RecvState_t;

/* 端口位掩码定义 */
#define PORT_MASK_UART0     (1 << 0)
#define PORT_MASK_UART1     (1 << 1)
#define PORT_MASK_UART2     (1 << 2)
#define PORT_MASK_UART3     (1 << 3)
#define PORT_MASK_ETH       (1 << 4)

/* 组定义 */
#define GROUP_A_MASK        (PORT_MASK_UART0 | PORT_MASK_UART1 | PORT_MASK_ETH)  /* 组A: UART0 + UART1 + ETH */
#define GROUP_B_MASK        (PORT_MASK_UART2 | PORT_MASK_UART3)                  /* 组B: UART2 + UART3 */

/**
 * @brief  根据源端口获取组掩码
 * @param  srcPort: 源端口
 * @return 对应组所有端口的掩码
 */
uint8_t DataForward_GetGroupMask(PortType_t srcPort);

/* 全局变量声明 */
DATA_FORWARD_Ex_ FrameQueue_t g_frameQueue;     /* 帧队列 */
DATA_FORWARD_Ex_ RecvState_t  g_recvState[4];   /* UART端口接收状态 (0-3:UART) */
DATA_FORWARD_Ex_ uint32_t     g_lastSendTime;   /* 上次发送时间 */
DATA_FORWARD_Ex_ bool        g_sendPending;     /* 发送挂起标志 */

/* 活动端口标记 - 动态路由 */
DATA_FORWARD_Ex_ PortType_t   g_groupA_ActivePort;  /* A组活动端口 (默认UART1) */
DATA_FORWARD_Ex_ PortType_t   g_groupB_ActivePort;  /* B组活动端口 (默认UART3) */

/* 组端口定义 */
#define GROUP_A_PORTS       (PORT_MASK_UART0 | PORT_MASK_UART1 | PORT_MASK_ETH)
#define GROUP_B_PORTS       (PORT_MASK_UART2 | PORT_MASK_UART3)

/* 函数声明 */

/**
 * @brief  数据转发模块初始化
 */
void DataForward_Init(void);

/**
 * @brief  串口接收到字节存入缓存
 * @param  port: 接收端口
 * @param  data: 接收到的字节
 */
void DataForward_RecvByte(PortType_t port, uint8_t data);

/**
 * @brief  帧接收完成（在UART_II_RECV_TOUT时调用）
 * @param  port: 接收端口
 */
void DataForward_FrameComplete(PortType_t port);

/**
 * @brief  网口接收到数据存入队列
 * @param  data: 数据指针
 * @param  len: 数据长度
 */
void DataForward_RecvFromEth(uint8_t *data, uint16_t len);

/**
 * @brief  在循环中调用，处理数据转发
 */
void DataForward_Process(void);

/**
 * @brief  发送一帧数据到指定端口
 * @param  frame: 帧指针
 * @return 0:成功, 负数:失败
 */
int DataForward_SendFrame(DataFrame_t *frame);

/**
 * @brief  检查是否可以发送（20ms周期控制）
 * @return true:可以发送, false:需要等待
 */
bool DataForward_CanSend(void);

/**
 * @brief  获取当前时间（毫秒）
 * @return 当前时间戳
 */
uint32_t DataForward_GetTick(void);

/**
 * @brief  重置指定端口的接收状态
 * @param  port: 端口
 */
void DataForward_ResetRecvState(PortType_t port);

/**
 * @brief  设置默认的数据路由
 * @param  src: 源端口
 * @param  dst: 目标端口
 */
void DataForward_SetDefaultRoute(PortType_t src, PortType_t dst);

/**
 * @brief  判断端口属于哪个组
 * @param  port: 端口
 * @return 0:不属于任何组, 1:A组, 2:B组
 */
uint8_t DataForward_GetPortGroup(PortType_t port);

/**
 * @brief  更新活动端口标记
 * @param  port: 接收到数据的端口
 */
void DataForward_UpdateActivePort(PortType_t port);

/**
 * @brief  获取对方组的活跃端口
 * @param  srcPort: 源端口
 * @return 对方组的活跃端口
 */
PortType_t DataForward_GetPeerActivePort(PortType_t srcPort);

/**
 * @brief  获取对方组的端口掩码
 * @param  srcPort: 源端口
 * @return 对方组的端口掩码
 * @note   A组第一次发送时广播到B组所有端口，之后单播到B组活动端口
 *          B组第一次发送时广播到A组所有端口，之后单播到A组活动端口
 */
uint8_t DataForward_GetPeerGroupMask(PortType_t srcPort);

#endif /* __DATA_FORWARD_H_ */

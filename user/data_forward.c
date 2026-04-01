#define DATA_FORWARD_C_

#include <string.h>
#include "data_forward.h"
#include "uart.h"
#include "net_tcp_server.h"

/* 全局变量定义 */
FrameQueue_t g_frameQueue;
RecvState_t  g_recvState[4];    /* 只给4个UART端口分配接收缓存 */
uint32_t     g_lastSendTime = 0;
bool         g_sendPending = false;

/* 活动端口标记 - 动态路由 */
PortType_t   g_groupA_ActivePort = PORT_UART1;  /* A组活动端口 (默认UART1) */
PortType_t   g_groupB_ActivePort = PORT_UART3;  /* B组活动端口 (默认UART3) */

/* 首次通讯标志 */
static bool g_firstCommAtoB = true;   /* A组首次发送到B组 */
static bool g_firstCommBtoA = true;   /* B组首次发送到A组 */

/**
 * @brief  判断端口属于哪个组
 * @param  port: 端口
 * @return 0:不属于任何组, 1:A组, 2:B组
 */
uint8_t DataForward_GetPortGroup(PortType_t port)
{
    switch (port) {
        case PORT_UART0:
        case PORT_UART1:
        case PORT_ETH:
            return 1;  /* A组 */
        case PORT_UART2:
        case PORT_UART3:
            return 2;  /* B组 */
        default:
            return 0;  /* 不属于任何组 */
    }
}

/**
 * @brief  更新活动端口标记
 * @param  port: 接收到数据的端口
 */
void DataForward_UpdateActivePort(PortType_t port)
{
    uint8_t group = DataForward_GetPortGroup(port);
    
    if (group == 1) {
        g_groupA_ActivePort = port;
    } else if (group == 2) {
        g_groupB_ActivePort = port;
    }
}

/**
 * @brief  获取对方组的活跃端口
 * @param  srcPort: 源端口
 * @return 对方组的活跃端口
 */
PortType_t DataForward_GetPeerActivePort(PortType_t srcPort)
{
    uint8_t group = DataForward_GetPortGroup(srcPort);
    
    if (group == 1) {
        /* A组 -> 返回B组活跃端口 */
        return g_groupB_ActivePort;
    } else if (group == 2) {
        /* B组 -> 返回A组活跃端口 */
        return g_groupA_ActivePort;
    }
    
    return PORT_NONE;
}

/**
 * @brief  端口转换为掩码
 * @param  port: 端口
 * @return 端口掩码
 */
static uint8_t PortToMask(PortType_t port)
{
    switch (port) {
        case PORT_UART0: return PORT_MASK_UART0;
        case PORT_UART1: return PORT_MASK_UART1;
        case PORT_UART2: return PORT_MASK_UART2;
        case PORT_UART3: return PORT_MASK_UART3;
        case PORT_ETH:   return PORT_MASK_ETH;
        default:         return 0;
    }
}

/**
 * @brief  获取对方组的端口掩码
 * @param  srcPort: 源端口
 * @return 对方组的端口掩码
 * @note   A组第一次发送时广播到B组所有端口，之后单播到B组活动端口
 *          B组第一次发送时广播到A组所有端口，之后单播到A组活动端口
 */
uint8_t DataForward_GetPeerGroupMask(PortType_t srcPort)
{
    uint8_t group = DataForward_GetPortGroup(srcPort);
    
    if (group == 1) {
        /* A组(上位机) -> B组(下位机) */
        if (g_firstCommAtoB) {
            g_firstCommAtoB = false;  /* 清除首次标志 */
            return GROUP_B_PORTS;     /* 首次广播到B组所有端口 */
        }
        /* 后续单播到B组活动端口 */
        return PortToMask(g_groupB_ActivePort);
    } else if (group == 2) {
        /* B组(下位机) -> A组(上位机) */
        if (g_firstCommBtoA) {
            g_firstCommBtoA = false;  /* 清除首次标志 */
            return GROUP_A_PORTS;     /* 首次广播到A组所有端口 */
        }
        /* 后续单播到A组活动端口 */
        return PortToMask(g_groupA_ActivePort);
    }
    
    return 0;
}

/**
 * @brief  数据转发模块初始化
 */
void DataForward_Init(void)
{
    uint8_t i;
    
    /* 清空帧队列 */
    memset(&g_frameQueue, 0, sizeof(FrameQueue_t));
    g_frameQueue.writeIdx = 0;
    g_frameQueue.readIdx = 0;
    g_frameQueue.count = 0;
    
    /* 清空各端口接收状态 */
    for (i = 0; i < 4; i++) {
        memset(&g_recvState[i], 0, sizeof(RecvState_t));
        g_recvState[i].srcPort = (PortType_t)i;
        g_recvState[i].dstMask = 0;
    }
    
    /* 初始化活动端口 */
    g_groupA_ActivePort = PORT_UART1;  /* A组默认UART1 */
    g_groupB_ActivePort = PORT_UART3;  /* B组默认UART3 */
    
    /* 初始化首次通讯标志 */
    g_firstCommAtoB = true;
    g_firstCommBtoA = true;
    
    g_lastSendTime = 0;
    g_sendPending = false;
}

/**
 * @brief  获取当前时间（毫秒）- 使用系统tick
 * @return 当前时间戳
 */
uint32_t DataForward_GetTick(void)
{
    extern int timer0_value;
    return (uint32_t)timer0_value;
}

/**
 * @brief  串口接收到字节存入缓存
 * @param  port: 接收端口
 * @param  data: 接收到的字节
 */
void DataForward_RecvByte(PortType_t port, uint8_t data)
{
    RecvState_t *state;
    
    if (port > PORT_UART3) return;  /* 只处理UART端口 */
    
    state = &g_recvState[port];
    
    /* 防止缓冲区溢出 */
    if (state->idx >= FRAME_BUF_SIZE) {
        return;
    }
    
    /* 存储字节 */
    state->buf[state->idx++] = data;
    state->srcPort = port;
}

/**
 * @brief  将帧推入发送队列
 * @param  state: 接收状态指针
 * @param  dstMask: 目标端口掩码
 * @return true:成功, false:队列满
 */
static bool DataForward_EnqueueFrame(RecvState_t *state, uint8_t dstMask)
{
    DataFrame_t *frame;
    
    if (state->idx == 0) return true;  /* 空帧直接返回 */
    
    /* 检查队列是否已满 */
    if (g_frameQueue.count >= FRAME_QUEUE_SIZE) {
        return false;  /* 队列满 */
    }
    
    /* 获取写位置 */
    frame = &g_frameQueue.frames[g_frameQueue.writeIdx];
    
    /* 复制数据 */
    memcpy(frame->data, state->buf, state->idx);
    frame->len = state->idx;
    frame->srcPort = state->srcPort;
    frame->dstMask = dstMask;  /* 设置目标端口掩码 */
    frame->recvTime = DataForward_GetTick();
    
    /* 更新队列计数 */
    g_frameQueue.writeIdx = (g_frameQueue.writeIdx + 1) % FRAME_QUEUE_SIZE;
    g_frameQueue.count++;
    
    /* 清空接收状态 */
    state->idx = 0;
    state->frameReady = false;
    
    return true;
}

/**
 * @brief  帧接收完成（在UART_II_RECV_TOUT时调用）
 * @param  port: 接收端口
 */
void DataForward_FrameComplete(PortType_t port)
{
    RecvState_t *state;
    uint8_t peerMask;
    
    if (port > PORT_UART3) return;  /* 只处理UART端口 */
    
    /* 更新活动端口标记 */
    DataForward_UpdateActivePort(port);
    
    state = &g_recvState[port];
    
    /* 获取对方组的端口掩码 */
    peerMask = DataForward_GetPeerGroupMask(port);
    
    /* 将帧推入队列，如果队列满则丢弃 */
    if (!DataForward_EnqueueFrame(state, peerMask)) {
        /* 队列满，丢弃当前帧 */
        state->idx = 0;
        state->frameReady = false;
    }
}

/**
 * @brief  网口接收到数据存入队列
 * @param  data: 数据指针
 * @param  len: 数据长度
 */
void DataForward_RecvFromEth(uint8_t *data, uint16_t len)
{
    DataFrame_t *frame;
    uint8_t peerMask;
    
    if (len == 0 || len > FRAME_BUF_SIZE) return;
    
    /* 检查队列是否已满 */
    if (g_frameQueue.count >= FRAME_QUEUE_SIZE) {
        return;  /* 队列满，直接丢弃 */
    }
    
    /* 更新活动端口标记 */
    DataForward_UpdateActivePort(PORT_ETH);
    
    /* 获取对方组的端口掩码 */
    peerMask = DataForward_GetPeerGroupMask(PORT_ETH);
    
    /* 获取写位置 */
    frame = &g_frameQueue.frames[g_frameQueue.writeIdx];
    
    /* 复制数据 */
    memcpy(frame->data, data, len);
    frame->len = len;
    frame->srcPort = PORT_ETH;
    frame->dstMask = peerMask;  /* 设置目标端口掩码 */
    frame->recvTime = DataForward_GetTick();
    
    /* 更新队列计数 */
    g_frameQueue.writeIdx = (g_frameQueue.writeIdx + 1) % FRAME_QUEUE_SIZE;
    g_frameQueue.count++;
}

/**
 * @brief  检查是否可以发送（20ms周期控制）
 * @return true:可以发送, false:需要等待
 */
bool DataForward_CanSend(void)
{
    uint32_t currentTick = DataForward_GetTick();
    
    if ((currentTick - g_lastSendTime) >= FRAME_SEND_INTERVAL) {
        return true;
    }
    return false;
}

/**
 * @brief  发送一帧数据到指定端口
 * @param  frame: 帧指针
 * @param  port: 目标端口
 * @return 0:成功, 负数:失败
 */
static int DataForward_SendFrameToPort(DataFrame_t *frame, PortType_t port)
{
    uint16_t i;
    
    if (frame == NULL || frame->len == 0) return -1;
    
    /* 不发送给源端口 */
    if (port == frame->srcPort) return 0;
    
    switch (port) {
        case PORT_UART0:
        case PORT_UART1:
        case PORT_UART2:
        case PORT_UART3:
            /* 发送到串口 */
            for (i = 0; i < frame->len; i++) {
                uart_send_byte((char)port, frame->data[i]);
            }
            break;
            
        case PORT_ETH:
            /* 发送到网口 */
            if (socket1 != NULL && socket1->connected_state == 1) {
                net_tcp_server_send(socket1, frame->data, frame->len);
            } else if (socket2 != NULL && socket2->connected_state == 1) {
                net_tcp_server_send(socket2, frame->data, frame->len);
            } else if (socket3 != NULL && socket3->connected_state == 1) {
                net_tcp_server_send(socket3, frame->data, frame->len);
            } else if (socket4 != NULL && socket4->connected_state == 1) {
                net_tcp_server_send(socket4, frame->data, frame->len);
            }
            break;
            
        default:
            return -2;  /* 无效目标端口 */
    }
    
    return 0;
}

/**
 * @brief  发送一帧数据到掩码指定的所有端口
 * @param  frame: 帧指针
 * @return 0:成功, 负数:失败
 */
int DataForward_SendFrame(DataFrame_t *frame)
{
    uint8_t dstMask = frame->dstMask;
    
    /* 发送到所有目标端口 */
    if (dstMask & PORT_MASK_UART0) {
        DataForward_SendFrameToPort(frame, PORT_UART0);
    }
    if (dstMask & PORT_MASK_UART1) {
        DataForward_SendFrameToPort(frame, PORT_UART1);
    }
    if (dstMask & PORT_MASK_UART2) {
        DataForward_SendFrameToPort(frame, PORT_UART2);
    }
    if (dstMask & PORT_MASK_UART3) {
        DataForward_SendFrameToPort(frame, PORT_UART3);
    }
    if (dstMask & PORT_MASK_ETH) {
        DataForward_SendFrameToPort(frame, PORT_ETH);
    }
    
    return 0;
}

/**
 * @brief  从队列取出一帧
 * @param  frame: 输出帧指针
 * @return true:成功, false:队列空
 */
static bool DataForward_DequeueFrame(DataFrame_t *frame)
{
    if (g_frameQueue.count == 0) {
        return false;  /* 队列空 */
    }
    
    /* 复制帧数据 */
    memcpy(frame, &g_frameQueue.frames[g_frameQueue.readIdx], sizeof(DataFrame_t));
    
    /* 更新队列计数 */
    g_frameQueue.readIdx = (g_frameQueue.readIdx + 1) % FRAME_QUEUE_SIZE;
    g_frameQueue.count--;
    
    return true;
}

/**
 * @brief  在循环中调用，处理数据转发
 */
void DataForward_Process(void)
{
    DataFrame_t frame;
    
    /* 检查是否有数据需要处理 */
    if (g_frameQueue.count == 0) {
        return;
    }
    
    /* 检查20ms周期 */
    if (!DataForward_CanSend()) {
        return;
    }
    
    /* 取出一帧并发送 */
    if (DataForward_DequeueFrame(&frame)) {
        DataForward_SendFrame(&frame);
        g_lastSendTime = DataForward_GetTick();
    }
}

/**
 * @brief  重置指定端口的接收状态
 * @param  port: 端口
 */
void DataForward_ResetRecvState(PortType_t port)
{
    if (port > PORT_UART3) return;  /* 只处理UART端口 */
    
    g_recvState[port].idx = 0;
    g_recvState[port].frameReady = false;
}

/**
 * @brief  设置默认的数据路由（保留供未来扩展使用）
 * @param  src: 源端口
 * @param  dst: 目标端口
 */
void DataForward_SetDefaultRoute(PortType_t src, PortType_t dst)
{
    /* 动态路由不需要设置默认路由 */
    (void)src;
    (void)dst;
}

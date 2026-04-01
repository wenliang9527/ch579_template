#define UART_C_

#include <stdio.h>
#include "uart.h"
#include "data_forward.h"

/**********上位机通讯数据接收发送***********/
//TCommDataPacket PCIssueBuf;
TCommDataPacket PCUploadBuf;
///**********下位机通讯数据接收发送***********/
//TCommDataPacket SlaveIssueBuf;
//TCommDataPacket SlaveUploadBuf;
///**********发送缓冲队列***********/
//TCommSentList PCsendlist;
//TCommSentList Slavesendlist;


void uart_init(void)
{

	GPIOPinRemap(ENABLE,RB_PIN_UART0);
	GPIOA_SetBits(GPIO_Pin_14);//TX设置高电平
	GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_PU);     // RXD-上拉输入模式
	GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);// TXD-推挽输出模式，注意某些IO需要高电平
	UART0_DefInit();
	/*使能接收中断,线路状态中断*/
	UART0_INTCfg( ENABLE, RB_IER_RECV_RDY|RB_IER_LINE_STAT );
	NVIC_SetPriority(UART0_IRQn,5);//设置中断优先级(也可以不设置,默认14)
	NVIC_EnableIRQ( UART0_IRQn );

	GPIOA_SetBits(GPIO_Pin_9);
	GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);     // RXD-上拉输入模式
	GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);// TXD-推挽输出模式，注意某些IO需要高电平
	UART1_DefInit();
	/*使能接收中断,线路状态中断*/
	UART1_INTCfg( ENABLE, RB_IER_RECV_RDY|RB_IER_LINE_STAT );
	NVIC_SetPriority(UART1_IRQn,3);//设置中断优先级(也可以不设置,默认14)
	NVIC_EnableIRQ( UART1_IRQn );

	GPIOA_SetBits(GPIO_Pin_7);
	GPIOA_ModeCfg(GPIO_Pin_6, GPIO_ModeIN_PU);     // RXD-上拉输入模式
	GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);// TXD-推挽输出模式，注意某些IO需要高电平
	UART2_DefInit();
	/*使能接收中断,线路状态中断*/
	UART2_INTCfg( ENABLE, RB_IER_RECV_RDY|RB_IER_LINE_STAT );
	NVIC_SetPriority(UART2_IRQn,2);//设置中断优先级(也可以不设置,默认14)
	NVIC_EnableIRQ( UART2_IRQn );

	GPIOPinRemap(ENABLE,RB_PIN_UART3);
	GPIOB_SetBits(GPIO_Pin_21);//TX设置高电平
	GPIOB_ModeCfg(GPIO_Pin_20, GPIO_ModeIN_PU);     // RXD-上拉输入模式
	GPIOB_ModeCfg(GPIO_Pin_21, GPIO_ModeOut_PP_5mA);// TXD-推挽输出模式，注意某些IO需要高电平
	UART3_DefInit();
	/*使能接收中断,线路状态中断*/
	UART3_INTCfg( ENABLE, RB_IER_RECV_RDY|RB_IER_LINE_STAT );
	NVIC_SetPriority(UART3_IRQn,2);//设置中断优先级(也可以不设置,默认14)
	NVIC_EnableIRQ( UART3_IRQn );
}



/**
* @brief  串口发送字节数据函数
* @param  *c:发送的数据指针  cnt:数据个数
* @param  None
* @param  None
* @retval None
* @example 
**/
void uart_send_bytes(char USARTx, unsigned char *c,uint32_t cnt)
{
	switch(USARTx)
	{
		case 0: UART0_SendString(c,cnt); break;
		case 1: UART1_SendString(c,cnt); break;
		case 2: UART2_SendString(c,cnt); break;
		case 3: UART3_SendString(c,cnt); break;
	}
}


/**
* @brief  串口发送一个字节
* @param  *c:发送的数据指针  cnt:数据个数
* @param  None
* @param  None
* @retval None
* @example 
**/
void uart_send_byte(char USARTx, unsigned char c)
{
	switch(USARTx)
	{
		case USART_0_IDNumber: UART0_SendByte(c); break;
		case USART_1_IDNumber: UART1_SendByte(c); break;
		case USART_2_IDNumber: UART2_SendByte(c); break;
		case USART_3_IDNumber: UART3_SendByte(c); break;
	}
}


/*串口中断 - UART0 */
void UART0_IRQHandler(void)
{
	UINT8 data;
    switch (UART0_GetITFlag())
    {
        case UART_II_LINE_STAT:          // 线路状态错误
            UART0_GetLinSTA();
            DataForward_ResetRecvState(PORT_UART0);  // 重置接收状态
            break;
            
        case UART_II_RECV_RDY:           // 数据达到接收阈值
            data = UART0_RecvByte();      // 读取接收数据
            /* 转发到数据处理 */
            DataForward_RecvByte(PORT_UART0, data);
            break;
            
        case UART_II_RECV_TOUT:          // 接收超时（一帧数据接收完成）
            data = UART0_RecvByte();      // 读取最后一个字节
            DataForward_RecvByte(PORT_UART0, data);
            /* 帧接收完成，推入发送队列，自动更新活动端口并转发到B组） */
            DataForward_FrameComplete(PORT_UART0);
            break;
            
        case UART_II_THR_EMPTY:          // 发送缓存区空
            UART0_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
            break;
            
        case UART_II_MODEM_CHG:          // 只支持串口 0
            break;
            
        default:
            break;
    }
}



/*串口中断 - UART1 */
void UART1_IRQHandler(void)
{
	UINT8 data;
    switch (UART1_GetITFlag())
    {
        case UART_II_LINE_STAT:          // 线路状态错误
            UART1_GetLinSTA();
            DataForward_ResetRecvState(PORT_UART1);  // 重置接收状态
            break;
            
        case UART_II_RECV_RDY:           // 数据达到接收阈值
            data = UART1_RecvByte();      // 读取接收数据
            /* 转发到数据处理 */
            DataForward_RecvByte(PORT_UART1, data);
            break;
            
        case UART_II_RECV_TOUT:          // 接收超时（一帧数据接收完成）
            data = UART1_RecvByte();      // 读取最后一个字节
            DataForward_RecvByte(PORT_UART1, data);
            /* 帧接收完成，推入发送队列，自动更新活动端口并转发到B组） */
            DataForward_FrameComplete(PORT_UART1);
            break;
            
        case UART_II_THR_EMPTY:          // 发送缓存区空
            UART1_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
            break;
            
        case UART_II_MODEM_CHG:          // 只支持串口 0
            break;
            
        default:
            break;
    }
}
/*串口中断 - UART2 */
void UART2_IRQHandler(void)
{
	UINT8 data;
    switch (UART2_GetITFlag())
    {
        case UART_II_LINE_STAT:          // 线路状态错误
            UART2_GetLinSTA();
            DataForward_ResetRecvState(PORT_UART2);  // 重置接收状态
            break;
            
        case UART_II_RECV_RDY:           // 数据达到接收阈值
            data = UART2_RecvByte();      // 读取接收数据
            /* 转发到数据处理 */
            DataForward_RecvByte(PORT_UART2, data);
            break;
            
        case UART_II_RECV_TOUT:          // 接收超时（一帧数据接收完成）
            data = UART2_RecvByte();      // 读取最后一个字节
            DataForward_RecvByte(PORT_UART2, data);
            /* 帧接收完成，推入发送队列，自动更新活动端口并转发到A组） */
            DataForward_FrameComplete(PORT_UART2);
            break;
            
        case UART_II_THR_EMPTY:          // 发送缓存区空
            UART2_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
            break;
            
        case UART_II_MODEM_CHG:          // 只支持串口 0
            break;
            
        default:
            break;
    }
}

/*串口中断 - UART3 */
void UART3_IRQHandler(void)
{
	UINT8 data;
    switch (UART3_GetITFlag())
    {
        case UART_II_LINE_STAT:          // 线路状态错误
            UART3_GetLinSTA();
            DataForward_ResetRecvState(PORT_UART3);  // 重置接收状态
            break;
            
        case UART_II_RECV_RDY:           // 数据达到接收阈值
            data = UART3_RecvByte();      // 读取接收数据
            /* 转发到数据处理 */
            DataForward_RecvByte(PORT_UART3, data);
            break;
            
        case UART_II_RECV_TOUT:          // 接收超时（一帧数据接收完成）
            data = UART3_RecvByte();      // 读取最后一个字节
            DataForward_RecvByte(PORT_UART3, data);
            /* 帧接收完成，推入发送队列，自动更新活动端口并转发到A组） */
            DataForward_FrameComplete(PORT_UART3);
            break;
            
        case UART_II_THR_EMPTY:          // 发送缓存区空
            UART3_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
            break;
            
        case UART_II_MODEM_CHG:          // 只支持串口 0
            break;
            
        default:
            break;
    }
}

/**
* @brief  使用microLib的函数,使用printf
* @warningg 需选 microLib
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/

int fputc(int ch, FILE *f)
{
	uint8_t data = ch;
	UART1_INTCfg( ENABLE,RB_IER_THR_EMPTY );//使能中断发送
  return ch;
}

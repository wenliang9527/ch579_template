#define UART_C_

#include <stdio.h>
#include "uart.h"

/**********上位机通讯接收与发送***********/
//TCommDataPacket PCIssueBuf;
TCommDataPacket PCUploadBuf;
///**********下位机通讯接收与发送***********/
//TCommDataPacket SlaveIssueBuf;
//TCommDataPacket SlaveUploadBuf;
///**********发送缓冲队列***********/
//TCommSentList PCsendlist;
//TCommSentList Slavesendlist;


void uart_init(void)
{

	GPIOPinRemap(ENABLE,RB_PIN_UART0);
	GPIOA_SetBits(GPIO_Pin_14);//TX输出高电平
	GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_PU);     // RXD-配置上拉输入
	GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);// TXD-配置推挽输出，注意先让IO口输出高电平
	UART0_DefInit();
	/*使能接收中断,接收错误中断*/
	UART0_INTCfg( ENABLE, RB_IER_RECV_RDY|RB_IER_LINE_STAT );
	NVIC_SetPriority(UART0_IRQn,5);//设置中断优先级(也可以不设置,默认14)
	NVIC_EnableIRQ( UART0_IRQn );

	GPIOA_SetBits(GPIO_Pin_9);
	GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);     // RXD-配置上拉输入
	GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);// TXD-配置推挽输出，注意先让IO口输出高电平
	UART1_DefInit();
	/*使能接收中断,接收错误中断*/
	UART1_INTCfg( ENABLE, RB_IER_RECV_RDY|RB_IER_LINE_STAT );
	NVIC_SetPriority(UART1_IRQn,3);//设置中断优先级(也可以不设置,默认14)
	NVIC_EnableIRQ( UART1_IRQn );

	GPIOA_SetBits(GPIO_Pin_7);
	GPIOA_ModeCfg(GPIO_Pin_6, GPIO_ModeIN_PU);     // RXD-配置上拉输入
	GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);// TXD-配置推挽输出，注意先让IO口输出高电平
	UART2_DefInit();
	/*使能接收中断,接收错误中断*/
	UART2_INTCfg( ENABLE, RB_IER_RECV_RDY|RB_IER_LINE_STAT );
	NVIC_SetPriority(UART2_IRQn,2);//设置中断优先级(也可以不设置,默认14)
	NVIC_EnableIRQ( UART2_IRQn );

	GPIOPinRemap(ENABLE,RB_PIN_UART3);
	GPIOB_SetBits(GPIO_Pin_21);//TX输出高电平
	GPIOB_ModeCfg(GPIO_Pin_20, GPIO_ModeIN_PU);     // RXD-配置上拉输入
	GPIOB_ModeCfg(GPIO_Pin_21, GPIO_ModeOut_PP_5mA);// TXD-配置推挽输出，注意先让IO口输出高电平
	UART3_DefInit();
	/*使能接收中断,接收错误中断*/
	UART3_INTCfg( ENABLE, RB_IER_RECV_RDY|RB_IER_LINE_STAT );
	NVIC_SetPriority(UART3_IRQn,2);//设置中断优先级(也可以不设置,默认14)
	NVIC_EnableIRQ( UART3_IRQn );
}



/**
* @brief  串口发送字符串数据
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


/**
* @brief  串口中断方式发送数据
* @param  *c:发送的数据指针  cnt:数据个数
* @param  None
* @param  None
* @retval None
* @example 
**/
void uart_send_byte_it(char USARTx, TCommDataPacket Buf)
{
	switch(USARTx)
	{
//		case USART_0_IDNumber: 
//			PCUploadBuf=Buf;
//		  PCUploadBuf.Point=0;
//		PCUploadBuf.TStatus=false;
//			UART0_INTCfg( ENABLE,RB_IER_THR_EMPTY );//使能中断发送
//			break;
//		case USART_1_IDNumber: 
//			PCUploadBuf=Buf;
//		  PCUploadBuf.Point=0;
//		PCUploadBuf.TStatus=false;
//			UART1_INTCfg( ENABLE,RB_IER_THR_EMPTY );//使能中断发送
//			break;
//		case USART_2_IDNumber: 
//			SlaveIssueBuf=Buf;
//		  SlaveIssueBuf.Point=0;
//		SlaveIssueBuf.TStatus=false;
//			UART2_INTCfg( ENABLE,RB_IER_THR_EMPTY );//使能中断发送
//			break;
//		case USART_3_IDNumber: 
//			SlaveIssueBuf=Buf;
//		  SlaveIssueBuf.Point=0;
//		SlaveIssueBuf.TStatus=false;
//			UART3_INTCfg( ENABLE,RB_IER_THR_EMPTY );//使能中断发送
//			break;
		 case Internet_IDNumber: 
			 tcp_send(client_ip, SocketInf[socket1->socket_id].DesPort, Buf.FrameBuf, Buf.FramLen);//给指定的客户端发送数据//网口数据发送
			break;
	}
}





/*串口中断*/
void UART0_IRQHandler(void)
{
	UINT8 data;
    switch (UART0_GetITFlag())
    {
        case UART_II_LINE_STAT:          // 线路状态错误
            UART0_GetLinSTA();
            break;
            
        case UART_II_RECV_RDY:           // 数据达到设置触发点
            /* 串口数据存储到缓存 */
				   data= UART0_RecvByte();  // 读取串口数据
           uart_send_byte(TcpMybuf.SLOMIDDOWN,data);
            TcpMybuf.SLOMIDUP = USART_0_IDNumber;
            TcpMybuf.PCInitializationCOM = 1;
            break;
            
        case UART_II_RECV_TOUT:          // 接收超时，暂时一帧数据接收完成
            data= UART0_RecvByte();  // 读取串口数据
             uart_send_byte(TcpMybuf.SLOMIDDOWN,data);
            TcpMybuf.SLOMIDUP = USART_0_IDNumber;
            TcpMybuf.PCInitializationCOM = 1;
            break;
            
        case UART_II_THR_EMPTY:          // 发送缓存区空，可继续发送
//            if (PCUploadBuf.FramLen)
//            {
//                if (PCUploadBuf.Point < PCUploadBuf.FramLen)
//                {
//                    UART0_SendByte(PCUploadBuf.FrameBuf[PCUploadBuf.Point]);
//                    PCUploadBuf.Point++;
//                }
//                else
//                {
//                    PCUploadBuf.Point = 0;
//                    PCUploadBuf.FramLen = 0;
//										PCUploadBuf.TStatus=true;
//									  TcpMybuf.PCIntervaltime=SLAVEtimeout;
//                    UART0_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
//                }
////            }
//            else
            {
                UART0_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
            }
            break;
            
        case UART_II_MODEM_CHG:          // 只支持串口 0
            break;
            
        default:
            break;
    }
}



/*串口中断*/
void UART1_IRQHandler(void)
{
		UINT8 data;
    switch (UART1_GetITFlag())
    {
        case UART_II_LINE_STAT:          // 线路状态错误
            UART1_GetLinSTA();
            break;
            
        case UART_II_RECV_RDY:           // 数据达到设置触发点
            /* 串口数据存储到缓存 */
            data= UART1_RecvByte();  // 读取串口数据
            uart_send_byte(TcpMybuf.SLOMIDDOWN,data);
            TcpMybuf.SLOMIDUP = USART_1_IDNumber;
            TcpMybuf.PCInitializationCOM = 1;
            break;
            
        case UART_II_RECV_TOUT:          // 接收超时，暂时一帧数据接收完成
            data= UART1_RecvByte();  // 读取串口数据
            uart_send_byte(TcpMybuf.SLOMIDDOWN,data);
            TcpMybuf.SLOMIDUP = USART_1_IDNumber;
            TcpMybuf.PCInitializationCOM = 1;
            break;
            
        case UART_II_THR_EMPTY:          // 发送缓存区空，可继续发送
//            if (PCUploadBuf.FramLen)
//            {
//                if (PCUploadBuf.Point < PCUploadBuf.FramLen)
//                {
//                    UART1_SendByte(PCUploadBuf.FrameBuf[PCUploadBuf.Point]);
//                    PCUploadBuf.Point++;
//                }
//                else
//                {
//                    PCUploadBuf.Point = 0;
//                    PCUploadBuf.FramLen = 0;
//									  PCUploadBuf.TStatus=true;
//									  TcpMybuf.PCIntervaltime=SLAVEtimeout;
//                    UART1_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
//                }
//            }
//            else
            {
                UART1_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
            }
            break;
            
        case UART_II_MODEM_CHG:          // 只支持串口 0
            break;
            
        default:
            break;
    }
}
/*串口中断*/
void UART2_IRQHandler(void)
{
	UINT8 data;
    switch (UART2_GetITFlag())
    {
        case UART_II_LINE_STAT:          // 线路状态错误
            UART2_GetLinSTA();
            break;
            
       case UART_II_RECV_RDY:           // 数据达到设置触发点
            /* 串口数据存储到缓存 */
             data= UART2_RecvByte();  // 读取串口数据
						PCUploadBuf.FrameBuf[PCUploadBuf.Point]=data;
						PCUploadBuf.Point++;
             uart_send_byte(TcpMybuf.SLOMIDUP,data);
            TcpMybuf.SLOMIDDOWN = USART_2_IDNumber;
            TcpMybuf.InitializationCOM = 1;
//						SlaveUploadBuf.TStatus =0;
            break;
            
        case UART_II_RECV_TOUT:          // 接收超时，暂时一帧数据接收完成
             data= UART2_RecvByte();  // 读取串口数据
						PCUploadBuf.FrameBuf[PCUploadBuf.Point]=data;
						PCUploadBuf.Point++;
						PCUploadBuf.FramLen = PCUploadBuf.Point;  // 接收数据长度
				if(TcpMybuf.SLOMIDUP==Internet_IDNumber)
					{
						tcp_send(client_ip, SocketInf[socket1->socket_id].DesPort, PCUploadBuf.FrameBuf, PCUploadBuf.FramLen);//给指定的客户端发送数据//网口数据发送
						PCUploadBuf.Point=0;
						PCUploadBuf.FramLen=0;
					}else
					{
						uart_send_byte(TcpMybuf.SLOMIDUP,data);
					}
            
            TcpMybuf.SLOMIDDOWN = USART_2_IDNumber;
            TcpMybuf.InitializationCOM = 1;
            break;
            
        case UART_II_THR_EMPTY:          // 发送缓存区空，可继续发送
//            if (SlaveIssueBuf.FramLen)
//            {
//                if (SlaveIssueBuf.Point < SlaveIssueBuf.FramLen)
//                {
//                    UART2_SendByte(SlaveIssueBuf.FrameBuf[SlaveIssueBuf.Point]);
//                    SlaveIssueBuf.Point++;
//                }
//                else
//                {
//                    SlaveIssueBuf.Point = 0;
//                    SlaveIssueBuf.FramLen = 0;
//									  SlaveIssueBuf.TStatus=true;
//									  TcpMybuf.SlaveIntervaltime=SLAVEtimeout;
//                    UART2_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
//                }
//            }
//            else
            {
                UART2_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
            }
            break;
            
        case UART_II_MODEM_CHG:          // 只支持串口 0
            break;
            
        default:
            break;
    }
}

/*串口中断*/
void UART3_IRQHandler(void)
{
	UINT8 data;
    switch (UART3_GetITFlag())
    {
        case UART_II_LINE_STAT:          // 线路状态错误
            UART3_GetLinSTA();
            break;
            
        case UART_II_RECV_RDY:           // 数据达到设置触发点
            /* 串口数据存储到缓存 */
             data= UART3_RecvByte();  // 读取串口数据
						PCUploadBuf.FrameBuf[PCUploadBuf.Point]=data;
						PCUploadBuf.Point++;
             uart_send_byte(TcpMybuf.SLOMIDUP,data);
            TcpMybuf.SLOMIDDOWN = USART_3_IDNumber;
            TcpMybuf.InitializationCOM = 1;
						//SlaveUploadBuf.TStatus =0;
            break;
            
        case UART_II_RECV_TOUT:          // 接收超时，暂时一帧数据接收完成
             data= UART3_RecvByte();  // 读取串口数据
					PCUploadBuf.FrameBuf[PCUploadBuf.Point]=data;
						PCUploadBuf.Point++;
						PCUploadBuf.FramLen = PCUploadBuf.Point;  // 接收数据长度
				if(TcpMybuf.SLOMIDUP==Internet_IDNumber)
					{
						tcp_send(client_ip, SocketInf[socket1->socket_id].DesPort, PCUploadBuf.FrameBuf, PCUploadBuf.FramLen);//给指定的客户端发送数据//网口数据发送
						PCUploadBuf.Point=0;
						PCUploadBuf.FramLen=0;
					}else
						{
							uart_send_byte(TcpMybuf.SLOMIDUP,data);
						}
            TcpMybuf.SLOMIDDOWN = USART_3_IDNumber;
            TcpMybuf.InitializationCOM = 1;
            break;
            
        case UART_II_THR_EMPTY:          // 发送缓存区空，可继续发送
//            if (SlaveIssueBuf.FramLen)
//            {
//                if (SlaveIssueBuf.Point < SlaveIssueBuf.FramLen)
//                {
//                    UART3_SendByte(SlaveIssueBuf.FrameBuf[SlaveIssueBuf.Point]);
//                    SlaveIssueBuf.Point++;
//                }
//                else
//                {
//                    SlaveIssueBuf.Point = 0;
//                    SlaveIssueBuf.FramLen = 0;
//									  SlaveIssueBuf.TStatus=true;
//									  TcpMybuf.SlaveIntervaltime=SLAVEtimeout;
//                    UART3_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
//                }
//            }
//            else
            {
                UART3_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
            }
            break;
            
        case UART_II_MODEM_CHG:          // 只支持串口 0
            break;
            
        default:
            break;
    }
}

/**
* @brief  使用microLib的方法,使用printf
* @warningg 勾选 microLib
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



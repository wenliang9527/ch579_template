
#include <stdio.h>
#include "uart.h"







///*串口中断*/
//void UART0_IRQHandler(void)
//{
//    switch (UART0_GetITFlag())
//    {
//        case UART_II_LINE_STAT:          // 线路状态错误
//            UART0_GetLinSTA();
//            break;
//            
//        case UART_II_RECV_RDY:           // 数据达到设置触发点
//            /* 串口数据存储到缓存 */
//            PCIssueBuf.FrameBuf[PCIssueBuf.Point] = UART0_RecvByte();  // 读取串口数据
//            PCIssueBuf.Point++;
//            TcpMybuf.SLOMIDUP = USART_0_IDNumber;
//            TcpMybuf.PCInitializationCOM = 1;
//            break;
//            
//        case UART_II_RECV_TOUT:          // 接收超时，暂时一帧数据接收完成
//            SlaveUploadBuf.FrameBuf[PCIssueBuf.Point] = UART0_RecvByte();  // 读取串口数据
//            /* 串口数据存储到缓存 */
//            PCIssueBuf.Point++;
//            PCIssueBuf.FramLen = PCIssueBuf.Point;  // 接收数据长度
//            PCIssueBuf.TStatus = 1;                     // 接收完成标志
//            PCIssueBuf.Point = 0;
//            TcpMybuf.SLOMIDUP = USART_0_IDNumber;
//            TcpMybuf.PCInitializationCOM = 1;
//            break;
//            
//        case UART_II_THR_EMPTY:          // 发送缓存区空，可继续发送
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
//            }
//            else
//            {
//                UART0_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
//            }
//            break;
//            
//        case UART_II_MODEM_CHG:          // 只支持串口 0
//            break;
//            
//        default:
//            break;
//    }
//}
/*串口中断*/
//void UART3_IRQHandler(void)
//{
//    switch (UART3_GetITFlag())
//    {
//        case UART_II_LINE_STAT:          // 线路状态错误
//            UART3_GetLinSTA();
//            break;
//            
//        case UART_II_RECV_RDY:           // 数据达到设置触发点
//            /* 串口数据存储到缓存 */
//            SlaveUploadBuf.FrameBuf[SlaveUploadBuf.Point] = UART3_RecvByte();  // 读取串口数据
//            SlaveUploadBuf.Point++;
//            TcpMybuf.SLOMIDDOWN = USART_3_IDNumber;
//            TcpMybuf.InitializationCOM = 1;
//						SlaveUploadBuf.TStatus =0;
//            break;
//            
//        case UART_II_RECV_TOUT:          // 接收超时，暂时一帧数据接收完成
//            SlaveUploadBuf.FrameBuf[SlaveUploadBuf.Point] = UART3_RecvByte();  // 读取串口数据
//            /* 串口数据存储到缓存 */
//            SlaveUploadBuf.Point++;
//            SlaveUploadBuf.FramLen = SlaveUploadBuf.Point;  // 接收数据长度
//            SlaveUploadBuf.TStatus = 1;                     // 接收完成标志
//            SlaveUploadBuf.Point = 0;
//            TcpMybuf.SLOMIDDOWN = USART_3_IDNumber;
//            TcpMybuf.InitializationCOM = 1;
//            break;
//            
//        case UART_II_THR_EMPTY:          // 发送缓存区空，可继续发送
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
//            {
//                UART3_INTCfg(DISABLE, RB_IER_THR_EMPTY);  // 关闭中断发送
//            }
//            break;
//            
//        case UART_II_MODEM_CHG:          // 只支持串口 0
//            break;
//            
//        default:
//            break;
//    }
//}


//void PCUpload_listADD(TCommDataPacket buf)       
//{
//    if (PCsendlist.W_Point < MAXSENTLISTLEN)  
//    {
//        PCsendlist.Field[PCsendlist.W_Point] = buf;
//        PCsendlist.W_Point++;
//        if(PCsendlist.W_Point >= MAXSENTLISTLEN)
//        {
//            PCsendlist.W_Point = 0;  
//        }
//    }	
//}
//void PCUpload_listsend()
//{
//    if (PCsendlist.W_Point != PCsendlist.R_Point) 
//    {
//        // 检查函数指针是否为空
//			if(TcpMybuf.PCInitializationCOM==0)
//			{
//				uart_send_byte_it(USART_0_IDNumber,PCsendlist.Field[PCsendlist.R_Point]);
//				while(PCUploadBuf.TStatus==false);
//				uart_send_byte_it(USART_1_IDNumber,PCsendlist.Field[PCsendlist.R_Point]);
//				while(PCUploadBuf.TStatus==false);
//				uart_send_byte_it(Internet_IDNumber,PCsendlist.Field[PCsendlist.R_Point]);
//			}else
//				{
//					uart_send_byte_it(TcpMybuf.SLOMIDUP,PCsendlist.Field[PCsendlist.R_Point]);
//				}
//        PCsendlist.R_Point++;
//        if(PCsendlist.R_Point >= MAXSENTLISTLEN)
//        {
//            PCsendlist.R_Point = 0;  // 如果读指针超出了缓冲区长度，重置为0
//        }
//    }
//}
//void SlaveIssue_listADD(TCommDataPacket buf)       
//{
//    if (Slavesendlist.W_Point < MAXSENTLISTLEN)  
//    {
//        Slavesendlist.Field[Slavesendlist.W_Point] = buf;
//        Slavesendlist.W_Point++;
//        if(Slavesendlist.W_Point >= MAXSENTLISTLEN)
//        {
//            Slavesendlist.W_Point = 0;  
//        }
//    }	
//}
//void SlaveIssue_listsend()
//{
//    if (Slavesendlist.W_Point != Slavesendlist.R_Point) 
//    {
//			if(TcpMybuf.InitializationCOM==0)
//			{
//				uart_send_byte_it(USART_2_IDNumber,Slavesendlist.Field[Slavesendlist.R_Point]);
//				while(SlaveIssueBuf.TStatus==false);
//				uart_send_byte_it(USART_3_IDNumber,Slavesendlist.Field[Slavesendlist.R_Point]);
//			}else
//			{
//        // 检查函数指针是否为空
//        uart_send_byte_it(TcpMybuf.SLOMIDDOWN,Slavesendlist.Field[Slavesendlist.R_Point]);
//			}
//        Slavesendlist.R_Point++;
//        if(Slavesendlist.R_Point >= MAXSENTLISTLEN)
//        {
//            Slavesendlist.R_Point = 0;  // 如果读指针超出了缓冲区长度，重置为0
//        }
//    }
//}









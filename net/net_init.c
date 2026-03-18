#define net_init_c

#include "net_init.h"

#if 1 //是否打印日志
#define debug_printf printf
#else
#define debug_printf(...)
#endif

#define KEEPLIVE_ENABLE                      1                                  /* 开启KEEPLIVE功能 */

/* 下面的缓冲区和全局变量必须要定义，库中调用 */
__align(16)UINT8    CH57xMACRxDesBuf[(RX_QUEUE_ENTRIES )*16];                   /* MAC接收描述符缓冲区，16字节对齐 */
__align(4) UINT8    CH57xMACRxBuf[RX_QUEUE_ENTRIES*RX_BUF_SIZE];                /* MAC接收缓冲区，4字节对齐 */
__align(4) SOCK_INF SocketInf[CH57xNET_MAX_SOCKET_NUM];                         /* Socket信息表，4字节对齐 */

UINT16 MemNum[8] = {CH57xNET_NUM_IPRAW,
                    CH57xNET_NUM_UDP,
                    CH57xNET_NUM_TCP,
                    CH57xNET_NUM_TCP_LISTEN,
                    CH57xNET_NUM_TCP_SEG,
                    CH57xNET_NUM_IP_REASSDATA,
                    CH57xNET_NUM_PBUF,
                    CH57xNET_NUM_POOL_BUF
                    };
 UINT16 MemSize[8] = {CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_IPRAW_PCB),
                    CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_UDP_PCB),
                    CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_TCP_PCB),
                    CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_TCP_PCB_LISTEN),
                    CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_TCP_SEG),
                    CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_IP_REASSDATA),
                    CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_PBUF) + CH57xNET_MEM_ALIGN_SIZE(0),
                    CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_PBUF) + CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_POOL_BUF)
                    };
__align(4)UINT8 Memp_Memory[CH57xNET_MEMP_SIZE];
__align(4)UINT8 Mem_Heap_Memory[CH57xNET_RAM_HEAP_SIZE];
__align(4)UINT8 Mem_ArpTable[CH57xNET_RAM_ARP_TABLE_SIZE];
/******************************************************************************/
/* 本演示程序的相关宏 */
#define RECE_BUF_LEN                          536  /* 接收缓冲区的大小 */
/* CH57xNET库TCP的MSS长度为536字节，即一个TCP包里的数据部分最长为536字节 */
/* TCP协议栈采用滑动窗口进行流控，窗口最大值为socket的接收缓冲区长度。在设定 */
/* RX_QUEUE_ENTRIES时要考虑MSS和窗口之间的关系，例如窗口值为4*MSS，则远端一次会发送 */
/* 4个TCP包，如果RX_QUEUE_ENTRIES小于4，则必然会导致数据包丢失，从而导致通讯效率降低 */
/* 建议RX_QUEUE_ENTRIES要大于( 窗口/MSS ),如果多个socket同时进行大批量发送数据，则 */ 
/* 建议RX_QUEUE_ENTRIES要大于(( 窗口/MSS )*socket个数) 在多个socket同时进行大批数据收发时 */
/* 为了节约RAM，请将接收缓冲区的长度设置为MSS */
																		
/* CH579相关定义 */
#define net_use_custom_mac 0 //1:使用用户自定义的MAC 0:使用模组自带的MAC
UINT8 MACAddr[6] = {0x84,0xc2,0xe4,0x02,0x03,0x04};                             /* CH579MAC地址 */
//UINT8 IPAddr[4] = {192,168,1,200};                                              /* CH579IP地址 */
//UINT8 GWIPAddr[4] = {192,168,1,1};                                              /* CH579网关 */
UINT8 IPAddr[4] = {160,128,1,200};                                              /* CH579IP地址 *///印度客户
UINT8 GWIPAddr[4] = {160,128,1,1};                                              /* CH579网关 *///印度客户
UINT8 IPMask[4] = {255,255,255,0};                                              /* CH579子网掩码 */

/* 网口灯定义 只针对PB口 */
//UINT16 CH57xNET_LEDCONN=GPIO_Pin_5;   GPIO_Pin_4                                              /* 连接指示灯 PB4 */
//UINT16 CH57xNET_LEDDATA=GPIO_Pin_6;   GPIO_Pin_7                                             /* 通讯指示灯 PB7 */ 

/* 网口灯定义 PB口低十六位有效 */
UINT16 CH57xNET_LEDCONN=0x0010;                                                 /* 连接指示灯 PB4 */
UINT16 CH57xNET_LEDDATA=0x0080;                                                 /* 通讯指示灯 PB7 */ 


/*******************************************************************************
* Function Name  : net_initkeeplive
* Description    : keeplive初始化
* Input          : None      
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef  KEEPLIVE_ENABLE
void net_initkeeplive(void)
{
    struct _KEEP_CFG  klcfg;

    klcfg.KLIdle = 20000;   /* 空闲该ms时间启动探测*/
    klcfg.KLIntvl = 10000;  /* 间隔该ms探测一次*/
    klcfg.KLCount = 5;      /* 探测最大次数 */
    CH57xNET_ConfigKeepLive(&klcfg);
}
#endif



/*******************************************************************************
* Function Name  : CH57xNET_LibInit
* Description    : 库初始化操作
* Input          : ip      ip地址指针
*                ：gwip    网关ip地址指针
*                : mask    掩码指针
*                : macaddr MAC地址指针 
* Output         : None
* Return         : 执行状态
*******************************************************************************/
UINT8 CH57xNET_LibInit(/*const*/ UINT8 *ip,/*const*/ UINT8 *gwip,/*const*/ UINT8 *mask,/*const*/ UINT8 *macaddr)
{
    UINT8 i;
    struct _CH57x_CFG cfg;

    if(CH57xNET_GetVer() != CH57xNET_LIB_VER)return 0xfc;                       /* 获取库的版本号，检查是否和头文件一致 */
    CH57xNETConfig = LIB_CFG_VALUE;                                             /* 将配置信息传递给库的配置变量 */
    cfg.RxBufSize = RX_BUF_SIZE; 
    cfg.TCPMss   = CH57xNET_TCP_MSS;
    cfg.HeapSize = CH57x_MEM_HEAP_SIZE;
    cfg.ARPTableNum = CH57xNET_NUM_ARP_TABLE;
    cfg.MiscConfig0 = CH57xNET_MISC_CONFIG0;
    CH57xNET_ConfigLIB(&cfg);
    i = CH57xNET_Init(ip,gwip,mask,macaddr);
#ifdef  KEEPLIVE_ENABLE
    net_initkeeplive( );
#endif
    return (i);                      
}


/*******************************************************************************
* Function Name  : GetMacAddr
* Description    : 系统获取MAC地址
* Input          : pMAC:指向用来存储Mac地址的缓冲
* Output         : None
* Return         : None
*******************************************************************************/
void GetMacAddr(UINT8 *pMAC)
{
	UINT8 transbuf[6],i;
	
	GetMACAddress(transbuf);
	for(i=0;i<6;i++)
	{
		pMAC[5-i]=transbuf[i];
	}
}

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_IRQHandler( void )						                             	/* 以太网中断 */
{
	CH57xNET_ETHIsr();								                            /* 以太网中断中断服务函数 */
}


void net_init(void)
{
	UINT16 state=0;
	#if (net_use_custom_mac==0)
		GetMacAddr(MACAddr);/* 获取MAC地址 */
	#endif
	/*初始化NET*/
	state = CH57xNET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* 库初始化 */
	if(state!=CH57xNET_ERR_SUCCESS)
	{
		debug_printf("mStopIfError: %02X\r\n", (UINT16)state);
		DelayMs(10);
		SYS_ResetExecute();//重启
	}
	NVIC_SetPriority(ETH_IRQn,0);//设置中断优先级(也可以不设置,默认3)
	NVIC_EnableIRQ(ETH_IRQn);
}










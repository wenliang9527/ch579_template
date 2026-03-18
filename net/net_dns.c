#define net_dns_c

#include "net_dns.h"

#if 1 //是否打印日志
#define debug_printf printf
#else
#define debug_printf(...)
#endif


unsigned char  net_dns_socket_index=255;             /* 保存socket索引*/

unsigned char  dns_server_ip[4]={114, 114, 114, 114};   /*DNS服务器，需根据实际DNS服务器修改*/
unsigned short dns_server_port   = 53;         /*DNS默认端口*/
unsigned short dns_local_port  = 1000;         /*Socket 本地端口 */

unsigned short nds_msg_id = 0x1100;               /*标识*/

/*缓存DNS包协议数据*/
#define    net_dns_buf_len    512
unsigned char  net_dns_buf[net_dns_buf_len];
/*缓存UDP整个数据包*/
#define net_dns_socket_buf_len 536
unsigned char net_dns_socket_buf[net_dns_socket_buf_len];
/*缓存udp接收的数据*/
#define net_dns_socket_recv_buf_len 512
unsigned char net_dns_socket_recv_buf[net_dns_socket_recv_buf_len];

unsigned char * net_dns_ip_copy;//记录存放IP的数组地址
unsigned char net_dns_start=0;//标记是否已经启动
int net_dns_time_out_cnt=0;//超时计数

//UINT8 domain_ip

/*********************************************************************************
* Function Name  : get16
* Description    : 将缓冲区UINT8数据转为UINT16格式数据
* Input          : UINT8 类型数据
* Output         : None
* Return         : 转化后的UINT16类型数据
**********************************************************************************/
unsigned short net_dns_get16(unsigned char * s)
{
  unsigned short i;
  i = *s++ << 8;
  i = i + *s;
  return i;
}

/**********************************************************************************
* Function Name  : ParseName
* Description    : 分析完整的域名
* Input          : msg            -指向报文的指针
                   compressed    -指向报文中主域名的指针
                   buf            -缓冲区指针，用于存放转化后域名
* Output         : None
* Return         : 压缩报文的长度
**********************************************************************************/
int net_dns_parse_name(unsigned char * msg, unsigned char * compressed, char * buf)
{
  unsigned short slen;                                                                            /* 当前片段长度*/
  unsigned char * cp;
  int clen = 0;                                                                           /* 压缩域名长度 */
  int indirect = 0;
  int nseg = 0;                                                                           /* 域名被分割的片段总数 */
  cp = compressed;
  for (;;)
  {
    slen = *cp++;                                                                          /* 首字节的计数值*/
    if (!indirect) clen++;
    if ((slen & 0xc0) == 0xc0)                                                             /*计数字节高两比特为1，用于压缩格式*/
    {
      if (!indirect)
        clen++;
        indirect = 1;
        cp = &msg[((slen & 0x3f)<<8) + *cp];                                              /*按计数字节数值指针偏移到指定位置*/
        slen = *cp++;
    } 
    if (slen == 0)                                                                        /*计数为0，结束*/
        break;
    if (!indirect) clen += slen;
    while (slen-- != 0) *buf++ = (char)*cp++;
    *buf++ = '.';
    nseg++;
  }
   if (nseg == 0)
   {
     *buf++ = '.';
   }
   *buf++ = '\0';
   return clen;                                                                            /*压缩报文长度 */
}

/*********************************************************************************
* Function Name  : DnsQuestion
* Description    : 分析响应报文中的问题记录部分
* Input          : msg          -指向响应报文的指针
                   cp           -指向问题记录的指针
* Output         : None
* Return         :指向下一记录的指针
**********************************************************************************/
unsigned char * net_dns_question(unsigned char * msg, unsigned char * cp)
{
  int len;
  char name[net_dns_buf_len];
  len = net_dns_parse_name(msg, cp, name);
  cp += len;
  cp += 2;                                                                                  /* 类型 */
  cp += 2;                                                                                  /* 类 */
  return cp;
}

/*********************************************************************************
* Function Name  : DnsAnswer
* Description    : 分析响应报文中的回答记录部分
* Input          : msg            -指向响应报文的指针
                   cp             -指向回答记录的指针
                   psip           
* Output         : None
* Return         :指向下一记录的指针
**********************************************************************************/
unsigned char * net_dns_answer(unsigned char * msg, unsigned char * cp, unsigned char * pSip)
{
  int len, type;
  char name[net_dns_buf_len];
  len = net_dns_parse_name(msg, cp, name);
  cp += len;
  type = net_dns_get16(cp);
  cp += 2;                                                                                   /* 类型 */
  cp += 2;                                                                                   /* 类 */
  cp += 4;                                                                                   /* 生存时间 */
  cp += 2;                                                                                   /* 资源数据长度 */
  switch (type)
  {
  case TYPE_A:
    pSip[0] = *cp++;
    pSip[1] = *cp++;
    pSip[2] = *cp++;
    pSip[3] = *cp++;
    break;
  case TYPE_CNAME:
  case TYPE_MB:
  case TYPE_MG:
  case TYPE_MR:
  case TYPE_NS:
  case TYPE_PTR:
    len = net_dns_parse_name(msg, cp, name);
    cp += len;
    break;
  case TYPE_HINFO:

  case TYPE_MX:

  case TYPE_SOA:

  case TYPE_TXT:
    break;
  default:
    break;
  }
  return cp;
}
/*********************************************************************************
* Function Name  : parseMSG
* Description    : 分析响应报文中的资源记录部分
* Input          : msg            -指向DNS报文头部的指针
                   cp             -指向响应报文的指针
                   pSip
* Output         : None
* Return         :成功返回0，否则返回1
**********************************************************************************/
unsigned char net_dns_parse_msg(struct dhdr * pdhdr, unsigned char * pbuf, unsigned char * pSip)
{
  unsigned short tmp;
  unsigned short i;
  unsigned char * msg;
  unsigned char * cp;
  msg = pbuf;
  memset(pdhdr, 0, sizeof(pdhdr));
  pdhdr->id = net_dns_get16(&msg[0]);
  tmp = net_dns_get16(&msg[2]);
  if (tmp & 0x8000) pdhdr->qr = 1;
  pdhdr->opcode = (tmp >> 11) & 0xf;
  if (tmp & 0x0400) pdhdr->aa = 1;
  if (tmp & 0x0200) pdhdr->tc = 1;
  if (tmp & 0x0100) pdhdr->rd = 1;
  if (tmp & 0x0080) pdhdr->ra = 1;
  pdhdr->rcode = tmp & 0xf;
  pdhdr->qdcount = net_dns_get16(&msg[4]);
  pdhdr->ancount = net_dns_get16(&msg[6]);
  pdhdr->nscount = net_dns_get16(&msg[8]);
  pdhdr->arcount = net_dns_get16(&msg[10]);
  /* 分析可变数据长度部分*/
  cp = &msg[12];
  for (i = 0; i < pdhdr->qdcount; i++)                                                      /* 查询问题 */
  {
    cp = net_dns_question(msg, cp);
  }  
  for (i = 0; i < pdhdr->ancount; i++)                                                      /* 回答 */
  {
    cp = net_dns_answer(msg, cp, pSip);
  }
  for (i = 0; i < pdhdr->nscount; i++)                                                      /*授权 */
  {
    /*待解析*/  ;
  }  
  for (i = 0; i < pdhdr->arcount; i++)                                                      /* 附加信息 */
  {
    /*待解析*/  ;
  }
  if(pdhdr->rcode == 0) return 0;                                                           /*rcode = 0:成功*/
  else return 1;
}

/*********************************************************************************
* Function Name  : put16
* Description    :UINT16 格式数据按UINT8格式存到缓冲区
* Input          : s -缓冲区首地址
                   i -UINT16数据
* Output         : None
* Return         : 偏移指针
**********************************************************************************/
unsigned char * net_dns_put16(unsigned char * s, unsigned short i)
{
  *s++ = i >> 8;
  *s++ = i;
  return s;
}

/**********************************************************************************
* Function Name  : MakeDnsQuery
* Description    : 制作DNS查询报文
  input          : op   - 递归
*                  name - 指向待查域名指针
*                  buf  - DNS缓冲区.
*                  len  - 缓冲区最大长度.
* Output         : None
* Return         : 指向DNS报文指针
**********************************************************************************/
unsigned short net_dns_make_query_msg(unsigned short op, char * name, unsigned char * buf, unsigned short len)
{
  unsigned char *cp;
  char *cp1;
  char tmpname[net_dns_buf_len];
  char *dname;
  unsigned short p;
  unsigned short dlen;
  cp = buf;
  nds_msg_id++;
  cp = net_dns_put16(cp, nds_msg_id);                                                             /*标识*/
  p = (op << 11) | 0x0100;      
  cp = net_dns_put16(cp, p);                                                                      /* 0x0100：Recursion desired */
  cp = net_dns_put16(cp, 1);                                                                      /*问题数：1*/
  cp = net_dns_put16(cp, 0);                                                                      /*资源记录数：0*/
  cp = net_dns_put16(cp, 0);                                                                      /*资源记录数：0*/
  cp = net_dns_put16(cp, 0);                                                                      /*额外资源记录数：0*/
  strcpy(tmpname, name);
  dname = tmpname;
  dlen = strlen(dname);
  for (;;)
  {                                                                                        /*按照DNS请求报文域名格式，把URI写入到buf里面去*/
    cp1 = strchr(dname, '.');
    if (cp1 != NULL) len = cp1 - dname;  
    else len = dlen;    
    *cp++ = len;      
    if (len == 0) break;    
    strncpy((char *)cp, dname, len);
    cp += len;
    if (cp1 == NULL)
    {
      *cp++ = 0;    
      break;
    }
    dname += len+1;                                                                        /*dname首地址后移*/
    dlen -= len+1;                                                                         /*dname长度减小*/
  }
  cp = net_dns_put16(cp, 0x0001);                                                                  /* type ：1------ip地址*/
  cp = net_dns_put16(cp, 0x0001);                                                                  /* class ：1-------互联网地址*/
  return ((UINT16)(cp - buf));
}


/**
* @brief   解析DNS服务器返回的数据
* @param   data  返回的数据首地址
* @param   None
* @param   None
* @param   None
* @retval  0:成功; others:错误
* @warning None
* @example 
**/
char net_dns_parse(unsigned char *data)
{
	struct dhdr dhp;
	return net_dns_parse_msg(&dhp, data, net_dns_ip_copy);
}


/**
* @brief   初始化socket
* @param   ipaddr  目的地址
* @param   desprot 目的端口号
* @param   surprot 本地端口号
* @retval  0:初始化成功; others:初始化失败
* @warning None
* @example 
**/
char net_dns_socket_init(unsigned char *addr ,unsigned short DesPort ,unsigned short SourPort)
{
	unsigned char state;                                                             
	SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */
	memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
	memcpy((void *)TmpSocketInf.IPAddr,addr,4);                                  /* 设置目的IP地址 */
	
	TmpSocketInf.DesPort = DesPort;                                              /* 设置目的端口 */
	TmpSocketInf.SourPort = SourPort;                                            /* 设置源端口 */
	TmpSocketInf.ProtoType = PROTO_TYPE_UDP;                                     /* 设置socekt类型 */
	TmpSocketInf.RecvStartPoint = (unsigned long)net_dns_socket_buf;                         /* 设置接收缓冲区的接收缓冲区 */
	TmpSocketInf.RecvBufLen = net_dns_socket_buf_len ;                                     /* 设置接收缓冲区的接收长度 */
	state = CH57xNET_SocketCreat((unsigned char *)(&net_dns_socket_index),&TmpSocketInf); /* 创建socket，将返回的socket索引保存在SockeId中 */
	debug_printf("net_dns_socket_init: %d\r\n", net_dns_socket_index);
	if(state!=CH57xNET_ERR_SUCCESS)
	{
		debug_printf("net_dns_socket_init_error: %02X\r\n", (UINT16)state);
	}
	return state;
}


/**
* @brief   发送数据
* @param   socketid
* @param   buf 数据地址
* @param   len 数据长度
* @retval  0:成功; others:失败
* @warning None
* @example 
**/
char net_dns_socket_send(unsigned char socketid, unsigned char *buf, unsigned long len)
{
	unsigned long length = len;
	return CH57xNET_SocketSend(socketid,buf,&length);
}


/**
* @brief   关闭socket
* @param   socketid
* @param   buf 数据地址
* @param   len 数据长度
* @retval  0:成功; others:失败
* @warning None
* @example 
**/
char net_dns_socket_close(unsigned char socketid)
{
	if(socketid!=255){//创建了socket
		net_dns_socket_index=255;
		return CH57xNET_SocketClose( socketid,TCP_CLOSE_NORMAL );
	}
	net_dns_socket_index=255;
	return 255;
}


/**
* @brief   DNS超时检测,放到定时器里面进行轮训
* @param   None
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning 推荐放到1ms定时器, 超时时间200ms就可以
* @example 
**/
void net_dns_loop(void)
{
	if(net_dns_start==1)
	{
		net_dns_time_out_cnt++;
		if(net_dns_time_out_cnt>3000)//超时200ms
		{
			net_dns_time_out_cnt=0;
			net_dns_start=0;
			
			net_dns_socket_close(net_dns_socket_index);
		}
	}
	else
	{
		net_dns_time_out_cnt=0;
	}
}


/**
* @brief   发送查询DNS指令
* @param   name  域名
* @param   ip    解析的IP地址存储的地址
* @param   None
* @param   None
* @retval  0:成功; others:错误
* @warning None
* @example
**/
char net_dns_query(char *domain_name, unsigned char *ip)
{
	unsigned long len=0;
	if(net_dns_start==0 && CH57xInf.PHYStat>=2){//没有在执行
		debug_printf("net_dns_query\r\n");
		net_dns_ip_copy = ip;
		net_dns_start=1;
		if(net_dns_socket_init(dns_server_ip,dns_server_port,dns_local_port) == 0)
		{
			debug_printf("net_dns_make_query_msg\r\n");
			len = net_dns_make_query_msg(0,domain_name, net_dns_buf, net_dns_buf_len);
			if(net_dns_socket_send(net_dns_socket_index, net_dns_buf, len)!=0)
			{
				net_dns_socket_close(net_dns_socket_index);
				return 2;
			}
		}
		else
		{
			debug_printf("net_dns_socket_init_err\r\n");
			net_dns_socket_close(net_dns_socket_index);
			return 1;
		}
	}
	else
	{
		return 3;
	}
	return 0;
}



/**
* @brief   DNS数据解析函数
* @param   sockeid
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void net_dns_socket_data(unsigned char sockeid, unsigned char initstat)
{
	unsigned long len;
	char   err=0;
	
	if(sockeid == net_dns_socket_index)
	{
		if(initstat & SINT_STAT_RECV)//接收到数据
    {
			len = CH57xNET_SocketRecvLen(sockeid,NULL);//读取数据个数
			if (len > 0)
			{
				CH57xNET_SocketRecv(sockeid,net_dns_socket_recv_buf,&len);//提取数据
				
				if(net_dns_start==1)
				{
					if((err=net_dns_parse(net_dns_socket_recv_buf)) == 0){//解析DNS数据包,解析成功会把IP数据缓存到用户设置的数组里面
						
						net_dns_socket_close(net_dns_socket_index);
						net_dns_start=0;
					}
					else
						{
					 }
				}
			}
    }
	}
}


/**
*@brief		用于解析字符串IP,返回1说明转换成功,换回0说明IP不可解析,可能是域名
*@param		str 要转换的数据
*@param		ip  转换后存储的位置
*@return 	0:转换失败,可能是域名   1:转换成功
*@example  net_dns_str_ip("192.168.0.1", &ip) ip[0]=192;ip[1]=168;ip[2]=0;ip[3]=1;
*/ 
uint8_t net_dns_str_ip(const char* str, void *ip)
{
	/* The count of the number of bytes processed. */
	int i;
	/* A pointer to the next digit to process. */
	const char * start;

	start = str;
	for (i = 0; i < 4; i++) {
			/* The digit being processed. */
			char c;
			/* The value of this byte. */
			int n = 0;
			while (1) {
					c = * start;
					start++;
					if (c >= '0' && c <= '9') {
							n *= 10;
							n += c - '0';
					}
					/* We insist on stopping at "." if we are still parsing
						 the first, second, or third numbers. If we have reached
						 the end of the numbers, we will allow any character. */
					else if ((i < 3 && c == '.') || i == 3) {
							break;
					}
					else {
							return 0;
					}
			}
			if (n >= 256) {
					return 0;
			}
			((uint8_t*)ip)[i] = n;
	}
	return 1;
}


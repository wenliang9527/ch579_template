#ifndef net_dns_h
#define net_dns_h

#ifndef net_dns_c//如果没有定义
#define net_dns_cx extern
#else
#define net_dns_cx
#endif

#include "CH57x_common.h"
#include "CH57xNET.H"
#include <stdio.h>


/*
********************************************************************************
Define Part
********************************************************************************
*/

#define    TYPE_A         1    /* Host address */
#define    TYPE_NS        2    /* Name server */
#define    TYPE_MD        3    /* Mail destination (obsolete) */
#define    TYPE_MF        4    /* Mail forwarder (obsolete) */
#define    TYPE_CNAME     5    /* Canonical name */
#define    TYPE_SOA       6    /* Start of Authority */
#define    TYPE_MB        7    /* Mailbox name (experimental) */
#define    TYPE_MG        8    /* Mail group member (experimental) */
#define    TYPE_MR        9    /* Mail rename name (experimental) */
#define    TYPE_NULL      10   /* Null (experimental) */
#define    TYPE_WKS       11   /* Well-known sockets */
#define    TYPE_PTR       12   /* Pointer record */
#define    TYPE_HINFO     13   /* Host information */
#define    TYPE_MINFO     14   /* Mailbox information (experimental)*/
#define    TYPE_MX        15   /* Mail exchanger */
#define    TYPE_TXT       16   /* Text strings */
#define    TYPE_ANY       255  /* Matches any type */

#define    CLASS_IN    1    /* The ARPA Internet */


struct dhdr         
{             
    UINT16  id;         /* 标识 */
    UINT8   qr;         /* 查询或应答标志*/
    UINT8   opcode;
    UINT8   aa;         /* 授权回答 */
    UINT8   tc;         /* 可截断的 */
    UINT8   rd;         /* 期望递归*/
    UINT8   ra;         /* 可以递归 */
    UINT8   rcode;      /* 应答码 */
    UINT16  qdcount;    /* 问题数 */
    UINT16  ancount;    /* 应答数 */
    UINT16  nscount;    /* 授权数 */
    UINT16  arcount;    /* 额外记录数 */
};

unsigned short net_dns_get16(unsigned char * s);

int net_dns_parse_name(unsigned char * msg, unsigned char * compressed, char * buf);

unsigned char * net_dns_question(unsigned char * msg, unsigned char * cp);

unsigned char * net_dns_answer(unsigned char * msg, unsigned char * cp, unsigned char * pSip);

unsigned char * net_dns_put16(unsigned char * s, unsigned short i);

unsigned short net_dns_make_query_msg(unsigned short op, char * name, unsigned char * buf, unsigned short len);

void net_dns_loop(void);

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
char net_dns_query(char *domain_name, unsigned char *ip);

/**
* @brief   socket处理函数
* @param   domain_ip  解析出来的IP地址
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void net_dns_socket_data(unsigned char sockeid, unsigned char initstat);

/**
*@brief		用于解析字符串IP,返回1说明转换成功,换回0说明IP不可解析,可能是域名
*@param		str 要转换的数据
*@param		ip  转换后存储的位置
*@return 	0:转换失败,可能是域名   1:转换成功
*@example  net_dns_str_ip("192.168.0.1", &ip) ip[0]=192;ip[1]=168;ip[2]=0;ip[3]=1;
*/ 
uint8_t net_dns_str_ip(const char* str, void *ip);

#endif


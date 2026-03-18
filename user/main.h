#ifndef __MAIN_H_
#define __MAIN_H_

#ifndef MAIN_Cx_
#define MAIN_Cx_ extern
#else
#define MAIN_Cx_
#endif

#include "CH57x_common.h"


//缓存数据使用
#define main_buffer_len 1024
extern unsigned char main_buffer[main_buffer_len];//缓存数据,全局通用
extern uint32_t  main_len;      //全局通用变量
extern unsigned char *main_str;    //全局通用变量

#endif


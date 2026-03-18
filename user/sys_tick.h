#ifndef sys_tick_h_
#define sys_tick_h_

#ifndef sys_tick_c_//如果没有定义
#define sys_tick_cx_ extern
#else
#define sys_tick_cx_
#endif


#include "CH57x_common.h"


uint64_t sys_tick_get_timer(void);

#endif



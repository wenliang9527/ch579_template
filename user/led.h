#ifndef LED_H_
#define LED_H_

#ifndef LED_C_//如果没有定义
#define LED_Cx_ extern
#else
#define LED_Cx_
#endif

#include "CH57x_common.h"

/*config pin*/
#define LED_PIN    GPIO_Pin_0
#define LED_MODE   GPIO_ModeOut_PP_20mA
#define LED_GPIO_MODE_CONFIG() (GPIOB_ModeCfg( LED_PIN, LED_MODE )) //配置引脚为输出模式

#define LED_GPIO_SET_BITS() (GPIOB_SetBits(LED_PIN)) //配置引脚输出高低电平
#define LED_GPIO_RESET_BITS() (GPIOB_ResetBits(LED_PIN)) //配置引脚输出高低电平

#define LED_GPIO_READ_BITS() (GPIOB_ReadPortPin(LED_PIN)) //获取引脚状态


void led_set(int value);
char led_get(void);
#endif

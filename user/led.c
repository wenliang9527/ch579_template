#define LED_C_

#include "led.h"



/**
* @brief  control  led  ON or OFF
* @param  value 1:ON; 0:OFF
* @param  None
* @param  None
* @retval None
* @example 
**/
void led_set(int value)
{
	static char flag=0;
	
	switch(value)
	{
		case 1:
			 LED_GPIO_SET_BITS();
		break;
		case 0:
			 LED_GPIO_RESET_BITS();
		break;
		case -1:
			if(LED_GPIO_READ_BITS())
				LED_GPIO_RESET_BITS();
			else
				LED_GPIO_SET_BITS();
		break;
	}
	
	if(flag==0)
	{
		flag=1;
		LED_GPIO_MODE_CONFIG();
	}
}

/**
* @brief  led_get
* @param  None
* @param  None
* @param  None
* @retval value 1:ON; 0:OFF
* @example 
**/
char led_get(void)
{
	return LED_GPIO_READ_BITS();
}


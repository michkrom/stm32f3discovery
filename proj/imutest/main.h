/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include <stm32f30x.h>
#include <stm32f3_discovery.h>
#include "stm32f3_discovery_lsm303dlhc.h"
#include "stm32f3_discovery_l3gd20.h"
#include "hw_config.h"
#include "platform_config.h"

/**
	* @brief	Inserts a delay time.
	* @param	nTime: specifies the delay time length, in system ticks (1ms or whatever is configured)
	* @retval None
	*/
extern void Delay(unsigned nTicks);


// defined in stm32fxxx_it.c
/**
	* @brief	Current count of system ticks (since power on)
	*/
extern long unsigned SysTickCount;


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

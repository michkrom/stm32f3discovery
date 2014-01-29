#ifndef __BOARD_H
#define __BOARD_H

/* Common file for board STM32FxDISCOVERY */

#include <stm32f30x_rcc.h>
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
void Delay(unsigned nTicks);


// defined in stm32fxxx_it.c
/**
 * @brief	Current count of system ticks (since power on)
 */
volatile extern uint64_t SysTickCount;

/**
	* @brief	Initialize SysTick interrupt to fire every # of us
	* @param	SysTick interrupt's period [us]
	* @retval	success
	*/
int InitSysTick(unsigned periodus);

/**
	* @brief	Returns number of system clock ticks (count of HCLK periods)
	* @param	None
	* @retval	64 bit count of system clock ticks @ RCC_Clocks.HCLK_Frequency
	*/
uint64_t GetHClockTicks();

/**
	* @brief	Get HCLK frequency
	* @param	None
	* @retval	GCLK frequency [Hz]
	*/
unsigned GetHClkFrequency();

#endif /* __MAIN_H */

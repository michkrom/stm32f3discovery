#ifndef _LEDS_H
#define _LEDS_H

#include <stm32f3_discovery.h>

/**
	* @brief	Initializes LEDs GPIOs
	*/
extern void InitLeds(void);

#define L1	LED3
#define L2	LED5
#define L3	LED7
#define L4	LED9
#define L5	LED10
#define L6	LED8
#define L7	LED6
#define L8	LED4
#define ON			0x00
#define OFF			0x10
#define TOGGLE	0x20
#define ALL			0x80

/**
	* @brief	Turns leds on or off
	* @param	led - Lx | ON | OFF | TOGGLE or ALL | OFF | ON | TOGGLE
	* @retval None
	*/
extern void SetLed(int led);



#endif

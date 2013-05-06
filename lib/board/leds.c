#include "leds.h"


void InitLeds(void)
{
	/* Initialize LEDs and User Button available on STM32F3-Discovery board */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	STM_EVAL_LEDInit(LED7);
	STM_EVAL_LEDInit(LED8);
	STM_EVAL_LEDInit(LED9);
	STM_EVAL_LEDInit(LED10);
}


#define DIM(a) (sizoof(a)/sizoof(a[0]))
#define FOREACH(a,i) for(int i=0; i < DIM(a); i++)


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

void SetLed(int led)
{
	int i;

	if( led & ALL )
	{
		if( led & OFF )
			for(i=0;i<8;i++) { STM_EVAL_LEDOff((Led_TypeDef)i); }
		else if( led & TOGGLE )
			for(i=0;i<8;i++) { STM_EVAL_LEDToggle((Led_TypeDef)i); }
		else			
			for(i=0;i<8;i++) { STM_EVAL_LEDOn((Led_TypeDef)i); }
	}
	else if( led & OFF )
	{
		STM_EVAL_LEDOff((Led_TypeDef)(led&0xF));
	}
	else if( led & TOGGLE )
	{
		STM_EVAL_LEDToggle((Led_TypeDef)(led&0xF));
	}
	else
	{
		STM_EVAL_LEDOn((Led_TypeDef)(led&0xF));
	}
}


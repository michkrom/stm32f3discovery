/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>

#include "board.h"

#include "leds.h"
#include "key.h"
#include "serial.h"

/* LED bumper ------------------------------------------------------------------*/

/**
* @brief  bump leds by one around the circle
* @param speed [1..10]
* @retval None
*/
void bumpLeds(int speed)
{
    static const char l[] = { L1, L2, L3, L4, L5, L6, L7, L8 };
    static int n = 0;

    SetLed( l[(n/10) % sizeof(l)/sizeof(l[0])] | OFF );
    n = n+speed;
    SetLed( l[(n/10) % sizeof(l)/sizeof(l[0])] | ON );

}

/* IR receiver code ------------------------------------------------------------------*/

volatile uint32_t irCommand;

/**
* @brief initialize IR recever
* @retval None
*/
void InitIrRcv()
{
}

/**
* @brief get next command received or 0 if none
* @retval IR 33bit command (zero==none)
*/
unsigned irRcv()
{
	return irCommand;
}

/* MAIN ------------------------------------------------------------------*/

/**
* @brief	Main program.
* @param	None
* @retval None
*/
int main(void)
{
	InitSysTick(1000/*us*/);	
    //	InitSerial(56700);
    InitSerial(115200);
    InitKey();
    InitLeds();
    InitIrRcv();

    printf("STM32F3DISCO IRRCV\r\n");
    printf("HCLK=%u\r\n",GetHClkFrequency());
    
    while( 1 )
    {
		unsigned cmd = irRcv();
		printf("%x\r\n",cmd);
		bumpLeds(10);
	}
}


/**
* @brief	the END
* @retval None
*/
void _sys_exit(int return_code)
{
    while(1);
}

/**************************************************************************************/

#ifdef	USE_FULL_ASSERT

/**
* @brief	Reports the name of the source file and the source line number
*		where the assert_param error has occurred.
* @param	file: pointer to the source file name
* @param	line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
    *		ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    printf("ASSERT %s in %d\r\n",file,line);
}

#endif

/**
* @}
*/


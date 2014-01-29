/* Includes ------------------------------------------------------------------*/

#include "board.h"

/**
	* @brief	Get HCLK frequency
	* @param	None
	* @retval	GCLK frequency [Hz]
	*/
unsigned GetHClkFrequency()
{
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
	return RCC_Clocks.HCLK_Frequency;
}

/**
	* @brief	Initialize SysTick interrupt to given # of us
	* @param	SysTick interrupt periodus
	* @retval	success
	*/
int InitSysTick(unsigned periodus)
{
	unsigned hclkfreq = GetHClkFrequency();
	unsigned everyNsysticks = (hclkfreq * periodus) / 1000000 /*1/us*/;
    /* SysTick */
    return SysTick_Config(everyNsysticks);
}

// global sys tick counter
volatile uint64_t SysTickCount = 0;

// used in Delay()
volatile uint32_t TimingDelayCounter = 0;

/**
	* @brief	Inserts a delay time.
	* @param	nTicks to delay by
	* @retval None
	*/
void Delay(unsigned nTicks)
{
	//long unsigned endTB = SysTickCount+nTime_ms;
	//while( SysTickCount < endTB );	
	TimingDelayCounter = nTicks;
	while(TimingDelayCounter != 0);
}

/**
	* @brief	Returns number of system clock ticks
	* @param	None
	* @retval	64 bit count of system clock ticks @ RCC_Clocks.HCLK_Frequency
	*/
uint64_t GetHClockTicks()
{
    // loop until we read something that did not change
    while(1)
    {
        uint64_t c = SysTickCount;
        // get current value of systick counter
        int c2 = SysTick->VAL;
        // if software counter did not change we are good; otherwise repeat
        if( c == SysTickCount )
        {
            return c*(SysTick->LOAD+1) + c2; // SysTick->LOAD+1 = SYS TIMER PERIOD
        }
    }
}

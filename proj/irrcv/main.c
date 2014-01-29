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

#include "stm32f30x_syscfg.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_exti.h"
#include "stm32f30x_gpio.h"

// IR PA.1 --> EXTI Line1
#define IR_INPUT_PIN                GPIO_Pin_1
#define IR_INPUT_GPIO_PORT          GPIOA
#define IR_INPUT_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define IR_INPUT_EXTI_LINE          EXTI_Line1
#define IR_INPUT_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define IR_INPUT_EXTI_PIN_SOURCE    EXTI_PinSource1
#define IR_INPUT_EXTI_IRQn          EXTI1_IRQn

// symbol timing (in system HCLK ticks)
uint32_t IR_SYMBOL_START;
uint32_t IR_SYMBOL_LONG;
uint32_t IR_SYMBOL_SHORT;
uint32_t TOLERANCE;

/**
* @brief initialize IR recever
* @retval None
*/
void InitIrRcv()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the IR INPUT Clock */
    RCC_AHBPeriphClockCmd(IR_INPUT_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure IR INPUT as input */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = IR_INPUT_PIN;
    GPIO_Init(IR_INPUT_GPIO_PORT, &GPIO_InitStructure);

    /* EXTI configuration (interrupt) */

    /* Connect IR EXTI Line to IR GPIO Pin */
    SYSCFG_EXTILineConfig(IR_INPUT_EXTI_PORT_SOURCE, IR_INPUT_EXTI_PIN_SOURCE);

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = IR_INPUT_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set IR EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = IR_INPUT_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    // symbol timing (in system HCLK ticks)
    int tickfreq = GetHClockFrequency();
    IR_SYMBOL_START = (2000 * tickfreq) / 1000000;
    IR_SYMBOL_LONG  = (1200 * tickfreq) / 1000000;
    IR_SYMBOL_SHORT = (800 * tickfreq) / 1000000;
    TOLERANCE = (200 * tickfreq) / 1000000;
}


//------------------------------ IR decoder ----------------------------------

volatile unsigned long irReceivedCommand;

// start symbol + 32bit, IR burst is fixed @ 200us, space variable as per below
// symbol times
// start = 1600us
// long(0) = 800us
// short(1) = 400us
// repeats in ~170ms

// the symbols are timed from edge of IR to next edge of IR
// the timing is using just timer A counter (TAR 16bit@1us)
// the alg hinges on receiving and recognizing correct start signal
// but since TAR wraps around (every ~65.5ms) it is not ideal
// a possibility exist that wrapping would produce a delta time matching start by chance
// ideally I would use longer counter or reset TAR everytime IR is received
// another problem is when the train of symbols (bits) is not finished the alg
// would not know about it and could potentially join two separate trains (commands)
// again it is a (small) possiblity as the wrapping of timer may end up being a valid time delta
// ideally I would time out on receiving remaining bits and restart to wait for next start

volatile uint32_t x = 0;
volatile static uint32_t a[32];
volatile static uint32_t b[32];
static int ai = 0;

/**
 * Called from interrupt handler when an rising edge of IR signal is detected
 */
void irReportRisingEdgeDetected()
{
    // locals
    // temp to accumulate command bits
    static uint32_t irCommand;
    // current bit count -
    // 0 marks reception of start;
    // > IR_BIT_COUNT means not started or after error
    static int irBitCount;
    // previous edge timestamp
    static uint32_t prev=0; // rounded to next 100us

    // IR protocol definitions
    const int IR_BIT_COUNT = 32;

    // measure time from prev rise to this rise
    uint32_t now = SysTick->VAL;
    uint32_t delta = now > prev ? now-prev :((uint32_t)0xFFFFFFFF) - (prev-now);
    prev = now;

    ai = (ai+1) % 32;
    a[ai] = delta;
    b[ai] = now;

    // classify received symbol based on timing since last edge
    if( delta > IR_SYMBOL_START-TOLERANCE && delta < IR_SYMBOL_START+TOLERANCE )
    {
        // start interval
        irBitCount = 0;
        irCommand = 0;
    }
    else // one, zero or error
    {
        int one = ( delta > IR_SYMBOL_LONG-TOLERANCE && delta < IR_SYMBOL_LONG+TOLERANCE );
        int zero = ( delta > IR_SYMBOL_SHORT-TOLERANCE && delta < IR_SYMBOL_SHORT+TOLERANCE );
        int error = 1;
        if( one || zero )
        {
            // good bit of zero or one and we have space to store it
            if( irBitCount <= IR_BIT_COUNT )
            {
                // shift command and count the bit
                irCommand <<= 1;
                irBitCount ++;
                if( one ) {
                    // long interval ==> bit=1
                    irCommand |= 1;
                }
                error = 0;
            }
        }
        if( error )
        {
            // mark end of command
            irBitCount = IR_BIT_COUNT+1;
        }
    }
    x++;

    if( x % 32 == 0 ) {
        x = 0;
        // for debug
        bumpLeds(1);
    }

    // store command when all bit are received
    if( irBitCount == IR_BIT_COUNT ) {
        irReceivedCommand = irCommand;
        // mark end of command
        irBitCount = IR_BIT_COUNT+1;
    }
}

/**
 * Returns the state of IR input pin
 * **/
uint32_t irInputGetState()
{
  return GPIO_ReadInputDataBit(IR_INPUT_GPIO_PORT, IR_INPUT_PIN);
}

/**
 * Interrupt Handler for EXTI Line 1
 */
void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        irReportRisingEdgeDetected();
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
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
* @brief    Main program.
* @param    None
* @retval None
*/
int main(void)
{
    InitSysTick(1000/*us*/);
    //  InitSerial(56700);
    InitSerial(115200);
    InitKey();
    InitLeds();
    InitIrRcv();

    printf("STM32F3DISCO IRRCV\r\n");
    printf("HCLK=%u\r\n",GetHClockFrequency());

    while( 1 )
    {
        unsigned cmd = irRcv();
        //printf("%8X\r\n",cmd);
        uint64_t t = GetHClockTicks();
        printf("%08x%08x\r\n", (unsigned)(t>> 32), (unsigned)(t & 0xFFFFFFFF));
        //bumpLeds(10);
    }
}


/**
* @brief    the END
* @retval None
*/
void _sys_exit(int return_code)
{
    while(1);
}

/**************************************************************************************/

#ifdef  USE_FULL_ASSERT

/**
* @brief    Reports the name of the source file and the source line number
*           where the assert_param error has occurred.
* @param    file: pointer to the source file name
* @param    line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
    *       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    printf("ASSERT %s in %d\r\n",file,line);
}

#endif

/**
* @}
*/


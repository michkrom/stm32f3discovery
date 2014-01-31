/**
 * IR receiver/decoder (hw dependent part) for IR remote codes
 * contain pin interrupt and callout to the decoder
 *
 * @author Michal Krombholz
 * @license GNU General Public License (GPL) v2 or later
 */

#include "board.h"

#include "stm32f30x_syscfg.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_exti.h"
#include "stm32f30x_gpio.h"

#include "leds.h"

// defined in irRcv protocol dependent file
extern void irRcvInitProtocol(uint32_t ticksPerUs);
extern void irRcvReportEdgeDetected(uint32_t timeNowTicks);

// IR PA.1 --> EXTI Line1
#define IR_INPUT_PIN                GPIO_Pin_1
#define IR_INPUT_GPIO_PORT          GPIOA
#define IR_INPUT_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define IR_INPUT_EXTI_LINE          EXTI_Line1
#define IR_INPUT_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define IR_INPUT_EXTI_PIN_SOURCE    EXTI_PinSource1
#define IR_INPUT_EXTI_IRQn          EXTI1_IRQn

/**
* @brief initialize IR recever hardware & interrupts
* @retval None
*/
void irRcvInit()
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
    //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set IR EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = IR_INPUT_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    // symbol timing (in system HCLK ticks)
    uint32_t ticksPerUs = GetHClockFrequency()/ 1000000;

    // initialize IR protocol parameters
    irRcvInitProtocol(ticksPerUs);
}

/**
 * @breaf Returns the state of IR input pin (for debugging)
 */
uint32_t irRcvGetInputState()
{
  return GPIO_ReadInputDataBit(IR_INPUT_GPIO_PORT, IR_INPUT_PIN);
}

/**
 * Interrupt Handler for EXTI Line 1
 * on STM32F3DISCOVERY @72MHz it takes about 2us for handler to execute
 */
void EXTI1_IRQHandler(void)
{
    SetLed( L1 | ON );

    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        uint32_t now = (uint32_t)GetHClockTicks();
        irRcvReportEdgeDetected(now);
        EXTI_ClearITPendingBit(EXTI_Line1);
    }

    SetLed( L1 | OFF );
}

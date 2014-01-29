#include "key.h"
#include <stm32f30x.h>
#include <stm32f3_discovery.h>

#include "stm32f30x_exti.h"
#include "stm32f30x_gpio.h"

static unsigned UserButtonPressed = 0;
static unsigned UserButtonPressedLast = 0;

void InitKey(void)
{
    /* Initialize UserButton */
    STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
}

unsigned KeyPressCount(void)
{
    uint32_t cnt = UserButtonPressed - UserButtonPressedLast;
    UserButtonPressedLast = UserButtonPressed;
    return cnt;
}

/**
    * @brief    wait for key pressed, ignored any previous unchecked pressses
    */
void WaitKeyPressed(void)
{
    uint32_t curr = UserButtonPressed;
    while(UserButtonPressed==curr);
}

/**
 * called buy interrupt
 */
void KeyPressGpioCheck(void)
{
  int i;
  if ((EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) == SET)&&(STM_EVAL_PBGetState(BUTTON_USER) != RESET))
  {
    /* Delay */
    for(i=0; i<0x7FFFF; i++);

    /* Wait for SEL button to be reelased  */
    while(STM_EVAL_PBGetState(BUTTON_USER) != RESET);

    /* Delay */
    for(i=0; i<0x7FFFF; i++);

    UserButtonPressed++;

    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
  }
}

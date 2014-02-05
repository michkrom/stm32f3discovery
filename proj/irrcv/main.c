/**
 * IR decoder for SYMA S107G, PROTOCOL TraceJet and U810 HELIcopters remote codes.
 *
 * @author Michal Krombholz
 * @license GNU General Public License (GPL) v2 or later
 */
/* Includes ------------------------------------------------------------------*/

#include <stdio.h>

#include "board.h"

#include "leds.h"
#include "key.h"
#include "serial.h"
#include "irRcv.h"

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

void printbin(uint32_t val, int startbit, int bitcount)
{
    while( --bitcount >= 0 )
    {
        printf( (val >> (startbit ++) ) & 1  ? "1" : "0");
    }
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

    irRcvInit();

    printf("STM32F3DISCO IRRCV\r\n");
    printf("HCLK=%u\r\n",GetHClockFrequency());

    // loop forever and decode IR codes
    while( 1 )
    {
        unsigned cmd = irRcvGetCommand();
        if( cmd != 0 ) {
            unsigned th = cmd >> 16;
            unsigned tl = cmd & 0xFFFF;
            printf("%04x %04x ",th,tl);
#if 0
//          printf(" %b %b ",th,tl);
            printbin(cmd, 24, 8);
            printf(" ");
            printbin(cmd, 16, 8);
            printf(" ");
            printbin(cmd,  8, 8);
            printf(" ");
            printbin(cmd,  0, 8);
#else
            unsigned pwr = IR_CMD_PWR(cmd);
            unsigned lr = IR_CMD_LR(cmd);
            unsigned fb = IR_CMD_FB(cmd);
            unsigned ch =  IR_CMD_CH(cmd);
            unsigned trm = IR_CMD_TRM(cmd);
            unsigned sum = IR_CMD_SUM(cmd);
            unsigned sum2 = IR_CMD_COMPUTE_SUM(cmd);
            char btnL = IR_CMD_BTNL(cmd);
            char btnR = IR_CMD_BTNR(cmd);
            char btnM = IR_CMD_BTNM(cmd);

            printf(" pwr=%i",pwr);
            printf(" fb=%i lr=%i",fb-8, lr-8);
            printf(" ch=%i",ch);
            printf(" trm=%i",trm);
            printf(" btn=%i%i%i",btnL,btnM,btnR);
            printf(" sum=%X %X %s",sum,sum2, IR_CMD_VALID(cmd)?"OK":"err");
#endif
            printf("\r\n");
        }
        else
        {
            if( irRcvErrorCount > 10 )
            {
                printf("Err=%u\r\n",(unsigned)irRcvErrorCount);
                irRcvErrorCount = 0;
            }
        }
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


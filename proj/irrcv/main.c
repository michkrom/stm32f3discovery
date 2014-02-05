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
//          printf(" %b %b ",th,tl);
            printbin(cmd, 24, 8);
            printf(" ");
            printbin(cmd, 16, 8);
            printf(" ");
            printbin(cmd,  8, 8);
            printf(" ");
            printbin(cmd,  0, 8);
#if 0
#ifdef TRACERJET
            // power: 0-100 (127?); 128 - off; > 128 dynamic boost (seen 130ish to 228max only)
            unsigned pwr = IR_CMD_PWR(cmd);
            unsigned lr = IR_CMD_LR(cmd); // roll: left=15...right=1; neutral=8 (-7..+7)
            unsigned fb = IR_CMD_FB(cmd); // pitch: forward=15..backward=1; neutral=8 (-7..+7)

            // channel 0-A; 2-B; 3-C; (value of 1 missing)
            unsigned ch =  IR_CMD_CH(cmd); // 2 bits
            // trim
            unsigned trm = IR_CMD_TRM(cmd); // 6 bits
            // 6 bits of 3 first bytes sum (sic!) - kinda checksum (ignores upper 2 bits)
            unsigned sum = IR_CMD_SUM(cmd);
            // potentially the 2 upper bits of last byte are .5 ch (for two pushbuttons?)
            // looks like sbdy did not think right as 2 upper bits of each byte are ignored this way

            // sum computed on 3 bytes then only lower 6 bit retained
            unsigned sum2 = IR_CMD_COMPUTE_SUM(cmd);//((th>>8)+(th&0xFF)+(tl>>8)) & 0x3f;

            printf(" pwr=%i",pwr);
            printf(" fb=%i lr=%i",fb-8, lr-8);
            printf(" ch=%i",ch);
            printf(" trm=%i",trm);
            printf(" sum=%X %X %s",sum,sum2,IR_CMD_VALID(cmd)?"OK":"err");
#else // syma
            int ch =  (cmd >> (8+7)) & 0x1;
            int pwr = (cmd >> 8) & 0x7F; // 0..127
            int trm = (cmd >> 0) & 0x7F; // 0..127 (1..125 in practice)
            int unk1 = (cmd >> 7) & 0x1; // top bit in trm (unused)
            int lr =  (cmd >> 24) & 0xFF;
            int unk2 = (cmd >> 31) & 0x1; // top bit in lr (unused)
            int fb =  (cmd >> 16) & 0xFF; // 0..127 (f..b)
            int unk3 = (cmd >> 23) & 0x1; // top bit in fb (unused)
            // note trim offsets the lr when pwr>0
            // trim=63 (middle) ==> lr=63 (middle)
            // "untrimmed" lr value formula (keeps lr nutral between 61-65 - not perfect)
            int lr0 = lr - ((trm-63)/3);

            printf(" ch=%i",ch);
            printf(" pwr=%i",pwr);
            printf(" fb=%i lr=%i,%i",fb,lr,lr0);
            printf(" trm=%i",trm);
#endif
#endif
            printf("\r\n");
        }
        else
        {
            extern uint32_t irRcvErrorCount;
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


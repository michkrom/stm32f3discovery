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
            printf("%x %x",th,tl);
//          printf(" %b %b ",th,tl);

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
            printf("\r\n");
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


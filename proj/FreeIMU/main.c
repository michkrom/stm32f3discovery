/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "stm32f30x_rcc.h"

#include "leds.h"
#include "key.h"
#include "serial.h"
#include "imu_util.h"
#include "imu_devs.h"


extern void RunFreeIMUTests();

/**
* @brief  bump leds by one
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

/**
* @brief sample all sensors (blocking - will wait for all sensors to get new data
* @param a - out accelerometer data
* @param m - out magnetometer data
* @param g - out gyro data
* @retval None
*/
void sampleSensors(float a[], float m[], float g[])
{
  // status has lower 3 bits to remember which sensor are already read
  unsigned status=0;
  while( (status & 0x7) != 0x7 )
  {
    if( (status & 0x1) == 0 && (LSM303DLHC_AccGetDataStatus() & 0x8) != 0 )
    {
      ReadAccelerometer(a);
      status |= 0x1;
    }
    if( (status & 0x2) == 0 && (LSM303DLHC_MagGetDataStatus() & 0x1) != 0 )
    {
      ReadMagnetometer(m);
      status |= 0x2;
    }
    if( (status & 0x4) == 0 && (L3GD20_GetDataStatus() & 0x8) != 0 )
    {
      ReadGyro(g);
      status |= 0x4;
    }
  }
}


/**
* @brief	Main program.
* @param	None
* @retval None
*/
int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);

  /* SysTick end of count event each 1ms */
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

  //	InitSerial(56700);
  InitSerial(115200);
  printf("STM32F3DISCO console\r\n");

  InitKey();
  InitLeds();
  InitAccAndMag();
  InitGyro();
  
  //RunFreeIMUTests();
  
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

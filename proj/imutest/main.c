/**
	******************************************************************************
	* @file		main.c 
	* @author	MCD Application Team
	* @version V1.1.0
	* @date		20-September-2012
	* @brief	 Main program body
	******************************************************************************
	* @attention
	*
	* <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
	*
	* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
	* You may not use this file except in compliance with the License.
	* You may obtain a copy of the License at:
	*
	*				http://www.st.com/software_license_agreement_liberty_v2
	*
	* Unless required by applicable law or agreed to in writing, software 
	* distributed under the License is distributed on an "AS IS" BASIS, 
	* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	* See the License for the specific language governing permissions and
	* limitations under the License.
	*
	******************************************************************************
	*/


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "leds.h"
#include "key.h"
#include "serial.h"
#include "imu_util.h"
#include "imu_devs.h"
#include "demos.h"

#include "MadgwickAHRS.h"
//#include "MahonyAHRS.h"
#include "MadgwickFullAHRS.h"

// global IMU vars
float samplePeriod = 1.0f/200.0f;
// quaternion of sensor frame relative to auxiliary frame
float quaternion[4] = {0,0,0,-1.0f};

/**
	* @brief	Run all demos
	* @param	None
	* @retval None
	*/
void RunDemos()
{
	/* Infinite loop */
	while (1)
	{			 
		DemoLeds();
		
		SetLed(ALL|ON);
		Delay(50);
		SetLed(ALL|OFF);

		DemoAccelerometer();

		SetLed(ALL|ON);
		Delay(50);
		SetLed(ALL|OFF);
		
		DemoMagnetometer();

		SetLed(ALL|ON);
		Delay(5);
		SetLed(ALL|OFF);
		
		DemoGyro();
	}
}

/**
	* @brief	sample all sensors (blocking - will wait for all sensors to get new data)
	* @param	a - out accelerometer data
	* @param	m - out magnetometer data
	* @param	g - out gyro data
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


void RunImuTests()
{
	long unsigned tb;
	int outputcnt = 0;
	float xmax,xmin,ymax,ymin,zmax,zmin;
	float quaternion[4] = {1,0,0,0}; 
	
	SetLed(ALL|ON);
	printf("Zeroing gyro...");
	tb = SysTickCount;
	//ZeroGyro(100);
  printf("100 avg took %d ms\r\n",SysTickCount-tb);
	SetLed(ALL|OFF);
	
	
	// zero mag max cal data
	xmin=xmax=ymin=ymax=zmin=zmax=0;
	
	// Madgwick filter coeff (orig 0.3 but increased for faster initial convergence)
	madgwickBeta=0.5f;
	
	// initial sample timestamp
	tb = SysTickCount;		

  while(1)	
	{
		float a[3],m[3],g[3];
		float gdeg[3];
		long unsigned tb2;
		float samplePeriod;
		
		// read all the sensors
		sampleSensors(a,m,g);

		// get time of acquisition
		tb2 = SysTickCount;				
		
    // sample period is time elapsed since previous sampling of sensors		
		samplePeriod=0.001f*(tb2-tb);

		// update timebase for next time
		tb = tb2;

		#if 1
		
		// g is in degrees so convert to radians
		gdeg[0]=g[0];
		gdeg[1]=g[1];
		gdeg[2]=g[2];
		imuDegToRadV3(g);

		//g[0]=g[1]=g[2]=0;
		//MadgwickAHRSupdateIMU( g[0], g[1], g[2], a[0], a[1], a[2] );
		//MadgwickAHRSupdate( g, a, m, samplePeriod, quaternion );		

		MadgwickFullAHRSUpdate( g, a, m, samplePeriod, quaternion );		
		// it's < 1ms to compute the above!
		
		if( KeyPressCount() > 0 ) outputcnt ++;
		switch( outputcnt )
		{
			case 0: // YPR reading based on madgwick algorithm
			{
							float ypr[3];
							imuQuaternionToYawPitchRoll(quaternion,ypr);
							imuRadToDegV3(ypr);
				
				      printf("%4.0f,%4.0f,%4.0f,YPR %3dHz\r\n", ypr[0],ypr[1],ypr[2], (int)(1.0f/samplePeriod));
				
							//float gr[3];
							//imuQuaternionToGravity(quaternion,gr);
				      //printf("%4.0f,%4.0f,%4.0f,YPR %3dHz grav %5.2f %5.2f %5.2f\r\n", ypr[0],ypr[1],ypr[2], (int)(1.0f/samplePeriod), g[0],g[1],g[2]);
							break;
			}
			case 1: // accelerometer data
							printf("%7.3f,%7.3f,%7.3f,ACC\r\n", a[0], a[1], a[2]);
							break;
			case 2: // gyro data in radians
							{
								printf("%7.3f,%7.3f,%7.3f,GDEG\r\n", gdeg[0], gdeg[1], gdeg[2]);
								break;
			        }
			case 3: // gyro data in radians
				      printf("%7.3f,%7.3f,%7.3f,GRAD\r\n", g[0]*100, g[1]*100, g[2]*100);
					  	break;
			case 4: // magnetometer data + acquireing the cal maxs; wiggle the thing to measure/find min&max for each axle
			{
							printf("%7.3f,%7.3f,%7.3f,MAGx0.01Ga %7.3Ga\r\n", m[0]*100.0f, m[1]*100.0f, m[2]*100.0f, sqrtf(m[0]*m[0]+m[1]*m[1]+m[2]*m[2])); // 0.01Ga
				      // collect data for calibration
				      if( m[0] > xmax ) xmax=m[0];
				      if( m[1] > ymax ) ymax=m[1];
				      if( m[2] > zmax ) zmax=m[2];
				      if( m[0] < xmin ) xmin=m[0];
				      if( m[1] < ymin ) ymin=m[1];
				      if( m[2] < zmin ) zmin=m[2];			
							break;
			}
			case 5: // magentometer cal output
			{
								// NOMINAL is Earth magnetic field strength at given location 
				        // Ga==mT from http://www.ngdc.noaa.gov/geomag-web/#igrfwmm CA95409
				        const float NOMINAL = 0.49118;
								float xo,yo,zo;
								float xg,yg,zg;
				        // offsets (mid points)
								xo=(xmax+xmin)/2;
								yo=(ymax+ymin)/2;
								zo=(zmax+zmin)/2;
								// gains
				        xg=2*NOMINAL/(xmax-xmin);
				        yg=2*NOMINAL/(ymax-ymin);
				        zg=2*NOMINAL/(zmax-zmin);
				        
				        // subtract the offset, then multiply by gain
				
								printf("Mag cal (min:max): %5.3f,%5.3f %5.3f,%5.3f %5.3f,%5.3f\r\n",xmin,xmax,ymin,ymax,zmin,zmax);
								printf("ofs:gain %5.3f,%5.3f %5.3f,%5.3f %5.3f,%5.3f\r\n", xo,xg,yo,yg,zo,zg);
								printf("calibrated %5.3f,%5.3f %5.3f,%5.3f %5.3f,%5.3f\r\n", (xmin-xo)*xg, (xmax-xo)*xg, (ymin-yo)*yg, (ymax-yo)*yg, (zmin-zo)*zg, (zmax-zo)*zg);
								// cal is done put results to the readMag()
								Delay(100);
			        break;
			}
			case 6: // all sensor data
							printf("A %5.2f,%5.2f,%5.2f G %5.2f,%5.2f,%5.2f M %5.2f,%5.2f,%5.2f\r\n",
											a[0],a[1],a[2],
											g[0],g[1],g[2],
											m[0],m[1],m[2]);
							break;
			case 7: // quaternion
							printf("%6.3f,%6.3f,%6.3f,%6.3f,%3.0fHz,Q\r\n",
										 quaternion[0],quaternion[1],quaternion[2],quaternion[3],
										 1.0f/samplePeriod);
							break;
			case 8: // compute heading Yaw (tilt-corrected magn) and Pitch & Roll w/o filtering 
							{				       
							float au[3];
							float mu[3];
							au[0] = a[0];
							au[1] = a[1];
							au[2] = a[2];
  				    mu[0] = m[0];
  				    mu[1] = m[1];
  				    mu[2] = m[2];
							imuNormalizeV3(au);
							imuNormalizeV3(mu);
							printf("%4.0f,%4.0f,%4.0f,%4.0f,HDG+PR\r\n",
											RAD2DEG(imuHeading(mu[0],mu[1],mu[2])),
											RAD2DEG(imuHeadingTiltCompensated(mu[0],mu[1],mu[2],au[0],au[1],au[2])),
											RAD2DEG(imuPitch(au[0],au[1],au[2])),
											RAD2DEG(imuRoll(au[0],au[1],au[2]))
							      );
							}
							break;
			default: outputcnt=0;
		}
#endif		
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
	
	
#ifdef DEMOS	 
	RunDemos();
#else
	RunImuTests();
#endif
}

 
void _sys_exit(int return_code)
{
label:  goto label;  /* endless loop */
}
 
/**************************************************************************************/

#ifdef	USE_FULL_ASSERT

/**
	* @brief	Reports the name of the source file and the source line number
	*				 where the assert_param error has occurred.
	* @param	file: pointer to the source file name
	* @param	line: assert_param error line source number
	* @retval None
	*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	
	printf("ASSERT %s in %d\r\n",file,line);
}
#endif

/**
	* @}
	*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

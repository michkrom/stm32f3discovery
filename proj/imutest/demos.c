/**
	******************************************************************************
	* @file		demos.c 
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
#include "imu_devs.h"
#include "leds.h"
#include "key.h"

#include <math.h>

#ifndef M_PI
#define M_PI 3.1415927f
#endif // M_PI

#ifndef ABS
#define ABS(x)				 ((x < 0) ? (-x) : x)
#endif

float MagBuffer[3] = {0.0f}, AccBuffer[3] = {0.0f}, Buffer[3] = {0.0f};
float fNormAcc,fSinRoll,fCosRoll,fSinPitch,fCosPitch = 0.0f, RollAng = 0.0f, PitchAng = 0.0f;
float fTiltedX,fTiltedY = 0.0f;


//  fXg = Xg * alpha + (fXg * (1.0 - alpha));
//  fYg = Yg * alpha + (fYg * (1.0 - alpha));
//	fZg = Zg * alpha + (fZg * (1.0 - alpha));

void ComputePitchAndRollFromAcc(float Xg, float Yg, float Zg, float* pitch, float* roll)
{
    //Roll & Pitch Equations		
    *roll  = (atan2(-Yg, Zg)*180.0f)/M_PI;
    *pitch = (atan2(Xg, sqrt(Yg*Yg + Zg*Zg))*180.0f)/M_PI;
}

void ComputePitchAndRoll()
{	
			float fNormAcc = sqrtf((AccBuffer[0]*AccBuffer[0])+(AccBuffer[1]*AccBuffer[1])+(AccBuffer[2]*AccBuffer[2]));

			fSinRoll = -AccBuffer[1]/fNormAcc;
			fCosRoll = sqrtf(1.0f-(fSinRoll * fSinRoll));
			fSinPitch = AccBuffer[0]/fNormAcc;
			fCosPitch = sqrtf(1.0f-(fSinPitch * fSinPitch));
			if ( fSinRoll >0)
			{
				if (fCosRoll>0)
				{
					RollAng = ((float)acos(fCosRoll))*180.0f/M_PI;
				}
				else
				{
					RollAng = ((float)acos(fCosRoll))*180.0f/M_PI + 180.0F;
				}
			}
			else
			{
			 if (fCosRoll>0)
			 {
				 RollAng = acos(fCosRoll)*180/M_PI + 360;
			 }
			 else
			 {
				 RollAng = acos(fCosRoll)*180/M_PI + 180;
			 }
			}
		 
			if ( fSinPitch >0)
			{
			 if (fCosPitch>0)
			 {
					PitchAng = acos(fCosPitch)*180/M_PI;
			 }
			 else
			 {
					PitchAng = acos(fCosPitch)*180/M_PI + 180;
			 }
			}
			else
			{
			 if (fCosPitch>0)
			 {
					PitchAng = acos(fCosPitch)*180/M_PI + 360;
			 }
			 else
			 {
					PitchAng = acos(fCosPitch)*180/M_PI + 180;
			 }
			}

			if (RollAng >=360)
			{
				RollAng = RollAng - 360;
			}
			
			if (PitchAng >=360)
			{
				PitchAng = PitchAng - 360;
			}
}

/**************************************************************************************************/

void DemoLeds()
{				
		int d=0;
		SetLed(ALL|OFF);
	  /* while user button is not pressed */
		while (KeyPressCount()==0)
		{
			SetLed(L1|TOGGLE);
			Delay(5);
			SetLed(L2|TOGGLE);
			Delay(5);
			SetLed(L3|TOGGLE);
			Delay(5);
			SetLed(L4|TOGGLE);
			Delay(5);
			SetLed(L5|TOGGLE);
			Delay(5);
			SetLed(L6|TOGGLE);
			Delay(5);
			SetLed(L7|TOGGLE);
			Delay(5);
			SetLed(L8|TOGGLE);
			Delay(5);
			printf("%d\r\n",d++);
		}
		SetLed(ALL|OFF);
}


void DemoAccelerometer()
{
		float roll, pitch;
	  /* while user button is not pressed */
		while (KeyPressCount()==0)
		{
			ReadAccelerometer(AccBuffer);				
			ComputePitchAndRollFromAcc( AccBuffer[0], AccBuffer[1], AccBuffer[2], &pitch, &roll );
			printf("A: %7.2f %7.2f %7.2f PR=%7.2f,%7.2f\r\n", AccBuffer[0], AccBuffer[1], AccBuffer[2], pitch, roll  );
			SetLed(ALL|OFF);
			if ((fabs(roll) <= 70.0f) && (fabs(pitch) <= 70.0f))
			{
				if( pitch > 25 )
					SetLed(L1);
				if( pitch < -25 )
					SetLed(L5);
				if( roll > 25 )
					SetLed(L3);
				if( roll < -25 )
					SetLed(L7);
				if( pitch > 25 && roll > 25 )
					SetLed(L2);
				if( pitch < -25 && roll > 25 )
					SetLed(L8);
				if( pitch < -25 && roll < -25 )
					SetLed(L6);
				if( pitch < -25 && roll > 25 )
					SetLed(L4);
			}
			else
			{
				SetLed(ALL|TOGGLE);
				/* Delay 50ms */
				Delay(5);
			}
		}
}


void DemoGyro()
{
		uint8_t Xval, Yval = 0x00;
	
		/* Waiting User Button is pressed */
		while (KeyPressCount()==0)
		{
			/* Wait for data ready - Demo just wait for 5ms */
			Delay(5);
			
			/* LEDs Off */
			SetLed(ALL|OFF);
			
			/* Read Gyro Angular data */
			ReadGyro(Buffer);
				 
			/* Update autoreload and capture compare registers value*/
			Xval = ABS((int8_t)(Buffer[0]));
			Yval = ABS((int8_t)(Buffer[1])); 
			
			if ( Xval>Yval)
			{
				if ((int8_t)Buffer[0] > 5.0f)
				{ 
					SetLed(LED10);
				}
				if ((int8_t)Buffer[0] < -5.0f)
				{ 
					SetLed(LED3);
				}
			}
			else
			{
				if ((int8_t)Buffer[1] < -5.0f)
				{
					SetLed(LED6);
				}
				if ((int8_t)Buffer[1] > 5.0f)
				{
					SetLed(LED7);
				} 
			}
		}
		SetLed(ALL|OFF);
}


void DemoMagnetometer()
{
		__IO float HeadingValue = 0.0f;	
		int led=L1;
		
		/* Waiting User Button is pressed */
		while (KeyPressCount()==0)
		{
			/* Wait for data ready */
			Delay(5);

			/* Read Compass data */
			ReadMagnetometer(MagBuffer);

			ReadAccelerometer(AccBuffer);
				
			ComputePitchAndRoll();
				
			/* tilt correction */
			fTiltedX = MagBuffer[0]*fCosPitch+MagBuffer[2]*fSinPitch;
			fTiltedY = MagBuffer[0]*fSinRoll*fSinPitch+MagBuffer[1]*fCosRoll-MagBuffer[1]*fSinRoll*fCosPitch;
			
			HeadingValue = (float) ((atan2f((float)fTiltedY,(float)fTiltedX))*180)/M_PI;
 
			if (HeadingValue < 0)
			{
				HeadingValue = HeadingValue + 360;		
			}
			
			if ((RollAng <= 40.0f) && (PitchAng <= 40.0f))
			{
				if (((HeadingValue < 25.0f)&&(HeadingValue >= 0.0f))||((HeadingValue >=340.0f)&&(HeadingValue <= 360.0f)))
				{
					led=LED10;
				}
				else	if ((HeadingValue <70.0f)&&(HeadingValue >= 25.0f))
				{
					led=LED9;
				} 
				else	if ((HeadingValue < 115.0f)&&(HeadingValue >= 70.0f))
				{
					led=LED7;
				}
				else	if ((HeadingValue <160.0f)&&(HeadingValue >= 115.0f))
				{
					led=LED5;
				} 
				else	if ((HeadingValue <205.0f)&&(HeadingValue >= 160.0f))
				{
					led=LED3;
				} 
				else	if ((HeadingValue <250.0f)&&(HeadingValue >= 205.0f))
				{
					led=LED4;
				} 
				else	if ((HeadingValue < 295.0f)&&(HeadingValue >= 250.0f))
				{
					led=LED6;
				}				
				else	if ((HeadingValue < 340.0f)&&(HeadingValue >= 295.0f))
				{
					led=LED8;
				}
				SetLed(ALL|OFF);
				SetLed(led);
			}
			else
			{
				SetLed(ALL|TOGGLE);
				/* Delay 50ms */
				Delay(5);
			}
		}
}

/**
	******************************************************************************
	* @file		
	* @author	
	* @version
	* @date		
	* @brief	 Support for acc,gyr,mag of STM32F3DISCOVERY specific IMU devices
	******************************************************************************
	* @attention
	* aerospace orientation:
  * x-nose, y-right wing, z-to the ground 
	* right-hand rotation rules:
	* roll on the right wing (rtwng down) is positive x-rotation
	* nose going up is positive y-rotation
	* right turn (yaw) is positive z-rotation
	* the "nose" is the side with LEDs on the STM32F3DISCOVERY
	*
	******************************************************************************
	*/
#include "imu_devs.h"

#include "stm32f3_discovery_lsm303dlhc.h"
#include "stm32f3_discovery_l3gd20.h"


#ifndef ABS
#define ABS(x)				 (x < 0) ? (-x) : x
#endif

#define L3G_Sensitivity_250dps		(float)	114.285f			/*!< gyroscope sensitivity with 250 dps full scale [LSB/dps] */
#define L3G_Sensitivity_500dps		(float)	57.1429f			/*!< gyroscope sensitivity with 500 dps full scale [LSB/dps] */
#define L3G_Sensitivity_2000dps		(float)	14.285f				/*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps] */

#define LSM_Acc_Sensitivity_2g		(float)		1.0f					/*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g		(float)		0.5f					/*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g		(float)		0.25f				  /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g		(float)		0.0834f				/*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */


/**
  * @brief holds gyro bias calibration
	*/
int GyroBias[3] = {0};


/**
	* @brief	Configure the L3G20 gyro
	* @param	None
	* @retval None
	*/
void InitGyro(void)
{
	L3GD20_InitTypeDef L3GD20_InitStructure;
	L3GD20_FilterConfigTypeDef L3GD20_FilterStructure;
	
	/* Configure Mems L3GD20 */
	L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
	L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
	// DR=00 BW=01 ==> 190Hz HFPcutoff=12.5
	L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_2;
	L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_1;
	L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Continous;
	L3GD20_InitStructure.Endianness = L3GD20_BLE_LSB;
	L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_2000; 
	L3GD20_Init(&L3GD20_InitStructure);
	 
	L3GD20_FilterStructure.HighPassFilter_Mode_Selection =L3GD20_HPM_NORMAL_MODE_RES;
	L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;
	L3GD20_FilterConfig(&L3GD20_FilterStructure) ;
	
	L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);
}


/**
	* @brief	Calculate the angular Data rate Gyroscope.
	* @param	pfData : Data out pointer
	* @retval CTRL_REG4
	*/
uint8_t ReadGyroRaw(int16_t RawData[3])
{
	int i;
	uint8_t tmpreg = 0;
	uint8_t tmpbuffer[6] ={0};
	uint8_t gyrInBE;

	L3GD20_Read(&tmpreg,L3GD20_CTRL_REG4_ADDR,1);
	L3GD20_Read(tmpbuffer,L3GD20_OUT_X_L_ADDR,6);
	
	/* check in the control register 4 the data alignment (Big Endian or Little Endian)*/
  gyrInBE = tmpreg & 0x40 != 0;
	if(!gyrInBE)
	{
		for(i=0; i<3; i++)
		{
			RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i+1] << 8) + tmpbuffer[2*i]);
		}
	}
	else
	{
		for(i=0; i<3; i++)
		{
			RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i] << 8) + tmpbuffer[2*i+1]);
		}
	}
	return tmpreg;
}

/**
	* @brief	Average N gyro readings to zero out (assume steady)
	* @param	samples number of samples to average
	*/
void ZeroGyro(int samples)
{
	int i;
	GyroBias[0] = 0;
	GyroBias[1] = 0;
	GyroBias[2] = 0;
	for(i=0; i < samples; i++)
  {
		int16_t RawData[3] = {0};
		while((L3GD20_GetDataStatus() & 0x8) == 0);
		ReadGyroRaw(RawData);
		GyroBias[0] += RawData[0];
		GyroBias[1] += RawData[1];
		GyroBias[2] += RawData[2];
	}
	GyroBias[0] /= samples;
	GyroBias[1] /= samples;
	GyroBias[2] /= samples;
}

/**
	* @brief	Calculate the angular Data rate Gyroscope.
	* @param	pfData : Data out pointer (in deg/sec)
	* @retval None
	*/
void ReadGyro(float* pfData)
{
	int16_t RawData[3] = {0};
	uint8_t tmpreg = 0;
	float sensitivity = 0;

  tmpreg = ReadGyroRaw(RawData);
	RawData[0] -= GyroBias[0];
	RawData[1] -= GyroBias[1];
	RawData[2] -= GyroBias[2];

	/* Switch the sensitivity value set in the CRTL4 */
	switch(tmpreg & 0x30)
	{
	case 0x00:
		sensitivity=1.0f/L3G_Sensitivity_250dps;
		break;
		
	case 0x10:
		sensitivity=1.0f/L3G_Sensitivity_500dps;
		break;
		
	case 0x20:
		sensitivity=1.0f/L3G_Sensitivity_2000dps;
		break;
	}
	/* realign the gyro on STM32F3Discovery to ACC&MAG XYZ */	
	pfData[0] = -(float)RawData[1]*sensitivity;
	pfData[1] = -(float)RawData[0]*sensitivity;
	pfData[2] = -(float)RawData[2]*sensitivity;
}

/**
	* @brief	Configure the Mems LSM303DLHC - magnetometer & accelerometer
	* @param	None
	* @retval None
	*/
void InitAccAndMag(void)
{
	LSM303DLHCMag_InitTypeDef LSM303DLHC_InitStructure;
	LSM303DLHCAcc_InitTypeDef LSM303DLHCAcc_InitStructure;
	LSM303DLHCAcc_FilterConfigTypeDef LSM303DLHCFilter_InitStructure;
	
	/* Configure MEMS magnetometer main parameters: temp, working mode, full Scale and Data rate */
	LSM303DLHC_InitStructure.Temperature_Sensor = LSM303DLHC_TEMPSENSOR_DISABLE;
	LSM303DLHC_InitStructure.MagOutput_DataRate =LSM303DLHC_ODR_220_HZ; /* carefull with these consts, acc-vs-mag differ by data rate only */
	LSM303DLHC_InitStructure.MagFull_Scale = LSM303DLHC_FS_4_0_GA;
	LSM303DLHC_InitStructure.Working_Mode = LSM303DLHC_CONTINUOS_CONVERSION;
	LSM303DLHC_MagInit(&LSM303DLHC_InitStructure);
	
	 /* Fill the accelerometer structure */
	LSM303DLHCAcc_InitStructure.Power_Mode = LSM303DLHC_NORMAL_MODE;
	LSM303DLHCAcc_InitStructure.AccOutput_DataRate = LSM303DLHC_ODR_200_HZ; /* carefull with these consts, acc-vs-mag differ by data rate only */
	LSM303DLHCAcc_InitStructure.Axes_Enable= LSM303DLHC_AXES_ENABLE;
	LSM303DLHCAcc_InitStructure.AccFull_Scale = LSM303DLHC_FULLSCALE_2G;
	LSM303DLHCAcc_InitStructure.BlockData_Update = LSM303DLHC_BlockUpdate_Continous;
	LSM303DLHCAcc_InitStructure.Endianness=LSM303DLHC_BLE_LSB;
	LSM303DLHCAcc_InitStructure.High_Resolution=LSM303DLHC_HR_ENABLE;
	/* Configure the accelerometer main parameters */
	LSM303DLHC_AccInit(&LSM303DLHCAcc_InitStructure);
	
	/* Fill the accelerometer LPF structure */
	LSM303DLHCFilter_InitStructure.HighPassFilter_Mode_Selection =LSM303DLHC_HPM_NORMAL_MODE;
	LSM303DLHCFilter_InitStructure.HighPassFilter_CutOff_Frequency = LSM303DLHC_HPFCF_16;
	LSM303DLHCFilter_InitStructure.HighPassFilter_AOI1 = LSM303DLHC_HPF_AOI1_DISABLE;
	LSM303DLHCFilter_InitStructure.HighPassFilter_AOI2 = LSM303DLHC_HPF_AOI2_DISABLE;

	/* Configure the accelerometer LPF main parameters */
	LSM303DLHC_AccFilterConfig(&LSM303DLHCFilter_InitStructure);
}

/**
* @brief Read LSM303DLHC output register, and calculate the acceleration ACC=(1/SENSITIVITY)* (out_h*256+out_l)/16 (12 bit rappresentation)
* @param pnData: pointer to float buffer where to store data
* @retval None
*/
void ReadAccelerometer(float* pfData)
{
	int16_t pnRawData[3];
	uint8_t ctrlx[2];
	uint8_t accInFifo;
	uint8_t accInBE;
	uint8_t accSens;
	uint8_t buffer[6];
	int i;
	int shift;
	float SensitivityMultiply = .01f/LSM_Acc_Sensitivity_2g;
	
	/* Read the register content */
	LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);
	LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, buffer, 6);

	accInFifo = ctrlx[1] & 0x40 != 0;
  accInBE   = ctrlx[0] & 0x40 != 0;
	accSens   = ctrlx[0] & 0x30;
	
	/* FIFO mode get shifted by 6 (10 bits), normal is 12 bits */
	shift = accInFifo ? 6 : 4;
	
	if(!accInBE || accInFifo) /* Little Endian Mode or FIFO mode */
	{
		for(i=0; i<3; i++)
		{
			pnRawData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) + buffer[2*i]) >> shift;
		}
	}
	else /* Big Endian Mode */
	{
		for(i=0; i<3; i++)
		{
			pnRawData[i]=((int16_t)((uint16_t)buffer[2*i] << 8) + buffer[2*i+1]) >> shift;
		}
	}
	
	/* FIFO mode */
	if(accInFifo)
	{
		SensitivityMultiply = 1/0.25;
	}
	else
	{
		/* normal mode */
		/* switch the sensitivity value set in the CRTL4*/
		switch(accSens)
		{
		case LSM303DLHC_FULLSCALE_2G:
			SensitivityMultiply = .01f/LSM_Acc_Sensitivity_2g; // 1
			break;
		case LSM303DLHC_FULLSCALE_4G:
			SensitivityMultiply = .01f/LSM_Acc_Sensitivity_4g; // 2
			break;
		case LSM303DLHC_FULLSCALE_8G:
			SensitivityMultiply = .01f/LSM_Acc_Sensitivity_8g; // 4 
			break;
		case LSM303DLHC_FULLSCALE_16G:
			SensitivityMultiply = .01f/LSM_Acc_Sensitivity_16g; // 12
			break;
		}
	}

	/* Obtain the g value for the three axis */
	pfData[0]= -(float)(pnRawData[0]*SensitivityMultiply);
	pfData[1]= (float)(pnRawData[1]*SensitivityMultiply);
	pfData[2]= (float)(pnRawData[2]*SensitivityMultiply);
}

// magnetomiter calibration - performed @ CA95409 2013/4/8
float magXofs=0.032;
float magXgain=1.040;
float magYofs=0.046;
float magYgain=0.951;
float magZofs=0.085;
float magZgain=0.889;

/**
	* @brief	calculate the magnetic field Magn.
	* @param	pfData: pointer to the data out
	* @retval None
	*/
void ReadMagnetometer(float* pfData)
{
	uint8_t buffer[6];
	uint8_t CTRLB = 0;
	
	float Magn_Sensitivity_XY = 0, Magn_Sensitivity_Z = 0;

	/* TODO - it's wastefull to read back the REG_M just to learn sensitivity - we should remember it in a var */
	LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_CRB_REG_M, &CTRLB, 1);
	
	/* Read them all - note that Y & Z are swapped in register order: X,Z,Y */
	LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_H_M, buffer, 6);
	
	/* Switch the sensitivity set in the CRTLB*/
	switch(CTRLB & 0xE0)
	{
	case LSM303DLHC_FS_1_3_GA:
		Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_1_3Ga;
		Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_1_3Ga;
		break;
	case LSM303DLHC_FS_1_9_GA:
		Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_1_9Ga;
		Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_1_9Ga;
		break;
	case LSM303DLHC_FS_2_5_GA:
		Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_2_5Ga;
		Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_2_5Ga;
		break;
	case LSM303DLHC_FS_4_0_GA:
		Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_4Ga;
		Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_4Ga;
		break;
	case LSM303DLHC_FS_4_7_GA:
		Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_4_7Ga;
		Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_4_7Ga;
		break;
	case LSM303DLHC_FS_5_6_GA:
		Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_5_6Ga;
		Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_5_6Ga;
		break;
	case LSM303DLHC_FS_8_1_GA:
		Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_8_1Ga;
		Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_8_1Ga;
		break;
	}
	/* convert to gauss; note register order swap: X,Z,Y */
	/* also applying calibrations to compensate for mag sensitivity variance in x,y,z */
  pfData[0]=  (float)((int16_t)(((uint16_t)buffer[0] << 8)|buffer[1]))/Magn_Sensitivity_XY;
	pfData[1]= -(float)((int16_t)(((uint16_t)buffer[4] << 8)|buffer[5]))/Magn_Sensitivity_XY;
  pfData[2]= -(float)((int16_t)(((uint16_t)buffer[2] << 8)|buffer[3]))/Magn_Sensitivity_Z;
	
	pfData[0]=(pfData[0]-magXofs)*magXgain;
	pfData[1]=(pfData[1]-magYofs)*magYgain;
	pfData[2]=(pfData[2]-magZofs)*magZgain;
}

/**
	* @brief	Basic management of the timeout situation.
	* @param	None.
	* @retval None.
	*/
uint32_t LSM303DLHC_TIMEOUT_UserCallback(void)
{
	return 0;
}

/**
	* @brief	Basic management of the timeout situation.
	* @param	None.
	* @retval None.
	*/
uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
	return 0;
}

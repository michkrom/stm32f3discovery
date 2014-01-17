#ifndef _IMUDEVS_H
#define _IMUDEVS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
	* @brief	Configure the L3G20 gyro
	* @param	None
	* @retval None
	*/
void InitGyro(void);

/**
	* @brief	Calculate the angular Data rate Gyroscope.
	* @param	pfData : Data out pointer
	* @retval None
	*/
void ReadGyro(float* pfData);

/**
	* @brief	Average N gyro readings to zero out (assume steady)
	* @param	samples: number of samples to average
	*/
void ZeroGyro(int samples);


/**
	* @brief	Configure the Mems LSM303DLHC - magnetometer & accelerometer
	* @param	None
	* @retval None
	*/
void InitAccAndMag(void);

/**
* @brief Read acceleration from LSM303DLHC
* @param pfData: pointer to float buffer where to store data
* @retval None
*/
void ReadAccelerometer(float* pfData);

/**
	* @brief	Read magnetometer data from LSM303DLHC
	* @param	pfData: pointer to the data out
	* @retval None
	*/
void ReadMagnetometer(float* pfData);

#ifdef __cplusplus
}
#endif

#endif

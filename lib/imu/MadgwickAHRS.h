//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h

//---------------------------------------------------------------------------------------------------
// Definitions/defaults

#define betaDef		0.1f		// 2 * proportional gain


//----------------------------------------------------------------------------------------------------
// Variable declaration

extern float madgwickBeta;				  // algorithm gain

//---------------------------------------------------------------------------------------------------
// Function declarations

void MadgwickAHRSupdate(float g[3], float a[3], float m[3], float samplePeriod, float quaternion[4]);
void MadgwickAHRSupdateIMU(float g[3], float a[3], float samplePeriod, float quaternion[4]);

#endif
//=====================================================================================================
// End of file
//=====================================================================================================

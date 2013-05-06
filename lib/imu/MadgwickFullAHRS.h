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
#ifndef MadgwickFullAHRS_h
#define MadgwickFullAHRS_h

//---------------------------------------------------------------------------------------------------
// Function declarations

void MadgwickFullAHRSUpdate(float g[3], float a[3], float m[3], float samplePeriod, float quaternion[4]);

#endif
//=====================================================================================================
// End of file
//=====================================================================================================

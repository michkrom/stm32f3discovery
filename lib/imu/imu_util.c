#include "imu_util.h"
#include <math.h>

// Converts quaternion to Euler angles in radians defined with the Aerospace sequence.
// result in radians in order of Roll, Pitch, Yaw
void imuQuaternionToEulerAerospace(float q[4], float angles[3]) 
{
    angles[0] = atan2f(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1); // psi
    angles[1] = -asinf(2 * q[1] * q[3] + 2 * q[0] * q[2]); // theta
    angles[2] = atan2f(2 * q[2] * q[3] - 2 * q[0] * q[1], 2 * q[0] * q[0] + 2 * q[3] * q[3] - 1); // phi
}


// Converts quaternion to Euler angles
// from: http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
// result in radians in order of Roll, Pitch, Yaw
void imuQuaternionToEuler(float q[4], float angles[3]) 
{
    angles[0] = atan2f(2 * (q[0] * q[1] - q[2] * q[3]), 1 - 2 * (q[1]*q[1] + q[2] * q[2]) ); // phi
    angles[1] =  asinf(2 * (q[0] * q[2] + q[3] * q[1])); // theta
    angles[2] = atan2f(2 * (q[0] * q[3] - q[1] * q[2]), 1 - 2 * (q[2] * q[2] + q[3] * q[3])); // psi
}


// Converts quaternion to Y/P/R angles
// result in radians
// *** not correct!
void imuQuaternionToGravity(float q[4], float g[3])
{
	  // gravity vector (not aerospace sequence)
    g[0] = 2 * (q[1]*q[3] - q[0]*q[2]);
    g[1] = 2 * (q[0]*q[1] + q[2]*q[3]);
    g[2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
}

#if 0
// Converts quaternion to Y/P/R angles
// result in radians
// alternate - not verified
void imuQuaternionToYawPitchRoll2(float q[4], float ypr[3])
{
	  // gravity vector (not aerospace sequence)
    float gx, gy, gz;

    gx = 2 * (q[1]*q[3] - q[0]*q[2]);
    gy = 2 * (q[0]*q[1] + q[2]*q[3]);
    gz = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
	
    // yaw
    ypr[0] = atan2f(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1);
	  // roll
    ypr[1] = atanf(gx / sqrt(gy*gy + gz*gz));
	  // pitch
    ypr[2] = atanf(gy / sqrt(gx*gx + gz*gz));
}
#endif

// quaternion to YPR in aerospace sequence
// based on wikipedia: 
// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
void imuQuaternionToYawPitchRoll(float q[4], float ypr[3]) 
{
	  // yaw
    ypr[0] = atan2f(2*(q[0]*q[3]+q[1]*q[2]), 1-2*(q[2]*q[2]+q[3]*q[3]));
	  // pitch
    ypr[1] = asinf(2*(q[0]*q[2]-q[3]*q[1]));
		// roll
    ypr[2] = atan2f(2*(q[0]*q[1]-q[2]*q[3]), 1-2*(q[1]*q[1]+q[2]*q[2]));
}


// based on Freescale AN3461
// computes Pitch from given gravity vector
// aerospace sequence (nose up is positive pitch)
float imuPitch(float ax, float ay, float az)
{
    return -atan2f(ax, sqrtf(ay*ay + az*az));
}

// based on Freescale AN3461
// computes Roll from given gravity vector
// aerospace sequence (right wing down is positive roll)
float imuRoll(float ax, float ay, float az)
{
		return atan2f(ay, az);
}


// heading from magnetic field vector
float imuHeading(float mx, float my, float mz)
{
   float heading = atan2f(my, mx);
   return heading;
}


// computes heading given mag and acc readings (tilt-compensated compass calculation)
// assumes all m* and a* are normalized
// based on: https://www.loveelectronics.co.uk/Tutorials/13/tilt-compensated-compass-arduino-tutorial
float imuHeadingTiltCompensated(float mx, float my, float mz, float ax, float ay, float az)
{
	// follow aviation rules: pitch is positive up, roll is positive in right bank (right wing down)
  float roll = imuRoll(ax,ay,az);
  float pitch = imuPitch(ax,ay,az);
	    
  float cosRoll = cosf(roll);
  float sinRoll = sinf(roll);  
  float cosPitch = cosf(pitch);
  float sinPitch = sinf(pitch);
  
  // compensate for tilt and progect to a level plane
  float xh = mx * cosPitch + mz * sinPitch;
  float yh = mx * sinRoll * sinPitch + my * cosRoll - mz * sinRoll * cosPitch;
  
  float heading = atan2f(yh, xh);
    
  // We cannot correct for tilt over 40 degrees with this alg, if the board is tilted as such, return 0.
  if(roll > 0.78f || roll < -0.78f || pitch > 0.78f || pitch < -0.78f)
  {
    heading = 0;
  }
	
  return heading;
}

// Normalize vector of 3 floats to have magnitude of 1
void imuNormalizeV3(float v[3])
{
	float r = (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	r = (1.0f / sqrtf(r));
	v[0] *= r;
	v[1] *= r;
	v[2] *= r;	
}

// Converts vector of 3 values from radians to degrees; 
// result in place (vector is modified
void imuRadToDegV3(float* v3)
{
	v3[0] = RAD2DEG(v3[0]);
	v3[1] = RAD2DEG(v3[1]);
	v3[2] = RAD2DEG(v3[2]);
}


// Converts vector of 3 values from radians to degrees; 
// result in place (vector is modified
void imuDegToRadV3(float* v3)
{
	v3[0] = DEG2RAD(v3[0]);
	v3[1] = DEG2RAD(v3[1]);
	v3[2] = DEG2RAD(v3[2]);
}

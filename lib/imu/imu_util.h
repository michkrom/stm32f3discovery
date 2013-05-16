#ifndef IMU_H_
#define IMU_H_

#ifndef M_PI
#define M_PI 3.1415927f
#endif // M_PI

#define DEG2RAD(d)   ((d)*(M_PI/180.0f))
#define RAD2DEG(r)   ((r)*(180.0f/M_PI))

void imuQuaternionToEuler(float q[4], float angles[3]);
void imuQuaternionToYawPitchRoll(float q[4], float ypr[3]);
void imuQuaternionToGravity(float q[4], float g[3]);

float imuHeadingTiltCompensated(float mx, float my, float mz, float ax, float ay, float az);
void  imuMagneticVectorToEarthFrame( /*in*/ float m[3], /*in*/ float q[4], /*out*/ float h[3] );

float imuHeading(float mx, float my, float mz);
float imuRoll(float ax, float ay, float az);
float imuPitch(float ax, float ay, float az);


void imuRadToDegV3( float v[3] );
void imuDegToRadV3( float v[3] );
void imuNormalizeV3( float v[3] );


#endif

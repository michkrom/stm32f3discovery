//---------------------------------------------------------------------------------------------------
// Inverse Square-Root
// can use native computation or a "fast" method that is not faster when CPU uses fp coprocessor

#ifndef FAST_INV_SQRT

//float invSqrt(float x) { return 1.0f/sqrtf(x); }
#include <math.h>
#define invSqrt(x) (1.0f/sqrtf(x))

#else

// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

float invSqrt(float x) 
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

#endif

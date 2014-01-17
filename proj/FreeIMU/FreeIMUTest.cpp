#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>

#include "FreeIMU.h"

extern "C" void RunFreeIMUTest()
{
  FreeIMU fimu;
  fimu.init();
  while(1)
  {
    float ypr[3];
    fimu.getYawPitchRoll(ypr);
    printf("%f,%f,%f\n",ypr[0],ypr[1],ypr[2]);
  }
}

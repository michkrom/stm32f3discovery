imutest

A test project for imu devices on stm32f3discovery, namely gyro, magnetomiter and accelerometer.

Inlcudes 
- routines to initialize system, clocks, serial port, test button, leds and imu devices
- routines to read devices' measument in correct aerospace order
- gyro bias calibration/removal
- test suite for step output through different test
- Madgwick algorithms for splicing all sensors and outputing quaternions

The project builds with uVision4 (MDK-ARM) under windows.

The project contains parts of ARM's CSIMS and STMicro support libraries.

All parts are licensed with their corresponding licence.

Parts created by me are licensed as 

Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0) 
http://creativecommons.org/licenses/by-sa/3.0/
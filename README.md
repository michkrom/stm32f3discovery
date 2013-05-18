STM32F3discovery
=================
  
Projects related to the stm32f3discovery board from STMicro.  

The board is a great little (ok, medium size) ARM STM32F3 with full IMU gear: gyro, accelerometer and magnetometer, led circle, pushbutton, USB OTG/device and on-board USB debugger

Build system based on Keil uVision and gcc/makefiles.

Projects/libraries
- imu/ahrs fully fused algs with quaternion output and set of conversion functions into yaw/pitch/roll, euler etc.
- complete template project for gcc/makefile


File contents
---------------

- lib - set of libraries for overall support of the board
    - board - stm32f3discovery board specific files and utilities
            - serial port init
            - leds control
            - button control
    - imulib - library of imu related functions
             - Madgwick filter fusion algs (adapted/optimized)
             - on-board IMU devices read/initialize in correct aerospace order (optimized)
             - set of IMU/AHRS helper functions
    - CMSIS - ARM CORTEX M3 support definitions
    - STM32F30x_StdPeriph_Driver - CPU support package for STM32F30x
    - STM32_USB-FS-Device_Driver - CPU support package for USB endpoint

- proj - set of projects based on this board
    - imutest - imu devices tests includes     
        - Madgwick fusion filter algs
        - corrections/cal algs
        - imu conversions library
		- test/demo with serial output
- extra - utilities for programming and starting gdb/ocd, linker scripts

Note
---------------
This works contains parts of ARM's CSIMS and STMicro support libraries and other related works by multiple authors.

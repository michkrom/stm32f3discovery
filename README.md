#stm32f3discovery cashe

Projects related to the stm32f3discovery board from STMicro.
Build system based on Keil uVision and gcc/makefiles.

##File content:
*lib - set of libraries for overall support of the board
    *CMSIS - ARM CORTEX M3 support definitions
        * STM32F30x_StdPeriph_Driver - CPU support package for STM32F30x
        * STM32_USB-FS-Device_Driver - CPU support package for USB endpoint
    * board - stm32f3discovery board specific files and utilities
    * imulib - library of imu related functions
*proj - set of projects based on this board
    * imutest - imu devices tests includes 
	    * Madgwick fusion filter algs
		* corrections/cal algs
		* imu conversions library
		* test/demo with serial output
*extra - utilities for programming and starting gdb/ocd, linker scripts

###Note
This works contains parts of ARM's CSIMS and STMicro support libraries and other related works by multiple authors.

# put your *.o targets here, make should handle the rest!
SRCS = main.c FreeIMU.cpp FreeIMUTest.cpp

# lib root
LIB=../../lib

# specific libs
BRD=$(LIB)/board
IMU=$(LIB)/imu

# additional includes
INCL=-I $(BRD) -I $(IMU)

# board library
SRCS += \
 $(BRD)/hw_config.c \
 $(BRD)/key.c \
 $(BRD)/serial.c \
 $(BRD)/stm32f3_discovery.c \
 $(BRD)/stm32f3_discovery_lsm303dlhc.c \
 $(BRD)/imu_devs.c \
 $(BRD)/leds.c \
 $(BRD)/stm32f30x_it.c \
 $(BRD)/stm32f3_discovery_l3gd20.c \
 $(BRD)/syscalls.c \
 $(BRD)/system_stm32f30x.c

SSRCS += \
 $(BRD)/startup_stm32f30x.s

 # imu library
SRCS += \
 $(IMU)/imu_util.c \

# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)
PROJ_NAME=FreeIMUtest

# Location of the linker scripts
LDSCRIPT_INC=$(BRD)/ldscripts

# location of OpenOCD Board .cfg files (only used with 'make program')
OPENOCD_BOARD_DIR=board

CPU=stm32f3

# misc config files
EXTRA=../../extra

# Configuration (cfg) file containing programming directives for OpenOCD
OPENOCD_PROC_FILE=$(EXTRA)/$(CPU)-openocd.cfg

# GDB startup file for "debug" target
GDB_CMDS_FILE=$(EXTRA)/start.gdb

# that's it, no need to change anything below this line!

###################################################

# upper case CPU used to access CMIS
CPUx=`echo $(CPU) | tr '[:lower:]' '[:upper:]'`

CC=arm-none-eabi-gcc
CPP=arm-none-eabi-g++
GDB=arm-none-eabi-gdb
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump
SIZE=arm-none-eabi-size

CFLAGS  = -Wall -g -Os
CFLAGS += -mlittle-endian -mcpu=cortex-m4 -mthumb

# floating point options
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant

CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS = $(CFLAGS)
LDLAGS += -Wl,--gc-sections -Wl,-Map=$(PROJ_NAME).map

###################################################

vpath %.c src
vpath %.a $(STD_PERIPH_LIB)

ROOT=$(shell pwd)

CFLAGS += -I . -I $(LIB) $(INCL)

STD_PERIPH_LIB=$(LIB)/$(CPUx)0x_StdPeriph_Driver

CFLAGS += -I $(STD_PERIPH_LIB)/inc
CFLAGS += -I $(LIB)/CMSIS/Include
CFLAGS += -I $(LIB)/CMSIS/Device/ST/$(CPUx)0x/Include

CPPFLAGS += $(CFLAGS)

#CC_SOURCES :=  $(filter %.cc, $(SRCS))
#CPP_SOURCES := $(filter %.cpp, $(SRCS))

OBJS:=$(addsuffix .o,$(basename $(SRCS)))
OBJS+=$(addsuffix .o,$(basename $(SSRCS)))

###################################################

.PHONY: lib proj

all: lib proj

lib:
	@echo BUILDING $(STD_PERIPH_LIB)
	$(MAKE) -C $(STD_PERIPH_LIB)

proj: 	$(PROJ_NAME).elf $(PROJ_NAME).bin $(PROJ_NAME).hex $(PROJ_NAME).lst


.s.o :
	@echo $^
	$(CC) $(CFLAGS) $^ -o $@  -std=c99 -c 

.c.o :
	@echo $^
	$(CC) $(CFLAGS) $^ -o $@  -std=c99 -c 
	
.cpp.o :
	@echo $^
	$(CPP) $(CPPFLAGS) $^ -o $@ -c

	
$(PROJ_NAME).elf: $(OBJS)
	@echo "Linking...."
	$(CC) $(OBJS) $(LDFLAGS) -L$(STD_PERIPH_LIB) -l$(CPU) -L$(LDSCRIPT_INC) -T$(CPU).ld -o $@
	#$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	#$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
	#$(OBJDUMP) -St $(PROJ_NAME).elf >$(PROJ_NAME).lst
	$(SIZE) $(PROJ_NAME).elf

$(PROJ_NAME).bin: $(PROJ_NAME).elf
	$(OBJCOPY) -O binary $< $@

$(PROJ_NAME).hex: $(PROJ_NAME).elf
	$(OBJCOPY) -O ihex $< $@

$(PROJ_NAME).lst: $(PROJ_NAME).elf
	$(OBJDUMP) -St $< >$@

program: $(PROJ_NAME).bin
	skill openocd
	openocd -f $(OPENOCD_BOARD_DIR)/$(CPU)discovery.cfg -f $(OPENOCD_PROC_FILE) -c "stm_flash `pwd`/$(PROJ_NAME).bin" -c shutdown

run: $(PROJ_NAME).bin
	skill openocd
	openocd -f $(OPENOCD_BOARD_DIR)/$(CPU)discovery.cfg -f $(OPENOCD_PROC_FILE) -c "stm_flash `pwd`/$(PROJ_NAME).bin;reset run" -c shutdown

startocd:
	skill openocd
	openocd -f $(OPENOCD_BOARD_DIR)/$(CPU)discovery.cfg -f $(OPENOCD_PROC_FILE) &


debug: $(PROJ_NAME).elf
	$(MAKE) startocd
	$(GDB) -x $(GDB_CMDS_FILE) $<

debugt: $(PROJ_NAME).elf
	$(MAKE) startocd
	$(GDB)tui -x $(GDB_CMDS_FILE) $<

ddd: $(PROJ_NAME).elf
	$(MAKE) startocd
	ddd --eval-command="target remote localhost:3333" --debugger $(GDB) -x $(GDB_CMDS_FILE) $<

clean:
	rm -f $(OBJS)
	find ./ -name '*~' | xargs rm -f
	rm -f *.o
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
	rm -f $(PROJ_NAME).map
	rm -f $(PROJ_NAME).lst

reallyclean: clean
	$(MAKE) -C $(STD_PERIPH_LIB) clean

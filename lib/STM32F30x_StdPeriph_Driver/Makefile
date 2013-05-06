CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar

###########################################

vpath %.c STM32F30x_StdPeriph_Driver/src

CFLAGS  = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m0 -march=armv6s-m
CFLAGS += -ffreestanding -nostdlib
CFLAGS += -includestm32f30x_conf.h -I../CMSIS/Include -I../CMSIS/Device/ST/STM32F30x/Include -Iinc -I..


SRCS = src/stm32f30x_adc.c     src/stm32f30x_dma.c    src/stm32f30x_misc.c   src/stm32f30x_syscfg.c \
       src/stm32f30x_can.c     src/stm32f30x_exti.c   src/stm32f30x_opamp.c  src/stm32f30x_tim.c \
       src/stm32f30x_comp.c    src/stm32f30x_flash.c  src/stm32f30x_pwr.c    src/stm32f30x_usart.c \
       src/stm32f30x_crc.c     src/stm32f30x_gpio.c   src/stm32f30x_rcc.c    src/stm32f30x_wwdg.c \
       src/stm32f30x_dac.c     src/stm32f30x_i2c.c    src/stm32f30x_rtc.c \
       src/stm32f30x_dbgmcu.c  src/stm32f30x_iwdg.c   src/stm32f30x_spi.c 


OBJS = $(SRCS:.c=.o)

.PHONY: libstm32f3.a

all: libstm32f3.a

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

libstm32f3.a: $(OBJS)
	$(AR) -r $@ $(OBJS)

clean:
	rm -f $(OBJS) libstm32f3.a

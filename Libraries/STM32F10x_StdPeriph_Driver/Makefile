CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar

###########################################

vpath %.c src

CFLAGS  = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m0 -march=armv6s-m
CFLAGS += -ffreestanding -nostdlib -DUSE_STDPERIPH_DRIVER  -DSTM32F10X_MD
CFLAGS += -I../CMSIS/Include -I../CMSIS/Device/ST/STM32F10x/Include -Iinc

SRCS = misc.c \
	stm32f10x_adc.c \
	stm32f10x_bkp.c \
	stm32f10x_can.c \
	stm32f10x_cec.c \
	stm32f10x_crc.c \
	stm32f10x_dac.c \
	stm32f10x_dbgmcu.c \
	stm32f10x_dma.c \
	stm32f10x_exti.c \
	stm32f10x_flash.c \
	stm32f10x_fsmc.c \
	stm32f10x_gpio.c \
	stm32f10x_i2c.c \
	stm32f10x_iwdg.c \
	stm32f10x_pwr.c \
	stm32f10x_rcc.c \
	stm32f10x_rtc.c \
	stm32f10x_sdio.c \
	stm32f10x_spi.c \
	stm32f10x_tim.c \
	stm32f10x_usart.c \
	stm32f10x_wwdg.c

OBJS = $(SRCS:.c=.o)

.PHONY: libstm32f10x.a

all: libstm32f10x.a

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

libstm32f10x.a: $(OBJS)
	$(AR) -r $@ $(OBJS)

clean:
	rm -f $(OBJS) libstm32f10x.a

CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar

###########################################

vpath %.c src

CFLAGS  = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m0 -march=armv6s-m
CFLAGS += -ffreestanding -nostdlib -DUSE_STDPERIPH_DRIVER -DSTM32L1XX_XL
CFLAGS += -I../CMSIS/Include -I../CMSIS/Device/ST/STM32L1xx/Include -Iinc

STM32L1XXSRCS = stm32l1xx_adc.c stm32l1xx_comp.c stm32l1xx_crc.c \
	stm32l1xx_dac.c stm32l1xx_dbgmcu.c stm32l1xx_dma.c stm32l1xx_exti.c \
	stm32l1xx_flash.c stm32l1xx_gpio.c stm32l1xx_i2c.c stm32l1xx_iwdg.c \
	misc.c stm32l1xx_pwr.c stm32l1xx_rcc.c stm32l1xx_rtc.c \
	stm32l1xx_spi.c stm32l1xx_syscfg.c stm32l1xx_tim.c stm32l1xx_lcd.c \
	stm32l1xx_usart.c stm32l1xx_wwdg.c

STM32L1XXOBJS = $(STM32L1XXSRCS:.c=.o)

.PHONY: libstm32l1.a

all: libstm32l1.a

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

libstm32l1.a: $(STM32L1XXOBJS)
	$(AR) -r $@ $(STM32L1XXOBJS)

clean:
	rm -f $(STM32L1XXOBJS) libstm32l1.a

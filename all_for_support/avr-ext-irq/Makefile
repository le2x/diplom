# Задаем имя проекта, в результате получатся файлы test.hex test.bin
TARG=ext-irq

CC = avr-gcc
OBJCOPY = avr-objcopy

# Задаем из каких файлов собирать проект, можно указать несколько файлов
SRCS= ext_irq.c 

# Из каждого файла с расширением .c будем делать объектный файл
OBJS = $(SRCS:.c=.o)

# Задаем для какого микроконтроллера будем компилировать 
MCU=atmega8

# Флаги компилятора, при помощи F_CPU определяем частоту на которой будет
# работать контроллер,
CFLAGS = -mmcu=$(MCU) -Wall -g -Os -Werror -lm  -mcall-prologues  -DF_CPU=7372800
LDFLAGS = -mmcu=$(MCU)  -Wall -g -Os  -Werror 

all: $(TARG)

$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@.elf  $(OBJS) -lm
	$(OBJCOPY) -O binary -R .eeprom -R .nwram  $@.elf $@.bin
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram  $@.elf $@.hex

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.elf *.bin *.hex $(OBJS) *.map

reflash: $(TARG)	
	avrdude  -P /dev/ttyUSB0 -B 2 -c stk500v2 -pm8 -e  -U flash:w:$(TARG).hex

	

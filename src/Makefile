CC 	= avr-gcc
OBJCOPY	= avr-objcopy
OBJDUMP	= avr-objdump
SIZE 	= avr-size
AVRDUDE = avrdude


TARGET = muos
MCU = atmega328p

SOURCES = $(TARGET).c
#SOURCES +=

#AVRDUDE_PROGRAMMER = -c usbasp
AVRDUDE_PROGRAMMER = -c arduino
AVRDUDE_BAUDRATE = -b 57600
AVRDUDE_PORT = -P /dev/ttyUSB0

AVRDUDE_PART = -p $(MCU)

AVRDUDE_FLAGS = $(AVRDUDE_PROGRAMMER) $(AVRDUDE_PART) $(AVRDUDE_BAUDRATE) $(AVRDUDE_PORT)
AVRDUDE_DEFAULT_SCK = -B 8
AVRDUDE_128kHz_SCK = -B 16

CPPFLAGS = -I .

CFLAGS = -std=gnu99 -mmcu=$(MCU) -g
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -Os
CFLAGS += -mtiny-stack
CFLAGS += -fshort-enums
CFLAGS += -flto
CFLAGS += -falign-functions=2
CFLAGS += $(CPPFLAGS)

DEPFLAGS = -M -MP

LDFLAGS += -mmcu=$(MCU) -Wl,--relax,--gc-sections

FUSE_DEFAULT = -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
#FUSE_RELEASE = -U lfuse:w:0x64:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

all: $(TARGET).hex $(TARGET).eep size

%.d: %.c
	@set -e; rm -f $@; \
        $(CC) $(DEPFLAGS) $(CPPFLAGS) $< > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
        rm -f $@.$$$$

-include $(SOURCES:.c=.d)

%.o %d: %c

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
asm: $(TARGET).asm

show_asm: $(TARGET).asm
	less $(TARGET).asm


%.elf: $(SOURCES:.c=.o) Makefile
	$(CC) $(CFLAGS) $(SOURCES:.c=.o) --output $@ $(LDFLAGS)

%.hex: %.elf
	$(OBJCOPY) -O ihex -j .text $< $@

%.eep: %.elf
	$(OBJCOPY) -j .eeprom  --change-section-lma .eeprom=0 -O ihex $< $@

%.asm: %.elf
	$(OBJDUMP) -S $< >$@

size: $(TARGET).hex $(TARGET).eep
	@$(SIZE) --target=ihex $(TARGET).hex $(TARGET).eep


program: all
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -U flash:w:$(TARGET).hex:i
#-U eeprom:w:$(TARGET).eep:i

read_program:
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -U flash:r:$(TARGET).hex:i -U eeprom:r:$(TARGET).eep:i

reset_fuse:
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_128kHz_SCK) $(FUSE_DEFAULT)

#fuse_128khz:
#	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) $(FUSE_RELEASE)

reset_device:
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -e $(FUSE_DEFAULT) || \
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_128kHz_SCK) -e $(FUSE_DEFAULT)


clean:
	rm -rf *.elf *.a *.o *.d *.d.*

mrproper: clean
	rm -rf *.hex *.eep



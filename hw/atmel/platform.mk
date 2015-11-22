CC 	= avr-gcc
OBJCOPY	= avr-objcopy
OBJDUMP	= avr-objdump
SIZE 	= avr-size
AVRDUDE = avrdude

CCFLAGS += -mmcu=$(MCU)
CCFLAGS += -Os
CCFLAGS += -mtiny-stack
CCFLAGS += -fshort-enums
CCFLAGS += -flto
#CCFLAGS += -mcall-prologues

FUSE_DEFAULT = -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
#FUSE_RELEASE = -U lfuse:w:0x64:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

COMPILE += $(MAIN).hex $(MAIN).eep



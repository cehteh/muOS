# Project configuration

PLATFORM=atmel
MCU = atmega328p
#MCU = attiny85

MAIN = muos

PROGRAMMER = avrdude
AVRDUDE_BAUDRATE = 57600
AVRDUDE_PORT = /dev/ttyUSB0
#AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PROGRAMMER = arduino

# loading variables, initialization and defaults
include muos/muos.mk

# Common CC Flags
#CCFLAGS +=

# Dependency generation Flags
#DEPFLAGS +=

# Preprocessor Flags
#CPPFLAGS +=

# Compile Flags
CFLAGS += -Wall -Wextra -Werror

# Linker flags
LDFLAGS += -Wl,--relax,--gc-sections
LDFLAGS += -Xlinker --no-fatal-warnings

include muos/muos_rules.mk


# Project configuration

PLATFORM = atmel
MCU = atmega328p
#MCU = attiny85

MAIN = example

PROGRAMMER = avrdude
#PROGRAMMER = micronucleus

AVRDUDE_BAUDRATE = 57600
AVRDUDE_PORT = /dev/ttyUSB0
#AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PROGRAMMER = arduino

# additional C Source file
#SOURCES +=

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

# loading variables, initialization and defaults
include muos/muos.mk

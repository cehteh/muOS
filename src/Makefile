# Project configuration

PLATFORM=atmel
MCU = atmega328p
#MCU = attiny85

MAIN = muos

PROGRAMMER=avrdude

# loading variables, initialization and defaults
include muos/muos.mk

# Common CC Flags
#CCFLAGS +=

# Preprocessor Flags
#CPPFLAGS +=

# Compile Flags
CFLAGS += -Wall -Wextra -Werror

# Dependency generation Flags
#DEPFLAGS +=

# Linker flags
LDFLAGS += -Wl,--relax,--gc-sections
LDFLAGS += -Xlinker --no-fatal-warnings

include muos/muos_rules.mk


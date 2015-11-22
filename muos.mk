
SOURCES = $(MAIN).c
SOURCES += $(wildcard muos/*.c)
SOURCES += $(wildcard muos/hw/*.c)

OBJECTS = $(SOURCES:.c=.o)

MAKEFILES = Makefile
MAKEFILES += muos/muos.mk
MAKEFILES += muos/muos_rules.mk
MAKEFILES += muos/$(PROGRAMMER).mk
MAKEFILES += muos/$(PROGRAMMER)_rules.mk
MAKEFILES += muos/hw/$(PLATFORM)/platform.mk
MAKEFILES += muos/hw/$(PLATFORM)/platform_rules.mk

# Common CC Flags
CCFLAGS = -std=gnu99

# Preprocessor Flags
CPPFLAGS = -I .

# Compile Flags
CFLAGS = $(CCFLAGS) $(CPPFLAGS)

# Dependency generation Flags
DEPFLAGS = -M -MP

# Linker flags
LDFLAGS = $(CCFLAGS)
LDFLAGS += -Wl,--relax,--gc-sections
LDFLAGS += -Xlinker --no-fatal-warnings

# What files must be generated for 'make all'
COMPILE=

include muos/$(PROGRAMMER).mk
include muos/hw/$(PLATFORM)/platform.mk


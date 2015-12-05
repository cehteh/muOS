
SOURCES = $(MAIN).c
SOURCES += $(wildcard muos/*.c)
SOURCES += $(wildcard muos/hw/*.c)

OBJECTS = $(SOURCES:.c=.o)

# Common CC Flags
CCFLAGS = -std=gnu99

# Dependency generation Flags
DEPFLAGS = -mmcu=$(MCU) -M -MP -MT $*.o

# Preprocessor Flags
CPPFLAGS = -I .

# Compile Flags
CFLAGS = $(CCFLAGS) $(CPPFLAGS)

# Linker flags
LDFLAGS = $(CCFLAGS)
LDFLAGS += -Wl,--relax,--gc-sections
LDFLAGS += -Xlinker --no-fatal-warnings

# What files must be generated for 'make all'
BUILD = $(MAIN).elf

PRINTFMT = printf "%-60s%16s\n"

MAKEFLAGS += -R -s -O -j $(shell nproc || echo 2)
.DEFAULT_GOAL = all

.SUFFIXES:

include muos/prg_$(PROGRAMMER).mk
include muos/hw/$(PLATFORM)/platform.mk
-include $(SOURCES:.c=.d)


all: $(BUILD)
	$(PRINTFMT) '$(BUILD)' [BUILD]

# Dependency generation and cleanup

%.d: %.c
	$(PRINTFMT) $@ [DEPGEN]
	$(CC) $(DEPFLAGS) $(CPPFLAGS) $< | sed 's,^$*.o,$*.o $*.d,g' > $@

%.o: %.d


depclean:
	rm -f $(SOURCES:.c=.d)

%.o: %.c Makefile muos/muos.mk
	$(PRINTFMT) $@ [COMPILE]
	$(CC) $(CFLAGS) -c $< -o $@


asm: $(MAIN).asm

show_asm: $(MAIN).asm
	less $(MAIN).asm


size: $(BUILD)
	@$(SIZE) --target=ihex $(BUILD)

clean: depclean
	rm -f *.elf *.a $(OBJECTS)

mrproper: clean
	rm -f $(BUILD)

#gitclean: git stash, git clean -dfx

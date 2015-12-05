
SOURCES += $(MAIN).c
SOURCES += $(wildcard muos/*.c)
SOURCES += $(wildcard muos/hw/*.c)

OBJECTS += $(SOURCES:.c=.o)

# Common CC Flags
CCFLAGS += -std=gnu99

# Dependency generation Flags
DEPFLAGS += -mmcu=$(MCU) -M -MP -MT $*.o

# Preprocessor Flags
CPPFLAGS += -I .

# Compile Flags
CFLAGS += $(CCFLAGS) $(CPPFLAGS)

# Linker flags
LDFLAGS += $(CCFLAGS)
LDFLAGS += -Wl,--relax,--gc-sections
LDFLAGS += -Xlinker --no-fatal-warnings

# What files must be generated for 'make all'
PROGRAMS += $(MAIN).elf

PRINTFMT = printf "%-60s%16s\n"

MAKEFLAGS = -R -s -O -j $(shell nproc || echo 2)
.DEFAULT_GOAL = all

.SUFFIXES:
.PRECIOUS: .v/%
.PHONY: clean program
FORCE:

include muos/prg_$(PROGRAMMER).mk
include muos/hw/$(PLATFORM)/platform.mk
-include $(SOURCES:.c=.d)


all: $(PROGRAMS)
	$(PRINTFMT) '$(PROGRAMS)' [PROGRAMS]

# dependencies on variables, stored in .v/
.v/%: FORCE
	mkdir -p .v
	echo "$($*)" | cmp - $@ 2>/dev/null >/dev/null || { echo "$($*)" > $@; $(PRINTFMT) $* [DEPVAR]; }


# Dependency generation and cleanup

%.d: %.c .v/DEPFLAGS .v/CPPFLAGS .v/CC
	$(PRINTFMT) $@ [DEPGEN]
	$(CC) $(DEPFLAGS) $(CPPFLAGS) $< | sed 's,^$*.o,$*.o $*.d,g' > $@

%.o: %.d


depclean:
	$(PRINTFMT) $@ [DEPCLEAN]
	rm -f $(SOURCES:.c=.d)

%.o: %.c .v/CFLAGS .v/CC
	$(PRINTFMT) $@ [COMPILE]
	$(CC) $(CFLAGS) -c $< -o $@


asm: $(MAIN).asm


size: $(PROGRAMS) .v/SIZE
	$(PRINTFMT) $@ [SIZE]
	$(SIZE) --target=ihex $(PROGRAMS)

clean: depclean
	$(PRINTFMT) $@ [CLEAN]
	rm -f *.elf *.a $(OBJECTS) .v/*

mrproper: clean .v/PROGRAMS
	$(PRINTFMT) $@ [MRPROPER]
	rm -f $(PROGRAMS)

#gitclean: git stash, git clean -dfx

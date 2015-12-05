#      mµOS            - my micro OS
#
# Copyright (C)
#      2015                            Christian Thäter <ct@pipapo.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

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

PROGRAMS += $(MAIN).hex $(MAIN).eep

elf: $(MAIN).elf

%.elf: $(OBJECTS) .v/LDFLAGS .v/CC
	$(PRINTFMT) $@ [LINK]
	$(CC) $(LDFLAGS) $(OBJECTS) --output $@ 2>&1 | sed 'h;:b;$b;N;N;/appears to be a misspelled signal handler/{N;d};$b;D'

%.asm: %.elf .v/OBJDUMP
	$(PRINTFMT) $@ [ASM]
	$(OBJDUMP) -S $< >$@


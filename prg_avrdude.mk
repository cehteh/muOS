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

AVRDUDE = avrdude

AVRDUDE_PART = -p $(MCU)

AVRDUDE_FLAGS += -c $(AVRDUDE_PROGRAMMER) $(AVRDUDE_PART) -b $(AVRDUDE_BAUDRATE) -P $(AVRDUDE_PORT)
AVRDUDE_DEFAULT_SCK += -B 8
#AVRDUDE_128kHz_SCK = -B 16

hex: $(MAIN).hex
eep: $(MAIN).eep

%.hex: %.elf .v/OBJCOPY
	$(PRINTFMT) $@ HEX
	$(OBJCOPY) -O ihex  $< $@

%.eep: %.elf .v/OBJCOPY
	$(PRINTFMT) $@ EEP
	$(OBJCOPY) -j .eeprom  --change-section-lma .eeprom=0 -O ihex $< $@ 2>/dev/null

upload: all .v/AVRDUDE .v/AVRDUDE_FLAGS .v/AVRDUDE_DEFAULT_SCK .v/MAIN
	$(PRINTFMT) $(MAIN) UPLOAD
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -U flash:w:$(MAIN).hex:i
#-U eeprom:w:$(MAIN).eep:i

download: .v/AVRDUDE .v/AVRDUDE_FLAGS .v/AVRDUDE_DEFAULT_SCK .v/MAIN
	$(PRINTFMT) $(MAIN) DOWNLOAD
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -U flash:r:$(MAIN).hex:i -U eeprom:r:$(MAIN).eep:i

#reset_fuse:
#	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_128kHz_SCK) $(FUSE_DEFAULT)

#fuse_128khz:
#	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) $(FUSE_RELEASE)

reset_device: .v/AVRDUDE .v/AVRDUDE_FLAGS .v/AVRDUDE_DEFAULT_SCK .v/FUSE_DEFAULT
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -e $(FUSE_DEFAULT)


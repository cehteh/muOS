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

MICRONUCLEUS = micronucleus

#PLANNED: generate images in another target specific makefile?

hex: $(MAIN).hex
eep: $(MAIN).eep

%.hex: %.elf .v/OBJCOPY
	$(PRINTFMT) $@ HEX
	$(OBJCOPY) -O ihex  $< $@

%.eep: %.elf .v/OBJCOPY
	$(PRINTFMT) $@ EEP
	$(OBJCOPY) -j .eeprom  --change-section-lma .eeprom=0 -O ihex $< $@ 2>/dev/null

upload: all .v/MICRONUCLEUS .v/MAIN
	$(PRINTFMT) $(MAIN) UPLOAD
	$(MICRONUCLEUS) $(MICRONUCLEUS_FLAGS) $(MAIN).hex

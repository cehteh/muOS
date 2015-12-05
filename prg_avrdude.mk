
AVRDUDE = avrdude

AVRDUDE_PART = -p $(MCU)

AVRDUDE_FLAGS += -c $(AVRDUDE_PROGRAMMER) $(AVRDUDE_PART) -b $(AVRDUDE_BAUDRATE) -P $(AVRDUDE_PORT)
AVRDUDE_DEFAULT_SCK += -B 8
#AVRDUDE_128kHz_SCK = -B 16

hex: $(MAIN).hex
eep: $(MAIN).eep

%.hex: %.elf .v/OBJCOPY
	$(PRINTFMT) $@ [HEX]
	$(OBJCOPY) -O ihex -j .text $< $@

%.eep: %.elf .v/OBJCOPY
	$(PRINTFMT) $@ [EEP]
	$(OBJCOPY) -j .eeprom  --change-section-lma .eeprom=0 -O ihex $< $@ 2>/dev/null

program: all .v/AVRDUDE .v/AVRDUDE_FLAGS .v/AVRDUDE_DEFAULT_SCK .v/MAIN
	printf $(PRINTFMT) $(MAIN) [PROGAM]
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -U flash:w:$(MAIN).hex:i
#-U eeprom:w:$(MAIN).eep:i

read_program: .v/AVRDUDE .v/AVRDUDE_FLAGS .v/AVRDUDE_DEFAULT_SCK .v/MAIN
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -U flash:r:$(MAIN).hex:i -U eeprom:r:$(MAIN).eep:i

#reset_fuse:
#	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_128kHz_SCK) $(FUSE_DEFAULT)

#fuse_128khz:
#	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) $(FUSE_RELEASE)

reset_device: .v/AVRDUDE .v/AVRDUDE_FLAGS .v/AVRDUDE_DEFAULT_SCK .v/FUSE_DEFAULT
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -e $(FUSE_DEFAULT)


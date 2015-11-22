
hex: $(MAIN).hex
eep: $(MAIN).eep

%.hex: %.elf
	$(OBJCOPY) -O ihex -j .text $< $@

%.eep: %.elf
	$(OBJCOPY) -j .eeprom  --change-section-lma .eeprom=0 -O ihex $< $@

program: all
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -U flash:w:$(MAIN).hex:i
#-U eeprom:w:$(MAIN).eep:i

read_program:
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -U flash:r:$(MAIN).hex:i -U eeprom:r:$(MAIN).eep:i

reset_fuse:
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_128kHz_SCK) $(FUSE_DEFAULT)

#fuse_128khz:
#	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) $(FUSE_RELEASE)

reset_device:
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_DEFAULT_SCK) -e $(FUSE_DEFAULT) || \
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_128kHz_SCK) -e $(FUSE_DEFAULT)

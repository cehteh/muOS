elf: $(MAIN).elf

%.elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) --output $@

%.asm: %.elf
	$(OBJDUMP) -S $< >$@


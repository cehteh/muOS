all: $(COMPILE) size

include muos/prg_$(PROGRAMMER)_rules.mk
include muos/hw/$(PLATFORM)/platform_rules.mk


# Dependency generation and cleanup
%.d: %.c
	set -e; rm -f $@; \
        $(CC) $(DEPFLAGS) $(CPPFLAGS) $< > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
        rm -f $@.$$$$

-include $(SOURCES:.c=.d)

%.o %d: %c

depclean:
	rm -f $(SOURCES:.c=.d) $(SOURCES:.c=.d.*)



*.o: $(MAKEFILES) $(shell which $(CC))


asm: $(MAIN).asm

show_asm: $(MAIN).asm
	less $(MAIN).asm


size: $(COMPILE)
	@$(SIZE) --target=ihex $(COMPILE)


clean: depclean
	rm -f *.elf *.a $(OBJECTS)

mrproper: clean
	rm -f $(COMPILE)

#gitclean: git stash, git clean -dfx

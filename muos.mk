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


SOURCES += $(wildcard $(MUOS_DIR)/*.c)
SOURCES += $(wildcard $(MUOS_DIR)/lib/*.c)
SOURCES += $(wildcard $(MUOS_DIR)/hw/$(PLATFORM)/*.c)

HEADERS += $(wildcard $(MUOS_DIR)/*.h)
HEADERS += $(wildcard $(MUOS_DIR)/lib/*.h)
HEADERS += $(wildcard $(MUOS_DIR)/hw/$(PLATFORM)/*.h)

TXTS += $(wildcard $(MUOS_DIR)/doc/*.txt) $(wildcard $(MUOS_DIR)/doc/*.pdoc) $(MUOS_DIR)/doc/pipadoc_config.lua VERSION
# Makefiles can include documentation too
MAKEFILE_DOCS += Makefile $(MUOS_DIR)/muos.mk $(wildcard $(MUOS_DIR)/prg_*.mk) $(widcard $(MUOS_DIR)/hw/*.mk)

OBJECTS += $(SOURCES:.c=.o)

# Common CC Flags
CCFLAGS += -std=gnu99

# Dependency generation Flags
DEPFLAGS += $(CCFLAGS) -M -MP -MT $*.o

# Preprocessor Flags
CPPFLAGS += -I $(MUOS_DIR)/.. -D'MUOS_HW_HEADER=<$(MUOS_DIR)/hw/$(PLATFORM)/$(MCU).h>'

# Compile Flags
CFLAGS += $(CCFLAGS) $(CPPFLAGS)

# Linker flags
LDFLAGS += $(CCFLAGS)
LDFLAGS += -Wl,--relax,--gc-sections
LDFLAGS += -Xlinker --no-fatal-warnings

# What files must be generated for 'make all'
IMAGES += $(MAIN).elf

PRINTFMT = printf "%-60s[ %10s ]\n"

GITBRANCH := $(shell git symbolic-ref --short HEAD)

PARALLEL = $(shell nproc || echo 2)
ifndef MAKE_DEBUG
	MAKEFLAGS = -R -s -j $(PARALLEL)
	PIPADOCFLAGS = -q
else
	MAKEFLAGS = -R
	PIPADOCFLAGS = -d
endif

.DEFAULT_GOAL = all

.SUFFIXES:
.PRECIOUS: %.v
.PHONY: clean depclean program
FORCE:

ifdef PROGRAMMER
include $(MUOS_DIR)/prg_$(PROGRAMMER).mk
endif
include $(MUOS_DIR)/hw/$(PLATFORM).mk
-include $(SOURCES:.c=.d)


all: $(IMAGES) show_issues
	$(PRINTFMT) '$(IMAGES)' IMAGES

# dependencies on variables
%.v: FORCE
	echo "$($*)" | md5sum - | cmp - $@ 2>/dev/null >/dev/null || { echo "$($*)"  | md5sum - > $@; $(PRINTFMT) $@ DEPVAR;}

# Dependency generation and cleanup

%.d: %.c DEPFLAGS.v CPPFLAGS.v CC.v MUOS_CONFIG.v
	$(PRINTFMT) $@ DEPGEN
	$(CC) $(DEPFLAGS) $(CPPFLAGS) $(MUOS_CONFIG) $< | sed 's,^$*.o,$*.o $*.d,g' > $@

%.o: %.d


depclean:
	$(PRINTFMT) $@ DEPCLEAN
	rm -rf $(SOURCES:.c=.d)	*.v

%.o: %.c CFLAGS.v CC.v MUOS_CONFIG.v
	$(PRINTFMT) $@ COMPILE
	$(CC) $(CFLAGS) $(MUOS_CONFIG) -c $< -o $@

#TODO: document init system
#PLANNED: make a complete header? not including drivers from muos.c
$(MUOS_DIR)/init.inc: $(filter-out $(MUOS_DIR)/muos.c,$(SOURCES)) $(HEADERS)
	$(PRINTFMT) $@ INIT_INC
	sed -e 's/^\(muos_.*_\([0-9]\+\)init\).*[^;]$$/\2 \1/p;d' $(SOURCES) $(HEADERS) |\
	sort -u |\
	sed -e 's/^[0-9]* \(\(muos_.*\)_[0-9]*.*\)/#ifdef \U\2\n\tMUOS_INIT\L(\1);\n#endif/p;d' > $@

$(MUOS_DIR)/muos.c: $(MUOS_DIR)/init.inc


asm: $(MAIN).asm


size: $(IMAGES) SIZE.v
	$(PRINTFMT) $@ SIZE
	$(SIZE) --target=ihex $(IMAGES)

clean: depclean docclean IMAGES.v
	$(PRINTFMT) $@ CLEAN
	rm -f *.elf *.a $(OBJECTS) $(IMAGES)

#gitclean: git stash, git clean -dfx

dir_info:
	find -name DIR_INFO -printf '%-30h: ' -exec head -1 {} \; | sort -k1,1 | cut -c 1-$${$(shell tput cols):-92}

#documentation targets


LUA := $(shell							\
	for i in lua5.3 lua-5.3 lua5.2 lua-5.2 lua lua-5.1; do	\
		program=$$(which "$$i") ;			\
		if test "$$program" -a -x "$$program"; then	\
			echo "$$program";			\
			return;					\
		fi;						\
	done;							\
	)

ASCIIDOC := $(shell which asciidoc)

A2X := $(shell which a2x)


docclean:
	$(PRINTFMT) $@ DOCCLEAN
	rm -f *.html *.txt *.pdf *.xml

doc: manual issues README WEB

txt: muos_manual.txt muos_issues.txt

manual: muos_manual.pdf muos_manual.html

issues: muos_issues.html

README: ../README

WEB: index.html

%.pdf: %.txt
ifneq ("$(A2X)","")
	$(PRINTFMT) $@ A2X
	$(A2X) -d book -L -k --dblatex-opts "-P latex.output.revhistory=0" --dblatex-opts "-b xetex" $<
else
	$(PRINTFMT) $@ "A2X NOT AVAILABLE"
endif

%.html: %.txt
ifneq ("$(ASCIIDOC)","")
	$(PRINTFMT) $@ ASCIIDOC
	$(ASCIIDOC) -d book -a toc $<
else
	$(PRINTFMT) $@ "ASCIIDOC NOT AVAILABLE"
endif

muos_manual.txt: $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ PIPADOC
	$(LUA) $(MUOS_DIR)/doc/pipadoc.lua $(PIPADOCFLAGS) -c $(MUOS_DIR)/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) >$@
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

muos_issues.txt: $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ ISSUES
	$(LUA) $(MUOS_DIR)/doc/pipadoc.lua $(PIPADOCFLAGS) -t ISSUES -c $(MUOS_DIR)/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) >$@
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

fixme: FORCE $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ FIXME
	$(LUA) $(MUOS_DIR)/doc/pipadoc.lua $(PIPADOCFLAGS) -t FIXME -c $(MUOS_DIR)/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) 1>&2
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

todo: FORCE $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ TODO
	$(LUA) $(MUOS_DIR)/doc/pipadoc.lua $(PIPADOCFLAGS) -t TODO -c $(MUOS_DIR)/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) 1>&2
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif



planned: FORCE $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ PLANNED
	$(LUA) $(MUOS_DIR)/doc/pipadoc.lua $(PIPADOCFLAGS) -t PLANNED -c $(MUOS_DIR)/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) 1>&2
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

planned_gitbranch: FORCE $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ PLANNED
	$(LUA) $(MUOS_DIR)/doc/pipadoc.lua $(PIPADOCFLAGS) -t PLANNED -c $(MUOS_DIR)/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) | { grep -A 5 -i '^[^ ]*$(GITBRANCH).*::'; true; } 1>&2
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif


show_issues: $(if $(filter master, $(GITBRANCH)), fixme, $(if $(filter devel, $(GITBRANCH)), todo fixme, planned_gitbranch todo fixme))


../README: $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ README
	$(LUA) $(MUOS_DIR)/doc/pipadoc.lua $(PIPADOCFLAGS) -t README -c $(MUOS_DIR)/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) >../README
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

index.txt: $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ WEBPAGE
	$(LUA) $(MUOS_DIR)/doc/pipadoc.lua $(PIPADOCFLAGS) -t WEB -c $(MUOS_DIR)/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) >$@
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

VERSION: FORCE
	echo ":version:       $$(git describe --tags --dirty --always)\n:muos_version:  $$(cd $(MUOS_DIR); git describe --tags --dirty --always)" |\
	cmp - $@ 2>/dev/null >/dev/null ||\
	{ echo ":version:       $$(git describe --tags --dirty --always)\n:muos_version:  $$(cd $(MUOS_DIR); git describe --tags --dirty --always)" > $@; $(PRINTFMT) $@ VERSION;}

version.h: FORCE
	{													\
		echo "#define VERSION \"$$(git describe --tags --dirty --always)\"";				\
		echo "#define MUOS_VERSION \"$$(cd $(MUOS_DIR); git describe --tags --dirty --always)\"";		\
	} | cmp - $@ 2>/dev/null >/dev/null ||									\
	{													\
		{												\
			echo "#define VERSION \"$$(git describe --tags --dirty --always)\"";			\
			echo "#define MUOS_VERSION \"$$(cd $(MUOS_DIR); git describe --tags --dirty --always)\"";	\
		} > $@;												\
		$(PRINTFMT) $@ VERSION_H;									\
	}


# maintainer targets
gitpush: FORCE
	$(PRINTFMT) $@ GIT_PUSH
	git push -q --all --force public;	\
	git push -q --tags --force public;	\
	git push -q --all --force github;	\
	git push -q --tags --force github;
	cd $(MUOS_DIR);				\
	git push -q --all --force public;	\
	git push -q --tags --force public;	\
	git push -q --all --force github;	\
	git push -q --tags --force github;

publish: doc gitpush FORCE
	$(PRINTFMT) $@ PUBLISH
	rsync *.html muos_*.pdf www.pipapo.org:/var/local/www_muos/

#EOF

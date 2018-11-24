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


SOURCES += $(wildcard muos/*.c)
SOURCES += $(wildcard muos/lib/*.c)
SOURCES += $(wildcard muos/hw/$(PLATFORM)/*.c)

HEADERS += $(wildcard muos/*.h)
HEADERS += $(wildcard muos/lib/*.h)
HEADERS += $(wildcard muos/hw/$(PLATFORM)/*.h)

TXTS += $(wildcard muos/doc/*.txt) $(wildcard muos/doc/*.pdoc) muos/doc/pipadoc_config.lua VERSION
# Makefiles can include documentation too
MAKEFILE_DOCS += Makefile muos/muos.mk $(wildcard muos/prg_*.mk) $(widcard muos/hw/*.mk)

OBJECTS += $(SOURCES:.c=.o)

# Common CC Flags
CCFLAGS += -std=gnu99

# Dependency generation Flags
DEPFLAGS += $(CCFLAGS) -M -MP -MT $*.o

# Preprocessor Flags
CPPFLAGS += -I . -D'MUOS_HW_HEADER=<muos/hw/$(PLATFORM)/$(MCU).h>'

# Compile Flags
CFLAGS += $(CCFLAGS) $(CPPFLAGS)

# Linker flags
LDFLAGS += $(CCFLAGS)
LDFLAGS += -Wl,--relax,--gc-sections
LDFLAGS += -Xlinker --no-fatal-warnings

# What files must be generated for 'make all'
IMAGES += $(MAIN).elf

PRINTFMT = printf "%-60s[ %8s ]\n"

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

include muos/prg_$(PROGRAMMER).mk
include muos/hw/$(PLATFORM).mk
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
muos/init.inc: $(filter-out muos/muos.c,$(SOURCES)) $(HEADERS)
	$(PRINTFMT) $@ INIT_INC
	sed -e 's/^\(muos_[^_]*_\([0-9]\+\)init\).*[^;]$$/\2 \1/p;d' $(SOURCES) $(HEADERS) |\
	sort -u |\
	sed -e 's/^[0-9]* \(\(muos_[^_]*\).*\)/#ifdef \U\2\n\t\L\1 ();\n#endif/p;d' > $@

muos/muos.c: muos/init.inc

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
	$(LUA) muos/doc/pipadoc.lua $(PIPADOCFLAGS) -c muos/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) >$@
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

muos_issues.txt: $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ ISSUES
	$(LUA) muos/doc/pipadoc.lua $(PIPADOCFLAGS) -t ISSUES -c muos/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) >$@
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

fixme: FORCE $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ FIXME
	$(LUA) muos/doc/pipadoc.lua $(PIPADOCFLAGS) -t FIXME -c muos/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) 1>&2
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

todo: FORCE $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ TODO
	$(LUA) muos/doc/pipadoc.lua $(PIPADOCFLAGS) -t TODO -c muos/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) 1>&2
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif



planned: FORCE $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ PLANNED
	$(LUA) muos/doc/pipadoc.lua $(PIPADOCFLAGS) -t PLANNED -c muos/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) 1>&2
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

planned_gitbranch: FORCE $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ PLANNED
	$(LUA) muos/doc/pipadoc.lua $(PIPADOCFLAGS) -t PLANNED -c muos/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) | { grep -A 5 -i '^[^ ]*$(GITBRANCH).*::'; true; } 1>&2
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif


show_issues: $(if $(filter master, $(GITBRANCH)), fixme, $(if $(filter devel, $(GITBRANCH)), todo fixme, planned_gitbranch todo fixme))


../README: $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ README
	$(LUA) muos/doc/pipadoc.lua $(PIPADOCFLAGS) -t README -c muos/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) >../README
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

index.txt: $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS)
ifneq ("$(LUA)","")
	$(PRINTFMT) $@ WEBPAGE
	$(LUA) muos/doc/pipadoc.lua $(PIPADOCFLAGS) -t WEB -c muos/doc/pipadoc_config.lua $(TXTS) $(SOURCES) $(HEADERS) $(MAKEFILE_DOCS) >$@
else
	$(PRINTFMT) $@ "LUA NOT AVAILABLE"
endif

VERSION: FORCE
	echo ":version:       $$(git describe --tags --dirty --always)\n:muos_version:  $$(cd muos; git describe --tags --dirty --always)" |\
	cmp - $@ 2>/dev/null >/dev/null ||\
	{ echo ":version:       $$(git describe --tags --dirty --always)\n:muos_version:  $$(cd muos; git describe --tags --dirty --always)" > $@; $(PRINTFMT) $@ VERSION;}

# maintainer targets
gitpush: FORCE
	$(PRINTFMT) $@ GIT_PUSH
	git push -q --all --force public;	\
	git push -q --tags --force public;	\
	git push -q --all --force github;	\
	git push -q --tags --force github;
	cd muos;				\
	git push -q --all --force public;	\
	git push -q --tags --force public;	\
	git push -q --all --force github;	\
	git push -q --tags --force github;

publish: doc gitpush FORCE
	$(PRINTFMT) $@ PUBLISH
	rsync *.html muos_*.pdf www.pipapo.org:/var/local/www_muos/


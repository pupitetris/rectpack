##
## Copyright (C) 2010 Eric Huang
##  
## This program is free software: you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation, either version 3 of the
## License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see
## <http://www.gnu.org/licenses/>.
##

EXECUTABLE  = rectpack
COMMONFLAGS = -Wall -Wno-parentheses -Wno-deprecated -march=native \
     -msse -msse2

# This addition to COMMONFLAGS pertains to linking to GTK libraries
# for the visualization. EH: Disabled for now. viz target broken.
# `pkg-config gtkmm-2.4 --cflags`

DEBUGFLAGS  = -ggdb3 -fno-inline -DDEBUG
RELFLAGS    = -O3 -pipe -DNDEBUG -momit-leaf-frame-pointer
PROFFLAGS   = -pg -ggdb3 -pipe -O3
VIZCFLAGS   = -DVIZ
LIBFLAGS    = \
    -lboost_thread \
    -lboost_program_options \
    -lgmp \
    -lgmpxx
#    -lgthread-2.0
#    `pkg-config gtkmm-2.4 --libs`

# The directories containing the source files, separated by ':'
VPATH=.

# To make "debug" the default (if invoked with just "make"):
#
ifeq ($(MAKECMDGOALS),)
MAKECMDGOALS=debug
endif

# The source files: regardless of where they reside in 
# the source tree, VPATH will locate them...

include Makefile.Sources

# Build a Dependency list and an Object list, by replacing 
# the .cc extension to .d for dependency files, and .o for 
# object files.
DEP = $(patsubst %.cc, deps/%.d, ${SRC})
OBJ = $(patsubst %.cc, $(MAKECMDGOALS)/%.o, \
	${SRC})

# What compiler to use for generating dependencies: 
# it will be invoked with -MM
CXXDEP = g++

# Separate compile options per configuration
ifeq ($(MAKECMDGOALS),release)
CXXFLAGS = $(RELFLAGS) $(COMMONFLAGS) $(DEFINES)
else
ifeq ($(MAKECMDGOALS),profile)
CXXFLAGS = $(PROFFLAGS) $(COMMONFLAGS) $(DEFINES)
else
ifeq ($(MAKECMDGOALS),viz)
CXXFLAGS = $(RELFLAGS) $(COMMONFLAGS) $(DEFINES) $(VIZCFLAGS)
else
ifeq ($(MAKECMDGOALS),vizd)
CXXFLAGS = $(DEBUGFLAGS) $(COMMONFLAGS) $(DEFINES) $(VIZCFLAGS)
else
CXXFLAGS = $(DEBUGFLAGS) $(COMMONFLAGS) $(DEFINES)
endif
endif
endif
endif

all:     inform $(MAKECMDGOALS)/${EXECUTABLE}

debug:   inform $(MAKECMDGOALS)/${EXECUTABLE}

release: inform $(MAKECMDGOALS)/${EXECUTABLE}

profile: inform $(MAKECMDGOALS)/${EXECUTABLE}

viz:     inform $(MAKECMDGOALS)/${EXECUTABLE}

vizd:    inform $(MAKECMDGOALS)/${EXECUTABLE}

inform:
ifneq ($(MAKECMDGOALS),profile)
ifneq ($(MAKECMDGOALS),release)
ifneq ($(MAKECMDGOALS),debug)
ifneq ($(MAKECMDGOALS),viz)
ifneq ($(MAKECMDGOALS),vizd)
	@echo "Invalid configuration "$(MAKECMDGOALS)" specified."
	@echo -n "You must specify a configuration when "
	@echo "running make, e.g. make debug"
	@echo  
	@echo -n "Possible choices for configuration are "
	@echo "'release', 'debug', 'profile', 'viz', and 'vizd'."
	@exit 1
endif
endif
endif
endif
endif
	@echo "Configuration "$(MAKECMDGOALS)
	@echo "------------------------"

$(MAKECMDGOALS)/${EXECUTABLE}: ${OBJ} | inform
	@mkdir -p $(MAKECMDGOALS)
	$(CXX) -g -o $@ $^ ${CXXFLAGS} ${LIBFLAGS}

$(MAKECMDGOALS)/%.o: %.cc
	@mkdir -p $(MAKECMDGOALS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

deps/%.d: %.cc
	@mkdir -p deps
	@echo Generating dependencies for $<
	@set -e ; $(CXXDEP) -MM $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(MAKECMDGOALS)\/\1.o $@ : ,g' \
		< $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	@rm -rf deps release profile debug viz vizd

cleanr:
	@rm -rf deps release

cleanp:
	@rm -rf deps profile

cleand:
	@rm -rf deps debug

cleanv:
	@rm -rf deps viz

cleanvd:
	@rm -rf deps vizd

# Unless "make clean" is called, include the dependency files
# which are auto-generated. Don't fail if they are missing
# (-include), since they will be missing in the first 
# invocation!
ifneq ($(MAKECMDGOALS),clean)
-include ${DEP}
endif

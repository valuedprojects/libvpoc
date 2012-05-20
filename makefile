#
# lib.org.sinkme.misc/makefile.
# Ken Lamarche - May, 2012
#
# Universal makefile for my lib.org.sinkme.misc project, built in different Linux style environments.
#

# PROJECT comes from the directory I'm working in.
PROJECT = $(notdir $(PWD))

# PLATFORM comes from version of linux style system I'm running.
PLATFORM = $(shell uname)
PLATFORM_DIR = $(KL_EXPERIMENTS)/$(PROJECT)/$(PLATFORM)

# Products are the .a and .so Libraries
LIBSO = $(subst .,_,$(PROJECT)).so
LIBA = $(subst .,_,$(PROJECT)).a

ELEMENTS += streamOps
ELEMENTS += tcpUtils

# Where the source code is...
SRC_DIR = $(KL_EXPERIMENTS)/$(PROJECT)/src

# When looking for dependents, make looks in the src directory
vpath %.c $(SRC_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.h $(SRC_DIR)

CPPFLAGS += -I$(KL_EXPERIMENTS)/include

.PHONY : all clean

all: $(PLATFORM_DIR)/$(LIBSO)($(addsuffix .o,$(ELEMENTS))) $(PLATFORM_DIR)/$(LIBA)($(addsuffix .o,$(ELEMENTS)))

$(PLATFORM_DIR) :
	mkdir $(PLATFORM_DIR)

$(PLATFORM_DIR)/$(LIBSO)($(addsuffix .o,$(ELEMENTS))) : $(PLATFORM_DIR)

$(PLATFORM_DIR)/$(LIBA)($(addsuffix .o,$(ELEMENTS))) : $(PLATFORM_DIR)

clean:
	$(RM) -rf $(PLATFORM_DIR)
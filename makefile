#
# lib.org.sinkme.misc/makefile.
# Ken Lamarche - Oct, 2012
#
# Universal makefile for my lib.org.sinkme.misc project, built in different Linux style environments.
#

ifndef DEV_ROOT
$(error "The DEV_ROOT variable must be set")
endif

# General development values:
DEV_INCLUDE_DIR = $(DEV_ROOT)/include
DEV_LIB_DIR = $(DEV_ROOT)/lib
DEV_BIN_DIR = $(DEV_ROOT)/bin

# PROJECT comes from the directory I'm working in.
PROJECT = $(notdir $(PWD))
PROJECT_DIR = $(PWD)

# PLATFORM comes from version of linux style system I'm running.
PLATFORM = $(shell uname)
PLATFORM_DIR = $(PROJECT_DIR)/$(PLATFORM)

# The object files in these libraries
ELEMENTS += streamOps
ELEMENTS += tcpUtils
ARMEMBERLIST = ($(addsuffix .o,$(ELEMENTS)))
$(info $(ARMEMBERLIST))

# Products are the .a and .so Libraries
LIBSO = $(subst .,_,$(PROJECT)).so
LIBS += $(LIBSO)
LIBA = $(subst .,_,$(PROJECT)).a
LIBS += $(LIBA)
PLATLIBS = $(addprefix $(PLATFORM_DIR)/,$(LIBS))
DEVLIBS = $(addprefix $(DEV_LIB_DIR)/,$(LIBS))
LIBSOBJS = $(addsuffix $(ARMEMBERLIST),$(PLATLIBS))
$(info $(LIBSOBJS))
$(info $(DEVLIBS))

# Define include files, and where they should be
HEADERS = $(addsuffix .h,$(ELEMENTS))
DEVHEADERS = $(addprefix $(DEV_INCLUDE_DIR)/,$(HEADERS))
$(info $(DEVHEADERS))

# Where the source code is...
SRC_DIR = $(PROJECT_DIR)/src

# When looking for dependents, make looks in the src directory
vpath %.c $(SRC_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.h $(SRC_DIR)

CPPFLAGS += -I$(DEV_ROOT)/include

.PHONY : all clean install $(DEV_LIB_DIR) $(DEV_INCLUDE_DIR) $(PLATFORM_DIR)

all: $(DEVLIBS) $(DEVHEADERS)

$(PLATLIBS): $(PLATFORM_DIR)
$(PLATLIBS): $(PLATFORM_DIR)/%: $(PLATFORM_DIR)/%$(ARMEMBERLIST)

$(DEV_LIB_DIR) $(DEV_INCLUDE_DIR) $(PLATFORM_DIR): 
	if [ ! -d $(@) ]; then \
	mkdir $(@); \
	fi

$(DEVLIBS): $(DEV_LIB_DIR)
$(DEVLIBS): $(DEV_LIB_DIR)/%: $(PLATFORM_DIR)/%
	cp $< $(@D)/.

$(DEVHEADERS): $(DEV_INCLUDE_DIR)
$(DEVHEADERS): $(DEV_INCLUDE_DIR)/%: $(PROJECT_DIR)/src/%
	cp $< $(@D)/.

install : $(addprefix $(DEV_LIB_DIR)/,$(LIBS)) $(addprefix $(DEV_INCLUDE_DIR)/,$(HEADERS))

clean:
	$(RM) -rf $(PLATFORM_DIR)
	$(RM) -rf $(DEVHEADERS) $(DEVLIBS)

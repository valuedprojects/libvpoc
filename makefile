#
# lib.org.sinkme.misc/makefile.
# Ken Lamarche - May, 2012
#
# Universal makefile for my lib.org.sinkme.misc project, built in different Linux style environments.
#

ifndef DEV_ROOT
$(error "The DEV_ROOT variable must be set")
endif

# General development values:
DEV_INCLUDE = $(DEV_ROOT)/include
DEV_LIB = $(DEV_ROOT)/lib
DEV_BIN = $(DEV_ROOT)/bin

# PROJECT comes from the directory I'm working in.
PROJECT = $(notdir $(PWD))
PROJECT_DIR = $(PWD)

# PLATFORM comes from version of linux style system I'm running.
PLATFORM = $(shell uname)
PLATFORM_DIR = $(PROJECT_DIR)/$(PLATFORM)

# Products are the .a and .so Libraries
LIBSO = $(subst .,_,$(PROJECT)).so
LIBS += $(LIBSO)
LIBA = $(subst .,_,$(PROJECT)).a
LIBS += $(LIBA)

ELEMENTS += streamOps
ELEMENTS += tcpUtils

HEADERS = $(addsuffix .h,$(ELEMENTS))

# Where the source code is...
SRC_DIR = $(PROJECT_DIR)/src

# When looking for dependents, make looks in the src directory
vpath %.c $(SRC_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.h $(SRC_DIR)

CPPFLAGS += -I$(DEV_ROOT)/include

.PHONY : all clean $(PLATFORM_DIR)

all: $(PLATFORM_DIR)/$(LIBSO)($(addsuffix .o,$(ELEMENTS))) $(PLATFORM_DIR)/$(LIBA)($(addsuffix .o,$(ELEMENTS)))

$(PLATFORM_DIR) :
	if [ ! -d $(PLATFORM_DIR) ]; then \
	mkdir $(PLATFORM_DIR); \
	fi

$(addprefix $(PLATFORM_DIR)/, $(addsuffix ($(addsuffix .o,$(ELEMENTS))),$(LIBS))): $(PLATFORM_DIR)/%: $(PLATFORM_DIR)

$(addprefix $(PLATFORM_DIR)/, $(LIBS)): $(PLATFORM_DIR)/%: $(PLATFORM_DIR)/%($(addsuffix .o,$(ELEMENTS)))

#$(PLATFORM_DIR)/$(LIBSO) : $(PLATFORM_DIR)/$(LIBSO)($(addsuffix .o,$(ELEMENTS)))

#$(PLATFORM_DIR)/$(LIBA) : $(PLATFORM_DIR)/$(LIBA)($(addsuffix .o,$(ELEMENTS)))

$(DEV_LIB) :
	mkdir $(DEV_LIB)

$(DEV_INCLUDE) :
	mkdir $(DEV_INCLUDE)

$(addprefix $(DEV_LIB)/,$(LIBS)): $(DEV_LIB)/%: $(PLATFORM_DIR)/%
	-mkdir $(DEV_LIB)
	cp $< $(@D)/.

$(addprefix $(DEV_INCLUDE)/,$(HEADERS)): $(DEV_INCLUDE)/%: $(PROJECT_DIR)/src/%
	-mkdir $(DEV_INCLUDE)
	cp $< $(@D)/.

install : $(addprefix $(DEV_LIB)/,$(LIBS)) $(addprefix $(DEV_INCLUDE)/,$(HEADERS))

clean:
	$(RM) -rf $(PLATFORM_DIR)
	$(RM) -rf $(addprefix $(DEV_LIB)/,$(LIBS)) $(addprefix $(DEV_INCLUDE)/,$(HEADERS))

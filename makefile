#
# libvpoc/makefile.
# Ken Lamarche - April, 2014
#
# Simple makefile to build a library from command line.
# Produces:
# "lib" directory to contain the library file.
# "include" directory to contain the header files.
#
# By default (Mac), the library is a .dylib file.
# (branches of the repo, or switches on make can create other types of libraries.
#
# Getting there.
# This version will produce files that need to be "Installed".
# The libvpc++ file should be put in /usr/local/lib
# The header files should be included in a new directory: /usr/local/include/vcpo
#

# Project Name:
PROJECT = vpoc
PROJECTLIB = lib$(PROJECT)

# Determine OS,
# (not a very complete solution, but OK for now)
ifeq ($(OS),Windows_NT)
$(error Not set up for Windows build yet)
else
UNAME_S = $(shell uname -s)
UNAME_P = $(shell uname -p)
ifeq ($(UNAME_S), Darwin)
OS := OSX
endif
ifeq ($(UNAME_S), Linux)
OS := Linux
endif
endif

# Default Install Directories:
INSTALLHEADER_DIR = /usr/local/include
INSTALLLIB_DIR = /usr/local/lib

INSTALLPROJECTHEADER_DIR = $(INSTALLHEADER_DIR)/$(PROJECT)

# Set the library name:
# (not proud of this solution, but it will do for now)
LIBEXT := $(if $(filter OSX, $(OS)),.dylib, $(if $(filter Linux, $(OS)),.so))
TARGETLIB = $(join $(PROJECTLIB),$(LIBEXT))


# The following are the object elements that are held in the library.
ELEMENTS += tcpUtils
ELEMENTS += hexDump
ELEMENTS += randomNumber
MEMBERLIST = $(addsuffix .o,$(ELEMENTS))
ifdef DEBUG
$(info $(MEMBERLIST))
endif

# Where the source code is, and where "make" searches to find the source code.
SRC_DIR = src
vpath %.c $(SRC_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.h $(SRC_DIR)

LIB_DIR = lib
INC_DIR = include

TARGETLIBPATH = $(LIB_DIR)/$(TARGETLIB)
HEADERSLIST = $(addsuffix .h,$(ELEMENTS))
HEADERSPATHLIST = $(addprefix $(INC_DIR)/,$(HEADERSLIST))
ifdef DEBUG
$(info $(HEADERSPATHLIST))
endif

INSTALLHEADERSPATHLIST = $(addprefix $(INSTALLPROJECTHEADER_DIR)/,$(HEADERSLIST))
INSTALLLIBPATH = $(INSTALLLIB_DIR)/$(TARGETLIB)

# In general, all compiles would have access to the headers in DEV_ROOT/include
CFLAGS += -fPIC
CXXFLAGS += -fPIC


# Finally, the rules...
# First, the .PHONYs are run even if there are files existing with these names.
.PHONY : all clean install

all: $(TARGETLIBPATH) $(HEADERSPATHLIST)

# Clean by removing the library.
clean:
	$(RM) -rf $(LIB_DIR) $(INC_DIR) $(MEMBERLIST)

$(TARGETLIBPATH) : $(LIB_DIR) $(TARGETLIB)
	mv $(TARGETLIB) $(LIB_DIR)/.

$(TARGETLIB) : $(MEMBERLIST)
	$(CXX) -shared -o $@ $^
	
$(HEADERSPATHLIST) : $(INC_DIR)
$(HEADERSPATHLIST) : $(INC_DIR)/% : $(SRC_DIR)/%
	cp $< $(@D)/.


# The recipe for non-existent directories is to simply make them.
$(LIB_DIR) $(INC_DIR) $(INSTALLPROJECTHEADER_DIR): 
	mkdir $(@)

install: $(INSTALLHEADERSPATHLIST) $(INSTALLLIBPATH)

$(INSTALLHEADERSPATHLIST) : $(INSTALLPROJECTHEADER_DIR)
$(INSTALLHEADERSPATHLIST) : $(INSTALLPROJECTHEADER_DIR)/% : $(INC_DIR)/%
	cp $< $(@D)/.

$(INSTALLLIBPATH) : $(TARGETLIBPATH) $(INSTALLLIB_DIR)
	cp $(TARGETLIBPATH) $(@D)/.

uninstall:
	rm -rf $(INSTALLPROJECTHEADER_DIR) $(INSTALLLIBPATH)

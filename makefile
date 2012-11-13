#
# lib.org.sinkme.misc/makefile.
# Ken Lamarche - Oct, 2012
#
# Universal makefile for my lib.org.sinkme.misc project, built in different Linux style environments.
#

ifndef DEV_ROOT
$(error "The DEV_ROOT variable must be set")
endif
# As an example, my DEV_ROOT is $HOME/Experiments

# Basic development directories.  These declarations may have to go in each of my makefiles...
DEV_INCLUDE_DIR = $(DEV_ROOT)/include
DEV_LIB_DIR = $(DEV_ROOT)/lib
# Lets make sure these DEV_ROOT directories are not files...
# We have to make "make" tap into the shell to do the file system check.
# There may be a better way to do this with pure "make".  But once through this code, we know
# that we have directories to put files into, or we can make the directories.
ifneq '$(shell if [[ -d $(DEV_LIB_DIR) || ! -e $(DEV_LIB_DIR) ]]; then echo "OK"; fi )' "OK"
$(error "$$DEV_ROOT/lib is a file rather than a directory. The library cannot be installed in a file.")
endif
ifneq '$(shell if [[ -d $(DEV_INCLUDE_DIR) || ! -e $(DEV_INCLUDE_DIR) ]]; then echo "OK"; fi )' "OK"
$(error "$$DEV_ROOT/include is a file rather than a directory. The header files cannot be installed in a file.")
endif

# PROJECT name comes from the directory I'm working in.
PROJECT = $(notdir $(PWD))
PROJECT_DIR = $(PWD)

# PLATFORM name comes from version of linux style system I'm running.
PLATFORM = $(shell uname)
PLATFORM_DIR = $(PROJECT_DIR)/$(PLATFORM)

# This project is a library.
# The following are the object elements that are held in the library.
# Archive elements are identified in make rules as libX.so(e1.o e2.o e3.o).
# This (e1.o e2.o e3.o) syntax is needed to declare an archives dependence
# on it's members object code.  A cool implicit rule exists that will make
# a .o file, add it to a library, then remove the .o file.
ELEMENTS += streamOps
ELEMENTS += tcpUtils
ELEMENTS += randomNumber
ARMEMBERLIST = ($(addsuffix .o,$(ELEMENTS)))
ifdef DEBUG
$(info $(ARMEMBERLIST))
endif

# Products are the .a and .so archive libraries.
# I use the project directory name with "_"s instead of "."s
# The library products are formed in the project area, and are moved to
# the DEV_LIB area when compete.
LIBS += $(subst .,_,$(PROJECT)).so
LIBS += $(subst .,_,$(PROJECT)).a
PLATLIBS = $(addprefix $(PLATFORM_DIR)/,$(LIBS))
DEVLIBS = $(addprefix $(DEV_LIB_DIR)/,$(LIBS))
LIBSOBJS = $(addsuffix $(ARMEMBERLIST),$(PLATLIBS))
ifdef DEBUG
$(info $(LIBSOBJS))
$(info $(DEVLIBS))
endif

# Libraries are nothing there are no APIs to access their functions.
# Here are the header files that other tools would use to access the libraries.
HEADERS = $(addsuffix .h,$(ELEMENTS))
DEVHEADERS = $(addprefix $(DEV_INCLUDE_DIR)/,$(HEADERS))
ifdef DEBUG
$(info $(DEVHEADERS))
endif


# Where the source code is, and where "make" searches to find the source code.
SRC_DIR = $(PROJECT_DIR)/src
vpath %.c $(SRC_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.h $(SRC_DIR)


# In general, all compiles would have access to the headers in DEV_ROOT/include
CPPFLAGS += -I$(DEV_ROOT)/include


# Finally, the rules...
# First, the .PHONYs are run even if there are files existing with these names.
.PHONY : all clean

# The fundamental make would asure that the libraries and header files of this
# project are in the DEV_ROOT area, and that they are up to date.
all: $(DEVLIBS) $(DEVHEADERS)

# When we clean, we do not remove DEV_ROOT directories, because we don't know
# what other products have been installed there.
clean:
	$(RM) -rf $(PLATFORM_DIR) $(DEVHEADERS) $(DEVLIBS)

# Libraries of the platform area are dependent on their member elements, and
# dependent on the existance of the target platform directory.
$(PLATLIBS): $(PLATFORM_DIR)
$(PLATLIBS): $(PLATFORM_DIR)/%: $(PLATFORM_DIR)/%$(ARMEMBERLIST)

# Libraries and headers in the DEV_ROOT area are dependent on the cooresponding
# files in the platform areas, and dependent on the existence of the DEV_ROOT
# directories.
$(DEVLIBS): $(DEV_LIB_DIR)
$(DEVLIBS): $(DEV_LIB_DIR)/%: $(PLATFORM_DIR)/%
	cp $< $(@D)/.
$(DEVHEADERS): $(DEV_INCLUDE_DIR)
$(DEVHEADERS): $(DEV_INCLUDE_DIR)/%: $(SRC_DIR)/%
	cp $< $(@D)/.

# The recipe for non-existent directories is to simply make them.
$(DEV_LIB_DIR) $(DEV_INCLUDE_DIR) $(PLATFORM_DIR): 
	mkdir $(@);

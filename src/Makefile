#
# lib.org.sinkme.misc/Makefile.
# Ken Lamarche - 2012
#
# This Make file must only be used after a "TARGET" variable
# is set.  That variable can be set from a make file specific
# to the target.
#

CPPFLAGS = -I$(KL_EXPERIMENTS)/include
PROJECT = lib.org.sinkme.misc
SRC_DIR = $(KL_EXPERIMENTS)/$(PROJECT)/src
TARGET_DIR = $(KL_EXPERIMENTS)/$(PROJECT)/$(TARGET)

vpath %.c $(SRC_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.h $(SRC_DIR)

.PHONY : all clean

all : $(TARGET_DIR)/libmisc.so $(TARGET_DIR)/libmisc.a

$(TARGET_DIR)/libmisc.so: libmisc.so(streamOps.o tcpUtils.o)

$(TARGET_DIR)/libmisc.a: libmisc.a(streamOps.o tcpUtils.o)

#$(TARGET_DIR)/libmisc.so: libmisc.so(streamOps.o tcpUtils.o)
#	cp libmisc.so $@

#$(TARGET_DIR)/libmisc.a: libmisc.a(streamOps.o tcpUtils.o)
#	cp libmisc.a $@

# We don't need rules for the .a or .so archive, because the .o elements
# will be out-of-date if changes are made to the .c files, and with the
# .o files out-of-date, make knows that the archives are out-of-date
# also.

# However, we do have to rule up the .o files so that if the header files
# are touched, the .o files will be out-of-date:
streamOps.o: streamOps.h tcpUtils.h

tcpUtils.o: streamOps.h tcpUtils.h

clean :
# Interesting, the "$(RM)" is a predefined make variable, but $(CP) is not.
	$(RM) $(TARGET_DIR)/libmisc.so
	$(RM) $(TARGET_DIR)/libmisc.a
	$(RM) $(TARGET_DIR)/*.o



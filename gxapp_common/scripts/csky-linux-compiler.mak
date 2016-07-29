CROSS_COMPILE=csky-linux-
CC     = $(CROSS_COMPILE)gcc
CPP    = $(CROSS_COMPILE)g++
LD     = $(CROSS_COMPILE)gcc
AR     = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib
STRIP  = $(CROSS_COMPILE)strip

CFLAGS     +=-DLINUX_OS -mlittle-endian
#CFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
LIBS       = -lgxcore -lm

CROSS_COMPILE=arm-linux-
CC     = $(CROSS_COMPILE)gcc
CPP    = $(CROSS_COMPILE)g++
LD     = $(CROSS_COMPILE)gcc
AR     = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib

TARGET_DEFS=-DLINUX_OS
LDFLAGS =
LIBS    = -lpthread -lm
GXLIB_PATH=/opt/goxceed/$(ARCH)-$(OS)

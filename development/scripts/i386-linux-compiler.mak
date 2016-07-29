CC     = gcc
CPP    = g++
LD     = gcc
AR     = ar
RANLIB = ranlib

TARGET_DEFS=-DLINUX_OS
LDFLAGS =
LIBS    += -lpthread -lm
GXLIB_PATH=/opt/goxceed/$(ARCH)-$(OS)


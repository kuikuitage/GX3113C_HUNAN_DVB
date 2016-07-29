CROSS_COMPILE=csky-elf-
CC     = $(CROSS_COMPILE)gcc
CPP    = $(CROSS_COMPILE)g++
LD     = $(CROSS_COMPILE)gcc
AR     = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib
OBJCOPY= $(CROSS_COMPILE)objcopy

TARGET_DEFS=-DECOS_OS
#GXLIB_PATH=/opt/goxceed/$(ARCH)-$(OS)

ECOS_GLOBAL_LDFLAGS = -mno-thumb-interwork  -Wl,--gc-sections -Wl,-static -O2 -nostdlib

CFLAGS  += $(ECOS_GLOBAL_CFLAGS)
LDFLAGS += $(ECOS_GLOBAL_LDFLAGS) -nostartfiles -Ttarget.ld -ltarget -lgxcore  -lgxfrontend  -lgxav 



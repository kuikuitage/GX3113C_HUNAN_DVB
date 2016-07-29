##############################################################################
SUBDIRS=

SRC += $(shell ls *.c)
SRC += $(shell ls ./menu/*.c)
ifeq  ($(DVB_CA_1_NAME),)
else
SRC += $(shell ls ./cas/$(DVB_CA_1_NAME)/menu/*.c)
endif
ifeq  ($(DVB_CA_2_NAME),)
else
SRC += $(shell ls ./cas/$(DVB_CA_2_NAME)/menu/*.c)
endif
ifeq  ($(DVB_PVR_FLAG), yes)
SRC += $(shell ls ./pvr/*.c)
endif

ifeq  ($(GAME_SNAKE), yes)
SRC += $(shell ls ./game/*.c)
endif

ifeq  ($(FACTORY_MENU), yes)
SRC += $(shell ls ./factory_menu/*.c)
endif

ifeq  ($(DVB_SUBTITLE_FLAG), yes)
SRC += $(shell ls ./subtitle/*.c)
endif
ifeq  ($(FACTORY_SERIALIZATION_SUPPORT), yes)
SRC += $(shell ls ./factory_serialization/*.c)
endif
ifeq  ($(DVB_NETWORK_FLAG), yes)
SRC += $(shell ls ./network/*.c)
endif

ifeq  ($(DVB_MEDIA_FLAG), yes)
SRC += $(shell find ./media -iname "*.c")
endif
SRC += bsp.cpp

LIB = libapp.a
BIN = out.elf

CFLAGS += -I./include 
CFLAGS += -I$(GXLIB_PATH)/include/app
CFLAGS += -I$(GXLIB_PATH)/include/app/bsp
CFLAGS += -I$(GXLIB_PATH)/include/app/key 
CFLAGS += -I$(GXLIB_PATH)/include/app/book
CFLAGS += -I$(GXLIB_PATH)/include/app/epg
CFLAGS += -I$(GXLIB_PATH)/include/app/flash
CFLAGS += -I$(GXLIB_PATH)/include/app/init
ifeq  ($(DVB_MEDIA_FLAG), yes)
CFLAGS += -I$(GXLIB_PATH)/include/app/media
endif
CFLAGS += -I$(GXLIB_PATH)/include/app/play
CFLAGS += -I$(GXLIB_PATH)/include/app/porting
CFLAGS += -I$(GXLIB_PATH)/include/app/prog
CFLAGS += -I$(GXLIB_PATH)/include/app/search
CFLAGS += -I$(GXLIB_PATH)/include/app/table
ifeq  ($(DVB_SUBTITLE_FLAG), yes)
CFLAGS += -I$(GXLIB_PATH)/include/app/subt
endif

ifeq  ($(DVB_PVR_FLAG), yes)
CFLAGS += -I$(GXLIB_PATH)/include/app/pvr
endif
CFLAGS += -I$(GXLIB_PATH)/include/app/nvod
CFLAGS += -I$(GXLIB_PATH)/include/app/usb
CFLAGS += -I$(GXLIB_PATH)/include/app/spplayer
#CFLAGS += -I$(GXLIB_PATH)/include/app/gxbusca0
ifeq  ($(DVB_MEDIA_FLAG), yes)
CFLAGS += -I$(GXLIB_PATH)/include/app/media
endif
ifeq  ($(DVB_CA_1_NAME),)
else
CFLAGS += -I$(GXLIB_PATH)/include/app/cas/$(DVB_CA_1_NAME)/porting
CFLAGS += -I$(GXLIB_PATH)/include/app/cas/$(DVB_CA_1_NAME)/api
CFLAGS += -I./cas/$(DVB_CA_1_NAME)/include/menu
endif
ifeq  ($(DVB_CA_2_NAME),)
else
CFLAGS += -I$(GXLIB_PATH)/include/app/cas/$(DVB_CA_2_NAME)/porting
CFLAGS += -I$(GXLIB_PATH)/include/app/cas/$(DVB_CA_2_NAME)/api
CFLAGS += -I./cas/$(DVB_CA_2_NAME)/include/menu
endif

ifeq  ($(DVB_AD_NAME),)
else
CFLAGS += -I$(GXLIB_PATH)/include/app/ads/$(DVB_AD_NAME)/porting
CFLAGS += -I$(GXLIB_PATH)/include/app/ads/$(DVB_AD_NAME)/api
endif

ifeq  ($(DVB_NETWORK_FLAG), yes)
CFLAGS += -I./network
endif

ifeq  ($(DVB_MEDIA_FLAG), yes)
CFLAGS += -I./media/include
endif

CFLAGS  += -I$(GXLIB_PATH)/include/gx_gui
CFLAGS  += -I$(GXLIB_PATH)/include/bus
CFLAGS  += -I$(GXLIB_PATH)/include/bus/module/pm
CFLAGS  += -I$(GXLIB_PATH)/include/bus/module/config
CFLAGS  += -I$(GXLIB_PATH)/include/bus/module/sys_info
CFLAGS  += -I$(GXLIB_PATH)/include/bus/module/player
CFLAGS  += -I$(GXLIB_PATH)/include/bus/service
CFLAGS  += -I$(GXLIB_PATH)/include/bus/gui_core
CFLAGS  += -I$(GXLIB_PATH)/include/bus/module/ca
 CFLAGS  += -I$(GXLIB_PATH)/include/frontend
##############################################################################
-include  $(GXSRC_PATH)/scripts/inc.Makefile
##############################################################################
LDFLAGS += -L$(GXLIB_PATH)/include/app/lib
ifeq  ($(DVB_AD_NAME),)
else
ifeq ($(DVB_AD_LIB),)
else
LDFLAGS +=  -l$(DVB_AD_LIB)
endif
endif
ifeq  ($(DVB_CA_1_NAME),)
else
ifeq ($(DVB_CA_1_LIB),)
else
LDFLAGS +=  -l$(DVB_CA_1_LIB)
endif
endif
ifeq  ($(DVB_CA_2_NAME),)
else
ifeq ($(DVB_CA_2_LIB),)
else
LDFLAGS +=  -l$(DVB_CA_2_LIB)
endif
endif
ifeq ($(OS), ecos)
LDFLAGS +=  -lappcommon -lui -lxml -lgxplayer -lenca -ldvbs -lgxfrontend -lfreetype -ljpeg -lpng -ldb -lgxav -lgxca -lz -lcyassl
endif
ifeq ($(OS), linux)
	LDFLAGS +=  -lappcommon -lui -lgxplayer -lenca -ldvbs -lfreetype -ljpeg -lpng -ldb -lgxca -lxml
endif
ifeq  ($(DVB_AD_NAME),)
else
ifeq    ($(DVB_AD_LIB),)
else
LDFLAGS +=  -l$(DVB_AD_LIB)
endif
endif
ifeq  ($(DVB_CA_1_NAME),)
else
ifeq	($(DVB_CA_1_LIB),)
else
LDFLAGS +=  -l$(DVB_CA_1_LIB)
endif
endif
ifeq  ($(DVB_CA_2_NAME),)
else
ifeq	($(DVB_CA_2_LIB),)
else
LDFLAGS += -l$(DVB_CA_2_LIB)
endif
endif

LDFLAGS +=  -lappcommon

#test2:
#	@echo $(LDFLAGS)
#test: 
#	@echo making test begin...	
#	$(CC)  main.c  -o test \
#	$(CFLAGS)                       \
#	-L$(GXLIB_PATH)/lib -lservice -L./ -lapp $(LDFLAGS) -lui   

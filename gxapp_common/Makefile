##############################################################################
SUBDIRS=

SRC += $(shell ls ./table/*.c)
SRC += $(shell ls ./book/*.c)
SRC += $(shell ls ./play/app_common_play.c)
ifeq  ($(AUDIO_DOLBY), yes)
SRC += $(shell ls ./play/app_common_dolby.c)
SRC += $(shell ls ./play/gx3201_dcv_main.c)
endif
SRC += $(shell ls ./nvod/*.c)
SRC += $(shell ls ./flash/*.c)
SRC += $(shell ls ./search/*.c)
SRC += $(shell ls ./prog/*.c)
SRC += $(shell ls ./epg/*.c)
SRC += $(shell ls ./init/*.c)
SRC += $(shell ls ./osd_msg/*.c)
#SRC += $(shell ls ./common/usb/*.c)
SRC += $(shell ls ./porting/*.c)

ifeq  ($(DVB_SUBTITLE_FLAG), yes)
SRC += $(shell ls ./subt/*.c)
endif

ifeq  ($(DVB_PVR_FLAG), yes)
SRC += $(shell ls ./pvr/*.c)
endif
ifeq  ($(DVB_SERIAL_SUPPORT), yes)
SRC += $(shell ls ./serial/*.c)
endif


#SRC += $(shell ls ./gxbusca0/*.c)
#SRC += $(shell ls ./gxbusca0/module/*.c)
ifeq  ($(DVB_CA_1_NAME),)
else
SRC += $(shell ls ./cas/$(DVB_CA_1_NAME)/$(DVB_CA_MARKET)/api/*.c)
SRC += $(shell ls ./cas/$(DVB_CA_1_NAME)/$(DVB_CA_MARKET)/porting/*.c)
endif
ifeq  ($(DVB_CA_2_NAME),)
else
SRC += $(shell ls ./cas/$(DVB_CA_2_NAME)/$(DVB_CA_MARKET)/api/*.c)
SRC += $(shell ls ./cas/$(DVB_CA_2_NAME)/$(DVB_CA_MARKET)/porting/*.c)
endif

ifeq ($(OS), ecos)
SRC += $(shell ls ./bsp/*.c)
endif
SRC += $(shell ls ./bsp/panel_common/*.c)

ifeq  ($(DVB_AUTO_TEST_FLAG), yes)
SRC += $(shell find ./autotest -iname "*.c")
endif

ifeq  ($(DVB_MEDIA_FLAG), yes)
SRC += $(shell find ./media -iname "*.c")
endif

ifeq  ($(DVB_AD_NAME),)
else
SRC += $(shell ls ./ads/$(DVB_AD_NAME)/$(DVB_AD_MARKET)/porting/*.c)
SRC += $(shell ls ./ads/$(DVB_AD_NAME)/$(DVB_AD_MARKET)/api/*.c)
endif

ifeq  ($(DVB_AD_NAME_1),)
else
SRC += $(shell ls ./ads/$(DVB_AD_NAME_1)/$(DVB_AD_MARKET_1)/porting/*.c)
SRC += $(shell ls ./ads/$(DVB_AD_NAME_1)/$(DVB_AD_MARKET_1)/api/*.c)
endif

SRC += $(shell ls ./spplayer/*.c)

#SRC += bsp.cpp

LIB = libappcommon.a
#BIN = outcommon.elf

#CFLAGS  += -I$(GXAPP_PATH)/app/include
CFLAGS += -I./include
CFLAGS += -I./include/key
CFLAGS += -I./include/bsp 
CFLAGS += -I./include/book
CFLAGS += -I./include/epg
CFLAGS += -I./include/flash
CFLAGS += -I./include/init
ifeq  ($(DVB_AUTO_TEST_FLAG), yes)
CFLAGS += -I./autotest/include
endif
ifeq ($(DVB_MEDIA_FLAG), yes)
CFLAGS += -I./include/media
endif
CFLAGS += -I./include/play
CFLAGS += -I./include/porting
CFLAGS += -I./include/prog
CFLAGS += -I./include/search
CFLAGS += -I./include/table
CFLAGS += -I./include/nvod
CFLAGS += -I./include/usb
CFLAGS += -I./include/osd_msg
#CFLAGS += -I./include/gxbusca0
#ifeq  ($(DVB_MEDIA_FLAG), yes)
CFLAGS += -I./media/include
#endif
ifeq  ($(LINUX_OTT_SUPPORT), yes)
CFLAGS += -I./include/player_include
endif
ifeq  ($(DVB_SUBTITLE_FLAG), yes)
CFLAGS += -I./include/subt
endif

ifeq  ($(DVB_PVR_FLAG), yes)
CFLAGS += -I./include/pvr
endif
ifeq  ($(DVB_SERIAL_SUPPORT), yes)
CFLAGS += -I./include/serial
endif


ifeq  ($(DVB_CA_1_NAME),)
else
CFLAGS += -I./cas/$(DVB_CA_1_NAME)/$(DVB_CA_MARKET)/include/porting
CFLAGS += -I./cas/$(DVB_CA_1_NAME)/$(DVB_CA_MARKET)/include/api
endif

ifeq  ($(DVB_CA_2_NAME),)
else
CFLAGS += -I./cas/$(DVB_CA_2_NAME)/$(DVB_CA_MARKET)/include/porting
CFLAGS += -I./cas/$(DVB_CA_2_NAME)/$(DVB_CA_MARKET)/include/api
endif

ifeq  ($(DVB_AD_NAME),)
else
CFLAGS += -I./ads/$(DVB_AD_NAME)/$(DVB_AD_MARKET)/include/porting
CFLAGS += -I./ads/$(DVB_AD_NAME)/$(DVB_AD_MARKET)/include/api
endif

ifeq  ($(DVB_AD_NAME_1),)
else
CFLAGS += -I./ads/$(DVB_AD_NAME_1)/$(DVB_AD_MARKET_1)/include/porting
CFLAGS += -I./ads/$(DVB_AD_NAME_1)/$(DVB_AD_MARKET_1)/include/api
endif

CFLAGS += -I./include/spplayer

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
#CFLAGS  += -I$(GXLIB_PATH)/include/bus/module/ca0
##############################################################################
-include  $(GXSRC_PATH)/scripts/inc.Makefile
##############################################################################
#ADS 1
ifeq  ($(DVB_AD_NAME),)
else
LDFLAGS += -L./ads/$(DVB_AD_NAME)/$(DVB_AD_MARKET)/lib
ifeq ($(DVB_AD_LIB),)
else
	LDFLAGS +=  -l$(DVB_AD_LIB)
endif 
endif 
#ADS 2
ifeq  ($(DVB_AD_NAME_1),)
else
	LDFLAGS += -L./ads/$(DVB_AD_NAME_1)/$(DVB_AD_MARKET_1)/lib
ifeq ($(DVB_AD_LIB_1),)
else
        LDFLAGS +=  -l$(DVB_AD_LIB_1)
endif
endif


ifeq  ($(DVB_CA_1_NAME),)
else
LDFLAGS += -L./cas/$(DVB_CA_1_NAME)/$(DVB_CA_MARKET)/lib
ifeq ($(DVB_CA_1_LIB),)
else
	LDFLAGS +=  -l$(DVB_CA_1_LIB)
endif
endif
ifeq  ($(DVB_CA_2_NAME),)
else
LDFLAGS += -L./cas/$(DVB_CA_2_NAME)/$(DVB_CA_MARKET)/lib
ifeq ($(DVB_CA_2_LIB),)
else
	LDFLAGS += -l$(DVB_CA_2_LIB)
endif
endif

ifeq ($(OS), ecos)
LDFLAGS +=  -lui -lgxplayer -ldvbs -lgxfrontend -lfreetype -ljpeg -lpng -ldb -lgxav -lfirmware  -lgxca -lcyassl
endif
ifeq ($(OS), linux)
	LDFLAGS +=  -lui -lgxplayer -ldvbs -lfreetype -ljpeg -lpng -ldb -lgxca
endif
#test: 
#	@echo making test begin...	
#	$(CC)  main.c  -o test \
#	$(CFLAGS)                       \
#	-L$(GXLIB_PATH)/lib -lservice -L./ -lapp $(LDFLAGS) -lui   

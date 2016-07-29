-include $(GXSRC_PATH)/scripts/$(ARCH)-$(OS)-compiler.mak

ifeq ($(ENABLE_MEMWATCH), yes)
	CFLAGS += -DMEMWATCH
endif

ifeq ($(debug),yes)
CFLAGS += -pipe -O0 -g -D_DEBUG
else
CFLAGS += -pipe -O2 -g 
endif

MAKE    += -s
MAKEFLAGS += -s
ifeq ($(OS), ecos)
#WARNING +=-Werror
WARNING += -Wundef -Wall
CFLAGS  += -I$(GXLIB_PATH)/include -I$(GXLIB_PATH)/include/bus \
	   $(TARGET_DEFS) -DARCH=$(ARCH) $(WARNING) -I. -I./include
#$(TARGET_DEFS) -DARCH=$(ARCH) $(WARNING) -I. -I./cdcas/include
LIBS += -lgxcore
endif

ifeq ($(OS), linux)
CFLAGS  += -Wall
CFLAGS  += -g -O0 -DMEMORY_DEBUG -I$(GXLIB_PATH)/include -I$(GXLIB_PATH)/include/bus  $(TARGET_DEFS) -DARCH=$(ARCH) $(WARNING) -I.
CFLAGS += -I$(GXLIB_PATH)/include/kernel_include/
LIBS += -lgxcore -lz -lrt -Wl --whole-archive -lpthread -Wl --no-whole-archive
LDFLAGS += -static

endif

LDFLAGS += -L$(GXLIB_PATH)/lib 

OBJS=$(addprefix objects/, $(addsuffix .o, $(basename $(notdir $(SRC)))))

all: conf panel env  $(BEFORE) deps objects $(OBJS) $(LIB) 
conf:
	echo "#ifndef _GXAPP_SYS_CONFIG_H_"> include/gxapp_sys_config.h
	echo "#define _GXAPP_SYS_CONFIG_H_">> include/gxapp_sys_config.h
	echo "  ">> include/gxapp_sys_config.h
	echo "  ">> include/gxapp_sys_config.h
	echo '#include "remote_enum.h"'>> include/gxapp_sys_config.h
	echo '#include "panel_enum.h"'>> include/gxapp_sys_config.h
	echo '#include "demod_enum.h"'>> include/gxapp_sys_config.h
	echo '#include "ota_enum.h"'>> include/gxapp_sys_config.h
	echo '#include "goxceed_csky.h"'>> include/gxapp_sys_config.h
	echo '#include "chip_enum.h"'>> include/gxapp_sys_config.h
	echo '#include "theme_enum.h"'>> include/gxapp_sys_config.h
	echo '#include "tuner_enum.h"'>> include/gxapp_sys_config.h


ifeq ($(OS), linux)
	echo '#define WORK_PATH $(DVB_WORK_PATH)'>> include/gxapp_sys_config.h
	echo '#define LOGO_PATH $(DVB_LOGO_PATH)'>> include/gxapp_sys_config.h
	echo '#define I_FRAME_PATH $(DVB_I_FRAME_PATH)'>> include/gxapp_sys_config.h
endif
ifeq ($(OS), ecos)
	echo '#define LOGO_PATH $(DVB_LOGO_PATH)'>> include/gxapp_sys_config.h
	echo '#define I_FRAME_PATH $(DVB_I_FRAME_PATH)'>> include/gxapp_sys_config.h
endif
	echo "  ">> include/gxapp_sys_config.h
	echo "  ">> include/gxapp_sys_config.h

ifeq  ($(DVB_CA_FLAG), yes)
	echo "#define CA_FLAG ">> include/gxapp_sys_config.h
endif
ifeq  ($(MOVE_FUNCTION_FLAG), yes)
	echo "#define MOVE_FUNCTION ">> include/gxapp_sys_config.h
endif
ifeq  ($(DVB_CA_FREE_STOP), yes)
	echo "#define DVB_CA_FREE_STOP ">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_SERIAL_SUPPORT), yes)
	echo "#define DVB_SERIAL_SUPPORT ">> include/gxapp_sys_config.h
endif
ifeq  ($(FACTORY_SERIALIZATION_SUPPORT), yes)
	echo "#define FACTORY_SERIALIZATION_SUPPORT  1">> include/gxapp_sys_config.h
else
	echo "#define FACTORY_SERIALIZATION_SUPPORT  0">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_CA_1_NAME),)
else
	echo '#define DVB_CA_DV $(DVB_CA_TYPE)'>> include/gxapp_sys_config.h
	echo '#define DVB_CA_1_NAME "$(DVB_CA_1_NAME)"'>> include/gxapp_sys_config.h
	echo '#define $(DVB_CA_1_FLAG)'>> include/gxapp_sys_config.h
endif
ifeq  ($(DVB_CA_2_NAME),)
else
	echo '#define DVB_CA_2_NAME "$(DVB_CA_2_NAME)"'>> include/gxapp_sys_config.h
	echo '#define $(DVB_CA_2_FLAG)'>> include/gxapp_sys_config.h
endif
	echo "  ">> include/gxapp_sys_config.h
	echo "  ">> include/gxapp_sys_config.h

ifeq  ($(DVB_AD_NAME),)
else
	echo '#define  AD_FLAG'>> include/gxapp_sys_config.h
	echo '#define  $(DVB_AD_FLAG)'>> include/gxapp_sys_config.h
endif

ifeq ($(DVB_AD_NAME_1),)
else
	echo '#define  AD_FLAG'>> include/gxapp_sys_config.h
	echo '#define $(DVB_AD_FLAG_1)'>> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_USB_FLAG), yes)
	echo "#define USB_FLAG ">> include/gxapp_sys_config.h
endif
	
	echo '#define $(CUSTOMER)'>> include/gxapp_sys_config.h

ifeq  ($(DVB_JPG_LOGO), yes)
	echo "#define DVB_JPG_LOGO 1">> include/gxapp_sys_config.h
else
	echo "#define DVB_JPG_LOGO 0">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_HD_LIST), yes)
	echo "#define DVB_HD_LIST 1">> include/gxapp_sys_config.h
else
	echo "#define DVB_HD_LIST 0">> include/gxapp_sys_config.h
endif



ifeq ($(OS), linux)
	echo '#define WORK_PATH $(DVB_WORK_PATH)'>> include/gxapp_sys_config.h
	echo '#define LOGO_PATH $(DVB_LOGO_PATH)'>> include/gxapp_sys_config.h
	echo '#define I_FRAME_PATH $(DVB_I_FRAME_PATH)'>> include/gxapp_sys_config.h
endif

ifeq  ($(AUDIO_DOLBY), yes)
	echo "#define AUDIO_DOLBY 1">> include/gxapp_sys_config.h
else
	echo "#define AUDIO_DOLBY 0">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_PVR_FLAG), yes)
	echo "#define DVB_PVR_FLAG 1">> include/gxapp_sys_config.h
else
	echo "#define DVB_PVR_FLAG 0">> include/gxapp_sys_config.h
endif


ifeq  ($(DVB_MEDIA_FLAG), yes)
	echo "#define DVB_MEDIA_FLAG 1">> include/gxapp_sys_config.h
else
	echo "#define DVB_MEDIA_FLAG 0">> include/gxapp_sys_config.h
endif


ifeq  ($(DVB_ZOOM_RESTART_PLAY), yes)
	echo "#define DVB_ZOOM_RESTART_PLAY 1">> include/gxapp_sys_config.h
else
	echo "#define DVB_ZOOM_RESTART_PLAY 0">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_BAD_SIGNAL_SHOW_LOGO), yes)
	echo "#define DVB_BAD_SIGNAL_SHOW_LOGO 1">> include/gxapp_sys_config.h
else
	echo "#define DVB_BAD_SIGNAL_SHOW_LOGO 0">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_NETWORK_FLAG), yes)
	echo "#define DVB_NETWORK_FLAG 1">> include/gxapp_sys_config.h
else
	echo "#define DVB_NETWORK_FLAG 0">> include/gxapp_sys_config.h
endif

ifeq ($(LINUX_OTT_SUPPORT),yes)
	echo "#define LINUX_OTT_SUPPORT 1">> include/gxapp_sys_config.h
else	
	echo "#define LINUX_OTT_SUPPORT 0">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_SUBTITLE_FLAG), yes)
	echo "#define MEDIA_SUBTITLE_SUPPORT 1">> include/gxapp_sys_config.h
else
	echo "#define MEDIA_SUBTITLE_SUPPORT 0">> include/gxapp_sys_config.h
endif

ifeq  ($(LOGO_SHOW_DELAY),yes)
	echo '#define LOGO_SHOW_DELAY 1'>> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_32MB),yes)
	echo '#define DVB_32MB 1'>> include/gxapp_sys_config.h
endif

ifeq  ($(CHIP),gx3113c)
	echo "#define DVB_CHIP_TYPE GX_CHIP_3113C">> include/gxapp_sys_config.h
	echo "#define DVB_DEFINITION_TYPE SD_DEFINITION">> include/gxapp_sys_config.h
endif

ifeq  ($(CHIP),gx3115)
	echo "#define DVB_CHIP_TYPE GX_CHIP_3115">> include/gxapp_sys_config.h
	echo "#define DVB_DEFINITION_TYPE SD_DEFINITION">> include/gxapp_sys_config.h
endif

ifeq  ($(CHIP),gx3201)
	echo "#define DVB_CHIP_TYPE GX_CHIP_3201">> include/gxapp_sys_config.h
	echo "#define DVB_DEFINITION_TYPE HD_DEFINITION">> include/gxapp_sys_config.h
endif


ifeq  ($(CHIP),gx3113h)
	echo "#define DVB_CHIP_TYPE GX_CHIP_3113H">> include/gxapp_sys_config.h
	echo "#define DVB_DEFINITION_TYPE HD_DEFINITION">> include/gxapp_sys_config.h
endif

ifeq  ($(CHIP),gx3201h)
	echo "#define DVB_CHIP_TYPE GX_CHIP_3201H">> include/gxapp_sys_config.h
	echo "#define DVB_DEFINITION_TYPE HD_DEFINITION">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_THEME),HD)
	echo "#define DVB_THEME_TYPE DVB_THEME_HD">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_THEME),SD)
	echo "#define DVB_THEME_TYPE DVB_THEME_SD">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_THEME),DTMB_MINI_HD)
	echo "#define MENCENT_FREEE_SPACE">> include/gxapp_sys_config.h
	echo "#define DVB_THEME_TYPE DVB_THEME_DTMB_MINI_HD">> include/gxapp_sys_config.h
endif

ifeq  ($(DVB_THEME),DTMB_HD)
	echo "#define DVB_THEME_TYPE DVB_THEME_DTMB_HD">> include/gxapp_sys_config.h
endif
	echo "  ">> include/gxapp_sys_config.h
	echo "  ">> include/gxapp_sys_config.h
	echo '#define DVB_CUSTOM "${DVB_CUSTOM}"'>> include/gxapp_sys_config.h
	echo '#define DVB_MARKET "${DVB_MARKET}"'>> include/gxapp_sys_config.h
	echo '#define DVB_DEMOD_TYPE $(DVB_DEMOD_TYPE)'>> include/gxapp_sys_config.h
	echo '#define DVB_DEMOD_MODE $(DVB_DEMOD_MODE)'>> include/gxapp_sys_config.h
	echo '#define DVB_TS_SRC $(DVB_TS_SRC)'>> include/gxapp_sys_config.h	
	
	echo "  ">> include/gxapp_sys_config.h
	echo '#define TUNER_TYPE $(DVB_TUNER_TYPE)'>> include/gxapp_sys_config.h
	echo '#define REMOTE_TYPE $(DVB_KEY_TYPE)'>> include/gxapp_sys_config.h
	echo "  ">> include/gxapp_sys_config.h
	echo '#define PANEL_TYPE $(DVB_PANEL_TYPE)'>> include/gxapp_sys_config.h
	echo '#define PANEL_CLK_GPIO $(PANEL_CLK_GPIO)'>> include/gxapp_sys_config.h
	echo '#define PANEL_DATA_GPIO $(PANEL_DATA_GPIO)'>> include/gxapp_sys_config.h
	echo '#define PANEL_STANDBY_GPIO $(PANEL_STANDBY_GPIO)'>> include/gxapp_sys_config.h
	echo '#define PANEL_LOCK_GPIO $(PANEL_LOCK_GPIO)'>> include/gxapp_sys_config.h
	echo "  ">> include/gxapp_sys_config.h
	echo '#define DVB_OTA_TYPE $(DVB_OTA_TYPE)'>> include/gxapp_sys_config.h

	echo "  ">> include/gxapp_sys_config.h
	echo "  ">> include/gxapp_sys_config.h

	echo '#define APP_$(DVB_RESOLUTION) 1'>> include/gxapp_sys_config.h
	echo '#define SYSC_MODE $(DVB_SYS_MODE)'>> include/gxapp_sys_config.h
	echo '#define VIDEO_WINDOW_X $(DVB_VIDEO_X)'>> include/gxapp_sys_config.h	
	echo '#define VIDEO_WINDOW_Y $(DVB_VIDEO_Y)'>> include/gxapp_sys_config.h
	echo '#define VIDEO_WINDOW_W $(DVB_VIDEO_W)'>> include/gxapp_sys_config.h
	echo '#define VIDEO_WINDOW_H $(DVB_VIDEO_H)'>> include/gxapp_sys_config.h
	echo '#define SAT_MAX_NUM $(DVB_SAT_MAX)'>> include/gxapp_sys_config.h
	echo '#define TP_MAX_NUM $(DVB_TP_MAX)'>> include/gxapp_sys_config.h
	echo '#define SERVICE_MAX_NUM $(DVB_SERVICE_MAX)'>> include/gxapp_sys_config.h
	echo '#define DVB_DDRAM_SIZE $(DVB_DDRAM_SIZE)'>> include/gxapp_sys_config.h
	echo '#define PAT_FILTER_TIMEOUT $(DVB_PAT_TIMEOUT)'>> include/gxapp_sys_config.h	
	echo '#define SDT_FILTER_TIMEOUT $(DVB_SDT_TIMEOUT)'>> include/gxapp_sys_config.h
	echo '#define NIT_FILTER_TIMEOUT $(DVB_NIT_TIMEOUT)'>> include/gxapp_sys_config.h
	echo '#define PMT_FILTER_TIMEOUT $(DVB_PMT_TIMEOUT)'>> include/gxapp_sys_config.h
	echo '#define TRICK_PLAY_SUPPORT $(DVB_PVR_SPEED_SUPPORT)'>> include/gxapp_sys_config.h
	echo '#define PROG_MAX_LCN $(DVB_LCN_DEFAULT)'>> include/gxapp_sys_config.h
	echo '#define DVB_CENTER_FRE $(DVB_CENTER_FRE)'>> include/gxapp_sys_config.h
	echo '#define DVB_CENTER_SYMRATE $(DVB_CENTER_SYMRATE)'>> include/gxapp_sys_config.h
	echo '#define DVB_CENTER_QAM $(DVB_CENTER_QAM)'>> include/gxapp_sys_config.h	
	echo '#define DVB_CENTER_BANDWIDTH $(DVB_CENTER_BANDWIDTH)'>> include/gxapp_sys_config.h
ifeq  ($(DVB_DUAL_MODE), yes)
	echo "#define DVB_DUAL_MODE 1 ">> include/gxapp_sys_config.h
else
	echo "#define DVB_DUAL_MODE 0 ">> include/gxapp_sys_config.h
endif

	echo "  ">> include/gxapp_sys_config.h
	echo "  ">> include/gxapp_sys_config.h
ifeq  ($(DVB_AUTO_TEST_FLAG), yes)
	echo "#define DVB_AUTO_TEST_FLAG ">> include/gxapp_sys_config.h
endif
	echo "#ifndef _GXAPPCOMMON_VERSION_H_"> include/gxappcommon_version.h
	echo "#define _GXAPPCOMMON_VERSION_H_">> include/gxappcommon_version.h
	echo "  ">> include/gxappcommon_version.h
	echo "  ">> include/gxappcommon_version.h
	echo "#define GXAPPCOMMON_BUILD \"./build $(CHIP) $(DEMOD) $(DVB_CUSTOM)_$(DVB_MARKET) $(OS)\""  >> include/gxappcommon_version.h
	
	if [ -f .svn/entries ] ; then \
                echo "#define GXAPPCOMMON_SVN `sed -n -e 11p .svn/entries`" >> include/gxappcommon_version.h; \
                echo "#define GXAPPCOMMON_SVN_URL \"`sed -n -e 5p .svn/entries`\"" >> include/gxappcommon_version.h; \
   	 fi;

	echo "  ">> include/gxappcommon_version.h
	echo "  ">> include/gxappcommon_version.h
	if [ -f $(GXSRC_PATH)/../.git/HEAD ] ; then \
                echo "#define GXAPPCOMMON_GIT `git rev-list HEAD | wc -l | awk '{print $1}'`" >> include/gxappcommon_version.h; \
                echo "#define GXAPPCOMMON_GIT_VER \"`git rev-list HEAD -n 1 | cut -c 1-10`\"" >> include/gxappcommon_version.h; \
   	 fi;
	echo "  ">> include/gxappcommon_version.h
	echo "  ">> include/gxappcommon_version.h
	echo "#endif">> include/gxappcommon_version.h

	
	echo "#endif">> include/gxapp_sys_config.h
	
panel:
	echo " linux compile bsp  "
ifeq ($(OS), linux)
	make  -C  $(GXSRC_PATH)/bsp
endif
	
env:
ifndef GXLIB_PATH
	$(error Error: you must set the GXLIB_PATH environment variable to point to your gxsoft Path.)
endif
	@-rm -f *.a;
#	@-rm -f *.elf;	
#	@sh $(GXSRC_PATH)/scripts/create_signal_connect.sh
#	@cd $(GXSRC_PATH)/theme/image && sh create_image.sh && cd -
#	@cd $(GXSRC_PATH)/theme/widget && sh create_widget.sh && cd -
	
# automatic generation of all the rules written by vincent by hand.	
deps: $(SRC) Makefile
	@echo "Generating new dependency file...";
	@-rm -f deps;
	@for f in $(SRC); do \
		OBJ=objects/`basename $$f|sed -e 's/\.cpp/\.o/' -e 's/\.cxx/\.o/' -e 's/\.cc/\.o/' -e 's/\.c/\.o/'`; \
		echo $$OBJ: $$f>> deps; \
		echo '	@echo -e "compiling \033[032m[$(CC)]\033[0m": ' $$f >> deps; \
		echo '	@$(CC) $$(CFLAGS) -c -o $$@ $$^'>> deps; \
	done
	
-include ./deps
#-include ./$(ARCH)-$(OS)-compiler.mak

objects:
	@mkdir objects
.PHONY: madlib

$(LIB): objects $(OBJS)
	$(AR) r $@ $(OBJS)
	$(RANLIB) $@
	@-rm -rf $(GXLIB_PATH)/include/app;
	@-rm -f $(GXLIB_PATH)/lib/$(LIB);
	cp -ar $(GXSRC_PATH)/include $(GXLIB_PATH)/include/app
	cp -ar $(GXSRC_PATH)/media/include/* $(GXLIB_PATH)/include/app/media
	find $(GXLIB_PATH)/include/app/ -type d -name ".svn" | xargs rm -rf
	@echo "install $(LIB)" "->" "$(INSTALL)$(GXLIB_PATH)/include/app/lib" 
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/lib"
	@install -m 644 $(LIB) "$(INSTALL)$(GXLIB_PATH)/include/app/lib"
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/key_xml"
#	@install -m 644 $(GXSRC_PATH)/key_xml/keymap.xml "$(INSTALL)$(GXLIB_PATH)/include/app/key_xml/$(DVB_KEY_TYPE).xml"
#	cp $( grep -ril "KEY_GONGBAN_NATIONALCHIP_NEW" $(GXSRC_PATH)/key_xml/|grep -v svn) $(GXLIB_PATH)/include/app/key_xml/$(DVB_KEY_TYPE).xml
	grep -rl $(DVB_KEY_TYPE) $(GXSRC_PATH)/key_xml/|xargs -i install -m 644 {} $(INSTALL)$(GXLIB_PATH)/include/app/key_xml/$(DVB_KEY_TYPE).xml	
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/sh"
	@install -m 644 $(GXSRC_PATH)/env.sh "$(INSTALL)$(GXLIB_PATH)/include/app/sh"		
ifeq ($(OS), linux)
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/linux"
	cp $(GXSRC_PATH)/bsp/panel.ko $(INSTALL)$(GXLIB_PATH)/include/app/linux/
	cp $(GXSRC_PATH)/linux/S05panel $(INSTALL)$(GXLIB_PATH)/include/app/linux/
	chmod 777 $(INSTALL)$(GXLIB_PATH)/include/app/linux/*
#	cp $(GXSRC_PATH)/linux/S04frontend_gx1001_$(DVB_TUNER_TYPE) $(INSTALL)$(GXLIB_PATH)/include/app/linux/S04frontend
endif

ifeq ($(DVB_LOGO_JPG), )
else
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/logo-jpg"
	cp $(GXSRC_PATH)/logo-jpg/$(DVB_LOGO_JPG) $(INSTALL)$(GXLIB_PATH)/include/app/logo-jpg/
endif	
#ADS 1
ifeq  ($(DVB_AD_NAME),)
else
ifeq ($(DVB_AD_LIB), )
else	
	@install -m 644 $(GXSRC_PATH)/ads/$(DVB_AD_NAME)/$(DVB_AD_MARKET)/lib/lib$(DVB_AD_LIB).a "$(INSTALL)$(GXLIB_PATH)/include/app/lib"
endif
endif	
ifeq  ($(DVB_AD_NAME),)
else
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/ads/$(DVB_AD_NAME)/porting"
	@install -m 644 $(GXSRC_PATH)/ads/$(DVB_AD_NAME)/$(DVB_AD_MARKET)/include/porting/*.h "$(INSTALL)$(GXLIB_PATH)/include/app/ads/$(DVB_AD_NAME)/porting"
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/ads/$(DVB_AD_NAME)/api"
	@install -m 644 $(GXSRC_PATH)/ads/$(DVB_AD_NAME)/$(DVB_AD_MARKET)/include/api/*.h "$(INSTALL)$(GXLIB_PATH)/include/app/ads/$(DVB_AD_NAME)/api"
endif
#ADS 2
ifeq  ($(DVB_AD_NAME_1),)
else
ifeq ($(DVB_AD_LIB_1), )
else
	@install -m 644 $(GXSRC_PATH)/ads/$(DVB_AD_NAME_1)/$(DVB_AD_MARKET_1)/lib/lib$(DVB_AD_LIB_1).a "$(INSTALL)$(GXLIB_PATH)/include/app/lib"
endif
endif
ifeq  ($(DVB_AD_NAME_1),)
else
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/ads/$(DVB_AD_NAME_1)/porting"
	@install -m 644 $(GXSRC_PATH)/ads/$(DVB_AD_NAME_1)/$(DVB_AD_MARKET_1)/include/porting/*.h "$(INSTALL)$(GXLIB_PATH)/include/app/ads/$(DVB_AD_NAME_1)/porting"			        
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/ads/$(DVB_AD_NAME_1)/api"
	@install -m 644 $(GXSRC_PATH)/ads/$(DVB_AD_NAME_1)/$(DVB_AD_MARKET_1)/include/api/*.h "$(INSTALL)$(GXLIB_PATH)/include/app/ads/$(DVB_AD_NAME_1)/api"
endif


ifeq  ($(DVB_CA_1_NAME),)
else
ifeq ($(DVB_CA_1_LIB), )
else	
	@install -m 644 $(GXSRC_PATH)/cas/$(DVB_CA_1_NAME)/$(DVB_CA_MARKET)/lib/lib$(DVB_CA_1_LIB).a "$(INSTALL)$(GXLIB_PATH)/include/app/lib"
endif
endif
ifeq  ($(DVB_CA_2_NAME),)
else
ifeq ($(DVB_CA_2_LIB), )
else
	@install -m 644 $(GXSRC_PATH)/cas/$(DVB_CA_2_NAME)/$(DVB_CA_MARKET)/lib/lib$(DVB_CA_2_LIB).a "$(INSTALL)$(GXLIB_PATH)/include/app/lib"
endif
endif		
ifeq  ($(DVB_CA_1_NAME),)
else
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/cas/$(DVB_CA_1_NAME)/porting"
	@install -m 644 $(GXSRC_PATH)/cas/$(DVB_CA_1_NAME)/$(DVB_CA_MARKET)/include/porting/*.h "$(INSTALL)$(GXLIB_PATH)/include/app/cas/$(DVB_CA_1_NAME)/porting"
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/cas/$(DVB_CA_1_NAME)/api"
	@install -m 644 $(GXSRC_PATH)/cas/$(DVB_CA_1_NAME)/$(DVB_CA_MARKET)/include/api/*.h "$(INSTALL)$(GXLIB_PATH)/include/app/cas/$(DVB_CA_1_NAME)/api"
endif
ifeq  ($(DVB_CA_2_NAME),)
else
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/cas/$(DVB_CA_2_NAME)/porting"
	@install -m 644 $(GXSRC_PATH)/cas/$(DVB_CA_2_NAME)/$(DVB_CA_MARKET)/include/porting/*.h "$(INSTALL)$(GXLIB_PATH)/include/app/cas/$(DVB_CA_2_NAME)/porting"
	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/cas/$(DVB_CA_2_NAME)/api"
	@install -m 644 $(GXSRC_PATH)/cas/$(DVB_CA_2_NAME)/$(DVB_CA_MARKET)/include/api/*.h "$(INSTALL)$(GXLIB_PATH)/include/app/cas/$(DVB_CA_2_NAME)/api"
endif	


	@install -d "$(INSTALL)$(GXLIB_PATH)/include/app/flashconf"
	cp -rf $(GXSRC_PATH)/flashconf/file_tree $(INSTALL)$(GXLIB_PATH)/include/app/flashconf
	cp -rf $(GXSRC_PATH)/flashconf/genflash $(INSTALL)$(GXLIB_PATH)/include/app/flashconf
	cp -rf $(GXSRC_PATH)/flashconf/mkimg $(INSTALL)$(GXLIB_PATH)/include/app/flashconf
	cp -rf $(GXSRC_PATH)/flashconf/block.bin $(INSTALL)$(GXLIB_PATH)/include/app/flashconf
	cp -rf $(GXSRC_PATH)/flashconf/causer.bin $(INSTALL)$(GXLIB_PATH)/include/app/flashconf

	@echo " ";
	
	if [ -f $(GXSRC_PATH)/flashconf/loader-sflash/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}/loader-sflash.bin ] ; then \
		echo "loader-sflash.bin from: $(GXSRC_PATH)/flashconf/loader-sflash/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}"; \
		cp -rf $(GXSRC_PATH)/flashconf/loader-sflash/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}/* /opt/goxceed/csky-ecos/include/app/flashconf/; \
	else \
		echo "\033[31m   can't find loader-sflash.bin file , please check \033[0m"; \
	fi;

	if [ -f $(GXSRC_PATH)/flashconf/ini/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}_${DVB_MARKET}/variable_oem.ini ] ; then \
			echo ".ini file from: $(GXSRC_PATH)/flashconf/ini/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}_${DVB_MARKET}"; \
            cp -rf $(GXSRC_PATH)/flashconf/ini/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}_${DVB_MARKET}/* /opt/goxceed/csky-ecos/include/app/flashconf/; \
	elif [ -f $(GXSRC_PATH)/flashconf/ini/$(CHIP)/$(DEMOD)/${DVB_MARKET}/variable_oem.ini ] ; then \
			echo ".ini file from: $(GXSRC_PATH)/flashconf/ini/$(CHIP)/$(DEMOD)/${DVB_MARKET}"; \
            cp -rf $(GXSRC_PATH)/flashconf/ini/$(CHIP)/$(DEMOD)/${DVB_MARKET}/* /opt/goxceed/csky-ecos/include/app/flashconf/; \
	else \
			echo "\033[31m   can't find .ini file , please check \033[0m"; \
	fi;

	if [ -f $(GXSRC_PATH)/flashconf/conf/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}_${DVB_MARKET}/flash.conf ] ; then \
			echo ".conf file from: $(GXSRC_PATH)/flashconf/conf/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}_${DVB_MARKET}"; \
			cp -rf $(GXSRC_PATH)/flashconf/conf/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}_${DVB_MARKET}/* /opt/goxceed/csky-ecos/include/app/flashconf/; \
	elif [ -f $(GXSRC_PATH)/flashconf/conf/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}/flash.conf ] ; then \
			echo ".conf file from: $(GXSRC_PATH)/flashconf/conf/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}"; \
			cp -rf $(GXSRC_PATH)/flashconf/conf/$(CHIP)/$(DEMOD)/${DVB_CUSTOM}/* /opt/goxceed/csky-ecos/include/app/flashconf/; \
	elif [ -f $(GXSRC_PATH)/flashconf/conf/$(CHIP)/$(DEMOD)/$(DVB_FLASH_SIZE)m/flash.conf ] ; then \
			echo ".conf file from: $(GXSRC_PATH)/flashconf/conf/$(CHIP)/$(DEMOD)/$(DVB_FLASH_SIZE)m/flash.conf"; \
			cp -rf $(GXSRC_PATH)/flashconf/conf/$(CHIP)/$(DEMOD)/$(DVB_FLASH_SIZE)m/* /opt/goxceed/csky-ecos/include/app/flashconf/; \
	else \
			echo "\033[31m   can't find .conf file , please check \033[0m"; \
	fi;

ifeq ($(DVB_OTA_TYPE), DVB_NATIONALCHIP_OTA)
ifeq ($(DVB_FLASH_SIZE),4)
	cp -rf $(GXSRC_PATH)/flashconf/otaimg/$(CHIP)/nationalchip/lzma/ota.img /opt/goxceed/csky-ecos/include/app/flashconf/
else
	cp -rf $(GXSRC_PATH)/flashconf/otaimg/$(CHIP)/nationalchip/$(CUSTOMER)/ota.img /opt/goxceed/csky-ecos/include/app/flashconf/
endif
endif

ifeq  ($(DVB_OTA_TYPE),DVB_3H_OTA)
ifeq ($(DVB_FLASH_SIZE),4)
	cp -rf $(GXSRC_PATH)/flashconf/otaimg/$(CHIP)/3H/lzma/* /opt/goxceed/csky-ecos/include/app/flashconf/
else
	cp -rf $(GXSRC_PATH)/flashconf/otaimg/$(CHIP)/3H/gz/* /opt/goxceed/csky-ecos/include/app/flashconf/
endif
endif

	
	
#ifeq ($(OS), ecos)
#$(BIN): $(OBJS) 
#	$(LD) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
#	@$(OBJCOPY) $@ -S -g -O binary ecos.bin
#	@rm -f ecos.bin.gz
#	@gzip ecos.bin
#	@rm -rf ecos
#	@mkdir ecos
#	@mv ecos.bin.gz ecos
#	@genromfs -f ecos.img -d ecos/
#	@rm  ../flash/ecos.img -rf
#	@mv ecos.img  ../flash
#	@bash ../flash/mkimg
#	@rm -rf ecos
#endif

#ifeq ($(OS), linux)
#$(BIN): $(OBJS)
#	$(LD) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
#	rm -rf output
#	mkdir -p output
#	cp out.elf output/out.elf
#	$(STRIP) output/out.elf
#	cp -ar ../theme output/
#	find output/ -type d -name ".svn" | xargs rm -rf
#	fakeroot mkfs.cramfs output user.bin
#endif

subdirs:
	@list='$(SUBDIRS)'; \
		for subdir in $$list; do \
			echo "Making $$target in $$subdir"; \
			cd $$subdir && $(MAKE); \
			cd ..; \
		done;

subdirsclean:
	@list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		echo "Making $$target in $$subdir"; \
		cd $$subdir && $(MAKE) clean; \
		cd ..; \
	done;

subdirinstall:
	@list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		echo "Making $$target in $$subdir"; \
		cd $$subdir && $(MAKE) install; \
		cd ..; \
	done;

install-dir:env subdirinstall
	install -d "$(GXLIB_PATH)/include"	
	install -d "$(GXLIB_PATH)/lib"

clean: subdirsclean
	@rm -rf .tmp_versions
	@rm -rf $(GXSRC_PATH)/bsp/.tmp_versions
ifeq ($(OS), linux)
	make  -C  $(GXSRC_PATH)/bsp clean
endif
ifeq ($(OS), ecos)
	@rm -rf $(GXSRC_PATH)/bsp/*.o $(GXSRC_PATH)/bsp/*.mod.* $(GXSRC_PATH)/bsp/*.ko  $(GXSRC_PATH)/bsp/*.order  $(GXSRC_PATH)/bsp/*.symvers @rm -rf $(GXSRC_PATH)/bsp/.*.o.cmd -rf $(GXSRC_PATH)/bsp/.*.ko.cmd -rf $(GXSRC_PATH)/bsp/.*.o.tmp
endif
	@rm -rf $(OBJS) *.o .*swp objects deps $(CLEANFILE) $(BIN) *.log $(LIB) cscope.* tags *.img *.gz
#	@rm -f ../flash/flash.dat
#	@rm -f ../app/out.elf
	@rm -f $(GXSRC_PATH)/include/gxapp_sys_config.h
	@find -name "*~" -exec rm {} \;

#format:
#	@echo "Makeing format...";
#	@find -name "*.c" -exec dos2unix -qU 2>d2utmp1 {} \;
#	@find -name "*.h" -exec dos2unix -qU 2>d2utmp1 {} \; 
#	@find -name "*.c" -exec indent -npro -kr -i8 -sob -l120 -ss -ncs  {} \;
#	@find -name "*~" -exec rm {} \;
#	@find -name "d2utmp*" -exec rm {} \;
#	@find -name "deps*" -exec rm {} \;

format:
	@echo "Makeing format...";
	@find -name "*.[chCH]" -print |xargs dos2unix -U

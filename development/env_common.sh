unset CHIP
unset DEMOD
unset ARCH
unset OS
unset CROSS_PATH
unset GXLIB_PATH
unset GXSRC_PATH
unset GX_KERNEL_PATH

if [ "$1" = "csky-ecos" ] ; then
	echo ARCH=csky OS=ecos Configuration  !
	export ARCH=csky
	export OS=ecos
	export CROSS_PATH=csky-ecos
fi

if [ "$1" = "csky-linux" ] ; then
	echo ARCH=csky OS=linux Configuration  !
	export ARCH=csky
	export OS=linux
	export CROSS_PATH=csky-linux
fi

if [ -z "$CROSS_PATH" ] ; then
	echo -e "\033[31m Config Errror!!! ,please check  ARCH_OS \033[0m"
	echo "               "              
	echo eg: source env.sh csky-ecos
	echo "    "source env.sh csky-linux
fi

# goxceed库版本路径
export GXLIB_PATH=/opt/goxceed/$CROSS_PATH
export GXSRC_PATH=`pwd`
# linux方案，内核代码路径
export GX_KERNEL_PATH=$GXSRC_PATH/../linux-2.6.27.55

unset DVB_CUSTOM
unset DVB_MARKET
unset DVB_CA_MARKET
unset DVB_AD_MARKET
unset DVB_CA_FLAG
unset DVB_USB_FLAG
unset DVB_PVR_SPEED_SUPPORT
unset DVB_PANEL_TYPE
unset PANEL_CLK_GPIO
unset PANEL_DATA_GPIO
unset PANEL_STANDBY_GPIO
unset PANEL_LOCK_GPIO
unset DVB_KEY_TYPE
unset DVB_TUNER_TYPE
unset DVB_DEMOD_TYPE
unset DVB_DEMOD_MODE
unset DVB_TS_SRC
unset DVB_AUTO_TEST_FLAG
unset DVB_THEME
unset DVB_MEDIA_FLAG
unset DVB_ZOOM_RESTART_PLAY
unset DVB_BAD_SIGNAL_SHOW_LOGO
unset CUSTOMER

unset MOVE_FUNCTION_FLAG
unset DVB_CA_FREE_STOP

# CA枚举（与app_common_porting_stb_api.h中dvb_ca_type_t结构体中对应），配置参考对应CA代码目录下的readme.txt
unset DVB_CA_TYPE
# 对应市场（厂家）CA链接库名称（如libY1120-tonghui-gx3001-20121212D.a，设为Y1120-tonghui-gx3001-20121212D）
unset DVB_CA_1_LIB
unset DVB_CA_2_LIB
# 对应CA代码中宏定义
unset DVB_CA_1_FLAG
unset DVB_CA_2_FLAG
# 对应CA名称（目录）,如（cdcas3.0）
unset DVB_CA_1_NAME
unset DVB_CA_2_NAME

# 广告名称（目录），配置参考对应广告代码目录下的readme.txt
unset DVB_AD_NAME
# 广告类型
unset DVB_AD_TYPE
# 对应市场（厂家）广告链接库名称
unset DVB_AD_LIB

# OTA协议（类型）
unset DVB_OTA_TYPE

unset DVB_JPG_LOGO
unset AUDIO_DOLBY
unset DVB_SUBTITLE_FLAG
unset DVB_PVR_FLAG
unset DVB_NETWORK_FLAG
unset LINUX_OTT_SUPPORT

unset DVB_HD_LIST
unset DVB_LOGO_JPG
unset DVB_WORK_PATH
unset DVB_LOGO_PATH
unset DVB_I_FRAME_PATH
unset DVB_RESOLUTION
unset DVB_SYS_MODE
unset DVB_VIDEO_X
unset DVB_VIDEO_Y
unset DVB_VIDEO_W
unset DVB_VIDEO_H
unset DVB_SAT_MAX
unset DVB_TP_MAX
unset DVB_SERVICE_MAX
unset DVB_DDRAM_SIZE
unset DVB_FLASH_SIZE
unset DVB_PAT_TIMEOUT
unset DVB_SDT_TIMEOUT
unset DVB_NIT_TIMEOUT
unset DVB_PMT_TIMEOUT
unset DVB_LCN_DEFAULT
unset DVB_CENTER_FRE
unset DVB_CENTER_SYMRATE
unset DVB_CENTER_QAM
unset DVB_CENTER_BANDWIDTH
unset DVB_DUAL_MODE
unset DVB_SERIAL_SUPPORT
unset CUSTOMER
unset DVB_32MB
# 主芯片类型
export CHIP=gx3113c
# demod芯片类型
export DEMOD=atbm886x

# 客户硬件（润德）。
# 同一客户相同芯片方案，可能存在多版硬件设计，例如runde,rundeddr1,runde32m等
export DVB_CUSTOM=gongban
# 市场
export DVB_MARKET=gongban

# 客户硬件市场:${DVB_CUSTOM}_$(DVB_MARKET)。

# 对应客户硬件的loader-sflash.bin、.boot 路径：
# gxappcommon/flashconf/loader-sflash/$(CHIP)/$(DEMOD)/$(DVB_CUSTOM)/loader-sflash.bin

# 对应客户市场的invariable_oem.ini、variable_oem.ini 路径(必须确保唯一性):
# gxappcommon/flashconf/ini/$(CHIP)/$(DEMOD)/$(DVB_CUSTOM)_$(DVB_MARKET)/invariable_oem.ini 、variable_oem.ini

# 对应客户市场的flash.conf、flash_ts.conf 路径：
# 如果市场版本存在特殊flash分区配置路径(个别市场）：
# gxappcommon/flashconf/conf/$(CHIP)/$(DEMOD)/$(DVB_CUSTOM)_$(DVB_MARKET)/flash.conf、flash_ts.conf
# 市场版本flash通用配置路径：
# gxappcommon/flashconf/conf/$(CHIP)/$(DEMOD)/$(DVB_CUSTOM)/flash.conf、flash_ts.conf

# 对应客户市场的env.sh 路径(必须确保唯一性):
# gxappcommon/sh/$(CHIP)/$(DEMOD)/$(DVB_CUSTOM)_$(DVB_MARKET)/env.sh

# CA市场
# 同一CA，可能存在不同市场版本
# 针对同一市场可能存在多个厂家。不同厂家因硬件平台、CA库等差异，需兼容差异化编译
# 对应CA目录gxapp_common/cas/$DVB_CA_1_NAME/$DVB_CA_MARKET/; gxapp_common/cas/$DVB_CA_2_NAME/$DVB_CA_MARKET/;
export DVB_CA_MARKET=gongban
export DVB_AD_MARKET=gongban

#是否编译CA，如值为no则为清流版本
export DVB_CA_FLAG=yes

if [ $DVB_CA_FLAG = "yes" ]; then
	#CA枚举（与app_common_porting_stb_api.h中dvb_ca_type_t结构体中对应）
#	export DVB_CA_TYPE=DVB_CA_TYPE_MG
#	export DVB_CA_1_NAME=mg_cas
#	export DVB_CA_1_LIB=mgcaslib_v423-GX3201
#	export DVB_CA_1_FLAG=DVB_CA_TYPE_MG_FLAG

#全智
#	export DVB_CA_TYPE=DVB_CA_TYPE_QZ
#	export DVB_CA_1_NAME=qz_cas
#	export DVB_CA_1_LIB=
#	export DVB_CA_1_FLAG=DVB_CA_TYPE_QZ_FLAG
# 博远
#export DVB_CA_TYPE=DVB_CA_TYPE_BY
#export DVB_CA_1_NAME=by_cas
#export DVB_CA_1_LIB=byca
#export DVB_CA_1_FLAG=DVB_CA_TYPE_BY_FLAG
#永新
#export DVB_CA_TYPE=DVB_CA_TYPE_CDCAS30
#export DVB_CA_1_NAME=cd_cas30
#export DVB_CA_1_LIB=Y1120-tonghui-gx3001-20121212D
#export DVB_CA_1_FLAG=DVB_CA_TYPE_CD_CAS30_FLAG
#乐豆
#export DVB_CA_TYPE=DVB_CA_TYPE_DVB
#export DVB_CA_1_NAME=dvb_cas
#export DVB_CA_1_LIB=
#export DVB_CA_1_FLAG=DVB_CA_TYPE_DVB_FLAG
#DVB
export DVB_CA_TYPE=DVB_CA_TYPE_DVB
export DVB_CA_1_NAME=dvb_cas
export DVB_CA_1_LIB=
export DVB_CA_1_FLAG=DVB_CA_TYPE_DVB_FLAG

fi

#兼容双CA。多数情况下只有一个CA，第二个CA不用设置
#if [ $DVB_CA_FLAG = "yes" ]; then
#	export DVB_CA_2_NAME=byca
#	export DVB_CA_2_LIB=byca
#	export DVB_CA_2_FLAG=DVB_CA_TYPE_BY_FLAG
#fi

# set ad support type
#export DVB_AD_NAME=byads
#export DVB_AD_FLAG=DVB_AD_TYPE_BY_FLAG
#export DVB_AD_NAME=dsads
#export DVB_AD_FLAG=DVB_AD_TYPE_DS_FLAG
#export DVB_AD_LIB=dsad
export DVB_AD_NAME=maikeads
export DVB_AD_FLAG=DVB_AD_TYPE_MAIKE_FLAG

# 选择OTA协议
export DVB_OTA_TYPE=DVB_NATIONALCHIP_OTA

# 设置是否支持高清列表单独显示
export DVB_HD_LIST=no

# 设置loader中是否已开启jpg logo显示支持(需与gxloader中的.config开关一致)
export DVB_JPG_LOGO=yes

# 设置芯片是否支持ac3解码
export AUDIO_DOLBY=no

# 设置是否开启PVR功能（录制、时移等）
export DVB_PVR_FLAG=yes

# 设置是否开启subtitle字幕显示功能
export DVB_SUBTITLE_FLAG=no

# 设置是否开启网络功能（youtube等） (仅linux方案支持此功能)
export DVB_NETWORK_FLAG=no

# 设置是否支持OTT功能
export LINUX_OTT_SUPPORT=no

#是否支持USB升级
export DVB_USB_FLAG=yes

# 是否支持PVR快进、快退
export DVB_PVR_SPEED_SUPPORT=0

# 选用前面板类型 panel_xxx(市场)_xxx(厂家)_xxx(面板类型)，如PANEL_TYPE_fd650_RUNDE
# 已支持面板参考：include/bsp/panel_enum.h
export DVB_PANEL_TYPE=PANEL_TYPE_fd650_THINEWTEC
export PANEL_CLK_GPIO=36
export PANEL_DATA_GPIO=35
export PANEL_STANDBY_GPIO=34
export PANEL_LOCK_GPIO=33

# 选用遥控器宏定义类型 keymap_xxx(市场)_(厂家)，如KEY_GONGBAN_NATIONALCHIP_NEW
# 已支持面板参考:key_xml目录下的.xml group="KEY_GONGBAN_NATIONALCHIP_NEW"等，注意同一个kex(x).xml不能出现遥控器串键的情况（串键遥控器必须不不同key(x).xml中定义）
export DVB_KEY_TYPE=KEY_THINEWTEC

# 解调芯片类型 ，参考demod_enum.h定义
export DVB_DEMOD_TYPE=DVB_DEMOD_ATBM886X

# 解调模式，参考demod_enum.h定义（目前不支持DVB_DEMOD_DVBS）
export DVB_DEMOD_MODE=DVB_DEMOD_DTMB

# 配置选用第几路ts输出 : 可选0,1,2，
export DVB_TS_SRC=2

# 配置tuner类型，参考demod/include/tuner_neum.h定义
export DVB_TUNER_TYPE=TUNER_MXL608

#export CUSTOMER=CUST_TAOYUAN
export CUSTOMER=CUST_TAIKANG
#export CUSTOMER=CUST_LINLI
#export CUSTOMER=CUST_JINGANGSHAN
#export CUSTOMER=CUST_SHANGSHUI
#export DVB_LOGO_JPG=logo_thinewtec.jpg
echo DVB_CUSTOMER_NAME=$CUSTOMER
if [ $CUSTOMER = "CUST_LINLI" ]; then
export DVB_LOGO_JPG=logo_linli.jpg		#湖南临澧
#export DVB_LOGO_JPG=logo_thinewtec.jpg
export DVB_CENTER_FRE=794

export DVB_KEY_TYPE=KEY_THINEWTEC

elif [ $CUSTOMER = "CUST_TAOYUAN" ]; then
export DVB_CENTER_FRE=794               #湖南桃源

export DVB_LOGO_JPG=logo_taoyuan.jpg
export DVB_KEY_TYPE=KEY_TAOYUAN

elif [ $CUSTOMER = "CUST_TAIKANG" ]; then
export DVB_LOGO_JPG=logo_taikang.jpg			#湖南太康
export DVB_CENTER_FRE=730

export DVB_KEY_TYPE=KEY_THINEWTEC
#export DVB_KEY_TYPE=KEY_TAOYUAN
elif [ $CUSTOMER = "CUST_JINGANGSHAN" ]; then
export DVB_LOGO_JPG=logo_maike.jpg			#湖南太康
export DVB_CENTER_FRE=730
export DVB_KEY_TYPE=KEY_THINEWTEC
elif [ $CUSTOMER = "CUST_SHANGSHUI" ]; then
export DVB_LOGO_JPG=logo_shangshui.jpg			#商水
export DVB_CENTER_FRE=730
export DVB_KEY_TYPE=KEY_THINEWTEC

fi

# 配置theme资源（有线、地面、标清、高清等），参考development/theme下对应目录
export DVB_THEME=SD

if [ "$1" = "csky-linux" ] ; then
# XML、图片等路径
export DVB_WORK_PATH='"/dvb/"'
# 广播背景图片路径
export DVB_LOGO_PATH='"/dvb/theme/logo.bin"'
export DVB_I_FRAME_PATH='"/dvb/theme/logo.bin"'
fi

if [ "$1" = "csky-ecos" ] ; then
# 广播背景图片路径
export DVB_LOGO_PATH='"/theme/logo.bin"'
export DVB_I_FRAME_PATH='"/theme/logo_audio.bin"'
export DVB_NETWORK_FLAG=no
fi


# 宏定义参数项
# 定义标清、高清
export DVB_RESOLUTION=SD
# 音视频同步方式 0-PCR_RECOVER 1-VPTS_RECOVER 2-APTS_RECOVER 3-AVPTS_RECOVER 4-NO_RECOVER
export DVB_SYS_MODE=0
# 全屏视频X位置
export DVB_VIDEO_X=0
# 全屏视频Y位置
export DVB_VIDEO_Y=0
# 全屏视频宽大小
export DVB_VIDEO_W=720
# 全屏视频高大小
export DVB_VIDEO_H=576
# 卫星最大个数（有线、地面方案设置为1）
export DVB_SAT_MAX=1
# TP频点最大个数
export DVB_TP_MAX=200
# 节目最大个数
export DVB_SERVICE_MAX=1000
# DDRAM 大小
export DVB_DDRAM_SIZE=128
# FLASH 大小
export DVB_FLASH_SIZE=8
# 搜索PAT超时时间（ms）
export DVB_PAT_TIMEOUT=3000
# 搜索SDT超时时间（ms）
export DVB_SDT_TIMEOUT=5000
# 搜索NIT超时时间（ms）
export DVB_NIT_TIMEOUT=10000
# 搜索PMT超时时间（ms）
export DVB_PMT_TIMEOUT=8000
# LCN开启模式下，非标码流无逻辑频道号的节目，默认起始逻辑频道号
export DVB_LCN_DEFAULT=500
# 主频点参数
export DVB_CENTER_FRE=730
export DVB_CENTER_SYMRATE=6875
export DVB_CENTER_QAM=2
# 带宽
export DVB_CENTER_BANDWIDTH=8
# 是否支持dvbc-dtmb双模
export DVB_DUAL_MODE=no
# 硬件自动测试开关。yes- 开启自动测试功能 no -关闭自动测试功能
export DVB_AUTO_TEST_FLAG=no
# 是否支持多媒体功能
export DVB_MEDIA_FLAG=yes
# 视频缩放是否重新播放
export DVB_ZOOM_RESTART_PLAY=no
#串口
export DVB_SERIAL_SUPPORT=no
#移动
export MOVE_FUNCTION_FLAG=no

export DVB_CA_FREE_STOP=no
export LOGO_SHOW_DELAY=yes
export FACTORY_SERIALIZATION_SUPPORT=no
# echo export path
if [ -z "$CROSS_PATH" ] ; then
	echo
else
	echo CHIP=$CHIP
	echo DEMOD=$DEMOD
	echo PATH:
	echo RC_VERSION=$RC_VERSION
	echo GX_KERNEL_PATH=$GX_KERNEL_PATH
	echo GXLIB_PATH=$GXLIB_PATH
	echo DVB_CUSTOM=$DVB_CUSTOM
	echo DVB_MARKET=$DVB_MARKET
	echo DVB_CA_FLAG=$DVB_CA_FLAG
	if [ $DVB_CA_FLAG = "yes" ]; then
		echo DVB_CA_MARKET=$DVB_CA_MARKET
		echo DVB_CA_TYPE=$DVB_CA_TYPE
		echo DVB_CA_1_NAME=$DVB_CA_1_NAME
#		echo DVB_CA_1_LIB=$DVB_CA_1_LIB
#		echo DVB_CA_1_FLAG=$DVB_CA_1_FLAG
		echo DVB_CA_2_NAME=$DVB_CA_2_NAME
#		echo DVB_CA_2_LIB=$DVB_CA_2_LIB
#		echo DVB_CA_2_FLAG=$DVB_CA_2_FLAG
	fi
	if [ -z "$DVB_AD_NAME" ] ; then
		echo
	else
		echo DVB_AD_MARKET=$DVB_AD_MARKET
		echo DVB_AD_NAME=$DVB_AD_NAME
#		echo DVB_AD_FLAG=$DVB_AD_FLAG
#		echo DVB_AD_LIB=$DVB_AD_LIB
	fi
	echo DVB_OTA_TYPE=$DVB_OTA_TYPE
	echo DVB_JPG_LOGO=$DVB_JPG_LOGO
	echo DVB_HD_LIST=$DVB_HD_LIST
	echo DVB_USB_FLAG=$DVB_USB_FLAG
	echo DVB_PVR_FLAG=$DVB_PVR_FLAG
	echo DVB_SUBTITLE_FLAG=$DVB_SUBTITLE_FLAG
	echo AUDIO_DOLBY=$AUDIO_DOLBY
	echo DVB_NETWORK_FLAG=$DVB_NETWORK_FLAG
	echo LINUX_OTT_SUPPORT=$LINUX_OTT_SUPPORT
	echo DVB_PVR_SPEED_SUPPORT=$DVB_PVR_SPEED_SUPPORT
	echo DVB_PANEL_TYPE=$DVB_PANEL_TYPE
	echo DVB_KEY_TYPE=$DVB_KEY_TYPE
	echo DVB_LOGO_JPG=$DVB_LOGO_JPG
	echo DVB_TUNER_TYPE=$DVB_TUNER_TYPE
	echo DVB_DEMOD_TYPE=$DVB_DEMOD_TYPE
	echo DVB_DEMOD_MODE=$DVB_DEMOD_MODE
	echo DVB_TS_SRC=$DVB_TS_SRC
	echo DVB_RESOLUTION=$DVB_RESOLUTION
	echo DVB_SYS_MODE=$DVB_SYS_MODE
	echo DVB_CENTER_FRE=$DVB_CENTER_FRE
	echo DVB_SERIAL_SUPPORT=$DVB_SERIAL_SUPPORT
	echo MOVE_FUNCTION_FLAG = $MOVE_FUNCTION_FLAG
	#	echo DVB_CENTER_SYMRATE=$DVB_CENTER_SYMRATE
	#	echo DVB_CENTER_QAM=$DVB_CENTER_QAM
	#   	echo DVB_CENTER_BANDWIDTH=$DVB_CENTER_BANDWIDTH
	echo DVB_DUAL_MODE=$DVB_DUAL_MODE
    	echo DVB_AUTO_TEST_FLAG=$DVB_AUTO_TEST_FLAG
	echo DVB_THEME=$DVB_THEME
	echo DVB_MEDIA_FLAG=$DVB_MEDIA_FLAG
	echo DVB_ZOOM_RESTART_PLAY=$DVB_ZOOM_RESTART_PLAY
	echo DVB_CA_FREE_STOP=$DVB_CA_FREE_STOP
fi

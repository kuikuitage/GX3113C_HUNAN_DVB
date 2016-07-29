#export GX_KERNEL_PATH=~/v1.9-rc3/linux-2.6.27.55

#panel-objs := gx_panel.o pt6964.o

EXTRA_CFLAGS  += -DLINUX_OS -I$(GXLIB_PATH)/include/bus/gui_core -I$(GXLIB_PATH)/include/gx_gui -I$(GXLIB_PATH)/include/bus -I$(GXLIB_PATH)/include -I$(GXSRC_PATH)/include/bsp -I$(GXSRC_PATH)/include/key -I$(GXSRC_PATH)/include/init -I$(GXSRC_PATH)/include -I$(GXSRC_PATH)/include/search
#TEST_OBJS := test.o
panel-objs := gx_panel_linux.o stb_panel.o stb_panel_config.o stb_panel_none.o stb_panel_ct1642_haixin.o stb_panel_ct1642.o stb_panel_ct1642_jinya.o stb_panel_fd650.o stb_panel_jizhong.o stb_panel_PT6964.o stb_panel_shenzhou_tm1635.o stb_panel_config_linux.o stb_panel_fd650_runde.o stb_panel_fd650_keneng.o stb_panel_ct1642_jinya_led.o
obj-m += panel.o


#ARCH=csky
#MAKE=make

all:
	rm -f ./gx_gpio.h
#	cp -f  $(GX_KERNEL_PATH)/drivers/char/gx_gpio/gx_gpio.h  ./
	make ARCH=$(ARCH) CROSS_COMPILE=$(ARCH)-linux- -C $(GX_KERNEL_PATH) SUBDIRS=$(GXSRC_PATH)/bsp


clean:
	rm -rf *.o *.mod.* *.ko *.order *.symvers .tmp_versions .*.o.cmd .*.ko.cmd .*.o.tmp

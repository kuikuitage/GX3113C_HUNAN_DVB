#!/bin/bash
file_name=signal_connect.c
func_name=signal_connect_handler

echo '//create signal_connect.c by bash!!'> ./$file_name
echo '#include "gui_core.h"'> ./$file_name
echo " ">>./$file_name
echo " ">>./$file_name


find $GXSRC_PATH/app/menu -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "extern int "$1 "(const char* widgetname, void *usrdata);"}' >>./$file_name
if [ -z "$DVB_CA_1_NAME" ] ; then
	echo " ">>./$file_name
else
	find $GXSRC_PATH/app/cas/$DVB_CA_1_NAME/menu -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "extern int "$1 "(const char* widgetname, void *usrdata);"}' >>./$file_name
fi
if [ -z "$DVB_CA_2_NAME" ] ; then
	echo " ">>./$file_name
else
	find $GXSRC_PATH/app/cas/$DVB_CA_2_NAME/menu -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "extern int "$1 "(const char* widgetname, void *usrdata);"}' >>./$file_name
fi

if [ $DVB_NETWORK_FLAG = yes ] ; then
	find $GXSRC_PATH/app/network -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "extern int "$1 "(const char* widgetname, void *usrdata);"}' >>./$file_name
fi

if [ $DVB_PVR_FLAG = yes ] ; then
	find $GXSRC_PATH/app/pvr -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "extern int "$1 "(const char* widgetname, void *usrdata);"}' >>./$file_name
fi


if [ $DVB_MEDIA_FLAG = yes ] ; then
	find $GXSRC_PATH/app/media -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "extern int "$1 "(const char* widgetname, void *usrdata);"}' >>./$file_name
fi

if [ $GAME_SNAKE = yes ] ; then
	find $GXSRC_PATH/app/game -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "extern int "$1 "(const char* widgetname, void *usrdata);"}' >>./$file_name
fi

if [ $FACTORY_MENU = yes ] ; then
	find $GXSRC_PATH/app/factory_menu -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "extern int "$1 "(const char* widgetname, void *usrdata);"}' >>./$file_name
fi
if [ $DVB_SUBTITLE_FLAG = yes ] ; then
	find $GXSRC_PATH/app/subtitle -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "extern int "$1 "(const char* widgetname, void *usrdata);"}' >>./$file_name
fi
echo " ">>./$file_name
echo " ">>./$file_name

echo "status_t $func_name(void)">>./$file_name
echo "{">>./$file_name
echo "	status_t ret = GXCORE_SUCCESS;">>./$file_name

find $GXSRC_PATH/app/menu -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "\tret |= SIGNAL_CONNECT("$1");"}' >>./$file_name
if [ -z "$DVB_CA_1_NAME" ] ; then
	echo " ">>./$file_name	
else
	find $GXSRC_PATH/app/cas/$DVB_CA_1_NAME/menu -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "\tret |= SIGNAL_CONNECT("$1");"}' >>./$file_name
fi
if [ -z "$DVB_CA_2_NAME" ] ; then
	echo " ">>./$file_name
else
	find $GXSRC_PATH/app/cas/$DVB_CA_2_NAME/menu -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "\tret |= SIGNAL_CONNECT("$1");"}' >>./$file_name
fi

if [ $DVB_NETWORK_FLAG = yes ] ; then
	find $GXSRC_PATH/app/network -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "\tret |= SIGNAL_CONNECT("$1");"}' >>./$file_name
fi

if [ $DVB_PVR_FLAG = yes ] ; then
	find $GXSRC_PATH/app/pvr -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "\tret |= SIGNAL_CONNECT("$1");"}' >>./$file_name
fi

if [ $DVB_MEDIA_FLAG = yes ] ; then
	find $GXSRC_PATH/app/media -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "\tret |= SIGNAL_CONNECT("$1");"}' >>./$file_name
fi


if [ $GAME_SNAKE = yes ] ; then
	find $GXSRC_PATH/app/game -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "\tret |= SIGNAL_CONNECT("$1");"}' >>./$file_name
fi

if [ $FACTORY_MENU = yes ] ; then
	find $GXSRC_PATH/app/factory_menu -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "\tret |= SIGNAL_CONNECT("$1");"}' >>./$file_name
fi
if [ $DVB_SUBTITLE_FLAG = yes ] ; then
	find $GXSRC_PATH/app/subtitle -iname "*.c" |xargs grep SIGNAL_HANDLER |awk '{print  $3}'|awk -F'(' '{print "\tret |= SIGNAL_CONNECT("$1");"}' >>./$file_name
fi
echo " ">>./$file_name

echo "	return ret;">>./$file_name
echo "}">>./$file_name

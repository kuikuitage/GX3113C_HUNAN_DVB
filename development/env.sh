#export ARCH=i386
#export ARCH=arm
#export ARCH=csky
#export ARCH=ckcore
#export OS=linux
#export OS=ecos

unset ARCH
unset OS
unset CROSS_PATH
unset GXLIB_PATH
unset GXSRC_PATH
unset GXFEKO_PATH
unset GXAVKO_PATH
unset GENROOTFS_PATH
unset NFSROOTFS_PATH

unset FACTORY_MENU

if [ "$1" = "csky-ecos" ] ; then
	export CROSS_PATH=csky-ecos
fi

if [ "$1" = "csky-linux" ] ; then
	export CROSS_PATH=csky-linux
fi

if [ -z "$CROSS_PATH" ] ; then
	echo Config Errror!!!
	echo "               "              
	echo eg: source env.sh csky-ecos
	echo "    "source env.sh csky-linux
else
	export GXLIB_PATH=/opt/goxceed/$CROSS_PATH
	rm -rf env_common.sh
	cp $GXLIB_PATH/include/app/sh/env.sh env_common.sh
	source env_common.sh $1

	export GAME_SNAKE=no
	export FACTORY_MENU=no

	if [  -f $GXLIB_PATH/include/app/flashconf/flash.conf ]; then	
		rm -rf $GXSRC_PATH/flash
#	cp -rf $GXSRC_PATH/chip_flash/$CHIP/$DVB_FLASH $GXSRC_PATH/flash
		cp -rf $GXLIB_PATH/include/app/flashconf $GXSRC_PATH/flash
		find $GXSRC_PATH/flash/ -type d -name ".svn" | xargs rm -rf
	fi
	
#	export GXFEKO_PATH=$GXSRC_PATH/../demod/linux/fe.ko
#	export GXAVKO_PATH=$GXSRC_PATH/../goxceed/av-csky.ko
	export GENROOTFS_PATH=$GXSRC_PATH/../genrootfs
	export NFSROOTFS_PATH=$GXSRC_PATH/../nfsroot
#	echo GXFEKO_PATH=$GXFEKO_PATH
#	echo GXAVKO_PATH=$GXAVKO_PATH
	echo GENROOTFS_PATH=$GENROOTFS_PATH
	echo NFSROOTFS_PATH=$NFSROOTFS_PATH
fi

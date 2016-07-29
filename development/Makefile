##############################################################################
SUBDIRS= app

##############################################################################
-include  $(GXSRC_PATH)/scripts/inc.Makefile
##############################################################################
ifeq ($(OS), ecos)
all:	subdirs
endif

ifeq ($(OS), linux)

#USERDIR=$(shell whoami)
USERDIR=gx

#NFSROOT=/opt/gx6602 
NFSROOT=/home/gx/nfs

all:subdirs
endif

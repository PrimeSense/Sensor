
#/******************************************************************************
#*                                                                             *
#*   PrimeSense Sensor 5.0 Makefile                                            *
#*   Copyright (C) 2010 PrimeSense Ltd.                                        *
#*                                                                             *
#******************************************************************************/
# default config is Release
ifndef CFG
    CFG = Release
endif

# list all modules
ALL_PROJS = \
	XnCore \
	XnFormats \
	XnDDK \
	XnDeviceSensorV2 \
	Utils/XnSensorServer \
	XnDeviceFile \

                
ALL_PROJS_CLEAN = $(foreach proj,$(ALL_PROJS),$(proj)-clean)

# define a function which creates a target for each proj
define CREATE_PROJ_TARGET

.PHONY: $1 $1-clean

$1:
	$(MAKE) -C $1 CFG=$(CFG)

$1-clean: 
	$(MAKE) -C $1 CFG=$(CFG) clean

endef

########### TARGETS ##############

.PHONY: all install uninstall clean

# make all makefiles
all: $(ALL_PROJS)

# create projects targets
$(foreach proj,$(ALL_PROJS),$(eval $(call CREATE_PROJ_TARGET,$(proj))))

# additional dependencies
XnFormats: XnCore
XnDDK: XnCore
XnDDK: XnFormats
XnLeanDeviceSensorV2: XnDDK
XnLeanDeviceSensorV2: XnFormats
XnLeanDeviceSensorV2: XnCore
Utils/XnSensorServer: XnDDK
Utils/XnSensorServer: XnDeviceSensorV2
Utils/XnSensorServer: XnFormats
XnDeviceFile: XnCore
XnDeviceFile: XnDDK
XnDeviceFile: XnFormats

# clean is cleaning all projects
clean: $(ALL_PROJS_CLEAN)

# redist target
redist: all
	cd ../CreateRedist; ./RedistMaker; cd -

# install target
install: redist
	cd ../Redist; ./install.sh; cd -

# uninstall
uninstall: 
	cd ../Redist; ./install.sh -u; cd -


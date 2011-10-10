#############################################################################
# Primesense Experience Engine makefile.
# 
# default configuration is Release. for a debug version use:
# 	make CFG=Debug
#
# default compiler is g++. for another one use:
#   make CXX=<comp>
#
# default platform is x86. for CE4100 use:
#   make PLATFORM=CE4100
#
#############################################################################

# default config is Release
ifndef CFG
	CFG = Release
endif
export CFG

# default staging dir
ifndef STAGING_DIR
	STAGING_DIR = ../../../../LinuxARM_Staging
endif

# Allow relative and absolute path
# Allow relative and absolute path
ifneq (,$(findstring ..,$(STAGING_DIR)))
	ABS_STAGING_DIR=$(CURDIR)/$(STAGING_DIR)
else
	ABS_STAGING_DIR=$(STAGING_DIR)
endif

# list all modules
ALL_MODULES = \
	XnDeviceSensorV2 \
	XnDeviceFile

# list all utils
ALL_UTILS = \
	Utils/XnSensorServer

# list all core projects (ones that should be installed)
ALL_CORE_PROJS = \
	$(ALL_MODULES) \
	$(ALL_UTILS) \
	XnCore \
	XnFormats \
	XnDDK

# list all samples
ALL_SAMPLES = \
	Samples/XnCommandConsole \
	Samples/XnVSimpleRead	\
	Samples/XnVSimpleViewer

# list all projects
ALL_PROJS = \
	$(ALL_CORE_PROJS) \
	XnEECore \
	$(ALL_SAMPLES)

ALL_PROJS_CLEAN = $(foreach proj,$(ALL_PROJS),$(proj)-clean)

# define a function which creates a target for each proj
define CREATE_PROJ_TARGET

.PHONY: $1 $1-clean

$1:
	$(MAKE) -C $1 CFG=$(CFG) STAGING_DIR=$(ABS_STAGING_DIR)

$1-clean: 
	$(MAKE) -C $1 CFG=$(CFG) STAGING_DIR=$(ABS_STAGING_DIR) clean

endef

########### TARGETS ##############

.PHONY: all install uninstall clean

# make all makefiles
all: $(ALL_CORE_PROJS)

internal: $(ALL_PROJS)

# create projects targets
$(foreach proj,$(ALL_PROJS),$(eval $(call CREATE_PROJ_TARGET,$(proj))))

# additional dependencies
XnFormats: XnCore
XnDDK: XnCore XnFormats
XnDeviceSensorV2: XnCore XnFormats XnDDK
XnDeviceFile: XnCore XnFormats XnDDK
XnEECore: XnCore XnFormats XnDDK
Samples/XnCommandConsole: XnCore XnEECore
Samples/XnVSimpleRead: XnCore XnEECore
Samples/XnVSimpleViewer: XnCore XnEECore

# clean is cleaning all projects
clean: $(ALL_PROJS_CLEAN)

# redist target
redist: all
	cd ../CreateRedist; ./RedistMaker; cd -

# install target
install: redist
	cd ../Redist; ./install.sh -i $(ABS_STAGING_DIR); cd -

# uninstall
uninstall: 
	cd ../Redist; ./install.sh -u $(ABS_STAGING_DIR); cd -


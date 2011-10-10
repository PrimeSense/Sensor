SRC_FILES = ../../../../Source/XnDeviceFile/*.cpp
	    
LIB_NAME = XnDeviceFile
ifeq "$(CFG)" "Release"
	LIB_DIRS = ../../../../../OpenNI/Platform/Linux-Arm/Bin/Release
else
	LIB_DIRS = ../../../../../OpenNI/Platform/Linux-Arm/Bin/Debug
endif
USED_LIBS = XnCore XnFormats XnDDK OpenNI
DEFINES = XN_DEVICE_EXPORTS

include ../EngineLibMakefile


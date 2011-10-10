SRC_FILES = \
	../../../../Source/XnCore/*.cpp
		
LIB_NAME = XnCore

ifeq "$(CFG)" "Release"
	LIB_DIRS = ../../../../../OpenNI/Platform/Linux-Arm/Bin/Release
else
	LIB_DIRS = ../../../../../OpenNI/Platform/Linux-Arm/Bin/Debug
endif
USED_LIBS = OpenNI
DEFINES = XN_CORE_EXPORTS

include ../EngineLibMakefile

SRC_FILES = ../../../../Source/XnDDK/*.cpp
	    
LIB_NAME = XnDDK
ifeq "$(CFG)" "Release"
	LIB_DIRS = ../../../../../OpenNI/Platform/Linux-Arm/Bin/Release
else
	LIB_DIRS = ../../../../../OpenNI/Platform/Linux-Arm/Bin/Debug
endif
USED_LIBS = XnCore XnFormats OpenNI
DEFINES = XN_DDK_EXPORTS

include ../EngineLibMakefile


SRC_FILES = \
			../../../../Source/XnFormats/*.cpp \
			../../../../Source/External/LibJPEG/*.c

LIB_NAME = XnFormats
ifeq "$(CFG)" "Release"
	LIB_DIRS = ../../../../../OpenNI/Platform/Linux-Arm/Bin/Release
else
	LIB_DIRS = ../../../../../OpenNI/Platform/Linux-Arm/Bin/Debug
endif
USED_LIBS = XnCore OpenNI
DEFINES = XN_FORMATS_EXPORTS
INC_DIRS = ../../../../Source/External/LibJPEG

include ../EngineLibMakefile

SRC_FILES = ../../../../Source/XnDeviceNet/XnDeviceNet.cpp ../../../../Source/XnDeviceNet/XnDeviceNetClient.cpp ../../../../Source/XnDeviceNet/XnDeviceNetInit.cpp ../../../../Source/XnDeviceNet/XnDeviceNetServer.cpp
	    
LIB_NAME = XnDeviceNet
USED_LIBS = XnCore XnFormats XnDDK
DEFINES = XN_DEVICE_EXPORTS

include ../EngineLibMakefile


SRC_FILES = \
			../../../../Source/EEFramework/XnEEFramework.cpp \
			../../../../Source/EEFramework/Algorithms/XnVCenterOfMass.cpp \
			../../../../Source/EEFramework/Algorithms/XnVClosestPointFinder.cpp \
			../../../../Source/EEFramework/Algorithms/XnVConnectedComponentDetector.cpp \
			../../../../Source/EEFramework/Algorithms/XnVDownscale.cpp \
			../../../../Source/EEFramework/Algorithms/XnVGeneralStaticMap.cpp \
			../../../../Source/EEFramework/Algorithms/XnVHumanStretch.cpp \
			../../../../Source/EEFramework/Algorithms/XnVLabelPruner.cpp \
			../../../../Source/EEFramework/Algorithms/XnVStaticMap.cpp \
			../../../../Source/EEFramework/Algorithms/XnVUpscale.cpp \
			../../../../Source/EEFramework/Algorithms/XnVUserExtractor.cpp \
			../../../../Source/EEFramework/Objects/XnVExtremes.cpp \
			../../../../Source/EEFramework/Objects/XnVVirtualObject.cpp \
			../../../../Source/EEFramework/Geometry/XnV3DBox.cpp \
			../../../../Source/EEFramework/Geometry/XnVSphere.cpp
	    
LIB_NAME = XnEEFramework
USED_LIBS = XnCore XnDDK XnEECore
DEFINES = XN_EE_FW_EXPORTS

include ../EngineLibMakefile


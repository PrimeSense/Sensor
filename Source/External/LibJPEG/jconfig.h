#ifdef _WIN32 // Microsoft Visual Studio
	#include "jconfig.vc"
	#include <stdio.h>
#elif SN_TARGET_PS3
	#include "jconfig.ps3"
	#include <stdio.h>
#elif (linux && i386)
	#include "jconfig.lnx86"
	#include <stdio.h>
#else
	#include "jconfig.lnx86"
	#include <stdio.h>
#endif

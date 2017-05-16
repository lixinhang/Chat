#ifndef __PRECOMPILE_CONFIG_H__
#define __PRECOMPILE_CONFIG_H__

#include "PlatformConfig.h"

//C++ Standard Library
#include <math.h>
#include <list>
#include <iostream>

//third party
#include "../third_party/json/include/json.h"
#include "../third_party/zlib/include/zlib.h"

//for all
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	#include <io.h>
	#include <WS2tcpip.h>
	#include <mutex>
#else
	#include <errno.h>
	#include <assert.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <arpa/inet.h>

	#include <unistd.h>
	#include <netdb.h>
	#include <semaphore.h>
#endif

//ios
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	#include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_IOS

//android
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

//windows
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	#include <windows.h>
	#include <Winsock2.h>
	#include <mmsystem.h>

	#include "win32/pthread/pthread.h"
	#pragma warning(disable: 4244)

	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "Includes/lib/pthreadVCE2.lib")


#ifdef _DEBUG
	#pragma comment(lib, "third_party/json/lib/libjson_d.lib")
	#pragma comment(lib, "third_party/zlib/lib/zlibd.lib")
#else
	#pragma comment(lib, "third_party/json/lib/libjson.lib")
	#pragma comment(lib, "third_party/zlib/lib/zlib.lib")
#endif

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

//linux
#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_LINUX

//mac
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#endif

#ifndef __PLATFORM_CONFIG_H__
#define __PLATFORM_CONFIG_H__

#define IPHONE

// define supported target platform macro.
#define CC_PLATFORM_UNKNOWN            0
#define CC_PLATFORM_IOS                1
#define CC_PLATFORM_ANDROID            2
#define CC_PLATFORM_WIN32              3
#define CC_PLATFORM_LINUX              4
#define CC_PLATFORM_MAC                5

// Determine target platform by compile environment macro.
#define CC_TARGET_PLATFORM         CC_PLATFORM_UNKNOWN

// iphone
#if defined(IPHONE)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_IOS
#endif

// android
#if defined(ANDROID)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_ANDROID
#endif

// win32
#if defined(WIN32) && defined(_WINDOWS)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_WIN32
#endif

// linux
#if defined(LINUX)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_LINUX
#endif

// mac
#if defined(MAC)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_MAC
#endif

// check user platform
#if ! CC_TARGET_PLATFORM
	#error  "Cannot recognize the target platform"
#endif

#endif // __PLATFORM_CONFIG_H__

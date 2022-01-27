#pragma once


// platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	#define PENGUIN_PLATFORM_WINDOWS 1 
	#ifndef _WIN64
		#error "Only 64-bit is supported on Windows"
	#endif
#elif defined(__linux__) || defined(__gnu_linux__)
	#define PENGUIN_PLATFORM_LINUX 1
	#if defined(__ANDROID__)
		#define PENGUIN_PLATFORM_ANDROID 1
	#endif
#else
	#error "unsupported platform"
#endif

#ifdef PENGUIN_EXPORT
	// export as dll/so
	#ifdef _MSC_VER 
		#define PENGUIN_API __declspec(dllexport)
	#else
		#define PENGUIN_API __attribute__((visibility("default")))
	#endif

// import from dll/so
#else
	#ifdef _MSC_VER 
		#define PENGUIN_API __declspec(dllimport)
	#else
		#define PENGUIN_API
	#endif
#endif


// #define TRY(fn, err_msg) try (fn) catch { }

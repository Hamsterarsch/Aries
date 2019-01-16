#pragma once

#ifdef _BUILD_DLL_
	#if defined(_WIN64) || defined(_WIN32)
		#define RHI_API __declspec(dllexport)
	#elif _APPLE_
		#define RHI_API
	#elif __linux__
		#define RHI_API
	#endif
#else
	#if defined(_WIN32) || defined(_WIN64)
		#define RHI_API _declspec(dllexport)
	#elif _APPLE_
		#define RHI_API
	#elif __linux__
		#define RHI_API
	#endif
#endif
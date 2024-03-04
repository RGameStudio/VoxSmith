#pragma once

#ifdef VS_BUILD_DLL
	#define VOX_SMITH_API _declspec(dllexport)
#else
	#define VOX_SMITH_API _declspec(dllimport)
#endif
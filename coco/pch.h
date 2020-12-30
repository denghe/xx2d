#pragma once
#include <winsdkver.h>
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>	// APPBARDATA

#include "xx_typehelpers.h"
#include "xx_ptr.h"

#include "glad/glad.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "CCGeometry.h"

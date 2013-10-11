// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <winsock.h>
#pragma comment(lib,"WS2_32.lib")
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

#include "GameServer.h"
#include "Log.h"
#include "File.h"
#include "Structs.h"
#include "Ptrs.h"
#include "Functions.h"
#include "Handler.h"
#include "Intercepts.h"
#include "Offset.h"

#define ArraySize(x) (sizeof(x) / sizeof(x[0]))
// TODO: reference additional headers your program requires here

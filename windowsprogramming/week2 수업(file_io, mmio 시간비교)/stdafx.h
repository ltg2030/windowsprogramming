// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include <crtdbg.h>
bool is_file_existsW(_In_ const wchar_t* file_path);
void print(_In_ const char* fmt, _In_ ...);
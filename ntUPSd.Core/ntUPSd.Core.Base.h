/*
Copyright 2016 Matthew Holder

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

// Configuration:

#ifndef STRICT
#define STRICT
#endif

#ifndef _ATL_FREE_THREADED
#define _ATL_FREE_THREADED
#endif

//#ifndef _ATL_NO_AUTOMATIC_NAMESPACE
//#define _ATL_NO_AUTOMATIC_NAMESPACE
//#endif

#ifndef _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#endif

#ifndef ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#endif

// Override Definitions:
#include "ntUPSd.PreDefs.h"

// Standard C++ Run-Time Libraries:
#include <utility>

// Visual C++ Run-Time Libraries:
#include <eh.h>

// ATL:
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlcoll.h>
#include <atlfile.h>
#include <atlutil.h>

// Microsoft Windows, System Libraries:
#include <SetupAPI.h>
#include <PowrProf.h>
#include <BatClass.h>
#include <DevGuid.h>

// Microsoft Windows, First-party Libraries:
#include <WinSock2.h>
#include <WS2TcpIp.h>

// Required Microsoft Windows, System Libraries:
#pragma comment(lib, "SetupAPI.lib")
#pragma comment(lib, "PowrProf.lib")

// Required Microsoft Windows, First-party Libraries:
#pragma comment(lib, "WS2_32.lib")

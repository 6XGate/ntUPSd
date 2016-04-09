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

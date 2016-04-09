#pragma once

#if !defined(_ATLTRY)
#	define _ATLTRY try
#	define _ATLCATCH(e) catch (::ATL::CAtlException e)
#	define _ATLCATCHALL() __pragma(warning(push)) __pragma(warning(disable: 4571)) catch (...) __pragma(warning(pop))
#	define _ATLRETHROW throw
#	define _ATLDELETEEXCEPTION(e) e
#endif

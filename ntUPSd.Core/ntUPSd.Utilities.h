#pragma once
#include "ntUPSd.Core.Base.h"

namespace CTL
{
	template <class T, typename... Args>
	ATL_FORCEINLINE T *_AtlNew(Args && ...args)
	{
		T *p = _ATL_NEW T(std::forward<Args>(args)...);
		if (p == nullptr)
		{
			AtlThrow(E_OUTOFMEMORY);
		}

		return p;
	}

	template <typename T>
	using CLocalPtr = ::ATL::CHeapPtr<T, ::ATL::CLocalAllocator>;

	template <typename T>
	using CGlobalPtr = ::ATL::CHeapPtr<T, ::ATL::CGlobalAllocator>;
}

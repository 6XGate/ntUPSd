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
#include "ntUPSd.Core.Base.h"

namespace CTL
{
	template <class T, typename... Args>
	ATL_FORCEINLINE T *_AtlNew(Args && ...args)
	{
		#pragma warning(suppress: 6387) // Unable to annotate or handle analyzing?
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

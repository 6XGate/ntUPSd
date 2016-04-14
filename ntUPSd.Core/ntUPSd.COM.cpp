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
#include "stdafx.h"
#include "ntUPSd.COM.h"

namespace CTL
{
	ULONG CComLiteObjectBase::InnerAddRef() noexcept
	{
		return ::InterlockedIncrement(&m_cRefs);
	}

	ULONG CComLiteObjectBase::InnerRelease() noexcept
	{
		ULONG cRefs = ::InterlockedDecrement(&m_cRefs);
		if (cRefs == 0)
		{
			delete this;
		}

		return cRefs;
	}

	HRESULT CComLiteObjectBase::InnerQueryInterface(_In_ REFIID riid, _Outptr_ void ** ppvObject) noexcept
	{
		if (IsEqualIID(riid, __uuidof(IUnknown)))
		{
			*ppvObject = static_cast<IUnknown*>(this);
			InnerAddRef();
			return S_OK;
		}
		else
		{
			return E_NOINTERFACE;
		}
	}
}

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
#include "ntUPSd.COM.h"

namespace CTL
{
	template <typename Base>
	STDMETHODIMP_(ULONG) CComLiteObject<Base>::AddRef() noexcept
	{
		return InnerAddRef();
	}

	template <typename Base>
	STDMETHODIMP_(ULONG) CComLiteObject<Base>::Release() noexcept
	{
		return InnerRelease();
	}

	template <typename Base>
	STDMETHODIMP CComLiteObject<Base>::QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppvObject) noexcept
	{
		if (ppvObject == nullptr)
		{
			return E_POINTER;
		}

		*ppvObject = nullptr;
		HRESULT hr = InnerQueryInterface(riid, ppvObject);
		if (FAILED(hr))
		{
			__analysis_assume(ppvObject == nullptr);
			return hr;
		}

		return S_OK;
	}

	template <class First, class ...Rest>
	inline HRESULT IUnknownImpl<First, Rest...>::InnerQueryInterface(_In_ REFIID riid, _Outptr_ void **ppvObject) noexcept
	{
		if (IsEqualIID(riid, __uuidof(First)))
		{
			*ppvObject = static_cast<First*>(this);
			InnerAddRef();
			return S_OK;
		}
		else
		{
			return IUnknownImpl<Rest...>::InnerQueryInterface(riid, ppvObject);
		}
	}

	template <class Last>
	inline HRESULT IUnknownImpl<Last>::InnerQueryInterface(_In_ REFIID riid, _Outptr_ void **ppvObject) noexcept
	{
		if (IsEqualIID(riid, __uuidof(Last)))
		{
			*ppvObject = static_cast<Last*>(this);
			InnerAddRef();
			return S_OK;
		}
		else
		{
			return CComLiteObjectBase::InnerQueryInterface(riid, ppvObject);
		}
	}
}

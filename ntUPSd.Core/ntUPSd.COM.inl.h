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

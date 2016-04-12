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

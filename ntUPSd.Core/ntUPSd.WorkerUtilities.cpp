#include "stdafx.h"
#include "ntUPSd.WorkerUtilities.h"

namespace CTL
{
	ULONG CReferenceCounted::AddRef() noexcept
	{
		return ::InterlockedIncrement(&m_cRefs);
	}

	ULONG CReferenceCounted::Release() noexcept
	{
		ULONG cRefs = ::InterlockedDecrement(&m_cRefs);
		if (cRefs == 0)
		{
			delete this;
		}

		return cRefs;
	}

	HRESULT CAbstractWorkClient::CloseHandle(HANDLE hHandle) noexcept
	{
		if (!::CloseHandle(hHandle))
		{
			return ::ATL::AtlHresultFromLastError();
		}

		delete this;
		return S_OK;
	}
}

#pragma once
#include "ntUPSd.ServerModule.h"

namespace CTL
{
	template <typename Derived, UINT idServiceName>
	HRESULT CServerModuleT<Derived, idServiceName>::Start(_In_ int nShowCmd) noexcept
	{
		SERVICE_TABLE_ENTRY st[] =
		{
			{ m_szServiceName, _ServiceMain },
			{ NULL, NULL }
		};

		if (::StartServiceCtrlDispatcher(st) == 0)
		{
			DWORD dwError = GetLastError();
			if (dwError == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
			{
				m_bService = FALSE;
			}
			else
			{
				m_status.dwWin32ExitCode = GetLastError();
			}
		}

		if (m_bService)
		{
			return ::AtlHresultFromWin32(m_status.dwWin32ExitCode);
		}

#ifndef _ATL_NO_COM_SUPPORT
		HRESULT hr = InitializeCom();
		if (FAILED(hr))
		{
			// Ignore RPC_E_CHANGED_MODE if CLR is loaded. Error is due to CLR initializing
			// COM and InitializeCOM trying to initialize COM with different flags.
			if (hr != RPC_E_CHANGED_MODE || GetModuleHandle(_T("Mscoree.dll")) == NULL)
			{
				return hr;
			}
		}
		else
		{
			m_bComInitialized = true;
		}
#endif //_ATL_NO_COM_SUPPORT

		m_status.dwWin32ExitCode = Run(nShowCmd);
		return ::AtlHresultFromWin32(m_status.dwWin32ExitCode);
	}
}
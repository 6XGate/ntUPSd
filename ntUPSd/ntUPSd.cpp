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
#include "ntUPSd.Server.h"

class CNetworkUpsDaemonModule : public CServerModuleT<CNetworkUpsDaemonModule, IDS_SERVICENAME>
{
public :
	DECLARE_REGISTRY_APPID_RESOURCEID(INFINITE, "{153BA2A2-89A0-425C-81E4-70A5FBA48498}")

	HRESULT PreMessageLoop(_In_ int nShowCmd) noexcept
	{
		HRESULT hr = __super::PreMessageLoop(nShowCmd);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = m_ws.Startup(MAKEWORD(2, 2));
		if (FAILED(hr))
		{
			return hr;
		}

		m_pListenerClient.Attach(_ATL_NEW CServerWorker);
		if (m_pListenerClient == nullptr)
		{
			return hr = E_OUTOFMEMORY;
		}

		hr = m_pListenerClient->Initialize();
		if (FAILED(hr))
		{
			return hr;
		}

		SetServiceStatus(SERVICE_RUNNING);
		return S_OK;
	}

	// TODO: Stop service handler, shutdown listener thread and client threads.
	// Maybe, probably can handle this completely in PostMessageLoop too.

	HRESULT PostMessageLoop() noexcept
	{
		// Clean-up code here...
		m_pListenerClient.Free();
		SetServiceStatus(SERVICE_STOPPED);

		HRESULT hr = __super::PostMessageLoop();
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}

	HRESULT InitializeSecurity() noexcept
	{
		if (m_bService)
		{
			// TODO : Call CoInitializeSecurity and provide the appropriate security settings for your service
			// Suggested - PKT Level Authentication,
			// Impersonation Level of RPC_C_IMP_LEVEL_IDENTIFY
			// and an appropriate Non NULL Security Descriptor.
		}

		return S_OK;
	}

private:
	CAutoPtr<CServerWorker> m_pListenerClient;
	CWinSock m_ws;
};

CNetworkUpsDaemonModule _AtlModule;

template <typename T>
using CLocalPtr = ::ATL::CHeapPtr<T, CLocalAllocator>;


/**
@breif `_tWinMain` is the main entry-point for the service.
@param [in] hInstance
	The instance handle or base-address of the module.
@param [in] hPrevInstance
	The previous instance handle of the module. This is always _NULL_.
@param [in] lpCmdLine
	The command-line used to invoke the service.
@param [in] nShowCmd
	The show command used to invoke the service.
@return
	**0** on success; otherwise, a _error-level_ status code indicating an error.
*/
EXTERN_C INT WINAPI _tWinMain(
	_In_ HINSTANCE /*hInstance*/,
	_In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_z_ LPTSTR /*lpCmdLine*/,
	_In_ INT nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

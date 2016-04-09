#include "stdafx.h"
#include "ntUPSd.Server.h"

HRESULT CServerWorker::Initialize() noexcept
{
	HRESULT hr = __super::Initialize();
	if (FAILED(hr))
	{
		return hr;
	}

	CHandle hAcceptEvent(::CreateEvent(nullptr, FALSE, FALSE, nullptr));
	if (hAcceptEvent == NULL)
	{
		return hr = AtlHresultFromLastError();
	}

	CAutoPtr<CServer> pServer(_ATL_NEW CServer);
	if (pServer == nullptr)
	{
		return hr = E_OUTOFMEMORY;
	}

	hr = pServer->Initialize(hAcceptEvent);
	if (FAILED(hr))
	{
		return hr;
	}

	CAutoPtr<CServerOnConnectProxy> pOnConnect(_ATL_NEW CServerOnConnectProxy(pServer));
	if (pOnConnect != nullptr)
	{
		pServer.Detach();
	}
	else
	{
		return hr = E_OUTOFMEMORY;
	}

	hr = AddHandle(hAcceptEvent, pOnConnect, 0);
	if (SUCCEEDED(hr))
	{
		hAcceptEvent.Detach();
		pOnConnect.Detach();
	}
	else
	{
		return hr;
	}

	return S_OK;
}

BOOL CClient::Initialize(void *pvParam) noexcept
{
	return TRUE;
}

void CClient::Execute(RequestType request, void *pWorkerParam, OVERLAPPED *pOverlapped) noexcept
{
	CSocket hClient(request);
	CReplDriver<CCommandProcessor> repl;
	HRESULT hr = repl.Initialize(hClient, hClient);
	if (SUCCEEDED(hr))
	{
		hr = repl.BeginProcessing();
		if (FAILED(hr))
		{
			ATLTRACE(_T("Client thread: REPL processor failed; %08Xh\n"), hr);
		}
	}
	else
	{
		ATLTRACE(_T("Client thread: REPL driver initialization failured; %08Xh\n"), hr);
	}

	hr = hClient.Shutdown();
	if (FAILED(hr) && hr != __HRESULT_FROM_WIN32(WSAECONNRESET))
	{
		ATLTRACE(_T("Client thread: Socket shutdown failed; %08Xh\n"), hr);
	}
}

void CClient::Terminate(void *pvParam) noexcept
{

}

HRESULT CServer::Initialize(HANDLE hAcceptEvent) noexcept
{
	HRESULT hr = m_ThreadPool.Initialize();
	if (FAILED(hr))
	{
		return hr;
	}

	ADDRINFOT aiHints = {};
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	aiHints.ai_flags = AI_PASSIVE;

	hr = m_ai.GetInfo(nullptr, _T("3493"), &aiHints);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_hServer.Create(m_ai);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_hServer.Select(hAcceptEvent, FD_ACCEPT);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_hServer.Bind(m_ai);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_hServer.Listen();
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

HRESULT CServer::OnConnect() noexcept
{
	_ATLTRY
	{
		CSocket hClient;
		HRESULT hr = hClient.Accept(m_hServer);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = hClient.Select(nullptr, 0);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = hClient.IOControlSet(FIONBIO, FALSE);
		if (FAILED(hr))
		{
			return hr;
		}

		if (m_ThreadPool.QueueRequest(hClient.GetHandle()))
		{
			hClient.Detach();
		}
		else
		{
			return hr = AtlHresultFromLastError();
		}

		return S_OK;
	}
	_ATLCATCH(ex)
	{
		return ex.m_hr;
	}
	_ATLCATCHALL()
	{
		return E_FAIL;
	}
}

HRESULT CServerOnConnectProxy::Execute(_In_opt_ DWORD_PTR dwParam, _In_ HANDLE hObject) noexcept
{
	return m_pTarget->OnConnect();
}

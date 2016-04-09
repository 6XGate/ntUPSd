#pragma once
#include "CommandProcessor.h"

class CServerWorker final : public CWorkerThread<>
{
public:
	CServerWorker() noexcept = default;
	CServerWorker(CServerWorker &&) = delete;
	CServerWorker(const CServerWorker &) = delete;
	CServerWorker &operator =(CServerWorker &&) = delete;
	CServerWorker &operator =(const CServerWorker &) = delete;
	~CServerWorker() noexcept = default;

	HRESULT Initialize() noexcept;
};

class CClient final
{
public:
	typedef SOCKET RequestType;

	CClient() noexcept = default;
	CClient(CClient &&) = delete;
	CClient(const CClient &) = delete;
	CClient &operator =(CClient &&) = delete;
	CClient &operator =(const CClient &) = delete;
	~CClient() noexcept = default;

	BOOL Initialize(_In_opt_ void *pvParam) noexcept;
	void Execute(_In_ RequestType request, _In_opt_ void *pWorkerParam, _In_opt_ OVERLAPPED *pOverlapped) noexcept;
	void Terminate(_In_ void *pvParam) noexcept;
};

class CServer final : public CReferenceCounted
{
public:
	CServer() noexcept = default;
	CServer(CServer &&) = delete;
	CServer(const CServer &) = delete;
	CServer &operator =(CServer &&) = delete;
	CServer &operator =(const CServer &) = delete;
	~CServer() noexcept = default;

	HRESULT Initialize(_In_ HANDLE hAcceptEvent) noexcept;

	HRESULT OnConnect() noexcept;

private:
	CThreadPool<CClient> m_ThreadPool;
	CAddressInfo m_ai;
	CSocket m_hServer;
};

class CServerOnConnectProxy : public CAbstractProxy<CServer>
{
public:
	using CAbstractProxy<CServer>::CAbstractProxy;

	virtual HRESULT Execute(_In_opt_ DWORD_PTR dwParam, _In_ HANDLE hObject) noexcept override;
};

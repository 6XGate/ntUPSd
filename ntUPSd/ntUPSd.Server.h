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

	HRESULT Initialize(_In_ HANDLE hStopEvent) noexcept;
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

	HRESULT Initialize(_In_ HANDLE hStopEvent, _In_ HANDLE hAcceptEvent) noexcept;

	HRESULT OnConnect() noexcept;

private:
	CThreadPool<CClient> m_ThreadPool;
	CAddressInfo m_ai;
	CSocket m_hServer;
	HANDLE m_hStopEvent;
};

class CServerOnConnectProxy : public CAbstractProxy<CServer>
{
public:
	using CAbstractProxy<CServer>::CAbstractProxy;

	virtual HRESULT Execute(_In_opt_ DWORD_PTR dwParam, _In_ HANDLE hObject) noexcept override;
};

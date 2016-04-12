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
#include "ntUPSd.Socket.h"

namespace CTL
{
	CWinSock::~CWinSock() noexcept
	{
		if (m_fInitialized)
		{
			::WSACleanup();
		}
	}

	HRESULT CWinSock::GetLastError() noexcept
	{
		return ::ATL::AtlHresultFromWin32(::WSAGetLastError());
	}

	HRESULT CWinSock::Startup(WORD wVersionRequested) noexcept
	{
		auto nError = ::WSAStartup(wVersionRequested, &m_wsadata);
		if (nError != NO_ERROR)
		{
			return ::ATL::AtlHresultFromWin32(nError);
		}

		m_fInitialized = TRUE;
		return S_OK;
	}

	CAddressInfo::CAddressInfo(ADDRINFOT *pai) noexcept :
		m_pai(pai)
	{
	}

	CAddressInfo::CAddressInfo(CAddressInfo &&rOther) noexcept :
		m_pai(rOther.Detach())
	{
	}

	CAddressInfo &CAddressInfo::operator =(CAddressInfo &&rOther) noexcept
	{
		m_pai = rOther.Detach();
		return *this;
	}

	CAddressInfo::~CAddressInfo() noexcept
	{
		Free();
	}

	ADDRINFOT *CAddressInfo::operator->() noexcept
	{
		return m_pai;
	}

	const ADDRINFOT *CAddressInfo::operator->() const noexcept
	{
		return m_pai;
	}

	CAddressInfo::operator ADDRINFOT*() noexcept
	{
		return m_pai;
	}

	CAddressInfo::operator const ADDRINFOT*() const noexcept
	{
		return m_pai;
	}

	void CAddressInfo::Attach(ADDRINFOT *pai) noexcept
	{
		ATLASSERT(m_pai == nullptr);
		m_pai = pai;
	}

	ADDRINFOT *CAddressInfo::Detach() noexcept
	{
		ADDRINFOT *pai = m_pai;
		m_pai = nullptr;
		return pai;
	}

	HRESULT CAddressInfo::Free() noexcept
	{
		if (m_pai != nullptr)
		{
			::FreeAddrInfo(Detach());
		}

		return S_OK;
	}

	HRESULT CAddressInfo::GetInfo(
		_In_opt_z_ LPCTSTR pszNodeName,
		_In_opt_z_ LPCTSTR pszServiceName,
		_In_opt_ const ADDRINFOT *pHints) noexcept
	{
		Free();

		auto nError = ::GetAddrInfo(pszNodeName, pszServiceName, pHints, &m_pai);
		if (nError != NO_ERROR)
		{
			return ::ATL::AtlHresultFromWin32(nError);
		}

		return S_OK;
	}

	CSocket::CSocket(SOCKET socket) noexcept :
		CHandle(reinterpret_cast<HANDLE>(socket))
	{
	}

	CSocket::CSocket(CSocket &&rOther) noexcept :
		CSocket(rOther.Detach())
	{
	}

	CSocket &CSocket::operator =(CSocket &&rOther) noexcept
	{
		Close();

		Attach(rOther.Detach());
		return *this;
	}

	SOCKET CSocket::GetHandle() const noexcept
	{
		return reinterpret_cast<SOCKET>(m_h);
	}

	void CSocket::Attach(SOCKET s) noexcept
	{
		__super::Attach(reinterpret_cast<HANDLE>(s));
	}

	SOCKET CSocket::Detach() noexcept
	{
		return reinterpret_cast<SOCKET>(__super::Detach());
	}

	HRESULT CSocket::Create(const ADDRINFOT * pai) noexcept
	{
		SOCKET hSocket = ::WSASocket(pai->ai_family, pai->ai_socktype, pai->ai_protocol, nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT);
		if (hSocket == INVALID_SOCKET)
		{
			return CWinSock::GetLastError();
		}

		Close();
		Attach(hSocket);
		return S_OK;
	}

	HRESULT CSocket::Bind(const ADDRINFOT *pai) noexcept
	{
		auto nResult = ::bind(GetHandle(), pai->ai_addr, static_cast<int>(pai->ai_addrlen));
		if (nResult == SOCKET_ERROR)
		{
			return CWinSock::GetLastError();
		}

		return S_OK;
	}

	HRESULT CSocket::Listen(int nBackLog) noexcept
	{
		auto nResult = ::listen(GetHandle(), nBackLog & SOMAXCONN);
		if (nResult == SOCKET_ERROR)
		{
			return CWinSock::GetLastError();
		}

		return S_OK;
	}

	HRESULT CSocket::Accept(const CSocket &hListeningSocket, SOCKADDR *pAddr, int *pcbAddr) noexcept
	{
		if (hListeningSocket == NULL)
		{
			return E_INVALIDARG;
		}

		SOCKET hClient = ::accept(hListeningSocket.GetHandle(), pAddr, pcbAddr);
		if (hClient == INVALID_SOCKET)
		{
			return CWinSock::GetLastError();
		}

		Close();
		Attach(hClient);
		return S_OK;
	}

	HRESULT CSocket::Shutdown() noexcept
	{
		auto nResult = ::shutdown(GetHandle(), SD_BOTH);
		if (nResult == SOCKET_ERROR)
		{
			return CWinSock::GetLastError();
		}

		return S_OK;
	}

	HRESULT CSocket::Receive(void *pvBuffer, int cbBuffer, int *pcbReceived, int eFlags) noexcept
	{
		int cbReceived = ::recv(GetHandle(), reinterpret_cast<char*>(pvBuffer), cbBuffer, eFlags);
		if (cbReceived == SOCKET_ERROR)
		{
			return CWinSock::GetLastError();
		}

		if (pcbReceived != nullptr)
		{
			*pcbReceived = cbReceived;
		}

		return S_OK;
	}

	HRESULT CSocket::Send(const void *pvBuffer, int cbBuffer, int *pcbSent, int eFlags) noexcept
	{
		int cbSent = ::send(GetHandle(), reinterpret_cast<const char *>(pvBuffer), cbBuffer, eFlags);
		if (cbSent == SOCKET_ERROR)
		{
			return CWinSock::GetLastError();
		}

		if (pcbSent != nullptr)
		{
			*pcbSent = cbSent;
		}

		return S_OK;
	}

	HRESULT CSocket::Select(HANDLE hEvent, long lEvents) noexcept
	{
		auto nResult = ::WSAEventSelect(GetHandle(), hEvent, lEvents);
		if (nResult == SOCKET_ERROR)
		{
			return CWinSock::GetLastError();
		}

		return S_OK;
	}

	HRESULT CSocket::IOControlSet(long eCommand, ULONG lArg) noexcept
	{
		return IOControlGet(eCommand, &lArg);
	}

	HRESULT CSocket::IOControlGet(long eCommand, ULONG *plArg) noexcept
	{
		auto nResult = ::ioctlsocket(GetHandle(), eCommand, plArg);
		if (nResult == SOCKET_ERROR)
		{
			return CWinSock::GetLastError();
		}

		return S_OK;
	}

	HRESULT CSocketStream::New(SOCKET s, HANDLE hCancelEvent, IStream **ppstm) noexcept
	{
		if (ppstm == nullptr)
		{
			return E_POINTER;
		}

		::ATL::CComObject<CSocketStream> *pstm = nullptr;
		HRESULT hr = ::ATL::CComObject<CSocketStream>::CreateInstance(&pstm);
		if (SUCCEEDED(hr))
		{
			if (hCancelEvent != NULL)
			{
				::ATL::CHandle hReadEvent(::CreateEvent(nullptr, TRUE, FALSE, nullptr));
				if (hReadEvent == NULL)
				{
					return ::ATL::AtlHresultFromLastError();
				}

				if (::WSAEventSelect(s, hReadEvent, FD_READ) == SOCKET_ERROR)
				{
					return CWinSock::GetLastError();
				}

				if (::WSAEventSelect(s, hCancelEvent, FD_READ) == SOCKET_ERROR)
				{
					DWORD dwLastError = ::WSAGetLastError();
					::WSAEventSelect(s, NULL, 0);

					return ::ATL::AtlHresultFromWin32(dwLastError);
				}

				pstm->m_hEventHandles[0].Attach(hCancelEvent);
				pstm->m_hEventHandles[1].Attach(hReadEvent.Detach());
			}

			pstm->m_hSocket.Attach(s);
			pstm->AddRef();
		}

		*ppstm = pstm;
		return hr;
	}

	STDMETHODIMP CSocketStream::Read(void *pv, ULONG cb, ULONG *pcbRead) noexcept
	{
		if (pv == nullptr)
		{
			return E_INVALIDARG;
		}

		if (cb > INT_MAX)
		{
			return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}

		if (m_hEventHandles[0] != NULL)
		{
			DWORD dwResult = ::WaitForMultipleObjects(_countof(m_hEventHandles), reinterpret_cast<HANDLE*>(m_hEventHandles), FALSE, INFINITE);
			switch (dwResult)
			{
			case WAIT_OBJECT_0 + 0:
				// The read was cancelled.
				return E_ABORT;
			case WAIT_OBJECT_0 + 1:
				// There is data waiting.
				break;
			case WAIT_FAILED:
				// Something went wrong while the wait.
				return ::ATL::AtlHresultFromLastError();
			case WAIT_TIMEOUT:
			default:
				// This should not happen.
				return E_FAIL;
			}
		}

		return m_hSocket.Receive(pv, static_cast<int>(cb), reinterpret_cast<int*>(pcbRead));
	}

	STDMETHODIMP CSocketStream::Write(const void *pv, ULONG cb, ULONG *pcbWritten) noexcept
	{
		if (pv == nullptr)
		{
			return E_INVALIDARG;
		}

		if (cb > INT_MAX)
		{
			return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}

		return m_hSocket.Send(pv, static_cast<int>(cb), reinterpret_cast<int*>(pcbWritten));
	}

	STDMETHODIMP CSocketStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition) noexcept
	{
		UNREFERENCED_PARAMETER(dlibMove);
		UNREFERENCED_PARAMETER(dwOrigin);
		UNREFERENCED_PARAMETER(plibNewPosition);
		return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	STDMETHODIMP CSocketStream::SetSize(ULARGE_INTEGER libNewSize) noexcept
	{
		UNREFERENCED_PARAMETER(libNewSize);
		return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	STDMETHODIMP CSocketStream::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) noexcept
	{
		if (pstm == nullptr)
		{
			return E_INVALIDARG;
		}

		if (cb.QuadPart == 0)
		{
			return S_OK;
		}

		::ATL::CHeapPtr<BYTE> pBuffer;
		if (!pBuffer.AllocateBytes(4194304))
		{
			return E_OUTOFMEMORY;
		}

		ULARGE_INTEGER cbTempRead;
		if (pcbRead == nullptr)
		{
			pcbRead = &cbTempRead;
		}

		ULARGE_INTEGER cbTempWritten;
		if (pcbWritten == nullptr)
		{
			pcbWritten = &cbTempWritten;
		}

		HRESULT hr;
		while (cb.QuadPart > 0)
		{
			ULONG cbRead, cbToRead = cb.QuadPart > 4194304 ? 4194304 : cb.LowPart;
			hr = Read(pBuffer.m_pData, cbToRead, &cbRead);
			if (FAILED(hr))
			{
				return hr;
			}

			if (cbRead == 0)
			{
				return __HRESULT_FROM_WIN32(WSAEDISCON);
			}

			pcbRead->QuadPart += cbRead;
			ULONG cbToWrite = cbRead;
			while (cbToWrite > 0)
			{
				ULONG cbWritten;
				hr = pstm->Write(pBuffer.m_pData, cbToWrite, &cbWritten);
				if (FAILED(hr))
				{
					return hr;
				}

				pcbWritten->QuadPart += cbWritten;
				cbToWrite -= cbWritten;
			}

			cb.QuadPart -= cbRead;
		}

		return S_OK;
	}

	STDMETHODIMP CSocketStream::Commit(DWORD grfCommitFlags) noexcept
	{
		UNREFERENCED_PARAMETER(grfCommitFlags);
		return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	STDMETHODIMP CSocketStream::Revert() noexcept
	{
		return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	STDMETHODIMP CSocketStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) noexcept
	{
		UNREFERENCED_PARAMETER(libOffset);
		UNREFERENCED_PARAMETER(cb);
		UNREFERENCED_PARAMETER(dwLockType);
		return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	STDMETHODIMP CSocketStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) noexcept
	{
		UNREFERENCED_PARAMETER(libOffset);
		UNREFERENCED_PARAMETER(cb);
		UNREFERENCED_PARAMETER(dwLockType);
		return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	STDMETHODIMP CSocketStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag) noexcept
	{
		if (pstatstg == nullptr)
		{
			return E_INVALIDARG;
		}

		if (grfStatFlag == 0)
		{
			return S_OK;
		}

		return E_NOTIMPL;
	}

	STDMETHODIMP CSocketStream::Clone(IStream **ppstm) noexcept
	{
		UNREFERENCED_PARAMETER(ppstm);
		return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}
}

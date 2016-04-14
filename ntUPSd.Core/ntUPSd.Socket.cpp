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

	void CSocket::Attach(_In_ SOCKET s) noexcept
	{
		__super::Attach(reinterpret_cast<HANDLE>(s));
	}

	SOCKET CSocket::Detach() noexcept
	{
		return reinterpret_cast<SOCKET>(__super::Detach());
	}

	HRESULT CSocket::Create(_In_ const ADDRINFOT * pai) noexcept
	{
		SOCKET hSocket = ::WSASocket(pai->ai_family, pai->ai_socktype, pai->ai_protocol, nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT);
		if (hSocket == INVALID_SOCKET)
		{
			HRESULT hr = CWinSock::GetLastError();
			__analysis_assume(FAILED(hr));
			return hr;
		}

		Close();
		Attach(hSocket);
		return S_OK;
	}

	HRESULT CSocket::Bind(_In_ const ADDRINFOT *pai) noexcept
	{
		auto nResult = ::bind(GetHandle(), pai->ai_addr, static_cast<int>(pai->ai_addrlen));
		if (nResult == SOCKET_ERROR)
		{
			HRESULT hr = CWinSock::GetLastError();
			__analysis_assume(FAILED(hr));
			return hr;
		}

		return S_OK;
	}

	HRESULT CSocket::Listen(int nBackLog) noexcept
	{
		auto nResult = ::listen(GetHandle(), nBackLog & SOMAXCONN);
		if (nResult == SOCKET_ERROR)
		{
			HRESULT hr = CWinSock::GetLastError();
			__analysis_assume(FAILED(hr));
			return hr;
		}

		return S_OK;
	}

	HRESULT CSocket::Accept(const CSocket &hListeningSocket, _Out_opt_bytecap_post_bytecount_(*pcbAddr, *pcbAddr) SOCKADDR *pAddr, _Inout_opt_ int *pcbAddr) noexcept
	{
		if (hListeningSocket == NULL)
		{
			return E_INVALIDARG;
		}

		SOCKET hClient = ::accept(hListeningSocket.GetHandle(), pAddr, pcbAddr);
		if (hClient == INVALID_SOCKET)
		{
			HRESULT hr = CWinSock::GetLastError();
			__analysis_assume(FAILED(hr));
			return hr;
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
			HRESULT hr = CWinSock::GetLastError();
			__analysis_assume(FAILED(hr));
			return hr;
		}

		return S_OK;
	}

	HRESULT CSocket::Receive(_Out_bytecap_post_bytecount_(cbBuffer, *pcbReceived) void *pvBuffer, _In_ int cbBuffer, _Out_opt_ int *pcbReceived, int eFlags) noexcept
	{
		int cbReceived = ::recv(GetHandle(), reinterpret_cast<char*>(pvBuffer), cbBuffer, eFlags);
		if (cbReceived == SOCKET_ERROR)
		{
			HRESULT hr = CWinSock::GetLastError();
			__analysis_assume(FAILED(hr));
			return hr;
		}

		if (pcbReceived != nullptr)
		{
			*pcbReceived = cbReceived;
		}

		return S_OK;
	}

	HRESULT CSocket::Send(_In_bytecount_(cbBuffer) const void *pvBuffer, _In_ int cbBuffer, _Out_opt_ int *pcbSent, int eFlags) noexcept
	{
		int cbSent = ::send(GetHandle(), reinterpret_cast<const char *>(pvBuffer), cbBuffer, eFlags);
		if (cbSent == SOCKET_ERROR)
		{
			HRESULT hr = CWinSock::GetLastError();
			__analysis_assume(FAILED(hr));
			return hr;
		}

		if (pcbSent != nullptr)
		{
			*pcbSent = cbSent;
		}

		return S_OK;
	}

	HRESULT CSocket::Select(_In_ HANDLE hEvent, long lEvents) noexcept
	{
		auto nResult = ::WSAEventSelect(GetHandle(), hEvent, lEvents);
		if (nResult == SOCKET_ERROR)
		{
			HRESULT hr = CWinSock::GetLastError();
			__analysis_assume(FAILED(hr));
			return hr;
		}

		return S_OK;
	}

	HRESULT CSocket::Deselect() noexcept
	{
		auto nResult = ::WSAEventSelect(GetHandle(), NULL, 0);
		if (nResult == SOCKET_ERROR)
		{
			HRESULT hr = CWinSock::GetLastError();
			__analysis_assume(FAILED(hr));
			return hr;
		}

		return S_OK;
	}

	HRESULT CSocket::IOControlSet(long eCommand, ULONG lArg) noexcept
	{
		return IOControlGet(eCommand, &lArg);
	}

	HRESULT CSocket::IOControlGet(long eCommand, _Inout_ ULONG *plArg) noexcept
	{
		auto nResult = ::ioctlsocket(GetHandle(), eCommand, plArg);
		if (nResult == SOCKET_ERROR)
		{
			HRESULT hr = CWinSock::GetLastError();
			__analysis_assume(FAILED(hr));
			return hr;
		}

		return S_OK;
	}

	CSocketStream::~CSocketStream() noexcept
	{
		if (m_hEventHandles[1] != NULL)
		{
			::CloseHandle(m_hEventHandles[1]);
		}
	}

	HRESULT CSocketStream::New(_In_ SOCKET s, _In_opt_ HANDLE hCancelEvent, _COM_Outptr_ IStream **ppstm) noexcept
	{
		if (ppstm == nullptr)
		{
			return E_POINTER;
		}

		*ppstm = nullptr;
		::ATL::CComPtr<CComLiteObject<CSocketStream>> pstm(_ATL_NEW CComLiteObject<CSocketStream>());
		if (pstm == nullptr)
		{
			return E_OUTOFMEMORY;
		}

		HRESULT hr = S_OK;
		if (hCancelEvent != NULL)
		{
			#pragma warning(suppress: 6387) // Checked after construction.
			::ATL::CHandle hReadEvent(::CreateEvent(nullptr, FALSE, FALSE, nullptr));
			if (hReadEvent == NULL)
			{
				hr = ::ATL::AtlHresultFromLastError();
				__analysis_assume(FAILED(hr));
				return hr;
			}

			if (::WSAEventSelect(s, hReadEvent, FD_READ | FD_CLOSE) == SOCKET_ERROR)
			{
				hr = CWinSock::GetLastError();
				__analysis_assume(FAILED(hr));
				return hr;
			}

			pstm->m_hEventHandles[0] = hCancelEvent;
			pstm->m_hEventHandles[1] = hReadEvent.Detach();
		}

		pstm->m_hSocket.Attach(s);
		pstm.p->AddRef();

		*ppstm = pstm;
		return hr;
	}

	STDMETHODIMP CSocketStream::Read(_Out_writes_bytes_to_(cb, *pcbRead) _Pre_defensive_ void *pv, ULONG cb, _Out_opt_ ULONG *pcbRead) noexcept
	{
		if (pv == nullptr)
		{
			return E_INVALIDARG;
		}

		if (cb > INT_MAX)
		{
			return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}

		if (m_hEventHandles[1] != NULL)
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

	STDMETHODIMP CSocketStream::Write(_In_reads_bytes_(cb) _Pre_defensive_ const void *pv, ULONG cb, _Out_opt_ ULONG *pcbWritten) noexcept
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

	STDMETHODIMP CSocketStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, _Out_opt_ ULARGE_INTEGER *plibNewPosition) noexcept
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

	STDMETHODIMP CSocketStream::CopyTo(_In_ _Pre_defensive_ IStream *pstm, ULARGE_INTEGER cb, _Out_opt_ ULARGE_INTEGER *pcbRead, _Out_opt_ ULARGE_INTEGER *pcbWritten) noexcept
	{
		if (pstm == nullptr)
		{
			return E_INVALIDARG;
		}

		ULARGE_INTEGER cbTempRead = { 0,0 };
		ULARGE_INTEGER cbTempWritten = { 0,0 };
		if (cb.QuadPart != 0)
		{
			::ATL::CHeapPtr<BYTE> pBuffer;
			if (!pBuffer.AllocateBytes(4194304))
			{
				return E_OUTOFMEMORY;
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

				cbTempRead.QuadPart += cbRead;
				ULONG cbToWrite = cbRead;
				while (cbToWrite > 0)
				{
					ULONG cbWritten;
					hr = pstm->Write(pBuffer.m_pData, cbToWrite, &cbWritten);
					if (FAILED(hr))
					{
						return hr;
					}

					cbTempRead.QuadPart += cbWritten;
					cbToWrite -= cbWritten;
				}

				cb.QuadPart -= cbRead;
			}
		}

		if (pcbRead != nullptr)
		{
			pcbRead->QuadPart = cbTempRead.QuadPart;
		}

		if (pcbWritten != nullptr)
		{
			pcbWritten->QuadPart = cbTempWritten.QuadPart;
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

	STDMETHODIMP CSocketStream::Stat(_Out_ _Pre_defensive_ STATSTG *pstatstg, DWORD grfStatFlag) noexcept
	{
		if (pstatstg == nullptr)
		{
			return E_INVALIDARG;
		}

		if (grfStatFlag == 0)
		{
			return E_INVALIDARG;
		}

		return E_NOTIMPL;
	}

	STDMETHODIMP CSocketStream::Clone(_COM_Outptr_ IStream **ppstm) noexcept
	{
		if (ppstm == nullptr)
		{
			return E_POINTER;
		}

		*ppstm = nullptr;
		return __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}
}

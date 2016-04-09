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

	void CSocket::Attach(SOCKET s) throw()
	{
		__super::Attach(reinterpret_cast<HANDLE>(s));
	}

	SOCKET CSocket::Detach() throw()
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
}

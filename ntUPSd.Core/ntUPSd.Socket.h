#pragma once

namespace CTL
{
	class CWinSock
	{
	public:
		WSADATA m_wsadata = {};

		CWinSock() noexcept = default;
		CWinSock(CWinSock &&) = delete;
		CWinSock(const CWinSock &) = delete;
		CWinSock &operator =(CWinSock &&) = delete;
		CWinSock &operator =(const CWinSock &) = delete;
		~CWinSock() noexcept;

		static HRESULT GetLastError() noexcept;

		HRESULT Startup(_In_ WORD wVersionRequested) noexcept;

	private:
		BOOL m_fInitialized = FALSE;
	};

	class CAddressInfo
	{
	public:
		ADDRINFOT *m_pai = nullptr;

		CAddressInfo() noexcept = default;
		explicit CAddressInfo(ADDRINFOT *pai) noexcept;
		CAddressInfo(CAddressInfo &&rOther) noexcept;
		CAddressInfo(const CAddressInfo &) = delete;
		CAddressInfo &operator =(CAddressInfo &&rOther) noexcept;
		CAddressInfo &operator =(const CAddressInfo &) = delete;
		~CAddressInfo() noexcept;

		ADDRINFOT *operator ->() noexcept;
		const ADDRINFOT *operator ->() const noexcept;

		operator ADDRINFOT *() noexcept;
		operator const ADDRINFOT *() const noexcept;

		void Attach(ADDRINFOT *pai) noexcept;
		ADDRINFOT *Detach() noexcept;

		HRESULT Free() noexcept;

		HRESULT GetInfo(
			_In_opt_z_ LPCTSTR pszNodeName,
			_In_opt_z_ LPCTSTR pszServiceName,
			_In_opt_ const ADDRINFOT *pHints) noexcept;
	};

	class CSocket :
		public ::ATL::CHandle
	{
	public:
		CSocket() noexcept = default;
		explicit CSocket(SOCKET socket) noexcept;
		CSocket(CSocket &&rOther) noexcept;
		CSocket(const CSocket &) = delete;
		CSocket &operator =(CSocket &&rOther) noexcept;
		CSocket &operator =(const CSocket &) = delete;
		~CSocket() noexcept = default;

		SOCKET GetHandle() const noexcept;

		void Attach(_In_ SOCKET s) throw();
		SOCKET Detach() throw();

		HRESULT Create(_In_ const ADDRINFOT *pai) noexcept;
		HRESULT Bind(_In_ const ADDRINFOT *pai) noexcept;
		HRESULT Listen(_In_ int nBackLog = SOMAXCONN) noexcept;
		HRESULT Accept(_In_ const CSocket &hListeningSocket, _Out_opt_bytecap_post_bytecount_(*pcbAddr, *pcbAddr) SOCKADDR *pAddr = nullptr, _Inout_opt_ int *pcbAddr = nullptr) noexcept;
		HRESULT Shutdown() noexcept;

		HRESULT Receive(_Out_bytecap_post_bytecount_(cbBuffer, *pcbReceived) void *pvBuffer, _In_ int cbBuffer, _Inout_opt_ int *pcbReceived = nullptr, int eFlags = 0) noexcept;
		HRESULT Send(_In_bytecount_(cbBuffer) const void *pvBuffer, _In_ int cbBuffer, _Inout_opt_ int *pcbSent = nullptr, int eFlags = 0) noexcept;

		HRESULT Select(_In_ HANDLE hEvent, long lEvents) noexcept;
		HRESULT IOControlSet(long eCommand, ULONG lArg) noexcept;
		HRESULT IOControlGet(long eCommand, _Inout_ ULONG *plArg) noexcept;
	};
}

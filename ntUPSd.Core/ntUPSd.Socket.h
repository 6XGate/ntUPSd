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
#include "ntUPSd.Core.Base.h"
#include "ntUPSd.COM.h"

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

		HRESULT Startup(WORD wVersionRequested) noexcept;

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

		void Attach(_In_ SOCKET s) noexcept;
		SOCKET Detach() noexcept;

		HRESULT Create(_In_ const ADDRINFOT *pai) noexcept;
		HRESULT Bind(_In_ const ADDRINFOT *pai) noexcept;
		HRESULT Listen(int nBackLog = SOMAXCONN) noexcept;
		HRESULT Accept(const CSocket &hListeningSocket, _Out_opt_bytecap_post_bytecount_(*pcbAddr, *pcbAddr) SOCKADDR *pAddr = nullptr, _Inout_opt_ int *pcbAddr = nullptr) noexcept;
		HRESULT Shutdown() noexcept;

		HRESULT Receive(_Out_bytecap_post_bytecount_(cbBuffer, *pcbReceived) void *pvBuffer, _In_ int cbBuffer, _Out_opt_ int *pcbReceived = nullptr, int eFlags = 0) noexcept;
		HRESULT Send(_In_bytecount_(cbBuffer) const void *pvBuffer, _In_ int cbBuffer, _Out_opt_ int *pcbSent = nullptr, int eFlags = 0) noexcept;

		HRESULT Select(_In_ HANDLE hEvent, long lEvents) noexcept;
		HRESULT Deselect() noexcept;
		HRESULT IOControlSet(long eCommand, ULONG lArg) noexcept;
		HRESULT IOControlGet(long eCommand, _Inout_ ULONG *plArg) noexcept;
	};

	class CSocketStream :
		public IUnknownImpl<IStream>
	{
	public:
		CSocketStream(CSocketStream &&) = delete;
		CSocketStream(const CSocketStream &) = delete;
		CSocketStream &operator =(CSocketStream &&) = delete;
		CSocketStream &operator =(const CSocketStream &) = delete;
		virtual ~CSocketStream() noexcept;

		static HRESULT New(_In_ SOCKET s, _In_opt_ HANDLE hCancelEvent, _COM_Outptr_ IStream **ppstm) noexcept;

		STDMETHOD(Read)(_Out_writes_bytes_to_(cb, *pcbRead) _Pre_defensive_ void *pv, ULONG cb, _Out_opt_ ULONG *pcbRead) noexcept override;
		STDMETHOD(Write)(_In_reads_bytes_(cb) _Pre_defensive_ const void *pv, ULONG cb, _Out_opt_ ULONG *pcbWritten) noexcept override;
		STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, _Out_opt_ ULARGE_INTEGER *plibNewPosition) noexcept override;
		STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize) noexcept override;
		STDMETHOD(CopyTo)(_In_ _Pre_defensive_ IStream *pstm, ULARGE_INTEGER cb, _Out_opt_ ULARGE_INTEGER *pcbRead, _Out_opt_ ULARGE_INTEGER *pcbWritten) noexcept override;
		STDMETHOD(Commit)(DWORD grfCommitFlags) noexcept override;
		STDMETHOD(Revert)() noexcept override;
		STDMETHOD(LockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) noexcept override;
		STDMETHOD(UnlockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) noexcept override;
		STDMETHOD(Stat)(_Out_ _Pre_defensive_ STATSTG *pstatstg, DWORD grfStatFlag) noexcept override;
		STDMETHOD(Clone)(_COM_Outptr_ IStream **ppstm) noexcept override;

	protected:
		CSocketStream() noexcept = default;

	private:
		HANDLE m_hEventHandles[2] = { NULL, NULL };
		CSocket m_hSocket;
	};
}

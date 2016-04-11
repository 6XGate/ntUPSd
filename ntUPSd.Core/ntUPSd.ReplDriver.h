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
#include "ntUPSd.Formatter.h"

namespace CTL
{
	[uuid("70024F71-9D71-4301-A9A8-24D2438A5478")]
	class ATL_NO_VTABLE IReplResult : public IUnknown
	{
	public:
		IReplResult(IReplResult &&) = delete;
		IReplResult(const IReplResult &) = delete;
		IReplResult &operator =(IReplResult &&) = delete;
		IReplResult &operator =(const IReplResult &) = delete;
		virtual ~IReplResult() noexcept = default;

		STDMETHOD(RenderResult)(::ATL::CStringA &strResult) noexcept PURE;

	protected:
		IReplResult() noexcept = default;
	};

	class ATL_NO_VTABLE CReplResult : public IReplResult
	{
	public:
		CReplResult(CReplResult &&) = delete;
		CReplResult(const CReplResult &) = delete;
		CReplResult &operator =(CReplResult &&) = delete;
		CReplResult &operator =(const CReplResult &) = delete;
		virtual ~CReplResult() noexcept = default;

		STDMETHOD_(ULONG, AddRef)() noexcept override;
		STDMETHOD_(ULONG, Release)() noexcept override;
		STDMETHOD(QueryInterface)(_In_ REFIID riid, _COM_Outptr_ void **ppvObject) noexcept override;

	protected:
		CReplResult() noexcept = default;

	private:
		ULONG m_cRefs = 0;
	};

	class CReplDriverBase
	{
	public:
		CReplDriverBase(CReplDriverBase &&) = delete;
		CReplDriverBase(const CReplDriverBase &) = delete;
		CReplDriverBase &operator =(CReplDriverBase &&) = delete;
		CReplDriverBase &operator =(const CReplDriverBase &) = delete;
		~CReplDriverBase() noexcept;

	protected:
		static const HRESULT E_DISCONNECTED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, WSAENOTCONN);
		static const SIZE_T BUFFER_SIZE = 65536;

		::ATL::CComPtr<IReplResult> m_pLastResult;
		::ATL::CHeapPtr<CHAR> m_pszBuffer;
		::ATL::CAtlFile m_hInput;
		::ATL::CAtlFile m_hOutput;

		CReplDriverBase() noexcept = default;

		LPSTR GetLine() noexcept;

		HRESULT Initialize(_In_ HANDLE hInput, _In_ HANDLE hOutput) noexcept;
		HRESULT ReadLine() noexcept;
		HRESULT WriteResult(_In_z_ LPCSTR pszResult) noexcept;
	};

	template <typename Processor>
	class CReplDriver : public CReplDriverBase
	{
	public:
		using CReplDriverBase::E_DISCONNECTED;

		CReplDriver() noexcept = default;
		CReplDriver(CReplDriver &&) = delete;
		CReplDriver(const CReplDriver &) = delete;
		CReplDriver &operator =(CReplDriver &&) = delete;
		CReplDriver &operator =(const CReplDriver &) = delete;

		HRESULT Initialize(_In_ HANDLE hInput, _In_ HANDLE hOutput) noexcept;
		HRESULT BeginProcessing() noexcept;

	private:
		Processor m_Processor;
	};
}

#include "ntUPSd.ReplDriver.inl.h"

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
#include "Battery.h"

class CCommandProcessor
{
public:
	CCommandProcessor() noexcept = default;
	CCommandProcessor(CCommandProcessor &&) = delete;
	CCommandProcessor(const CCommandProcessor &) = delete;
	CCommandProcessor &operator =(CCommandProcessor &&) = delete;
	CCommandProcessor &operator =(const CCommandProcessor &) = delete;
	~CCommandProcessor() noexcept = default;

	HRESULT Initialize() noexcept;
	HRESULT Eval(_In_z_ LPCSTR pszCommandLine, CComPtr<IReplResult> &rpResult) noexcept;

	BOOL Continue() const noexcept { return TRUE; }
	LPCSTR ReportError(HRESULT hr, LPCSTR) noexcept;
	HRESULT DefaultResult(CStringA &strResult) noexcept;

private:
	typedef HRESULT(CCommandProcessor::*PFNCOMMAND)(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult);

	static const int LAST_ERROR_BUFFER_LENGTH = 256;

	CAtlMap<CStringA, PFNCOMMAND> m_rgPrimeHandlers;
	CAtlMap<CStringA, PFNCOMMAND> m_rgListHandlers;
	CAtlMap<CStringA, PFNCOMMAND> m_rgGetHandlers;
	CAtlMap<HRESULT, CStringA> m_rgErrors;
	CHeapPtr<CHAR> m_pszLastError;

	CComPtr<CBatteryCollection> m_pBatteries;
	CStringA m_strUserName;
	CStringA m_strPassWord;

	LPCSTR GetPart(_Inout_z_ LPSTR &pszLine) noexcept;

	// Primary Commands:
	HRESULT OnStartTls(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept;
	HRESULT OnUserName(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept;
	HRESULT OnPassWord(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept;
	HRESULT OnGet(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept;
	HRESULT OnList(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept;
	HRESULT OnLogin(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept;
	HRESULT OnLogout(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept;

	// Sub-commands:
	HRESULT OnGetVar(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept;
	HRESULT OnListUps(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept;
};

#include "CommandProcessor.inl.h"

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

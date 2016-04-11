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
#include "CommandProcessor.h"

namespace
{
	class CSimpleResult : public CReplResult
	{
	public:
		explicit CSimpleResult(LPCSTR pszStaticResult) : m_pszStaticResult(pszStaticResult) { }
		STDMETHOD(RenderResult)(CStringA &strResult) noexcept
		{
			_ATLTRY
			{
				strResult = m_pszStaticResult;
				return S_OK;
			}
			_ATLCATCH(ex)
			{
				return ex.m_hr;
			}
			_ATLCATCHALL()
			{
				return E_FAIL;
			}
		}

	private:
		LPCSTR m_pszStaticResult;
	};
}

HRESULT CCommandProcessor::Initialize() noexcept
{
	_ATLTRY
	{
		if (!m_pszLastError.Allocate(LAST_ERROR_BUFFER_LENGTH))
		{
			return E_OUTOFMEMORY;
		}

		m_pBatteries = _ATL_NEW CBatteryCollection;
		if (m_pBatteries == nullptr)
		{
			return E_OUTOFMEMORY;
		}

		HRESULT hr = m_pBatteries->LoadBatteries();
		if (FAILED(hr))
		{
			return hr;
		}

		m_rgErrors.SetAt(S_OK, "OK");

		m_rgPrimeHandlers.SetAt("STARTTLS", &CCommandProcessor::OnStartTls);
		m_rgPrimeHandlers.SetAt("USERNAME", &CCommandProcessor::OnUserName);
		m_rgPrimeHandlers.SetAt("PASSWORD", &CCommandProcessor::OnPassWord);
		m_rgPrimeHandlers.SetAt("GET", &CCommandProcessor::OnGet);
		m_rgPrimeHandlers.SetAt("LIST", &CCommandProcessor::OnList);
		m_rgPrimeHandlers.SetAt("LOGIN", &CCommandProcessor::OnLogin);
		m_rgPrimeHandlers.SetAt("LOGOUT", &CCommandProcessor::OnLogout);

		m_rgGetHandlers.SetAt("VAR", &CCommandProcessor::OnGetVar);

		m_rgListHandlers.SetAt("UPS", &CCommandProcessor::OnListUps);

		return S_OK;
	}
	_ATLCATCH(ex)
	{
		return ex.m_hr;
	}
	_ATLCATCHALL()
	{
		return E_FAIL;
	}
}

HRESULT CCommandProcessor::Eval(_In_z_ LPCSTR pszCommandLine, CComPtr<IReplResult> &rpResult) noexcept
{
	CStringA strCommandLine = pszCommandLine;
	LPSTR pszParameters = strCommandLine.GetBuffer();
	LPCSTR pszCommand = GetPart(pszParameters);

	HRESULT hr = S_FALSE;
	if (pszCommand != nullptr)
	{
		auto pos = m_rgPrimeHandlers.Lookup(pszCommand);
		if (pos != nullptr)
		{
			auto pfnHandler = m_rgPrimeHandlers.GetValueAt(pos);
			hr = (this->*pfnHandler)(pszParameters, rpResult);
		}
		else
		{
			hr = NUT_E_UNKNOWNCMD;
		}
	}
	else
	{
		hr = S_OK;
	}

	strCommandLine.ReleaseBuffer();
	return hr;
}

LPCSTR CCommandProcessor::ReportError(HRESULT hr, LPCSTR) noexcept
{
	if (!(hr & 0x20000000) || HRESULT_FACILITY(hr) != FACILITY_NUT)
	{
		hr = NUT_E_UNREPORTABLE;
	}

	if (!::FormatMessageA(
		FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, 0x0000, m_pszLastError, LAST_ERROR_BUFFER_LENGTH, nullptr))
	{
		StringCchCopyA(m_pszLastError, LAST_ERROR_BUFFER_LENGTH, "UNKNOWN-ERROR");
	}

	return m_pszLastError;
}

HRESULT CCommandProcessor::DefaultResult(CStringA &strResult) noexcept
{
	_ATLTRY
	{
		strResult = "OK";
		return S_OK;
	}
	_ATLCATCH(ex)
	{
		return ex.m_hr;
	}
	_ATLCATCHALL()
	{
		return E_FAIL;
	}
}

LPCSTR CCommandProcessor::GetPart(_Inout_z_ LPSTR &pszLine) noexcept
{
	// A character position alias for pszLine.
	LPCH &pchPos = pszLine;

	// This all assumes the lines have been trimmed.
	if (*pchPos == 0)
	{
		return nullptr;
	}

	LPCSTR pszResult = nullptr;
	if (*pchPos == '"')
	{
		pszResult = ++pchPos;
		LPCH pchTo = pchPos;

		// Quoted part.
		bool fContinue = true;
		while (fContinue && *pchPos != '"' && *pchPos != 0)
		{
			if (*pchPos == '\\')
			{
				// Possible escaped character
				++pchPos;
				switch (*pchPos)
				{
				case 0:
					// Unexpected end-of-line, just leave the back-slash.
					--pchPos;
					fContinue = false;
					break;
				case '"':
				case '\\':
					break;
				default:
					// Push the position back, the next character cannot be escaped.
					--pchPos;
					break;
				}
			}

			*pchTo++ = *pchPos++;
		}

		// Zero out everything between pchTo and pchPos.
		while (pchTo != pchPos)
		{
			*pchTo++ = 0;
		}

		// Remove the ending quote.
		if (*pchPos == '"')
		{
			*pchPos++ = 0;
		}
	}
	else
	{
		// Find the first non-white-space character.  The part start at the beginning.
		pszResult = pchPos;
		while (*pchPos != 0)
		{
			if (isspace(*pchPos))
			{
				*pchPos++ = 0;
				break;
			}

			++pchPos;
		}
	}

	// Move past any other white-space characters.
	while (*pchPos != 0)
	{
		if (!isspace(*pchPos))
		{
			break;
		}

		++pchPos;
	}

	return pszResult;
}

HRESULT CCommandProcessor::OnStartTls(LPSTR pszParameters, CComPtr<IReplResult>& rpResult) noexcept
{
	UNREFERENCED_PARAMETER(pszParameters);
	UNREFERENCED_PARAMETER(rpResult);

	// TLS is currently not supported.
	return NUT_E_NOTSUPPORTED;
}

HRESULT CCommandProcessor::OnUserName(LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept
{
	UNREFERENCED_PARAMETER(rpResult);

	_ATLTRY
	{
		LPCSTR pszUserName = GetPart(pszParameters);
		if (pszUserName == nullptr || strlen(pszUserName) == 0)
		{
			return NUT_E_INVALIDARG;
		}

		if (!m_strUserName.IsEmpty())
		{
			return NUT_E_USERNAME_SET;
		}

		m_strUserName = pszUserName;
		return S_OK;
	}
	_ATLCATCH(ex)
	{
		return ex.m_hr;
	}
	_ATLCATCHALL()
	{
		return E_FAIL;
	}
}

HRESULT CCommandProcessor::OnPassWord(LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept
{
	UNREFERENCED_PARAMETER(rpResult);

	_ATLTRY
	{
		LPCSTR pszPassWord = GetPart(pszParameters);
		if (pszPassWord == nullptr && strlen(pszPassWord) == 0)
		{
			return NUT_E_INVALIDARG;
		}

		if (!m_strPassWord.IsEmpty())
		{
			return NUT_E_PASSWORD_SET;
		}

		m_strPassWord = pszPassWord;
		return S_OK;
	}
	_ATLCATCH(ex)
	{
		return ex.m_hr;
	}
	_ATLCATCHALL()
	{
		return E_FAIL;
	}
}

HRESULT CCommandProcessor::OnGet(LPSTR pszParameters, CComPtr<IReplResult>& rpResult) noexcept
{
	HRESULT hr = S_OK;

	LPCSTR pszCommand = GetPart(pszParameters);
	if (pszCommand != nullptr)
	{
		auto pos = m_rgGetHandlers.Lookup(pszCommand);
		if (pos != nullptr)
		{
			auto pfnHandler = m_rgGetHandlers.GetValueAt(pos);
			hr = (this->*pfnHandler)(pszParameters, rpResult);
		}
		else
		{
			hr = NUT_E_INVALIDARG;
		}
	}
	else
	{
		hr = NUT_E_INVALIDARG;
	}

	return hr;
}

HRESULT CCommandProcessor::OnList(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept
{
	HRESULT hr = S_OK;

	LPCSTR pszCommand = GetPart(pszParameters);
	if (pszCommand != nullptr)
	{
		auto pos = m_rgListHandlers.Lookup(pszCommand);
		if (pos != nullptr)
		{
			auto pfnHandler = m_rgListHandlers.GetValueAt(pos);
			hr = (this->*pfnHandler)(pszParameters, rpResult);
		}
		else
		{
			hr = NUT_E_INVALIDARG;
		}
	}
	else
	{
		hr = NUT_E_INVALIDARG;
	}

	return hr;
}

HRESULT CCommandProcessor::OnLogin(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept
{
	UNREFERENCED_PARAMETER(pszParameters);
	UNREFERENCED_PARAMETER(rpResult);

	return S_OK;
}

HRESULT CCommandProcessor::OnLogout(LPSTR pszParameters, CComPtr<IReplResult>& rpResult) noexcept
{
	UNREFERENCED_PARAMETER(pszParameters);

	rpResult = _ATL_NEW CSimpleResult("OK Goodbye");
	if (rpResult == nullptr)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}

HRESULT CCommandProcessor::OnGetVar(LPSTR pszParameters, CComPtr<IReplResult>& rpResult) noexcept
{
	HRESULT hr = S_OK;
	LPCSTR pszUps = GetPart(pszParameters);
	if (pszUps != nullptr)
	{
		LPCSTR pszName = GetPart(pszParameters);
		if (pszName != nullptr)
		{
			POSITION pos = m_pBatteries->FindBattery(pszUps);
			if (pos != NULL)
			{
				auto &battery = m_pBatteries->GetAt(pos);
				hr = battery.GetVariable(pszName, rpResult);
			}
			else
			{
				hr = NUT_E_UNKNOWN_UPS;
			}
		}
		else
		{
			hr = NUT_E_INVALIDARG;
		}
	}
	else
	{
		hr = NUT_E_INVALIDARG;
	}

	return hr;
}

HRESULT CCommandProcessor::OnListUps(_In_z_ LPSTR pszParameters, CComPtr<IReplResult> &rpResult) noexcept
{
	UNREFERENCED_PARAMETER(pszParameters);

	rpResult = m_pBatteries;
	return S_OK;
}

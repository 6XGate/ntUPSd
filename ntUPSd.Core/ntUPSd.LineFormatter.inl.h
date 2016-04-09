#pragma once
#include "ntUPSd.LineFormatter.h"

namespace CTL
{
	template<typename ...Args>
	inline HRESULT Format::Text(::ATL::CStringA &rstrResult, LPCSTR pszFormat, Args && ...args) noexcept
	{
		return TextImpl(rstrResult, pszFormat, std::forward<Args>(args)...);
	}

	template<typename Arg0, typename ...Args>
	inline HRESULT Format::TextImpl(::ATL::CStringA &rstrResult, LPCSTR &rpszFormat, Arg0 &&arg, Args && ...args) noexcept
	{
		// Handle the next tag.
		HRESULT hr = TextParse(rstrResult, rpszFormat, arg);
		if (FAILED(hr))
		{
			return hr;
		}

		// Handle the remaining tags.
		return TextImpl(rstrResult, rpszFormat, std::forward<Args>(args)...);
	}

	template<typename Arg0>
	inline HRESULT Format::TextImpl(::ATL::CStringA &rstrResult, LPCSTR &rpszFormat, Arg0 &&arg) noexcept
	{
		// Handle the final tag.
		HRESULT hr = TextParse(rstrResult, rpszFormat, arg);
		if (FAILED(hr))
		{
			return hr;
		}

		// Handle the remaining string.
		return TextImpl(rstrResult, rpszFormat);
	}

	template<typename Arg>
	inline HRESULT Format::TextParse(::ATL::CStringA &rstrResult, LPCSTR &rpszFormat, Arg &&arg) noexcept
	{
		_ATLTRY
		{
			// Find the next tag.
			HRESULT hr = E_UNEXPECTED;
			LPCCH pchPos = strchr(rpszFormat, '%');
			if (pchPos != nullptr)
			{
				// Found a tag...
				switch (*++pchPos)
				{
				case '%':
					// Escaping a percent (_%_), take the current format line and pass it to a recall.
					rstrResult.Append(rpszFormat, pchPos - rpszFormat);
					rpszFormat = 1 + pchPos;
					hr = TextParse(rstrResult, rpszFormat, std::forward<Arg>(arg));
					break;

				case '$':
					// Append the current format string and convert the value as well.
					rstrResult.Append(rpszFormat, --pchPos - rpszFormat);
					rpszFormat = 2 + pchPos;
					hr = ToString(rstrResult, std::forward<Arg>(arg));
					break;
				}
			}

			return hr;
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
}

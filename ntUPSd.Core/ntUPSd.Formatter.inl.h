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
#include "ntUPSd.Formatter.h"

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
					rstrResult.Append(rpszFormat, static_cast<int>(pchPos - rpszFormat));
					rpszFormat = 1 + pchPos;
					hr = TextParse(rstrResult, rpszFormat, std::forward<Arg>(arg));
					break;

				case '$':
					// Append the current format string and convert the value as well.
					rstrResult.Append(rpszFormat, static_cast<int>(--pchPos - rpszFormat));
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

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
#include "ntUPSd.Formatter.h"

namespace CTL
{
	HRESULT Format::TextImpl(::ATL::CStringA &rstrResult, LPCSTR &rpszFormat) noexcept
	{
		_ATLTRY
		{
			// Find any remaining tags.
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
					return TextImpl(rstrResult, rpszFormat);

				default:
					// No more data arguments are present, so anything else is unexpected.
					return E_UNEXPECTED;
				}
			}

			rstrResult.Append(rpszFormat);
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

	LPCSTR Format::FindOneOf(LPCSTR pszSubject, LPCSTR pszCharacters) noexcept
	{
		for (LPCCH pchPos = pszSubject; *pchPos != 0; ++pchPos)
		{
			for (LPCCH pchChr = pszCharacters; *pchChr != 0; ++pchChr)
			{
				if (*pchChr == *pchPos)
				{
					return pchPos;
				}
			}
		}

		return nullptr;
	}

	HRESULT Format::ToString(::ATL::CStringA &rstrResult, LPCSTR pszValue) noexcept
	{
		return ToString(rstrResult, pszValue, static_cast<int>(strlen(pszValue)));
	}

	HRESULT Format::ToString(::ATL::CStringA &rstrResult, LPCSTR pszValue, int cchValue) noexcept
	{
		_ATLTRY
		{
			if (FindOneOf(pszValue, "\r\n\t \""))
			{
				// The string must be quoted and escaped.
				rstrResult.AppendChar('"');
				for (int i = 0; i != cchValue; ++i)
				{
					CHAR ch = pszValue[i];
					switch (ch)
					{
					case '"':
					case '\\':
						rstrResult.AppendChar('\\');
						rstrResult.AppendChar(ch);
						break;
					default:
						rstrResult.AppendChar(ch);
						break;
					}
				}

				rstrResult.AppendChar('"');
			}
			else
			{
				rstrResult.Append(pszValue, cchValue);
			}

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

	HRESULT Format::ToString(::ATL::CStringA &rstrResult, const::ATL::CStringA &strValue) noexcept
	{
		return ToString(rstrResult, strValue, strValue.GetLength());
	}
}

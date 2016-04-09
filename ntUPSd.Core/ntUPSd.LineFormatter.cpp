#include "stdafx.h"
#include "ntUPSd.LineFormatter.h"

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
					rstrResult.Append(rpszFormat, pchPos - rpszFormat);
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
		return ToString(rstrResult, pszValue, strlen(pszValue));
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

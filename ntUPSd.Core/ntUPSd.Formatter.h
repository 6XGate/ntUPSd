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

namespace CTL
{
	class Format final
	{
	public:
		template <typename... Args>
		static HRESULT Text(::ATL::CStringA &rstrResult, LPCSTR pszFormat, Args&& ...args) noexcept;

	protected:
		Format() = delete;
		Format(Format &&) = delete;
		Format(const Format &) = delete;
		Format &operator =(Format &&) = delete;
		Format &operator =(const Format &) = delete;
		~Format() = delete;

	private:
		template <typename Arg0, typename... Args>
		static HRESULT TextImpl(::ATL::CStringA &rstrResult, LPCSTR &rpszFormat, Arg0 &&arg, Args&& ...args) noexcept;

		template <typename Arg0>
		static HRESULT TextImpl(::ATL::CStringA &rstrResult, LPCSTR &rpszFormat, Arg0 &&arg) noexcept;

		static HRESULT TextImpl(::ATL::CStringA &rstrResult, LPCSTR &rpszFormat) noexcept;

		template <typename Arg>
		static HRESULT TextParse(::ATL::CStringA &rstrResult, LPCSTR &rpszFormat, Arg &&arg) noexcept;

		static LPCSTR FindOneOf(_In_z_ LPCSTR pszSubject, _In_z_ LPCSTR pszCharacters) noexcept;

		static HRESULT ToString(::ATL::CStringA &rstrResult, char nValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, unsigned char nValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, short nValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, unsigned short nValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, int nValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, unsigned int nValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, long long nValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, unsigned long long nValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, float nValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, double nValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, long double nValue) noexcept;

		static HRESULT ToString(::ATL::CStringA &rstrResult, _In_z_ LPCSTR pszValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, _In_count_(cchValue) LPCSTR pszValue, int cchValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, const ::ATL::CStringA &strValue) noexcept;
	};
}

#include "ntUPSd.Formatter.inl.h"

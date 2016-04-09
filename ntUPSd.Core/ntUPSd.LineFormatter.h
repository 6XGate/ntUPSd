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

		static HRESULT ToString(::ATL::CStringA &rstrResult, LPCSTR pszValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, LPCSTR pszValue, int cchValue) noexcept;
		static HRESULT ToString(::ATL::CStringA &rstrResult, const ::ATL::CStringA &strValue) noexcept;
	};
}

#include "ntUPSd.LineFormatter.inl.h"

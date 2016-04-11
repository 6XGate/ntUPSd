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
	class CReferenceCounted
	{
	public:
		CReferenceCounted(CReferenceCounted &&) = delete;
		CReferenceCounted(const CReferenceCounted &) = delete;
		CReferenceCounted &operator =(CReferenceCounted &&) = delete;
		CReferenceCounted &operator =(const CReferenceCounted &) = delete;
		virtual ~CReferenceCounted() noexcept = default;

		ULONG AddRef() noexcept;
		ULONG Release() noexcept;

	protected:
		CReferenceCounted() noexcept = default;

	private:
		ULONG m_cRefs = 0;
	};

	class CAbstractWorkClient : public ::ATL::IWorkerThreadClient
	{
	public:
		CAbstractWorkClient(CAbstractWorkClient &&) = delete;
		CAbstractWorkClient(const CAbstractWorkClient &) = delete;
		CAbstractWorkClient &operator =(CAbstractWorkClient &&) = delete;
		CAbstractWorkClient &operator =(const CAbstractWorkClient &) = delete;
		virtual ~CAbstractWorkClient() noexcept = default;

		virtual HRESULT CloseHandle(HANDLE hHandle) noexcept override;

	protected:
		CAbstractWorkClient() noexcept = default;
	};

	template <typename Target>
	class CAbstractProxy : public CAbstractWorkClient
	{
	public:
		static_assert(__is_base_of(CReferenceCounted, Target), "The Target must implement CReferenceCounted");

		CAbstractProxy(Target *pTarget) noexcept;
		CAbstractProxy(CAbstractProxy &&) = delete;
		CAbstractProxy(const CAbstractProxy &) = delete;
		CAbstractProxy &operator =(CAbstractProxy &&) = delete;
		CAbstractProxy &operator =(const CAbstractProxy &) = delete;
		virtual ~CAbstractProxy() noexcept;

	protected:
		Target *const m_pTarget;
	};
}

#include "ntUPSd.WorkerUtilities.inl.h"

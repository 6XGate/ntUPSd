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

#pragma once
#include "ntUPSd.Core.Base.h"

namespace CTL
{
	template <typename Derived, UINT idServiceName>
	class CServerModuleT : public ::ATL::CAtlServiceModuleT<Derived, idServiceName>
	{
	public:
		INT WinMain(_In_ INT nShowCmd) noexcept;
		HRESULT Start(_In_ int nShowCmd) noexcept;
		HRESULT RegisterClassObjects(...) noexcept;

	private:
		static void TranslateStructuredException(UINT nCode, EXCEPTION_POINTERS *pEx);
	};
}

#include "ntUPSd.ServerModule.inl.h"

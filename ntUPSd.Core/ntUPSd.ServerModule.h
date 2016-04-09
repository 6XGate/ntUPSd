#pragma once

namespace CTL
{
	template <typename Derived, UINT idServiceName>
	class CServerModuleT : public ::ATL::CAtlServiceModuleT<Derived, idServiceName>
	{
	public:
		HRESULT Start(_In_ int nShowCmd) noexcept;
		HRESULT RegisterClassObjects(...) noexcept { return S_OK; }
	};
}

#include "ntUPSd.ServerModule.inl.h"

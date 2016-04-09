#pragma once
#include "ntUPSd.WorkerUtilities.h"

namespace CTL
{
	template <typename Target>
	CAbstractProxy<Target>::CAbstractProxy(Target *pTarget) noexcept :
		m_pTarget(pTarget)
	{
		pTarget->AddRef();
	}

	template <typename Target>
	CAbstractProxy<Target>::~CAbstractProxy() noexcept
	{
		m_pTarget->Release();
	}
}

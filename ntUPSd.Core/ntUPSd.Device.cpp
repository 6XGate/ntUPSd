#include "stdafx.h"
#include "ntUPSd.Device.h"

namespace CTL
{
	CDevice::CDevice(_In_opt_ HANDLE hDevice) noexcept :
		#pragma warning(suppress: 6387) // CHandle and CAtlFile won't be hurt.
		::ATL::CAtlFile(hDevice != INVALID_HANDLE_VALUE ? hDevice : NULL)
	{
	}

	CDevice::CDevice(CDevice &&other) noexcept :
		::ATL::CAtlFile(other.Detach())
	{
	}

	CDevice &CDevice::operator=(CDevice &&other) noexcept
	{
		Close();
		Attach(other.Detach());
		return *this;
	}

	HRESULT CDevice::DeviceIoControl(
		_In_ DWORD dwIoControlCode,
		_In_reads_bytes_opt_(cbInBuffer) const void *pvInBuffer /*= nullptr*/,
		_In_ DWORD cbInBuffer /*= 0*/,
		_Out_writes_bytes_to_opt_(cbOutBuffer, *pcbReturned) void *pvOutBuffer /*= nullptr*/,
		_In_ DWORD cbOutBuffer /*= 0*/,
		_Out_opt_ DWORD *pcbReturned /*= nullptr*/,
		_Inout_opt_ OVERLAPPED *pOverlapped /*= nullptr*/) noexcept
	{
		BOOL fSuccess = ::DeviceIoControl(
			m_h, dwIoControlCode,
			const_cast<void*>(pvInBuffer), cbInBuffer,
			pvOutBuffer, cbOutBuffer, pcbReturned,
			pOverlapped);
		if (!fSuccess)
		{
			return ::ATL::AtlHresultFromLastError();
		}

		return S_OK;
	}
}

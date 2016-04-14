#pragma once
#include "ntUPSd.Core.Base.h"

namespace CTL
{
	class CDevice : public ::ATL::CAtlFile
	{
	public:
		CDevice() noexcept = default;
		explicit CDevice(_In_opt_ HANDLE hFile) noexcept;
		CDevice(CDevice &&other) noexcept;
		CDevice(const CDevice &) = delete;
		CDevice &operator =(CDevice &&other) noexcept;
		CDevice &operator =(const CDevice &) = delete;
		~CDevice() noexcept = default;

		HRESULT DeviceIoControl(
			_In_ DWORD dwIoControlCode,
			_In_reads_bytes_opt_(cbInBuffer) const void *pvInBuffer = nullptr,
			_In_ DWORD cbInBuffer = 0,
			_Out_writes_bytes_to_opt_(cbOutBuffer, *pcbReturned) void *pvOutBuffer = nullptr,
			_In_ DWORD cbOutBuffer = 0,
			_Out_opt_ DWORD *pcbReturned = nullptr,
			_Inout_opt_ OVERLAPPED *pOverlapped = nullptr) noexcept;
	};
}

#include "ntUPSd.Device.inl.h"

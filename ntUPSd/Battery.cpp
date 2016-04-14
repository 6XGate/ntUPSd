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
#include "stdafx.h"
#include "Battery.h"

CBatteryVariable::CBatteryVariable(_In_z_ LPCSTR pszUps, _In_z_ LPCSTR pszName) noexcept :
	m_pszUps(pszUps), m_pszName(pszName)
{
}

CBattery::~CBattery()
{
}

const CStringA & CBattery::GetKeyName() const noexcept
{
	return m_strKeyName;
}

HRESULT CBattery::Open(_In_z_ LPCWSTR pszDevicePath) noexcept
{
	_ATLTRY
	{
		CDevice hBattery;
		HRESULT hr = hBattery.Create(
			pszDevicePath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL);
		if (FAILED(hr))
		{
			return hr;
		}

		DWORD cbOut;

		BATTERY_QUERY_INFORMATION bqi = { 0 };
		DWORD dwWait = 0;

		// First we need the battery tag.
		hr = hBattery.DeviceIoControl(IOCTL_BATTERY_QUERY_TAG, &dwWait, sizeof(dwWait), &bqi.BatteryTag, sizeof(bqi.BatteryTag), &cbOut);
		if (FAILED(hr))
		{
			return hr;
		}

		// Get the basic information.
		bqi.InformationLevel = BatteryInformation;
		hr = hBattery.DeviceIoControl(IOCTL_BATTERY_QUERY_INFORMATION, &bqi, sizeof(bqi), &m_BatteryInfo, sizeof(m_BatteryInfo), &cbOut);
		if (FAILED(hr))
		{
			return hr;
		}

		// Make sure the battery is a UPS.
		if (!(m_BatteryInfo.Capabilities & BATTERY_IS_SHORT_TERM))
		{
			return NTUPSD_E_NOT_UPS;
		}

		if (m_BatteryInfo.DefaultAlert1 >= m_BatteryInfo.FullChargedCapacity)
		{
			// Normalize the low default alert if it makes no sense.
			m_BatteryInfo.DefaultAlert1 = m_BatteryInfo.FullChargedCapacity * 20 / 100;
		}

		if (m_BatteryInfo.DefaultAlert2 >= m_BatteryInfo.FullChargedCapacity)
		{
			// Normalize the warning default alert if it makes no sense.
			m_BatteryInfo.DefaultAlert2 = m_BatteryInfo.FullChargedCapacity * 50 / 100;
		}

		// Now that we have a tag, next get some static information.
		CStringA strDeviceName, strManufacturerName, strSerialNumber, strManufatureDate, strType;

		// The battery name.
		hr = GetStringInfo(hBattery, bqi.BatteryTag, BatteryDeviceName, strDeviceName);
		if (FAILED(hr))
		{
			return hr;
		}

		// The manufacturer.
		hr = GetStringInfo(hBattery, bqi.BatteryTag, BatteryManufactureName, strManufacturerName);
		if (FAILED(hr))
		{
			return hr;
		}

		// The serial number.
		hr = GetStringInfo(hBattery, bqi.BatteryTag, BatterySerialNumber, strSerialNumber);
		if (FAILED(hr))
		{
			return hr;
		}

		// The manufagure date.
		BATTERY_MANUFACTURE_DATE bmd = { 0 };
		bqi.InformationLevel = BatteryManufactureDate;
		if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &bqi, sizeof(bqi), &bmd, sizeof(bmd), &cbOut, nullptr))
		{
			strManufatureDate.Format("%04hu/%02hu/%02hu", bmd.Year, static_cast<USHORT>(bmd.Month), static_cast<USHORT>(bmd.Day));
		}

		// The battery type.
		size_t i = 0;
		SecureZeroMemory(m_szType, sizeof(m_szType));
		for (char ch : m_BatteryInfo.Chemistry)
		{
			if (ch == 0)
			{
				break;
			}

			m_szType[i++] = ch;
		}

		// Calculate some static values.
		CStringA strLowBatteryPercentage, strWarningBatteryPercentage;
		strLowBatteryPercentage.Format("%lu", m_BatteryInfo.DefaultAlert1 * 100 / m_BatteryInfo.DesignedCapacity);
		strWarningBatteryPercentage.Format("%lu", m_BatteryInfo.DefaultAlert2 * 100 / m_BatteryInfo.DesignedCapacity);

		m_rgVariables.SetAt("device.model", _AtlNew<CBatteryStaticVariable>("usbhid", "device.model", strDeviceName));
		m_rgVariables.SetAt("device.mfr", _AtlNew<CBatteryStaticVariable>("usbhid", "device.mfr", strManufacturerName));
		m_rgVariables.SetAt("device.serial", _AtlNew<CBatteryStaticVariable>("usbhid", "device.serial", strSerialNumber));
		m_rgVariables.SetAt("device.type", _AtlNew<CBatteryStaticVariable>("usbhid", "device.type", "ups"));

		m_rgVariables.SetAt("ups.status", _AtlNew<CBatteryDynamicVariable>(*this, "usbhid", "ups.status", &CBattery::GetUpsStatus));
		m_rgVariables.SetAt("ups.model", _AtlNew<CBatteryStaticVariable>("usbhid", "ups.model", strDeviceName));
		m_rgVariables.SetAt("ups.mfr", _AtlNew<CBatteryStaticVariable>("usbhid", "ups.mfr", strManufacturerName));
		m_rgVariables.SetAt("ups.serial", _AtlNew<CBatteryStaticVariable>("usbhid", "ups.serial", strSerialNumber));
		m_rgVariables.SetAt("ups.temperature", _AtlNew<CBatteryDynamicVariable>(*this, "usbhid", "ups.temperature", &CBattery::GetBatteryTemperature));

		m_rgVariables.SetAt("driver.name", _AtlNew<CBatteryStaticVariable>("usbhid", "driver.name", "usbhid-ups"));
		m_rgVariables.SetAt("driver.version", _AtlNew<CBatteryStaticVariable>("usbhid", "driver.version", DAEMON_VERSION));
		m_rgVariables.SetAt("driver.version.internal", _AtlNew<CBatteryStaticVariable>("usbhid", "driver.version.internal", DAEMON_VERSION));

		m_rgVariables.SetAt("battery.charge", _AtlNew<CBatteryDynamicVariable>(*this, "usbhid", "battery.charge", &CBattery::GetBatteryCharge));
		m_rgVariables.SetAt("battery.charge.low", _AtlNew<CBatteryStaticVariable>("usbhid", "battery.charge.low", strLowBatteryPercentage));
		m_rgVariables.SetAt("battery.charge.warning", _AtlNew<CBatteryStaticVariable>("usbhid", "battery.charge.warning", strWarningBatteryPercentage));
		m_rgVariables.SetAt("battery.charger.status", _AtlNew<CBatteryDynamicVariable>(*this, "usbhid", "battery.charger.status", &CBattery::GetBatteryChargerStatus));
		m_rgVariables.SetAt("battery.temperature", _AtlNew<CBatteryDynamicVariable>(*this, "usbhid", "battery.temperature", &CBattery::GetBatteryTemperature));
		m_rgVariables.SetAt("battery.type", _AtlNew<CBatteryStaticVariable>("usbhid", "battery.type", m_szType));
		m_rgVariables.SetAt("battery.voltage", _AtlNew<CBatteryDynamicVariable>(*this, "usbhid", "battery.voltage", &CBattery::GetBatteryVoltage));

		// Optional; the manufacture date.
		if (!strManufatureDate.IsEmpty())
		{
			m_rgVariables.SetAt("ups.mfr.date", _AtlNew<CBatteryStaticVariable>("usbhid", "ups.mfr.date", strManufatureDate));
			m_rgVariables.SetAt("battery.mfr.date", _AtlNew<CBatteryStaticVariable>("usbhid", "battery.mfr.date", strManufatureDate));
		}

		// Server information;
		m_rgVariables.SetAt("server.info", _AtlNew<CBatteryStaticVariable>("usbhid", "server.info", SERVER_INFO));
		m_rgVariables.SetAt("server.version", _AtlNew<CBatteryStaticVariable>("usbhid", "server.version", DAEMON_VERSION));

		m_nBatteryTag = bqi.BatteryTag;
		m_strKeyName = "usbhid"; // Just going to use this key name for now.
		m_strDeviceName = strDeviceName;
		m_strManufacturerName = strManufacturerName;
		m_strManufacturerDate = strManufatureDate;
		m_strSerialNumber = strSerialNumber;
		m_hBattery.Attach(hBattery.Detach());
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

HRESULT CBattery::GetVariable(_In_z_ LPCSTR pszName, CComPtr<IReplResult>& rpResult) noexcept
{
	POSITION pos = m_rgVariables.Lookup(pszName);
	if (pos != nullptr)
	{
		rpResult = m_rgVariables.GetValueAt(pos);
		return S_OK;
	}

	return NUT_E_VARNOTSUPPORTED;
}

HRESULT CBattery::RenderListUpsEntry(CStringA &strOutput) noexcept
{
	_ATLTRY
	{
		return Format::Text(strOutput, "UPS %$ %$\r\n", m_strKeyName, m_strDeviceName);
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

HRESULT CBattery::ToUtf8(_In_z_ LPCWSTR pszValue, CStringA &rstrValue) noexcept
{
	_ATLTRY
	{
		DWORD cchValue = static_cast<DWORD>(wcslen(pszValue)), cchRequired = 0;
		if (cchValue == 0)
		{
			rstrValue.Empty();
			return S_OK;
		}

		cchRequired = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, pszValue, ++cchValue, nullptr, 0, nullptr, nullptr);
		if (cchRequired == 0)
		{
			return AtlHresultFromLastError();
		}

		LPSTR pszResult = rstrValue.GetBufferSetLength(++cchRequired);
		cchRequired = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, pszValue, cchValue, pszResult, cchRequired, nullptr, nullptr);
		if (cchRequired == 0)
		{
			return AtlHresultFromLastError();
		}

		rstrValue.ReleaseBuffer();
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

HRESULT CBattery::GetStringInfo(CDevice &hBattery, ULONG nBatteryTag, BATTERY_QUERY_INFORMATION_LEVEL eInfoLevel, CStringA &rstrValue) noexcept
{
	DWORD cchBuffer = 128;
	CAtlArray<WCHAR> pszBuffer;
	BATTERY_QUERY_INFORMATION bqi = { nBatteryTag, eInfoLevel };
	for (;;)
	{
		pszBuffer.SetCount(cchBuffer);
		DWORD cbBuffer = cchBuffer * sizeof(WCHAR), cbReturned;
		HRESULT hr = hBattery.DeviceIoControl(IOCTL_BATTERY_QUERY_INFORMATION, &bqi, sizeof(bqi), pszBuffer.GetData(), cbBuffer, &cbReturned);
		if (FAILED(hr))
		{
			if (hr != E_NOT_SUFFICIENT_BUFFER)
			{
				return hr;
			}

			cchBuffer += 128;
			continue;
		}

		break;
	}

	return ToUtf8(pszBuffer.GetData(), rstrValue);
}

HRESULT CBattery::GetStringInfo(BATTERY_QUERY_INFORMATION_LEVEL eInfoLevel, CStringA &rstrValue) noexcept
{
	return GetStringInfo(m_hBattery, m_nBatteryTag, eInfoLevel, rstrValue);
}

template <typename InStruct, typename OutStruct>
HRESULT CBattery::GetStructData(DWORD dwIoCtlCode, _In_ const InStruct *pInData, _Out_ OutStruct *pOutData) noexcept
{
	DWORD cbInStruct = static_cast<DWORD>(sizeof(InStruct));
	DWORD cbOutStruct = static_cast<DWORD>(sizeof(OutStruct));
	HRESULT hr = m_hBattery.DeviceIoControl(dwIoCtlCode, pInData, cbInStruct, pOutData, cbOutStruct);
	if (FAILED(hr))
	{
		if (hr == __HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION))
		{
			hr = NUT_E_VARNOTSUPPORTED;
		}
	}

	return hr;
}

template<typename OutStruct>
HRESULT CBattery::GetInfoData(BATTERY_QUERY_INFORMATION_LEVEL eInfoLevel, OutStruct * pOutData) noexcept
{
	BATTERY_QUERY_INFORMATION bqi = { m_nBatteryTag, eInfoLevel };
	return GetStructData(IOCTL_BATTERY_QUERY_INFORMATION, &bqi, pOutData);
}

HRESULT CBattery::GetStatusData(BATTERY_STATUS * pbs)
{
	BATTERY_WAIT_STATUS bws = { m_nBatteryTag, 0, 0xF, ULONG_MAX, ULONG_MAX };
	return GetStructData(IOCTL_BATTERY_QUERY_STATUS, &bws, pbs);
}

HRESULT CBattery::GetUpsStatus(CStringA &rstrValue) noexcept
{
	_ATLTRY
	{
		BATTERY_STATUS bs;
		HRESULT hr = GetStatusData(&bs);
		if (FAILED(hr))
		{
			return hr;
		}

		if (bs.Capacity > m_BatteryInfo.FullChargedCapacity)
		{
			bs.Capacity = m_BatteryInfo.FullChargedCapacity;
		}

		if (bs.PowerState & BATTERY_POWER_ON_LINE)
		{
			rstrValue.Append("OL ");
		}

		if (bs.PowerState & BATTERY_DISCHARGING)
		{
			rstrValue.Append("OB ");
		}

		if (bs.Capacity <= m_BatteryInfo.DefaultAlert1)
		{
			rstrValue.Append("LB ");
		}

		if (bs.PowerState & BATTERY_CRITICAL)
		{
			rstrValue.Append("FSD ");
		}

		rstrValue.Trim();
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

HRESULT CBattery::GetBatteryCharge(CStringA &rstrValue) noexcept
{
	_ATLTRY
	{
		BATTERY_STATUS bs;
		HRESULT hr = GetStatusData(&bs);
		if (FAILED(hr))
		{
			return hr;
		}

		if (bs.Capacity > m_BatteryInfo.FullChargedCapacity)
		{
			bs.Capacity = m_BatteryInfo.FullChargedCapacity;
		}

		ULONGLONG nRemainingCapacity = bs.Capacity * 100 / m_BatteryInfo.FullChargedCapacity;
		rstrValue.AppendFormat("%I64u", nRemainingCapacity);
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

HRESULT CBattery::GetBatteryChargerStatus(CStringA &rstrValue) noexcept
{
	_ATLTRY
	{
		BATTERY_STATUS bs;
		HRESULT hr = GetStatusData(&bs);
		if (FAILED(hr))
		{
			return hr;
		}

		if (bs.PowerState & BATTERY_CHARGING)
		{
			rstrValue = "charging";
		}
		else if (bs.PowerState & BATTERY_DISCHARGING)
		{
			rstrValue = "discharging";
		}
		else
		{
			rstrValue = "resting";
		}

		rstrValue.Trim();
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

HRESULT CBattery::GetBatteryTemperature(CStringA & rstrValue) noexcept
{
	_ATLTRY
	{
		ULONG nTemperature;
		HRESULT hr = GetInfoData(BatteryTemperature, &nTemperature);
		if (FAILED(hr))
		{
			return hr;
		}

		// C = K - 273.15; // C = nTemperature - 2732 / 10;
		float dTemperature = static_cast<float>(static_cast<int>(nTemperature) - 2732) / 10.0f;
		rstrValue.AppendFormat("%f", dTemperature);
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

HRESULT CBattery::GetBatteryVoltage(CStringA & rstrValue) noexcept
{
	_ATLTRY
	{
		BATTERY_STATUS bs;
		HRESULT hr = GetStatusData(&bs);
		if (FAILED(hr))
		{
			return hr;
		}

		if (bs.Voltage == BATTERY_UNKNOWN_VOLTAGE)
		{
			return NUT_E_VARNOTSUPPORTED;
		}

		double dVoltage = static_cast<double>(bs.Voltage) / 1000.0f;

		rstrValue.AppendFormat("%f", dVoltage);
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

CBatteryStaticVariable::CBatteryStaticVariable(_In_z_ LPCSTR pszUps, _In_z_ LPCSTR pszName, _In_z_ LPCSTR pszValue) noexcept :
	CBatteryVariable(pszUps, pszName), m_pszValue(pszValue)
{
}

bool CBatteryStaticVariable::IsReadOnly() const noexcept
{
	return true;
}

STDMETHODIMP CBatteryStaticVariable::RenderResult(CStringA &rstrResult) noexcept
{
	_ATLTRY
	{
		return Format::Text(rstrResult, "VAR %$ %$ %$\r\n", m_pszUps, m_pszName, m_pszValue);
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

CBatteryDynamicVariable::CBatteryDynamicVariable(CBattery &battery, _In_z_ LPCSTR pszUps, _In_z_ LPCSTR pszName, _In_ PFNVARGETTER pfnGetter, _In_opt_ PFNVARSETTER pfnSetter) noexcept :
	CBatteryVariable(pszUps, pszName), m_Battery(battery), m_pfnGetter(pfnGetter), m_pfnSetter(pfnSetter)
{
}

bool CBatteryDynamicVariable::IsReadOnly() const noexcept
{
	return m_pfnSetter != nullptr;
}

STDMETHODIMP CBatteryDynamicVariable::RenderResult(CStringA &rstrResult) noexcept
{
	_ATLTRY
	{
		CStringA strValue;
		HRESULT hr = (m_Battery.*m_pfnGetter)(strValue);
		if (FAILED(hr))
		{
			return hr;
		}

		return Format::Text(rstrResult, "VAR %$ %$ %$\r\n", m_pszUps, m_pszName, strValue);
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

HRESULT CBatteryCollection::LoadBatteries() noexcept
{
	HDEVINFO hDev = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, nullptr, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDev != INVALID_HANDLE_VALUE)
	{
		DWORD iDev = 0;
		SP_DEVICE_INTERFACE_DATA did = { sizeof(SP_DEVICE_INTERFACE_DATA) };
		for (;;)
		{
			if (SetupDiEnumDeviceInterfaces(hDev, nullptr, &GUID_DEVCLASS_BATTERY, iDev, &did))
			{
				DWORD cbRequired = 0;
				SetupDiGetDeviceInterfaceDetail(hDev, &did, nullptr, 0, &cbRequired, nullptr);
				if (cbRequired)
				{
					CHeapPtr<SP_DEVICE_INTERFACE_DETAIL_DATA> pdidd;
					if (pdidd.AllocateBytes(cbRequired))
					{
						pdidd->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
						if (SetupDiGetDeviceInterfaceDetail(hDev, &did, pdidd, cbRequired, &cbRequired, nullptr))
						{
							_ATLTRY
							{
								POSITION posNewItem = AddTail();
								CBattery &battery = GetAt(posNewItem);
								HRESULT hrLoad = battery.Open(pdidd->DevicePath);
								if (SUCCEEDED(hrLoad))
								{
									// We are only going to support the first battery for right now.
									// TODO: We need to make it possible to configure the names for these in a UI.
									break;
								}
								else
								{
									// Removed the failed battery.
									RemoveTailNoReturn();
								}
							}
							_ATLCATCHALL()
							{
								// Skipping battery...
							}
						}
					}
				}
			}
			else
			{
				// No more batteries...
				break;
			}

			++iDev;
		}

		SetupDiDestroyDeviceInfoList(hDev);
	}
	else
	{
		return HRESULT_FROM_SETUPAPI(GetLastError());
	}

	return IsEmpty() ? NTUPSD_E_NO_UPS : S_OK;
}

POSITION CBatteryCollection::FindBattery(_In_z_ LPCSTR pszName) const noexcept
{
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		auto &battery = GetAt(pos);
		if (battery.GetKeyName() == pszName)
		{
			return pos;
		}

		GetNext(pos);
	}

	return nullptr;
}

STDMETHODIMP CBatteryCollection::RenderResult(CStringA &rstrResult) noexcept
{
	_ATLTRY
	{
		rstrResult.Append("BEGIN LIST UPS\r\n");

		POSITION pos = GetHeadPosition();
		while (pos != NULL)
		{
			CBattery &battery = GetNext(pos);
			HRESULT hr = battery.RenderListUpsEntry(rstrResult);
			if (FAILED(hr))
			{
				return hr;
			}
		}

		rstrResult.Append("END LIST UPS\r\n");
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

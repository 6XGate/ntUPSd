#pragma once

class CBatteryVariable : public CReplResult
{
public:
	CBatteryVariable(CBatteryVariable &&) = delete;
	CBatteryVariable(const CBatteryVariable &) = delete;
	CBatteryVariable &operator =(CBatteryVariable &&) = delete;
	CBatteryVariable &operator =(const CBatteryVariable &) = delete;
	~CBatteryVariable() noexcept = default;

	virtual bool IsReadOnly() const noexcept = 0;

	// TODO: GetType(CComPtr<IReplResult> &rpResult);
	// TODO: GetDesc(CComPtr<IReplResult> &rpResult);

protected:
	CBatteryVariable(_In_z_ LPCSTR pszUps, _In_z_ LPCSTR pszName) noexcept;

	const LPCSTR m_pszUps;
	const LPCSTR m_pszName;
};

class CBattery
{
public:
	CBattery() noexcept = default;
	CBattery(CBattery &&) = delete;
	CBattery(const CBattery &) = delete;
	CBattery &operator =(CBattery &&) = delete;
	CBattery &operator =(const CBattery &) = delete;
	~CBattery();

	const CStringA &GetKeyName() const noexcept;

	HRESULT Open(_In_z_ LPCWSTR pszDevicePath) noexcept;

	HRESULT GetVariable(LPCSTR pszName, CComPtr<IReplResult> &rpResult) noexcept;

	HRESULT RenderListUpsEntry(CStringA &strOutput) noexcept;

private:
	CAtlFile m_hBattery;
	ULONG m_nBatteryTag = 0;
	CStringA m_strKeyName;
	CStringA m_strDeviceName;
	CStringA m_strManufacturerName;
	CStringA m_strSerialNumber;
	BATTERY_INFORMATION m_BatteryInfo;

	CAtlMap<CStringA, CComPtr<CBatteryVariable>> m_rgVariables;

	static HRESULT ToUtf8(LPCWSTR pszValue, CStringA &strValue) noexcept;
	static HRESULT GetStringInfo(_In_ HANDLE hBattery, ULONG nBatteryTag, BATTERY_QUERY_INFORMATION_LEVEL eInfoLevel, CStringA &strValue) noexcept;
	HRESULT GetStringInfo(BATTERY_QUERY_INFORMATION_LEVEL eInfoLevel, CStringA &strValue) noexcept;

	HRESULT GetUpsStatus(CStringA &strValue) noexcept;
	HRESULT GetBatteryCharge(CStringA &strValue) noexcept;
};

class CBatteryStaticVariable final : public CBatteryVariable
{
public:
	CBatteryStaticVariable(_In_z_ LPCSTR pszUps, _In_z_ LPCSTR pszName, _In_z_ LPCSTR pszValue) noexcept;
	CBatteryStaticVariable(CBatteryStaticVariable &&) = delete;
	CBatteryStaticVariable(const CBatteryStaticVariable &) = delete;
	CBatteryStaticVariable &operator =(CBatteryStaticVariable &&) = delete;
	CBatteryStaticVariable &operator =(const CBatteryStaticVariable &) = delete;
	~CBatteryStaticVariable() noexcept = default;

	virtual bool IsReadOnly() const noexcept override;

	STDMETHOD(RenderResult)(CStringA &strResult) noexcept;

private:
	const LPCSTR m_pszValue;
};

class CBatteryDynamicVariable final : public CBatteryVariable
{
public:
	typedef HRESULT(CBattery::*PFNVARGETTER)(CStringA &strValue);
	typedef HRESULT(CBattery::*PFNVARSETTER)(LPCSTR pszNewValue);

	CBatteryDynamicVariable(CBattery &battery, _In_z_ LPCSTR pszUps, _In_z_ LPCSTR pszName, _In_ PFNVARGETTER pfnGetter, _In_ PFNVARSETTER pfnSetter = nullptr) noexcept;
	CBatteryDynamicVariable(CBatteryDynamicVariable &&) = delete;
	CBatteryDynamicVariable(const CBatteryDynamicVariable &) = delete;
	CBatteryDynamicVariable &operator =(CBatteryDynamicVariable &&) = delete;
	CBatteryDynamicVariable &operator =(const CBatteryDynamicVariable &) = delete;
	~CBatteryDynamicVariable() noexcept = default;

	virtual bool IsReadOnly() const noexcept override;

	STDMETHOD(RenderResult)(CStringA &strResult) noexcept;

private:
	CBattery &m_Battery;
	const PFNVARGETTER m_pfnGetter;
	const PFNVARSETTER m_pfnSetter;
};

class CBatteryCollection : public CAtlList<CBattery>, public CReplResult
{
public:
	typedef CAtlList<CBattery> MyBase;

	CBatteryCollection() noexcept = default;
	CBatteryCollection(CBatteryCollection &&) = delete;
	CBatteryCollection(const CBatteryCollection &) = delete;
	CBatteryCollection &operator =(CBatteryCollection &&) = delete;
	CBatteryCollection &operator =(const CBatteryCollection &) = delete;
	~CBatteryCollection() noexcept = default;

	HRESULT LoadBatteries() noexcept;
	POSITION FindBattery(LPCSTR pszName) const noexcept;

	STDMETHOD(RenderResult)(CStringA &strResult) noexcept;

private:
	using MyBase::AddHead;
	using MyBase::AddHeadList;
	using MyBase::AddTail;
	using MyBase::AddTailList;
	using MyBase::InsertAfter;
	using MyBase::InsertBefore;
	using MyBase::MoveToHead;
	using MyBase::MoveToTail;
	using MyBase::RemoveAll;
	using MyBase::RemoveAt;
	using MyBase::RemoveHead;
	using MyBase::RemoveHeadNoReturn;
	using MyBase::RemoveTail;
	using MyBase::RemoveTailNoReturn;
	using MyBase::SetAt;
	using MyBase::SwapElements;

	CStringA m_strLastRender;
};
#pragma once
#include "ntUPSd.Core.Base.h"

namespace CTL
{
	template <typename Base>
	class CComLiteObject : public Base
	{
	public:
		CComLiteObject() noexcept = default;
		CComLiteObject(CComLiteObject &&) = delete;
		CComLiteObject(const CComLiteObject &) = delete;
		CComLiteObject &operator =(CComLiteObject &&) = delete;
		CComLiteObject &operator =(const CComLiteObject &) = delete;
		virtual ~CComLiteObject() noexcept = default;

		STDMETHOD_(ULONG, AddRef)() noexcept override;
		STDMETHOD_(ULONG, Release)() noexcept override;
		STDMETHOD(QueryInterface)(_In_ REFIID riid, _COM_Outptr_ void **ppvObject) noexcept override;

	private:
		using Base::InnerAddRef;
		using Base::InnerRelease;
		using Base::InnerQueryInterface;
	};

	class CComLiteObjectBase : public IUnknown
	{
	public:
		CComLiteObjectBase(CComLiteObjectBase &&) = delete;
		CComLiteObjectBase(const CComLiteObjectBase &) = delete;
		CComLiteObjectBase &operator =(CComLiteObjectBase &&) = delete;
		CComLiteObjectBase &operator =(const CComLiteObjectBase &) = delete;

		ULONG InnerAddRef() noexcept;
		ULONG InnerRelease() noexcept;
		HRESULT InnerQueryInterface(_In_ REFIID riid, _Outptr_ void **ppvObject) noexcept;

	protected:
		CComLiteObjectBase() noexcept = default;
		virtual ~CComLiteObjectBase() noexcept = default;

	private:
		ULONG m_cRefs = 0;
	};

	template <class First, class ...Rest>
	class ATL_NO_VTABLE IUnknownImpl : public First, public IUnknownImpl<Rest...>
	{
	public:
		IUnknownImpl(IUnknownImpl &&) = delete;
		IUnknownImpl(const IUnknownImpl &) = delete;
		IUnknownImpl &operator =(IUnknownImpl &&) = delete;
		IUnknownImpl &operator =(const IUnknownImpl &) = delete;

		HRESULT InnerQueryInterface(_In_ REFIID riid, _Outptr_ void **ppvObject) noexcept;

	protected:
		IUnknownImpl() noexcept = default;
		virtual ~IUnknownImpl() noexcept = default;
	};

	template <class Last>
	class ATL_NO_VTABLE IUnknownImpl<Last> : public Last, public CComLiteObjectBase
	{
	public:
		IUnknownImpl(IUnknownImpl &&) = delete;
		IUnknownImpl(const IUnknownImpl &) = delete;
		IUnknownImpl &operator =(IUnknownImpl &&) = delete;
		IUnknownImpl &operator =(const IUnknownImpl &) = delete;

	protected:
		IUnknownImpl() noexcept = default;
		virtual ~IUnknownImpl() noexcept = default;

		HRESULT InnerQueryInterface(_In_ REFIID riid, _Outptr_ void **ppvObject) noexcept;
	};
}

#include "ntUPSd.COM.inl.h"

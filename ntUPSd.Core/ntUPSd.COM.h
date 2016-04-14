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

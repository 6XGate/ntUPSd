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
#include "ntUPSd.ReplDriver.h"

namespace CTL
{
	STDMETHODIMP_(ULONG) CReplResult::AddRef() noexcept
	{
		return ::InterlockedIncrement(&m_cRefs);
	}

	STDMETHODIMP_(ULONG) CReplResult::Release() noexcept
	{
		ULONG cRefs = ::InterlockedDecrement(&m_cRefs);
		if (cRefs == 0)
		{
			delete this;
		}

		return cRefs;
	}

	STDMETHODIMP CReplResult::QueryInterface(REFIID riid, void **ppvObject) noexcept
	{
		if (ppvObject == nullptr)
		{
			return E_POINTER;
		}

		*ppvObject = nullptr;

		HRESULT hr = E_NOINTERFACE;
		if (IsEqualIID(riid, __uuidof(IUnknown)))
		{
			*ppvObject = static_cast<IUnknown*>(this);
			hr = S_OK;
			AddRef();
		}
		else if (IsEqualIID(riid, __uuidof(IReplResult)))
		{
			*ppvObject = static_cast<IReplResult*>(this);
			hr = S_OK;
			AddRef();
		}

		return hr;
	}

	CReplDriverBase::~CReplDriverBase() noexcept
	{
		// We don't own these handles, just using CAtlFile them as a wrapper.
		m_hInput.Detach();
		m_hOutput.Detach();
	}

	LPSTR CReplDriverBase::GetLine() noexcept
	{
		// Front trim the line.
		CHAR *pchStart = m_pszBuffer;
		while (*pchStart != 0)
		{
			if (isspace(*pchStart))
			{
				++pchStart;
			}
			else
			{
				break;
			}
		}

		// Rear trim the line.
		CHAR *pchEnd = m_pszBuffer + strlen(m_pszBuffer);
		while (pchEnd > pchStart)
		{
			if (isspace(*pchEnd) || *pchEnd == 0)
			{
				*pchEnd-- = 0;
			}
			else
			{
				break;
			}
		}

		return pchStart;
	}

	HRESULT CReplDriverBase::Initialize(HANDLE hInput, HANDLE hOutput) noexcept
	{
		if (!m_pszBuffer.Allocate(BUFFER_SIZE))
		{
			return E_OUTOFMEMORY;
		}

		m_hInput.Attach(hInput);
		m_hOutput.Attach(hOutput);
		return S_OK;
	}

	HRESULT CReplDriverBase::ReadLine() noexcept
	{
		// Empty the line buffer.
		SecureZeroMemory(m_pszBuffer, BUFFER_SIZE);

		// Keep reading until the buffer is full or the loop returns.
		HRESULT hr = S_OK;
		SIZE_T cchUsed = 0;
		CHAR *pchPos = m_pszBuffer;
		while (cchUsed < BUFFER_SIZE)
		{
			// Read the next character.
			DWORD fReceived = FALSE;
			hr = m_hInput.Read(pchPos, 1, fReceived);
			if (hr == __HRESULT_FROM_WIN32(ERROR_NETNAME_DELETED))
			{
				// File API way of telling us the client has disconnected, so return that error.
				return E_DISCONNECTED;
			}
			else if (FAILED(hr))
			{
				// Other general failures.
				return hr;
			}

			if (fReceived)
			{
				switch (*pchPos)
				{
				case '\n':
				case '\r':
					// Don't leave the CRLF characters on the end.
					*pchPos = 0;
					// If the pointer did not move, then the line is empty.
					return m_pszBuffer.m_pData != pchPos ? S_OK : S_FALSE;
				default:
					++pchPos;
					++cchUsed;
				}
			}
			else
			{
				// No more data would mean the end of the input has been reached, this is either:
				// 1. Socket I/O would have indicated a disconnection from the client side.
				// 2. The console or terminal has been closed.
				// 3. The end of a pipe or file.
				return E_DISCONNECTED;
			}
		}

		// If the line will not fit in the buffer, consider it an error, this should prevent any
		// buffer overrun exploits.
		return E_NOT_SUFFICIENT_BUFFER;
	}

	HRESULT CReplDriverBase::WriteResult(LPCSTR pszLine) noexcept
	{
		// Don't echo empty lines.
		DWORD cchLine = static_cast<DWORD>(strlen(pszLine));
		if (cchLine == 0)
		{
			return S_OK;
		}

		HRESULT hr;
		if (!(pszLine[cchLine - 1] == '\r' || pszLine[cchLine - 1] == '\n'))
		{
			// The line is missing its ending, so provide one.
			ATLTRACE("WR: %s\n", pszLine);
			hr = m_hOutput.Write(pszLine, cchLine);
			if (SUCCEEDED(hr))
			{
				hr = m_hInput.Write("\r\n", 2);
			}
		}
		else
		{
			// The line has its ending, so don't add one.
			ATLTRACE("WR: %s", pszLine);
			hr = m_hOutput.Write(pszLine, cchLine);
		}

		return hr;
	}
}

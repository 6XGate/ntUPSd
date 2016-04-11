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
#include "ntUPSd.ReplDriver.h"

namespace CTL
{
	template <typename Processor>
	inline HRESULT CReplDriver<Processor>::Initialize(HANDLE hInput, HANDLE hOutput) noexcept
	{
		HRESULT hr = __super::Initialize(hInput, hOutput);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = m_Processor.Initialize();
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}

	template <typename Processor>
	inline HRESULT CReplDriver<Processor>::BeginProcessing() noexcept
	{
		HRESULT hr = S_OK;
		for (;;)
		{
			if (!m_Processor.Continue())
			{
				break;
			}

			m_pLastResult.Release();

			hr = ReadLine();
			if (hr == S_OK)
			{
				_ATLTRY
				{
					LPCSTR pszLine = GetLine();
					ATLTRACE("RD: %s\n", pszLine);
					HRESULT hrEval = m_Processor.Eval(pszLine, m_pLastResult);
					if (SUCCEEDED(hrEval))
					{
						CStringA strResult;
						if (m_pLastResult != nullptr)
						{
							hr = m_pLastResult->RenderResult(strResult);
						}
						else
						{
							hr = m_Processor.DefaultResult(strResult);
						}

						if (SUCCEEDED(hr))
						{
							hr = WriteResult(strResult);
						}
					}
					else
					{
						LPCSTR pszError = m_Processor.ReportError(hrEval, pszLine);
						hr = WriteResult(pszError);
					}
				}
				_ATLCATCH(ex)
				{
					hr = ex.m_hr;
				}
				_ATLCATCHALL()
				{
					hr = E_FAIL;
				}
			}

			if (FAILED(hr))
			{
				break;
			}
		}

		return hr == E_DISCONNECTED ? S_OK : hr;
	}
}

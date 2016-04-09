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

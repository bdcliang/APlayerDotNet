
#include <tchar.h>
#include "playersink.h"
#include "resource.h"
#include "simpleplayer.h"

#define EventId_Message				1
#define EventId_StateChanged		2
#define EventId_OpenSuccess			3
#define EventId_SeekCompleted		4
#define EventId_OnBuffer			5
#define EventId_OnVideoSizeChanged	6
#define EventId_OnDownloadCodec		7
#define EventId_OnEvent				8


CPlayerSink::CPlayerSink(void)
{
	m_pAPlayerEventHandler = NULL;
}

CPlayerSink::~CPlayerSink(void)
{
}

STDMETHODIMP CPlayerSink::QueryInterface(REFIID iid, void ** ppvObject)
{
	*ppvObject = this;
	return S_OK;
}

ULONG __stdcall CPlayerSink::AddRef(void)
{
	return 1;
}

ULONG __stdcall CPlayerSink::Release(void)
{
	return 0;
}

STDMETHODIMP CPlayerSink::GetTypeInfoCount(unsigned int FAR* pctinfo)
{
	*pctinfo = 0;
	return E_NOTIMPL;
}

STDMETHODIMP CPlayerSink::GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo)
{
	*ppTInfo = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP CPlayerSink::GetIDsOfNames(REFIID riid, 
										OLECHAR FAR* FAR* rgszNames, 
										unsigned int cNames, 
										LCID lcid, 
										DISPID FAR* rgDispId)
{
	return E_NOTIMPL;
}

STDMETHODIMP CPlayerSink::Invoke(DISPID dispIdMember, 
								 REFIID riid, 
								 LCID lcid, 
								 WORD wFlags, 
								 DISPPARAMS FAR* pDispParams, 
								 VARIANT FAR* pVarResult, 
								 EXCEPINFO FAR* pExcepInfo, 
								 unsigned int FAR* puArgErr)
{
	switch(dispIdMember)
	{
	case EventId_Message:
		{
			if(pDispParams->cArgs == 3)
			{
				m_pAPlayerEventHandler->OnMessage(pDispParams->rgvarg[2].lVal,
					pDispParams->rgvarg[1].lVal,
					pDispParams->rgvarg[0].lVal);
			}
		}
		break;

	case EventId_StateChanged:
		{
			if(pDispParams->cArgs == 2)
			{
				m_pAPlayerEventHandler->OnStateChanged(pDispParams->rgvarg[1].lVal, 
					pDispParams->rgvarg[0].lVal);
			}
		}
		break;

	case EventId_OpenSuccess:
		{
			if(pDispParams->cArgs == 0)
			{
				m_pAPlayerEventHandler->OnOpenSucceeded();
			}
		}
		break;

	case EventId_SeekCompleted:
		{
			if (pDispParams->cArgs == 1)
			{
				m_pAPlayerEventHandler->OnSeekCompleted(pDispParams->rgvarg[0].lVal);
			}
		}
		break;

	case EventId_OnBuffer:
		{
			if(pDispParams->cArgs == 1)
			{
				m_pAPlayerEventHandler->OnBuffer(pDispParams->rgvarg[0].lVal);
			}
		}
		break;

	case EventId_OnVideoSizeChanged:
		{
			if(pDispParams->cArgs == 0)
			{
				m_pAPlayerEventHandler->OnVideoSizeChanged();
			}
		}
		break;

	case EventId_OnDownloadCodec:
		{
			if(pDispParams->cArgs == 1)
			{
				m_pAPlayerEventHandler->OnDownloadCodec(pDispParams->rgvarg[0].bstrVal);
			}
		}
		break;

	case EventId_OnEvent:
		{
			if(pDispParams->cArgs == 2)
			{

			}
		}
		break;
	}

	return S_OK;
}

void CPlayerSink::SetEventHandler(CPlayerEventHandler * pAPlayerEventHandler)
{
	m_pAPlayerEventHandler = pAPlayerEventHandler;
}
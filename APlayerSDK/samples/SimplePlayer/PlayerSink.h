#pragma once
#include "oaidl.h"


#import "../../Bin/APlayer.dll"

class CPlayerEventHandler;
class CPlayerSink :	public IDispatch
{
public:
	CPlayerSink(void);
	~CPlayerSink(void);

	// IUnknown
	ULONG __stdcall AddRef(void);
	ULONG __stdcall Release(void);
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject);

	// IDispatch
	STDMETHOD(GetTypeInfoCount)(unsigned int FAR* pctinfo);
	STDMETHOD(GetTypeInfo)(unsigned int iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo);
	STDMETHOD(GetIDsOfNames)(REFIID riid, 
							OLECHAR FAR* FAR* rgszNames, 
							unsigned int cNames, 
							LCID lcid, 
							DISPID FAR* rgDispId);
	STDMETHOD(Invoke)(DISPID dispIdMember, 
						REFIID riid, 
						LCID lcid, 
						WORD wFlags, 
						DISPPARAMS FAR* pDispParams, 
						VARIANT FAR* pVarResult, 
						EXCEPINFO FAR* pExcepInfo, 
						unsigned int FAR* puArgErr);


	CPlayerEventHandler * m_pAPlayerEventHandler;
	void SetEventHandler(CPlayerEventHandler * pAPlayerEventHandler);
};

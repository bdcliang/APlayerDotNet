#include <Windowsx.h>
#include <tchar.h>
#include <atlbase.h>
#include "PlayerSink.h"


enum PLAY_STATE
{
	PS_READY		= 0,
	PS_OPENING		= 1,
	PS_PAUSING		= 2,
	PS_PAUSED		= 3,
	PS_PLAYING		= 4,
	PS_PLAY			= 5,
	PS_CLOSING		= 6,
};

class CPlayerEventHandler
{
public:
	virtual	HRESULT	OnMessage(LONG nMessage, LONG wParam, LONG lParam) = 0;
	virtual	HRESULT	OnStateChanged(LONG nOldState, LONG nNewState) = 0;
	virtual	HRESULT	OnOpenSucceeded() = 0;
	virtual	HRESULT	OnSeekCompleted(LONG nPosition) = 0;
	virtual	HRESULT	OnBuffer(LONG nPercent) = 0;
	virtual	HRESULT	OnVideoSizeChanged(void) = 0;
	virtual	HRESULT	OnDownloadCodec(BSTR strCodecPath) = 0;
};

class CMyPlayer : public CPlayerEventHandler
{
public:
	int						m_nPosition;
	int						m_nDuration;
	APlayer3Lib::IPlayer *	m_pAPlayer;
	IConnectionPoint *		m_pConnectionPoint;
	DWORD					m_dwCookie;
	HWND					m_hAPlayerWnd;
	HMODULE					m_hModule;

public:
	int				GetConfigInt(int nConfigId);
	void			Initialize();
	void			Release();
	BOOL			GetPathFromFullName(const TCHAR * pcszFullName, TCHAR * pszPath);
	BOOL			CreateAPlayerWindow();
	BOOL			SetAPlayerWindowSize(HWND hwnd);
	void			OpenLocalFile(void);
	void			MillisecondToText(int nMs, TCHAR * pszText);
	HRESULT			CreateInstanceFromFile(const TCHAR * pcszPath, REFCLSID rclsid, 
										   REFIID riid, IUnknown * pUnkOuter, LPVOID * ppv);
	BOOL			SelectFileDlg(HWND hOwner, BOOL bIsSave, const TCHAR * pcszTitle, const TCHAR * pcszFilter,
								  TCHAR * pszFilePath, int nMaxFilePath, TCHAR * pszFileTitle, int nMaxFileTitle);

	// aplayer event handler
	virtual	HRESULT	OnMessage(LONG nMessage, LONG wParam, LONG lParam);
	virtual	HRESULT	OnStateChanged(LONG nOldState, LONG nNewState);
	virtual	HRESULT	OnOpenSucceeded();
	virtual	HRESULT	OnSeekCompleted(LONG nPosition);
	virtual	HRESULT	OnBuffer(LONG nPercent);
	virtual	HRESULT	OnVideoSizeChanged(void);
	virtual	HRESULT	OnDownloadCodec(BSTR strCodecPath);
};

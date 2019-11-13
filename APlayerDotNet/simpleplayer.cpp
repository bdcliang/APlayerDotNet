#include "simpleplayer.h"
#include "PlayerSink.h"
#include <CommCtrl.h>
#include "atlhost.h"
#include "Winsock.h"

#pragma comment(lib, "atls.lib")
#define countof(x)			(sizeof(x)/sizeof(x[0]))

#define CONFIGID_PLAYRESULT		7		// Play result

#ifdef UNICODE
	#define lstrtoi		_wtoi
#else
	#define lstrtoi		atoi
#endif

HRESULT CMyPlayer::OnMessage(LONG nMessage, LONG wParam, LONG lParam)
{
	return S_OK;
}
//
int CMyPlayer::GetConfig(int nConfigId)
{
	USES_CONVERSION;
	_bstr_t strConfig = m_pAPlayer->GetConfig(nConfigId);
	LPTSTR pConfig = W2T(strConfig);
	int nResult = lstrtoi(pConfig);
	return nResult;
}
//
HRESULT CMyPlayer::OnStateChanged(LONG nOldState, LONG nNewState)
{
	PLAY_STATE state;
	//
	switch(nNewState)
	{
	case PS_READY:
		{
		state = PS_READY;
		}
		break;

	case PS_OPENING:
		state = PS_OPENING;
		break;

	case PS_PAUSING:
		state = PS_PAUSING;
		break;

	case PS_PAUSED:
		state = PS_PAUSED;
		break;

	case PS_PLAYING:
		state = PS_PLAYING;
		if(this->enableStrech)
			SetAspectRatio(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
		break;

	case PS_PLAY:
		state = PS_PLAY;
		break;

	case PS_CLOSING:
		state = PS_CLOSING;
		break;
	}
	if (_phandler)
	{
		_phandler(state);
	}
	return S_OK;
}
//
HRESULT CMyPlayer::OnOpenSucceeded()
{
	m_nDuration = m_pAPlayer->GetDuration();
	if (this->enableStrech)
		SetAspectRatio(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
	return S_OK;
}
//
HRESULT CMyPlayer::OnSeekCompleted(LONG nPosition)
{
	return S_OK;
}
//
HRESULT CMyPlayer::OnBuffer(LONG nPercent)
{
	return S_OK;
}
//
HRESULT CMyPlayer::OnVideoSizeChanged(void)
{
	if (this->enableStrech)
		SetAspectRatio(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
	return S_OK;
}
//
HRESULT CMyPlayer::OnDownloadCodec(BSTR strCodecPath)
{
	USES_CONVERSION;
	TCHAR szText[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	GetPathFromFullName(W2T(strCodecPath), szPath);
	wsprintf(szText, _T("缺少解码器 %s, 下载后解压放在%s文件夹下。\n下载地址：http://aplayer.open.xunlei.com/codecs.zip"), W2T(strCodecPath), szPath);
	MessageBox(g_hWnd, szText, _T("SimplePlayer"), MB_ICONINFORMATION);
	// 异步下载解码器模式，通知APlayer取消当前下载
	m_pAPlayer->SetConfig(19, _bstr_t("0"));
	return S_OK;
}

void CMyPlayer::Initialize()
{
	m_nPosition = 0;
	m_nDuration = 1000;
	m_pAPlayer = NULL;
	m_pConnectionPoint = NULL;
	m_dwCookie = 0;
	m_hAPlayerWnd = NULL;
	m_hModule = NULL;
}
//
void CMyPlayer::MillisecondToText(int nMs, TCHAR * pszText)
{
	int nSecond = nMs / 1000;
	int nMillisecond = nMs % 1000;

	int nHour = nSecond / 3600;
	nSecond = nSecond % 3600;
	int nMinute = nSecond / 60;
	nSecond = nSecond % 60;

	_stprintf_s((wchar_t*)pszText, 20, _T("%02d:%02d:%02d"), nHour, nMinute, nSecond);
}

BOOL CMyPlayer::GetPathFromFullName(const TCHAR * pcszFullName, TCHAR * pszPath)
{
	int nLength = lstrlen(pcszFullName);
	if(nLength >= MAX_PATH)
		return FALSE;
	lstrcpy(pszPath, pcszFullName);
	TCHAR * pEnd = pszPath + nLength - 1;
	while(pEnd > pszPath && *pEnd != '\\') pEnd--;
	*pEnd = '\0';
	return TRUE;
}
//
void CMyPlayer::OpenLocalFile(void)
{
	if (m_pAPlayer == NULL)
	{
		TCHAR szFullName[MAX_PATH];
		GetModuleFileNameW(NULL, szFullName, MAX_PATH);
		TCHAR szPath[MAX_PATH];
		GetPathFromFullName(szFullName, szPath);
		wsprintf(szFullName, _T("请先下载APlayer SDK，并将其放解压放在%s文件夹下。下载地址：http://aplayer.open.xunlei.com/codecs.zip"), szPath);
		MessageBox(g_hWnd, szFullName, _T("SimplePlayer"), MB_ICONINFORMATION);
		return ;
	}
	// The file path
	TCHAR szFileName[MAX_PATH];
	memset(szFileName, 0, sizeof(szFileName));

	// Build filter
	TCHAR szAllFormat[512];
	lstrcpy(szAllFormat, _T(""));
	lstrcat(szAllFormat, _T("*.wmv;*.wmp;*.wm;*.asf;*.wma;*.avi;*.wav;*.mpg;*.mpeg;*.dat;"));
	lstrcat(szAllFormat, _T("*.ts;*.mpa;*.mp2;*.vob;*.ifo;*.mp3;*.mid;*.ogm;*.ogg;*.cda;"));
	lstrcat(szAllFormat, _T("*.d2v;*.mp4;*.3gp;*.mkv;*.rm;*.ram;*.rmvb;*.rpm;*.ra;*.mov;"));
	lstrcat(szAllFormat, _T("*.qt;*.amr;*.mpc;*.flv;*.swf;"));
	lstrcat(szAllFormat, _T("*.evo;*.tta;*.m4b;"));
	lstrcat(szAllFormat, _T("*.xv;*.xvx;*.xlmv"));

	TCHAR szFilter[512];
	TCHAR * pFilter = szFilter;
	wsprintf(pFilter, _T("支持的媒体文件(%s)"), szAllFormat);
	pFilter += (lstrlen(pFilter) + 1);
	lstrcpy(pFilter, szAllFormat);
	pFilter += (lstrlen(pFilter) + 1);
	lstrcpy(pFilter, _T("所有文件(*.*)"));
	pFilter += (lstrlen(pFilter) + 1);
	lstrcpy(pFilter, _T("*.*"));
	pFilter += (lstrlen(pFilter) + 1);
	*pFilter = 0; pFilter++;

	// Select file
	BOOL bResult = SelectFileDlg(g_hWnd, FALSE, _T("选择媒体文件"), szFilter,
		szFileName, countof(szFileName), NULL, 0);
	if(bResult)
		m_pAPlayer->Open(szFileName);
}

void CMyPlayer::Release()
{
	if (m_pAPlayer != NULL)
	{
		m_pAPlayer->Close();

		if (m_dwCookie != 0)
		{
			m_pConnectionPoint->Unadvise(m_dwCookie);
			m_dwCookie = 0;
		}

		m_pAPlayer->Release();
		m_pAPlayer = NULL;

		if (IsWindow(m_hAPlayerWnd))
		{
			DestroyWindow(m_hAPlayerWnd);
			m_hAPlayerWnd = NULL;
		}

		FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}

HRESULT CMyPlayer::CreateInstanceFromFile(const TCHAR * pcszPath,
											REFCLSID rclsid,
											REFIID riid,
											IUnknown * pUnkOuter,
											LPVOID * ppv)
{
	m_hModule = ::LoadLibraryEx(pcszPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if(m_hModule == NULL)
		return TYPE_E_CANTLOADLIBRARY;

	typedef HRESULT (STDAPICALLTYPE *_pfnDllGetClassObject)(REFCLSID, REFIID, LPVOID*);
	_pfnDllGetClassObject pfnDllGetClassObject = (_pfnDllGetClassObject)GetProcAddress(m_hModule, "DllGetClassObject");
	if(pfnDllGetClassObject == NULL)
		return CO_E_ERRORINDLL;

	CComPtr<IClassFactory> spCF;
	HRESULT hr = (*pfnDllGetClassObject)(rclsid, IID_IClassFactory, (LPVOID*)&spCF);
	if(FAILED(hr))
		return hr;

	hr = spCF->CreateInstance(pUnkOuter, riid, ppv);
	if(FAILED(hr))
		return hr;

	return S_OK;
}

BOOL CMyPlayer::CreateAPlayerWindow()
{
	TCHAR szFullName[MAX_PATH];
	GetModuleFileNameW(NULL, szFullName, MAX_PATH);
	TCHAR szPath[MAX_PATH];
	GetPathFromFullName(szFullName, szPath);
	wsprintf(szPath, _T("%s\\APlayer.dll"), szPath);
	HRESULT hr = CreateInstanceFromFile(szPath, 
										__uuidof(APlayer3Lib::Player), 
										__uuidof(APlayer3Lib::IPlayer), 
										NULL, 
										(void **)&m_pAPlayer);

	//CComModule _Module;
	_pAtlModule = &_Module;
	if (SUCCEEDED(hr))
	{
		m_hAPlayerWnd = g_hWnd;
		/*m_hAPlayerWnd = ::CreateWindowEx(WS_EX_TOOLWINDOW, _T(ATLAXWIN_CLASS), _T(""),
										WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
										CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
										g_hWnd, NULL, NULL, NULL);*/
	}

	if (::IsWindow(m_hAPlayerWnd))
	{
		hr = AtlAxAttachControl(m_pAPlayer, m_hAPlayerWnd, NULL);
	}

	CComQIPtr<IConnectionPointContainer> spContainer;
	if (SUCCEEDED(hr))
	{
		hr = m_pAPlayer->QueryInterface(__uuidof(IConnectionPointContainer), 
										(void **)&spContainer);
	}

	if (SUCCEEDED(hr))
	{
		hr = spContainer->FindConnectionPoint(__uuidof(APlayer3Lib::_IPlayerEvents), 
												&m_pConnectionPoint);
	}

	if (SUCCEEDED(hr))
	{
		g_APlayerSink.SetEventHandler(this);
		hr = m_pConnectionPoint->Advise(&g_APlayerSink, &m_dwCookie);
	}

	if (SUCCEEDED(hr))
		return TRUE;
	else
		return FALSE;
}

BOOL CMyPlayer::SetAPlayerWindowSize(HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);

	BOOL bResult = FALSE;
	if (::IsWindow(m_hAPlayerWnd))
	{
		bResult = ::SetWindowPos(m_hAPlayerWnd, 
								HWND_TOP, 
								rect.left, 
								rect.top, 
								rect.right - rect.left, 
								rect.bottom - rect.top - 60, 
								SWP_SHOWWINDOW);
	}

	return bResult;
}

BOOL CMyPlayer::SelectFileDlg(HWND hOwner,
				   BOOL bIsSave,
				   const TCHAR * pcszTitle,
				   const TCHAR * pcszFilter,
				   TCHAR * pszFilePath,
				   int nMaxFilePath,
				   TCHAR * pszFileTitle,
				   int nMaxFileTitle)
{
	return S_OK;
}

void CMyPlayer::Play(TCHAR* path)
{
	m_pAPlayer->Open(path);
}
void CMyPlayer::Pause()
{
	m_pAPlayer->Pause();
}

void CMyPlayer::Resume()
{
	m_pAPlayer->Play();
}

void CMyPlayer::Stop()
{
	m_pAPlayer->Close();
}

void CMyPlayer::SetAspectRatio(int w, int h)
{
	TCHAR ratio[20];
	wsprintf(ratio, _T("%d;%d"),w,h);
	m_pAPlayer->SetConfig(204, ratio);
}

void CMyPlayer::SetCustomLogo(long nLogo)
{
	m_pAPlayer->SetCustomLogo(nLogo);
}
void CMyPlayer::SetConfig(long nConfigId, TCHAR* strValue)
{
	m_pAPlayer->SetConfig(nConfigId, strValue);
}

void CMyPlayer::SetHwnd(HWND hwnd)
{
	this->Initialize();
	g_hWnd = hwnd;
	if (this->CreateAPlayerWindow() == TRUE)
	{
		//this->SetAPlayerWindowSize(hwnd);
	}
	GetClientRect(g_hWnd, &clientRect);
}
void CMyPlayer::Mute()
{
	m_pAPlayer->SetVolume(0);
}

long CMyPlayer::GetState() {
	return m_pAPlayer->GetState();
}
long CMyPlayer::GetDuration() {
	return m_pAPlayer->GetDuration();
}
long CMyPlayer::GetPosition() {
	return m_pAPlayer->GetDuration();
}
void CMyPlayer::SetPosition(long pos) {
	m_pAPlayer->SetPosition(pos);
}
long CMyPlayer::GetVideoWidth() {
	return m_pAPlayer->GetVideoHeight();
}
long CMyPlayer::GetVideoHeight() {
	return m_pAPlayer->GetVideoHeight();
}
long CMyPlayer::GetVolume() {
	return m_pAPlayer->GetVolume();
}
void CMyPlayer::SetVolume(long num) {
	m_pAPlayer->SetVolume(num);
}
bool CMyPlayer::IsSeeking() {
	return m_pAPlayer->IsSeeking()?true:false;
}
long CMyPlayer::GetBufferProgress()
{
	return m_pAPlayer->GetBufferProgress();
}

TCHAR* CMyPlayer::GetVersion()
{
	return m_pAPlayer->GetVersion();
}
void CMyPlayer::SetStrechFill(bool enable)
{
	this->enableStrech = enable;
}
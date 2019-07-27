
#include "windows.h"
#include "resource.h"
#include <stdio.h>
#include <tchar.h>
#include <atlconv.h>

#import "../Bin/APlayer.dll"
#pragma comment(lib, "winmm.lib")

#define EXPORT_FUNCTION extern "C" __declspec(dllexport)

#define CONFIGID_LOADPLUGIN				24		// Load plugin
#define CONFIGID_FREEPLUGIN				25		// Free plugin
#define CONFIGID_IMAGENORMALIZEENABLE	305		// Image normalize

struct VIDEO_FRAME_INFO
{
	GUID			type;
	int				width;
	int				height;
	BYTE *			frame;
	__int64			time_stamp;
};

struct AUDIO_FRAME_INFO
{
	int				channel;
	int				bit_per_sample;
	BOOL			is_float;
	int				sample_count;
	BYTE *			frame;
	__int64			time_stamp;
};


HINSTANCE				g_hInstance = NULL;
HWND					g_hDlg = NULL;

APlayer3Lib::IPlayer *	g_pAPlayer = NULL;
int						g_nVideoFrameCount = 0;
int						g_nAudioFrameCount = 0;
int						g_nDuration = 0;
int						g_nPosition = 0;

int						g_nFPSCount = 0;
DWORD					g_dwLastTick = 0;
float					g_fFPS = 0.0;


extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hInstance;
		break;
	}
	return TRUE;
}


BOOL IsChecked(HWND hDlg, int nControlId)
{
	UINT uResult = IsDlgButtonChecked(hDlg, nControlId);
	BOOL bResult = (uResult == BST_CHECKED);
	return bResult;
}


int WINAPI MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_NORMALIZE:
				{
					if(g_pAPlayer != NULL)
					{
						if(IsChecked(hDlg, IDC_NORMALIZE))
							g_pAPlayer->SetConfig(CONFIGID_IMAGENORMALIZEENABLE, L"1");
						else
							g_pAPlayer->SetConfig(CONFIGID_IMAGENORMALIZEENABLE, L"0");
					}
				}
				break;
			}
		}
		break;
	}
	return FALSE;
}


void MsToText(int nMs, TCHAR * pszText)
{
	int nSecond = nMs / 1000;
	int nMillisecond = nMs % 1000;

	int nHour = nSecond / 3600;
	nSecond = nSecond % 3600;
	int nMinute = nSecond / 60;
	nSecond = nSecond % 60;

	wsprintf(pszText, _T("%02d:%02d:%02d.%03d"), nHour, nMinute, nSecond, nMillisecond);
}


void UpdateUI(void)
{
	USES_CONVERSION;

	TCHAR szText[MAX_PATH];
	wsprintf(szText, _T("视频 %d, 音频 %d"), g_nVideoFrameCount, g_nAudioFrameCount);
	::SetDlgItemText(g_hDlg, IDC_STATIC_FRAME, szText);

	TCHAR szPosition[100];
	TCHAR szDuration[100];
	MsToText(g_nPosition, szPosition);
	MsToText(g_nDuration, szDuration);
	wsprintf(szText, _T("%s / %s"), szPosition, szDuration);
	::SetDlgItemText(g_hDlg, IDC_STATIC_TIME, szText);

	if(g_fFPS > 0.0)
	{
		char szFPS[100];
		sprintf(szFPS, "%.2f", g_fFPS);
		lstrcpy(szText, A2T(szFPS));
	}
	else
		lstrcpy(szText, _T("计算中..."));
	::SetDlgItemText(g_hDlg, IDC_STATIC_FPS, szText);
}


#define LIMIT_BYTE(x)	if(x<0) x=0; else if(x>255) x=255;

BOOL DrawFrame(HWND hWnd, int nWidth, int nHeight, const BYTE * pFrameData)
{
	if(nWidth == 0 || nHeight == 0)
		return FALSE;
	HDC hDC = ::GetDC(hWnd);
	if(hDC == NULL)
		return FALSE;

	BITMAPINFO bi;
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biWidth = nWidth;
	bi.bmiHeader.biHeight = -int(nHeight);
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = nWidth * nHeight * 4;
	
	BYTE * pBits = NULL;
	HBITMAP hBmp = ::CreateDIBSection(hDC, &bi, DIB_RGB_COLORS, (void **)&pBits, NULL, 0);
	if(hBmp != NULL && pBits != NULL)
	{
		HDC hMemDC = ::CreateCompatibleDC(hDC);
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBmp);

		BYTE * pD = pBits;
		const BYTE * pY = pFrameData;
		const BYTE * pV = pFrameData + nWidth * nHeight;
		const BYTE * pU = pFrameData + nWidth * nHeight * 5 / 4;

		int Y, U, V;
		int R, G, B;
		DWORD D = 0;
		for(DWORD y=0; y<nHeight; y++)
		{
			const BYTE * pOldU = pU;
			const BYTE * pOldV = pV;
			for(DWORD x=0; x<nWidth; x++)
			{
				Y = *pY;
				V = *pV;
				U = *pU;
				
				int N = Y << 10;
				R = (N + V * 1900 - 240000) >> 10;
				G = (N - V * 570 - U * 450 + 136000) >> 10;
				B = (N + U * 1630 - 200000) >> 10;

				LIMIT_BYTE(R);
				LIMIT_BYTE(G);
				LIMIT_BYTE(B);

				D = R << 16 | G << 8 | B;
				*(DWORD *)pD = D;
				pD += 4;

				pY++;
				if(x % 2 == 1)
				{
					pV++;
					pU++;
				}
			}
			pU = pOldU;
			pV = pOldV;

			if(y % 2 == 1)
			{
				pU += nWidth / 2;
				pV += nWidth / 2;
			}
		}

		RECT rcClient;
		::GetClientRect(hWnd, &rcClient);
		int cw = rcClient.right-rcClient.left;
		int ch = rcClient.bottom-rcClient.top;
		double dVideo = (double)nWidth / nHeight;
		double dClient = (double)cw / ch;
		int dx, dy, dw, dh;
		if(dVideo >= dClient)
		{
			dw = cw;
			dh = cw * nHeight / nWidth;
			dx = 0;
			dy = (ch - dh) / 2;
		}
		else
		{
			dw = ch * nWidth / nHeight;
			dh = ch;
			dx = (cw - dw) / 2;
			dy = 0;
		}

		::SetStretchBltMode(hDC, COLORONCOLOR);
		BOOL bOK = ::StretchBlt(hDC, dx, dy, dw, dh, hMemDC, 0, 0, nWidth, nHeight, SRCCOPY);

		::SelectObject(hMemDC, hOldBmp);
		::DeleteDC(hMemDC);
		::DeleteObject(hBmp);
	}
	
	::ReleaseDC(hWnd, hDC);
	return TRUE;
}


EXPORT_FUNCTION BOOL APlayerPluginInit(IUnknown * pAPlayer)
{
	g_pAPlayer = (APlayer3Lib::IPlayer *)pAPlayer;
	g_hDlg = ::CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDlgProc);
	if(g_hDlg == NULL)
		return FALSE;
	::ShowWindow(g_hDlg, SW_SHOW);
	return TRUE;
}


EXPORT_FUNCTION void APlayerPluginUninit(void)
{
	if(g_hDlg != NULL)
	{
		::DestroyWindow(g_hDlg);
		g_hDlg = NULL;
	}
}


EXPORT_FUNCTION BOOL APlayerPluginOpenMedia(const TCHAR * pcszUrl, __int64 nFileSize, int nDuration)
{
	g_nVideoFrameCount = 0;
	g_nAudioFrameCount = 0;
	g_nDuration = nDuration;
	g_nPosition = 0;
	return TRUE;
}


EXPORT_FUNCTION void APlayerPluginCloseMedia(void)
{

}


// 注意：APlayerPluginVideoFrame 是视频帧回调函数，
// 因为该函数是在渲染链路中执行的，所以务必保证这个函数快速返回，否则会引起视频滞后和慢动作。

EXPORT_FUNCTION void APlayerPluginVideoFrame(VIDEO_FRAME_INFO * pFrame)
{
	g_nVideoFrameCount++;
	g_nPosition = int(pFrame->time_stamp / 10000);

	// Calc FPS
	g_nFPSCount++;
	if(g_nFPSCount > 120)
	{
		DWORD dwTick = ::timeGetTime();
		if(g_dwLastTick != 0)
		{
			DWORD dwTime = dwTick - g_dwLastTick;
			if(dwTime > 0)
				g_fFPS = float(g_nFPSCount * 1000) / dwTime;
		}
		g_dwLastTick = dwTick;
		g_nFPSCount = 0;
	}

	// Update UI
	UpdateUI();

	// Draw image
	HWND hWnd = ::GetDlgItem(g_hDlg, IDC_VIDEO);
	DrawFrame(hWnd, pFrame->width, pFrame->height, pFrame->frame);
}


EXPORT_FUNCTION void APlayerPluginAudioFrame(AUDIO_FRAME_INFO * pFrame)
{
	g_nAudioFrameCount++;
	UpdateUI();
}



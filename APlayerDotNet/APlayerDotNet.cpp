#include "stdafx.h"

#include "APlayerDotNet.h"
namespace APlayerDotNet
{
	APlayer::APlayer()
	{
		player = new CMyPlayer();
	}
	APlayer::~APlayer()
	{
		gchandle.Free();
		if (player)
		{
			player->Release();
			delete player;
		}
		player = NULL;
	}
	bool APlayer::SetHwnd(IntPtr hwnd)
	{
		g_hwnd = hwnd;
		player->SetHwnd(static_cast<HWND>(g_hwnd.ToPointer()));
		MediaHandler^ mdelegate = gcnew MediaHandler(this,&APlayer::eventHandler);
		gchandle = GCHandle::Alloc(mdelegate);
		IntPtr pFunc = Marshal::GetFunctionPointerForDelegate(mdelegate);
		player->SetEventHandler(static_cast<p_Handler>(pFunc.ToPointer()));
		
		return true;
	}
	void APlayer::Play(String^ path)
	{
		IntPtr pStrPtr= Marshal::StringToHGlobalUni(path);
		TCHAR* pStr = static_cast<TCHAR*>(pStrPtr.ToPointer());
		player->Play(pStr);
		Marshal::FreeHGlobal(pStrPtr);
	}
	void APlayer::Pause()
	{
		player->Pause();
	}
	void APlayer::Resume()
	{
		player->Resume();
	}
	void APlayer::Stop()
	{
		player->Stop();
	}
	void APlayer::Mute()
	{
		player->Mute();
	}

	void APlayer::UnMute()
	{
		player->SetVolume(100);
	}
	void APlayer::SetCustomLogo(System::Drawing::Bitmap^ logo)
	{
		IntPtr hbit= logo->GetHbitmap();
		player->SetCustomLogo(hbit.ToInt32());
	}
	void APlayer::SetAspectRatio(int w, int h)
	{
		player->SetAspectRatio(w, h);
	}
	int APlayer::GetConfig(int nConfigId)
	{
		return player->GetConfig(nConfigId);
	}
	void APlayer::SetConfig(int nConfigId, String^ strVal)
	{
		IntPtr pStrPtr = Marshal::StringToHGlobalUni(strVal);
		TCHAR* pStr = static_cast<TCHAR*>(pStrPtr.ToPointer());
		player->SetConfig(nConfigId, pStr);
		Marshal::FreeHGlobal(pStrPtr);
	}
	long APlayer::GetState() {
		return player->GetState();
	}
	long APlayer::GetDuration() {
		return player->GetDuration();
	}
	long APlayer::GetPosition() {
		return player->GetPosition();
	}
	void APlayer::SetPosition(long pos) {
		player->SetPosition(pos);
	}
	long APlayer::GetVideoWidth() {
		return player->GetVideoWidth();
	}
	long APlayer::GetVideoHeight() {
		return player->GetVideoHeight();
	}
	long APlayer::GetVolume() {
		return player->GetVolume();
	}
	void APlayer::SetVolume(long num) {
		player->SetVolume(num);
	}
	bool APlayer::IsSeeking()
	{
		return player->IsSeeking();
	}
	long APlayer::GetBufferProgress()
	{
		return player->GetBufferProgress();
	}
	String^ APlayer::GetVersion()
	{
		TCHAR* pstr = player->GetVersion();
		IntPtr pstrPtr = static_cast<IntPtr>(pstr);
		String^ version=Marshal::PtrToStringUni(pstrPtr);
		return version;
	}
	void APlayer::SetStrechFill(bool enable)
	{
		player->SetStrechFill(enable);
	}

	void APlayer::eventHandler(PLAY_STATE state)
	{
		try
		{
			switch (state)
			{
			case APlayerDotNet::PS_READY:
				MediaReady();
				break;
			case APlayerDotNet::PS_OPENING:
				MediaOpening();
				break;
			case APlayerDotNet::PS_PAUSING:
				MediaPausing();
				break;
			case APlayerDotNet::PS_PAUSED:
				MediaPaused();
				break;
			case APlayerDotNet::PS_PLAYING:
				MediaPlaying();
				break;
			case APlayerDotNet::PS_PLAY:
				MediaPlay();
				break;
			case APlayerDotNet::PS_CLOSING:
				MediaClosing();
				break;
			default:
				break;
			}
		}
		catch (...)
		{
		
		}
			
	}

}

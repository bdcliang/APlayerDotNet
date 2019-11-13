#pragma once
#include "simpleplayer.h"
using namespace System;
using namespace System::Runtime::InteropServices;

namespace APlayerDotNet {

	public  enum  PLAY_STATE
	{
		PS_READY=0,
		PS_OPENING,
		PS_PAUSING,
		PS_PAUSED,
		PS_PLAYING,
		PS_PLAY,
		PS_CLOSING
	};


	[UnmanagedFunctionPointer(CallingConvention::StdCall)]

	public delegate void MediaEventHandler();
	public delegate void MediaHandler(PLAY_STATE);
	public ref class APlayer
	{
	public:
		APlayer();
		~APlayer();
		/// <summary>
		/// 设置视频播放窗口
		/// </summary>
		bool SetHwnd(IntPtr hwnd);
		///
		///play the specified video path to play
		///
		void Play(String^ path);
		///
		///pause the video
		///
		void Pause();
		///
		///resume the paused video
		///
		void Resume();
		///
		///stop the video
		///
		void Stop();
		///
		///set the volume mute
		///
		void Mute();
		///
		///recovery the volume to normal
		///
		void UnMute();
		///
		///set the default the screen logo of the player
		///
		void SetCustomLogo(System::Drawing::Bitmap^ logo);
		/*
		@prief  需要在视频播放的时候进行设置
		@[in] w 视频的宽度
		@[in] h 视频的高度
		*/
		void SetAspectRatio(int w,int h);
		///
		///setconfig function
		///
		void SetConfig(int nConfigId,String^ strVal);
		///
		///get config function
		///
		int GetConfig(int nConfigId);

		long GetState();
		long GetDuration();
		long GetPosition();
		void SetPosition(long pos);
		long GetVideoWidth();
		long GetVideoHeight();
		long GetVolume();
		void SetVolume(long num);
		bool IsSeeking();
		long GetBufferProgress();
		///
		///get the current version of the Aplayer
		///
		String^ GetVersion();
		///
		///set the video strech to fill the parent control or not
		///
		void SetStrechFill(bool enable);

		event MediaEventHandler^ MediaReady;
		event MediaEventHandler^ MediaOpening;
		event MediaEventHandler^ MediaPausing;
		event MediaEventHandler^ MediaPaused;
		event MediaEventHandler^ MediaPlaying;
		event MediaEventHandler^ MediaPlay;
		event MediaEventHandler^ MediaClosing;
	private:
		CMyPlayer* player;
		IntPtr g_hwnd;
		GCHandle gchandle;
		void eventHandler(PLAY_STATE);
	};
}


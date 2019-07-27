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
		//
		void Play(String^ path);
		void Pause();
		void Resume();
		void Stop();
		void Mute();
		void UnMute();
		void SetCustomLogo(System::Drawing::Bitmap^ logo);
		/*
		@prief  需要在视频播放的时候进行设置
		@[in] w 视频的宽度
		@[in] h 视频的高度
		*/
		void SetAspectRatio(int w,int h);
		void SetConfig(int nConfigId,String^ strVal);

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
		String^ GetVersion();
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
		void eventHandler(PLAY_STATE);
	};
}


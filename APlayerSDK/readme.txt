
目录说明:
=========

bin:

  包含 APlayer 引擎主 DLL(APlayer.dll)和解码库目录 codecs, 使用之前需要复制需要的解码库到 codecs 目录中,
  完整解码库下载地址 http://aplayer.open.xunlei.com/codecs.zip

docs:
  APlayerSDK 帮助文档

include:
  1 - aplayer.idl    	APlayer.dll 引擎的接口定义文件
  2 - aplayerUI.idl  	APlayerUI.dll 控件的接口定义文件

samples:
  SimplePlayer:         Win32 调用 APlayer.dll 引擎开发的简单播放器。
  PluginDemo:           APlayer 插件的例子。
  APlayerUI sample:     利用 APlayerUI 控件做的一个页面, APlayerUI有播放控制条, 且可以在视频上叠加各种可视元素；使用前必须将页面和引用的 flash 文件放在 Http Server 上。

  运行例子前，请先使用 install.bat 安装APlayerSDK。


注：使用者分发调用 APlayer 的应用时，如果不是在网页中使用 APlayer，就可以不用在系统注册表中注册，
    可以参考帮助文档中的方法从文件中创建 APlayer 实例，绿色环保，整个 Application 解压即可运行。

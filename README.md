# APlayerDotNet
this is a dotnet wraper of APlayer,using CLR C++ to wrap unmanaged APlayer C++ libray,which wraps the common used natived functions of Aplayer,
and also include some simplify dotnet managed function,for user easy using.

usage:
  1.in your dotnet application,in the debug directory,APlayer.dll && codecs must be included in the root
  2.refrence the AplayerDotNet.dll into your project
  3.usage of APlayer:
  
  
     APlayer player=new APlayer();
     player.SetHwnd(controls.handle);
     
     play:
        player.play(path);
     pause:
        player.pause();
     resume:
        player.resume();
     sotp:
        player.stop();
       
     ..........  
     
    of course,you can explorer your own wraper of APlayer.

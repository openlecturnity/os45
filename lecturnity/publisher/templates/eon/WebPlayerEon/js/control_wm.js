var MEDIA_THRESHOLD = 300; // ms

var g_lastPositionMs = 0;

// 0: Stopped
// 1: Paused
// 2: Playing
var g_playerState = 0;
var g_masterSyncingToClip = -1;
var g_clipSyncingToMaster = -1;
var g_currentClipPlayer   = -1;

var g_pausedPositionValid = false;

var g_startPosition = 0;

var g_controlPlayerChecked = 0;

var g_controlIsSliding = false;
var g_controlIsSeeking = false;
var g_controlIsStoppedSeeking = false;
var g_controlWasPlaying = false;
var g_startReplayAfterStopmark = false;

var g_controlIsMuted = false;
var g_controlVolumeBackup = 50;

var g_controlHasPlayedOnce = false;

var g_controlIsProgressiveDownload = false;
var g_controlIsWmp9Plugin = false;
var g_controlHasWarnedProgressiveDownloadOnce = false;
var g_controlHasHiddenStatusBarOnce = false;

function control_getClipPlayer(playerNumber)
{
  if (!isNS4)
  {
    var clipPlayer = getEmbeddedObject('', 'ClipPlayer' + playerNumber);
    return clipPlayer;
  }
  else
  {
    var clipPlayer = document.layers["ClipLayer" + playerNumber].document.embeds["ClipPlayer" + playerNumber];
    return clipPlayer;
  }
}

function control_getMasterPlayer()
{
  var masterPlayer = getEmbeddedObject('', 'MasterPlayer');
  return masterPlayer;
}

function control_getCurrentPosition(player)
{
  try
  {
    if (isNS4 || isOpera)
      return Math.round(player.GetCurrentPosition() * 1000);
    else
      return Math.round(player.CurrentPosition * 1000);
  }
  catch (e)
  {
    return 0;
  }
}

function control_setCurrentPosition(player, msTime)
{
  try
  {
    if (isNS4 || isOpera)
      player.SetCurrentPosition(msTime / 1000.0);
    else
      player.CurrentPosition = msTime / 1000.0;
  }
  catch (e)
  {
  }
}

function control_hideStatusBar()
{
  if (g_controlHasHiddenStatusBarOnce)
    return; 

  var masterPlayer = control_getMasterPlayer();
  masterPlayer.ShowStatusBar = false;
  g_controlHasHiddenStatusBarOnce = true;

  var masterPlayerObj = masterPlayer;
  if (document.all)
    masterPlayerObj = document.getElementsByName("MasterPlayer")[0];
  
  var newPlayerWidth = masterPlayerObj.width;
  var newPlayerHeight = masterPlayerObj.height - 24;
  if (newPlayerHeight <= 0)
  {
    newPlayerWidth = 1;
    newPlayerHeight = 1;
  }
  masterPlayerObj.width = newPlayerWidth;
  masterPlayerObj.height = newPlayerHeight;
}

function control_beginSlide()
{
  g_controlIsSliding = true;
  g_controlWasPlaying = control_isPlaying();
  if (g_controlWasPlaying)
    control_stop();
}

function control_endSlide()
{
  g_controlIsSliding = false;
  g_controlHasWarnedProgressiveDownloadOnce = false;
  if (g_controlWasPlaying)
    control_playPause();
}

function controlInit()
{
  // Position the clips layers
  var clipsLeft = layout_getSlidesLeft();
  var clipsTop  = layout_getSlidesTop();
  for (var i=0; i<g_numOfClips; ++i)
  {
    var layerStyle = getHtmlObject('', 'ClipLayer' + i, '', true, true);
    var deltaW = (g_slidesWidth - g_clipEntries[i].width) / 2;
    var deltaH = (g_slidesHeight - g_clipEntries[i].height) / 2;
    if (deltaW < 0)
      deltaW = 0;
    if (deltaH < 0)
      deltaH = 0;
    layerStyle.left = clipsLeft + deltaW;
    layerStyle.top  = clipsTop  + deltaH;
  }

  layout_setPlayIsPause(false);
  layout_enableControl("play", true);

  setInterval("control_onSync()", 199);
  setTimeout("control_notifyCurrentTime(0);", 500);
}

function control_checkAutostart()
{
  if (g_isAutostartMode)
  {
    var masterPlayer = control_getMasterPlayer();
    if ((masterPlayer != null) && (!control_isPlaying()))
      control_play();
    else if (g_controlPlayerChecked < 5)
      setTimeout("control_checkAutostart()", 125);
  }
}

function control_checkPluginAndUrl()
{
  try
  {
    // Look for WMP 9 (or higher) PlugIn
    if (control_getMasterPlayer().ClientID == "{3300AD50-2C39-46c0-AE0A-000000000000}")
      g_controlIsWmp9Plugin = true;
      
    // Look for Progressive Download (URL begins with "http://")
    if (control_getMasterPlayer().SourceLink.substr(0,7) == "http://")
      g_controlIsProgressiveDownload = true;
      
    // Progressive Download on WMP 6.4 PlugIn?
    if (g_controlIsProgressiveDownload && !g_controlIsWmp9Plugin)
    {
      layout_enableControl("end", false);
      layout_enableControl("home", false);
      layout_enableControl("back", false);
      layout_enableControl("forward", false);
    }
  }
  catch (e)
  {
  }
}

function control_isPlayerVisible(player, playerNumber)
{
  var playerStyle = getHtmlObject('', 'ClipLayer' + playerNumber, '', true, true);
  return (playerStyle.visibility != 'hidden' &&
          playerStyle.visibility != 'hide');
}

function control_setVisible(player, playerNumber, bVisible)
{
  var playerStyle = getHtmlObject('', 'ClipLayer' + playerNumber, '', true, true);
  if (bVisible)
    playerStyle.visibility = 'visible';
  else
    playerStyle.visibility = 'hidden';

  if (!isNS4 && player)
  {
    if (bVisible)
      player.style.visibility = 'visible';
    else
    {
      player.style.visibility = 'hidden';
    }
  }
}

function control_notifyCurrentTime(valueMs)
{
  g_lastPositionMs = valueMs;
  
  var playerToBeVisible = -1;
  for (var i=0; i<g_numOfClips && playerToBeVisible == -1; ++i)
  {
    if (valueMs >= g_mediaStartTimes[i+1] &&
        valueMs < g_mediaStartTimes[i+1] + g_mediaDurations[i+1])
    {
      playerToBeVisible = i;
    }
  }

  for (var i=0; i<g_numOfClips; ++i)
  {
    var clipPlayer = control_getClipPlayer(i); //document.getElementById('ClipPlayer' + (i+1));
    if (i == playerToBeVisible)
    {
      if (!control_isPlayerVisible(clipPlayer, playerToBeVisible))
        control_setVisible(clipPlayer, playerToBeVisible, true);
    }
    else
    {
      if (control_isPlayerVisible(clipPlayer, i))
      {
        control_setVisible(clipPlayer, i, false);
        // Don't try to stop a clip player which never has been
        // started. Will lead to severe JavaScript errors under
        // certain circumstances.
        if (!control_isPlayerStopped(clipPlayer) && g_controlHasPlayedOnce)
        {
          try
          {
            clipPlayer.Stop(); 
          } 
          catch (e)
          {
          }
        }
      }
    }
  }

  layout_updateTime(valueMs);
  g_currentClipPlayer = playerToBeVisible;

  // this method is in slides.js
  slides_notifyNewMediaPlayerPosition(valueMs);
  
  return playerToBeVisible;
}

function control_validatePausedPosition()
{
  g_pausedPositionValid = true;
  if (g_controlWasPlaying)
    g_startReplayAfterStopmark = true;
}

function control_playPause()
{
  if (!g_controlHasPlayedOnce)
    g_controlHasPlayedOnce = true;

  // No pause/play in sync mode
  if (g_masterSyncingToClip >= 0 ||
      g_clipSyncingToMaster >= 0)
  {
    return;
  }
    
  if (g_playerState != 2)
  {
    control_play();
  }
  else // player state is 2 = playing
  {
    // Pause it
    control_pause();
  }
}

function control_play()
{
  g_playerState = 2;

  var positionMs = g_lastPositionMs;
  var masterPlayer = control_getMasterPlayer();

  if (control_isPlayerPlaying(masterPlayer))
  {
    // Emergency stop. Something's wrong.
    control_stop();
    return;
  }
  
  if (!control_isPlayerPaused(masterPlayer) || !g_pausedPositionValid)
  {
     control_setCurrentPosition(masterPlayer, positionMs);
     g_startPosition = positionMs;

     layout_displayIsBuffering(true, -1);
     layout_enableControl("play", false);
  }
  else
  {
    // Master player is paused and the slider was not 
    // touched; kick off the clip player, too, if possible.
    if (g_currentClipPlayer >= 0)
    {
      var clipPlayer = control_getClipPlayer(g_currentClipPlayer); //document.getElementById('ClipPlayer' + (g_currentClipPlayer + 1));
      try
      {
        if (control_isPlayerPaused(clipPlayer))
        {
          clipPlayer.Play();
          if (clipPlayer.HasError)
          {
            alert(clipPlayer.ErrorDescription + "\n\n" + clipPlayer.FileName);
            control_stop();
            return;
          }
          // In case of NS4: focus the popup
          // control_setVisible(clipPlayer, g_currentClipPlayer, true);
        }
        else
        {
          // No good. Stop the clip player just in case.
          clipPlayer.Stop();
        }
      }
      catch (e)
      {
      }
    }
  }

  try
  {
    masterPlayer.Play();

    if ((actualTimestamp+225) >= g_mediaDurations[0]) // 225ms as buffer
      control_seekTime(0);

    if (masterPlayer.HasError)
    {
      alert(masterPlayer.ErrorDescription + "\n\n" + masterPlayer.FileName);
      control_stop();
    }
    else
    {
      g_startReplayAfterStopmark = false;
      layout_setPlayIsPause(true);
    }
  }
  catch (e)
  {
    alert("Error: Could not start Master Player!");
  }
}

function control_pause()
{
  g_playerState = 1;

  var masterPlayer = control_getMasterPlayer();

  if (control_isPlayerPlaying(masterPlayer))
  {
    g_lastPositionMs = control_getCurrentPosition(masterPlayer);
    g_pausedPositionValid = true;

    try
    {
      masterPlayer.Pause();
    }
    catch (e)
    {
      alert("Error: Could not pause Master Player!");
    }

    if (g_currentClipPlayer >= 0)
    {
      var clipPlayer = control_getClipPlayer(g_currentClipPlayer);
      if (control_isPlayerPlaying(clipPlayer))
      {
        try
        {
          clipPlayer.Pause();
        }
        catch (e)
        {
          alert("Warning: Could not pause Clip Player!");
        }
      }
      else
      {
        // The clip was not playing when it should have,
        // Emergency stop!
        control_stop();
        return;
      }
    }

    layout_setPlayIsPause(false);
  }
}

function control_stop()
{
  try
  {
    var masterPlayer = control_getMasterPlayer();
    // Stop the master player if not already stopped
    if (!control_isPlayerStopped(masterPlayer))
      masterPlayer.Stop();
    
    // Now stop all running clip players (should not be more
    // than one).
    for (var i=0; i<g_numOfClips; ++i)
    {
      var clipPlayer = control_getClipPlayer(i);
      if (!control_isPlayerStopped(clipPlayer))
      {
        clipPlayer.Stop();
      }
    }
    
    g_playerState = 0;
    // Get out of any sync modes.
    g_masterSyncingToClip = -1;
    g_clipSyncingToMaster = -1;
    
    layout_enableControl("stop", false);
    layout_enableControl("play", true);
    layout_setPlayIsPause(false);
    layout_displayIsBuffering(false, 0);
  }
  catch (e)
  {
  }
}

function control_isPlayerPlaying(player)
{
  // Is playing if is buffering or is playing
  if (isNS4 || isOpera)
  {
    var playState = -1;
    try
    {
      playState = player.GetPlayState();
    }
    catch (e)
    {
    }

    return (playState == 2 ||
            playState == 3);
  }
  else
  {
    var playState = -1;
    try
    {
      playState = player.PlayState;
    }
    catch (e)
    {
    }

    return (playState == 2 ||
            playState == 3);
  }
}

function control_isPlayerStopped(player)
{
  // Is playing if is buffering or is playing
  if (isNS4 || isOpera)
  {
    var playState = -1;
    try
    {
      playState = player.GetPlayState();
    }
    catch (e)
    {
    }

    return (playState == 0);
  }
  else
  {
    var playState = -1;
    try
    {
      playState = player.PlayState;
    }
    catch (e)
    {
    }

    return (playState == 0);
  }
}

function control_isPlayerPaused(player)
{
  if (isNS4 || isOpera)
  {
    var playState = -1;
    try
    {
      playState = player.GetPlayState();
    }
    catch (e)
    {
    }

    return (playState == 1);
  }
  else
  {
    var playState = -1;
    try
    {
      playState = player.PlayState;
    }
    catch (e)
    {
    }

    return (playState == 1);
  }
}
  

function control_isPlaying()
{
  return (control_isPlayerPlaying(control_getMasterPlayer()));
}

function control_getTimeMs()
{
  return g_lastPositionMs;
}

function control_seekTime(valueMs)
{
  g_controlIsSeeking = true;
  var masterPlayer = control_getMasterPlayer();
  
  try
  {
    // Progressive Download?
    if ((g_controlIsProgressiveDownload) && (masterPlayer.Bandwidth > 0))
    {
      if (g_controlIsSliding)
      {
        if (!g_controlHasWarnedProgressiveDownloadOnce)
        {
          g_controlHasWarnedProgressiveDownloadOnce = true;
          if (g_controlIsWmp9Plugin)
            alert(g_wmProgressiveDownloadAlert);
          else
            alert(g_wmProgressiveDownloadWmp64);
  
          return;
        }
      }
      else
      {
        if (g_controlIsWmp9Plugin)
          alert(g_wmProgressiveDownloadAlert);
        else
          alert(g_wmProgressiveDownloadWmp64);
  
        return;
      }
    }
  }
  catch (e)
  {
  }
 
  var wasPlaying = control_isPlayerPlaying(masterPlayer);
  if (wasPlaying)
    control_stop();

  g_pausedPositionValid = false;
  layout_updateTime(valueMs);
  control_notifyCurrentTime(valueMs);
  if (wasPlaying)
    setTimeout("control_playPause()", 200);
  if (g_startReplayAfterStopmark)
  {
    g_startReplayAfterStopmark = false;
    setTimeout("control_play()", 200);
  }

  g_controlIsSeeking = false;
}

function control_checkPlayers()
{
  g_controlPlayerChecked++;

  if (isNS4)
    return;

  if (!isIE)
  {
    var masterPlayer = control_getMasterPlayer();
    var success = false;
    if (masterPlayer != null)
    {
       var playFunc = masterPlayer.Play;
       if (playFunc != null)
          success = true;
    }

    if (!success)
    {
      // That went wrong
      if (g_controlPlayerChecked < 5)
        setTimeout("control_checkPlayers()", 200);
      else
      {
//        alert(g_playerWarningMessage);
        control_lookForGeckoActiveXControl();

        layout_enableControl("play", false);
        layout_enableControl("end", false);
        layout_enableControl("home", false);
        layout_enableControl("back", false);
        layout_enableControl("forward", false);
        layout_enableControl("mute", false);
      }
    }
  }
  else
  {
    g_controlPlayerChecked = 1;
  }
}

function control_onSync()
{
  if (g_controlPlayerChecked == 0)
  {
    control_checkPluginAndUrl();
    control_checkPlayers();
  }

  // Get the master player        
  var masterPlayer = control_getMasterPlayer();
  
  // Check, if there is interaction at the very end
  if (control_isPlayerStopped(masterPlayer) && (g_lastPositionMs > (g_mediaDurations[0]-255)))
  {
    var valueMs = -1;
    for (var i = (g_stopmarkTimes.length-1); i >= 0; --i)
    {
      if (g_stopmarkTimes[i] > g_mediaDurations[0]-255)
      {
        valueMs = g_stopmarkTimes[i];
        break;
      }
    }
    if (valueMs < 0)
    {
      for (var k = (g_interactionEntries.length-1); k >= 0; --k)
      {
        if (g_interactionEntries[k].timeActiveEnd > g_mediaDurations[0]-255)
        {
          valueMs = g_interactionEntries[k].timeActiveEnd;
          break;
        }
      }
    }
    if (valueMs >= 0)
    {
      control_setCurrentPosition(masterPlayer, valueMs);
      control_notifyCurrentTime(valueMs)
    }
  }
  
  // Is invoked every 250 ms.
  if (g_playerState != 2) // not playing currently
    return;

  if (g_controlIsProgressiveDownload)
  {
    try
    {
      if (masterPlayer.Bandwidth > 0)
        layout_displayIsDownloading(true);
    }
    catch (e)
    {
    }
  }
  
  // We think we're playing. Are we?
  if (!control_isPlayerPlaying(masterPlayer))
  {
    // Are we syncing the master to a clip?
    if (g_masterSyncingToClip >= 0)
    {
      var clipPlayer = control_getClipPlayer(g_masterSyncingToClip);
      var bufferingProgress = clipPlayer.BufferingProgress;
      if (bufferingProgress < 100)
        layout_displayIsBuffering(true, bufferingProgress);
      // Check that clip's time.
      var clipTimeMs = g_mediaStartTimes[g_masterSyncingToClip + 1] + control_getCurrentPosition(clipPlayer);
      var currentTimeMs = control_getCurrentPosition(masterPlayer);
      if (clipTimeMs >= currentTimeMs - MEDIA_THRESHOLD)
      {
        // The clip player has caught up, restart master player
        try
        {
          masterPlayer.Play();
        }
        catch (e)
        {
        }
        g_masterSyncingToClip = -1;

        layout_displayIsBuffering(false, 0);
        layout_setPlayIsPause(true);
        layout_enableControl("play", true);
        return;
      }
      else
      {
        return;
      }
    }
    else
    {
      // No sync, something has gone wrong.
      // Emergency stop
      control_stop();
      return;
    }
  }
  
  // Are we syncing a clip to the master?
  if (g_clipSyncingToMaster >= 0)
  {
    var clipPlayer = control_getClipPlayer(g_clipSyncingToMaster);
    // Check that clip's time.
    var clipTimeMs = g_mediaStartTimes[g_clipSyncingToMaster + 1] + control_getCurrentPosition(clipPlayer);
    var currentTimeMs = control_getCurrentPosition(masterPlayer);
    if (currentTimeMs >= clipTimeMs - MEDIA_THRESHOLD)
    {
      // The master player has caught up, restart clip player
      try  
      {
        clipPlayer.Play();
      }
      catch (e)
      {
      }
      // For NS4:
      // control_setVisible(clipPlayer, g_clipSyncingToMaster, true);
      g_clipSyncingToMaster = -1;

      layout_displayIsBuffering(false, 0);
      layout_setPlayIsPause(true);
      layout_enableControl("play", true);
      return;
    }
    else
    {
      return;
    }
  }
  
  // What time is it?
  var currentTimeMs = control_getCurrentPosition(masterPlayer);
  var visiblePlayer = control_notifyCurrentTime(currentTimeMs);
  layout_updateTime(currentTimeMs);
  ///slides_notifyNewMediaPlayerPosition(currentTimeMs);

  // Were we buffering but aren't anymore?
  if (g_startPosition != -1 && g_startPosition != currentTimeMs)
  {
    g_startPosition = -1;
    layout_displayIsBuffering(false, 0);
    layout_enableControl("play", true);
    //getVideoFrameObject('statusDisplay').innerHTML = '';
  }
  
  // Is there a player visible right now?
  if (visiblePlayer >= 0)
  {
    var clipPlayer = control_getClipPlayer(visiblePlayer);

    var bufferingProgress = clipPlayer.BufferingProgress;
    if (bufferingProgress < 100)
      layout_displayIsBuffering(true, bufferingProgress);
    else
      layout_updateTime(currentTimeMs);

    // Now check sync on clip player
    if (control_isPlayerPlaying(clipPlayer))
    {
      var clipPos = g_mediaStartTimes[visiblePlayer + 1] + control_getCurrentPosition(clipPlayer);
      var deltaMs = currentTimeMs - clipPos;
      if (Math.abs(deltaMs) > MEDIA_THRESHOLD)
      {
        layout_enableControl("play", false);

        // We're beyond the Media threshold, do something
        if (deltaMs > 0)
        {
          // We need to pause the master player to let
          // the clip player catch up.
          g_masterSyncingToClip = visiblePlayer;
          try
          {
            masterPlayer.Pause();
          }
          catch (e)
          {
            alert("Error: Could not pause Master Player. Stopping.");
            control_stop();
          }

        }
        else
        {
          // The master player is behind, let's pause
          // the clip player for a while
          g_clipSyncingToMaster = visiblePlayer;
          try
          {
            clipPlayer.Pause();
          }
          catch (e)
          {
            alert("Error: Could not pause Clip Player. Stopping.");
            control_stop();
          }
        }
      }
    }
    else
    {
      control_setCurrentPosition(clipPlayer, currentTimeMs - g_mediaStartTimes[visiblePlayer + 1]);
      try
      {
        clipPlayer.Play();
        if (clipPlayer.HasError)
        {
          alert(clipPlayer.ErrorDescription + "\n\n" + clipPlayer.FileName);
          control_stop();
        }
      }
      catch (e)
      {
        alert("Error: Could not start Clip Player. Stopping.");
        control_stop();
      }
      // For NS4:
      // control_setVisible(clipPlayer, visiblePlayer, true);
    }
  }
}

function control_setVolume(relVol)
{
  var targVolume = -10000;
  if (relVol > 0)
  {
    var logVol = Math.log(relVol / 100.0);
    if (logVol > -4)
      targVolume = Math.ceil(logVol * 2500);
  }
  
  var masterPlayer = control_getMasterPlayer(); //document.getElementById('MasterPlayer');
  try
  {
    if (isNS4 || isOpera)
      masterPlayer.SetVolume(targVolume);
    else
      masterPlayer.Volume = targVolume;
  }
  catch (e)
  {
  }
}

function control_getVolume()
{
  var masterPlayer = control_getMasterPlayer(); //document.getElementById('MasterPlayer');
  var playerVol = 0;
  try
  {
    if (isNS4 || isOpera)
      playerVol = masterPlayer.GetVolume();
    else
      playerVol = masterPlayer.Volume;
  }
  catch (e)
  {
  }

  var volValue = 0;
  if (playerVol > -10000)
    volValue = Math.round(100.0 * Math.exp(playerVol / 2500));

  return volValue;
}

function control_mute()
{
  if (g_controlIsMuted)
    control_muteOff();
  else
    control_muteOn();
}

function control_muteOn()
{
  g_controlVolumeBackup = control_getVolume();
  control_setVolume(0);
  g_controlIsMuted = true;

  layout_setIsMuted(g_controlIsMuted);
}

function control_muteOff()
{
  g_controlIsMuted = false;
  control_setVolume(g_controlVolumeBackup);

  layout_setIsMuted(g_controlIsMuted);
}

function control_lookForGeckoActiveXControl()
{
   var ua = navigator.userAgent;

   // Netscape/Mozilla/Firefox are Gecko browser
   var isGeckoBrowser = (ua.indexOf("Gecko") > -1);
   
   if (!isGeckoBrowser)
   {
     alert(g_playerWarningMessage);
     return;
   }
     
   var isNetscape = (ua.indexOf("Netscape") > -1);
   var isFirefox = (ua.indexOf("Firefox") > -1);

   var doOpenUrl = false;
   
   if (isNetscape)
   {
     if ( (ua.indexOf("Netscape/7.0") > -1) || (ua.indexOf("Netscape/6.") > -1) )
     {
       alert(g_wmNetscapeLess71);
     }
     else if (ua.indexOf("Netscape/8.") > -1)
     {
       alert(g_wmNetscape8);
     }
   }
   else if (isFirefox)
   {
     if (ua.indexOf("Firefox/2.0") > -1)
     {
       doOpenUrl = confirm("Firefox 2.0 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-15.xpi");
     }
     else if (ua.indexOf("Firefox/1.5") > -1)
     {
       doOpenUrl = confirm("Firefox 1.5 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-15.xpi");
     }
     else if (ua.indexOf("Firefox/1.0.7") > -1)
     {
       doOpenUrl = confirm("Firefox 1.0.7 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-107.xpi");
     }
     else if (ua.indexOf("Firefox/1.0.6") > -1)
     {
       doOpenUrl = confirm("Firefox 1.0.6 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-106.xpi");
     }
     else if (ua.indexOf("Firefox/1.0.4") > -1)
     {
       doOpenUrl = confirm("Firefox 1.0.4 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-104.xpi");
     }
     else if (ua.indexOf("Firefox/1.0.3") > -1)
     {
       doOpenUrl = confirm("Firefox 1.0.3 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-103.xpi");
     }
     else if (ua.indexOf("Firefox/1.0.2") > -1)
     {
       doOpenUrl = confirm("Firefox 1.0.2 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("Firefox/1.0.1") > -1)
     {
       doOpenUrl = confirm("Firefox 1.0.1 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if ( (ua.indexOf("Firefox/1.0") > -1) && !(ua.indexOf("Firefox/1.0.") > -1) )
     {
       doOpenUrl = confirm("Firefox 1.0 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("Firefox/1.0RC2") > -1)
     {
       doOpenUrl = confirm("Firefox 1.0 RC2" + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("Firefox/1.0RC1") > -1)
     {
       doOpenUrl = confirm("Firefox 1.0 RC1" + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("Firefox/0.") > -1)
     {
       alert(g_wmFirefoxLess10);
     }
     else
     {
       // unknown Firefox version
       doOpenUrl = confirm("Firefox: " + g_wmActivexControlUnknown + g_wmActivexControlWarning + g_wmActivexControlDownload);
     }

     if (doOpenUrl == true)
       top.location.href = "http://www.iol.ie/~locka/mozilla/plugin.htm#download";
   }
   else
   {
     // assuming Mozilla

     if (ua.indexOf("rv:1.8b") > -1)
     {
       alert("Mozilla 1.8 Beta " + g_wmBrowserNotSupported);
     }
     else if (ua.indexOf("rv:1.8a") > -1)
     {
       alert("Mozilla 1.8 Alpha " + g_wmBrowserNotSupported);
     }
     else if (ua.indexOf("rv:1.7.13") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7.13 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("rv:1.7.12") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7.12 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("rv:1.7.11") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7.11 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("rv:1.7.10") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7.10 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("rv:1.7.8") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7.8 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("rv:1.7.7") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7.7 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("rv:1.7.6") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7.6 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("rv:1.7.5") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7.5 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex-ff-10.xpi");
     }
     else if (ua.indexOf("rv:1.7.3") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7.3 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex17.xpi");
     }
     else if (ua.indexOf("rv:1.7.2") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7.2 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex17.xpi");
     }
     else if ( (ua.indexOf("rv:1.7.1)") > -1) && !(ua.indexOf("rv:1.7.10") > -1) && !(ua.indexOf("rv:1.7.11") > -1) && !(ua.indexOf("rv:1.7.12") > -1) )
     {
       doOpenUrl = confirm("Mozilla 1.7.1 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex17.xpi");
     }
     else if ( (ua.indexOf("rv:1.7)") > -1) && !(ua.indexOf("rv:1.7.") > -1) )
     {
       doOpenUrl = confirm("Mozilla 1.7 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex17.xpi");
     }
     else if (ua.indexOf("rv:1.7b") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.7 Beta " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex17.xpi");
     }
     else if (ua.indexOf("rv:1.7a") > -1)
     {
       alert("Mozilla 1.7 Alpha " + g_wmBrowserNotSupported);
     }
     else if (ua.indexOf("rv:1.6)") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.6 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex16.xpi");
     }
     else if (ua.indexOf("rv:1.6b") > -1)
     {
       alert("Mozilla 1.6 Beta " + g_wmBrowserNotSupported);
     }
     else if (ua.indexOf("rv:1.6a") > -1)
     {
       alert("Mozilla 1.6 Alpha " + g_wmBrowserNotSupported);
     }
     else if (ua.indexOf("rv:1.5") > -1)
     {
       alert("Mozilla 1.5 " + g_wmBrowserNotSupported);
     }
     else if (ua.indexOf("rv:1.4.1)") > -1)
     {
       doOpenUrl = confirm("Mozilla 1.4.1 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex14.xpi");
     }
     else if ( (ua.indexOf("rv:1.4)") > -1) && !(ua.indexOf("rv:1.4.)") > -1) )
     {
       doOpenUrl = confirm("Mozilla 1.4 " + g_wmActivexControlRequired + g_wmActivexControlDownload + "nmozactivex14.xpi");
     }
     else if ( (ua.indexOf("rv:1.3") > -1)  || (ua.indexOf("rv:1.2") > -1)  || (ua.indexOf("rv:1.1") > -1)  || (ua.indexOf("rv:1.0") > -1)  || (ua.indexOf("rv:0.") > -1) )
     {
       alert(g_wmMozillaLess16);
     }
     else
     {
       // unknown Mozilla version
       doOpenUrl = confirm("Mozilla: " + g_wmActivexControlUnknown + g_wmActivexControlWarning + g_wmActivexControlDownload);
     }

     if (doOpenUrl == true)
       top.location.href = "http://www.iol.ie/~locka/mozilla/plugin.htm#download";
   }
}

function control_getType()
{
  return "WM";
}

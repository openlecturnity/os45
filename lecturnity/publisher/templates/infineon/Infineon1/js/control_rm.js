var MEDIA_THRESHOLD = 300; // ms

var g_controlPlayerChecked = 0;
var g_controlPlayerOk = true;
var g_controlErrorIssued = false;

var g_lastPositionMs = 0;

// 0: Stopped
// 1: Paused
// 2: Playing
var g_playerState = 0;
var g_masterSyncingToClip = -1;
var g_clipSyncingToMaster = -1;
var g_currentClipPlayer   = -1;

var g_pausedPositionValid = true;

var g_startPosition = 0;

var g_clipPlayers = new Array();
var g_clipPlayerStyles = new Array();

var g_masterPlayer = null;
var g_controlIsSliding = false;
var g_controlWasPlaying = false;

var g_controlIsMuted = false;
var g_controlVolumeBackup = -1;

function control_getMasterPlayer()
{
  if (g_masterPlayer)
    return g_masterPlayer;
  
  if (isNS4)
    g_masterPlayer = document.PlayerImageWindow; 
  else
    g_masterPlayer = getHtmlObject("", "ImageWindowLayer", "PlayerImageWindow");

  return g_masterPlayer;
}

function control_getCurrentPosition(player)
{
  return player.GetPosition();
}

function control_setCurrentPosition(player, msTime)
{
  player.SetPosition(msTime);
}

function controlInit()
{
  var clipsLeft = layout_getSlidesLeft();
  var clipsTop = layout_getSlidesTop();

  for (var i = 0; i < g_numOfClips; ++i)
  {
    if (isNS4 && false)
    {
      g_clipPlayerStyles[i] = getHtmlObject(slidesDivPre, ("ClipLayer" + i), ("ClipImageWindow" + i), true, true);
      g_clipPlayers[i] = getHtmlObject(slidesDivPre, ("ClipLayer" + i), ("ClipImageWindow" + i));
    }
    else
    {
      g_clipPlayerStyles[i] = getHtmlObject("", ("ClipLayer" + i), ("ClipImageWindow" + i), true, true);
      g_clipPlayers[i] = getHtmlObject("", ("ClipLayer" + i), ("ClipImageWindow" + i));
    }

    var deltaW = (g_slidesWidth - g_clipEntries[i].width) / 2;
    var deltaH = (g_slidesHeight - g_clipEntries[i].height) / 2;
    if (deltaW < 0)
      deltaW = 0;
    if (deltaH < 0)
      deltaH = 0;
    g_clipPlayerStyles[i].left = clipsLeft + deltaW;
    g_clipPlayerStyles[i].top  = clipsTop + deltaH;
  }

  layout_setPlayIsPause(false);
  layout_enableControl("play", true);
  
  if (g_controlPlayerChecked == 0)
    control_checkPlayers();
  
  //The next items are called from control_checkPlayers() on success
  //setInterval("control_onSync()", 200);
  //setTimeout("control_notifyCurrentTime(0)", 500);
}

function control_isPlayerVisible(player, playerNumber)
{
  var clipPlayer = g_clipPlayerStyles[playerNumber];
  return (clipPlayer.visibility != 'hidden' &&
          clipPlayer.visibility != 'hide');
}

function control_setVisible(player, playerNumber, bVisible)
{
  var clipPlayer = g_clipPlayerStyles[playerNumber];
  if (bVisible)
    clipPlayer.visibility = 'visible';
  else
    clipPlayer.visibility = 'hidden';
}

function control_notifyCurrentTime(valueMs)
{
  if (g_lastPositionMs != valueMs)
  {
    g_lastPositionMs = valueMs;
    g_pausedPositionValid = false;
  }
  
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
    var clipPlayer = g_clipPlayerStyles[i];
    if (i == playerToBeVisible)
    {
      if (clipPlayer.visibility != 'visible' && clipPlayer.visibility != 'show')
        clipPlayer.visibility = 'visible';
    }
    else
    {
      if (clipPlayer.visibility != 'hidden' && clipPlayer.visibility != 'hide')
        clipPlayer.visibility = 'hidden';
    }
  }

  layout_updateTime(valueMs);
  g_currentClipPlayer = playerToBeVisible;

  // this method is in slides.js
  slides_notifyNewMediaPlayerPosition(valueMs);
  
  return playerToBeVisible;
}

function control_beginSlide()
{
  g_controlIsSliding = true;
  g_controlWasPlaying = control_isPlaying();
  if (g_controlWasPlaying)
    control_playPause();
}

function control_endSlide()
{
  g_controlIsSliding = false;
  if (g_controlWasPlaying)
    control_playPause();
}

function control_delayedJump(player, timeMs)
{
  if (control_isPlayerPlaying(player))
  {
    // You can only reliably set the current position
    // if the RealPlayer is really playing.
    control_setCurrentPosition(player, timeMs);
  }
  else
  {
    if (control_isPlayerStopped(player))
    {
      // Something has gone wrong. Stop everything and
      // restart.
      control_stop();
      control_playPause();
    }
    else
    {
      // Player is probably buffering or loading,
      // let's wait another while.
      setTimeout("control_delayedJump(g_masterPlayer, " + timeMs + ")", 250);
    }
  }
}

function control_playPause()
{
  if (g_playerState != 2)
  {
    g_playerState = 2;

    var positionMs = g_lastPositionMs;
    var masterPlayer = control_getMasterPlayer();

    masterPlayer.DoPlay();
    // It is not always so good to do a play and immediately
    // after that a SetPosition. Let's delay that a bit if necessary.
    if (!g_pausedPositionValid)
      setTimeout("control_delayedJump(g_masterPlayer, " + positionMs + ")", 500);
      //setTimeout("control_setCurrentPosition(g_masterPlayer, " + positionMs + ")", 1000);

    layout_setPlayIsPause(true);
  }
  else
  {
    var masterPlayer = control_getMasterPlayer();
    if (control_isPlayerPlaying(masterPlayer))
    {
      g_playerState = 1;

      masterPlayer.DoPause();
      layout_setPlayIsPause(false);
      g_pausedPositionValid = true;
    }
    else if (control_isPlayerStopped(masterPlayer))
    {
      // We think we're playing, but we're stopped, actually.
      // This must mean we have reached the end of the stream
      // or something else, likewise nasty.
      g_lastPositionMs = 0;
      g_pausedPositionValid = true;
      g_playerState = 0;
  
      layout_setPlayIsPause(false);
      layout_enableControl("play", true);

      control_notifyCurrentTime(0);
      slides_notifyNewMediaPlayerPosition(0);
    }
    // 1.7.0.p1: Ignore Pause if not playing at the moment.
    //    else
    //    {
    //  // Emergency stop
    //      control_stop();
    //    }
  }
}

function control_stop()
{
  var masterPlayer = control_getMasterPlayer();
  masterPlayer.DoStop();

  g_playerState = 0;
  
  layout_enableControl("stop", false);
  layout_enableControl("play", true);
  layout_setPlayIsPause(false);

  return;
}

function control_isPlayerPlaying(player)
{
  return (player.GetPlayState() == 3);
}

function control_isPlaying()
{
  return (control_isPlayerPlaying(control_getMasterPlayer()));
}

function control_isPlayerStopped(player)
{
  return (player.GetPlayState() == 0);
}

function control_isStopped()
{
  return (control_isPlayerStopped(control_getMasterPlayer()));
}

function control_getTimeMs()
{
  return g_lastPositionMs;
}

function control_delayedSeek(valueMs)
{
  var masterPlayer = control_getMasterPlayer();

  if (!g_controlIsSliding && control_isPlaying())
    control_setCurrentPosition(masterPlayer, valueMs);
}

function control_seekTime(valueMs)
{
  var masterPlayer = control_getMasterPlayer();

  if (!g_controlIsSliding && control_isPlaying())
  {
    // Delay the seek for half a second to check if this seekTime comes
    // from a beginning slide event.
    setTimeout("control_delayedSeek(" + valueMs + ")", 500);
  }
  else
  {
    layout_updateTime(valueMs);
    control_notifyCurrentTime(valueMs);
  }
}

function control_checkPlayers()
{
  g_controlPlayerChecked++;

  if (isNS4)
    return;

  try
  {
    control_stop();

    var masterPlayer = control_getMasterPlayer();

    // Let us turn off errors display in message boxes.
    // We will handle errors by ourselves.
    masterPlayer.SetWantErrors(true);

    // Check for HTTPS protocol. This is not supported by the RealPlayer. 
    // Warn, if HTTPS is used.
    var playerUrl = "" + masterPlayer.GetSource();
    var playerProtocol = playerUrl.toLowerCase().substr(0,5);
    if ((playerProtocol.indexOf("https") > -1) && (g_controlPlayerChecked == 1))
      alert(g_realHttpsWarning + playerUrl);
    
    setInterval("control_onSync()", 200);
    setTimeout("control_notifyCurrentTime(0)", 500);
  }
  catch (err)
  {
    // That went wrong
    if (g_controlPlayerChecked < 5)
      setTimeout("control_checkPlayers()", 500);
    else
    {
      g_controlPlayerOk = false;
      alert(g_playerWarningMessage);
      layout_enableControl("play", false);
      layout_enableControl("end", false);
      layout_enableControl("home", false);
      layout_enableControl("back", false);
      layout_enableControl("forward", false);
      layout_enableControl("mute", false);
    }
  }
}

function control_onSync()
{
  if (g_controlPlayerOk)
  {
    // Let's do some error checking
    try
    {
      var masterPlayer = control_getMasterPlayer();
      var errorCode = masterPlayer.GetLastErrorRMACode();
      if (errorCode != 0 && !g_controlErrorIssued)
      {
        if (errorCode == PNR_DOC_MISSING)
          alert(g_realPnrDocMissing + masterPlayer.GetSource());
        else if (errorCode == PNR_NET_CONNECT)
          alert(g_realPnrNetConnect + masterPlayer.GetSource());
        else if (errorCode == PNR_INVALID_HOST)
          alert(g_realPnrInvalidHost + masterPlayer.GetSource());
        else if (errorCode == PNR_SERVER_ALERT)
          alert(g_realPnrServerAlert + masterPlayer.GetSource());
        else if (errorCode == PNR_DNR) // (DNS)
          alert(g_realPnrDnr + masterPlayer.GetSource());
        else if (errorCode == PNR_SERVER_TIMEOUT)
          alert(g_realPnrServerTimeout + masterPlayer.GetSource());
        else if (errorCode == PNR_FAIL)
          alert(g_realPnrFail + masterPlayer.GetSource());
        else // unknown error
          alert(g_realUnknownError + errorCode);

        // In some cases, replay will work after a server alert anyway
        if (errorCode != g_realPnrServerAlert)
          g_controlErrorIssued = true;
      }
    }
    catch (err)
    {
      if (!g_controlErrorIssued)
      {
        alert(g_realPluginCheckError);
        g_controlErrorIssued = true;
      }
    }
  }

  // Is invoked every 250 ms.
  if (g_playerState != 2) // not playing currently
    return;

  var masterPlayer = control_getMasterPlayer();
  if (control_isPlayerPlaying(masterPlayer))
  {
    var currentTimeMs = control_getCurrentPosition(masterPlayer);
    var visiblePlayer = control_notifyCurrentTime(currentTimeMs);
    layout_updateTime(currentTimeMs);
    slides_notifyNewMediaPlayerPosition(currentTimeMs);
    return;
  }
}

function control_getVolume()
{
  if (g_controlIsMuted)
  {
    return g_controlVolumeBackup;
  }
  else
  {
    var masterPlayer = control_getMasterPlayer();
    return masterPlayer.GetVolume();
  }
}

function control_setVolume(volume)
{
  if (!g_controlIsMuted)
  {
    var masterPlayer = control_getMasterPlayer();
    return masterPlayer.SetVolume(volume);
  }
  else
  {
    g_controlVolumeBackup = volume;
  }
}

function control_mute()
{
  if (g_controlIsMuted)
  {
    g_controlIsMuted = false;
    control_setVolume(g_controlVolumeBackup);
  }
  else
  {
    g_controlVolumeBackup = control_getVolume();
    control_setVolume(0);
    g_controlIsMuted = true;
  }

  layout_setIsMuted(g_controlIsMuted);
}

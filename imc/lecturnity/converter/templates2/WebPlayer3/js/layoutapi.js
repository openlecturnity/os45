function layout_enableControl(btId, bEnable)
{
  // We do not have a stop button
  if (btId == "stop")
    return;
  // Are the navigation buttons disabled (1) or hidden (2)?
  if ((g_layoutNavigationStandardButtons > 0) && (btId != "mute"))
    return;

  button_enableButton("layout" + btId, bEnable);
}

function layout_setPlayIsPause(bPlayIsPause)
{
  if (bPlayIsPause)
    button_setButtonState("layoutplay", "1");
  else
    button_setButtonState("layoutplay", "0");
}

function layout_setIsMuted(bIsMuted)
{
  if (bIsMuted)
    button_setButtonState("layoutmute", "1");
  else
    button_setButtonState("layoutmute", "0");
}

function layout_updateTime(timeMs)
{
  if (isNS4)
  {
    var curSlide = layout_getSlide(timeMs);
    var selForm = document.Form1;
    if (selForm)
    {
      if (selForm.slideoption.selectedIndex != curSlide)
	selForm.slideoption.selectedIndex = curSlide;
    }

    return;
  }

  if (!g_isSliding)
  {
    g_layoutIgnoreTimeChange = true;
    timeSlider.setValue(timeMs);
    g_layoutIgnoreTimeChange = false;
  }

  if (g_timeLabel == null)
    g_timeLabel = getHtmlObject('', 'TimeLabel', '', false, true);
  var delta = g_lastDisplayedTimeMs - timeMs;
  if (delta >= 1000 || delta <= -1000)
  {
    g_timeLabel.innerHTML = msToTime(timeMs);
    g_lastDisplayedTimeMs = timeMs;
  }

  if (!g_isStandaloneMode)
    frames['thumbsFrame'].thumbs_updateTime(timeMs);

  //document.getElementById('TimeLabel').innerHTML = msToTime(timeMs);
  //getHtmlObject('', 'TimeLabel', '', false, true).innerHTML = msToTime(timeMs);
}

function layout_getSlidesTop()
{
  if (isNS4)
    return 40;
  return 20;
}

function layout_getSlidesLeft()
{
  return g_slidesOffset;
}

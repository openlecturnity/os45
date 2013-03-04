var l_scormLoaded = false;
function scorm_init()
{
  if (g_scormEnabled)
  {
    var result = doLMSInitialize();
    if (result != "false")
    {
	l_scormLoaded = true;
        return true;
    }
    return false;
  }
  return false;
}

function scorm_unload()
{
  if (g_scormEnabled && l_scormLoaded)
  {
    doLMSFinish();
    }
}



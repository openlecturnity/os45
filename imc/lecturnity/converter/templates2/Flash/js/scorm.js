
function scorm_init()
{
  if (g_scormEnabled)
  {
    var result = doLMSInitialize();
    if (result != "false")
    {
	g_scormLoaded = true;
    }
  }
}

function scorm_unload()
{
  if (g_scormEnabled && g_scormLoaded)
  {
    scutils_Finish();
    doLMSFinish();
  }
}

scorm_init();



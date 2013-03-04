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
    doLMSFinish();
}

// This is called directly on loading the page.
// This is on purpose!
var g_scormLoaded = false;
scorm_init();

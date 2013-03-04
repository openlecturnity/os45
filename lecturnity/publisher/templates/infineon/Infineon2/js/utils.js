// browser specific stuff
var isNS4                  = false;
var isIE4                  = false;
var isDOM                  = false;
var isOpera                = false;
var pre                    = '';
var post                   = '';
var pre1                   = '';
var post1                  = '';
var pre2                   = '';
var pre2_embed             = '';
var post_embed             = '';
var post_style             = '';
var bgCol                  = '';

var detectableWithVB       = false;
var pluginFound            = false;

/* ------------------------------------------------------------------------
 * Init: Determine the browser
 * ------------------------------------------------------------------------ */
function determineBrowser()
{
  var ua = navigator.userAgent.toLowerCase();
  var netscapebrowser = ( (ua.indexOf("mozilla") != -1) 
                       && (ua.indexOf("compatible") == -1) 
                       && (ua.indexOf("opera") == -1) );
  var av = navigator.appVersion;
  var version = av.substring(0, av.indexOf(" ")) *1;
  var version2 = av.substring(0, av.indexOf(".")) *1;

  if (ua.indexOf("opera") != -1)
    isOpera = true;

  // Netscape 4.x
  if (document.layers)
  {
    isNS4          = true;
    isIE4          = false;
    isDOM          = false;
    pre            = 'document.';
    post           = '';
    pre1           = 'document.layers["';
    post1          = '';
    pre2           = '"].document.';
    pre2_embed     = '"].document.embeds["';
    post_embed     = '"]';
    post_style     = '';
    bgCol          = '.bgColor';
  }

  // Internet Explorer 4.x or higher
  if (document.all && ua.indexOf("mac")<0)
  {
    isNS4          = false;
    isIE4          = true;
    isDOM          = false;
    pre            = 'document.all.';
    post           = '';
    pre1           = 'document.all.';
    post1          = '';
    pre2           = '';
    pre2_embed     = '';
    post_embed     = '';
    post_style     = '.style';
    bgCol          = '.backgroundColor';
  }

  // DOM interpreting browser 
  if (document.documentElement)
  {
    isNS4          = false;
    isIE4          = false;
    isDOM          = true;
    pre            = 'document.getElementById("';
    post           = '")';
    pre1           = 'document.getElementsByName("';
    post1          = '")[0]';
    pre2           = '';
    pre2_embed     = '';
    post_embed     = '")[0]';
    post_style     = '.style';
    bgCol          = '.backgroundColor';
  }
}

/* ------------------------------------------------------------------------
 * PlugIn Detection
 * ------------------------------------------------------------------------ */
    
function goURL(daURL)
{
  // if the browser can do it, use replace to preserve back button
  if(javascriptVersion1_1)
  {
    window.location.replace(daURL);
  }
  else
  {
    window.location = daURL;
  }
  return;
}

function redirectCheck(pluginFound, redirectURL, redirectIfFound)
{
  // check for redirection
  if( redirectURL && ((pluginFound && redirectIfFound) || 
    (!pluginFound && !redirectIfFound)) )
  {
    // go away
    goURL(redirectURL);
    return pluginFound;
  }
  else
  {
    // stay here and return result of plugin detection
    return pluginFound;
  }	
}

function canDetectPlugins()
{
  if( detectableWithVB || (navigator.plugins && navigator.plugins.length > 0) )
  {
    return true;
  }
  else
  {
    return false;
  }
}

function detectReal(redirectURL, redirectIfFound)
{
  pluginFound = detectPlugin('RealPlayer');
  // if not found, try to detect with VisualBasic
  if(!pluginFound && detectableWithVB)
  {
    pluginFound = (detectActiveXControl('rmocx.RealPlayer G2 Control') ||
                   detectActiveXControl('RealPlayer.RealPlayer(tm) ActiveX Control (32-bit)') ||
                   detectActiveXControl('RealVideo.RealVideo(tm) ActiveX Control (32-bit)'));
  }
  return redirectCheck(pluginFound, redirectURL, redirectIfFound);
}

function detectWindowsMedia(redirectURL, redirectIfFound)
{
  pluginFound = detectPlugin('Windows Media');
  // if not found, try to detect with VisualBasic
  if(!pluginFound && detectableWithVB)
  {
    pluginFound = detectActiveXControl('MediaPlayer.MediaPlayer.1');
  }
  return redirectCheck(pluginFound, redirectURL, redirectIfFound);
}

function detectPlugin()
{
  // allow for multiple checks in a single pass
  var daPlugins = detectPlugin.arguments;
  // consider pluginFound to be false until proven true
  var pluginFound = false;
  // if plugins array is there and not fake
  if (navigator.plugins && navigator.plugins.length > 0)
  {
    var pluginsArrayLength = navigator.plugins.length;
    // for each plugin...
    for (pluginsArrayCounter=0; pluginsArrayCounter < pluginsArrayLength; pluginsArrayCounter++ )
    {
      // loop through all desired names and check each against the current plugin name
      var numFound = 0;
      for(namesCounter=0; namesCounter < daPlugins.length; namesCounter++)
      {
        // if desired plugin name is found in either plugin name or description
        if( (navigator.plugins[pluginsArrayCounter].name.indexOf(daPlugins[namesCounter]) >= 0) || 
            (navigator.plugins[pluginsArrayCounter].description.indexOf(daPlugins[namesCounter]) >= 0) )
        {
          // this name was found
          numFound++;
        }
      }
      // now that we have checked all the required names against this one plugin,
      // if the number we found matches the total number provided then we were successful
      if(numFound == daPlugins.length)
      {
        pluginFound = true;
        // if we've found the plugin, we can stop looking through at the rest of the plugins
        break;
      }
    }
  }
  return pluginFound;
} // detectPlugin
    
/* ------------------------------------------------------------------------
 * Init: Get HTML objects and styles
 * ------------------------------------------------------------------------ */
function getEmbeddedObject(frame, name)
{
  var htmlObject = null;

  var preNS4 = '';
  var postNS4 = '';
  
  if (isNS4)
  {
    preNS4 = 'embeds["';
    postNS4 = '"]';
  }

  var htmlObjectString = pre + preNS4 + name + post + postNS4;
  if (isOpera)
    htmlObjectString = 'document.' + name;

  htmlObject = eval(htmlObjectString);
  
  return htmlObject;
}

function getHtmlObject(prefix, layer, name)
{
  return getHtmlObject(prefix, layer, name, false);
}

function getHtmlObject(prefix, layer, name, bGetStyle)
{
  return getHtmlObject(prefix, layer, name, bGetStyle, false);
}

function getHtmlObject(prefix, layer, name, bGetStyle, bOnlyLayer)
{
  var htmlObject = null;
  
  var styleSuffix = '';
  if (bGetStyle)
    styleSuffix = post_style;
     
  // this should work for most browsers and things (except for the special cases below): 
  var htmlObjectString = prefix + pre1 + name + post1;
  if (isNS4)
    htmlObjectString = prefix + pre1 + layer + pre2 + name + post1;

  // only layer should be detected
  if (bOnlyLayer)
    htmlObjectString = prefix + pre + layer + post;
  
  htmlObject = eval(htmlObjectString + styleSuffix);
  
  if (htmlObject == null && isNS4 && !bOnlyLayer)
  {
    // Netscape 4.x specific special case:

    // look for an embedded player
    htmlObjectString = htmlObjectString = pre1 + layer + pre2_embed + name + post_embed;

    htmlObject = eval(htmlObjectString + styleSuffix);
  }

  return htmlObject;
} 


/* ------------------------------------------------------------------------
 * Move Objects via CSS properties "left" and "top"
 * Note: some browsers use e.g. "20" (number) and other use "20px"
 * ------------------------------------------------------------------------ */

function moveHtmlObjectToXY(objStyle, x, y)
{
  if (typeof(objStyle.left) == 'number')
  {
    objStyle.left = x;
    objStyle.top  = y;
  }
  else
  {
    objStyle.left = '' + x + 'px';
    objStyle.top  = '' + y + 'px';
  }
}

function moveHtmlObjectToX(objStyle, x)
{
  if (typeof(objStyle.left) == 'number')
  {
    objStyle.left = x;
  }
  else
  {
    objStyle.left = '' + x + 'px';
  }
}

function moveHtmlObjectToY(objStyle, y)
{
  if (typeof(objStyle.top) == 'number')
  {
    objStyle.top  = y;
  }
  else
  {
    objStyle.top  = '' + y + 'px';
  }
}

function msToTime(valueMs)
{
  var hours = Math.floor(valueMs / 3600000);
  valueMs = valueMs - hours * 3600000;
  var minutes = Math.floor(valueMs / 60000);
  valueMs = valueMs - minutes * 60000;
  var seconds = Math.floor(valueMs / 1000);
  var timeString = "" + hours + ":";
  if (minutes < 10)
    timeString += "0";
  timeString += minutes + ":";
  if (seconds < 10)
    timeString += "0";
  timeString += seconds;
  
  return timeString;
}

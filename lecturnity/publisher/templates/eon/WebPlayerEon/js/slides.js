/* ------------------------------------------------------------------------
 * The "SlideEntry" object
 * ------------------------------------------------------------------------ */
function SlideEntry(slideNumber, slideTime, url, 
                    layerIndex, x, y, width, height)
{
  this.slideNumber = slideNumber;
  this.slideTime = slideTime;
  this.url = url;
  this.layerIndex = layerIndex;
  this.x = x;
  this.y = y;
  this.width = width;
  this.height = height;
}

/* ------------------------------------------------------------------------
 * The "TpEntry" object
 * ------------------------------------------------------------------------ */
function TpEntry(tpTime, x, y, isVisible)
{
  this.tpTime = tpTime;
  this.x = x;
  this.y = y;
  this.isVisible = isVisible;
}

/* ------------------------------------------------------------------------
 * The "ThumbEntry" object
 * ------------------------------------------------------------------------ */
function ThumbEntry(title, fullTitle, timeStamp, thumbUrl)
{
  this.title = title;
  this.fullTitle = fullTitle;
  this.timeStamp = timeStamp;
  this.thumbUrl = thumbUrl;
}

/* ------------------------------------------------------------------------
 * The "ClipEntry" object
 * ------------------------------------------------------------------------ */
function ClipEntry(timeStamp, width, height)
{
  this.timeStamp = timeStamp;
  this.width = width;
  this.height = height;
}

/* ------------------------------------------------------------------------
 * The "InteractionEntry" object
 * ------------------------------------------------------------------------ */
function InteractionEntry(strId, bIsButton, bIsTestingButton, x, y, 
                          timeVisibleBegin, timeVisibleEnd, 
                          timeActiveBegin, timeActiveEnd)
{
  this.strId            = strId;
  this.bIsButton        = bIsButton;
  this.bIsTestingButton = bIsTestingButton;
  this.x                = x;
  this.y                = y;
  this.timeVisibleBegin = timeVisibleBegin;
  this.timeVisibleEnd   = timeVisibleEnd;
  this.timeActiveBegin  = timeActiveBegin;
  this.timeActiveEnd    = timeActiveEnd;
}

/* ------------------------------------------------------------------------
 * Other stuff
 * ------------------------------------------------------------------------ */
// Array of images for the slides
var slides                 = new Array();

// Objects and styles
var TelepointerStyle       = null;
var TelepointerImage       = null;
var TableMetadataStyle     = null;
var TableSlideview         = null;
var SlideViewStyle         = null;
var StillimageImage        = null;
var StillimageStyle        = null;
var ListField              = null;
var SlideAreasImage        = new Array();
var SlideAreasStyle        = new Array();
var InitImageStyle         = null;

var slidesDivPre           = '';

// Timestamps
var actualTimestamp        = 0;
var previousTimestamp      = 0;
var lastStopmarkTimestamp  = 0;

// Indices
var slideIndex             = 0;
var oldSlideIndex          = 0;
var curSlideIndex          = 0;
var curPointerIndex        = 0;
var curStopmarkIndex       = 0;
var curInteractiveIndex    = 0;

var scaleSlide = 1.0;

var g_slidesTop = 0;
var g_slidesLeft = 0;

function slidesInit()
{
  if (g_isStandaloneMode)
    return;
  
  initSlides();
  loadAllSlides();

  determineDocumentObjects();
  loadSlide(0);

  //InitImageStyle = getHtmlObject(slidesDivPre, "MessageLayer", "InitImage", true, true);
  InitImageStyle = getHtmlObject("", "MessageLayer", "InitImage", true, true);
  InitImageStyle.visibility = 'hidden';
}

/*
 * setSlidesNS4Prefix: Use this function in order to be able to address
 *   the slides divs, in case they are nested inside another div
 *   tag. This is only important if you are using Netscape 4.
 */
function setSlidesNS4Prefix(slidesPrefix)
{
  if (isNS4)
    slidesDivPre = slidesPrefix;
}

/* ------------------------------------------------------------------------
 * Init: Determine some objects and their styles from the document 
 *       (Player, Telepointer, Slidearea, etc.)
 * ------------------------------------------------------------------------ */
function determineDocumentObjects()
{
  // Telepointer
  //TelepointerImage = getHtmlObject(slidesDivPre, "TelepointerLayer", "Telepointer24");
  //TelepointerStyle = getHtmlObject(slidesDivPre, "TelepointerLayer", "Telepointer24", true, true);
  TelepointerImage = getHtmlObject("", "TelepointerLayer", "Telepointer24");
  TelepointerStyle = getHtmlObject("", "TelepointerLayer", "Telepointer24", true, true);
  oldTpWidth = TelepointerImage.width;

  g_slidesTop = layout_getSlidesTop();
  g_slidesLeft = layout_getSlidesLeft();

  //alert("g_slidesLeft/Top: (" + g_slidesLeft + ", " + g_slidesTop + ")");

  // Slide images
  for (i = 0; i <= maxLayerIndex; i++)
  {
    //SlideAreasImage[i] = getHtmlObject(slidesDivPre, ("SlideareaLayer"+i), ("Slidefield"+i));
    //SlideAreasStyle[i] = getHtmlObject(slidesDivPre, ("SlideareaLayer"+i), ("Slidefield"+i), true, true);
    SlideAreasImage[i] = getHtmlObject("", ("SlideareaLayer"+i), ("Slidefield"+i));
    SlideAreasStyle[i] = getHtmlObject("", ("SlideareaLayer"+i), ("Slidefield"+i), true, true);

    var slideEntry = slides_findSlideEntry(i);
    SlideAreasStyle[i].left = g_slidesLeft + g_slideEntries[slideEntry].x;
    SlideAreasStyle[i].top = g_slidesTop + g_slideEntries[slideEntry].y;
  }
}

function slides_findSlideEntry(areaNr)
{
  var slideEntry = -1;
  
  for (var index = 0; index<g_slideEntries.length && (slideEntry == -1); ++index)
  {
    if (g_slideEntries[index].layerIndex == areaNr)
      slideEntry = index;
  }

  return slideEntry;
}

/* ------------------------------------------------------------------------
 * Functions for slides
 * ------------------------------------------------------------------------ */
function initSlides()
{
  for(i = 0; i < g_slideEntries.length; i++)
    slides[i] = new Image();
}

function loadAllSlides()
{
  var percent = 0;
  for(i = 0; i < g_slideEntries.length; i++)
    slides[i].src = 'slides/' + g_slideEntries[i].url;
}

function loadSlide(position)
{
  // set visited pages
  g_visitedPages[g_slideEntries[position].slideNumber] = "true";
  // add interactions to LMS
  if(g_LMSResumeSupported)
  {
    var pageNumber = g_slideEntries[position].slideNumber+1;
    var qIdx = getQuestionIndex(pageNumber);
    if (qIdx >= 0)
    {
      if( g_questionEntries[qIdx].nQuestionAddedLMS == "false")
      {
        g_questionEntries[qIdx].nQuestionAddedLMS = true;
        checkMultipleChoice(pageNumber);
        checkDragAndDrop(pageNumber) ;
        checkFillInBlank(pageNumber) ;
	if(g_scormEnabled && g_scormLoaded && g_LMSResumeSupported)
	{
           var t_interactionPattern = scutils_makeInteractionPattern(pageNumber, g_interactionType);
           var rightNow = new Date();	   
           scutils_addInteraction(g_questionEntries[qIdx].strId, g_interactionType,rightNow ,g_questionEntries[qIdx].nAchievablePoints, t_interactionPattern);
	}
      }
    }
  }

  // Hide the Telepointer
  TelepointerStyle.visibility = "hidden";

  oldSlideIndex = slideIndex;
  slideIndex = position;
  
  if (slides[position].src == "")
    slides[position].src = g_slideEntries[position].url;
 
  var layerIndex = g_slideEntries[position].layerIndex;
  // Layer index 0 ==> full slide
  if (layerIndex == 0)
  {
    // Clear all slide layers on top of this layer
    for (i = 1; i <= maxLayerIndex; i++)
    {
      SlideAreasStyle[i].visibility = "hidden";
    }

    SlideAreasImage[0].src = slides[position].src;
  }
  else
  {
    var sldNumber  = g_slideEntries[position].slideNumber;
    for (i = 0; i < g_slideEntries.length; i++)
    {
      if (g_slideEntries[i].layerIndex > 0)
      {
        if (g_slideEntries[i].slideNumber == sldNumber)
        {
          if (g_slideEntries[i].slideTime <= actualTimestamp)
          {
            SlideAreasStyle[g_slideEntries[i].layerIndex].visibility = "visible";
          }
          else
          {
            SlideAreasStyle[g_slideEntries[i].layerIndex].visibility = "hidden";
          }
        }
        else
        {
          SlideAreasStyle[g_slideEntries[i].layerIndex].visibility = "hidden";
        }
      }
      else
      {
        if (g_slideEntries[i].slideNumber == sldNumber)
        {
          SlideAreasImage[0].src = slides[i].src;
        }
      }
    }  
  }
}

function slides_notifyNewMediaPlayerPosition(newPosition)
{
  if (g_hasTesting)
  {
    if((newPosition != actualTimestamp) && (g_bFeedbackMessageVisible))
      testing_closeFeedbackMessage();
  }
  previousTimestamp = actualTimestamp;
  actualTimestamp = newPosition;

  if (g_isStandaloneMode)
    return;
  if(g_scormEnabled && g_scormLoaded)
     scutils_setLocation(actualTimestamp);  

///  if (actualTimestamp < previousTimestamp)
    resetCurrentIndices();

  var bStopmarkFound = false;
  if ((control_isPlaying()) && (!g_controlIsSliding) && (!g_controlIsSeeking) && (!g_controlIsStoppedSeeking))
    bStopmarkFound = checkForStopmark(previousTimestamp, actualTimestamp);
    
  if (bStopmarkFound)
    return;

  setSlide(actualTimestamp);
  setTP(actualTimestamp);
  
  checkForInteractiveObjects(actualTimestamp);
}

function setSlide(msTime)
{
  var n = g_slideEntries.length - 1;
  for (i = curSlideIndex; i < n; i++)
  {
    if ((msTime >= g_slideEntries[i].slideTime) && (msTime < g_slideEntries[i+1].slideTime))
    {
      if (i != slideIndex)
      {
        loadSlide(i);
        curSlideIndex = i;
        return;
      }
    }
    if (msTime < g_slideEntries[i].slideTime)
      break;
  }

  if (msTime < g_slideEntries[0].slideTime)
  {
    if (0 != slideIndex)
    {
      loadSlide(0);
      curSlideIndex = 0;
    }
  }

  if (msTime >= g_slideEntries[n].slideTime)
  {
    if (n != slideIndex)
    {
      loadSlide(n);
      curSlideIndex = n;
    }
  }
}

function setTP(msTime)
{
  var n = g_tpEntries.length - 1;
  for (i = curPointerIndex; i < n; i++)
  {
    if ((msTime >= g_tpEntries[i].tpTime) && (msTime < g_tpEntries[i+1].tpTime))
    {
      showTP(i);
      curPointerIndex = i;
      break;
    }
    if (msTime < g_tpEntries[i].tpTime)
      break;
  }

  if (msTime >= g_tpEntries[n].tpTime)
  {
    showTP(n);
    curPointerIndex = n;
  }
}

function showTP(idx)
{
  if (g_tpEntries[idx].isVisible == true)
  {
    var tpX = g_tpEntries[idx].x + g_slidesLeft;
    var tpY = g_tpEntries[idx].y + g_slidesTop;

    moveHtmlObjectToXY(TelepointerStyle, tpX, tpY)
    TelepointerStyle.visibility = "visible";
  }
  else
    TelepointerStyle.visibility = "hidden";
}

function checkForStopmark(msPreviousTime, msActualTime)
{
  var deltaTimestamp = msActualTime - msPreviousTime; 
  if (deltaTimestamp <= 0)
  {
    lastStopmarkTimestamp = -1;
    return false;
  }

  var bStopmarkFound = false;
  var msStopmark = -1;

  var n = g_stopmarkTimes.length;
  for (i = curStopmarkIndex; i < n; ++i)
  {
    msStopmark = g_stopmarkTimes[i];

    // check last time interval
    if ((msStopmark > msPreviousTime) && (msStopmark <= msActualTime)
      && (msStopmark != lastStopmarkTimestamp))
    {
      bStopmarkFound = true;
      break;
    }
    if (msStopmark > msActualTime)
      break;
  }

  if (bStopmarkFound && (msStopmark > -1))
  {
    curStopmarkIndex = i;
    lastStopmarkTimestamp = msStopmark;
    g_controlWasPlaying = control_isPlaying();
    control_pause();
    setTimeout("control_seekTime(" + msStopmark + ")", 100);
    setTimeout("control_validatePausedPosition()", 110);
  }
  
  return bStopmarkFound;
}

function checkForInteractiveObjects(msTime)
{
  var n = g_interactionEntries.length;
  for (i = curInteractiveIndex; i < n; i++)
  {
    var strId = g_interactionEntries[i].strId;
    var strIdLayer = strId + "Layer";

    var InteractiveButtonStyle = getHtmlObject("", strIdLayer, strId, true, true);
    var bIsButton = g_interactionEntries[i].bIsButton;
    // Check visibility of interactive object
    var bIsVisible = false;
    var bIsActive = false;
    if ((msTime >= g_interactionEntries[i].timeVisibleBegin) && (msTime <= g_interactionEntries[i].timeVisibleEnd))
      bIsVisible = true;
    if ((msTime >= g_interactionEntries[i].timeActiveBegin) && (msTime <= g_interactionEntries[i].timeActiveEnd))
      bIsActive = true;

    if (bIsButton)
    {
      if (bIsVisible)
      {
        if (InteractiveButtonStyle.visibility != "visible")
          InteractiveButtonStyle.visibility = "visible";
        if (bIsActive)
        {
          // Activate button, if necessary
          if (!button_isEnabled(strId)) 
            button_enableButton(strId, true);
        }
        else
        {
          // Deactivate button, if necessary
          if (button_isEnabled(strId)) 
            button_enableButton(strId, false);
        }
      }
      else
      {
        if (InteractiveButtonStyle.visibility != "hidden")
          InteractiveButtonStyle.visibility = "hidden";
      }
    }
    else
    {
      if (bIsActive)
      {
        if (InteractiveButtonStyle.visibility != "visible")
          InteractiveButtonStyle.visibility = "visible";
      }
      else
      {
        if (InteractiveButtonStyle.visibility != "hidden")
          InteractiveButtonStyle.visibility = "hidden";
      }
    }

  }
}

function resetCurrentIndices()
{
  curSlideIndex = 0;
  curPointerIndex = 0;
  curStopmarkIndex = 0;
  curInteractiveIndex = 0;
}

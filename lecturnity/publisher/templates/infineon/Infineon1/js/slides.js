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

function ClipEntry(timeStamp, width, height)
{
  this.timeStamp = timeStamp;
  this.width = width;
  this.height = height;
}

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

var slideIndex             = 0;
var oldSlideIndex          = 0;
var curSlideIndex          = 0;
var curPointerIndex        = 0;

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
  
  for (var index = 0; i<g_slideEntries.length && (slideEntry == -1); ++index)
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
  actualTimestamp = newPosition;
	
  if (g_isStandaloneMode)
    return;
  
  resetCurrentIndices();

  setSlide(actualTimestamp);
  setTP(actualTimestamp);
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

function resetCurrentIndices()
{
  curSlideIndex = 0;
  curPointerIndex = 0;
}

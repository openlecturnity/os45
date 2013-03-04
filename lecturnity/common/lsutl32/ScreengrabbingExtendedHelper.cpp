#include "stdafx.h"
#include "ScreengrabbingExtendedHelper.h"
#include "dvdmedia.h"
#include <atlimage.h>


/**********************************************************
* Sampler to extract structuring values from video file
***********************************************************/

struct __declspec(  uuid("{71771540-2017-11cf-ae26-0020afd79767}")  ) CLSID_Sampler;

Sampler::Sampler( IUnknown* unk, HRESULT *hr) : CBaseVideoRenderer(__uuidof(CLSID_Sampler), NAME("Frame Sampler"), unk, hr) {
   m_iFrameCounter = 0;
   prevImage = NULL;
   m_paStructuringValues = new CArray<LMisc::StructuringValues>();
};

Sampler::~Sampler() {
   if(prevImage) 
      free(prevImage);
}

HRESULT Sampler::CheckMediaType(const CMediaType *media ) {    
   /*
   VIDEOINFO* vi; 
   if(!IsEqualGUID( *media->Subtype(), MEDIASUBTYPE_RGB24) || !(vi=(VIDEOINFO *)media->Format()) ) 
      return E_FAIL;
   bmih=vi->bmiHeader;
   return  S_OK;
   */

   // Bug 5338: no structure created
   // some codecs return VIDEOINFOHEADER2 instead of VIDEOINFOHEADER
   if (  !(*(media->Type()) == MEDIATYPE_Video) 
      || !IsEqualGUID( *media->Subtype(), MEDIASUBTYPE_RGB24) 
      || !((VIDEOINFO *)media->Format()) ) 
      return E_FAIL;

   if (*(media->FormatType()) == FORMAT_VideoInfo) {
      VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)media->Format();
      bmih = pVIH->bmiHeader;
   } else if (*(media->FormatType()) == FORMAT_VideoInfo2) {
      VIDEOINFOHEADER2 *pVIH = (VIDEOINFOHEADER2*)media->Format();
      bmih = pVIH->bmiHeader;
   } else {
      TRACE("    MEDIA FORMAT TYPE DOES NOT MATCH!!!!!!\n");
      return E_FAIL;
   }
   return  S_OK;
}

// renders a frame and computes structuring values
HRESULT Sampler::DoRenderSample(IMediaSample *sample) {
   // BUGFIX 5025: set correct frame number
   // counting is not sufficient because it does not respect dropped frames
   //m_iFrameCounter++;
   REFERENCE_TIME * pTimeStart = new REFERENCE_TIME();
   REFERENCE_TIME * pTimeEnd = new REFERENCE_TIME();
   sample->GetTime(pTimeStart, pTimeEnd);
   int iFrameDuration = *pTimeEnd - *pTimeStart;
   m_iFrameCounter = *pTimeStart / iFrameDuration + 1;
  
   // Get the image data buffer
   BYTE* pImage; 
   HRESULT hr = sample->GetPointer(&pImage);
   if (FAILED(hr))
      return E_FAIL;

   bool setIndex = false;

   if(!prevImage) {
      // first frame is full update (max pixel)
      LMisc::StructuringValues *pStructuringValues = new LMisc::StructuringValues();
      pStructuringValues->iFrameNr = 1;
      pStructuringValues->iPixelCount = bmih.biWidth * bmih.biHeight;
      pStructuringValues->iAreaCount = bmih.biWidth * bmih.biHeight;
      m_paStructuringValues->Add(*pStructuringValues);
   }
   // compare with previous image
   else if(prevImage) {
      RECT rect = {0, 0, bmih.biWidth-1, bmih.biHeight-1};
      //TRACE("   RECT  at (%i,%i) size [%i x %i] \n",  rect.top, rect.left, rect.right-rect.left, rect.bottom-rect.top); 
      int stride = bmih.biBitCount / 8;
      // determineChangeArea from lsgc/twlc
      int changedPixelCount = LMisc::determineChangeArea(prevImage, pImage, bmih.biWidth, bmih.biHeight, stride, &rect);
      bool changed = changedPixelCount > 0;
      if(changed) {
         int area = 0;
         int SPLIT_LIMIT = 40; // as used by lsgc/twlc
         RECT *changes = (RECT*)malloc(SPLIT_LIMIT*sizeof(RECT));
         int splitLength = SPLIT_LIMIT;
         LMisc::splitChangeArea(prevImage, pImage, bmih.biWidth, bmih.biHeight, stride, rect, changes, &splitLength);
         for (int i=0; i<splitLength; i++) {
            int width = changes[i].right-changes[i].left+1;
            int height = changes[i].bottom-changes[i].top+1;
            area += width * height;
            //TRACE("  change rect Nr.%i : %i,%i - %i x %i\n", i+1, changes[i].left,bmih.biHeight-changes[i].top,width, height);
         }

         // add data for later structuring
         LMisc::StructuringValues *pStructuringValues = new LMisc::StructuringValues();
         pStructuringValues->iFrameNr = m_iFrameCounter;
         pStructuringValues->iPixelCount = changedPixelCount;
         pStructuringValues->iAreaCount = area;
         m_paStructuringValues->Add(*pStructuringValues);
      }
   }

   // keep image to compare current frame against next frame
   if (!prevImage)
      prevImage = (unsigned char *) malloc(sample->GetSize());
   if (prevImage)
      memcpy(prevImage, pImage, sample->GetSize());

   return  S_OK;
}

HRESULT ShouldDrawSampleNow(IMediaSample *sample, REFERENCE_TIME *start, REFERENCE_TIME *stop) {
   return S_OK; // disable droping of frames
}


/**********************************************************
* read and write structuring values to/from file
***********************************************************/

static int GetNumber(char *szToken, int iValidCharacters)
{
   szToken[iValidCharacters] = 0;
   return atoi(szToken);
}

CArray<LMisc::StructuringValues>* ReadStructuringValues(CString csFilename) {
   if(!csFilename)
      return NULL;

   HANDLE sgeFile = CreateFile(csFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

   if (!sgeFile)
      return NULL;

   unsigned long read;
   char charBufi[16*1024];
   int iBlankCount = 0;
   int iLineCount = 0;

   char aOneToken[10]; // no number should be longer
   ZeroMemory(aOneToken, 10);
   int iTokenCharacters = 0;

   LMisc::StructuringValues *pStructuringValues = NULL;
   CArray<LMisc::StructuringValues> *paStructuringValues = new CArray<LMisc::StructuringValues>();

   while (ReadFile(sgeFile, charBufi, 16*1024, &read, NULL) == TRUE && read != 0)
   {
      for (unsigned int i = 0; i < read; ++i)
      {
         char charBuf = charBufi[i];
         if (charBuf == '\r') 
               continue; // skip Carriage Return

         if (charBuf == ' ' || charBuf == '\n')
         {
            // it is a separator - parse read data now
            if(iLineCount == 0) {
               // read version
               switch(iBlankCount) {
                  case 0:
                     // check if version
                     if(strnicmp(aOneToken, "VERSION", 6) != 0)
                        // unknown file format
                        // TODO: show error dialog
                        break;
                  case 1:
                     int iVersionNumber = GetNumber(aOneToken, iTokenCharacters);
                     if(iVersionNumber != 1)
                        // unsupported version number
                        // TODO: show error dialog
                        break;
                     break;
               }
            } else {
               // read structuring values
               switch(iBlankCount) {
                  case 0:
                     // memory should be released if it is the case.
                     SAFE_DELETE(pStructuringValues);
                     pStructuringValues = new LMisc::StructuringValues();
                     pStructuringValues->iFrameNr = GetNumber(aOneToken, iTokenCharacters);
                     break;
                  case 1:
                     pStructuringValues->iPixelCount = GetNumber(aOneToken, iTokenCharacters);
                     break;
                  case 2:
                     pStructuringValues->iAreaCount = GetNumber(aOneToken, iTokenCharacters);
                     paStructuringValues->Add(*pStructuringValues);
                     break;
               }               
            }
            
            if (charBuf == ' ')
               ++iBlankCount;
            else if (charBuf == '\n') {
               iBlankCount = 0;
               ++iLineCount;
            } 

            iTokenCharacters = 0;
         }
         else
         {
            // it is part of a number
            aOneToken[iTokenCharacters++] = charBuf;
         }
      }
   }

   CloseHandle(sgeFile);
   // delete pStructuringValues. paStructuringValues ads the value (*pStructuringValues) and not the pointer so here it should be released.
   SAFE_DELETE(pStructuringValues);

   if(paStructuringValues && !paStructuringValues->IsEmpty())
      return paStructuringValues;
   else
      return NULL;
}

void WriteStructuringValues(CString csFilename, CArray<LMisc::StructuringValues> *paStructuringValues ) {
   if(!csFilename || !paStructuringValues)
      return;

   // create file for structuring data
   char filename[512];
   sprintf(filename, "%ls", csFilename);
   FILE *sgExtendeFile = fopen(filename, "w");

   if(sgExtendeFile) { 
      // write: version number
      fprintf(sgExtendeFile, "VERSION 1\n"); 
      fflush(sgExtendeFile); 

      if(paStructuringValues) {
         for( int i = 0; i < paStructuringValues->GetSize(); i++ ) {
            LMisc::StructuringValues pStructuringValues = paStructuringValues->GetAt(i);
            char buffer[50];
            sprintf(buffer, "%i %i %i\n", pStructuringValues.iFrameNr, pStructuringValues.iPixelCount, pStructuringValues.iAreaCount);
            fprintf(sgExtendeFile, buffer); 
            fflush(sgExtendeFile); 
         }
         fclose(sgExtendeFile);
         sgExtendeFile = NULL;
      }
   }
}

/**********************************************************
* read user action values (mouse events) from file
***********************************************************/

// read user actions from sga-file
CArray<UserAction>* ReadUserActions(CString csFilename) {
   if(!csFilename)
      return NULL;

   HANDLE sgaFile = CreateFile(csFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

   if (!sgaFile)
      return NULL;

   unsigned long read;
   char charBufi[16*1024];
   int iBlankCount = 0;
   int iLineCount = 0;

   char aOneToken[15]; // no number/value should be longer
   ZeroMemory(aOneToken, 15);
   int iTokenCharacters = 0;

   UserAction *pUserAction = NULL;
   CArray<UserAction> *paUserAction = new CArray<UserAction>();

   bool bReadingLiveContextElementInfo = false;
   //CString csLiveContextElementInfoString = "";

   while (ReadFile(sgaFile, charBufi, 16*1024, &read, NULL) == TRUE && read != 0)
   {
      for (unsigned int i = 0; i < read; ++i)
      {
         char charBuf = charBufi[i];

         // skip Live Context data
         if (charBuf == '{')
            bReadingLiveContextElementInfo = true;         
         if (bReadingLiveContextElementInfo) {
            if (charBuf == '\n')
               bReadingLiveContextElementInfo = false;
            continue;
         }


         if (charBuf == '\r') 
               continue; // skip Carriage Return

         if (charBuf == ' ' || charBuf == '\n' || charBuf == '(' || charBuf == ',' || charBuf == ')')
         {
            // it is a separator - parse read data now
            switch(iBlankCount) {
               case 0:  // timestamp
                  // memory should be released if it is the case.
                  SAFE_DELETE(pUserAction);
                  pUserAction = new UserAction();

                  // set optional LiveContext data to undefined
                  pUserAction->pElementInfo = NULL;
                  pUserAction->iLiveContextXoffset = 0;
                  pUserAction->iLiveContextYoffset = 0;

                  // set timestamp
                  pUserAction->iTimeMsec = GetNumber(aOneToken, iTokenCharacters);
                  break;
               case 1: // frame number
                  pUserAction->iFrameNr = GetNumber(aOneToken, iTokenCharacters);
                  break;
               case 2: // x coord
                  if ( charBuf != '(' )
                     pUserAction->iXcoord = GetNumber(aOneToken, iTokenCharacters);
                  break;
               case 3: // y coord
                  if ( charBuf == ')' )
                     pUserAction->iYcoord = GetNumber(aOneToken, iTokenCharacters);
                  break;
               case 4: // button or mouse movement
                  if ( strnicmp(aOneToken, "LEFT", 4) == 0)
                     pUserAction->dwEventType = MH_LEFT_BUTTON;
                  else if ( strnicmp(aOneToken, "MIDDLE", 6) == 0)
                     pUserAction->dwEventType = MH_MIDDLE_BUTTON;
                  else if ( strnicmp(aOneToken, "RIGHT", 5) == 0)
                     pUserAction->dwEventType = MH_RIGHT_BUTTON;
                  else if ( strnicmp(aOneToken, "MOUSE", 5) == 0)
                     pUserAction->dwEventType = MH_POINTER;
				  else if ( strnicmp(aOneToken, "TELE", 4) == 0)
                     pUserAction->dwEventType = MH_TELE;
                  break;
               case 5: // action of button or mouse movement
                  if ( strnicmp(aOneToken, "UP", 2) == 0)
                     pUserAction->dwAction = MH_BUTTON_UP;
                  else if ( strnicmp(aOneToken, "DOWN", 4) == 0)
                     pUserAction->dwAction = MH_BUTTON_DOWN;
                  else if ( strnicmp(aOneToken, "DOUBLECLICK", 11) == 0)
                     pUserAction->dwAction = MH_BUTTON_DBLCLICK;
                  else if ( strnicmp(aOneToken, "MOVE", 4) == 0)
                     pUserAction->dwAction = MH_MOVE;
                  else if ( strnicmp(aOneToken, "DRAG", 4) == 0)
                     pUserAction->dwAction = MH_DRAG;
				  else if ( strnicmp(aOneToken, "REM", 3) == 0)
                     pUserAction->dwAction = MH_REM;
                  break;
               case 6: // Live Context offset x coord (optional)
                  if ( charBuf != '(' )
                     pUserAction->iLiveContextXoffset = GetNumber(aOneToken, iTokenCharacters);
                  break;
               case 7: // Live Context offset y coord (optional)
                  if ( charBuf == ')' )
                     pUserAction->iLiveContextYoffset = GetNumber(aOneToken, iTokenCharacters);
                  break;
            }               
            
            if (charBuf == ' ' || charBuf == ',')
               ++iBlankCount;
            else if (charBuf == '\n') {
               // line completed: add copy to array
               paUserAction->Add(*pUserAction);
               SAFE_DELETE(pUserAction);

               // clear counter
               iBlankCount = 0;
               ++iLineCount;
            } 

            iTokenCharacters = 0;
         }
         else
         {
            // it is part of a number
            aOneToken[iTokenCharacters++] = charBuf;
         }
      }
   }

   CloseHandle(sgaFile);
   // delete pStructuringValues. paStructuringValues ads the value (*pStructuringValues) and not the pointer so here it should be released.
   SAFE_DELETE(pUserAction);

   if(paUserAction && !paUserAction->IsEmpty())
      return paUserAction;
   else
      return NULL;
}

// read Live Context Data from sga-file
CArray<CLiveContextElementInfo*, CLiveContextElementInfo*>* ReadLiveContextElementInfos(CString csFilename) {
    if(!csFilename)
        return NULL;

    HANDLE sgaFile = CreateFile(csFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (!sgaFile)
        return NULL;

    unsigned long read;
    BYTE charBufi[16*1024];

    CArray<CLiveContextElementInfo*, CLiveContextElementInfo*> *paElementInfos = new CArray<CLiveContextElementInfo*, CLiveContextElementInfo*>();

    bool bReadingLiveContextElementInfo = false;
    CString csLiveContextElementInfoString = "";

    while (ReadFile(sgaFile, charBufi, 16*1024, &read, NULL) == TRUE && read != 0) {
        for (unsigned int i = 0; i < read; ++i) {
            BYTE charBuf = charBufi[i];
            if (charBuf == '{') {
                bReadingLiveContextElementInfo = true;
            }
            if (bReadingLiveContextElementInfo) {
                csLiveContextElementInfoString += charBuf;
                if (charBuf == '\n') {
                    // parse LC
                    char *ansiText = LString::WideChar2Ansi(csLiveContextElementInfoString);
                    if (ansiText != NULL) {
                        DWORD dwSize = strlen(ansiText);
                        CLiveContextElementInfo *pElementInfo = new CLiveContextElementInfo( CPoint(0,0) );
                        pElementInfo->ParseJsonString(ansiText, dwSize);
                        SAFE_DELETE(ansiText);
                        CString result = pElementInfo->ToJsonString();

                        TRACE("READ ELEMNT INFO:\n---> ");
                        TRACE(result);

                        paElementInfos->Add(pElementInfo);
                    }

                    // cleanup
                    csLiveContextElementInfoString = "";
                    bReadingLiveContextElementInfo = false;
                }
            }
        }
    }

    CloseHandle(sgaFile);

    if(paElementInfos && !paElementInfos->IsEmpty())
        return paElementInfos;
    else
        return NULL;
}

/**********************************************************
* DrawSDK helper
***********************************************************/

// TODO maybe transferred to DrawSdk::Text
DrawSdk::Text* CreateTextObject(CString text, int posX, int posY, int width, int height, int fontSize) {
   CString sgmlString;
   StringManipulation::TransformForSgml(text,sgmlString);

   DrawSdk::Text *textObject = NULL;

   Gdiplus::ARGB penColor  = RGB(255, 255, 255);
   CString fontFamily = _T("Arial");
   int size           = fontSize;
   CString weight     = _T("normal");
   CString slant      = _T("roman");
   CString ttfName    = _T("ARIAL.TTF");
   int textStyle      = 0;
   bool bIsUnderlined = (textStyle & FLAG_UNDERLINE) != 0;

   LPCTSTR szTextType = NULL;
   DrawSdk::TextTypeId idTextType = DrawSdk::TEXT_TYPE_NOTHING;

   if (szTextType != NULL)
      idTextType = DrawSdk::Text::GetTextTypeForString(szTextType);

   LOGFONT logFont;
   FontManipulation::Create(fontFamily, size, weight, slant, bIsUnderlined, logFont);
   if (ttfName.IsEmpty())
      textObject = new DrawSdk::Text((double)posX, (double)posY, (double)width, (double)height, penColor, sgmlString, sgmlString.GetLength(), &logFont, idTextType);
   else
   {
      CString sgmlTTFName;
      StringManipulation::TransformForSgml(ttfName, sgmlTTFName);
      textObject = new DrawSdk::Text((double)posX, (double)posY, (double)width, (double)height, penColor, sgmlString, sgmlString.GetLength(), &logFont, sgmlTTFName, idTextType);
   }
   sgmlString.ReleaseBuffer();
   text.ReleaseBuffer();

   return textObject;
}

/***************************************************************
* create poster frames / static images for thumbnails - HELPERS
****************************************************************/
template<typename T> void** IID_PPV_ARGS_Helper(T** pp)
{
   // make sure everyone derives from IUnknown
   static_cast<IUnknown*>(*pp);
 
   return reinterpret_cast<void**>(pp);
}

#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)

// Query whether a pin is connected to another pin.
//
// Note: This function does not return a pointer to the connected pin.
HRESULT IsPinConnected(IPin *pPin, BOOL *pResult)
{
    IPin *pTmp = NULL;
    HRESULT hr = pPin->ConnectedTo(&pTmp);
    if (SUCCEEDED(hr))
    {
        *pResult = TRUE;
    }
    else if (hr == VFW_E_NOT_CONNECTED)
    {
        // The pin is not connected. This is not an error for our purposes.
        *pResult = FALSE;
        hr = S_OK;
    }

    SafeRelease(&pTmp);
    return hr;
}

// Query whether a pin has a specified direction (input / output)
HRESULT IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pResult)
{
    PIN_DIRECTION pinDir;
    HRESULT hr = pPin->QueryDirection(&pinDir);
    if (SUCCEEDED(hr))
    {
        *pResult = (pinDir == dir);
    }
    return hr;
}

// Match a pin by pin direction and connection state.
HRESULT MatchPin(IPin *pPin, PIN_DIRECTION direction, BOOL bShouldBeConnected, BOOL *pResult)
{
    //assert(pResult != NULL);

    BOOL bMatch = FALSE;
    BOOL bIsConnected = FALSE;

    HRESULT hr = IsPinConnected(pPin, &bIsConnected);
    if (SUCCEEDED(hr))
    {
        if (bIsConnected == bShouldBeConnected)
        {
            hr = IsPinDirection(pPin, direction, &bMatch);
        }
    }

    if (SUCCEEDED(hr))
    {
        *pResult = bMatch;
    }
    return hr;
}

// Return the first unconnected input pin or output pin.
HRESULT FindUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
    IEnumPins *pEnum = NULL;
    IPin *pPin = NULL;
    BOOL bFound = FALSE;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        goto done;
    }

    while (S_OK == pEnum->Next(1, &pPin, NULL))
    {
        hr = MatchPin(pPin, PinDir, FALSE, &bFound);
        if (FAILED(hr))
        {
            goto done;
        }
        if (bFound)
        {
            *ppPin = pPin;
            (*ppPin)->AddRef();
            break;
        }
        SafeRelease(&pPin);
    }

    if (!bFound)
    {
        hr = VFW_E_NOT_FOUND;
    }

done:
    SafeRelease(&pPin);
    SafeRelease(&pEnum);
    return hr;
}

// Connect output pin to filter.
HRESULT ConnectFilters(
    IGraphBuilder *pGraph, // Filter Graph Manager.
    IPin *pOut,            // Output pin on the upstream filter.
    IBaseFilter *pDest)    // Downstream filter.
{
    IPin *pIn = NULL;
        
    // Find an input pin on the downstream filter.
    HRESULT hr = FindUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
    if (SUCCEEDED(hr))
    {
        // Try to connect them.
        hr = pGraph->Connect(pOut, pIn);
        pIn->Release();
    }
    return hr;
}

// Connect filter to input pin.
HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IPin *pIn)
{
    IPin *pOut = NULL;
        
    // Find an output pin on the upstream filter.
    HRESULT hr = FindUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
    if (SUCCEEDED(hr))
    {
        // Try to connect them.
        hr = pGraph->Connect(pOut, pIn);
        pOut->Release();
    }
    return hr;
}

// Connect filter to filter
HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest)
{
    IPin *pOut = NULL;

    // Find an output pin on the first filter.
    HRESULT hr = FindUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
    if (SUCCEEDED(hr))
    {
        hr = ConnectFilters(pGraph, pOut, pDest);
        pOut->Release();
    }
    return hr;
}

/**********************************************************
* create poster frames / static images for thumbnails
***********************************************************/

FILE *logFile = NULL;
#define LOG(x)  if(logFile) { fprintf(logFile, x); fflush(logFile); }

// write poster frames of video at given timestamps
// [in] tszVideoFileName: clip filename with path
// [in] tszImageFileNamePrefix: prefix of image filename with path
// [in] timestamps: list of timestamps in msec
// [in] offset: clip offset related to recording (used to create filenames with timestamps related to recording instead of clip)
HRESULT WritePosterFrames(LPCTSTR tszVideoFileName, LPCTSTR tszImageFileNamePrefix, CList<int> *timestamps, int clipOffset, HRESULT (*fnProgress)() )
{

   char buffer[100];
   
   /*
   // open log file
   CString csLogfile = tszVideoFileName;
   StringManipulation::GetPathFilePrefix(csLogfile);
   csLogfile += _T(".log");   
   char* lpszString = new char [ csLogfile.GetLength () + 1 ];   
   int i;
   for(i=0; i < csLogfile.GetLength(); ++i) 
      lpszString[i] = csLogfile[i];
   lpszString[i] = 0;
   errno_t err;
   if( (err = fopen_s( &logFile, lpszString, "w" )) != 0 ) {
      wprintf(L"Creating log file failed. Error Code: %d\n", err);
      return err;
   }
   delete lpszString;
   LOG("logging\n");

   // log current time
   time_t ltime;
   wchar_t buf[26];
   time( &ltime );
   if ( (err = _wctime_s( buf, 26, &ltime )) != 0)
   {
      wprintf(L"Invalid Arguments for _wctime_s. Error Code: %d\n", err);
   } else {
      sprintf_s(buffer, "log start time %ls", buf);
      LOG(buffer);
   }
   */
   

   // Configure LSGC to write frames without mouse pointer
   SetEnvironmentVariable(_T("LSGC_NO_POINTER"),_T("TRUE"));

   LOG("\nINIT\n");
   // initialize
   HRESULT hr = ::CoInitialize(NULL);
   sprintf_s(buffer, "::CoInitialize : result = %X\n", hr);
   LOG(buffer);

   IGraphBuilder *pGraph = NULL;
   IMediaControl *pControl = NULL;
   IMediaEventEx *pEvent = NULL;
   IBaseFilter *pGrabberF = NULL;
   ISampleGrabber *pGrabber = NULL;
   IMediaSeeking *pSeek = NULL;
   IBaseFilter *pSourceF = NULL;
   IEnumPins *pEnum = NULL;
   IPin *pPin = NULL;
   IBaseFilter *pNullF = NULL;
   BYTE *pBuffer = NULL;


   // get graph and controls
   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pGraph));
   sprintf_s(buffer, "CoCreateInstance(CLSID_FilterGraph) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      hr = pGraph->QueryInterface(IID_PPV_ARGS(&pControl));
   sprintf_s(buffer, "pGraph->QueryInterface(&pControl) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      hr = pGraph->QueryInterface(IID_PPV_ARGS(&pEvent));
   sprintf_s(buffer, "pGraph->QueryInterface(&pEvent) : result = %X\n", hr);
   LOG(buffer);

   // Bugfix 5684: force use of "AVI Splitter" if available
   // should overwrite other third party splitters that are not fully working,
   // e.g. Nero MP4 Splitter or Funbox AVI Splitter
   if (SUCCEEDED(hr)) {
      IBaseFilter *pAviSplitter = NULL;
      HRESULT hrOptional = S_OK;
      if (SUCCEEDED(hrOptional))
         hrOptional = CoCreateInstance(CLSID_AviSplitter, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pAviSplitter));
      sprintf_s(buffer, "CoCreateInstance(CLSID_AviSplitter, ...) : result = %X\n", hrOptional);
      LOG(buffer);
      if (SUCCEEDED(hrOptional))
         hrOptional = pGraph->AddFilter(pAviSplitter, L"AVI Splitter");
      sprintf_s(buffer, "pGraph->AddFilter(pAviSplitter) : result = %X\n", hrOptional);
      LOG(buffer);
   }   

   // Create the Sample Grabber filter.
   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pGrabberF));
   sprintf_s(buffer, "CoCreateInstance(CLSID_SampleGrabber, ...) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");
   sprintf_s(buffer, "pGraph->AddFilter(pGrabberF) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      hr = pGrabberF->QueryInterface(IID_PPV_ARGS(&pGrabber));
   sprintf_s(buffer, "pGrabberF->QueryInterface(&pGrabber) : result = %X\n", hr);
   LOG(buffer);

   // set media type
   AM_MEDIA_TYPE mt;
   ZeroMemory(&mt, sizeof(mt));
   mt.majortype = MEDIATYPE_Video;
   mt.subtype = MEDIASUBTYPE_RGB24;
   if (SUCCEEDED(hr))
      hr = pGrabber->SetMediaType(&mt);
   sprintf_s(buffer, "pGrabber->SetMediaType(&mt) : result = %X\n", hr);
   LOG(buffer);

   // build graph
   if (SUCCEEDED(hr))
      hr = pGraph->AddSourceFilter(tszVideoFileName, L"Source", &pSourceF);
   sprintf_s(buffer, "pGraph->AddSourceFilter(aviFileName) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      hr = pSourceF->EnumPins(&pEnum);
   sprintf_s(buffer, "pSourceF->EnumPins(&pEnum) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr)) {
      while (S_OK == pEnum->Next(1, &pPin, NULL))
      {
         hr = ConnectFilters(pGraph, pPin, pGrabberF);
         sprintf_s(buffer, "try to ConnectFilters(pGraph, pPin, pGrabberF) : result = %X\n", hr);
         LOG(buffer);
         SafeRelease(&pPin);
         if (SUCCEEDED(hr))
            break;
      }
   }
   sprintf_s(buffer, "ConnectFilters() : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pNullF));
   sprintf_s(buffer, "CoCreateInstance(CLSID_NullRenderer) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      hr = pGraph->AddFilter(pNullF, L"Null Filter");
   sprintf_s(buffer, "pGraph->AddFilter(Null Filter) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      hr = ConnectFilters(pGraph, pGrabberF, pNullF);
   sprintf_s(buffer, "ConnectFilters(pGraph, pGrabberF, pNullF) : result = %X\n", hr);
   LOG(buffer);

   // unset graph clock -> process as fast as possible
   IMediaFilter *pMediaFilter = 0;
   if (SUCCEEDED(hr)) {
      hr = pGraph->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter);
      sprintf_s(buffer, "pGraph->QueryInterface(IID_IMediaFilter) : result = %X\n", hr);
      LOG(buffer);
      pMediaFilter->SetSyncSource(NULL);
      pMediaFilter->Release();
   }

   if (SUCCEEDED(hr))
      hr = pGrabber->SetBufferSamples(TRUE);
   sprintf_s(buffer, "pGrabber->SetBufferSamples(TRUE) : result = %X\n", hr);
   LOG(buffer);

   // get seek & test control
   if (SUCCEEDED(hr))
      hr = pGraph->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
   sprintf_s(buffer, "pGraph->QueryInterface(IID_IMediaSeeking) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr)) {
      DWORD dwCap;
      hr = pSeek->GetCapabilities(&dwCap);
      sprintf_s(buffer, "pSeek->GetCapabilities(&dwCap) : result = %X\n", hr);
      LOG(buffer);
      if (!AM_SEEKING_CanSeekAbsolute & dwCap)
         hr = E_FAIL;
      sprintf_s(buffer, "AM_SEEKING_CanSeekAbsolute : result = %X\n", hr);
      LOG(buffer);
   }

   if (SUCCEEDED(hr))   
      hr = pGrabber->GetConnectedMediaType(&mt);
   sprintf_s(buffer, "pGrabber->GetConnectedMediaType(&mt) : result = %X\n", hr);
   LOG(buffer);

   // Examine the format block and get headers
   VIDEOINFOHEADER *pVih = NULL;
   BITMAPINFOHEADER *bmih = NULL;
   BITMAPINFO bmi;
   if ((mt.formattype == FORMAT_VideoInfo) && 
      (mt.cbFormat >= sizeof(VIDEOINFOHEADER)) &&
      (mt.pbFormat != NULL)) 
   {
      pVih = (VIDEOINFOHEADER*)mt.pbFormat;
      bmih = &pVih->bmiHeader;

      ZeroMemory(&bmi, sizeof(BITMAPINFO));
      CopyMemory(&(bmi.bmiHeader), bmih, sizeof(BITMAPINFOHEADER));
   } else {
      // Invalid format.
      hr = VFW_E_INVALIDMEDIATYPE; 
   }
   sprintf_s(buffer, "VIDEOINFOHEADER Format Match : result = %X\n", hr);
   LOG(buffer);

   LOG("\nPROCESS\n");

   // loop all timestamps and grab poster frame
   POSITION position = timestamps->GetHeadPosition();
   while (SUCCEEDED(hr) && position != NULL) {
      LOG("\n");

      // seek to position
      int streamTimeMsec = timestamps->GetNext(position);
      int iOrigTimeMsec = streamTimeMsec;

      // BUGFIX 6279: incorrect screenshots for low frame rates (might be cause by rounding errors)
      streamTimeMsec++;

      if (streamTimeMsec < 0) streamTimeMsec = 0;
	  
      // BUGFIX 5914: no thumbnails after approx 3:30 due to overflow -> force cast to REFERENCE_TIME
      REFERENCE_TIME rtStop = (REFERENCE_TIME)streamTimeMsec * (REFERENCE_TIME)10000;
	  
      if (SUCCEEDED(hr))
         hr = pSeek->SetPositions( NULL, AM_SEEKING_NoPositioning, &rtStop, AM_SEEKING_AbsolutePositioning);
      sprintf_s(buffer, "pSeek->SetPositions(%d) : result = %X\n", streamTimeMsec, hr);
      LOG(buffer);

      if (SUCCEEDED(hr))
         hr = pControl->Run();
      sprintf_s(buffer, "pControl->Run() : result = %X\n", hr);
      LOG(buffer);

      // TODO: is this required??
      long evCode;
      if (SUCCEEDED(hr))
         hr = pEvent->WaitForCompletion(INFINITE, &evCode);
      sprintf_s(buffer, "pEvent->WaitForCompletion() : result = %X\n", hr);
      LOG(buffer);

      // Pauses the filter graph, allowing filters to queue data, and then stops the filter graph.
      while(SUCCEEDED(hr) && pControl->StopWhenReady() == S_FALSE);
      sprintf_s(buffer, "pControl->StopWhenReady() : result = %X\n", hr);
      LOG(buffer);
   
      // get image buffer
      long cbBuffer;
      //if (SUCCEEDED(hr))
      //   // Find the required buffer size.
      //   hr = pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
      //sprintf_s(buffer, "pGrabber->GetCurrentBuffer() determine size : result = %X\n", hr);
      //LOG(buffer);
					 
      // BUGFIX 5684: retry on VFW_E_WRONG_STATE
      if (SUCCEEDED(hr)) {
         int i = 0;
         do {
            i++;
            // Find the required buffer size.
            hr = pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
            sprintf_s(buffer, "pGrabber->GetCurrentBuffer() determine size : try %d : result = %X\n", i, hr);
            LOG(buffer);
            if ( hr == VFW_E_WRONG_STATE )
               // wait a little bit before trying again
               Sleep(100);
         } while( hr == VFW_E_WRONG_STATE && i<50  );
      }
      sprintf_s(buffer, "pGrabber->GetCurrentBuffer() determine size : final result = %X\n", hr);
      LOG(buffer);

      if (FAILED(hr)) {
         CString csError;
         switch ( hr ) {
            case VFW_E_WRONG_STATE:
               csError = L"VFW_E_WRONG_STATE";
               break;
            default:
               csError.Format(L"%X", hr);

         }
         // TODO: localize error message
         CString csErrorMsg = L"Couldn't grab preview image.\nError: "+csError;
         MessageBox(NULL, csErrorMsg, L"Error", MB_OK | MB_TOPMOST);
      }

      if (SUCCEEDED(hr)) {
         pBuffer = (BYTE*)CoTaskMemAlloc(cbBuffer);
         if (!pBuffer) 
            hr = E_OUTOFMEMORY;
      }
      sprintf_s(buffer, "CoTaskMemAlloc() : result = %X\n", hr);
      LOG(buffer);
      if (SUCCEEDED(hr))
         hr = pGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);
      sprintf_s(buffer, "pGrabber->GetCurrentBuffer() get buffer : result = %X\n", hr);
      LOG(buffer);

      // write image
      if (SUCCEEDED(hr)) {
         HDC hdcDest = GetDC(0);
         // get device dependent bitmap (DDB)
         HBITMAP hBitmap = CreateDIBitmap(hdcDest, bmih, CBM_INIT, pBuffer, &bmi, DIB_RGB_COLORS);
         if ( hBitmap == NULL )
            hr = E_FAIL;
         sprintf_s(buffer, "CreateDIBitmap : result = %X\n", hr);
         LOG(buffer);
         if (SUCCEEDED(hr)) {
            // write to file
            CString filename;
            filename.Format(_T("%s%d.png"), tszImageFileNamePrefix, iOrigTimeMsec + clipOffset);
            CImage imageDDB;
            imageDDB.Attach(hBitmap);
            hr = imageDDB.Save(filename);
         }
         sprintf_s(buffer, "imageDDB.Save() : result = %X\n", hr);
         LOG(buffer);
         ::DeleteDC(hdcDest);
      }

      // BUGFIX 5823: update progress & handle cancel
	   if (SUCCEEDED(hr) && fnProgress != NULL )
         hr = fnProgress();
      sprintf_s(buffer, "UpdateProgress : result = %X\n", hr);
      LOG(buffer);
   }   
   sprintf_s(buffer, "\nDONE : result = %X\n", hr);
   LOG(buffer);

   // cleanup
   FreeMediaType(mt);
   CoTaskMemFree(pBuffer);
   SafeRelease(&pSeek);
   SafeRelease(&pPin);
   SafeRelease(&pEnum);
   SafeRelease(&pNullF);
   SafeRelease(&pSourceF);
   SafeRelease(&pGrabber);
   SafeRelease(&pGrabberF);
   SafeRelease(&pControl);
   SafeRelease(&pEvent);
   SafeRelease(&pGraph);

   ::CoUninitialize();

   // unset LSGC configuration
   SetEnvironmentVariable(_T("LSGC_NO_POINTER"), NULL);

   // close log file
   if(logFile) {
      LOG("log end\n");
      fflush(logFile);
      fclose(logFile);
      logFile = NULL;
   }

   return hr;
};

/*
// alternative code used to research bug 5684
HRESULT WritePosterFrames(LPCTSTR tszVideoFileName, LPCTSTR tszImageFileNamePrefix, CList<int> *timestamps, int clipOffset, HRESULT (*fnProgress)() )
{
   // open log file
   CString csLogfile = tszVideoFileName;
   StringManipulation::GetPathFilePrefix(csLogfile);
   csLogfile += _T(".log");   
   char* lpszString = new char [ csLogfile.GetLength () + 1 ];   
   int i;
   for(i=0; i < csLogfile.GetLength(); ++i) 
      lpszString[i] = csLogfile[i];
   lpszString[i] = 0;
   errno_t err;
   if( (err = fopen_s( &logFile, lpszString, "w" )) != 0 ) {
      wprintf(L"Creating log file failed. Error Code: %d\n", err);
      return err;
   }
   delete lpszString;
   LOG("logging\n");

   // log current time
   char buffer[1000];
   time_t ltime;
   wchar_t buf[26];
   time( &ltime );
   if ( (err = _wctime_s( buf, 26, &ltime )) != 0)
   {
      wprintf(L"Invalid Arguments for _wctime_s. Error Code: %d\n", err);
   } else {
      sprintf_s(buffer, "log start time %ls", buf);
      LOG(buffer);
   }

   // Configure LSGC to write frames without mouse pointer
   SetEnvironmentVariable(_T("LSGC_NO_POINTER"),_T("TRUE"));

   LOG("\nINIT\n");

   // initialize
   IGraphBuilder *pGraphBuilder = NULL;
   IMediaControl *pMediaControl = NULL;
   IBaseFilter *pSampleGrabberFilter = NULL;
   ISampleGrabber *pSampleGrabber = NULL;
   IMediaEvent *pMediaEvent = NULL;
   IMediaFilter *pMediaFilter = NULL;
   IMediaSeeking *pSeek = NULL;
   long eventCode;
   AM_MEDIA_TYPE am_media_type;

   // initialize COM
   HRESULT hr = CoInitialize(NULL);
   sprintf_s(buffer, "CoInitialize(NULL) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      // create FilterGraph
      hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (LPVOID *)&pGraphBuilder);
   sprintf_s(buffer, "CoCreateInstance(CLSID_FilterGraph) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      // create SampleGrabber(Filter)
      hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, (LPVOID *)&pSampleGrabberFilter);
   sprintf_s(buffer, "CoCreateInstance(CLSID_SampleGrabber) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr)) 
      // get ISampleGrabber interface from Filter
      hr = pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID *)&pSampleGrabber);
   sprintf_s(buffer, "pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber) : result = %X\n", hr);
   LOG(buffer);

   // determine the format for connecting SampleGrabber.
   // You can configure the SampleGrabber insertion place
   // by changing the values in this structure.
   // If you use the values in this sample,
   // you can get the video frame data right before
   // it is displayed.
   ZeroMemory(&am_media_type, sizeof(am_media_type));
   am_media_type.majortype = MEDIATYPE_Video;
   am_media_type.subtype = MEDIASUBTYPE_RGB24;
   am_media_type.formattype = FORMAT_VideoInfo;
   pSampleGrabber->SetMediaType(&am_media_type);

   if (SUCCEEDED(hr)) 
      // add SampleGrabber Filter to the Graph
      hr = pGraphBuilder->AddFilter(pSampleGrabberFilter, L"Sample Grabber");
   sprintf_s(buffer, "pGraphBuilder->AddFilter(pSampleGrabberFilter) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr)) 
      // get MediaControl interface
      hr = pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID *)&pMediaControl);
   sprintf_s(buffer, "pGraphBuilder->QueryInterface(IID_IMediaControl) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr)) 
      // get event to synchronize
      hr = pGraphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID *)&pMediaEvent);
   sprintf_s(buffer, "pGraphBuilder->QueryInterface(IID_IMediaEvent) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr)) 
      // create Graph. Graph that contains SampleGrabber will be created automatically.
      hr = pGraphBuilder->RenderFile(tszVideoFileName, NULL);
   sprintf_s(buffer, "pGraphBuilder->RenderFile(%ls) : result = %X\n", tszVideoFileName, hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      // Get connection information. This must be done after the Graph is created by RenderFile.
      hr = pSampleGrabber->GetConnectedMediaType(&am_media_type);
   sprintf_s(buffer, "pSampleGrabber->GetConnectedMediaType() : result = %X\n", hr);
   LOG(buffer);

   VIDEOINFOHEADER *pVideoInfoHeader = NULL;
   BITMAPINFOHEADER *bmih = NULL;
   BITMAPINFO bmi;

   if (SUCCEEDED(hr)) {
      pVideoInfoHeader = (VIDEOINFOHEADER *)am_media_type.pbFormat;

      // Print the width and height of the image.
      sprintf_s(buffer, "size = %d x %d\n", pVideoInfoHeader->bmiHeader.biWidth, pVideoInfoHeader->bmiHeader.biHeight);
      LOG(buffer);

      // Print the data size.
      sprintf_s(buffer, "sample size = %d\n", am_media_type.lSampleSize);
      LOG(buffer);

      // Examine the format block and get headers
      bmih = &pVideoInfoHeader->bmiHeader;
      ZeroMemory(&bmi, sizeof(BITMAPINFO));
      CopyMemory(&(bmi.bmiHeader), bmih, sizeof(BITMAPINFOHEADER));
   }

   if (SUCCEEDED(hr))
      hr = pGraphBuilder->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter);
   sprintf_s(buffer, "pGraphBuilder->QueryInterface(IID_IMediaFilter) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      // unset graph clock -> process as fast as possible
      hr = pMediaFilter->SetSyncSource(NULL);
   sprintf_s(buffer, "pMediaFilter->SetSyncSource(NULL) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      hr = pMediaControl->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
   sprintf_s(buffer, "pMediaControl->QueryInterface(IID_IMediaSeeking) : result = %X\n", hr);
   LOG(buffer);
   if (SUCCEEDED(hr))
      // Configure SampleGrabber to do grabbing.
      // Buffer data can not be obtained if you do not use SetBufferSamples.
      hr = pSampleGrabber->SetBufferSamples(TRUE);
   sprintf_s(buffer, "pSampleGrabber->SetBufferSamples(TRUE) : result = %X\n", hr);
   LOG(buffer);

   // loop all timestamps and grab poster frame
   POSITION position = timestamps->GetHeadPosition();
   REFERENCE_TIME rtPrevious = 0;
   while (SUCCEEDED(hr) && position != NULL) {
      // seek to position
      int streamTimeMsec = timestamps->GetNext(position);
	  
      // BUGFIX 5914: no thumbnails after approx 3:30 due to overflow -> force cast to REFERENCE_TIME
      REFERENCE_TIME rtStop = (REFERENCE_TIME)streamTimeMsec * (REFERENCE_TIME)10000;
	  
      REFERENCE_TIME rtCurrent = 0;
      if (SUCCEEDED(hr))
         // seek from previous to current. otherwise images corresponding to dropped frames are incorrect
         hr = pSeek->SetPositions( &rtPrevious, AM_SEEKING_AbsolutePositioning, &rtStop, AM_SEEKING_AbsolutePositioning);
      rtPrevious = rtStop;
      sprintf_s(buffer, "pSeek->SetPositions(%d) : result = %X\n", streamTimeMsec, hr);
      LOG(buffer);
      if (SUCCEEDED(hr))
         // start playing
         hr = pMediaControl->Run();
      sprintf_s(buffer, "pMediaControl->Run() : result = %X\n", hr);
      LOG(buffer);

      if (SUCCEEDED(hr))
         // The first argument is timeout value.
         // If you change the "-1" part into "2",
         // WaitForCompletion will timeout after 2 seconds.
         hr = pMediaEvent->WaitForCompletion(-1, &eventCode);
      sprintf_s(buffer, "pMediaEvent->WaitForCompletion() : result = %X\n", hr);
      LOG(buffer);
      if (SUCCEEDED(hr)) {
         switch (eventCode) {
            case 0:
               LOG("timeout\n");
            break;
            case EC_COMPLETE:
               LOG("complete\n");
            break;
            case EC_ERRORABORT:
               LOG("errorabort\n");
            break;
            case EC_USERABORT:
               LOG("userabort\n");
            break;
         }
      }

      //// Pauses the filter graph, allowing filters to queue data, and then stops the filter graph.
      //while(pMediaControl->StopWhenReady() > 0);

      // TODO: only for logging -> remove
      if (SUCCEEDED(hr)) {
         LONGLONG lCurrent;
         LONGLONG lStop;
         HRESULT hrTemp = pSeek->GetPositions(&lCurrent, &lStop);
         sprintf_s(buffer, "pSeek->GetPositions() : result = %X\n", hrTemp);
         LOG(buffer);
         if (SUCCEEDED(hrTemp)) {
            sprintf_s(buffer, "position expected: %d current: %d stop: %d\n" ,streamTimeMsec, (long)(lCurrent/10000), (long)(lStop/10000));
            LOG(buffer);
         }
      }

      // prepare buffer
      // TODO: allocate outside loop (if always same size)
      long nBufferSize = am_media_type.lSampleSize;
      long *pBuffer = (long *)malloc(nBufferSize);
      if (pBuffer == NULL)
         hr = E_OUTOFMEMORY;
      sprintf_s(buffer, "malloc(%d) : result = %X\n", nBufferSize, hr);
      LOG(buffer);
      if (SUCCEEDED(hr)) {
         int i = 0;
         do {
            i++;
            // grab image data.
            hr = pSampleGrabber->GetCurrentBuffer(&nBufferSize, pBuffer);
            sprintf_s(buffer, "pSampleGrabber->GetCurrentBuffer() : try %d : result = %X\n", i, hr);
            LOG(buffer);
            if ( hr == VFW_E_WRONG_STATE )
               // wait a little bit before trying again
               Sleep(100);
         } while( hr == VFW_E_WRONG_STATE && i<50  );
      }
      sprintf_s(buffer, "pSampleGrabber->GetCurrentBuffer() : final result = %X\n", hr);
      LOG(buffer);

      if (FAILED(hr)) {
         CString csError;
         switch ( hr ) {
            case VFW_E_WRONG_STATE:
               csError = L"VFW_E_WRONG_STATE";
               break;
            default:
               csError.Format(L"%X", hr);

         }
         CString csErrorMsg = L"Couldn't grab preview image.\nError: "+csError;
         MessageBox(NULL, csErrorMsg, L"Error", MB_OK | MB_TOPMOST);
      }

      if (SUCCEEDED(hr)) {
         // write image
         HDC hdcDest = GetDC(0);
         // get device dependent bitmap (DDB)
         HBITMAP hBitmap = CreateDIBitmap(hdcDest, bmih, CBM_INIT, pBuffer, &bmi, DIB_RGB_COLORS);
         // write to file
         CString filename;
         filename.Format(_T("%s%d.png"), tszImageFileNamePrefix, streamTimeMsec + clipOffset);
         CImage imageDDB;
         imageDDB.Attach(hBitmap);
         HRESULT hrTemp = imageDDB.Save(filename);
         sprintf_s(buffer, "imageDDB.Save(%ls) : result = %X\n", filename, hrTemp);
         LOG(buffer);
         ::DeleteDC(hdcDest);
      }

      free(pBuffer);
   } // end of for loop

   hr = pMediaControl->Stop();
   sprintf_s(buffer, "pMediaControl->Stop() : result = %X\n", hr);
   LOG(buffer);

   // release
   SafeRelease(&pMediaFilter);
   SafeRelease(&pSampleGrabber);
   SafeRelease(&pSampleGrabberFilter);
   SafeRelease(&pMediaEvent);
   SafeRelease(&pSeek);
   SafeRelease(&pMediaControl);
   SafeRelease(&pGraphBuilder);
   FreeMediaType(am_media_type);

   // finalize COM
   CoUninitialize();

   // unset LSGC configuration
   SetEnvironmentVariable(_T("LSGC_NO_POINTER"), NULL);

   // close log file
   if(logFile) {
      LOG("log end\n");
      fflush(logFile);
      fclose(logFile);
      logFile = NULL;
   }

   return hr;
};
*/
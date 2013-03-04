#include "global.h"
#include "resource.h"

#include "LanguageSupport.h"        // lsutl32
#include "MiscFunctions.h"          // filesdk
#include "LiveContextElementInfo.h"
#include "LiveContextCommunicator.h"
#include "DebugLog.h"
#include "LiveContext.h"
#include "SgSelectionPreparationBar.h"
#include "lutils.h" // LIo::MessagePipe

#include <gdiplus.h>
using namespace Gdiplus;

#define CAPFRAMEWNDCLASS _T("CaptureFrameWindow")
#define FRAMESIZE        5
#define FRAMEWIDTH       40
#define FRAMESPACE       3
#define ORANGE           RGB(0xff, 0x8d, 0x13)

LcMessageString::LcMessageString() {
    m_iBufferSize = 4096;
    m_szMessage = new char[m_iBufferSize];
    m_dwRead = 0;
}

LcMessageString::LcMessageString(char *szMessage, DWORD dwRead) {
    m_iBufferSize = dwRead+1;
    m_szMessage = new char[m_iBufferSize];

    m_dwRead = dwRead;
    strncpy(m_szMessage, szMessage, dwRead);
    m_szMessage[dwRead] = '\0';
}

LcMessageString::~LcMessageString() {
    if (m_szMessage != NULL)
        delete[] m_szMessage;
    m_szMessage = NULL;
    m_dwRead = 0;
}

SrMouseData::SrMouseData() {
    m_pMouseBgraData = NULL;
    m_iMouseDataLength = 0;
    m_ptCursorSize.x = 0;
    m_ptCursorSize.y = 0;
    m_ptPositionInVideo.x = 0;
    m_ptPositionInVideo.y = 0;
}

SrMouseData::~SrMouseData() {
    if (m_pMouseBgraData != NULL)
        delete[] m_pMouseBgraData;
}

void SrMouseData::UpdateData(RECT *prcRecordArea) {
    bool bSuccess = true;

    CURSORINFO ci;

    memset(&ci, 0, sizeof(CURSORINFO));
    ci.cbSize = sizeof(CURSORINFO);

    // Retrieve information about cursor look
    bSuccess = ::GetCursorInfo(&ci) == TRUE;

    if (bSuccess && ci.hCursor != NULL) {
        ICONINFO iconInfo;

        // Retrieve information about hotspot
        bSuccess = ::GetIconInfo(ci.hCursor, &iconInfo) == TRUE;

        if (bSuccess) {
            m_ptPositionInVideo.x = ci.ptScreenPos.x - iconInfo.xHotspot;
            m_ptPositionInVideo.y = ci.ptScreenPos.y - iconInfo.yHotspot;
            if (prcRecordArea != NULL) {
                m_ptPositionInVideo.x -= prcRecordArea->left;
                m_ptPositionInVideo.y -= prcRecordArea->top;
            }
        }

        // Cursor size
        m_ptCursorSize.x = ::GetSystemMetrics(SM_CXCURSOR);
        m_ptCursorSize.y = ::GetSystemMetrics(SM_CYCURSOR);


        PixelFormat BitmapFormat = PixelFormat32bppARGB;
        Bitmap bmCursor3(m_ptCursorSize.x, m_ptCursorSize.y, BitmapFormat);
        Graphics gCursor(&bmCursor3);
        gCursor.Clear(Color::Transparent);

        HDC hdcCursor3 = gCursor.GetHDC();
        ::DrawIcon(hdcCursor3, 0, 0, ci.hCursor);
        gCursor.ReleaseHDC(hdcCursor3);


        int iThisDataLength = m_ptCursorSize.x * m_ptCursorSize.y * 4;

        if (m_pMouseBgraData == NULL || iThisDataLength != m_iMouseDataLength) {
            if (m_pMouseBgraData != NULL)
                delete[] m_pMouseBgraData;

            m_pMouseBgraData = new BYTE[iThisDataLength];
            m_iMouseDataLength = iThisDataLength;
        }
        
        /* Does not seem to work?? All is black.
        BitmapData data;
        Rect rcAll(0, 0, m_ptCursorSize.x, m_ptCursorSize.y);
        Status stOk = bmCursor3.LockBits(&rcAll, ImageLockModeRead, BitmapFormat, &data);
        BYTE *pCursorArgbData = (BYTE *)data.Scan0;

        _ASSERT(stOk == Ok);
        */
        
        Color clrOneNonTransparent;
        bool bFoundOneNonTransparent = false;
        bool bFoundTwoNonTransparent = false;

        Color clrPixel;

        // Reorder/copy pixel data: ARGB -> BGRA
        for (int y=0; y<m_ptCursorSize.y; ++y) {
            for (int x=0; x<m_ptCursorSize.x; ++x) {

                int iIdx = (y*m_ptCursorSize.x + x) * 4;

                bmCursor3.GetPixel(x, y, &clrPixel);

                // BGRA is expected by lsgc
                /*
                m_pMouseBgraData[iIdx] = pCursorArgbData[iIdx+3];
                m_pMouseBgraData[iIdx+1] = pCursorArgbData[iIdx+2];
                m_pMouseBgraData[iIdx+2] = pCursorArgbData[iIdx+1];
                m_pMouseBgraData[iIdx+3] = pCursorArgbData[iIdx];
                */
                m_pMouseBgraData[iIdx] = clrPixel.GetBlue();
                m_pMouseBgraData[iIdx+1] = clrPixel.GetGreen();
                m_pMouseBgraData[iIdx+2] = clrPixel.GetRed();
                m_pMouseBgraData[iIdx+3] = clrPixel.GetAlpha();

                bool bTransparent = m_pMouseBgraData[iIdx+3] != 0xff;

                if (!bTransparent) {
                    //clrOneNonTransparent.SetValue(((ARGB *)(pCursorArgbData + iIdx))[0]);

                    if (!bFoundOneNonTransparent) {
                        clrOneNonTransparent = clrPixel;

                        bFoundOneNonTransparent = true;
                    } else if (clrPixel.GetValue() != clrOneNonTransparent.GetValue()) {
                        bFoundTwoNonTransparent;
                    }
                }
            }
        }

        //bmCursor3.UnlockBits(&data);

        Color clrTextCursorBugGray(242, 244, 243);
        if (bFoundOneNonTransparent && !bFoundTwoNonTransparent 
            && clrOneNonTransparent.GetValue() == clrTextCursorBugGray.GetValue()) {
            // Only one non-transparent pixel with this light gray found.
            // Fix the text cursor being gray instead of black:

            for (int y=0; y<m_ptCursorSize.y; ++y) {
                for (int x=0; x<m_ptCursorSize.x; ++x) {

                    int iIdx = (y*m_ptCursorSize.x + x) * 4;
                    bool bTransparent = m_pMouseBgraData[iIdx+3] != 0xff;

                    if (!bTransparent)
                        memset(m_pMouseBgraData + iIdx, 0, 3); // 0 = black
                }
            }
        }

        if (bSuccess && iconInfo.hbmColor != NULL)
            ::DeleteBitmap(iconInfo.hbmColor);

        if (bSuccess && iconInfo.hbmMask != NULL)
            ::DeleteBitmap(iconInfo.hbmMask);
    }
}

int SrMouseData::WriteData(BYTE *pBufferEnd) {
    if (pBufferEnd != NULL) {
        // A little bit hacky: this data is appended to the very end of a buffer

        int iSecurityMarginForExtraInfo = 100000;

        if (m_iMouseDataLength < iSecurityMarginForExtraInfo) {
            BYTE *pDataBegin = pBufferEnd - 10;

            short *pValueBuffer = (short *)pDataBegin;
            pValueBuffer[0] = (short)m_ptPositionInVideo.x;
            pValueBuffer[1] = (short)m_ptPositionInVideo.y;
            pValueBuffer[2] = (short)m_ptCursorSize.x;
            pValueBuffer[3] = (short)m_ptCursorSize.y;

            pDataBegin[8] = 'M';
            pDataBegin[9] = 'o';

            // Image data size correct for ARGB data?
            _ASSERT(m_iMouseDataLength == m_ptCursorSize.x * m_ptCursorSize.y * 4);

            pDataBegin -= m_iMouseDataLength;
            memcpy(pDataBegin, m_pMouseBgraData, m_iMouseDataLength);

            return m_iMouseDataLength + 10;
        }
        // else pointer data is too big; ignored silently
        // However this should normally not happen: standard cursor size is 32x32 (=4000 bytes)
    }

    return 0;
}

SrClickData::SrClickData() {
    m_iCurrentClickCount = 0;
}

SrClickData::~SrClickData() {
    // No deletion necessary; everthing's a stack object
}


void SrClickData::UpdateData(CLICKDATA *aCurrentClicks, 
                             int iCurrentClickCount, int iCurrentTimeMs,
                             int iAnimationMaxRadius, int iAnimationDurationMs, RECT *prcVideoOnScreen) {
    if (aCurrentClicks == NULL)
        return;

    if (iAnimationDurationMs == 0) {
        m_iCurrentClickCount = 0;
        return;
    }

    m_iCurrentClickCount = iCurrentClickCount;

    int iOffsetX = 0;
    int iOffsetY = 0;

    if (prcVideoOnScreen != NULL) {
        iOffsetX = prcVideoOnScreen->left;
        iOffsetY = prcVideoOnScreen->top;
    }

    for (int i=0; i<iCurrentClickCount; ++i) {
        m_aptPositionInVideo[i].x = aCurrentClicks[i].clickPos.x - iOffsetX;
        m_aptPositionInVideo[i].y = aCurrentClicks[i].clickPos.y - iOffsetY;

        m_abIsLeftMouseButton[i] = (aCurrentClicks[i].mouseFlags & MH_LEFT_BUTTON) != 0;

        int iDistanceMs = iCurrentTimeMs - aCurrentClicks[i].timeStamp;
        double dRadius = iAnimationMaxRadius * (iDistanceMs / (double) iAnimationDurationMs);
        if (dRadius > iAnimationMaxRadius)
            dRadius = iAnimationMaxRadius;
        if (dRadius < 1)
            dRadius = 1;

        m_aiRadius[i] = (int)dRadius;
    }
}

int SrClickData::WriteData(BYTE *pBufferEnd) {
    if (pBufferEnd == NULL)
        return 0;

    int iDataLength = m_iCurrentClickCount * 6 + 4;

    BYTE *pData = pBufferEnd - iDataLength;
    for (int i=0; i<m_iCurrentClickCount; ++i) {
        short *pValues = (short *)pData;

        pValues[0] = (short)m_aptPositionInVideo[i].x;
        pValues[1] = (short)m_aptPositionInVideo[i].y;
        pData += 4;

        pData[0] = m_abIsLeftMouseButton[i] ? 0xff : 0;
        pData[1] = (unsigned char)m_aiRadius[i];
        pData += 2;
    }

    short *pValues = (short *)pData;
    pValues[0] = (short)m_iCurrentClickCount;

    pData[2] = 'C';
    pData[3] = 'k';

    return iDataLength;
}

/* ========================================================================= */
/*  C O N S T R U C T O R                                                    */
/* ========================================================================= */

ScreenCapturer::ScreenCapturer(_TCHAR *tszFcc, VideoCapturer *pVideoCapturer, CLiveContext *pLiveContext) : VideoCapturer(-1),
   pVideoCapturer_(pVideoCapturer)
{
    m_pThreadStore = NULL;
    m_pLiveContext = pLiveContext;
    if (m_pLiveContext != NULL && m_pLiveContext->IsLiveContextMode()) {
        m_pThreadStore = new CThreadStore();
    }
    m_pDebugLog = new CDebugLog();
    m_pDebugLog->Init(_T("IMC_LEC_LC_LOG"));

#ifdef _DEBUG
   cout << "ScreenCapturer()" << endl;
#endif
   if (!registerFrameWndClass())
      throw exception("Could not register WNDCLASS");

   GdiplusStartupInput gdiplusStartupInput;
   Status stOk = GdiplusStartup(&m_pGdiplus, &gdiplusStartupInput, NULL);

   _ASSERT(stOk == Ok);

   // initialize the VideoCapturer variables
   initVariables();

   // now, init ScreenCapturer custom variables:
   paVideoHdr_      = NULL;

   pPngClsid        = NULL;

   m_bIsLsgc        = false;

   hBufferEvent_    = NULL;

   // annotation variables:
   nCurrentMode_    = MODE_INTERACTION;
   nAnnoTool_       = 0;
   bDeleteOnInteractionMode_ = true;
   bHideWhilePanning_        = false;
   m_pDrawWindow     = NULL;

   hbitmap_            = NULL;
   hdcBitmap_          = NULL;

   dwFirstTimeMs_      = -1;

   clickAnimMs_        = DEFAULT_ANIM_MS;
   clickAnimMaxSize_   = DEFAULT_ANIM_MAXSIZE;
   hRedPen_            = ::CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
   hBluePen_           = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
   doAudioClickEffect_ = true;
   doVideoClickEffect_ = true;

   // NOTE: This line may look verrry naughty, but is necessary:
   // In the VideoCapturer, videoFormat_ always contains a BITMAPINFO
   // structure that is given by the capture device. In case that
   // such a BITMAPINFO structure does not contain a palette
   // (biClrImportant == 0), then the BITMAPINFO structure only consists
   // of the BITMAPINFOHEADER member; the palette entry is not present.
   // Thus: The structure (which should be a BITMAPINFO structure) has
   // the size of a BITMAPINFOHEADER structure. In our case, we never
   // have a palette (minimum 16 Bit), so we need a BITMAPINFOHEADER
   // sized videoFormat_.
   // This kind of code can be found in the WdmCapturer also.
   videoFormat_     = (BITMAPINFO*) malloc(sizeof BITMAPINFOHEADER);
   videoFormatSize_ = sizeof BITMAPINFOHEADER;

   pChangedRectsLast_   = new CRectKeeper();
   pChangedRectsNow_    = new CRectKeeper();
   bQuickCaptureMode_   = false;
   qcFrameInterval_     = 10;
   qcLastCompleteFrame_ = 0;
   hQcBackup_           = NULL;
   hQcBackupDc_         = NULL;

   updateScreenFormat(320, 240);

   // Do we need to set a codec?
   if (tszFcc)
   {
      int nCodecs = getCodecListSize();
      CodecInfo **paCodecs = new CodecInfo*[nCodecs];
      for (int i=0; i<nCodecs; ++i)
         paCodecs[i] = new CodecInfo;
      getCodecList(paCodecs);

      int nDesiredCodec = -1;

      for (i=0; i<nCodecs; ++i)
      {
         if (_tcscmp(tszFcc, paCodecs[i]->fccString) == 0)
            nDesiredCodec = i;
      }

      if (nDesiredCodec != -1)
      {
         paCodecs[nDesiredCodec]->iQuality = 10000;
         paCodecs[nDesiredCodec]->iKeyframeRate = 25;
         paCodecs[nDesiredCodec]->bUseKeyframes = true;
         setCodec(paCodecs[nDesiredCodec]);
      }
      else
      {
         _TCHAR t[256];
         _stprintf(t, _T("Codec '%s' not found for current settings.\nNo codec has been set!"), tszFcc);
         MessageBox(NULL, t, _T("Error"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
      }

      for (i=0; i<nCodecs; ++i)
         delete paCodecs[i];
      delete[] paCodecs;
      paCodecs = NULL;
   }

   // by default: do not use automatic frame rate adaption:
   setUseAutoFrameRate(false);
   // Default frame rate: 5 fps.
   lastSetFrameRate_ = 5.0;

   // load the click icons:
   hClickIcon_ = LoadIcon(g_hDllInstance, MAKEINTRESOURCE(IDI_CLICK));
   if (hClickIcon_ == NULL)
   {
      _TCHAR t[256];
      _stprintf(t, _T("Loading left click icon failed.\nGetLastError() == 0x%x"), GetLastError());
      MessageBox(NULL, t, _T("Warning"), MB_ICONEXCLAMATION | MB_OK | MB_TOPMOST);
   }
   hRClickIcon_ = LoadIcon(g_hDllInstance, MAKEINTRESOURCE(IDI_RCLICK));
   if (hRClickIcon_ == NULL)
   {
      _TCHAR t[256];
      _stprintf(t, _T("Loading right click icon failed.\nGetLastError() == 0x%x"), GetLastError());
      MessageBox(NULL, t, _T("Warning"), MB_ICONEXCLAMATION | MB_OK | MB_TOPMOST);
   }
   bClickIconEnabled_ = true;
	bIsCustomSet = false;

    m_pFileSgActions = NULL;
    m_lLastMouseMove = 0;
    m_bIsDragging = false;
    m_iLastX = -1;
    m_iLastY = -1;

    m_mapLcElementInfo.InitHashTable(59, FALSE);
}

/* ========================================================================= */
/*  D E S T R U C T O R                                                      */
/* ========================================================================= */

ScreenCapturer::~ScreenCapturer()
{
    if (m_pLiveContext != NULL && m_pLiveContext->IsLiveContextMode()) {
        SAFE_DELETE(m_pThreadStore);
    }
    SAFE_DELETE(m_pDebugLog);
    
#ifdef _DEBUG
   cout << "~ScreenCapturer()" << endl;
#endif

   GdiplusShutdown(m_pGdiplus);

   if (videoFormat_)
      free(videoFormat_);
   videoFormat_ = NULL;

   if (paVideoHdr_)
   {
      cerr << "**** paVideoHdr_ is not NULL at destruction!" << endl;
      // this should be NULL!
      deleteVHDRs();
   }

   SAFE_DELETE(pPngClsid);

   UnregisterClass(CAPFRAMEWNDCLASS, g_hDllInstance);

   NMouseHook::DestroyMouseHook();

   unregisterInstance();

   if (pChangedRectsLast_)
      delete pChangedRectsLast_;
   pChangedRectsLast_ = NULL;
   if (pChangedRectsNow_)
      delete pChangedRectsNow_;
   pChangedRectsNow_ = NULL;

   if (m_pDrawWindow) {
      m_pDrawWindow->DestroyWindow();
      delete m_pDrawWindow;
   }
   m_pDrawWindow = NULL;

   ::DeleteObject(hRedPen_);
   ::DeleteObject(hBluePen_);
   if ( hBufferEvent_ )
      CloseHandle(hBufferEvent_);

   if (m_pFileSgActions != NULL) 
       delete m_pFileSgActions;
   m_pFileSgActions = NULL;
   m_lLastMouseMove = 0;
   
   // Reset map for requested element infos
   EmptyLcElementInfoMap();

}

/* ========================================================================= */

bool ScreenCapturer::setScreenCaptureRect(RECT *pRect)
{
#ifdef _DEBUG
   cout << "ScreenCapturer::setScreenCaptureRect(pRect)" << endl;
#endif
   if (pRect->left > pRect->right)
      swap(pRect->left, pRect->right);
   if (pRect->top > pRect->bottom)
      swap(pRect->top, pRect->bottom);

   if (pRect->left < 0)
      pRect->left = 0;
   if (pRect->top < 0)
      pRect->top = 0;
   if (pRect->right > screenWidth_)
      pRect->right = screenWidth_;
   if (pRect->bottom > screenHeight_)
      pRect->bottom = screenHeight_;

   // lsgc does not need to have height/width multiples of 4 and/or 4,
   // neither does lsgc bother about small (<16 pixel) videos. says ulrich.
   // check for lsgc?
   m_bIsLsgc = _tcsnicmp(codecInfo_.fccString, _T("lsgc"), 4) == 0;
   if ( !m_bIsLsgc ) // not lsgc
   {
      // first divx fix: video has to be 16x16 or larger
      if (pRect->right - pRect->left < 16)
         pRect->right = pRect->left + 16;
      if (pRect->bottom - pRect->top < 16)
         pRect->bottom = pRect->top + 16;

      // divx fix: width has to be a multiple of 4, height a
      // multiple of 2. iv50 needs height multiple of 4. so do so.
      int width = pRect->right - pRect->left;
      int height = pRect->bottom - pRect->top;
      if (width % 4 != 0)
      {
         // make width a multiple of 4
         int mod = width % 4;
         
         pRect->left -= (4 - mod);
      }
      
      if (height % 4 != 0)
      {
         // make height a multiple of 4
         int mod = height % 4;
         
         pRect->top -= (4 - mod);
      }
   }
   else
   {
   }

   bool isOk = (pRect->right - pRect->left > 0) &&
               (pRect->bottom - pRect->top > 0);
   if (isOk)
   {
      memcpy(&captureRect_, pRect, sizeof RECT);
      updateScreenFormat(captureRect_.right - captureRect_.left,
                         captureRect_.bottom - captureRect_.top);
   }
   return isOk;
}

bool ScreenCapturer::getScreenCaptureRect(RECT *pRect)
{
   memcpy(pRect, &captureRect_, sizeof RECT);

   return true;
}

bool ScreenCapturer::chooseCaptureRect()
{
   CaptureRectChooser crc;
   RECT rect;
   if (crc.chooseCaptureRect(&rect))
   {
      return setScreenCaptureRect(&rect);
   }
   else
   {
      return false;
   }
}

/* ========================================================================= */

void ScreenCapturer::start(_TCHAR *tszFileName, AudioCapturer *audio, bool bStartPaused, bool bCaptureActions)
{
   // do we need some automatic frame rate adaption done?
   // Important: We _must_ do this before initStartVariables()
   // is called as some values are calculated using the frame
   // rate, though this has to be correct.
    
   if (!NMouseHook::InitMouseHook(this))
   {
      MessageBox(NULL, _T("Could not set Windows mouse hook.\nMouse pan will not work."), _T("Error"), MB_OK | MB_TOPMOST);
   }

   // Reset map for requested element infos
   EmptyLcElementInfoMap();

   if (useAutoFrameRate_)
   {
      double pixels = (captureRect_.right - captureRect_.left) *
                      (captureRect_.bottom - captureRect_.top);
      double givenPixels = autoFrameRateData_.nWidth * autoFrameRateData_.nHeight;

      double frameRate = (autoFrameRateData_.fDesiredRate * givenPixels) / pixels;

      if (frameRate > autoFrameRateData_.fMaxRate)
         frameRate = autoFrameRateData_.fMaxRate;
      if (frameRate < autoFrameRateData_.fMinRate)
         frameRate = autoFrameRateData_.fMinRate;

#ifdef _DEBUG
      cout << "Auto Frame Rate set to " << frameRate << "fps." << endl;
#endif

      VideoCapturer::setFrameRate(frameRate);
   }
   else
   {
      setFrameRate(lastSetFrameRate_);
   }

   if (bQuickCaptureMode_)
   {
      double fr = (1000000.0 / ((double) getUSecsPerFrame()));
      qcFrameInterval_ = (int) (fr + 0.99);
      qcLastCompleteFrame_ = - 2 * qcFrameInterval_;

      pChangedRectsLast_->Clear();
      pChangedRectsNow_->Clear();
   }

   initStartVariables(tszFileName, audio);

   // overwrite isPaused_ with value from bStartPaused
   isPaused_ = bStartPaused;

   // true if Ctrl+Shift are pressed
   m_bDoPanning = false;

   // We're not yet mouse panning
   m_bIsPanningActive = false;

   // Neither are we dragging the window:
   isDragging_ = false;

   // Nor do we need a click icon to start with:
   dwInsertClickIcon_ = 0;

   // no clicks yet, as said.
   m_iActiveClickCount = 0;

   // reset all click data containers:
   for (int i=0; i<MAX_CLICKS; ++i)
   {
      clickData_[i].frameCount = 0;
      clickData_[i].timeStamp  = 0;
      clickData_[i].mouseFlags = 0;
      clickData_[i].clickPos.x = 0;
      clickData_[i].clickPos.y = 0;
   }

   // Initialize Whiteboard to capture Screenshots and mouse movements
   // This is only be done if bCaptureActions is true
   if (bCaptureActions) {
       CString csPathFilePrefix = szFileName_;
       StringManipulation::GetPathFilePrefix(csPathFilePrefix);

       // determine path used to store pointer cursor background, 
       // which is used to restore screenshots without pointer
       m_csImageFilepath = szFileName_;
       StringManipulation::GetPathFilePrefix(m_csImageFilepath);

       CString csSgActionsFilename = csPathFilePrefix;
       csSgActionsFilename += _T(".sga");
       m_pFileSgActions = new CFileOutput;
       HRESULT hr = m_pFileSgActions->OpenForWrite(csSgActionsFilename);
       if (!SUCCEEDED(hr))
          m_pFileSgActions = NULL;
       m_lLastMouseMove = 0;
   }
   
   // stop has not qet been requested...
   bStopRequested_ = false;
   m_bStopLcCommunication = false;

   // ok, we're off... let's rumble
   captureInProgress_ = true;

   // init VIDEOHDR structures:
   initVHDRs();

   // reset the current buffers:
   nCurrentGrabBuffer_     = 0;
   nCurrentCompressBuffer_ = 0;

   // create the notification event handle:
   hBufferEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
   if (!hBufferEvent_)
   {
      // TODO: Output some error message
      cerr << "*** CreateEvent for hBufferEvent_ in ScreenCapturer failed!" << endl;
      return;
   }

   // create an annotation window:
   if (m_pDrawWindow) {
       m_pDrawWindow->DestroyWindow();
       delete m_pDrawWindow;
   }
   m_pDrawWindow = new CNewDrawWindow(audio, tszFileName);
   m_pDrawWindow->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 
                           AfxRegisterWndClass(0,0,0,0), _T("CDrawWindow"), WS_POPUP, captureRect_, NULL, 0);

   hWindowCreatedEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);

   m_hFileSgActionsEvent = CreateEvent( 
       NULL,         // default security attributes
       TRUE,         // manual-reset event
       TRUE,        // initial state is signaled
       _T("WriteFileSgActionsEvent")  // object name
       ); 


   _beginthread(FrameWindowThread, 0, this);

   // wait max. 15 secs. for window to be created.
   ::WaitForSingleObject(hWindowCreatedEvent_, 15000);
   ::CloseHandle(hWindowCreatedEvent_);
   hWindowCreatedEvent_ = NULL;

   setInteractionMode();

   _beginthread(GrabberThreadLauncher, 0, this);
   _beginthread(CompressorThread, 0, this);
 
   if (GetLiveContext() != NULL && GetLiveContext()->IsLiveContextMode()) 
       _beginthread(LcClickRequestThread, 0, this);
}

void ScreenCapturer::stop() {
#ifdef _DEBUG
    cout << "ScreenCapturer::stop()" << endl;
#endif

    // If we're not capturing, we don't need to stop,
    // do we?
    if (!captureInProgress_)
        return;

    // Make the two worker threads quit:
    bStopRequested_ = true;

    if (m_pDrawWindow)
        m_pDrawWindow->Hide();

    // And close the flashy capture frame window:
    ::PostMessage(hFlashWnd_, WM_QUIT, 0, 0);

    // Wait for GrabberThread, CompressorThread and Mouse Hook Thread to stop
    while (!bGrabThreadStopped_ || !bCompressThreadStopped_) {
        Sleep(50);
    }

    
    // do not close here in LC Mode, because still required to write delayed LC responces
    // -> moved at end of WaitClickElementInfo()
    // Close action file here if not in LC Mode
    if (!(GetLiveContext() != NULL && GetLiveContext()->IsLiveContextMode())) {
       if (m_pFileSgActions)
           delete m_pFileSgActions;
       m_pFileSgActions = NULL;
       m_lLastMouseMove = 0;
    }

    // Close the AVI file:
    closeAviFile();

    // Delete the video buffers for the screen shots:
    deleteVHDRs();

    if (bQuickCaptureMode_) {
        // Free the quick capture backup buffer
        if (hQcBackup_ != NULL)
            ::DeleteObject(hQcBackup_);
        hQcBackup_ = NULL;
        if (hQcBackupDc_ != NULL)
            ::DeleteDC(hQcBackupDc_);
        hQcBackupDc_ = NULL;
    }

    // Delete the bitmap for the current screen:
    if (hbitmap_)
        ::DeleteObject(hbitmap_);
    hbitmap_ = NULL;
    if (hdcBitmap_)
        ::DeleteDC(hdcBitmap_);
    hdcBitmap_ = NULL;

    NMouseHook::DestroyMouseHook();

	if (m_pDrawWindow) {
       m_pDrawWindow->DestroyWindow();
       delete m_pDrawWindow;
	   m_pDrawWindow = NULL;
   }

    // And we're done:
    captureInProgress_ = false;

}

/* ========================================================================= */

void ScreenCapturer::setUseAutoFrameRate
   (bool bEnable, int nWidth, int nHeight, float fRate, float fMaxRate, float fMinRate)
{
   useAutoFrameRate_ = bEnable;

   if (useAutoFrameRate_)
   {
      autoFrameRateData_.fDesiredRate = fRate;
      autoFrameRateData_.fMaxRate     = fMaxRate;
      autoFrameRateData_.fMinRate     = fMinRate;
      autoFrameRateData_.nWidth       = nWidth;
      autoFrameRateData_.nHeight      = nHeight;
   }
}

TIME_US ScreenCapturer::setFrameRate(double frameRate)
{
   lastSetFrameRate_ = (float) frameRate;

   if (useAutoFrameRate_)
      return 1000000i64;

   return VideoCapturer::setFrameRate(frameRate);
}

/* ========================================================================= */

// --------------------------------- //
// VideoCapturer Overridden methods
// --------------------------------- //

/* ========================================================================= */

/* ========================================================================= */

/* ========================================================================= */

void ScreenCapturer::WaitClickElementInfo(CSingleProgressDialog *pProgress) {

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (m_pLiveContext != NULL 
        && m_pLiveContext->IsLiveContextMode() 
        && CLiveContextCommunicator::GetInstance()->Connected()) {

        int iElementCount = m_mapLcElementInfo.GetCount();
        if (pProgress != NULL) {
            pProgress->SetRange(0, iElementCount * 2);
            CString csLabel;
            csLabel.LoadString(IDS_WAIT_FOR_CLICKS);
            pProgress->SetLabel(csLabel);
        }
        int iTimeoutCounter = 0;
        while (m_mapLcElementInfo.GetCount() > 0 && iTimeoutCounter < 100) {
            m_pDebugLog->LogEntryInt(_T("Progress: "), (iElementCount - m_mapLcElementInfo.GetCount()) * 2 + 1);
            pProgress->SetPos((iElementCount - m_mapLcElementInfo.GetCount()) * 2 + 1);
            Sleep(100);
            ++iTimeoutCounter;
        }

        m_bStopLcCommunication = true;

        m_lLastMouseMove = 0;

        int iCount = 0;
        while (!m_bLcReceiveMessageStopped && iCount < 10) {
            m_pDebugLog->LogEntryInt(_T("WaitClickElementInfo wait: "), iCount);
            Sleep(100);
            ++iCount;
        }

        // TODO: Display message
        if (m_mapLcElementInfo.GetCount() > 0) {
            UINT nKey = 0;
            CLiveContextElementInfo *pInfo = NULL;
            POSITION iter = m_mapLcElementInfo.GetStartPosition();
            while (iter != NULL) {
                m_mapLcElementInfo.GetNextAssoc(iter, nKey, pInfo);
                if (pInfo != NULL)
                    ReportElementInfo(pInfo);
            }
            m_pDebugLog->LogEntryInt(_T("Click infos lost: "), m_mapLcElementInfo.GetCount());
        }

        // delete messages
        EmptyLcElementInfoMap();
        
    }
    
    // Close action file
    if (m_pFileSgActions)
        delete m_pFileSgActions;
    m_pFileSgActions = NULL;
}

/*
 * Virtual functions derived from iMouseHookListener
 */
void ScreenCapturer::MouseMoved(POINT ptMouse, DWORD dwFlag) {

    // If Ctrl And Shift are pressed, moving
    // the mouse moves the screen recording area.
    if (!bStopRequested_ && m_bDoPanning) {
        if (m_bIsPanningActive) 
            doMousePan();
        else 
            startMousePan();
    } else {
        if (m_bIsPanningActive)
            endMousePan();
    }

    if (UseMouseClick(ptMouse))
        WriteMoveActionEntry(ptMouse);

    TRACE("MouseMoved: %d %d\n", ptMouse.x, ptMouse.y);
}

void ScreenCapturer::MouseDown(POINT ptMouse, DWORD dwFlag) {
    m_bIsDragging = true;

    if (UseMouseClick(ptMouse)) {
        insertClickNoise(ptMouse, dwFlag);
        // Append new line to action file
        long lTimestamp = WriteClickActionEntry(ptMouse, dwFlag, MH_BUTTON_DOWN);
    }
    TRACE("MouseDown: %d %d\n", ptMouse.x, ptMouse.y);
}

void ScreenCapturer::MouseUp(POINT ptMouse, DWORD dwFlag) {
    m_bIsDragging = false;
    static int iLastTimestamp = 0;

    if (UseMouseClick(ptMouse)) {
        int iTimestamp = getVideoTime();
        // If there was a new recording started, reset timestamp
        if (iLastTimestamp > iTimestamp)
            iLastTimestamp = 0;

        // Throw clicks away if they are too close together
        if (iLastTimestamp == 0 || (iTimestamp - iLastTimestamp) >= 200) {
            long lWriteTimestamp = WriteClickActionEntry(ptMouse, dwFlag, MH_BUTTON_UP);

            // if is livecontext mode and connected, and a click/doubleclick was made inside selected area.
            if (IsLiveContextConnected() && nCurrentMode_ == MODE_INTERACTION) {
                TRACE("RequestElementInfoFromLiveContext(lWriteTimestamp=%d, ptMouse(%d,%d))\n", lWriteTimestamp, ptMouse.x, ptMouse.y);
                RequestElementInfoFromLiveContext(lWriteTimestamp, ptMouse);
            }
            iLastTimestamp = iTimestamp;
        }
    }
    TRACE("MouseUp: %d %d - currMode=%d\n", ptMouse.x, ptMouse.y, nCurrentMode_);
}

void ScreenCapturer::MouseDblClick(POINT ptMouse, DWORD dwFlag) {
    m_bIsDragging = false;

    TRACE("WM_BUTTONDBLCLK\n");
    if (UseMouseClick(ptMouse)) {
        insertClickNoise(ptMouse, dwFlag);

        // Append new line to action file
        long lTimestamp = WriteClickActionEntry(ptMouse, dwFlag, MH_BUTTON_DBLCLICK);

        // if is livecontext mode and connected, and a click/doubleclick was made inside selected area.
        if (IsLiveContextConnected() && nCurrentMode_ == MODE_INTERACTION)
            RequestElementInfoFromLiveContext(lTimestamp, ptMouse); 
    }
    TRACE("MouseDblClick: %d %d - currMode=%d\n", ptMouse.x, ptMouse.y, nCurrentMode_);
}

void ScreenCapturer::SetPanningMode(bool bDoPanning) {
    m_bDoPanning = bDoPanning;
    TRACE("SetPanningMode: %d\n", bDoPanning);
}

void ScreenCapturer::ParseElementInfo(const char *szMessage, DWORD dwRead) {
    CLiveContextElementInfo *pInfo = new CLiveContextElementInfo(CPoint(0, 0));
    pInfo->ParseJsonString(szMessage, dwRead);

    // Find the ElementInfo we wait for and write
    CLiveContextElementInfo *pSavedInfo;
    BOOL bFound = m_mapLcElementInfo.Lookup(pInfo->GetID(), pSavedInfo);
    if (bFound && pSavedInfo != NULL) {
        ReportElementInfo(pInfo);
        SAFE_DELETE(pSavedInfo);
        m_mapLcElementInfo.RemoveKey(pInfo->GetID());
    }

    SAFE_DELETE(pInfo);
}

/* ========================================================================= */
/* STATIC PRIVATE FUNCTIONS                                                         */
/* ========================================================================= */

LRESULT CALLBACK ScreenCapturer::FrameWindowProc(HWND hWnd, UINT uMsg,
                                                 WPARAM wParam, LPARAM lParam)
{
    TRACE("FrameWindowProc: %d\n", uMsg);
   switch (uMsg)
   {
   case WM_CREATE:
      // Initialize the frame color to imc orange here
      {
         LPCREATESTRUCT lpcs = (LPCREATESTRUCT) lParam;
         ScreenCapturer *sc = (ScreenCapturer *) lpcs->lpCreateParams;// retrieveInstance(hWnd);
         sc->cFrameColor_        = sc->isPaused_ ? RGB(128, 128, 128) : ORANGE;
         sc->bFrameColorIsBlack_ = false;

         // register this HWND with the screen capturer:
         sc->registerInstance(hWnd);
      }
      break;

   case WM_QUIT:
   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   case WM_CLOSE:
      // we do not want the window to react on Alt-F4 calls:
      return 0;

   case WM_SIZE:
      // At WM_SIZE, set the new window region (frame with
      // empty inner part
      {
         RECT rect;
         ::GetClientRect(hWnd, &rect);
         ScreenCapturer *sc = (ScreenCapturer *) retrieveInstance(hWnd);

         /*
         HRGN hrgn1 = ::CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
         HRGN hrgn2 = ::CreateRectRgn(rect.left + FRAMESIZE, rect.top + FRAMESIZE, 
                                      rect.right - FRAMESIZE, rect.bottom - FRAMESIZE);
         ::CombineRgn(hrgn1, hrgn1, hrgn2, RGN_XOR);
         ::SetWindowRgn(hWnd, hrgn1, TRUE);
         ::DeleteObject(hrgn1);
         ::DeleteObject(hrgn2);
         */

         HRGN h1 = ::CreateRectRgn(rect.left, rect.top, rect.left + FRAMEWIDTH, rect.top + FRAMESIZE);
         HRGN h2 = ::CreateRectRgn(rect.left, rect.top, rect.left + FRAMESIZE, rect.top + FRAMEWIDTH);
         ::CombineRgn(h1, h1, h2, RGN_OR);
         HRGN h3 = ::CreateRectRgn(rect.left, rect.bottom - FRAMEWIDTH, rect.left + FRAMESIZE, rect.bottom);
         HRGN h4 = ::CreateRectRgn(rect.left, rect.bottom - FRAMESIZE, rect.left + FRAMEWIDTH, rect.bottom);
         ::CombineRgn(h3, h3, h4, RGN_OR);
         HRGN h5 = ::CreateRectRgn(rect.right - FRAMEWIDTH, rect.top, rect.right, rect.top + FRAMESIZE);
         HRGN h6 = ::CreateRectRgn(rect.right - FRAMESIZE, rect.top, rect.right, rect.top + FRAMEWIDTH);
         ::CombineRgn(h5, h5, h6, RGN_OR);
         HRGN h7 = ::CreateRectRgn(rect.right - FRAMESIZE, rect.bottom - FRAMEWIDTH, rect.right, rect.bottom);
         HRGN h8 = ::CreateRectRgn(rect.right - FRAMEWIDTH, rect.bottom - FRAMESIZE, rect.right, rect.bottom);
         ::CombineRgn(h7, h7, h8, RGN_OR);
         ::CombineRgn(h1, h1, h3, RGN_OR);
         ::CombineRgn(h5, h5, h7, RGN_OR);
         ::CombineRgn(h1, h1, h5, RGN_OR);
         ::SetWindowRgn(hWnd, h1, TRUE);
         ::DeleteObject(h1);
         ::DeleteObject(h2);
         ::DeleteObject(h3);
         ::DeleteObject(h4);
         ::DeleteObject(h5);
         ::DeleteObject(h6);
         ::DeleteObject(h7);
         ::DeleteObject(h8);
      }
      break;

   case WM_PAINT:
      // Draw the frame
      {
         // MessageBox(NULL, "Flupp", NULL, MB_OK);
         HDC hdc = ::GetDC(hWnd);
         RECT rect;
         ::GetClientRect(hWnd, &rect);
         HBRUSH brush = CreateSolidBrush(((ScreenCapturer *) retrieveInstance(hWnd))->cFrameColor_);
         ::FillRect(hdc, &rect, brush);
         ::DeleteObject(brush);
         ::ReleaseDC(hWnd, hdc);
      }
      break;

   case WM_NCHITTEST:
      return HTCLIENT;

   case WM_SETCURSOR:
      // Make the cursor the "SIZEALL" cursor with
      // arrows in all directions
      SetCursor(LoadCursor(NULL, IDC_SIZEALL));
      return TRUE;

   case WM_MOVE:
      {
         // If the frame around the capture area is moved,
         // notify the ScreenCapturer that the capture area
         // has changed:
         RECT rect;
         POINTS point = MAKEPOINTS(lParam);
         ScreenCapturer *sc = (ScreenCapturer *) retrieveInstance(hWnd);

         int width = sc->captureRect_.right - sc->captureRect_.left;
         int height = sc->captureRect_.bottom - sc->captureRect_.top;

         rect.left   = point.x + FRAMESIZE + FRAMESPACE;
         rect.top    = point.y + FRAMESIZE + FRAMESPACE;
         rect.right  = rect.left + width;
         rect.bottom = rect.top + height;

         sc->captureRect_ = rect;
      }
      break;

   case WM_LBUTTONDOWN:
      {
         ScreenCapturer *sc = (ScreenCapturer *) retrieveInstance(hWnd);
         sc->isDragging_ = true;
         if (sc->m_pDrawWindow && sc->nCurrentMode_ == MODE_ANNOTATION)
            sc->m_pDrawWindow->Hide();
         ::SetCapture(sc->hFlashWnd_);

         ::GetCursorPos(&sc->dragStartPoint_);
         ::GetWindowRect(sc->hFlashWnd_, &sc->dragStartWndPos_);
      }
      break;

   case WM_MOUSEMOVE:
      {
         ScreenCapturer *sc = (ScreenCapturer *) retrieveInstance(hWnd);
         if (sc->isDragging_)
         {
            static POINT cursorPos;
            static int deltaX, deltaY;
            ::GetCursorPos(&cursorPos);

            deltaX = cursorPos.x - sc->dragStartPoint_.x;
            deltaY = cursorPos.y - sc->dragStartPoint_.y;

            ::SetWindowPos(sc->hFlashWnd_, NULL, 
                           sc->dragStartWndPos_.left + deltaX,
                           sc->dragStartWndPos_.top + deltaY,
                           0, 0,
                           SWP_NOZORDER | SWP_NOSIZE);
         } 
      }
      break;

   case WM_LBUTTONUP:
      {
         ScreenCapturer *sc = (ScreenCapturer *) retrieveInstance(hWnd);
         sc->isDragging_ = false;
         ::ReleaseCapture();

         if (sc->m_pDrawWindow && sc->nCurrentMode_ == MODE_ANNOTATION)
         {
            sc->m_pDrawWindow->SetTopLeft(sc->captureRect_.left, sc->captureRect_.top);
            sc->m_pDrawWindow->Show();
         }
      }
      break;

   case WM_TIMER:
      {
         // Update the color of the frame at every WM_TIMER
         // call. These occur every 500 ms.
         ScreenCapturer *sc = (ScreenCapturer *) retrieveInstance(hWnd);

         // If we're dragging the window around, do not
         // hide it. Would not be so good.
         //if (sc->isDragging_)
         //   break;

         sc->bFrameColorIsBlack_ = !sc->bFrameColorIsBlack_;
         if (sc->bFrameColorIsBlack_)
            sc->cFrameColor_ = RGB(0, 0, 0);
         else 
         {
            if (!sc->isPaused_)
               sc->cFrameColor_ = ORANGE;
            else
               sc->cFrameColor_ = RGB(128, 128, 128);
         }
         ::InvalidateRect(hWnd, NULL, FALSE);
         TRACE("Blink\n");
         //sc->displayCaptureFrame(sc->bFrameColorIsBlack_);
      }
      break;
   }

   return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void ScreenCapturer::FrameWindowThread(void *lpData)
{
#ifdef _DEBUG
   cout << "ScreenCapturer::FrameWindowThread()" << endl;
#endif
   CLanguageSupport::SetThreadLanguage();

   ((ScreenCapturer *) lpData)->frameWindowLoop();
}

void ScreenCapturer::GrabberThreadLauncher(void *lpData)
{
   CLanguageSupport::SetThreadLanguage();

   ScreenCapturer *sc = (ScreenCapturer *) lpData;
   sc->GrabberThread();
}

void ScreenCapturer::CompressorThread(void *lpData)
{
#ifdef _DEBUG
   cout << "ScreenCapturer::CompressorThread()" << endl;
#endif
   
   CLanguageSupport::SetThreadLanguage();

   ScreenCapturer *sc = (ScreenCapturer *) lpData;
   sc->bCompressThreadStopped_ = false;

   while (!sc->bStopRequested_)
   {
      // We're waiting for a frame to arrive
      DWORD dres = WaitForSingleObject(sc->hBufferEvent_, INFINITE);

      // Did we get one, or was it a fluke?
      VIDEOHDR* videoHdr = sc->paVideoHdr_[sc->nCurrentCompressBuffer_];
      while ((videoHdr->dwFlags & VHDR_DONE) != 0)
      {
         // Ok, we have a frame with valid data, call the
         // videoStreamCallback to let the user work with the
         // data. Here: Compress the data and write it to the
         // AVI file. Through hFlashWnd_ the videoStreamCallback
         // function can retrieve the current ScreenCapturer
         // instance.
         videoStreamCallback(sc->hFlashWnd_, videoHdr);

         // Reset the VHDR_DONE flag:
         videoHdr->dwFlags &= ~VHDR_DONE;

         // Update the compress buffer counter:
         ++sc->nCurrentCompressBuffer_;
         if (sc->nCurrentCompressBuffer_ >= getVideoBuffers())
            sc->nCurrentCompressBuffer_ = 0;
         // Look at the next video buffer:
         videoHdr = sc->paVideoHdr_[sc->nCurrentCompressBuffer_];

         // Let's yield a bit:
         Sleep(0);
      }
   }

   // Notify the stop() method that this method has stopped
   sc->bCompressThreadStopped_ = true;
}

void __cdecl ScreenCapturer::LcClickRequestThread(void *lpData) {
    HRESULT hr = S_OK;

    ScreenCapturer *pScreenCapturer = (ScreenCapturer *)lpData;
    if (pScreenCapturer == NULL)
        return;

    pScreenCapturer->m_bLcReceiveMessageStopped = false;

    const UINT iBufferSize = 4096;
    CStringA csMessage;
    char szMessage[iBufferSize];
    DWORD dwRead = 0;
    while (SUCCEEDED(hr) && !pScreenCapturer->m_bStopLcCommunication) {
        csMessage.Empty();
        bool bFirstPart = true;
        bool bIsReplyMessage = false;

        // Read all content
        do {
            hr = CLiveContextCommunicator::GetInstance()->PeekElement(szMessage, iBufferSize, dwRead);
            if (SUCCEEDED(hr) && dwRead > 0) {
                if (bFirstPart) {
                    CString csReceivedMessage = szMessage;
                    int iPos;
                        iPos = csReceivedMessage.Find(_T("ELEMENT-INFO-REPLY"));
                        if (iPos > 0) {
                            bIsReplyMessage = true;
                        }
                    bFirstPart = false;
                }

                hr = CLiveContextCommunicator::GetInstance()->GetMessageString(szMessage, iBufferSize, dwRead);
                if (bIsReplyMessage) {
                    csMessage.Append(szMessage, dwRead);
                    if (dwRead < iBufferSize) {
                        break;
                    }
                } else
                    break; // Sleep also in case there is another (unwanted) message
            }
        } while (dwRead > 0 && SUCCEEDED(hr));

        if (!csMessage.IsEmpty()) {
            if (bIsReplyMessage) {
                    pScreenCapturer->ParseElementInfo(csMessage, csMessage.GetLength());
            }
        }
        Sleep(50);
    }

    pScreenCapturer->m_bLcReceiveMessageStopped = true;
    
}

/* ========================================================================= */
/* PRIVATE FUNCTIONS                                                         */
/* ========================================================================= */

bool ScreenCapturer::registerFrameWndClass()
{
   HINSTANCE hInstance = g_hDllInstance;

   WNDCLASSEX wndClassEx;
   memset(&wndClassEx, 0, sizeof WNDCLASSEX);

   wndClassEx.cbSize      = sizeof WNDCLASSEX;
   wndClassEx.style       = 0; // no special style required
   wndClassEx.lpfnWndProc = FrameWindowProc;
   wndClassEx.cbClsExtra  = 0;
   wndClassEx.cbWndExtra  = 0;
   wndClassEx.hInstance   = hInstance;
   wndClassEx.hIcon       = NULL; // LoadIcon(NULL, IDI_APPLICATION);
   HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEALL);
   wndClassEx.hCursor     = hCursor; // LoadCursor(NULL, IDC_CROSS);
   wndClassEx.hbrBackground = NULL; //GetStockObject(BLACK_BRUSH);
   wndClassEx.lpszMenuName  = NULL;
   wndClassEx.lpszClassName = CAPFRAMEWNDCLASS;
   wndClassEx.hIconSm       = NULL;

   ATOM atom = RegisterClassEx(&wndClassEx);
   
   if  (atom != NULL)
      return true;
   else
   {
      DWORD dwErrorCode = GetLastError();

      if (ERROR_CLASS_ALREADY_EXISTS == dwErrorCode)
         return true;
   }
   
   // some arbitrary error occured
   return false;
}

void ScreenCapturer::frameWindowLoop()
{
   HINSTANCE hInstance = g_hDllInstance;

   // Create a window for a flashy frame around the capture
   // area:
   hFlashWnd_ = CreateWindowEx
      (/*WS_EX_TRANSPARENT | */WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
       CAPFRAMEWNDCLASS,
       CAPFRAMEWNDCLASS, 
       WS_POPUP | WS_VISIBLE,
       captureRect_.left - FRAMESIZE - FRAMESPACE, 
       captureRect_.top - FRAMESIZE - FRAMESPACE,
       (captureRect_.right - captureRect_.left) + 2*(FRAMESIZE + FRAMESPACE),
       (captureRect_.bottom - captureRect_.top) + 2*(FRAMESIZE + FRAMESPACE),
       NULL,
       NULL,
       hInstance, 
       (LPVOID) this);

   // register this HWND with this instance:
   // registerInstance(hFlashWnd_); // done in WM_CREATE!!!

   // ok, we're done registering and creating the window,
   // set the corresponding event
   ::SetEvent(hWindowCreatedEvent_);

   if (hFlashWnd_)
   {
      // All went well: Display the window.
      ShowWindow(hFlashWnd_, SW_SHOW);
      UpdateWindow(hFlashWnd_);

      // Create a timer which sends WM_TIMER messages
      // every half a second to make the window flash.
      SetTimer(hFlashWnd_, 1234, 500, NULL);
   }

   // The window message loop:
   MSG msg;
   BOOL bRet;
   while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
   {    
       TRACE("GetMessage: %d\n", msg.message);
      if (bRet == -1) {
          // handle the error and possibly exit
      } else {

          TranslateMessage(&msg);
          DispatchMessage(&msg);
      }
   }

   // We're done, destroy the window:
   DestroyWindow(hFlashWnd_);

#ifdef _DEBUG
   cout << "### Left the flashy window" << endl;
#endif
}

void ScreenCapturer::GrabberThread()
{
#ifdef _DEBUG
   cout << "ScreenCapturer::GrabberThread()" << endl;
#endif
   bGrabThreadStopped_ = false;

   while (isPaused_ && !bStopRequested_) {
      Sleep(100);
   }

   nFrame_ = 0;
   // DWORD dwFirstTimeMs = -1;
   dwFirstTimeMs_ = -1;
   DWORD dwCurrentTimeMs = -1;
   DWORD dwNextFrameMs;
   long lSleepMs;

   // throw out the PREROLL event:
   controlCallback(hFlashWnd_, CONTROLCALLBACK_PREROLL);

   while (!bStopRequested_) {
      // Step 1: Do we have an empty buffer to play with?
      VIDEOHDR* videoHdr = paVideoHdr_[nCurrentGrabBuffer_];

      if (((videoHdr->dwFlags & VHDR_DONE) == 0) && (!isPaused_)) {
         // Yes, this one is ok.
         // We're not paused. Only really grab screen if the
         // recording is not paused.

         // Check for the time...
         dwCurrentTimeMs = getRefTime();
         if (dwFirstTimeMs_ == -1)
            dwFirstTimeMs_ = dwCurrentTimeMs;

         // Step 1a: Call the controlCallback handler:
         controlCallback(hFlashWnd_, CONTROLCALLBACK_CAPTURING);

         // Step 2: Grab the screen into the current buffer


         grabScreen(videoHdr->lpData, 
             audio_ != NULL ? audio_->getWaveInTimeMs() : dwCurrentTimeMs, 
             paMouseData_[nCurrentGrabBuffer_], paClickData_[nCurrentGrabBuffer_]);

         // Step 3: mark this buffer as done: And it's always a keyframe, too:
         videoHdr->dwFlags |= (VHDR_DONE | VHDR_KEYFRAME);

         // Step 3a: Quick capture mode: Add data to the header
         if (bQuickCaptureMode_ && !m_bIsLsgc) {
             // !m_bIsLsgc: Lsgc now separately stores information about mouse cursor and 
             //             mouse click visualization; so the respective rectangles do not
             //             need to be compressed.

            if (qcLastCompleteFrame_ != nFrame_) {
               // We're in "delta mode" of quick capture
               if (bQcNothingChanged_) {
                  // Nothing has changed in this frame
                  videoHdr->dwUser = VHDR_QC_NOCHANGE;
               } else {
                  // Something has changed. Now what?
                  videoHdr->dwUser = VHDR_QC_DATA;
                  // Write data into suiting QCDATA instance
                  paQcData_[nCurrentGrabBuffer_]->cbBufferSize = 
                     CRectKeeper::MergeAndOutput(*pChangedRectsLast_, *pChangedRectsNow_,
                                                 paQcData_[nCurrentGrabBuffer_]->pData, 1024);
                  videoHdr->dwReserved[0] = (DWORD) paQcData_[nCurrentGrabBuffer_];
               }
            } else {
               // QC "keyframe"
               videoHdr->dwUser = 0;
            }

            // swap the two rect keeper pointers:
            CRectKeeper *pRC   = pChangedRectsLast_;
            pChangedRectsLast_ = pChangedRectsNow_;
            pChangedRectsNow_  = pRC;
         } else if (m_bIsLsgc) {  
            // LSGC: use special mouse pointer handling
            videoHdr->dwUser = VHDR_CONTAINS_MOUSE | VHDR_CONTAINS_CLICK;

            videoHdr->dwReserved[1] = (DWORD)paMouseData_[nCurrentGrabBuffer_];
            videoHdr->dwReserved[2] = (DWORD)paClickData_[nCurrentGrabBuffer_];
         } else {
            videoHdr->dwUser = 0;
            videoHdr->dwReserved[1] = NULL;
         }

         // save the current time:
         videoHdr->dwTimeCaptured = (dwCurrentTimeMs - dwFirstTimeMs_);
      } else {
#ifdef _DEBUG
         if (!isPaused_)
            cout << "+++ could not grab a frame: no frames left" << endl;
#endif
      }

      // Step 4: Update frame number. Note: This is done even
      // if the recording is paused!
      ++nFrame_;
      TRACE("Grab: %d\n", nFrame_);

      // Step 5: Notify CompressorThread:
      SetEvent(hBufferEvent_);

      // Step 6: Wait for the next frame "opportunity"

      // Now, let's see how long we need to sleep for the next frame:
      // (a) calculate when the next frame _should_ occur
      dwNextFrameMs = nFrame_ * usecsPerFrame_ / 1000;

      // (b) what time is it now?
      dwCurrentTimeMs = getRefTime();

      lSleepMs = dwNextFrameMs - (dwCurrentTimeMs - dwFirstTimeMs_);

      if (lSleepMs >= 0) {
         // Good, we still have time for the next frame:
         Sleep(lSleepMs);
      } else {
         if ((-lSleepMs) <= (usecsPerFrame_ / 2000)) {
            // We're late, but less than the time for half a frame,
            // so let's give it a shot. Perhaps it will catch up.

            // But let's yield in order not to block the system
            // completely.
            Sleep(0);
            continue;
         }  else {
            // We are later than half a frame time, so we will
            // have to drop one or more frames. This will look
            // like a "hardware drop" for the videoStreamCallback
            int nFramesToSkip = 1000i64 * (-lSleepMs) / usecsPerFrame_;
            nFrame_ += nFramesToSkip;
            long lSyncSleepMs = (usecsPerFrame_ / 1000) - ((-lSleepMs) % (usecsPerFrame_ / 1000));

            // Sleep for a bit to get in sync for the next frame
            Sleep(lSyncSleepMs);
         }
      }

      ++nCurrentGrabBuffer_;
      if (nCurrentGrabBuffer_ >= getVideoBuffers())
          nCurrentGrabBuffer_ = 0;
   }

   // notify CompressorThread, just in case:
   SetEvent(hBufferEvent_);
   // notify the stop() method that we're done here:
   bGrabThreadStopped_ = true;
}

void ScreenCapturer::grabScreen(unsigned char *lpData, DWORD timeStamp, 
                                SrMouseData *pMouse, SrClickData *pClickData)
{
#ifdef _DEBUG
   //printf("ScreenCapturer::grabScreen()\n");
#endif
   // cursor size is only needed here and is constant:
   static int cursorSizeX = ::GetSystemMetrics(SM_CXCURSOR);
   static int cursorSizeY = ::GetSystemMetrics(SM_CYCURSOR);

   // ok, let's see...
   POINT ptCursor;

   // We'll copy the captureRect_ so that it cannot be 
   // changed during the execution time of this method
   RECT curRect;
   curRect = captureRect_;

   if (bQuickCaptureMode_)
   {
      pChangedRectsNow_->Clear();
   }

   int w = curRect.right - curRect.left;
   int h = curRect.bottom - curRect.top;

   // Get the DC for the current display:
   HDC hdc = ::GetDC(NULL);

   if (hbitmap_ == NULL)
   {
      // We do not have a bitmap yet
      // Create a bitmap which is compatible with the current
      // display settings:
      hbitmap_   = ::CreateCompatibleBitmap(hdc, w, h);
      // We also need a Device Context (DC) for that bitmap:
      hdcBitmap_ = ::CreateCompatibleDC(hdc);

      // Select the bitmap into the new context, so that we
      // can draw onto the bitmap:
      ::SelectObject(hdcBitmap_, hbitmap_);
   }

   if (!bQuickCaptureMode_)
   {
      // Copy the screen content to the bitmap
      ::BitBlt(hdcBitmap_, 0, 0, w, h, hdc, curRect.left, curRect.top, SRCCOPY);
   }
   if (bQuickCaptureMode_)
   {
      // Do we already have a backup buffer?
      if (hQcBackup_ == NULL)
      {
         // No, create one
         hQcBackup_   = ::CreateCompatibleBitmap(hdc, w, h);
         hQcBackupDc_ = ::CreateCompatibleDC(hdc);
         ::SelectObject(hQcBackupDc_, hQcBackup_);
      }

      if (qcLastCompleteFrame_ + qcFrameInterval_ <= getCurrentFrame())
      {
         // Copy the screen content to the bitmap
         ::BitBlt(hQcBackupDc_, 0, 0, w, h, hdc, curRect.left, curRect.top, SRCCOPY);

         qcLastCompleteFrame_ = nFrame_;
      }

      // Just copy the backup buffer back. This is usually much
      // faster than a VRAM-RAM-copy, as it is either a
      // VRAM-VRAM copy or a RAM-RAM copy. Normally it is a
      // RAM-RAM copy.
      ::BitBlt(hdcBitmap_, 0, 0, w, h, hQcBackupDc_, 0, 0, SRCCOPY);
   }

   // CString csString;
   // csString.Format(_T("%d : %d ms"), nFrame_, timeStamp/*-dwFirstTimeMs_*/);
   // Gdiplus::Font myFont(L"Arial", 16);
   // PointF origin(10.0f, 10.0f);
   // SolidBrush blackBrush(Color(255, 0, 0, 255));
   // Graphics graphics2(hdcBitmap_);
   // graphics2.DrawString(csString, -1, &myFont, origin, &blackBrush);

   // Try to get the current cursor, disregarding who is
   // owning it at the moment
   HCURSOR hCursor = getCurrentCursorHandle(ptCursor, NULL);

   /* removed: not fully working because not fully synchronized: should use "pausedTime_"
   // create images to remove mouse pointer from non-lcgs codec recordings for demo mode
   if ( !m_bIsLsgc && hCursor && !m_csImageFilepath.IsEmpty() )
   {
      // not required for LSGC

      // copy background of cursor area to restore screenshot without cursor in DEMO MODE
      // note: must be called before click visualization
      int x1 = ptCursor.x - curRect.left;
      int y1 = ptCursor.y - curRect.top;

      // Graphics graphics(hdcBitmap_);
      // Pen pen(Color::Blue);
      // graphics.DrawRectangle( &pen, x1, y1, cursorSizeX, cursorSizeY );

      if ( x1+cursorSizeX >= 0 && x1 < w && y1+cursorSizeY >= 0 && y1 < h ) {
         // create transparent bitmap
         Bitmap bitmap( w, h );
         Graphics graphics(&bitmap);
         graphics.Clear(Color::Transparent);

         // capture area behind mouse pointer
         HDC hCaptureDC = CreateCompatibleDC(hdcBitmap_);
         HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hdcBitmap_, cursorSizeX, cursorSizeY);
         SelectObject(hCaptureDC,hCaptureBitmap);
         BitBlt(hCaptureDC, 0,0, cursorSizeX, cursorSizeY, hdcBitmap_, x1, y1, SRCCOPY);

         // draw area behind mouse pointer
         Bitmap *pBitmap;
         pBitmap = Bitmap::FromHBITMAP(hCaptureBitmap, NULL);
         graphics.DrawImage( pBitmap, x1, y1 );
         // Pen pen(Color::Red);
         // graphics.DrawRectangle( &pen, x1, y1, cursorSizeX, cursorSizeY );

         // origin.Y = 50.0f;
         // blackBrush.SetColor(Color(255, 255, 0, 0));
         // graphics.DrawString(csString, -1, &myFont, origin, &blackBrush);

         // write mouse pointer remove png
         CString csFilename;
         long nFrameForFile = nFrame_ + dropCounterAudio_;
         csFilename.Format(_T("%s_%d.png"), m_csImageFilepath, nFrameForFile );
         if (!pPngClsid ) {
            pPngClsid = new CLSID();
            VideoInformation::GetEncoderClsid(L"image/png", pPngClsid);
         }
         bitmap.Save(csFilename, pPngClsid);

         // cleanup
         pBitmap->~Bitmap();
         DeleteDC(hCaptureDC);
         DeleteObject(hCaptureBitmap);
      }
   }
   */
   
   if (m_bIsLsgc && pClickData != NULL) {
       pClickData->UpdateData(clickData_, m_iActiveClickCount, timeStamp, 
           clickAnimMaxSize_, clickAnimMs_, &curRect);
   }
   // TODO use the above calculated data to paint below!


   bool bClicksPainted = false;

   // Do we need to insert a click visualization here?
   if (!m_bIsLsgc && m_iActiveClickCount > 0)
   {
      bClicksPainted = true;

      HPEN   hOldPen    = (HPEN) ::SelectObject(hdcBitmap_, hRedPen_);
      HBRUSH hNullBrush = (HBRUSH) ::GetStockObject(NULL_BRUSH);
      HBRUSH hOldBrush  = (HBRUSH) ::SelectObject(hdcBitmap_, hNullBrush);

      for (int i=0; i<m_iActiveClickCount; ++i)
      {
         //frame = clickData_[i].frameCount++;
         double size = ((double) clickAnimMaxSize_) * ((double) (timeStamp - clickData_[i].timeStamp)) 
            / ((double) clickAnimMs_);

         double radius = size < clickAnimMaxSize_ ? (int) size : clickAnimMaxSize_;

#ifdef _DEBUG
         printf("size: %f, radius: %d, timeStamp: %d, clickTime: %d\n", size, radius, timeStamp, clickData_[i].timeStamp);
#endif

         // left mouse button?
         if ((clickData_[i].mouseFlags & MH_LEFT_BUTTON) > 0)
            ::SelectObject(hdcBitmap_, hRedPen_);
         else // assume right mouse button
            ::SelectObject(hdcBitmap_, hBluePen_);

         short x1 = (short) clickData_[i].clickPos.x - radius - curRect.left;
         short y1 = (short) clickData_[i].clickPos.y - radius - curRect.top;
         short x2 = (short) clickData_[i].clickPos.x + radius - curRect.left;
         short y2 = (short) clickData_[i].clickPos.y + radius - curRect.top;
         ::Ellipse(hdcBitmap_, x1, y1, x2, y2);

         if (bQuickCaptureMode_)
         {
            if (x2 >= 0 && x1 <= w &&
                y2 >= 0 && y1 <= h)
            {
               if (x2 >= w) x2 = w-1;
               if (x1 <  0) x1 = 0;
               if (y2 >= h) y2 = h-1;
               if (y1 <  0) y1 = 0;
               // Flip the y values!
               pChangedRectsNow_->AddRect(QCRECT(x1, h-y1-1, x2, h-y2-1));
            }
         }
      }

      ::SelectObject(hdcBitmap_, hOldPen);
      ::SelectObject(hdcBitmap_, hOldBrush);
      ::DeleteObject(hNullBrush);
   }


   // Cleanup/update clicks array
   while (m_iActiveClickCount > 0 && timeStamp - clickData_[0].timeStamp >= clickAnimMs_) {
       for (int i=1; i<MAX_CLICKS; ++i)
           clickData_[i-1] = clickData_[i];
       --m_iActiveClickCount;
   }


   if (m_bIsLsgc && pMouse != NULL) {
      // update LSGC mouse pointer
       pMouse->UpdateData(&curRect);
   }
   else if (hCursor)
   {
      // draw mouse pointer 

      // Ok, we have a cursor. 
      // Draw the icon onto the correct position.
      // Note: The hot spot is already taken into account
      // by the getCurrentCursorHandle method

       // The curRect is the current grabbing area on screen

       /*
       if (ptMouse != NULL) {
           ptMouse->x -= curRect.left;
           ptMouse->y -= curRect.top;
       } 
       */

      int x1 = ptCursor.x - curRect.left;
      int y1 = ptCursor.y - curRect.top;

      ::DrawIcon(hdcBitmap_, x1, y1, hCursor);

      if (bQuickCaptureMode_)
      {
         bQcNothingChanged_ = false;

         short x2 = x1 + cursorSizeX;
         short y2 = y1 + cursorSizeY;

         if (x2 >= 0 && x1 < w &&
             y2 >= 0 && y1 < h)
         {
            if (x2 >= w) x2 = w-1;
            if (x1 <  0) x1 = 0;
            if (y2 >= h) y2 = h-1;
            if (y1 <  0) y1 = 0;

            // Flip the y values!
            pChangedRectsNow_->AddRect(QCRECT(x1, h-y1-1, x2, h-y2-1));
         }
         else
         {
            // -10000 symbolizes "nothing changed"
            x1 = -10000;
            y1 = -10000;
         }

         if (!bQcLastDrewClicks_ && !bClicksPainted &&
             x1 == qcLastMousePosX_ && y1 == qcLastMousePosY_)
         {
            bQcNothingChanged_ = true;
         }
      }

      // Update the QC state variables
      bQcLastDrewClicks_ = bClicksPainted;
      qcLastMousePosX_   = x1;
      qcLastMousePosY_   = y1;

#ifdef _DEBUG
      /*
      // DO NOT DELETE!!!
      // Debugging code for the quick capture mode; draws red rectangles around
      // the update areas and blue around the areas which are passed on to the
      // TWLC codec.
      if (!bQcNothingChanged_)
      {
         vector<QCRECT> v = pChangedRectsLast_->GetRectVector();
         vector<QCRECT>::const_iterator iter;

         HPEN hOldPen = (HPEN) ::SelectObject(hdcBitmap_, hRedPen_);
         HBRUSH hOldBrush = (HBRUSH) ::SelectObject(hdcBitmap_, (HBRUSH) ::GetStockObject(HOLLOW_BRUSH));

         for (iter = v.begin(); iter != v.end(); ++iter)
         {
            ::Rectangle(hdcBitmap_, (*iter).x1, h-1-(*iter).y1, (*iter).x2, h-1-(*iter).y2);
         }

         v = pChangedRectsNow_->GetRectVector();

         for (iter = v.begin(); iter != v.end(); ++iter)
         {
            ::Rectangle(hdcBitmap_, (*iter).x1, h-1-(*iter).y1, (*iter).x2, h-1-(*iter).y2);
         }

         ::SelectObject(hdcBitmap_, hBluePen_);

         CRectKeeper rk;
         rk.Merge(*pChangedRectsLast_);
         rk.Merge(*pChangedRectsNow_);
         v = rk.GetRectVector();

         for (iter = v.begin(); iter != v.end(); ++iter)
         {
            ::Rectangle(hdcBitmap_, (*iter).x1, h-1-(*iter).y1, (*iter).x2, h-1-(*iter).y2);
         }

         if (hOldPen)
            ::SelectObject(hdcBitmap_, hOldPen);
         if (hOldBrush)
            ::SelectObject(hdcBitmap_, hOldBrush);
      }
      */
#endif
   }

   // Now check if we're "out of bounds" somewhere; in that case,
   // we will have to black out the parts of the bitmap which
   // are not part of the screen

   // In case we're totally out of bounds, let's remember that
   bool allDrawn = false;
   if (curRect.left < 0)
   {
      RECT leftRect;
      leftRect.left = 0;
      if (-curRect.left < curRect.right - curRect.left)
         leftRect.right = -curRect.left;
      else
      {
         leftRect.right = curRect.right - curRect.left;
         allDrawn = true;
      }
      leftRect.top = 0;
      leftRect.bottom = curRect.bottom - curRect.top;

      HBRUSH hBlack = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
      ::FillRect(hdcBitmap_, &leftRect, hBlack);
      ::DeleteObject(hBlack);
   }

   if (curRect.right > screenWidth_ && !allDrawn)
   {
      RECT rightRect;
      if (screenWidth_ - curRect.left < curRect.right - curRect.left)
         rightRect.left = screenWidth_ - curRect.left;
      else
      {
         rightRect.left = 0;
         allDrawn = true;
      }
      rightRect.right = curRect.right - curRect.left;
      rightRect.top = 0;
      rightRect.bottom = curRect.bottom - curRect.top;

      HBRUSH hBlack = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
      ::FillRect(hdcBitmap_, &rightRect, hBlack);
      ::DeleteObject(hBlack);
   }

   if (curRect.top < 0 && !allDrawn)
   {
      RECT topRect;
      topRect.bottom = 0;
      if (-curRect.top < curRect.bottom - curRect.top)
         topRect.bottom = -curRect.top;
      else
      {
         topRect.bottom = curRect.bottom - curRect.top;
         allDrawn = true;
      }
      topRect.left = 0;
      topRect.right = curRect.right - curRect.left;

      HBRUSH hBlack = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
      ::FillRect(hdcBitmap_, &topRect, hBlack);
      ::DeleteObject(hBlack);
   }

   if (curRect.bottom > screenHeight_ && !allDrawn)
   {
      RECT bottomRect;
      if (screenHeight_ - curRect.top < curRect.bottom - curRect.top)
         bottomRect.top = screenHeight_ - curRect.top;
      else
         bottomRect.top = 0;
      bottomRect.bottom = curRect.bottom - curRect.top;
      bottomRect.left = 0;
      bottomRect.right = curRect.right - curRect.left;

      HBRUSH hBlack = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
      ::FillRect(hdcBitmap_, &bottomRect, hBlack);
      ::DeleteObject(hBlack);
   }


   // Copy the bitmap bits into the current buffer; we
   // have a pointer to it in lpData:
   ::GetDIBits(hdcBitmap_, hbitmap_, 0, h, lpData, videoFormat_, DIB_RGB_COLORS);

   ::DeleteDC(hdc);
}

void ScreenCapturer::updateScreenFormat(int width, int height)
{
#ifdef _DEBUG
   cout << "ScreenCapturer::updateScreenFormat(" << width << ", " << height << ")" << endl;
#endif
   HDC hdc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
   if (hdc)
   {
      // we need a BITMAPINFOHEADER for the codec to know
      // which format the data has that is passed to it.
      // retrieve some information on the current screen:

      // TODO: Trap 8 Bit Modes, non BI_RGB modes!
      BITMAPINFOHEADER* bh = &videoFormat_->bmiHeader;

      bh->biSize          = sizeof BITMAPINFOHEADER;
      bh->biWidth         = width; // default
      bh->biHeight        = height; // default
      bh->biPlanes        = 1;   // has to be 1
      bh->biBitCount      = GetDeviceCaps(hdc, BITSPIXEL);
      bh->biCompression   = BI_RGB;

      int pixelLen = bh->biBitCount / 8;
      int padding = (width * pixelLen) % 4 == 0 ? 0 : 4 - (width * pixelLen) % 4;

      bh->biSizeImage     = width * height * pixelLen + padding * height;
      bh->biXPelsPerMeter = GetDeviceCaps(hdc, LOGPIXELSX);
      bh->biYPelsPerMeter = GetDeviceCaps(hdc, LOGPIXELSY);
      bh->biClrUsed       = 0;
      bh->biClrImportant  = 0;

      screenWidth_      = GetDeviceCaps(hdc, HORZRES);
      screenHeight_     = GetDeviceCaps(hdc, VERTRES);

      int rasterCaps = GetDeviceCaps(hdc, RASTERCAPS);

#ifdef _DEBUG
      cout << "biBitCount == " << bh->biBitCount << endl;
      cout << "rasterCaps == " << rasterCaps << endl;
      cout << "screen size: " << screenWidth_ << "x" << screenHeight_ << endl;
#endif

      DeleteDC(hdc);
   }
}

/* ========================================================================= */

void ScreenCapturer::startMousePan()
{
   ::GetCursorPos(&panStartMouse_);
   ::GetWindowRect(hFlashWnd_, &panStartWnd_);

   if (m_pDrawWindow)
       m_pDrawWindow->Hide();

   if (bHideWhilePanning_)
      ::ShowWindow(hFlashWnd_, SW_HIDE);

   m_bIsPanningActive = true;
}

void ScreenCapturer::doMousePan()
{
   POINT curPos;
   ::GetCursorPos(&curPos);
   int deltaX = curPos.x - panStartMouse_.x;
   int deltaY = curPos.y - panStartMouse_.y;

   ::SetWindowPos(hFlashWnd_, NULL, 
                  panStartWnd_.left + deltaX,
                  panStartWnd_.top + deltaY,
                  0, 0, 
                  SWP_NOSIZE | SWP_NOZORDER);
}

void ScreenCapturer::endMousePan()
{
   m_bIsPanningActive = false;

   if (bHideWhilePanning_)
      ::ShowWindow(hFlashWnd_, SW_SHOW);

   if (m_pDrawWindow && nCurrentMode_ == MODE_ANNOTATION)
   {
      m_pDrawWindow->SetTopLeft(captureRect_.left, captureRect_.top);
      m_pDrawWindow->Show();
   }
}

/* ========================================================================= */


/* ========================================================================= */

void ScreenCapturer::insertClickNoise(POINT &ptMouse, DWORD clickFlags)
{
   long timeStamp = 0;

   // no click visualization in annotation mode.
   if (nCurrentMode_ == MODE_ANNOTATION || isPaused_ == true)
      return;

   if (audioSync_ && audio_ != NULL && doAudioClickEffect_)
   {
      // If we're recording audio, take audio time stamp,
      // otherwise, use the reference time of this video
      // device (system time).
      timeStamp = audio_->insertClickNoise();
   }
   else if (audio_ != NULL)
   {
      timeStamp = audio_->getWaveInTimeMs();
   }
   else
   {
      timeStamp = getRefTime();
   }

   if (doVideoClickEffect_ && timeStamp > -1)
   {
      if (m_iActiveClickCount >= MAX_CLICKS)
         return;

      int t = m_iActiveClickCount;
      clickData_[t].timeStamp  = timeStamp;
      clickData_[t].frameCount = 0;
      clickData_[t].mouseFlags = clickFlags;
      clickData_[t].clickPos   = ptMouse;
#ifdef _DEBUG
      printf("Inserting click at (%d, %d)\n", ptMouse.x, ptMouse.y);
#endif

      ++m_iActiveClickCount;
   }
}

void ScreenCapturer::WriteRemoveTeleEntry(POINT &ptMouse)
{
	if ( m_pFileSgActions == NULL || isPaused_)
		return ;
	// BUGFIX 5764: respect pause time
	long lClipTime = getVideoTime() - ((long) (pausedTime_ / 1000i64));
	long lClipFrame = getTotalFramesCount();

	// special handling required after pause because "pausedTime_" is updated when computing frames only
	// and therefore is incorrect within a short timespan (indicated by "wasPaused_") after pause end 
	// until the next frame is handled,
	if ( wasPaused_ ) {        
		// cannot use pausedTime_ because not updated yet
		if (audio_ != NULL )
			// use audio time instead which may be out of sync slighty but still ok
			lClipTime = audio_->getWaveInTimeMs();
		else
			// cannot determine meaningful time
			return ;
	}

	CString csAction = _T("TELE REM");
	CString csLine;
	LONG x = ptMouse.x - captureRect_.left;
	LONG y = ptMouse.y - captureRect_.top;

	csLine.Format(_T("%d %d (%d,%d) %s\n"), lClipTime, lClipFrame, x, y, csAction);
	m_pFileSgActions->Append(csLine);
	m_pFileSgActions->Flush();

}

long ScreenCapturer::WriteClickActionEntry(POINT &ptMouse, DWORD dwMouseFlags, DWORD dwActionFlags) {

	if (nCurrentMode_ == MODE_ANNOTATION && nAnnoTool_ != DrawSdk::TELEPOINTER)
		WriteRemoveTeleEntry(ptMouse);
	if ( m_pFileSgActions == NULL || isPaused_  || (nCurrentMode_ == MODE_ANNOTATION && nAnnoTool_ != DrawSdk::TELEPOINTER))
        // skip because no recording or paused or paint tools are used
        return -1;
				   
    // BUGFIX 5764: respect pause time
    long lClipTime = getVideoTime() - ((long) (pausedTime_ / 1000i64));
    long lClipFrame = getTotalFramesCount();

    // special handling required after pause because "pausedTime_" is updated when computing frames only
    // and therefore is incorrect within a short timespan (indicated by "wasPaused_") after pause end 
    // until the next frame is handled,
    if ( wasPaused_ ) {        
        // cannot use pausedTime_ because not updated yet
        if (audio_ != NULL )
            // use audio time instead which may be out of sync slighty but still ok
            lClipTime = audio_->getWaveInTimeMs();
        else
           // cannot determine meaningful time
           return -1;
    }

    CString csAction; // = _T("MOUSE");
	if (nCurrentMode_ == MODE_ANNOTATION && nAnnoTool_ == DrawSdk::TELEPOINTER)
		csAction += _T("TELE");
	else
		if ((dwMouseFlags & MH_LEFT_BUTTON) != 0)
			csAction += _T("LEFT");
		else if ((dwMouseFlags & MH_MIDDLE_BUTTON) != 0)
			csAction += _T("MIDDLE");
		else if ((dwMouseFlags & MH_RIGHT_BUTTON) != 0)
			csAction += _T("RIGHT");

		if ((dwActionFlags & MH_BUTTON_DOWN) != 0)
			csAction += _T(" DOWN");
		else if ((dwActionFlags & MH_BUTTON_UP) != 0)
			csAction += _T(" UP");
		else if ((dwActionFlags & MH_BUTTON_DBLCLICK) != 0)
			csAction += _T(" DOUBLECLICK");
	
    CString csLine;
    LONG x = ptMouse.x - captureRect_.left;
    LONG y = ptMouse.y - captureRect_.top;
    m_iLastX = x;
    m_iLastY = y;
    // required to translate absolute to relative coordinates for LiveContext data
    LONG lLiveContextOffsetX = -captureRect_.left;
    LONG lLiveContextOffsetY = -captureRect_.top;

    csLine.Format(_T("%d %d (%d,%d) %s (%d,%d)\n"), lClipTime, lClipFrame, x, y, csAction, lLiveContextOffsetX, lLiveContextOffsetY);
    m_pFileSgActions->Append(csLine);
    m_pFileSgActions->Flush();

    return lClipTime;
}

void ScreenCapturer::WriteMoveActionEntry(POINT &ptMouse) {

    if ( m_pFileSgActions == NULL || isPaused_ ) 
        return;

    // BUGFIX 5764: respect pause time
    long lClipTime = getVideoTime() - ((long) (pausedTime_ / 1000i64));
    long lClipFrame = getTotalFramesCount();

    // special handling required after pause because "pausedTime_" is updated when computing frames only
    // and therefore is incorrect within a short timespan (indicated by "wasPaused_") after pause end 
    // until the next frame is handled,
    if ( wasPaused_ ) {        
        // cannot use pausedTime_ because not updated yet
        if (audio_ != NULL )
            // use audio time instead which may be out of sync slighty but still ok
            lClipTime = audio_->getWaveInTimeMs();
        else
           // cannot determine meaningful time
           return;
    }

    if (m_lLastMouseMove != 0 && lClipTime - m_lLastMouseMove < 10)
        return;

    m_lLastMouseMove = lClipTime;

    LONG x = ptMouse.x - captureRect_.left;
    LONG y = ptMouse.y - captureRect_.top;

    if ( x == m_iLastX && y == m_iLastY )
       // same position: do nothing
       return;

    m_iLastX = x;
    m_iLastY = y;

    CString csAction = _T("MOUSE");
	if (nCurrentMode_ == MODE_ANNOTATION && nAnnoTool_ == DrawSdk::TELEPOINTER && m_bIsDragging)
	{
		csAction = _T("TELE");
	}
	if ( m_bIsDragging )
        csAction += _T(" DRAG");
    else 
        csAction += _T(" MOVE");

    CString csLine;
    csLine.Format(_T("%d %d (%d,%d) %s\n"), lClipTime, lClipFrame, x, y, csAction);
    m_pFileSgActions->Append(csLine);
    m_pFileSgActions->Flush();
}

/* ========================================================================= */

void ScreenCapturer::displayCaptureFrame(bool bDisplay)
{
   RedrawWindow(hFlashWnd_, NULL, NULL, RDW_UPDATENOW);
   /*
   if (bDisplay)
      ::ShowWindow(hFlashWnd_, SW_SHOWNOACTIVATE);
   else
      ::ShowWindow(hFlashWnd_, SW_HIDE);
   */
}

// The methods for the time control:
long ScreenCapturer::getRefTime()
{
   return timeGetTime();
}

long ScreenCapturer::getVideoTime()
{
   if (dwFirstTimeMs_ != -1)
      return (getRefTime() - dwFirstTimeMs_);
   else
      return 0;
}

/* ========================================================================= */

/* ========================================================================= */

/* ========================================================================= */

/* ========================================================================= */

/* ========================================================================= */

void ScreenCapturer::initVHDRs()
{
   // if we happen to have any video headers left, delete them
   // first (this should _NOT_ occur!)
   if (paVideoHdr_)
   {
      deleteVHDRs();
   }

   int iBufferCount = getVideoBuffers();
   paVideoHdr_ = new VIDEOHDR*[iBufferCount];
   if (!paVideoHdr_)
   {
      // TODO: Return error message
#ifdef _DEBUG
      cerr << "*** Could not create VIDEOHDR array" << endl;
#endif
      return;
   }

   paQcData_ = new QCDATA*[iBufferCount];
   paMouseData_ = new SrMouseData*[iBufferCount];
   paClickData_ = new SrClickData*[iBufferCount];

   bool allOk = true;
   for (int i=0; i<iBufferCount; ++i)
   {
      paVideoHdr_[i] = NULL;
      paVideoHdr_[i] = new VIDEOHDR;
      if (paVideoHdr_[i])
      {
         paVideoHdr_[i]->lpData = NULL;
         paVideoHdr_[i]->lpData = 
            new unsigned char[videoFormat_->bmiHeader.biSizeImage];
         if (paVideoHdr_[i]->lpData)
         {
            paVideoHdr_[i]->dwBufferLength = videoFormat_->bmiHeader.biSizeImage;
            paVideoHdr_[i]->dwBytesUsed    = videoFormat_->bmiHeader.biSizeImage;
            paVideoHdr_[i]->dwTimeCaptured = 0;
            paVideoHdr_[i]->dwUser         = 0;
            paVideoHdr_[i]->dwFlags        = VHDR_INQUEUE | VHDR_PREPARED | VHDR_KEYFRAME;
            paVideoHdr_[i]->dwReserved[0]  = 0;
            paVideoHdr_[i]->dwReserved[1]  = 0;
            paVideoHdr_[i]->dwReserved[2]  = 0;
            paVideoHdr_[i]->dwReserved[3]  = 0;
         }
         else
         {
            // TODO: Return error message
#ifdef _DEBUG
            cerr << "*** Could not create VIDEOHDR lpData instance" << endl;
#endif
            allOk = false;
         }
      }
      else
      {
         // TODO: Return error message
#ifdef _DEBUG
         cerr << "*** Could not create VIDEOHDR instance" << endl;
#endif
         allOk = false;
      }

      paQcData_[i] = new QCDATA;
      if (!paQcData_[i])
         allOk = false;

      paMouseData_[i] = new SrMouseData;
      if (!paMouseData_[i])
         allOk = false;

      // TODO changes to these variables are not noticed in these objects
      paClickData_[i] = new SrClickData;
      if (!paClickData_[i])
         allOk = false;
   }

   if (!allOk)
      throw AVGrabberException(_T("Not enough memory for screen grabbing structures!"));
}

/* ========================================================================= */

void ScreenCapturer::deleteVHDRs() {
    int iBufferCount = getVideoBuffers();

   for (int i=0; i<iBufferCount; ++i) {
      delete[] paVideoHdr_[i]->lpData;
      delete   paVideoHdr_[i];
   }
   delete[] paVideoHdr_;
   paVideoHdr_ = NULL;

   if (paQcData_) {
      for (i=0; i<iBufferCount; ++i) {
         if (paQcData_[i])
            delete paQcData_[i];
      }
      delete[] paQcData_;
      paQcData_ = NULL;
   }

   if (paMouseData_) {
      for (i=0; i<iBufferCount; ++i) {
         if (paMouseData_[i])
            delete paMouseData_[i];
      }
      delete[] paMouseData_;
      paMouseData_ = NULL;
   }

   if (paClickData_) {
      for (i=0; i<iBufferCount; ++i) {
         if (paClickData_[i])
            delete paClickData_[i];
      }
      delete[] paClickData_;
      paClickData_ = NULL;
   }
}

/* ========================================================================= */

// Obsolete! by SrMouseData
HCURSOR ScreenCapturer::getCurrentCursorHandle(POINT &pt, POINT *ptMouseHotPos)
{
   HCURSOR hCursor = NULL;
   static ICONINFO iconInfo;
   static CURSORINFO ci;

   memset(&ci, 0, sizeof(CURSORINFO));
   ci.cbSize = sizeof(CURSORINFO);
   bool success = false;

   success = (::GetCursorInfo(&ci) != FALSE);

   if (success)
   {
      hCursor = ci.hCursor;
      pt.x    = ci.ptScreenPos.x;
      pt.y    = ci.ptScreenPos.y;

      if (ptMouseHotPos != NULL)
          *ptMouseHotPos = ci.ptScreenPos;
   }

   // Did we get a cursor?
   if (hCursor)
   {
      // Yes, then retrieve information on the hot spot
      // of the cursor so that it may be positioned
      // correctly.
      if (GetIconInfo(hCursor, &iconInfo))
      {
         pt.x -= iconInfo.xHotspot;
         pt.y -= iconInfo.yHotspot;

         ::DeleteObject(iconInfo.hbmColor);
         ::DeleteObject(iconInfo.hbmMask);
      }
   }

   return hCursor;
}


// /////////////////////////////////////////
//
//   A N N O T A T I O N   M E T H O D S
//
// /////////////////////////////////////////

// PUBLIC METHODS

void ScreenCapturer::setAnnotationMode()
{
   // are we already in annotation mode?
   if (nCurrentMode_ == MODE_ANNOTATION)
      return;

   // no, we're not
   enterAnnotationMode();

   nCurrentMode_ = MODE_ANNOTATION;
}

void ScreenCapturer::setInteractionMode()
{
   // are we already in interaction mode?
   if (nCurrentMode_ == MODE_INTERACTION)
      return;

   // no, we're not
   enterInteractionMode();

   nCurrentMode_ = MODE_INTERACTION;
}

void ScreenCapturer::setAnnotationTool(int nTool)
{
   nAnnoTool_ = nTool;
   if (m_pDrawWindow)
      m_pDrawWindow->SelectTool(nTool);
}

void ScreenCapturer::setAnnotationPenColor(Gdiplus::ARGB color)
{
   if (m_pDrawWindow)
      m_pDrawWindow->SetPenColor(color);
}

void ScreenCapturer::setAnnotationFillColor(Gdiplus::ARGB color)
{
   if (m_pDrawWindow)
      m_pDrawWindow->SetFillColor(color);
}

void ScreenCapturer::setAnnotationIsFilled(bool b)
{
   if (m_pDrawWindow)
      m_pDrawWindow->SetFilled(b);
}

void ScreenCapturer::setAnnotationIsClosed(bool b)
{
   if (m_pDrawWindow)
      m_pDrawWindow->SetClosed(b);
}

void ScreenCapturer::setAnnotationLineWidth(int lineWidth)
{
   if (m_pDrawWindow)
      m_pDrawWindow->SetLineWidth(lineWidth);
}

void ScreenCapturer::setAnnotationLineStyle(int lineStyle)
{
   if (m_pDrawWindow)
      m_pDrawWindow->SetLineStyle(lineStyle);
}

void ScreenCapturer::setAnnotationArrowStyle(int arrowStyle)
{
   if (m_pDrawWindow)
      m_pDrawWindow->SetArrowStyle(arrowStyle);
}

void ScreenCapturer::setAnnotationArrowConfig(int arrowConfig)
{
   if (m_pDrawWindow)
      m_pDrawWindow->SetArrowConfig(arrowConfig);
}

void ScreenCapturer::setAnnotationFont(LOGFONT *pLogFont)
{
   if (m_pDrawWindow)
      m_pDrawWindow->SetFont(pLogFont);
}

void ScreenCapturer::annotationUndo()
{
   if (m_pDrawWindow)
      m_pDrawWindow->Undo();
}

long ScreenCapturer::getAnnotationLastUndoTimeStamp()
{
   if (m_pDrawWindow)
      return m_pDrawWindow->GetLastUndoTime();
   return -1;
}

void ScreenCapturer::annotationRedo()
{
   if (m_pDrawWindow)
      m_pDrawWindow->Redo();
}

long ScreenCapturer::getAnnotationNextRedoTimeStamp()
{
   if (m_pDrawWindow)
      return m_pDrawWindow->GetNextRedoTime();
   return -1;
}

void ScreenCapturer::setDeleteAllOnInteractionMode(bool b)
{
   bDeleteOnInteractionMode_ = b;
}

void ScreenCapturer::setHideWhilePanning(bool b) 
{ 
   bHideWhilePanning_ = b;
}

void ScreenCapturer::setClickEffectFrames(int time, int maxSize)
{
   clickAnimMs_      = time;
   clickAnimMaxSize_ = maxSize;
}

void ScreenCapturer::getClickEffectFrames(int *time, int *maxSize)
{
   if (time)
      *time = clickAnimMs_;
   if (maxSize)
      *maxSize = clickAnimMaxSize_;
}

void ScreenCapturer::setEnableClickEffects(bool audioEffects, bool videoEffects)
{
   doAudioClickEffect_ = audioEffects;
   doVideoClickEffect_ = videoEffects;
}

// PRIVATE METHODS

void ScreenCapturer::enterAnnotationMode()
{
    if (m_pDrawWindow) {
        m_pDrawWindow->SetTopLeft(captureRect_.left, captureRect_.top);
        if (bDeleteOnInteractionMode_)
            m_pDrawWindow->OnDeleteAll();
        m_pDrawWindow->Show();
    }
}

void ScreenCapturer::enterInteractionMode()
{
    if (m_pDrawWindow) 
        m_pDrawWindow->Hide();
}

void ScreenCapturer::setQuickCaptureMode(bool b)
{
   bQuickCaptureMode_ = b;
}

bool ScreenCapturer::getQuickCaptureMode()
{
   return bQuickCaptureMode_;
}

HWND ScreenCapturer::GetAnnotationWindow()
{
    if (m_pDrawWindow)
        return m_pDrawWindow->GetSafeHwnd();

    return NULL;
}


/* ========================================================================= */
/* Private funcions for mouse click handling                                 */
/* ========================================================================= */

bool ScreenCapturer::MousePosInSelectionRect(POINT &ptMouse) {
    return (ptMouse.x >= captureRect_.left && 
            ptMouse.x <= captureRect_.right && 
            ptMouse.y >= captureRect_.top &&
            ptMouse.y <= captureRect_.bottom);
}

void ScreenCapturer::ReportElementInfo(CLiveContextElementInfo *pInfo) {
    DWORD dwWaitResult = WaitForSingleObject(m_hFileSgActionsEvent, 1000);
    ResetEvent(m_hFileSgActionsEvent);
    switch (dwWaitResult) {
        // The thread got mutex ownership.
        case WAIT_OBJECT_0:
            break; 
            // Cannot get mutex ownership due to time-out so we just return
            // Some other thread has it
        case WAIT_TIMEOUT: 
        case WAIT_ABANDONED: 
        case WAIT_FAILED:
            {
                CString csID;
                csID.Format(CString(_T("%d")), pInfo->GetID());
                CLiveContextCommunicator::GetInstance()->AddWarning(csID);
                SetEvent(m_hFileSgActionsEvent);
                return;
            }
    }

    if (pInfo) {
        CString csResult = pInfo->ToJsonString();

        m_pDebugLog->LogEntry(CString("Pipe response: ") + csResult);

        // Reset m_hFileSgActionsEvent to nonsignaled, to block readers.
        
        if (m_pFileSgActions) { // no pause check here; LC results may be delayed // || isPaused_ ) 
            m_pFileSgActions->Append(csResult);
            m_pFileSgActions->Flush();
        }
        // Set m_hFileSgActionsEvent to signaled.
    }
    SetEvent(m_hFileSgActionsEvent);
}

void ScreenCapturer::RequestElementInfoFromLiveContext(long lTimestamp, POINT &ptMouse) {
    CLiveContextElementInfo *pInfo = new CLiveContextElementInfo(CPoint(ptMouse));
    // use timestamp as ID to preserve relation between click event and ElementInfo
    if (lTimestamp >= 0)
        pInfo->SetID(lTimestamp);
    CLiveContextCommunicator::GetInstance()->RequestElementInfo(pInfo);
    m_mapLcElementInfo.SetAt(pInfo->GetID(), pInfo);
}

bool ScreenCapturer::UseMouseClick(POINT &ptMouse) {
    if (!bStopRequested_ && !isPaused_ && MousePosInSelectionRect(ptMouse))
        return true;
    else
        return false;
}

bool ScreenCapturer::IsLiveContextConnected() {
    if (GetLiveContext() != NULL && m_pThreadStore != NULL &&
        GetLiveContext()->IsLiveContextMode() && CLiveContextCommunicator::GetInstance()->Connected())
        return true;
    else
        return false;
}

void ScreenCapturer::EmptyLcElementInfoMap() {
    if (m_mapLcElementInfo.GetCount() > 0) {
        m_pDebugLog->LogEntryInt(_T("RemoveClickElementInfo: "), m_mapLcElementInfo.GetCount());
        UINT nKey = 0;
        CLiveContextElementInfo *pInfo = NULL;
        POSITION iter = m_mapLcElementInfo.GetStartPosition();
        while (iter != NULL) {
            m_mapLcElementInfo.GetNextAssoc(iter, nKey, pInfo);
            SAFE_DELETE(pInfo);
        }
        m_mapLcElementInfo.RemoveAll();
    }
}
#include "stdafx.h"

#include "FlashEngine.h"
#include "FlashEngineHelper.h"
#include "FlashLostStopmarksDlg.h"

#include "ZipArchive.h" // ZipArchive: CZipArchive

#include "mingpp.h"
#include "imc_lecturnity_converter_LPLibs.h"
#include "lutils.h" // Using LFile for temporary files
#include "mmsystem.h"
#include "PublisherLibs.h"
#include "KeyGenerator.h"
#include "LrdFile.h"

// For getting audio length and sample rate ...
#include <qedit.h> // DirectShow
#include "streams.h" // BaseClasses DirectShow
#include <comdef.h> // _bstr_t
#include <atlbase.h> // CComPtr, CoInitialize...
// For memory allocation error, etc.
#include <errno.h>

#include <initguid.h> // before Guid definition needed
DEFINE_GUID(CLSID_LadFilter,
            0x67cd5cef, 0xe69b, 0x43f2, 0xac, 0x36, 0x87, 0x44, 0x1e, 0x88, 0x23, 0xe2);

#include "DsHelper.h"

#define sign(x) ( ((x)<0) ? -1 : ((x)>0) ? 1 : 0 )

CFlashEngine::CFlashEngine()
{
   m_docType = DOCUMENT_TYPE_UNKNOWN;

   m_pWhiteboardStream = NULL;
   m_pPageStream = NULL;

   m_pOkButton = NULL;

   m_tszSwfFile = NULL;
   m_tszPreloaderSwfFile = NULL;
   m_csPreloaderFilename = _T("");

   m_tszLadFile = NULL;
   m_tszMp3File = NULL;

   m_tszVideoFile = NULL;
   m_tszVideoFlvFile = NULL;

   m_tszStillImageFile = NULL;

   m_tszRtmpPath = NULL;

   m_atszClipFiles = NULL;
   m_atszClipFlvFiles = NULL;
   m_aSizeClips = NULL;
   m_alClipsBeginMs = NULL;
   m_alClipsEndMs = NULL;

   m_iFlashVersion = 7; // default: 5

   m_iMaxIndexSoFar = 0;
   m_iMaxInteractionIndex = 0;
   m_iAudioLengthMs = -1;
   m_iVideoLengthMs = -1;
   m_iWbLengthMs = -1;
   m_iMovieLengthMs = -1;

   m_iWbInputWidth = -1;
   m_iWbInputHeight = -1;
   m_iWbOutputWidth = -1;
   m_iWbOutputHeight = -1;
   
   m_iLogoWidth = -1;
   m_iLogoHeight = -1;

   m_iAccVideoWidth = -1; // Accompanying video
   m_iAccVideoHeight = -1; // Accompanying video

   m_iStillImageWidth = -1;
   m_iStillImageHeight = -1;

   m_iNumOfClips = 0;

   m_rcMovie.SetRectEmpty();
   m_rcFlashPlayer.SetRectEmpty();

   m_fMovieOffsetX = 0.0f;
   m_fMovieOffsetY = 0.0f;

   m_dGlobalScale = (double)(1.0f);
   m_dContentScale = (double) 1.0f;
   m_dFeedbackScale = (double)(1.0f);
   m_dWhiteboardScale = (double)(1.0f);
   m_fFrameRate = 8.0f; // initial value

   m_bIsNewLecPlayer = false;

   m_bAutostartMode = false;
   m_bCancelRequested = false;
   m_bCleanupCalled = false;
   m_bScormEnabled = false;
   m_bIsSlidestar = false;
   m_csSlidestarAddFiles = _T("");
   m_bIsPspDesign = false;
   m_bShowClipsInVideoArea = false;
   m_bLecIdentityHidden = false;
   m_bIncreaseKeyframes = false;
   m_bIsRtmpStreaming = false;

   m_swfLinearGradient = NULL;
   m_swfRadialGradient = NULL;
   m_swfRadialGradientSlider = NULL;
   m_swfLinearGradientBackground = NULL;
   m_swfLinearGradientMouseUp = NULL;
   m_swfLinearGradientMouseDown = NULL;
   m_swfLinearGradientDarker = NULL;
   m_swfRadialGradientDarker = NULL;
   m_swfLinearGradientLighter = NULL;
   m_swfRadialGradientLighter = NULL;


   // this has to be done by a JNI call later on
   SetBasicColors(0xEA, 0xE7, 0xD7);

   // Define a standard color
   m_argbStd = 0x000000;
   // Extra shapes for arrow heads
   m_pShpArrowBegin = NULL;
   m_pShpArrowEnd = NULL;

   // Gdiplus is used here to rescale the images
   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

   m_fScaleMetadata = 1.0f;
   m_fScaleVideo = 1.0f;
   m_fScalePages = 1.0f;
   m_fScaleControlBar = 1.0f;
   m_fScalePspContent = 1.0f;

   m_bRectanglesCorrected = false;

   m_bHasVideo = false;
   m_bHasClips = false;
   m_bHasMP3Audio = false;
   m_bHasFlvAudio = false;
   m_bHasPages = false;
   m_bHasControlBar = false;
   m_bHasMetadata = false;
   m_bIsSimulationDocument = false;

   m_fClipSynchronisationDelta = 3.0;
   m_hrPossibleWarning = S_OK;

   // Add a standard font "ARIAL.TTF" to the font table as standard font for metadata, etc.
   m_mapFontNames.SetAt(_T("ARIAL.TTF"), _T(""));

   m_csTooShortPagesTimes.Empty();

   // For debugging purposes
   //_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
   //_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
}

CFlashEngine::~CFlashEngine()
{
   if (m_tszLadFile)
      delete[] m_tszLadFile;
   if (m_tszMp3File)
      delete[] m_tszMp3File;
   if (m_tszSwfFile)
      delete[] m_tszSwfFile;
   if (m_tszPreloaderSwfFile)
      delete[] m_tszPreloaderSwfFile;
   if (m_tszVideoFile)
      delete[] m_tszVideoFile;
   if (m_tszVideoFlvFile)
      delete[] m_tszVideoFlvFile;
   if (m_tszRtmpPath)
      delete[] m_tszRtmpPath;

   int i = 0;
   if (m_atszClipFiles)
   {
      for (i = 0; i < m_iNumOfClips; ++i)
         delete[] m_atszClipFiles[i];
      delete[] m_atszClipFiles;
   }
   if (m_atszClipFlvFiles)
   {
      for (i = 0; i < m_iNumOfClips; ++i)
         delete[] m_atszClipFlvFiles[i];
      delete[] m_atszClipFlvFiles;
   }
   if (m_aSizeClips)
      delete[] m_aSizeClips;
   if (m_alClipsBeginMs)
      delete[] m_alClipsBeginMs; 
   if (m_alClipsEndMs)
      delete[] m_alClipsEndMs; 

   if (m_pOkButton)
      delete m_pOkButton;


   ClearWhiteboardArraysAndMaps();

   if (!m_bCleanupCalled)
      Cleanup();
   // handles m_mapTempFiles and Ming_cleanup()

   // Close Gdiplus 
   Gdiplus::GdiplusShutdown(m_gdiplusToken);
}


void CFlashEngine::ClearWhiteboardArraysAndMaps()
{
   if (m_pWhiteboardStream)
   {
      m_pWhiteboardStream->Clear();
      delete m_pWhiteboardStream;
   }

   if (m_pPageStream)
   {
      m_pPageStream->Clear();
      delete m_pPageStream;
   }

   m_aiFrameTimestampsMs.RemoveAll();
   m_aiPagingFrames.RemoveAll();
   m_aFlashStopmarks.RemoveAll();
   m_aFlashTargetmarks.RemoveAll();
   m_aFlashFulltexts.RemoveAll();
   m_aInteractionAreas.RemoveAll();
   m_aQuestionnaires.RemoveAll();
   m_aObjects.RemoveAll();
   m_aInteractionObjects.RemoveAll();
   m_mapBitmaps.RemoveAll();
   m_mapImageNames.RemoveAll();
   m_mapFontNames.RemoveAll();
   m_mapFontNamesFontNotFound.RemoveAll();
   m_mapFontNamesGlyphNotFound.RemoveAll();
   m_mapFontNamesStylesNotSupported.RemoveAll();
   m_mapSwfFonts.RemoveAll();
   m_mapInteractiveOpenedFileNames.RemoveAll();
}

UINT CFlashEngine::ResetAllDimensions()
{
   HRESULT hr = S_OK;

   m_iWbInputWidth = -1;
   m_iWbInputHeight = -1;
   m_iWbOutputWidth = -1;
   m_iWbOutputHeight = -1;
   
   m_iLogoWidth = -1;
   m_iLogoHeight = -1;

   m_iAccVideoWidth = -1;
   m_iAccVideoHeight = -1;

   m_fMovieOffsetX = 0.0f;
   m_fMovieOffsetY = 0.0f;
   m_fScaleControlBar = 1.0f;

   m_rcMovie.SetRectEmpty();
   m_rcFlashPlayer.SetRectEmpty();
   m_rcLogo.SetRectEmpty();
   m_rcMetadata.SetRectEmpty();
   m_rcVideo.SetRectEmpty();
   m_rcPages.SetRectEmpty();
   m_rcControlBar.SetRectEmpty();
   m_rcPspContent.SetRectEmpty();

   return hr;
}

UINT CFlashEngine::SetInputFiles(const _TCHAR *tszLrdFile, 
                                 const _TCHAR *tszEvqFile, 
                                 const _TCHAR *tszObjFile, 
                                 const _TCHAR *tszLadFile)
{
   HRESULT hr = S_OK;
   
   if (tszLrdFile) {
       // Fix for Bug 5945: Get the 'IsSimulationDocument' info from the LRD
       CLrdFile *pLrdFile = new CLrdFile();
       if (pLrdFile->Open(tszLrdFile)) {
           m_bIsSimulationDocument = pLrdFile->IsSimulationDocument();
       }
       delete pLrdFile;
   }

   if (tszLadFile)
   {
      m_tszLadFile = new _TCHAR[_tcslen(tszLadFile)+1];
      _tcscpy(m_tszLadFile, tszLadFile);
      m_tszLadFile[_tcslen(tszLadFile)] = '\0';

   }

   // else not necessary (at least for testing purposes)
   // if not specified nothing gets embeded
   
   if ((!tszEvqFile || !tszObjFile) && m_docType != DOCUMENT_TYPE_DENVER)
      hr = E_POINTER;

   if (SUCCEEDED(hr) && m_docType == DOCUMENT_TYPE_DENVER)
   {
      // "Denver" Documents have no whiteboard stream
      return hr;
   }

   if (SUCCEEDED(hr))
   {
      // Get the input path (containing '\' at the end)
	   m_csSlidestarAddFiles.SetString(tszEvqFile);
      m_csInputPath.Format(_T("%s"), tszEvqFile);
      StringManipulation::GetPath(m_csInputPath);
      m_csInputPath += _T("\\");
   }

   if (SUCCEEDED(hr) && m_docType != DOCUMENT_TYPE_DENVER)
   {
      // Get the whiteboard stream
      LFile eFile(tszEvqFile);
      LFile oFile(tszObjFile);

      if (!eFile.Exists() || !oFile.Exists())
      {
         m_csErrorDetail.Empty();
         m_csErrorDetail += tszEvqFile;
         m_csErrorDetail += _T("; ");
         m_csErrorDetail += tszObjFile;;
         
         hr = imc_lecturnity_converter_LPLibs_INPUT_FILE_MISSING;
      }

      if (SUCCEEDED(hr))
      {
         DWORD anfTime = timeGetTime();

         // Clear all stuff belonging to the whiteboard
         ClearWhiteboardArraysAndMaps();

         // Create a new whiteboard stream
         m_pWhiteboardStream = new CWhiteboardStream();
         m_pWhiteboardStream->Init();
         m_pWhiteboardStream->SetObjFilename(tszObjFile);
         m_pWhiteboardStream->SetEvqFilename(tszEvqFile);

         // Read and process the whiteboard stream: 
         // get (normal) objects, stopmarks, interactive objects, ...
         hr = ReadAndProcessWhiteboardStream();

         m_iWbLengthMs = m_pWhiteboardStream->GetLength();
         DWORD endTime = timeGetTime();
         _RPT1(_CRT_WARN, "CFlashEngine::SetInputFiles(): Parsing time: %dms\n", endTime-anfTime);
      }
   }

   return hr;
}

UINT CFlashEngine::SetInputAudioIsFromFlv(bool bIsAudioFlv, const _TCHAR *tszAudioFlvFile)
{
   m_bHasFlvAudio = bIsAudioFlv;

   if (tszAudioFlvFile)
   {
      m_tszVideoFlvFile = new _TCHAR[_tcslen(tszAudioFlvFile)+1];
      _tcscpy(m_tszVideoFlvFile, tszAudioFlvFile);
      m_tszVideoFlvFile[_tcslen(tszAudioFlvFile)] = '\0';
   }

   return S_OK;
}

/*
UINT CFlashEngine::SetInputAudioFile(const _TCHAR *tszAudioFile, const _TCHAR *tszAudioFlvFile)
{
   HRESULT hr = S_OK;

   m_bHasFlvAudio = true;

   // The Audio (FLV) file is treated like a Video (FLV) file
   if (tszAudioFile)
   {
      m_tszVideoFile = new _TCHAR[_tcslen(tszAudioFile)+1];
      _tcscpy(m_tszVideoFile, tszAudioFile);
      m_tszVideoFile[_tcslen(tszAudioFile)] = '\0';
   }

   if (tszAudioFlvFile)
   {
      m_tszVideoFlvFile = new _TCHAR[_tcslen(tszAudioFlvFile)+1];
      _tcscpy(m_tszVideoFlvFile, tszAudioFlvFile);
      m_tszVideoFlvFile[_tcslen(tszAudioFlvFile)] = '\0';
   }

   return hr;
}
*/

UINT CFlashEngine::SetInputVideoFile(const _TCHAR *tszVideoFile, const _TCHAR *tszVideoFlvFile, 
                                     int nVideoWidth, int nVideoHeight, bool bDoPadding)
{
   HRESULT hr = S_OK;

   if (tszVideoFile)
   {
      m_tszVideoFile = new _TCHAR[_tcslen(tszVideoFile)+1];
      _tcscpy(m_tszVideoFile, tszVideoFile);
      m_tszVideoFile[_tcslen(tszVideoFile)] = '\0';


      // Bugfix 3812/4353: Video size must be a multiple of 8x2
      if (bDoPadding)
      {
         // Add padding to video size to get a multiple of 8x2
         if (nVideoWidth % 8 != 0)
            nVideoWidth += (8 - (nVideoWidth % 8));
         if (nVideoHeight % 2 != 0)
            nVideoHeight += (2 - (nVideoHeight % 2));
      }
      else
      {
         // Cut video size to get a multiple of 8x2
         if (nVideoWidth % 8 != 0)
            nVideoWidth -= (nVideoWidth % 8);
         if (nVideoHeight % 2 != 0)
            nVideoHeight -= (nVideoHeight % 2);
      }

      m_iAccVideoWidth = nVideoWidth;
      m_iAccVideoHeight = nVideoHeight;

      // Get output path from output file
      CString csPathOut = GetOutputPath();

      if (tszVideoFlvFile)
      {
         m_tszVideoFlvFile = new _TCHAR[_tcslen(tszVideoFlvFile)+1];
         _tcscpy(m_tszVideoFlvFile, tszVideoFlvFile);
         m_tszVideoFlvFile[_tcslen(tszVideoFlvFile)] = '\0';
      }
      else
      {
         // Get FLV file name from the AVI file
         CString csAviName(m_tszVideoFile);
         StringManipulation::GetFilePrefix(csAviName);

         CString csFlvFile;
         csFlvFile.Format(_T("%s\\%s.flv"), csPathOut, csAviName);

         m_tszVideoFlvFile = new _TCHAR[csFlvFile.GetLength()+1];
         _tcscpy(m_tszVideoFlvFile, csFlvFile);
         m_tszVideoFlvFile[csFlvFile.GetLength()] = '\0';
      }

      // Get the video length (needed for Action Script later on)
      m_iVideoLengthMs = -1;
   }

   return hr;
}

UINT CFlashEngine::SetNumberOfClips(int iNumOfClips)
{
   HRESULT hr = S_OK;

   m_iNumOfClips = iNumOfClips;

   hr = InitClipFilenames();

   return hr;
}

UINT CFlashEngine::SetInputClipFile(int iIndex, const _TCHAR *tszClipFile, const _TCHAR *tszClipFlvFile, 
                                    int nClipWidth, int nClipHeight, bool bDoPadding, 
                                    long lClipBeginMs, long lClipEndMs)
{
   HRESULT hr = S_OK;

   if (iIndex >= m_iNumOfClips)
   {
      // TODO: error handling?
      return E_FAIL;
   }

   if (tszClipFile)
   {
      m_atszClipFiles[iIndex] = new _TCHAR[_tcslen(tszClipFile)+1];
      _tcscpy(m_atszClipFiles[iIndex], tszClipFile);
      m_atszClipFiles[iIndex][_tcslen(tszClipFile)] = '\0';

      // Get output path from output file
      CString csPathOut = GetOutputPath();

      if (tszClipFlvFile)
      {
         m_atszClipFlvFiles[iIndex] = new _TCHAR[_tcslen(tszClipFlvFile)+1];
         _tcscpy(m_atszClipFlvFiles[iIndex], tszClipFlvFile);
         m_atszClipFlvFiles[iIndex][_tcslen(tszClipFlvFile)] = '\0';
      }
      else
      {
         // Get FLV file name from the AVI file
         CString csAviName(m_atszClipFiles[iIndex]);
         StringManipulation::GetFilePrefix(csAviName);

         CString csFlvFile;
         csFlvFile.Format(_T("%s\\%s.flv"), csPathOut, csAviName);

         m_atszClipFlvFiles[iIndex] = new _TCHAR[csFlvFile.GetLength()+1];
         _tcscpy(m_atszClipFlvFiles[iIndex], csFlvFile);
         m_atszClipFlvFiles[iIndex][csFlvFile.GetLength()] = '\0';
      }

      // Bugfix 3812/4353: Video size must be a multiple of 8x2
      if (bDoPadding)
      {
         // Add padding to video size to get a multiple of 8x2
         if (nClipWidth % 8 != 0)
            nClipWidth += (8 - (nClipWidth % 8));
         if (nClipHeight % 2 != 0)
            nClipHeight += (2 - (nClipHeight % 2));
      }
      else
      {
         // Cut video size to get a multiple of 8x2
         if (nClipWidth % 8 != 0)
            nClipWidth -= (nClipWidth % 8);
         if (nClipHeight % 2 != 0)
            nClipHeight -= (nClipHeight % 2);
      }

      m_aSizeClips[iIndex].cx = nClipWidth;
      m_aSizeClips[iIndex].cy = nClipHeight;
      m_alClipsBeginMs[iIndex] = lClipBeginMs;
      m_alClipsEndMs[iIndex] = lClipEndMs;
   }

   return hr;
}

UINT CFlashEngine::SetInputStillImageFile(const _TCHAR *tszStillImageFile, int nWidth, int nHeight)
{
   HRESULT hr = S_OK;

   if (tszStillImageFile)
   {
      m_tszStillImageFile = new _TCHAR[_tcslen(tszStillImageFile)+1];
      _tcscpy(m_tszStillImageFile, tszStillImageFile);
      m_tszStillImageFile[_tcslen(tszStillImageFile)] = '\0';

      m_iStillImageWidth = nWidth;
      m_iStillImageHeight = nHeight;
   }

   return hr;
}

UINT CFlashEngine::InitClipFilenames()
{
   HRESULT hr = S_OK;

   m_atszClipFiles = new TCHAR*[m_iNumOfClips];
   m_atszClipFlvFiles = new TCHAR*[m_iNumOfClips];
   m_aSizeClips = new SIZE[m_iNumOfClips];
   m_alClipsBeginMs = new long[m_iNumOfClips];
   m_alClipsEndMs = new long[m_iNumOfClips];

   for (int i = 0; i < m_iNumOfClips; ++i)
   {
      m_atszClipFiles[i] = NULL;
      m_atszClipFlvFiles[i] = NULL;
      m_aSizeClips[i].cx = -1;
      m_aSizeClips[i].cy = -1;
      m_alClipsBeginMs[i] = -1;
      m_alClipsEndMs[i] = -1;
   }

   return hr;
}

UINT CFlashEngine::ReadAndProcessWhiteboardStream()
{
   HRESULT hr = S_OK;

   /*
   _CrtMemState state;
   _CrtMemCheckpoint(&state);
   */

   // Open and parse event & object queue
   // Bugfix 5301: 
   // Setting the flag 'false' in the 'Open' command avoids having too 
   // much image data in the memory (which may lead to problems during 
   // the "rescaling images" step). 
   hr = m_pWhiteboardStream->Open(false);
   if (SUCCEEDED(hr))
   {
      m_pPageStream = m_pWhiteboardStream->CreatePageStreamFromEvents();
   }
   else
   {
      _TCHAR tszMessage[1024];
      _stprintf(tszMessage, _T("Error: Unable to read Object/Event Queue!\n"));
      _tprintf(tszMessage);
      
      hr = imc_lecturnity_converter_LPLibs_WHITEBOARD_CORRUPT;
   }

   if (!m_pPageStream)
   {
      _TCHAR tszMessage[1024];
      _stprintf(tszMessage, _T("Error: Page stream creation from Whiteboard stream failed!\n"));
      _tprintf(tszMessage);
      
      hr = imc_lecturnity_converter_LPLibs_WHITEBOARD_CORRUPT;
   }

   /*
   _CrtMemState stateNew;
   _CrtMemState stateDiff;

   _CrtMemCheckpoint(&stateNew);
   _CrtMemDifference(&stateDiff, &state, &stateNew);

   _RPT1(_CRT_WARN, _T("Memory allocated: %d bytes\n"), stateDiff.lTotalCount);
   */

      
   if (SUCCEEDED(hr))
      hr = GetObjectListFromWhiteboardStream();

   if (SUCCEEDED(hr))
      hr = GetStopJumpMarksFromWhiteboardStream();

   if (SUCCEEDED(hr))
      hr = GetInteractionObjectsFromWhiteboardStream();

   if (SUCCEEDED(hr))
      hr = GetQuestionnaireFromWhiteboardStream();

   return hr;
}

UINT CFlashEngine::GetObjectListFromWhiteboardStream()
{
   HRESULT hr = S_OK;

   // Get an object list
   m_pWhiteboardStream->GetObjectArray(m_aObjects);
   if (m_aObjects.GetSize() < 1)
   {
      _TCHAR tszMessage[1024];
      _stprintf(tszMessage, _T("Error: No objects found in Object Queue!\n"));
      _tprintf(tszMessage);
      
      hr = imc_lecturnity_converter_LPLibs_WHITEBOARD_CORRUPT;
   }
   
   if (SUCCEEDED(hr))
   {
      // Scan the event/object list for superfluous background rectangles
      // (Bugs #3860, #3862); replace them
      //

      CArray<CWhiteboardEvent *, CWhiteboardEvent *> aEvents;
      m_pWhiteboardStream->GetEventArray(aEvents);
      int iEventCount = aEvents.GetSize();
      if (iEventCount > 0)
      {
         int iLastPageNumber = -1;
         DrawSdk::Rectangle* pLastBackground = NULL;

         for (int e=0; e<iEventCount; ++e)
         {
            CWhiteboardEvent *pEvent = aEvents.GetAt(e);
            CPtrArray *pObjects = pEvent->GetObjectsPointer();

            if (pObjects->GetSize() > 0)
            {
               DrawSdk::DrawObject* pDrawObject = (DrawSdk::DrawObject*)pObjects->ElementAt(0);
               if (pDrawObject != NULL && pDrawObject->GetType() == DrawSdk::RECTANGLE)
               {
                  DrawSdk::Rectangle* pRectangle = (DrawSdk::Rectangle*)pDrawObject;

                  if (pEvent->GetPageNumber() != iLastPageNumber)
                  {
                     pLastBackground = pRectangle;
                     iLastPageNumber = pEvent->GetPageNumber();
                  }
                  else if (pLastBackground != NULL && pRectangle != pLastBackground 
                     && pRectangle->HasSameValues(pLastBackground))
                  {
                     // the same page but another background rectangle with the same values;
                     // replace this:
                     pEvent->SetObject(0, pLastBackground);

                     m_bRectanglesCorrected = true;
                  }
               }
            }
         }
      }



      // Scan the object list for images and texts:
      // images and fonts need to be converted beforehand
      //
      
      // Debug info
      _tprintf(_T("In FlashSetInputFiles(): %d objects found.\n"), m_aObjects.GetSize());
      
      for (int i = 0; i < m_aObjects.GetSize(); ++i)
      {
         // Detect the object
         DrawSdk::DrawObject *pObject = m_aObjects.ElementAt(i);

         // Fill the hash tables for images and text
         FillHashTablesForDrawObject(pObject);
      }
      
      // Debug info
      _tprintf(_T("- CMap Images: %d objects found.\n"), m_mapImageNames.GetCount());
      _tprintf(_T("- CMap Fonts:  %d objects found.\n"), m_mapFontNames.GetCount());
   }

   return hr;
}

UINT CFlashEngine::GetStopJumpMarksFromWhiteboardStream()
{
   HRESULT hr = S_OK;

   if (m_pWhiteboardStream->ContainsMarks())
   {
      // Get a Stop/Jump mark list
      UINT iNumOfMarks = 0;

      hr = m_pWhiteboardStream->ExtractMarks(NULL, &iNumOfMarks);
      _tprintf(_T("- Stop/Jump:   %d marks found.\n"), iNumOfMarks);

      if (!SUCCEEDED(hr))
      {
         // No marks found - but this is no error
      }

      if (iNumOfMarks > 0)
      {
         CStopJumpMark *aMarks = new CStopJumpMark[iNumOfMarks];
         hr = m_pWhiteboardStream->ExtractMarks(aMarks, &iNumOfMarks);

         if (SUCCEEDED(hr))
         {
            // extract stopmarks from the list
            for (int i = 0; i < iNumOfMarks; ++i)
            {
               FLASH_STOPMARK flashStopmark;
               flashStopmark.nMsTime = aMarks[i].GetPosition();
               flashStopmark.nFrame = -1; // will be determined later on ('EmbedObjects()')
               if (aMarks[i].IsStopMark())
               {
                  // Add to stop mark list
                  m_aFlashStopmarks.Add(flashStopmark);
               } 
               else {
                  // For bugfix 5302: add to target mark list
                  m_aFlashTargetmarks.Add(flashStopmark);
               }
            }
         }
         else
         {
            _TCHAR tszMessage[1024];
            _stprintf(tszMessage, _T("Error: Unable to read Marks from Object Queue!\n"));
            _tprintf(tszMessage);
      
            hr = imc_lecturnity_converter_LPLibs_WHITEBOARD_CORRUPT;
         }

         delete[] aMarks;
         aMarks = NULL;
      }
   }
   
   return hr;
}

UINT CFlashEngine::GetInteractionObjectsFromWhiteboardStream()
{
   HRESULT hr = S_OK;

   if (m_pWhiteboardStream->ContainsInteractions())
   {
      // Get a list of interaction objects
      UINT iNumOfInteractions = 0;

      hr = m_pWhiteboardStream->ExtractInteractions(NULL, &iNumOfInteractions);
      _tprintf(_T("- Interaction: %d objects found.\n"), iNumOfInteractions);

      if (!SUCCEEDED(hr))
      {
         // No interactive objects found - but this is no error
      }

      if (iNumOfInteractions > 0)
      {
         CInteractionArea **paInteractions = new CInteractionArea*[iNumOfInteractions];
         hr = m_pWhiteboardStream->ExtractInteractions(paInteractions, &iNumOfInteractions);

         if (SUCCEEDED(hr))
         {
            // Extract interactive objects from the list
            for (int i = 0; i < iNumOfInteractions; ++i)
            {
               // Fill the interactive areas Array 
               m_aInteractionAreas.Add(paInteractions[i]);

               // Fill the hash tables for images and text 
               // -> images and text have to be converted before building the movie
               DrawSdk::DrawObject *pDrawObject= paInteractions[i]->GetSupportObject();
               if (pDrawObject != NULL)
               {
                  FillHashTablesForDrawObject(pDrawObject);
                  m_aInteractionObjects.Add(pDrawObject);
               }

               CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *pNormalObjects 
                  = paInteractions[i]->GetNormalObjects();
               CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *pOverObjects 
                  = paInteractions[i]->GetOverObjects();
               CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *pPressedObjects 
                  = paInteractions[i]->GetPressedObjects();
               CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *pInactiveObjects 
                  = paInteractions[i]->GetInactiveObjects();

               int k = 0;
               for (k = 0; k < pNormalObjects->GetSize(); ++k)
               {
                  FillHashTablesForDrawObject(pNormalObjects->ElementAt(k));
                  m_aInteractionObjects.Add(pNormalObjects->ElementAt(k));
               }
               for (k = 0; k < pOverObjects->GetSize(); ++k)
               {
                  FillHashTablesForDrawObject(pOverObjects->ElementAt(k));
                  m_aInteractionObjects.Add(pOverObjects->ElementAt(k));
               }
               for (k = 0; k < pPressedObjects->GetSize(); ++k)
               {
                  FillHashTablesForDrawObject(pPressedObjects->ElementAt(k));
                  m_aInteractionObjects.Add(pPressedObjects->ElementAt(k));
               }
               for (k = 0; k < pInactiveObjects->GetSize(); ++k)
               {
                  FillHashTablesForDrawObject(pInactiveObjects->ElementAt(k));
                  m_aInteractionObjects.Add(pInactiveObjects->ElementAt(k));
               }
            }
         }
         else
         {
            _TCHAR tszMessage[1024];
            _stprintf(tszMessage, _T("Error: Unable to read Interaction Areas from Object Queue!\n"));
            _tprintf(tszMessage);
      
            hr = imc_lecturnity_converter_LPLibs_WHITEBOARD_CORRUPT;
         }
         
         // Cleanup --> no good idea here
         //delete[] paInteractions;
      }
      // Debug info
      _tprintf(_T("After scanning interactions:\n"));
      _tprintf(_T("- CMap Images: %d objects found.\n"), m_mapImageNames.GetCount());
      _tprintf(_T("- CMap Fonts:  %d objects found.\n"), m_mapFontNames.GetCount());
   }

   return hr;
}

UINT CFlashEngine::GetQuestionnaireFromWhiteboardStream()
{
   HRESULT hr = S_OK;

   if (m_pWhiteboardStream->ContainsQuestions())
   {
      // Get a list of questionnaires
      UINT iNumOfQuestionnaires = 0;

      hr = m_pWhiteboardStream->ExtractQuestionnaires(NULL, &iNumOfQuestionnaires);
      _tprintf(_T("- Questionnaires: %d found.\n"), iNumOfQuestionnaires);

      if (!SUCCEEDED(hr))
      {
         // No questionnaire found - but this is no error
      }

      if (iNumOfQuestionnaires > 0)
      {
         CQuestionnaire **paQuestionnaires = new CQuestionnaire*[iNumOfQuestionnaires];
         hr = m_pWhiteboardStream->ExtractQuestionnaires(paQuestionnaires, &iNumOfQuestionnaires);

         if (SUCCEEDED(hr))
         {
            // Extract questionnaires from the list
            for (int i = 0; i < iNumOfQuestionnaires; ++i)
            {
               // Fill the Questionnaires Array 
               m_aQuestionnaires.Add(paQuestionnaires[i]);

               // Fill the hash tables for images and text for the questionnaire
               // -> images and text have to be converted before building the movie
               CArray<CFeedbackDefinition *, CFeedbackDefinition *> aFeedbacks;
               aFeedbacks.RemoveAll();
               aFeedbacks.Append(paQuestionnaires[i]->GetFeedback());
               _tprintf(_T("- Questionnaire(%d): %d Feedbacks\n"), i, aFeedbacks.GetSize());

               FillHashTablesForFeedbacks(&aFeedbacks);

               // Fill the hash tables for images and text for the questions
               // -> images and text have to be converted before building the movie
               CArray<CQuestion *, CQuestion *> aQuestions;
               aQuestions.RemoveAll();
               aQuestions.Append(paQuestionnaires[i]->GetQuestions());
               _tprintf(_T("- Questions: %d\n"), aQuestions.GetSize());
               for (int j = 0; j < aQuestions.GetSize(); ++j)
               {
                  aFeedbacks.RemoveAll();
                  aFeedbacks.Append(aQuestions.ElementAt(j)->GetFeedback());
                  _tprintf(_T("- Question(%d): %d Feedbacks\n"), j, aFeedbacks.GetSize());

                  FillHashTablesForFeedbacks(&aFeedbacks);
               }
            }
         }
         else
         {
            _TCHAR tszMessage[1024];
            _stprintf(tszMessage, _T("Error: Unable to read Questionnaires from Object Queue!\n"));
            _tprintf(tszMessage);
      
            hr = imc_lecturnity_converter_LPLibs_WHITEBOARD_CORRUPT;
         }
         
         // Cleanup --> no good idea here
         //delete[] paQuestionnaires;
      }
   }

   // TODO: Warning?
   if (hr > S_OK)
   {
      _tprintf(_T("Warning: Corrupt testing data in the ObjectQueue (hr=%d)!\n"), hr);
      hr = S_OK;
   }

   return hr;
}

void CFlashEngine::FillHashTablesForFeedbacks(CArray<CFeedbackDefinition *, CFeedbackDefinition *> *paFeedbacks)
{
   for (int i = 0; i < paFeedbacks->GetSize(); ++i)
   {
      CFeedbackDefinition *pFeedback = paFeedbacks->ElementAt(i);
//      if (!pFeedback->IsInherited())
      {
         CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> aDrawObjects;
         aDrawObjects.RemoveAll();
         aDrawObjects.Append(pFeedback->GetObjects());
         _tprintf(_T("- Feedback: %d DrawObjects\n"), aDrawObjects.GetSize());

         int k = 0;
         for (k = 0; k < aDrawObjects.GetSize(); ++k)
         {
            FillHashTablesForDrawObject(aDrawObjects.ElementAt(k));
            m_aInteractionObjects.Add(aDrawObjects.ElementAt(k));
         }

         CInteractionArea *pOkButton = pFeedback->GetOkButton();
         
         if (pOkButton != NULL)
         {
            CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *pNormalObjects 
               = pOkButton->GetNormalObjects();
            CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *pOverObjects 
               = pOkButton->GetOverObjects();
            CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *pPressedObjects 
               = pOkButton->GetPressedObjects();
            CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *pInactiveObjects 
               = pOkButton->GetInactiveObjects();

            for (k = 0; k < pNormalObjects->GetSize(); ++k)
            {
               FillHashTablesForDrawObject(pNormalObjects->ElementAt(k));
               m_aInteractionObjects.Add(pNormalObjects->ElementAt(k));
            }
            for (k = 0; k < pOverObjects->GetSize(); ++k)
            {
               FillHashTablesForDrawObject(pOverObjects->ElementAt(k));
               m_aInteractionObjects.Add(pOverObjects->ElementAt(k));
            }
            for (k = 0; k < pPressedObjects->GetSize(); ++k)
            {
               FillHashTablesForDrawObject(pPressedObjects->ElementAt(k));
               m_aInteractionObjects.Add(pPressedObjects->ElementAt(k));
            }
            for (k = 0; k < pInactiveObjects->GetSize(); ++k)
            {
               FillHashTablesForDrawObject(pInactiveObjects->ElementAt(k));
               m_aInteractionObjects.Add(pInactiveObjects->ElementAt(k));
            }
         }
      }
   }
}

void CFlashEngine::FillHashTablesForDrawObject(DrawSdk::DrawObject *pObject)
{
   // Detect the object type
   int type = pObject->GetType();

   switch ( type )
   {
      case DrawSdk::IMAGE:
         {
            // Fill the hash table with the images
            DrawSdk::Image *pImageObject = (DrawSdk::Image*)pObject;
            CString csImageName = pImageObject->GetImageName();
            if (pImageObject->IsDndMoveable())
            {
               // Bugfix 3189: DnD image in different sizes --> create copies
               CString csNewImageName;
               GetNewDragableImageName(pImageObject, csNewImageName);
               // Copy image with new name to target directory
               ::CopyFile(csImageName, csNewImageName, FALSE);
               // Add new file to temporary file list
               m_mapTempFiles.SetAt(csNewImageName, NULL);
               // Use new filename for image map 
               csImageName.Format(_T("%s"), csNewImageName);
            }
            m_mapImageNames.SetAt(csImageName, _T(""));
            break;
         }
      case DrawSdk::TEXT:
         {
            // Fill the hash table with the fonts
            DrawSdk::Text *pTextObject = (DrawSdk::Text*)pObject;
            CString csTtfName = pTextObject->GetTtfName();
            if (csTtfName.IsEmpty())
            {
               LOGFONT lf = pTextObject->GetLogFont();
               csTtfName = lf.lfFaceName;
               // this is a dummy value: it ends not on ".ttf"
               // and can thus be detected lateron
            }
            m_mapFontNames.SetAt(csTtfName, _T(""));
            break;
         }
      default:
         // do nothing
         break;
   }
}

UINT CFlashEngine::SetOutputFile(const _TCHAR *tszSwfFile)
{
   if (tszSwfFile)
   {
      // Set the SWF file name
      m_tszSwfFile = new _TCHAR[_tcslen(tszSwfFile)+1];
      _tcscpy(m_tszSwfFile, tszSwfFile);
      m_tszSwfFile[_tcslen(tszSwfFile)] = '\0';

      // Set the Preloader SWF file name
      CString csPreloaderSwf(m_tszSwfFile);
      StringManipulation::GetPath(csPreloaderSwf);
      csPreloaderSwf += _T("\\");
      if (B_USE_PRELOADER_FILENAME)
      {
         // Alternative 1: Identical Preloader file name for all LEC Flash docments
         m_csPreloaderFilename.SetString(STR_PRELOADER_FILENAME);
      }
      else
      {
         // Alternative 2: Preloader file name depending on the output file name
         m_csPreloaderFilename.SetString(m_tszSwfFile);
         StringManipulation::GetFilePrefix(m_csPreloaderFilename);
         m_csPreloaderFilename += STR_PRELOADER_SUFFIX;
         m_csPreloaderFilename += _T(".swf");
      }
      csPreloaderSwf += m_csPreloaderFilename;
      m_tszPreloaderSwfFile = new _TCHAR[csPreloaderSwf.GetLength()+1];
      _tcscpy(m_tszPreloaderSwfFile, csPreloaderSwf);
      m_tszPreloaderSwfFile[csPreloaderSwf.GetLength()] = '\0';

      return S_OK;
   }
   else
      return E_POINTER;
}

UINT CFlashEngine::SetRtmpPath(const _TCHAR *tszRtmpPath)
{
   if (tszRtmpPath)
   {
      m_tszRtmpPath = new _TCHAR[_tcslen(tszRtmpPath)+1];
      _tcscpy(m_tszRtmpPath, tszRtmpPath);
      m_tszRtmpPath[_tcslen(tszRtmpPath)] = '\0';

      return S_OK;
   }
   else
      return E_POINTER;
}

UINT CFlashEngine::SetScormEnabled(bool bScormEnabled)
{
   m_bScormEnabled = bScormEnabled;

   return S_OK;
}

UINT CFlashEngine::SetWbInputDimension(int iWidth, int iHeight)
{
   if (iWidth >= 0 && iHeight >= 0)
   {
      m_iWbInputWidth = iWidth;
      m_iWbInputHeight = iHeight;
      return S_OK;
   }
   else
      return E_INVALIDARG;
}   

UINT CFlashEngine::SetWbOutputDimension(int iWidth, int iHeight)
{
   _tprintf(_T("# SetWbOutputDimension(): %dx%d\n"), iWidth, iHeight);
   if (iWidth >= 0 && iHeight >= 0)
   {
      m_iWbOutputWidth = iWidth;
      m_iWbOutputHeight = iHeight;
      return S_OK;
   }
   else
      return E_INVALIDARG;
}

UINT CFlashEngine::SetMovieDimension(int iWidth, int iHeight)
{
   m_rcMovie.SetRectEmpty();
   if (iWidth >= 0 && iHeight >= 0)
   {
      m_rcMovie.left = 0;
      m_rcMovie.top = 0;
      m_rcMovie.right = iWidth;
      m_rcMovie.bottom = iHeight;
      return S_OK;
   }
   else
      return E_INVALIDARG;
}

UINT CFlashEngine::SetFlashVersion(int iFlashVersion)
{
   m_iFlashVersion = iFlashVersion;
   return S_OK;
}

UINT CFlashEngine::SetAutostartMode(bool bAutostartMode)
{
   m_bAutostartMode = bAutostartMode;
   return S_OK;
}

UINT CFlashEngine::SetSgStandAloneMode(bool bSgStandAloneMode)
{
   if (bSgStandAloneMode)
      m_docType = DOCUMENT_TYPE_DENVER;
   else
      m_docType = DOCUMENT_TYPE_UNKNOWN;

   return S_OK;
}

UINT CFlashEngine::SetDrawEvalNote(bool bDrawEvalNote)
{
   m_bDrawEvalNote = bDrawEvalNote;
   return S_OK;
}

UINT CFlashEngine::SetShowClipsInVideoArea(bool bShowClipsInVideoArea)
{
   m_bShowClipsInVideoArea = bShowClipsInVideoArea;
   return S_OK;
}

UINT CFlashEngine::SetPspDesign(bool bIsPspDesign)
{
   m_bIsPspDesign = bIsPspDesign;
   return S_OK;
}

UINT CFlashEngine::HideLecturnityIdentity(bool bHideLecIdentity)
{
   m_bLecIdentityHidden = bHideLecIdentity;
   return S_OK;
}

UINT CFlashEngine::SetIncreaseKeyframes(bool bIncreaseKeyframes)
{
   m_bIncreaseKeyframes = bIncreaseKeyframes;
   return S_OK;
}

UINT CFlashEngine::GetMovieWidth()
{
   //return m_iMovieWidth;

   // The embedded movie size can differ from the movie size 
   // if input and output slide size is not equal
   int iEmbeddedMovieWidth = (int)(0.5f + (m_dGlobalScale * m_rcMovie.Width()));
         
   // Debug info
   _tprintf(_T("- movie width:  %g\n- movie embedded width:  %d\n"), m_rcMovie.Width(), iEmbeddedMovieWidth);

   return iEmbeddedMovieWidth;
}

UINT CFlashEngine::GetMovieHeight()
{
   //return m_iMovieHeight;

   // The embedded movie size can differ from the movie size 
   // if input and output slide size is not equal
   int iEmbeddedMovieHeight = (int)(0.5f + (m_dGlobalScale * m_rcMovie.Height()));

   // Debug info
   _tprintf(_T("- movie height: %g\n- movie embedded height: %d\n"), m_rcMovie.Height(), iEmbeddedMovieHeight);
   return iEmbeddedMovieHeight;
}

UINT CFlashEngine::GetContentDimensions(const SIZE *sizeMovie, const SIZE *sizeVideoIn, const SIZE *sizePagesIn, const SIZE *sizeLogo, 
                                        bool bDisplayHeaderText, bool bIsControlbarVisible, bool bShowVideoOnPages, 
                                        SIZE *sizeVideoOut, SIZE *sizePagesOut)
{
   UINT hr = S_OK;

   if (sizeMovie->cx > 0 && sizeMovie->cy > 0)
      SetMovieDimension(sizeMovie->cx, sizeMovie->cy);
   else // Movie size unknown
      m_rcMovie.SetRectEmpty();

   m_iAccVideoWidth = sizeVideoIn->cx;
   m_iAccVideoHeight = sizeVideoIn->cy;
   m_iWbInputWidth = sizePagesIn->cx;
   m_iWbInputHeight = sizePagesIn->cy;
   m_iLogoWidth = sizeLogo->cx;
   m_iLogoHeight = sizeLogo->cy;
   _tprintf(_T("# GetContentDimensions() wbInput: %dx%d\n"), m_iWbInputWidth, m_iWbInputHeight);

   m_bHasControlBar = bIsControlbarVisible;
   if ((sizeLogo->cx > 0 && sizeLogo->cy > 0) || bDisplayHeaderText)
      m_bHasMetadata = true;
   if ((m_iWbInputWidth > 0) && (m_iWbInputHeight > 0))
      m_bHasPages = true;
   if ((m_iAccVideoWidth > 0) && (m_iAccVideoHeight > 0))
      m_bHasVideo = true;

   int iNewWbWidth = -1;
   int iNewWbHeight = -1;
   if (m_bIsPspDesign)
   {
      hr = CalculateSpritePositionsPspNew(sizeMovie->cx, sizeMovie->cy, 
                                          m_iAccVideoWidth, m_iAccVideoHeight, 
                                          m_iWbInputWidth, m_iWbInputHeight, 
                                          iNewWbWidth, iNewWbHeight, 
                                          bShowVideoOnPages);
   }
   else if (m_bIsSlidestar)
   {
	   hr = CalculateSpritePositionsSlidestarNew(sizeMovie->cx, sizeMovie->cy, 
                                       m_iAccVideoWidth, m_iAccVideoHeight,
                                       m_iWbInputWidth, m_iWbInputHeight,
                                       /*m_iLogoWidth, m_iLogoHeight,*/
                                       MINIMUM_CONTROLBAR_WIDTH, DEFAULT_CONTROLBAR_HEIGHT,
                                       /*MINIMUM_CONTROLBAR_WIDTH, FLASH_METADATAHEIGHT, */
                                       iNewWbWidth, iNewWbHeight, 
                                       bShowVideoOnPages);
   }
   else
   {
      hr = CalculateSpritePositionsNew(sizeMovie->cx, sizeMovie->cy, 
                                       m_iAccVideoWidth, m_iAccVideoHeight,
                                       m_iWbInputWidth, m_iWbInputHeight,
                                       m_iLogoWidth, m_iLogoHeight,
                                       MINIMUM_CONTROLBAR_WIDTH, DEFAULT_CONTROLBAR_HEIGHT,
                                       MINIMUM_CONTROLBAR_WIDTH, FLASH_METADATAHEIGHT, 
                                       iNewWbWidth, iNewWbHeight, 
                                       bShowVideoOnPages);
    }

   sizeVideoOut->cx = m_rcVideo.Width();
   sizeVideoOut->cy = m_rcVideo.Height();
   sizePagesOut->cx = m_rcPages.Width();
   sizePagesOut->cy = m_rcPages.Height();

   _tprintf(_T("# Pages: %d,%d %dx%d\n"), m_rcPages.left, m_rcPages.top, m_rcPages.Width(), m_rcPages.Height());
   _tprintf(_T("# Video: %d,%d %dx%d\n"), m_rcVideo.left, m_rcVideo.top, m_rcVideo.Width(), m_rcVideo.Height());

/*
   if (m_bIsPspDesign)
   {
      CalculateSpritePositionsPsp(m_iAccVideoWidth, m_iAccVideoHeight, m_iWbInputWidth, m_iWbInputHeight);

      sizeVideoOut->cx = m_rcVideo.Width();
      sizeVideoOut->cy = m_rcVideo.Height();
      sizePagesOut->cx = m_rcPages.Width();
      sizePagesOut->cy = m_rcPages.Height();
   }
   else
   {
      UINT iMovieWidth;
      UINT iMovieHeight;

      hr = GetMovieSize(iMovieWidth, iMovieHeight);

      float fScaleX = (float)sizeMovie->cx / (float)iMovieWidth;
      float fScaleY = (float)sizeMovie->cy / (float)iMovieHeight;
      float fScale = fScaleX < fScaleY ? fScaleX : fScaleY;
      _tprintf(_T("# Pages: %d,%d %dx%d\n"), m_rcPages.left, m_rcPages.top, m_rcPages.Width(), m_rcPages.Height());
      _tprintf(_T("# Scales: fx=%f fy=%f  scale=%f\n"), fScaleX, fScaleY, fScale);

      sizeVideoOut->cx = m_rcVideo.Width() * fScale;
      sizeVideoOut->cy = m_rcVideo.Height() * fScale;
      sizePagesOut->cx = m_rcPages.Width() * fScale;
      sizePagesOut->cy = m_rcPages.Height() * fScale;
   }
*/

   return hr;
}

UINT CFlashEngine::ObtainInsets(RECT *rectInsets, const SIZE *sizeWbInput, bool bDisplayHeaderText, const SIZE *sizeLogo, 
                                const SIZE *sizeVideo, bool bIsControlbarVisible)
{
   float fControlbarHeight = 0.0f;
   
   return CalculateInsets(rectInsets, fControlbarHeight, bDisplayHeaderText, bIsControlbarVisible,
                          sizeWbInput, sizeVideo, sizeLogo);
}

UINT CFlashEngine::SetBasicColors(int r, int g, int b)
{
   // Background color
   m_rgb100.r = r;
   m_rgb100.g = g;
   m_rgb100.b = b;

   // Debug test: the next line should lead to the identical color
   //ChangeColorLightness(m_rgb100, m_rgb100, 1.00f);

   // Colors for highlights / shadows
   ChangeColorLightness(m_rgb100, m_rgb005, 0.05f);
   ChangeColorLightness(m_rgb100, m_rgb033, 0.33333f);
   ChangeColorLightness(m_rgb100, m_rgb045, 0.45f);
   ChangeColorLightness(m_rgb100, m_rgb050, 0.50f);
   ChangeColorLightness(m_rgb100, m_rgb055, 0.55f);
   ChangeColorLightness(m_rgb100, m_rgb067, 0.66667f);
   ChangeColorLightness(m_rgb100, m_rgb080, 0.80f);
   ChangeColorLightness(m_rgb100, m_rgb090, 0.90f);
   ChangeColorLightness(m_rgb100, m_rgb095, 0.95f);
   ChangeColorLightness(m_rgb100, m_rgb105, 1.05f);
   ChangeColorLightness(m_rgb100, m_rgb115, 1.15f);
   ChangeColorLightness(m_rgb100, m_rgb125, 1.25f);
   ChangeColorLightness(m_rgb100, m_rgb150, 1.50f);
   ChangeColorLightness(m_rgb100, m_rgb200, 2.00f);


   // Colors for button symbols (from Alexander Jäger)
///   m_rgbUp.r =   0x8A; m_rgbUp.g =   0x82; m_rgbUp.b =   0x6D;
///   m_rgbOver.r = 0xA6; m_rgbOver.g = 0xC7; m_rgbOver.b = 0xF5;
///   m_rgbDown.r = 0x30; m_rgbDown.g = 0x84; m_rgbDown.b = 0xF7;
   ChangeColorLightness(m_rgb100, m_rgbUp, 0.50f);
   ChangeColorLightness(m_rgb100, m_rgbOver, 0.75f);
   ChangeColorLightness(m_rgb100, m_rgbDown, 0.35f);

   
   // Fill gradients

   // For rect. Buttons
   m_swfLinearGradient = new SWFGradient();
   m_swfLinearGradient->addEntry(0.00f, m_rgb125.r, m_rgb125.g, m_rgb125.b);
   m_swfLinearGradient->addEntry(0.20f, m_rgb100.r, m_rgb100.g, m_rgb100.b);
   m_swfLinearGradient->addEntry(0.80f, m_rgb100.r, m_rgb100.g, m_rgb100.b);
   m_swfLinearGradient->addEntry(1.00f, m_rgb050.r, m_rgb050.g, m_rgb050.b);

   // For oval Buttons
   m_swfRadialGradient = new SWFGradient();
   m_swfRadialGradient->addEntry(0.00f, m_rgb115.r, m_rgb115.g, m_rgb115.b);
   m_swfRadialGradient->addEntry(0.50f, m_rgb100.r, m_rgb100.g, m_rgb100.b);
   m_swfRadialGradient->addEntry(1.00f, m_rgb050.r, m_rgb050.g, m_rgb050.b);

   // For oval slider Buttons
   m_swfRadialGradientSlider = new SWFGradient();
//   m_swfRadialGradientSlider->addEntry(0.00f, m_rgb105.r, m_rgb105.g, m_rgb105.b);
//   m_swfRadialGradientSlider->addEntry(0.50f, m_rgb095.r, m_rgb095.g, m_rgb095.b);
//   m_swfRadialGradientSlider->addEntry(1.00f, m_rgb050.r, m_rgb050.g, m_rgb050.b);
   m_swfRadialGradientSlider->addEntry(0.00f, m_rgb150.r, m_rgb150.g, m_rgb150.b);
   m_swfRadialGradientSlider->addEntry(0.50f, m_rgb125.r, m_rgb125.g, m_rgb125.b);
   m_swfRadialGradientSlider->addEntry(1.00f, m_rgb055.r, m_rgb055.g, m_rgb055.b);


   // For the control bar background
   m_swfLinearGradientBackground = new SWFGradient();
   m_swfLinearGradientBackground->addEntry(0.00f, m_rgb095.r, m_rgb095.g, m_rgb095.b);
   m_swfLinearGradientBackground->addEntry(1.00f, m_rgb105.r, m_rgb105.g, m_rgb105.b);
//   m_swfLinearGradientBackground->addEntry(0.00f, 0xE3, 0xE0, 0xCC);
//   m_swfLinearGradientBackground->addEntry(1.00f, 0xF2, 0xEF, 0xE2);

   // For sliders (mouse up)
   m_swfLinearGradientMouseUp = new SWFGradient();
   m_swfLinearGradientMouseUp->addEntry(0.00f, m_rgb150.r, m_rgb150.g, m_rgb150.b);
   m_swfLinearGradientMouseUp->addEntry(0.45f, m_rgb100.r, m_rgb100.g, m_rgb100.b);
   m_swfLinearGradientMouseUp->addEntry(0.55f, m_rgb100.r, m_rgb100.g, m_rgb100.b);
   m_swfLinearGradientMouseUp->addEntry(1.00f, 0x0A, 0x09, 0x08);

   // For sliders (mouse down)
   m_swfLinearGradientMouseDown = new SWFGradient();
   m_swfLinearGradientMouseDown->addEntry(0.00f, m_rgb150.r, m_rgb150.g, m_rgb150.b);
   m_swfLinearGradientMouseDown->addEntry(0.45f, m_rgb095.r, m_rgb095.g, m_rgb095.b);
   m_swfLinearGradientMouseDown->addEntry(0.55f, m_rgb095.r, m_rgb095.g, m_rgb095.b);
   m_swfLinearGradientMouseDown->addEntry(1.00f, 0x0A, 0x09, 0x08);


   // For rect. Buttons (darker)
   m_swfLinearGradientDarker = new SWFGradient();
   m_swfLinearGradientDarker->addEntry(0.00f, m_rgb080.r, m_rgb080.g, m_rgb080.b);
   m_swfLinearGradientDarker->addEntry(0.20f, m_rgb050.r, m_rgb050.g, m_rgb050.b);
   m_swfLinearGradientDarker->addEntry(0.80f, m_rgb050.r, m_rgb050.g, m_rgb050.b);
   m_swfLinearGradientDarker->addEntry(1.00f, m_rgb033.r, m_rgb033.g, m_rgb033.b);

   // For oval Buttons (darker)
   m_swfRadialGradientDarker = new SWFGradient();
///   m_swfRadialGradientDarker->addEntry(0.00f, m_rgb080.r, m_rgb080.g, m_rgb080.b);
///   m_swfRadialGradientDarker->addEntry(0.50f, m_rgb050.r, m_rgb050.g, m_rgb050.b);
///   m_swfRadialGradientDarker->addEntry(1.00f, m_rgb033.r, m_rgb033.g, m_rgb033.b);
//   m_swfRadialGradientDarker->addEntry(0.00f, 0x55, 0x55, 0x53);
//   m_swfRadialGradientDarker->addEntry(0.50f, m_rgb050.r, m_rgb050.g, m_rgb050.b);
//   m_swfRadialGradientDarker->addEntry(1.00f, 0x95, 0x95, 0x93);
   m_swfRadialGradientDarker->addEntry(0.00f, m_rgb033.r, m_rgb033.g, m_rgb033.b);
   m_swfRadialGradientDarker->addEntry(0.50f, m_rgb050.r, m_rgb050.g, m_rgb050.b);
   m_swfRadialGradientDarker->addEntry(1.00f, m_rgb055.r, m_rgb055.g, m_rgb055.b);

   // For rect. Buttons (lighter)
   m_swfLinearGradientLighter = new SWFGradient();
   m_swfLinearGradientLighter->addEntry(0.00f, m_rgb150.r, m_rgb150.g, m_rgb150.b);
   m_swfLinearGradientLighter->addEntry(0.20f, m_rgb125.r, m_rgb125.g, m_rgb125.b);
   m_swfLinearGradientLighter->addEntry(0.80f, m_rgb125.r, m_rgb125.g, m_rgb125.b);
   m_swfLinearGradientLighter->addEntry(1.00f, m_rgb105.r, m_rgb105.g, m_rgb105.b);

   // For oval Buttons (lighter)
   m_swfRadialGradientLighter = new SWFGradient();
   m_swfRadialGradientLighter->addEntry(0.00f, m_rgb115.r, m_rgb115.g, m_rgb115.b);
   m_swfRadialGradientLighter->addEntry(0.50f, m_rgb100.r, m_rgb100.g, m_rgb100.b);
   m_swfRadialGradientLighter->addEntry(1.00f, m_rgb080.r, m_rgb080.g, m_rgb080.b);


   // For disabled buttons
   // Color for disabled button symbols
   float fGamma = 1.50f;
   MYRGB rgbDisabled1, rgbDisabled2, rgbDisabled3; 
   MYRGB rgbDisabledDarker1, rgbDisabledDarker2, rgbDisabledDarker3; 
   MYRGB rgbDisabledLighter1, rgbDisabledLighter2, rgbDisabledLighter3; 
   ChangeColorLightness(m_rgbUp, m_rgbDisabled, fGamma); // button symbols
   ChangeColorLightness(m_rgb115, rgbDisabled1, fGamma);
   ChangeColorLightness(m_rgb100, rgbDisabled2, fGamma);
   ChangeColorLightness(m_rgb050, rgbDisabled3, fGamma);
   ChangeColorLightness(m_rgb033, rgbDisabledDarker1, fGamma);
   ChangeColorLightness(m_rgb050, rgbDisabledDarker2, fGamma);
   ChangeColorLightness(m_rgb055, rgbDisabledDarker3, fGamma);
   ChangeColorLightness(m_rgb115, rgbDisabledLighter1, fGamma);
   ChangeColorLightness(m_rgb100, rgbDisabledLighter2, fGamma);
   ChangeColorLightness(m_rgb080, rgbDisabledLighter3, fGamma);

   m_swfRadialGradientDisabled = new SWFGradient();
   m_swfRadialGradientDisabled->addEntry(0.00f, rgbDisabled1.r, rgbDisabled1.g, rgbDisabled1.b);
   m_swfRadialGradientDisabled->addEntry(0.50f, rgbDisabled2.r, rgbDisabled2.g, rgbDisabled2.b);
   m_swfRadialGradientDisabled->addEntry(1.00f, rgbDisabled3.r, rgbDisabled3.g, rgbDisabled3.b);

   m_swfRadialGradientDisabledDarker = new SWFGradient();
   m_swfRadialGradientDisabledDarker->addEntry(0.00f, rgbDisabledDarker1.r, rgbDisabledDarker1.g, rgbDisabledDarker1.b);
   m_swfRadialGradientDisabledDarker->addEntry(0.50f, rgbDisabledDarker2.r, rgbDisabledDarker2.g, rgbDisabledDarker2.b);
   m_swfRadialGradientDisabledDarker->addEntry(1.00f, rgbDisabledDarker3.r, rgbDisabledDarker3.g, rgbDisabledDarker3.b);

   m_swfRadialGradientDisabledLighter = new SWFGradient();
   m_swfRadialGradientDisabledLighter->addEntry(0.00f, rgbDisabledLighter1.r, rgbDisabledLighter1.g, rgbDisabledLighter1.b);
   m_swfRadialGradientDisabledLighter->addEntry(0.50f, rgbDisabledLighter2.r, rgbDisabledLighter2.g, rgbDisabledLighter2.b);
   m_swfRadialGradientDisabledLighter->addEntry(1.00f, rgbDisabledLighter3.r, rgbDisabledLighter3.g, rgbDisabledLighter3.b);


   return S_OK;
}

UINT CFlashEngine::RescaleImages(float scaleX, float scaleY)
{
   HRESULT hr = S_OK;

   if (!m_tszSwfFile)
      hr = E_POINTER;


   if (SUCCEEDED(hr) && !m_mapImageNames.IsEmpty())
   {
      _tprintf(_T("\nRescaling Images..."));

      int i = 0;
      for (i = 0; SUCCEEDED(hr) && i < m_aObjects.GetSize(); ++i)
      {
         DrawSdk::DrawObject *pObject = m_aObjects.ElementAt(i);
         hr = RescaleImageObject(pObject, scaleX, scaleY);
      }

      for (i = 0; SUCCEEDED(hr) && i < m_aInteractionObjects.GetSize(); ++i)
      {
         DrawSdk::DrawObject *pObject = m_aInteractionObjects.ElementAt(i);
         hr = RescaleImageObject(pObject, scaleX, scaleY);
      }

      _tprintf(_T(" Done.\n\n"));
   }

   return hr;
}

UINT CFlashEngine::RescaleImageObject(DrawSdk::DrawObject *pObject, float scaleX, float scaleY)
{
   HRESULT hr = S_OK;
   // Detect the object type
   int type = pObject->GetType();

   switch ( type )
   {
      case DrawSdk::IMAGE:
         {
            DrawSdk::Image *pImageObject = (DrawSdk::Image*)pObject;
            CString csImageIn = pImageObject->GetImageName();
            if (pImageObject->IsDndMoveable())
            {
               CString csNewImageName;
               GetNewDragableImageName(pImageObject, csNewImageName);
               csImageIn.Format(_T("%s"), csNewImageName);
            }

            CString csImageOut;

            hr = WriteRescaledImage(pImageObject, csImageIn, csImageOut, scaleX, scaleY);

            // set the (temporary) image output file name
            m_mapImageNames.SetAt(csImageIn, csImageOut);

            break;
         }
      default:
         {
            // do nothing
            break;
         }
   }

   return hr;
}

UINT CFlashEngine::WriteRescaledImage(DrawSdk::Image *pImageObject, CString csImageIn, CString &csImageOut, 
                                      float scaleX, float scaleY)
{
   HRESULT hr = S_OK;

   // Get output path from output file
   CString csPathOut = GetOutputPath();

   // Get the image name without path
   CString csImageName = csImageIn;
   StringManipulation::GetFilename(csImageName);

   // Get the suffix from the image name --> conversion of GIF, PNG ,JPG only
   CString csImageSuffix = csImageName;
   StringManipulation::GetFileSuffix(csImageSuffix);
   csImageSuffix.MakeLower();

   // Cut the suffix from the image name
   StringManipulation::GetFilePrefix(csImageName);

   // Set the output image name
   //csImageOut = csPathOut + _T("\\") + csImageName + _T("_tmp.") + csImageSuffix;
   csImageOut.Format(_T("%s\\%s_tmp.%s"), csPathOut, csImageName, csImageSuffix);
   int nLen = csImageOut.GetLength();
   WCHAR* wszImageOut = new WCHAR[nLen+1];
#ifdef _UNICODE
   wcscpy(wszImageOut, csImageOut);
#else
   ::MultiByteToWideChar(CP_ACP, 0, csImageOut, -1, wszImageOut, nLen+1);
#endif

///   _tprintf(_T("--> %d %S\n"), nLen, wszImageOut);

   if (csImageSuffix.Compare(_T("gif")) == 0)
   {
      hr = pImageObject->WriteScaledImageFile(wszImageOut, scaleX, scaleY, L"image/gif");
   }
   else if (csImageSuffix.Compare(_T("png")) == 0)
   {
      hr = pImageObject->WriteScaledImageFile(wszImageOut, scaleX, scaleY, L"image/png");
   }
   else if (csImageSuffix.Compare(_T("jpg")) == 0 || csImageSuffix.Compare(_T("jpeg")) == 0)
   {
      hr = pImageObject->WriteScaledImageFile(wszImageOut, scaleX, scaleY, L"image/jpeg");
   }
   else
   {
      // do nothing here, other image types are not supported in Flash
   }

   // Add path & name of temporary file to a container
   m_mapTempFiles.SetAt(csImageOut, NULL);

   delete[] wszImageOut;

   return hr;
}

UINT CFlashEngine::ConvertImage(CString csImageIn, CString csImageTmp, CString &csImageOut)
{
   // If an Image is converted without resizing, use 'csImageIn' as 'csImageTmp'
   HRESULT hr = S_OK;

   _TCHAR szCommandLine[3*MAX_PATH+256];

   // Get output path from output file
   CString csPathOut = GetOutputPath();

   // Get the image name without path
   CString csImageName = csImageTmp;
   StringManipulation::GetFilename(csImageName);

   // Get the suffix from the image name --> conversion of GIF, PNG (,JPG) only
   CString csImageSuffix = csImageName;
   StringManipulation::GetFileSuffix(csImageSuffix);
   csImageSuffix.MakeLower();

   // Cut the suffix from the image name
   StringManipulation::GetFilePrefix(csImageName);

   // Set the output image name
   //csImageOut = csPathOut + _T("\\") + csImageName + _T(".dbl");
   csImageOut.Format(_T("%s\\%s.dbl"), csPathOut, csImageName);

   // Set the String for the command line
   // This can be either a GIF or PNG conversion - or a JPG
   bool bIsSupportedImageFormat = true;
   if (csImageSuffix.Compare(_T("gif")) == 0)
   {
      _stprintf(szCommandLine, _T("%sgif2dbl.exe \"%s\" \"%s\""), GetFlashBinPath(), csImageTmp, csImageOut);
      hr = CommandCall(szCommandLine);
   }
   else if (csImageSuffix.Compare(_T("png")) == 0)
   {
      _stprintf(szCommandLine, _T("%spng2dbl.exe \"%s\" \"%s\""), GetFlashBinPath(), csImageTmp, csImageOut);
      hr = CommandCall(szCommandLine);
   }
   else if (csImageSuffix.Compare(_T("jpg")) == 0 || csImageSuffix.Compare(_T("jpeg")) == 0)
   {
      // Copy the JPG image to a temporary file in the output directory
      //csImageOut = csPathOut + _T("\\") + csImageName + _T(".jpg");
      csImageOut.Format(_T("%s\\%s.jpg"), csPathOut, csImageName);
      
      _stprintf(szCommandLine, _T("")); // noop, do nothing later on
   
      if (csImageTmp.Compare(csImageOut) != 0)
      {
         // doesn't work: 
         //_stprintf(szCommandLine, _T("copy \"%s\" \"%s\""), csImageTmp, csImageOut);
         
         BOOL bSuccess = ::CopyFile(csImageTmp, csImageOut, FALSE);

         if (!bSuccess)
         {
            DWORD dwError = ::GetLastError();

            _TCHAR tszMessage[1024];
            _stprintf(tszMessage, _T("! Copy failed.\n  Error: 0x%08x\n"), dwError);
            _tprintf(tszMessage);
      
            hr = imc_lecturnity_converter_LPLibs_COMMAND_EXECUTION_FAILED;
         }
      }
      // else it can be the same input and output directory
      // then no copy needed for jpgs and the file shouldn't be deleted (twice later)
      
   }
   else
   {
      bIsSupportedImageFormat = false;
   }

   if (!bIsSupportedImageFormat) {
      m_csErrorDetail.Empty();
      m_csErrorDetail += csImageTmp;
  
      hr = imc_lecturnity_converter_LPLibs_IMAGE_CONVERSION_FAILED;
   }

   if (SUCCEEDED(hr))
   {
//      if (_tcslen(szCommandLine) > 0)
//         hr = CommandCall(szCommandLine);

      LFile lFile(csImageOut);

      if (lFile.Exists())
      {
         // Update CMap: Set the corresponding output file
         m_mapImageNames.SetAt(csImageIn, csImageOut);
            
         // Add path & name of temporary file to a container
         if (csImageIn.Compare(csImageOut) != 0)
            m_mapTempFiles.SetAt(csImageOut, NULL);
      }
      else
      {
         m_csErrorDetail.Empty();
         m_csErrorDetail += csImageTmp;
  
         hr = imc_lecturnity_converter_LPLibs_IMAGE_CONVERSION_FAILED;
      }
   }

   if (!SUCCEEDED(hr)) {
      // Error: Conversion step failed
      // Bugfix 5616: Continue anyway?
      CString csWarning;
      csWarning.LoadStringW(IDS_WARNING);
      CString csWarnImgConversionFailed;
      csWarnImgConversionFailed.LoadString(IDS_WARN_IMG_CONVERSION_FAILED);
      CString csDescImgConversionFailed;
      csDescImgConversionFailed.LoadString(IDS_DESC_IMG_CONVERSION_FAILED);
      CString csWarningMsg;
      csWarningMsg.Format(_T("%s\n\n%s\n\n%s"), csWarnImgConversionFailed, csImageIn, csDescImgConversionFailed);
      int iResult = ::MessageBox(::GetForegroundWindow(), 
                                 csWarningMsg, csWarning, 
                                 MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2);

      if (IDYES == iResult) {
         // Ignore the warning and continue 
         // Update CMap: Remove the entry 
         m_mapImageNames.RemoveKey(csImageIn);
         hr = S_OK;
      } else {
         // Abort/Cancel publishing process
         hr = imc_lecturnity_converter_LPLibs_CANCELLED;
      }
   }

   return hr;
}

UINT CFlashEngine::ConvertImages()
{
   HRESULT hr = S_OK;

   if (!m_tszSwfFile)
      hr = E_POINTER;

   // Determine output size for the slides
   if (m_iWbInputWidth == -1)
   {
      // try size estimation with first object (background rectangle)
      m_iWbInputWidth = (int)m_aObjects[0]->GetWidth();
      m_iWbInputHeight = (int)m_aObjects[0]->GetHeight();
   }

   if (m_iWbOutputWidth == -1)
   {
      m_iWbOutputWidth = m_iWbInputWidth;
      m_iWbOutputHeight = m_iWbInputHeight;
   }

   float fScaleX = 1.0f; float fScaleY = 1.0f;
   if ((m_iWbInputWidth > 0) && (m_iWbInputHeight > 0))
   {
      fScaleX = (float)m_iWbOutputWidth/(float)m_iWbInputWidth;
      fScaleY = (float)m_iWbOutputHeight/(float)m_iWbInputHeight;
   }

   // Debug info
   _tprintf(_T("- m_iWbInputWidth:  %d   m_iWbInputHeight:  %d\n"), m_iWbInputWidth, m_iWbInputHeight);
   _tprintf(_T("- m_iWbOutputWidth: %d   m_iOWbutputHeight: %d\n"), m_iWbOutputWidth, m_iWbOutputHeight);
   _tprintf(_T("- scaleX: %g   scaleY: %g\n"), fScaleX, fScaleY);


   if (SUCCEEDED(hr) && !m_mapImageNames.IsEmpty())
   {
      // We have to rescale the images first
      // This method also modifies the output file name in the image hash table
      hr = RescaleImages(fScaleX, fScaleY);

      // Now convert the images into the DBL format
      POSITION pos = m_mapImageNames.GetStartPosition();
      while(SUCCEEDED(hr) && pos != NULL)
      {
         if (m_bCancelRequested == true)
            break;

         CString csImageIn;
         CString csImageOut;
         m_mapImageNames.GetNextAssoc(pos, csImageIn, csImageOut);

         // If 'csImageOut' is empty, then the images are not rescaled in the step before
         // otherwise: images are rescaled. We have to use 'csImageOut' as input file name for conversion
         CString csImageTmp;
         if (!csImageOut.IsEmpty())
            csImageTmp = csImageOut;
         else
            csImageTmp = csImageIn;

         LFile mFile(csImageIn);
         
         if (!mFile.Exists())
         {
            m_csErrorDetail.Empty();
            m_csErrorDetail += csImageIn;
        
            hr = imc_lecturnity_converter_LPLibs_INPUT_FILE_MISSING;
         }

         if (SUCCEEDED(hr))
         {
            hr = ConvertImage(csImageIn, csImageTmp, csImageOut);
         }
      }
   }

   return hr;
}

UINT CFlashEngine::ConvertFonts()
{
   HRESULT hr = S_OK;

   if (!m_tszSwfFile)
      hr = E_POINTER;

   if (SUCCEEDED(hr) && !m_mapFontNames.IsEmpty())
   {
      _TCHAR szCommandLine[3*MAX_PATH+256];

      POSITION pos = m_mapFontNames.GetStartPosition();
      while(SUCCEEDED(hr) && pos != NULL)
      {
         if (m_bCancelRequested == true)
            break;

         CString csTtfIn;
         CString csFdbOut;
         m_mapFontNames.GetNextAssoc(pos, csTtfIn, csFdbOut);

         // Get input path from input audio file 
         // we need it to look for font files in it
         CString csPathIn = m_tszLadFile;
         StringManipulation::GetPath(csPathIn);

         // Get output path from output file
         CString csPathOut = GetOutputPath();
         
         // Get the suffix from the TTF file --> conversion to FFT and FDB
         CString csTtfSuffix = csTtfIn;
         StringManipulation::GetFileSuffix(csTtfSuffix);
         csTtfSuffix.MakeLower();
         
         // Cut the prefix from the font file name
         CString csTtfName = csTtfIn;
         StringManipulation::GetFilePrefix(csTtfName);
         
         // Get the Windows directory (%SystemRoot%)
         // (assuming %SystemRoot%\Fonts is the default fonts directory)
         _TCHAR* winDir = _tgetenv(_T("WINDIR"));
         
         // Set the output file names
         //CString csFftOut = csPathOut + _T("\\") + csTtfName + _T(".fft");
         //csFdbOut = csPathOut + _T("\\") + csTtfName + _T(".fdb");
         CString csFftOut;
         csFftOut.Format(_T("%s\\%s.fft"), csPathOut, csTtfName);
         csFdbOut.Format(_T("%s\\%s.fdb"), csPathOut, csTtfName);

         // The path and file name of the TTF file (determined below)
         CString csTtfPathAndFile;

         // Convert TTF only
         // Bugfix 5614: Consider also TTC (True Type Collection) 
         boolean bIsTrueTypeFont = false;
         if (csTtfSuffix.Compare(_T("ttf")) == 0)
             bIsTrueTypeFont = true;
         if (csTtfSuffix.Compare(_T("ttc")) == 0)
             bIsTrueTypeFont = true;

         if (bIsTrueTypeFont)
         {
            if (SUCCEEDED(hr))
            {
               // Look for the TTF file
               // it can be:
               //  - already described with the correct path
               //  - in the input directory 
               //  - in the %SystemRoot%\Fonts directory
               // If not found: return warning 

               // 1. Try the TTF directly - maybe it is described with the full path
               csTtfPathAndFile.Format(_T("%s"), csTtfIn);
               LFile lFile_1(csTtfPathAndFile);

               if (!lFile_1.Exists())
               {
                  // 2. Try input directory
                  csTtfPathAndFile.Format(_T("%s\\%s"), csPathIn, csTtfIn);
                  LFile lFile_2(csTtfPathAndFile);

                  if (!lFile_2.Exists())
                  {
                     // 3. Try %SystemRoot%\Fonts
                     csTtfPathAndFile.Format(_T("%s\\Fonts\\%s"), winDir, csTtfIn);
                     LFile lFile_3(csTtfPathAndFile);

                     if (!lFile_3.Exists())
                     {
                        // TTF not found! --> Warning
                        _tprintf(_T("Warning: Font %s not found for conversion!\n"), csTtfIn);

                        m_csErrorDetail.Empty();
                        m_csErrorDetail += csTtfIn;

                        // Add font name to font list
                        m_mapFontNamesFontNotFound.SetAt(csTtfIn, NULL);

                        hr = imc_lecturnity_converter_LPLibs_WARN_FONT_NOT_FOUND;

                        if ((m_hrPossibleWarning & imc_lecturnity_converter_LPLibs_WARN_FONT_NOT_FOUND) 
                            != imc_lecturnity_converter_LPLibs_WARN_FONT_NOT_FOUND)
                        {
                           m_hrPossibleWarning += imc_lecturnity_converter_LPLibs_WARN_FONT_NOT_FOUND;
                        }
                     }
                  }
               }
            }

            if (S_OK == hr)
            {
               // Conversion TTF --> FFT
               _stprintf(szCommandLine, _T("%sttf2fft.exe \"%s\" -o \"%s\""), GetFlashBinPath(), csTtfPathAndFile, csFftOut);
               hr = CommandCall(szCommandLine);

               LFile lFile(csFftOut);

               if (SUCCEEDED(hr) && lFile.Exists())
               {
                  // Add path & name of temporary file to a container
                  m_mapTempFiles.SetAt(csFftOut, NULL);
               }
            }
  
            if (S_OK == hr)
            {
               // Conversion FFT --> FDB
               _stprintf(szCommandLine, _T("%smakefdb.exe \"%s\" \"%s\""), GetFlashBinPath(), csFftOut, csFdbOut);
               hr = CommandCall(szCommandLine);

               LFile lFile(csFdbOut);

               if (SUCCEEDED(hr))
               {
                  if (lFile.Exists())
                  {
                     // Update CMap: Set the corresponding output file
                     m_mapFontNames.SetAt(csTtfIn, csFdbOut);
                     
                     // Add path & name of temporary file to a container
                     m_mapTempFiles.SetAt(csFdbOut, NULL);
                  }
                  else
                  {
                     m_csErrorDetail.Empty();
                     m_csErrorDetail += csTtfIn;
                     
                     hr = imc_lecturnity_converter_LPLibs_FONT_CONVERSION_FAILED;
                  }
                  
               }
            }
 
         }
         // else no ttf specified; gets handled below

      } // for all font names
   }

   return hr;
}

UINT CFlashEngine::ConvertAudioToMp3Ds(_TCHAR *tszOutputFile, int iAudioKBit) {
    return ConvertAudioToMp3Ds(m_tszLadFile, tszOutputFile, iAudioKBit);
}

DWORD g_dwRegisterMp3 = 0;

UINT CFlashEngine::ConvertAudioToMp3Ds(_TCHAR *tszInputFile, _TCHAR *tszOutputFile, int iAudioKBit) {
    if (tszInputFile == NULL)
        return E_POINTER;

    if (_taccess(tszInputFile, 04) != 0)
        return E_INVALIDARG;

    HRESULT hr = ::CoInitialize(NULL);

    CComPtr<ICaptureGraphBuilder2> pBuilder;
    if (SUCCEEDED(hr)) {
        hr = ::CoCreateInstance(CLSID_CaptureGraphBuilder2, 
            NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void **)&pBuilder);
    }

    CComPtr<IGraphBuilder> pGraph;
    if (SUCCEEDED(hr)) {
        hr = ::CoCreateInstance(CLSID_FilterGraph, 
            NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
    }

    if (SUCCEEDED(hr))
        hr = pBuilder->SetFiltergraph(pGraph);;

    // This might make graph building faster; if not its just an unused filter
    CComPtr<IBaseFilter> pLadParser;
    if (SUCCEEDED(hr))
        hr = CoCreateInstance(CLSID_LadFilter, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pLadParser);

    if (SUCCEEDED(hr))
        hr = pGraph->AddFilter(pLadParser, L"LAD Parser");


    CComBSTR bstrInputAudio(tszInputFile);
    
    CComPtr<IBaseFilter> pFileSource;
    if (SUCCEEDED(hr))
        pGraph->AddSourceFilter(bstrInputAudio, L"Input Filter", &pFileSource);


    CComPtr<IBaseFilter> pMp3Encoder;

    if (SUCCEEDED(hr))
        hr = CDsHelper::CreateLameMp3Filter(pMp3Encoder, iAudioKBit);

    // add MP3 Encoder to the graph
    if (SUCCEEDED(hr))
        hr = pGraph->AddFilter(pMp3Encoder, L"MP3 Encoder");


    

    CString csOutputFile; 
    if (tszOutputFile != NULL)
        csOutputFile = tszOutputFile;
    else {
        CString csInputFile = tszInputFile;
        if (csInputFile.Right(4).GetAt(0) == _T('.'))
            csOutputFile = csInputFile.Left(csInputFile.GetLength() - 4);
        else
            csOutputFile = csInputFile;

        csOutputFile += _T(".mp3");
    }


    CComPtr<IBaseFilter> pMp3FileWrite;
    if (SUCCEEDED(hr))
        hr = CoCreateInstance(CLSID_FileWriter, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pMp3FileWrite);

    CComPtr<IFileSinkFilter2> pMp3SetFileName;
    if (SUCCEEDED(hr))
        hr = pMp3FileWrite->QueryInterface(IID_IFileSinkFilter2, (void **)&pMp3SetFileName);

    if (SUCCEEDED(hr))
        hr = pMp3SetFileName->SetFileName(csOutputFile, NULL);

    if (SUCCEEDED(hr))
        hr = pGraph->AddFilter(pMp3FileWrite, L"Direct File Writer");


    // For the connection between mp3 encoder and file writer filter to work
    // Lame 3.98 is necessary; otherwise they cannot agree to a format (file writeer expects "stream").
    if (SUCCEEDED(hr))
        hr = pBuilder->RenderStream(NULL, NULL, pFileSource, pMp3Encoder, pMp3FileWrite);
    // Note: Do not specify media type; otherwise a "Wave parser" filter cannot be added
    // automatically (which is strange).

    //Connect current DirectShow graph to the ROT.
    if (SUCCEEDED(hr) && g_dwRegisterMp3 == 0)
        CDsHelper::AddToRot(pGraph, &g_dwRegisterMp3);

#ifdef _DEBUG
    //::MessageBox(NULL, _T("Ready to inspect."), NULL, MB_OK);
#endif

    if (SUCCEEDED(hr))
        hr = CDsHelper::WaitForFinishSimple(pGraph); // no progress tracking with this simple graph

    ::CoUninitialize();

    // Get MP3 file name from the output SWF file
    CString csMp3File(m_tszSwfFile);
    StringManipulation::GetPathFilePrefix(csMp3File);
    csMp3File += _T(".tmp");

    m_tszMp3File = new _TCHAR[csMp3File.GetLength()+1];
    _tcscpy(m_tszMp3File, csMp3File);
    m_tszMp3File[csMp3File.GetLength()] = '\0';

    return hr;
}

UINT CFlashEngine::BuildFont(CString csFont)
{
   HRESULT hr = S_OK;

   LFile fFile(csFont);
   if (!fFile.Exists())
   {
      m_csErrorDetail.Empty();
      m_csErrorDetail += csFont;
      
      hr = imc_lecturnity_converter_LPLibs_INPUT_FILE_MISSING;
   }
   
   return hr;
}

UINT CFlashEngine::SetStandardNavigationStates(int nNavigationControlBar, int nNavigationStandardButtons, 
                                               int nNavigationTimeLine, int nNavigationTimeDisplay, 
                                               int nNavigationDocumentStructure, int nNavigationPluginContextMenu)
{
   HRESULT hr = S_OK;

   m_iNavigationControlBar          = nNavigationControlBar;
   m_iNavigationStandardButtons     = nNavigationStandardButtons;
   m_iNavigationTimeLine            = nNavigationTimeLine;
   m_iNavigationTimeDisplay         = nNavigationTimeDisplay;
   m_iNavigationDocumentStructure   = nNavigationDocumentStructure;
   m_iNavigationPluginContextMenu   = nNavigationPluginContextMenu;

   // Note: So far we do not have any 'document structure' in the Flash files

   // Hide control bar elements, if the control bar itself is hidden
   if (m_iNavigationControlBar == 2) // 2 = hidden
   {
      m_iNavigationStandardButtons = 2;
      m_iNavigationTimeLine = 2;
      m_iNavigationTimeDisplay = 2;
   }

   _tprintf(_T("- Standard navigation states: %d %d %d %d %d %d\n"), m_iNavigationControlBar, m_iNavigationStandardButtons, m_iNavigationTimeLine, nNavigationTimeDisplay, nNavigationDocumentStructure, nNavigationPluginContextMenu);
   
   return hr;
}

UINT CFlashEngine::BuildMovie(CMap<CString, LPCTSTR, CString, LPCTSTR> *pMapMetadata, 
                              Gdiplus::ARGB refBasicColor, Gdiplus::ARGB refBackColor, Gdiplus::ARGB refTextColor, 
                              _TCHAR *tszLogoFile, _TCHAR *tszLogoUrl, 
                              int iFlashFormatType, bool bShowVideoOnPages)
{
   HRESULT hr = S_OK;

   if (m_bIncreaseKeyframes)
      m_fClipSynchronisationDelta = 1.0;
   else
      m_fClipSynchronisationDelta = 3.0;

   //m_bIsSlidestar = bIsSlidestar;
   //m_bIsPspDesign = bIsPspDesign;
   switch (iFlashFormatType)
   {
   case imc_lecturnity_converter_LPLibs_FLASH_FORMAT_NORMAL:
      m_bIsSlidestar = false;
      m_bIsPspDesign = false;
      m_bIsNewLecPlayer = false;
      break;
   case imc_lecturnity_converter_LPLibs_FLASH_FORMAT_SLIDESTAR:
      m_bIsSlidestar = true;
      m_bIsPspDesign = false;
      m_bIsNewLecPlayer = false;
      break;
   case imc_lecturnity_converter_LPLibs_FLASH_FORMAT_PSP:
      m_bIsSlidestar = false;
      m_bIsPspDesign = true;
      m_bIsNewLecPlayer = false;
      break;
   case imc_lecturnity_converter_LPLibs_FLASH_FORMAT_FLEX:
      m_bIsSlidestar = true;
      m_bIsPspDesign = false;
      m_bIsNewLecPlayer = true;
      break;
   }
      
   if (m_docType != DOCUMENT_TYPE_DENVER &&
       (!m_pWhiteboardStream || m_aObjects.GetSize() < 1))
      hr = E_UNEXPECTED;


   // Set the Basic color for the WebPlayer
   if (SUCCEEDED(hr))
      hr = SetBasicColors((refBasicColor >> 16) & 0xff, (refBasicColor >> 8) & 0xff, (refBasicColor >> 0) & 0xff);


   // Ming init stuff
   if (SUCCEEDED(hr))
   { 
      Ming_init();
      Ming_useSWFVersion(m_iFlashVersion);
   }

   // Calculate scale factor
   if (SUCCEEDED(hr))
   {
      hr = CalculateScaleFactor();
      // if psp may be insert *fContentScale
      //if (m_bIsPspDesign)
      //   m_dGlobalScale = m_dContentScale;
   }

   // Look for metadata
   bool bHasMetadata = false;
   if (SUCCEEDED(hr))
   {
      if (!m_bIsPspDesign && !m_bIsSlidestar)
         hr = LookForMetadata(bHasMetadata, pMapMetadata);
   }

   // Look for logo image
   bool bShowLogoImage = false;
   SWFBitmap *pBmpLogo = NULL;
   if (SUCCEEDED(hr))
   {
	   if (!m_bIsPspDesign && !m_bIsSlidestar)
         hr = InsertLogoImage(tszLogoFile, tszLogoUrl, bShowLogoImage, &pBmpLogo);
   }

   m_bHasMetadata = false;
   if (pBmpLogo != NULL || bHasMetadata)
      m_bHasMetadata = true;


   // If audio length is not set so far (e.g. combined clips without video): 
   // --> read audio length from LAD
   if (SUCCEEDED(hr) && m_iAudioLengthMs < 0)
   {
      CString csLadFile(m_tszLadFile);
      
      hr = ::CoInitialize(NULL);
      {            
         CComPtr<IMediaDet> pMediaDet;
         if (SUCCEEDED(hr))
            hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, IID_IMediaDet, (void **) &pMediaDet);

         CComBSTR bstrInputAudio(csLadFile);
         if (SUCCEEDED(hr))
            hr = pMediaDet->put_Filename(bstrInputAudio);

         double dLength = -1.0;
         if (SUCCEEDED(hr))
            hr = pMediaDet->get_StreamLength(&dLength);

         if (SUCCEEDED(hr) && dLength > 0)
            m_iAudioLengthMs = (int)(dLength * 1000.0f + 0.5f);

         
         _tprintf(_T("BuildMovie: dlength: %f, audioLength: %d, ladFile: %s\n"), dLength, m_iAudioLengthMs, csLadFile);
      }
      
      ::CoUninitialize();
   }

   // Combined Clips without video: video length is not known --> use audio length
   if (SUCCEEDED(hr) && (m_iVideoLengthMs < 0) && (m_iAccVideoWidth > 0 && m_iAccVideoHeight > 0))
      m_iVideoLengthMs = m_iAudioLengthMs;


   // Find out the type of recording
   if (SUCCEEDED(hr))
   {
      // find out which components are in the recording
      m_bHasVideo = ((m_iAccVideoWidth > 0L) && 
                   (m_iAccVideoHeight > 0L)) ? true : false;

      m_bHasClips = (m_iNumOfClips > 0) ? true : false; 

      m_bHasMP3Audio = ((m_tszVideoFlvFile == NULL) && (m_tszMp3File != NULL)) ? true : false;
      
      m_bHasPages = ((m_pWhiteboardStream != NULL) && (m_pPageStream != NULL)) ? true : false;

      if(m_bIsSlidestar)
         m_bHasControlBar = false;
      else
         m_bHasControlBar = (m_iNavigationControlBar == 2) ? false : true;

   }

   // Determine the length of the movie
   if (m_bHasMP3Audio && m_iAudioLengthMs >= 0)
      m_iMovieLengthMs = m_iAudioLengthMs;
   else if (m_bHasVideo && m_iVideoLengthMs >= 0)
      m_iMovieLengthMs = m_iVideoLengthMs;

   if (m_iMovieLengthMs < 0 && m_bHasPages)
      m_iMovieLengthMs = m_iWbLengthMs;


   // New Flex LEC Player: Video and clips (if any) are embedded in the Flex Application
   if (m_bIsNewLecPlayer)
   {
      m_bHasVideo = false;
      m_bHasClips = false;
   }

   // Calculate the movie dimension
   // Borders, metadata and logo image lead to a larger movie size than the slide size itself
   // - also an accompanying video
   //
   // --> We have to calculate the real movie size considering the scale factor 
   // and an offset for the upper left corner of the slides

/*
   if (SUCCEEDED(hr))
   {
      if (m_bIsPspDesign)
	   {
         hr = CalculateSpritePositionsPsp(m_iAccVideoWidth, m_iAccVideoHeight,
                                          m_iWbInputWidth, m_iWbInputHeight);	  
	   }
	   else if (m_bIsSlidestar)
	   {
		   hr = CalculateSpritePositionsSlidestar(m_iAccVideoWidth, m_iAccVideoHeight,
                                                m_iWbOutputWidth, m_iWbOutputHeight);
	   }
      else
      {
         hr = CalculateSpritePositions(m_iAccVideoWidth, m_iAccVideoHeight,
                                       m_iWbInputWidth, m_iWbInputHeight,   
                                       m_iLogoWidth, m_iLogoHeight,
                                       MINIMUM_CONTROLBAR_WIDTH, DEFAULT_CONTROLBAR_HEIGHT,
                                       MINIMUM_CONTROLBAR_WIDTH, FLASH_METADATAHEIGHT);
      }
   }

   // Set the dimension of the swf movie
   if (SUCCEEDED(hr) && (!m_bIsPspDesign || m_rcMovie.IsRectEmpty()))
   {
      hr = CalculateMovieDimension(m_bHasMetadata, m_bHasControlBar, m_bHasPages);
   }
*/

   if (SUCCEEDED(hr))
   {
      if (m_bIsPspDesign)
	   {
         hr = CalculateSpritePositionsPspNew(m_rcMovie.Width(), m_rcMovie.Height(), 
                                             m_iAccVideoWidth, m_iAccVideoHeight,
                                             m_iWbInputWidth, m_iWbInputHeight, 
                                             m_iWbOutputWidth, m_iWbOutputHeight, 
                                             bShowVideoOnPages);
	   }
	   else if (m_bIsSlidestar)
	   {
		   hr = CalculateSpritePositionsSlidestarNew(m_rcMovie.Width(), m_rcMovie.Height(), 
                                          m_iAccVideoWidth, m_iAccVideoHeight,
                                          m_iWbInputWidth, m_iWbInputHeight,   
                                          /*m_iLogoWidth, m_iLogoHeight,*/
                                          MINIMUM_CONTROLBAR_WIDTH, DEFAULT_CONTROLBAR_HEIGHT,
                                          /*MINIMUM_CONTROLBAR_WIDTH, FLASH_METADATAHEIGHT, */
                                          m_iWbOutputWidth, m_iWbOutputHeight, 
                                          bShowVideoOnPages);
	   }
      else
      {
         hr = CalculateSpritePositionsNew(m_rcMovie.Width(), m_rcMovie.Height(), 
                                          m_iAccVideoWidth, m_iAccVideoHeight,
                                          m_iWbInputWidth, m_iWbInputHeight,   
                                          m_iLogoWidth, m_iLogoHeight,
                                          MINIMUM_CONTROLBAR_WIDTH, DEFAULT_CONTROLBAR_HEIGHT,
                                          MINIMUM_CONTROLBAR_WIDTH, FLASH_METADATAHEIGHT, 
                                          m_iWbOutputWidth, m_iWbOutputHeight, 
                                          bShowVideoOnPages);

         // Calculate the used movie dimension, which may differ from the given movie size
         CRect rcSprites;
         if (SUCCEEDED(hr))
            hr = GetSpritesDimension(m_bHasMetadata, m_bHasControlBar, m_bHasPages, rcSprites);

         m_fMovieOffsetX = (0.5 * (m_rcMovie.Width() - rcSprites.Width()));
         m_fMovieOffsetY = (0.5 * (m_rcMovie.Height() - rcSprites.Height()));
      }
   }

   if (SUCCEEDED(hr) && (!m_bIsPspDesign || m_rcMovie.IsRectEmpty()) && m_bIsSlidestar)
   {
      hr = CalculateMovieDimension(m_bHasMetadata, m_bHasControlBar, m_bHasPages);
   }
  

   int nMsecPerFrame = 0;
   int nLength = 0;
   if (SUCCEEDED(hr))
   {
      nLength = m_iMovieLengthMs;
      CalculateFrameRate(nLength, nMsecPerFrame);
   }

   // Add Fonts 
   // - Standard font (for time display etc) 
   // - "Logo" font (for LECTURNITY webplayer)
   m_pFntStd = NULL; 
   m_pFntLogo = NULL;
   if (SUCCEEDED(hr))
   {
      hr = SearchForFonts();
   }

   //
   // Create a Preloader movie (not for new Flex LEC Player)
   //
   if (SUCCEEDED(hr) && !m_bIsNewLecPlayer)
      hr = CreatePreloaderSwf(refBackColor);

   //
   // New Movie
   //
   SWFMovie *movie = NULL;
   if (SUCCEEDED(hr))
   {
      movie = new SWFMovie();
 
      // Now set the dimension, calculated a few steps before
      movie->setDimension(m_rcMovie.Width(), m_rcMovie.Height());

      // Set the frame rate
      movie->setRate(m_fFrameRate);

      // Set background color
	   if (m_bIsSlidestar)
		   movie->setBackground(0,0,0);
	   else
         movie->setBackground((refBackColor >> 16) & 0xff, (refBackColor >> 8) & 0xff, (refBackColor >> 0) & 0xff);
   }
   
   // A (General) Message Box
   if (SUCCEEDED(hr))
   {
      hr = DrawMessageBox(movie);
   }

   if(!m_bIsSlidestar)
   {
	   if (SUCCEEDED(hr))
	   {
		   hr = DrawPlayerBackground(movie);
	   }

	   // Logo image
	   if (SUCCEEDED(hr) && bShowLogoImage)
	   {
		   if (!m_bIsPspDesign)
   	      hr = DrawLogoImage(movie, pBmpLogo, (float)FLASH_PADDING, (float)FLASH_PADDING, tszLogoUrl);
	   }

	   // Metadata text
	   if (SUCCEEDED(hr) && m_bHasMetadata)
	   {
		   if (!m_bIsPspDesign)
			   hr = DrawMetadataText(movie, refTextColor, m_pFntStd);
	   }
   }

   if (SUCCEEDED(hr))
   {
      hr = CreateTooltipField(movie, tszLogoUrl);
   }

   // Add common ActionScript for the movie
   if (SUCCEEDED(hr))
   {
      hr = AddActionScriptVariables(movie);
   }

   if (SUCCEEDED(hr))
   {
      hr = AddActionScriptForLoading(movie);
   }

   /*if(SUCCEEDED(hr) && m_bIsSlidestar)
   {
	   hr = AddActionScriptForSlidestar(movie, m_bHasPages, m_bHasVideo, m_bHasClips);
   }*/

   if (SUCCEEDED(hr))
   {
      hr = AddActionScriptInitFunctions(movie, m_iFlashVersion, m_bHasPages, m_bHasVideo, m_bHasClips);
   }

   if (SUCCEEDED(hr))
   {
      hr = AddActionScriptCommonFunctions(movie);
   }

   if(SUCCEEDED(hr) && m_bIsSlidestar)
   {
      hr = AddActionScriptForSlidestar(movie);
   }

   if (SUCCEEDED(hr))
   {
      hr = AddActionScriptForMessageBoxToMovie(movie);
   }

   // Paging
   int nNumOfPages = 0;
   if (SUCCEEDED(hr) && m_bHasPages)
   {
      hr = GetPageTimestamps(nLength);

      if (hr > 0) {
          // Warning flag is returned
          if ((m_hrPossibleWarning & imc_lecturnity_converter_LPLibs_WARN_PAGE_TOO_SHORT) 
            != imc_lecturnity_converter_LPLibs_WARN_PAGE_TOO_SHORT) {
              m_hrPossibleWarning += imc_lecturnity_converter_LPLibs_WARN_PAGE_TOO_SHORT;
          }
      }

      // Add ActionScript with paging info
      if (SUCCEEDED(hr))
      {
         hr = AddActionScriptForPagingToMovie(movie);
         nNumOfPages = m_aiPagingFrames.GetSize();
      }
   }

   // Create the Control Bar
   if (SUCCEEDED(hr))
   {
      if (m_bIsPspDesign)
	  {
         hr = CreateControlBarPsp(movie, m_pFntLogo);
	  }
	  else if (m_bIsSlidestar)
	  {
		  /*hr = CheckForSlidestarControlBarFlash();
		  if (SUCCEEDED(hr))
		 	 hr = CreateSlidestarControlBar(movie);*/
	  }
      else
	  {
         hr = CreateControlBar(movie, m_pFntLogo);
	  }
   }

   // Create the border around the slides & control bar
   if (SUCCEEDED(hr))
   {
      if (!m_bIsPspDesign && !m_bIsSlidestar)
         DrawSlidesBorder(movie, m_bHasMetadata);
   }

   // Add Sound
   // Can be removed if audio is part of video
   if (SUCCEEDED(hr) && m_bHasMP3Audio)
   {
      AddAudioToMovie(movie);
   }  

   // Add ActionScript for video & clips
   if (SUCCEEDED(hr) && (m_bHasVideo || m_bHasClips))
   {
      hr = AddNetConnectionToMovie(movie);
   }

   // Add Video (if any)
   if (SUCCEEDED(hr) && m_bHasVideo)
   {
      AddVideoToMovie(movie);
   }

   // Add Clips (if any)
   if (SUCCEEDED(hr) && m_bHasClips)
   {
      hr = AddClipsToMovie(movie);
   }

   // Add Whiteboard objects
   if (SUCCEEDED(hr))
   {
      if (m_bHasPages)
      {
         hr = AddWbObjectsToMovie(movie, nMsecPerFrame);
      }
      else  // Add frames to movie, necessary for scrollbar
      {
         m_iAbsoluteFramesCount = (m_iMovieLengthMs * m_fFrameRate) / 1000;
         for (int i = 0; i < m_iAbsoluteFramesCount; ++i)
            movie->nextFrame();
      }
   }

   // Now save (write) the SWF file
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      FILE *file = _tfopen(m_tszSwfFile, _T("wb"));
      int nSwfFileSize = movie->save(file);
      _tprintf(_T("> nSwfFileSize: %d \n"), nSwfFileSize);
      fclose(file);

      // Bugfix (5301):
      // in case of 'malloc' tries to allocate too much memory 
      // a SWF file size of '0' is created 
      // and an "Error No Memory" (ENOMEM=12) or "Error Bad File" (EBADF=9) is returned
      if (nSwfFileSize <= 0) {
          //_tprintf(_T("> errno: %d \n"), errno);
          //CString csZeroSwfSize;
          //csZeroSwfSize.Format(_T("SWF file size: %d\nerrno: %d\n"), nSwfFileSize, errno);
          //::MessageBox(NULL, csZeroSwfSize, _T("Error"), MB_OK);

          // '-12' as an error value for ENOMEM is defined in LPMing/src/movie.c
          if ( (errno == ENOMEM) || (nSwfFileSize == -12) ) {
              hr = imc_lecturnity_converter_LPLibs_NO_MEMORY;
          } else {
              hr = imc_lecturnity_converter_LPLibs_NO_SWF_CREATED;
          }
      }

      _tprintf(_T("- CMap Fonts:  %d objects:\n"), m_mapSwfFonts.GetCount());
      CString csFontName;
      SWFFont *pSwfFont = NULL;
      POSITION pos = m_mapSwfFonts.GetStartPosition();
      while(pos != NULL) {
          m_mapSwfFonts.GetNextAssoc(pos, csFontName, pSwfFont);
          _tprintf(_T(" - %s 0x%08x %d\n"), csFontName, pSwfFont, pSwfFont->getNumOfUndefinedGlyphs());
          // Bugfix 5614: Warning message in case of undefined glyph in a font
          if (pSwfFont->getNumOfUndefinedGlyphs()) {
              // Add font name to font list
              m_mapFontNamesGlyphNotFound.SetAt(csFontName, NULL);

              if ((m_hrPossibleWarning & imc_lecturnity_converter_LPLibs_WARN_GLYPH_NOT_FOUND)
                != imc_lecturnity_converter_LPLibs_WARN_GLYPH_NOT_FOUND) {
                  m_hrPossibleWarning += imc_lecturnity_converter_LPLibs_WARN_GLYPH_NOT_FOUND;
              }
          }
      }
   }

   _tprintf(_T("Build movie ... Done(hr=%d)\n"), hr);
   // Cleanup() should be called by the calling thread (java) lateron

   if (SUCCEEDED(hr) && m_hrPossibleWarning != S_OK)
      return m_hrPossibleWarning;
   else
      return hr;
}

UINT CFlashEngine::CreateSlidestar(CString csThumbFiles)
{
   //create document.xml
   CString csMetadata;
   csMetadata.SetString(m_tszSwfFile);
   int nNamePos = csMetadata.ReverseFind(_T('\\'));
   CString csFilePath = csMetadata.Left(nNamePos + 1);
   CString csDocXml = csFilePath + _T("document.xml");
   CString csContent;
   csContent.Append(_T("<fileinfo>\n"));
   if (m_bHasVideo)
   {
      CString csVideo;
      csVideo.SetString(m_tszVideoFlvFile);
      int namepos = csVideo.ReverseFind(_T('\\'));
      csVideo = csVideo.Right(csVideo.GetLength() - namepos - 1);
      csContent.Append(_T("  <video>"));
      csContent.Append(csVideo);
      csContent.Append(_T("</video>\n"));
   }
   if (m_bHasClips)
   {
      CString csClip;
      for (int i = 0; i < m_iNumOfClips; i++)
      {
         csClip.SetString(m_atszClipFlvFiles[i]);
         int namepos = csClip.ReverseFind(_T('\\'));
         csClip = csClip.Right(csClip.GetLength() - namepos - 1);
         csContent.Append(_T("  <clip>"));
         csContent.Append(csClip);
         csContent.Append(_T("</clip>\n"));
      }
   }
   csContent.Append(_T("  <controlbar>\n"));
   csContent.Append(_T("    <controlBarEnabled>"));
   csContent.Append((m_iNavigationControlBar == 2) ? _T("false") : _T("true"));
   csContent.Append(_T("</controlBarEnabled>\n"));

   /*csContent.Append(_T("    <pageBasedControlEnabled>"));
   csContent.Append((m_iNavigationDocumentStructure == 2) ? _T("false") : _T("true"));
   csContent.Append(_T("</pageBasedControlEnabled>\n"));
   csContent.Append(_T("    <timeBasedControlEnabled>"));
   csContent.Append((m_iNavigationControlBar == 2) ? _T("false") : _T("true"));
   csContent.Append(_T("</timeBasedControlEnabled>\n"));*/
   csContent.Append(_T("  </controlbar>\n"));
   csContent.Append(_T("</fileinfo>"));
   CStdioFile doc(csDocXml,CFile::modeCreate | CFile::modeWrite | CFile::typeText );
   doc.WriteString(csContent);
   doc.Close();

   //create .lzp file
   CZipArchive zip;
   /*CScormZipCallback zipCallback(this);
   zip.SetCallback(&zipCallback);*/
   CArray<_TCHAR*, _TCHAR*> aZipedFiles;
   CString csSlidestar;
   csSlidestar.SetString(m_tszSwfFile);
   int namepos = csSlidestar.ReverseFind(_T('\\'));
   CString csPath = csSlidestar.Left(namepos + 1);
   //CString csSwfInZip = csSlidestar.Right(csSlidestar.GetLength() - namepos - 1);
   CString csSwfInZip = _T("document.swf");
   csSlidestar = csSlidestar.Left(csSlidestar.GetLength() - 4);
   csSlidestar += _T(".lzp");
   zip.Open(csSlidestar, CZipArchive::zipCreate);
   zip.AddNewFile(m_tszSwfFile, csSwfInZip);
   aZipedFiles.Add(m_tszSwfFile);
   CString csSwfPreloaderInZip = m_csPreloaderFilename;
   zip.AddNewFile(m_tszPreloaderSwfFile, csSwfPreloaderInZip);
   aZipedFiles.Add(m_tszPreloaderSwfFile);
   if(m_tszVideoFlvFile != NULL)
   {
      //CString csVideoFlv;
      //csVideoFlv.SetString(m_tszVideoFlvFile);
      //int nameVpos = csVideoFlv.ReverseFind(_T('\\'));
      //CString csVflvInZip = csVideoFlv.Right(csVideoFlv.GetLength() - nameVpos - 1);
	   CString csVflvInZip = _T("document.flv");
      zip.AddNewFile(m_tszVideoFlvFile, csVflvInZip);
      aZipedFiles.Add(m_tszVideoFlvFile);
   }
   for(int i = 0 ; i < m_iNumOfClips; i++)
   {
      CString csClipName;
      csClipName.SetString(m_atszClipFlvFiles[i]);
      int nameCpos = csClipName.ReverseFind(_T('\\'));
      CString csClipInZip;// = csClipName.Right(csClipName.GetLength() - nameCpos - 1);
	  csClipInZip.Format(_T("clip_%d.flv"), i);
      zip.AddNewFile(m_atszClipFlvFiles[i], csClipInZip);
      aZipedFiles.Add(m_atszClipFlvFiles[i]);
   }
   if(m_csSlidestarAddFiles != _T(""))
   {
      CString csEvq = m_csSlidestarAddFiles;
      CString csObj = m_csSlidestarAddFiles.Left(m_csSlidestarAddFiles.GetLength() - 3) + _T("obj");

      CString csEvqInZip = _T("document.evq");
      CString csObjInZip = _T("document.obj");
      zip.AddNewFile(csEvq, csEvqInZip);
      zip.AddNewFile(csObj, csObjInZip);
   }
   // Bugfix 5388: xml and lmd file are always created (also for "Denver")
   CString csXml = csPath + _T("document.xml");
   CString csXmlInZip = _T("document.xml");
   zip.AddNewFile(csXml, csXmlInZip);
   CFile::Remove(csXml);
   CString csLmd;
   // Get the LMD file name from the LAD file name
   csLmd.SetString(m_tszLadFile);
   csLmd.Truncate(csLmd.GetLength() - 4);
   csLmd += _T(".lmd");

   CString csNewLmd = csFilePath + _T("doc_mod.lmd");
   HRESULT hr = ConvertLmdFile(csLmd, csNewLmd);
   _tprintf(_T("Converting %s --> %s (%s)\n"), csLmd, csNewLmd, (hr==1 ? _T("ok") : _T("failed")));
   if (hr != S_OK) // Conversion failure; TODO: New error code?
      return imc_lecturnity_converter_LPLibs_OPEN_FILE_ERROR;

   CString csLmdInZip = _T("document.lmd");
   zip.AddNewFile(csNewLmd, csLmdInZip);
   CFile::Remove(csNewLmd);

   if(csThumbFiles != _T(""))
   {
      int nCount = 0;
      CString resToken = csThumbFiles.Tokenize(_T("<>"), nCount);
      while(resToken != _T(""))
      {
         CString csThumbName = resToken;
         CString csThumbPath = csPath + csThumbName;
         zip.AddNewFile(csThumbPath, csThumbName);
         aZipedFiles.Add(csThumbPath.AllocSysString());
         resToken = csThumbFiles.Tokenize(_T("<>"), nCount);
      }
   }

   zip.Close();

   CFile temp;
   CFileException e;
   //CString csResH = csFilePath + _T("resource.h");
   for(int i = 0; i < aZipedFiles.GetSize(); i++)
   {
      if(temp.Open(aZipedFiles.GetAt(i), CFile::modeRead, &e))
      {
         temp.Close();
         try
         {
            CFile::Remove(aZipedFiles.GetAt(i));//Rename( csFileName, csNewName );
         }
         catch(CFileException* pEx )
         {
            pEx->Delete();
            return S_FALSE;
         }
      }
   }

   return S_OK;
}

UINT CFlashEngine::CreateFlexFiles(const _TCHAR *tszBaseUrl, 
                                   const _TCHAR *tszStreamUrl, 
                                   bool bShowVideoOnPages, 
                                   bool bShowClipsInVideoArea, 
                                   bool bStripFlvSuffix, 
                                   bool bDoAutostart, 
                                   bool bScormEnabled, 
                                   bool bUseOriginalSlideSize)
{
   // Determine target path and document name
   CString csTargetPath;
   csTargetPath.Format(_T("%s"), m_tszSwfFile);
   StringManipulation::GetPath(csTargetPath);
   csTargetPath += _T("\\");
   CString csDocName;
   csDocName.Format(_T("%s"), m_tszSwfFile);
   StringManipulation::GetFilePrefix(csDocName);

   // A flag for structured documents which should treat the clip as video 
   // and show the clip in the page area
   bool bShowStructuredClipOnPageArea = bShowVideoOnPages;
   
   // Calculate the dimension of the New Flash Player 4
   CalculateFlashPlayerDimension();

   // Create document.xml
   CString csDocXml = csTargetPath + _T("document.xml");
   CFileOutput *pDoc;
   HANDLE hXmlFile = CreateFile(csDocXml, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hXmlFile == INVALID_HANDLE_VALUE)
   {
      TRACE("ERROR WRITING EMPTY XML FILE: %ls\n", csDocXml);
      return imc_lecturnity_converter_LPLibs_OPEN_FILE_ERROR;
   } 
   else 
   {
      pDoc = new CFileOutput;
      pDoc->Init(hXmlFile, 0, true);
   }

   CString csContent;
   // Begin <fileinfo> tag
   csContent.Append(_T("<fileinfo>\n"));
   // Flash Player dimension
   CString csFlashPlayerWidth;
   CString csFlashPlayerHeight;
   csFlashPlayerWidth.Format(_T("%d"), m_rcFlashPlayer.Width());
   csFlashPlayerHeight.Format(_T("%d"), m_rcFlashPlayer.Height());
   csContent.Append(_T("  <flashPlayer>\n"));
   csContent.Append(_T("    <width>"));
   csContent.Append(csFlashPlayerWidth);
   csContent.Append(_T("</width>\n"));
   csContent.Append(_T("    <height>"));
   csContent.Append(csFlashPlayerHeight);
   csContent.Append(_T("</height>\n"));
   csContent.Append(_T("  </flashPlayer>\n"));
   pDoc->Append(csContent);
   csContent.Empty();
   // SWF file
   CString csSwfFile;
   CString csSwfWidth;
   CString csSwfHeight;
   csSwfFile.Format(_T("%s.swf"), csDocName);
   csSwfWidth.Format(_T("%d"), m_rcMovie.Width());
   csSwfHeight.Format(_T("%d"), m_rcMovie.Height());
   csContent.Append(_T("  <swf>\n"));
   csContent.Append(_T("    <file>"));
   csContent.Append(csSwfFile);
   csContent.Append(_T("</file>\n"));
   csContent.Append(_T("    <width>"));
   csContent.Append(csSwfWidth);
   csContent.Append(_T("</width>\n"));
   csContent.Append(_T("    <height>"));
   csContent.Append(csSwfHeight);
   csContent.Append(_T("</height>\n"));
   csContent.Append(_T("  </swf>\n"));
   pDoc->Append(csContent);
   csContent.Empty();
   // Audio from FLV?
   if (m_bHasFlvAudio)
   {
      CString csAudioFile;
      csAudioFile.Format(_T("%s.flv"), csDocName);
      csContent.Append(_T("  <audio>\n"));
      csContent.Append(_T("    <file>"));
      csContent.Append(csAudioFile);
      csContent.Append(_T("</file>\n"));
      csContent.Append(_T("  </audio>\n"));
   }
   pDoc->Append(csContent);
   csContent.Empty();
   // StillImage?
   if (m_tszStillImageFile != NULL && _tcslen(m_tszStillImageFile) > 0)
   {
      CString csStillImageFile;
      CString csStillImageWidth;
      CString csStillImageHeight;
      csStillImageFile.SetString(m_tszStillImageFile);
      StringManipulation::GetFilename(csStillImageFile);
      csStillImageWidth.Format(_T("%d"), m_iStillImageWidth);
      csStillImageHeight.Format(_T("%d"), m_iStillImageHeight);
      csContent.Append(_T("  <stillImage>\n"));
      csContent.Append(_T("    <file>"));
      csContent.Append(csStillImageFile);
      csContent.Append(_T("</file>\n"));
      csContent.Append(_T("    <width>"));
      csContent.Append(csStillImageWidth);
      csContent.Append(_T("</width>\n"));
      csContent.Append(_T("    <height>"));
      csContent.Append(csStillImageHeight);
      csContent.Append(_T("</height>\n"));
      csContent.Append(_T("  </stillImage>\n"));
   }
   // Video?
   if ((m_iAccVideoWidth > 0L) && (m_iAccVideoHeight > 0L))
   {
      CString csVideoFile;
      CString csVideoWidth;
      CString csVideoHeight;
      CString csVideoLength;
      CString csShowOnPages;
      csVideoFile.Format(_T("%s.flv"), csDocName);
      csVideoWidth.Format(_T("%d"), m_iAccVideoWidth);
      csVideoHeight.Format(_T("%d"), m_iAccVideoHeight);
      csVideoLength.Format(_T("%d"), m_iVideoLengthMs);
      csShowOnPages.SetString(bShowStructuredClipOnPageArea ? _T("true") : _T("false"));
      csContent.Append(_T("  <video>\n"));
      csContent.Append(_T("    <file>"));
      csContent.Append(csVideoFile);
      csContent.Append(_T("</file>\n"));
      csContent.Append(_T("    <width>"));
      csContent.Append(csVideoWidth);
      csContent.Append(_T("</width>\n"));
      csContent.Append(_T("    <height>"));
      csContent.Append(csVideoHeight);
      csContent.Append(_T("</height>\n"));
      csContent.Append(_T("    <length>"));
      csContent.Append(csVideoLength);
      csContent.Append(_T("</length>\n"));
      csContent.Append(_T("    <showOnPages>"));
      csContent.Append(csShowOnPages);
      csContent.Append(_T("</showOnPages>\n"));
      csContent.Append(_T("  </video>\n"));
   }
   pDoc->Append(csContent);
   csContent.Empty();
   // Clips?
   if (m_iNumOfClips > 0)
   {
      CString csClipFile;
      CString csClipWidth;
      CString csClipHeight;
      CString csClipBegin;
      CString csClipEnd;
      for (int i = 0; i < m_iNumOfClips; i++)
      {
         csClipFile.Format(_T("%s_clip%d.flv"), csDocName, i);
         csClipWidth.Format(_T("%d"), m_aSizeClips[i].cx);
         csClipHeight.Format(_T("%d"), m_aSizeClips[i].cy);
         csClipBegin.Format(_T("%d"), m_alClipsBeginMs[i]);
         csClipEnd.Format(_T("%d"), m_alClipsEndMs[i]);
         csContent.Append(_T("  <clip>\n"));
         csContent.Append(_T("    <file>"));
         csContent.Append(csClipFile);
         csContent.Append(_T("</file>\n"));
         csContent.Append(_T("    <width>"));
         csContent.Append(csClipWidth);
         csContent.Append(_T("</width>\n"));
         csContent.Append(_T("    <height>"));
         csContent.Append(csClipHeight);
         csContent.Append(_T("</height>\n"));
         csContent.Append(_T("    <begin>"));
         csContent.Append(csClipBegin);
         csContent.Append(_T("</begin>\n"));
         csContent.Append(_T("    <end>"));
         csContent.Append(csClipEnd);
         csContent.Append(_T("</end>\n"));
         csContent.Append(_T("  </clip>\n"));
      }
   }
   pDoc->Append(csContent);
   csContent.Empty();
   // Show clips in video area?
   CString csShowClipsInVideoArea;
   csShowClipsInVideoArea.SetString(bShowClipsInVideoArea ? _T("true") : _T("false"));
   csContent.Append(_T("  <showClipsInVideoArea>"));
   csContent.Append(csShowClipsInVideoArea);
   csContent.Append(_T("</showClipsInVideoArea>\n"));
   pDoc->Append(csContent);
   csContent.Empty();
   // Base URL
   CString csBaseUrl;
   if (tszBaseUrl != NULL)
      csBaseUrl.Format(_T("%s"), tszBaseUrl);
   else
      csBaseUrl.SetString(_T(""));
   csContent.Append(_T("  <baseUrl>"));
   csContent.Append(csBaseUrl);
   csContent.Append(_T("</baseUrl>\n"));
   // Stream URL
   CString csStreamUrl;
   if (tszStreamUrl != NULL)
      csStreamUrl.Format(_T("%s"), tszStreamUrl);
   else
      csStreamUrl.SetString(_T(""));
   csContent.Append(_T("  <streamUrl>"));
   csContent.Append(csStreamUrl);
   csContent.Append(_T("</streamUrl>\n"));
   // Stream: Strip Flv Suffix (default: true)
   CString csStripFlvSuffix;
   csStripFlvSuffix.SetString(bStripFlvSuffix ? _T("true") : _T("false"));
   csContent.Append(_T("  <streamStripFlvSuffix>"));
   csContent.Append(csStripFlvSuffix);
   csContent.Append(_T("</streamStripFlvSuffix>\n"));
   // Autostart?
   CString csAutostart;
   csAutostart.SetString(bDoAutostart ? _T("true") : _T("false"));
   csContent.Append(_T("  <autostart>"));
   csContent.Append(csAutostart);
   csContent.Append(_T("</autostart>\n"));
   pDoc->Append(csContent);
   csContent.Empty();
   // Use original slide size?
   CString csUseSlideSize;
   csUseSlideSize.SetString(bUseOriginalSlideSize ? _T("100%") : _T("flexible"));
   csContent.Append(_T("  <useSlideSize>"));
   csContent.Append(csUseSlideSize);
   csContent.Append(_T("</useSlideSize>\n"));
   pDoc->Append(csContent);
   csContent.Empty();

   // States of Controlbar and Document Structure
   csContent.Append(_T("  <controlbar>\n"));
//   // Controlbar enabled (only for SLIDESTAR compatibility)
//   csContent.Append(_T("    <controlBarEnabled>"));
//   csContent.Append((m_iNavigationControlBar == 2) ? _T("false") : _T("true"));
//   csContent.Append(_T("</controlBarEnabled>\n"));
   // Controlbar
   csContent.Append(_T("    <controlbarState>"));
   csContent.Append(GetNavigationStateFromIndex(m_iNavigationControlBar));
   csContent.Append(_T("</controlbarState>\n"));
   // Standard buttons
   csContent.Append(_T("    <standardButtonState>"));
   csContent.Append(GetNavigationStateFromIndex(m_iNavigationStandardButtons));
   csContent.Append(_T("</standardButtonState>\n"));
   // Time line
   csContent.Append(_T("    <timeLineState>"));
   csContent.Append(GetNavigationStateFromIndex(m_iNavigationTimeLine));
   csContent.Append(_T("</timeLineState>\n"));
   // Time display
   csContent.Append(_T("    <timeDisplayState>"));
   csContent.Append(GetNavigationStateFromIndex(m_iNavigationTimeDisplay));
   csContent.Append(_T("</timeDisplayState>\n"));
   csContent.Append(_T("  </controlbar>\n"));
   // Document Structure
   csContent.Append(_T("  <documentStructureState>"));
   int iNavigationDocumentStructure = m_iNavigationDocumentStructure;
   // Pure SG ("Denver") Documents have no Document Structure
   if (m_docType == DOCUMENT_TYPE_DENVER)
      iNavigationDocumentStructure = 2; // hidden
   csContent.Append(GetNavigationStateFromIndex(iNavigationDocumentStructure));
   csContent.Append(_T("</documentStructureState>\n"));
   // Plugin Context Menu
//   // This is obsolete for Flash Player 9 and higher
//   csContent.Append(_T("  <pluginContextMenuState>"));
//   csContent.Append(GetNavigationStateFromIndex(m_iNavigationPluginContextMenu));
//   csContent.Append(_T("</pluginContextMenuState>\n"));

   // SCORM
   CString csScormEnabled;
   csScormEnabled.SetString(bScormEnabled ? _T("true") : _T("false"));
   csContent.Append(_T("  <scormEnabled>"));
   csContent.Append(csScormEnabled);
   csContent.Append(_T("</scormEnabled>\n"));
   pDoc->Append(csContent);
   csContent.Empty();

   // Frames and Framerate
   CString csFrames;
   CString csFramerate;
   csFrames.Format(_T("%d"), m_iAbsoluteFramesCount);
   csFramerate.Format(_T("%g"), m_fFrameRate);
   csContent.Append(_T("  <frames>"));
   csContent.Append(csFrames);
   csContent.Append(_T("</frames>\n"));
   csContent.Append(_T("  <framerate>"));
   csContent.Append(csFramerate);
   csContent.Append(_T("</framerate>\n"));
   pDoc->Append(csContent);
   csContent.Empty();

   // Version, Language, Document Type, "LEC Identity"
   _TCHAR sVersion[256];
   _TCHAR sLanguageCode[256];
   KerberokHandler *kh = new KerberokHandler();
   kh->GetVersionStringShortShort(sVersion);
   kh->GetLanguageCode(sLanguageCode);
   CString csVersion;
   CString csLanguageCode;
   CString csDocumentType;
   CString csLecIdentityHidden;
   csVersion.Format(_T("%s"), sVersion);
   csLanguageCode.Format(_T("%s"), sLanguageCode);
   csDocumentType.SetString(m_bIsSimulationDocument ? _T("simulation") : _T("recording"));
   csLecIdentityHidden.SetString(m_bLecIdentityHidden ? _T("true") : _T("false"));
   // Version
   csContent.Append(_T("  <version>"));
   csContent.Append(csVersion);
   csContent.Append(_T("</version>\n"));
   // Language
   csContent.Append(_T("  <language>"));
   csContent.Append(csLanguageCode);
   csContent.Append(_T("</language>\n"));
   // Document Type ("recording" or "simulation");
   csContent.Append(_T("  <type>"));
   csContent.Append(csDocumentType);
   csContent.Append(_T("</type>\n"));
   // "LEC Identity" (LEC Web Player logo, etc.)
   csContent.Append(_T("  <lecIdentityHidden>"));
   csContent.Append(csLecIdentityHidden);
   csContent.Append(_T("</lecIdentityHidden>\n"));
   csContent.Append(_T("\n"));
   pDoc->Append(csContent);
   csContent.Empty();

   // Fulltext
   csContent.Append(_T("  <fulltext>\n"));
   pDoc->Append(csContent);
   csContent.Empty();
   WriteFullTextList(pDoc);
   csContent.Append(_T("  </fulltext>\n"));

   // End <fileinfo> tag
   csContent.Append(_T("\n"));
   csContent.Append(_T("</fileinfo>\n"));
   pDoc->Append(csContent);
   csContent.Empty();
   pDoc->Flush();

   // close file handles
   CloseHandle(hXmlFile);
   delete pDoc;
   pDoc = NULL;


   // Create document.cfg
   CString csDocCfg = csTargetPath + _T("document.cfg");
   hXmlFile = CreateFile(csDocCfg, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hXmlFile == INVALID_HANDLE_VALUE)
   {
      TRACE("ERROR WRITING EMPTY CFG FILE: %ls\n", csDocCfg);
      return imc_lecturnity_converter_LPLibs_OPEN_FILE_ERROR;
   } 
   else 
   {
      pDoc = new CFileOutput;
      pDoc->Init(hXmlFile, 0, true);
   }

   CString csHtmlFile;
   csHtmlFile.Format(_T("%s.html"), csDocName);
   csContent.Append(csHtmlFile);
   pDoc->Append(csContent);
   csContent.Empty();
   pDoc->Flush();

   // close file handles
   CloseHandle(hXmlFile);
   delete pDoc;
   pDoc = NULL;


   // Copy the LMD file to the target directory
   CString csSourceLmd;
   // Get the LMD file name from the LAD file name
   csSourceLmd.SetString(m_tszLadFile);
   csSourceLmd.Truncate(csSourceLmd.GetLength() - 4);
   csSourceLmd += _T(".lmd");
   CString csTargetLmd = csTargetPath + _T("document.lmd");
   HRESULT hr = ConvertLmdFile(csSourceLmd, csTargetLmd);
   _tprintf(_T("Converting %s --> %s (%s)\n"), csSourceLmd, csTargetLmd, (hr==1 ? _T("ok") : _T("failed")));
   if (hr != S_OK) // Conversion failure
      return imc_lecturnity_converter_LPLibs_OPEN_FILE_ERROR;

   return S_OK;
}

UINT CFlashEngine::ConvertLmdFile(const _TCHAR *tszPathFileLmdIn, const _TCHAR *tszPathFileLmdOut) {
    // Check for special characters in titles (Bugfix 5532)
    HRESULT hr = S_OK;

    // Replace strings for special characters (Bugfix 5532)
    const char* szQuot = "&quot;";
    const char* szApos = "&apos;";
    const char* szAmp  = "&amp;";
    const char* szLt   = "&lt;";
    const char* szGt   = "&gt;";
    const char  cCast  = '\\';

    const char* szCdataPre  = "<![CDATA[";
    const char* szCdataPost = "]]>";
    const char* szTitlePre  = "<title>";
    const char* szTitlePost = "</title>";
    const char* szAuthorPre  = "<author>";
    const char* szAuthorPost = "</author>";
    const char* szCreatorPre  = "<creator>";
    const char* szCreatorPost = "</creator>";

    bool bSlidestarOnly = m_bIsSlidestar && !m_bIsNewLecPlayer;
    
    // Read the original file
    CFile fOrig;
    if (fOrig.Open(tszPathFileLmdIn, CFile::modeRead) == 0)
        return E_FAIL;

    int nFsize = fOrig.GetLength();
    int nFCount = nFsize;
    fOrig.SeekToBegin();

    char *fContent = new char[nFCount];
    
    fOrig.Read(fContent, nFCount);

    fOrig.Close();


    // Write modified file
    CFile modFile;
    if (modFile.Open(tszPathFileLmdOut, CFile::modeCreate | CFile::modeWrite) == 0)
        return E_FAIL;
    
    bool bLastCharIsCastingBackslash = false;

    for (int i = 0; i < nFCount; i++) {
        char aa = fContent[i];

        // Special case: SLIDESTAR upload
        if (bSlidestarOnly) {
            // Cast content of <title>/<author>/<creator> tag 
            // by using '<![CDATA[content]]>'
            if (aa == '<') {
                char *subContent = fContent + i; 
                // <title> or </title> tag?
                if (0 == strncmp(subContent, szTitlePre, 7)) {
                    modFile.Write(szTitlePre, 7);
                    modFile.Write(szCdataPre, 9);
                    i += 6;
                    continue;
                }
                if (0 == strncmp(subContent, szTitlePost, 8)) {
                    modFile.Write(szCdataPost, 3);
                    modFile.Write(szTitlePost, 8);
                    i += 7;
                    continue;
                }
                // <author> or </author> tag?
                if (0 == strncmp(subContent, szAuthorPre, 8)) {
                    modFile.Write(szAuthorPre, 8);
                    modFile.Write(szCdataPre, 9);
                    i += 7;
                    continue;
                }
                if (0 == strncmp(subContent, szAuthorPost, 9)) {
                    modFile.Write(szCdataPost, 3);
                    modFile.Write(szAuthorPost, 9);
                    i += 8;
                    continue;
                }
                // <creator> or </creator> tag?
                if (0 == strncmp(subContent, szCreatorPre, 9)) {
                    modFile.Write(szCreatorPre, 9);
                    modFile.Write(szCdataPre, 9);
                    i += 8;
                    continue;
                }
                if (0 == strncmp(subContent, szCreatorPost, 10)) {
                    modFile.Write(szCdataPost, 3);
                    modFile.Write(szCreatorPost, 10);
                    i += 9;
                    continue;
                }
            }
        }

        if (aa == '&') {
            // '&' can be - but not must be - casted
            // --> write '&amp;' in case of '\&' and '&'
            if (bSlidestarOnly) // --> write uncasted character
                modFile.Write(&aa, 1);
            else
                modFile.Write(szAmp, 5);
            bLastCharIsCastingBackslash = false;
        } else if (aa == '\\') {
            // Casting backslash found
            // --> write nothing, but keep in mind
            bLastCharIsCastingBackslash = true;
        } else {
            if (bLastCharIsCastingBackslash) {
                // Check for special characters
                if (aa == '"') {
                    // Replace '\"' by '&quot;'
                    modFile.Write(szQuot, 6);
                } else if (aa == '\'') {
                    // Replace '\'' by '&apos;'
                    modFile.Write(szApos, 6);
                } else if (aa == '<') {
                    // Replace '\<' by '&lt;'
                    modFile.Write(szLt, 4);
                } else if (aa == '>') {
                    // Replace '\>' by '&gt;'
                    modFile.Write(szGt, 4);
                } else {
                    // Unknown casted character
                    // --> write the character with a leading casting backslash
                    modFile.Write(&cCast, 1);
                    modFile.Write(&aa, 1);
                }
            } else {
                // Normal case: 
                // no casting backslash before, no special character
                // --> write the character
                modFile.Write(&aa, 1);
            }
            bLastCharIsCastingBackslash = false;
        }

    }
    
    modFile.Close();

    return hr;
}

UINT CFlashEngine::WriteFullTextList(CFileOutput *pDoc)
{
   int i = 0;

   // The FullText hash map is already filled in "EmbedObjects"
   // but it does not include text from interactive objects
   // --> Append text from interactive objects to the hash map

   // Get the interactive DrawObjects
   int nCount = m_aInteractionObjects.GetSize();
   for (i = 0; i < nCount; ++i)
   {
      DrawSdk::DrawObject *pObject = m_aInteractionObjects.ElementAt(i);
      if (pObject->GetType() == DrawSdk::TEXT)
      {
         // Get the visibility begin time from the text object
         CString csVisibility = pObject->GetVisibility();
         // Some text objects (e.g. button text) have no visibility --> ignore them
         if (csVisibility.GetLength() > 0)
         {
            CArray<CPoint, CPoint> aVisibilityTimes;
            StringManipulation::ExtractAllTimePeriodsFromString(csVisibility, &aVisibilityTimes);
            int iTimeMsActiveBegin  = aVisibilityTimes.ElementAt(0).x;
            FLASH_FULLTEXT flashFulltext;
            flashFulltext.nMsTime = iTimeMsActiveBegin;
            flashFulltext.pTextObject = (DrawSdk::Text*)pObject;
            m_aFlashFulltexts.Add(flashFulltext);
         }
      }
   }

   // Now write the text objects and their times into the (document.xml) file
   DrawSdk::CharArray array;
   CString csContent;
   for (i = 0; i < m_aFlashFulltexts.GetSize(); ++i)
   {
      int nTimeBeginMs = m_aFlashFulltexts.ElementAt(i).nMsTime;
      DrawSdk::Text *pTextObject = m_aFlashFulltexts.ElementAt(i).pTextObject;
      pTextObject->Write(&array, false, true, nTimeBeginMs);
      csContent.Empty();
      csContent.Append(array.GetBuffer(), array.GetLength());
      // Write the info to the file
      pDoc->Append(csContent);
      array.Reset();
   }

   return S_OK;
}

CString CFlashEngine::GetNavigationStateFromIndex(int idx)
{
   CString csState;
   switch (idx)
   {
   case 0: 
      csState.SetString(_T("enabled"));
      break;
   case 1: 
      csState.SetString(_T("disabled"));
      break;
   case 2: 
      csState.SetString(_T("hidden"));
      break;
   }

   return csState;
}

UINT CFlashEngine::LookForMetadata(bool &bShowMetadata, CMap<CString, LPCTSTR, CString, LPCTSTR> *pMapMetadata)
{
   HRESULT hr = S_OK;

   CString csKeyA = _T("author");
   CString csKeyT = _T("title");

   bShowMetadata = false;
   if (pMapMetadata && pMapMetadata->GetSize() > 0)
   {
      int iHasAuthor = pMapMetadata->Lookup(csKeyA, m_csAuthor);
      int iHasTitle = pMapMetadata->Lookup(csKeyT, m_csTitle);
      int iAuthorLen = 0;
      int iTitleLen = 0;
      if (0 != iHasAuthor)
         iAuthorLen = m_csAuthor.GetLength();
      if (0 != iHasTitle)
         iTitleLen = m_csTitle.GetLength();
      bShowMetadata = ((iAuthorLen > 0 ) || (iTitleLen > 0)) ? true : false;
   }

   return hr;
}

UINT CFlashEngine::SearchForFonts()
{
   HRESULT hr = S_OK;

   // Use standard  font "ARIAL.fdb" which has been added manually to the 
   // font hash table in "SetInputFiles()" before.
   if (!m_mapSwfFonts.Lookup(_T("ARIAL.TTF"), m_pFntStd))
   {
      CString csFdbOut;
      csFdbOut.Format(_T("%s\\ARIAL.fdb"), GetOutputPath());

      FILE *pFile;
      if (NULL == (pFile = _tfopen(csFdbOut, _T("rb"))))
      {
         hr = imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;

         _TCHAR tszMessage[1024];
         _stprintf(tszMessage, _T("Error: Cannot open font file! %s\n"), csFdbOut);
         _tprintf(tszMessage);
      }

      if (SUCCEEDED(hr))
      {
         m_pFntStd = new SWFFont(pFile);
         m_mapSwfFonts.SetAt(_T("ARIAL.TTF"), m_pFntStd);
         m_mapTempFiles.SetAt(csFdbOut, pFile);
      }
   }

   // If no "ARIAL" can be found, use "Bitstream" instead.
   if (!m_pFntStd)
   {
      CString csFont;
      csFont.Format(_T("%sBitstream Vera Sans.fdb"), GetFlashFdbPath());
      hr = BuildFont(csFont);
      if (SUCCEEDED(hr))
      {
         FILE *pFile = _tfopen(csFont, _T("rb"));
         m_pFntStd = new SWFFont(pFile);
      }
   }

   // 2nd Font used for the "LECTURNITY Webplayer" logo (Frutiger)
   if (SUCCEEDED(hr))
   {
      CString csFont;
      csFont.Format(_T("%sLecturnity.fdb"), GetFlashFdbPath());
      hr = BuildFont(csFont);
      if (SUCCEEDED(hr))
      {
         FILE *pFile = _tfopen(csFont, _T("rb"));
         m_pFntLogo = new SWFFont(pFile);
      }
   }

   return hr;
}

UINT CFlashEngine::CalculateScaleFactor()
{
   HRESULT hr = S_OK;

   double dGlobalScaleX = 1.0;
   double dGlobalScaleY = 1.0;
   if (m_iWbInputWidth > 0 && m_iWbInputHeight > 0)
   {
      dGlobalScaleX = (double)m_iWbOutputWidth / (double)m_iWbInputWidth;
      dGlobalScaleY = (double)m_iWbOutputHeight / (double)m_iWbInputHeight;
   }

   m_dGlobalScale = (dGlobalScaleX < dGlobalScaleY) ? dGlobalScaleX : dGlobalScaleY;
   _tprintf(_T("# CalculateScaleFactor() wbIn: %dx%d, wbOut: %dx%d,  scale: %f\n"), m_iWbInputWidth, m_iWbInputHeight, m_iWbOutputWidth, m_iWbOutputHeight, m_dGlobalScale);

   return hr;
}

UINT CFlashEngine::CalculateMovieDimension(bool bShowMetadata, bool bIsControlbarVisible, bool bHasPages)
{
   HRESULT hr = S_OK;

   m_rcMovie.SetRectEmpty();

   if (m_bIsPspDesign)
   {
      if (m_rcMovie.IsRectEmpty())
      {
         m_rcMovie.left = 0;
         m_rcMovie.top = 0;
         m_rcMovie.right = PSP_MOVIE_WIDTH;
         m_rcMovie.bottom = PSP_MOVIE_HEIGHT;
      }
   }
   else // Normal design
   {
      // the x offset is the left border and the width of the video, if there is one
      if (!m_rcLogo.IsRectEmpty())
         m_rcMovie.UnionRect(m_rcMovie, m_rcLogo);
      if (m_bHasMetadata && !m_rcMetadata.IsRectEmpty())
         m_rcMovie.UnionRect(m_rcMovie, m_rcMetadata);
      if (m_bHasVideo && !m_rcVideo.IsRectEmpty())
         m_rcMovie.UnionRect(m_rcMovie, m_rcVideo);
      if (m_bHasPages && !m_rcPages.IsRectEmpty())
         m_rcMovie.UnionRect(m_rcMovie, m_rcPages);
      if (m_bHasControlBar && !m_rcControlBar.IsRectEmpty())
         m_rcMovie.UnionRect(m_rcMovie, m_rcControlBar);

      // No ControlBar --> add border at the bottom
      if (!m_bHasControlBar && !m_bIsSlidestar)
         m_rcMovie.InflateRect(0, 0, 0, FLASH_BORDERSIZE);

      // Add border/padding
      if (m_bHasMetadata && (!m_bHasVideo || m_docType == DOCUMENT_TYPE_DENVER))
      {
         m_rcMovie.InflateRect(FLASH_PADDING, FLASH_PADDING);
         // border at the right side must be added
         m_rcMovie.InflateRect(0, 0, FLASH_BORDERSIZE, 0);
      }
      else
			m_rcMovie.InflateRect(FLASH_BORDERSIZE, FLASH_BORDERSIZE);

///      // Consider global scale factor
///      m_rcMovie.right *= m_dGlobalScale;
///      m_rcMovie.bottom *= m_dGlobalScale;

   } // end if (!)m_bIsPspDesign
   /*if(m_bIsSlidestar)
   {
	   if(m_bHasVideo && m_bHasPages)
		   m_rcMovie.InflateRect(40,0);
	   if(m_bHasControlBar)
		   m_rcMovie.InflateRect(0,46);
   }*/

   _tprintf(_T("- m_dGlobalScale: %f\n"), m_dGlobalScale);
   _tprintf(_T("- Movie Dimension: %d,%d %dx%d\n"), m_rcMovie.left, m_rcMovie.top, m_rcMovie.right, m_rcMovie.bottom);

   return hr;
}

UINT CFlashEngine::GetSpritesDimension(bool bShowMetadata, bool bIsControlbarVisible, bool bHasPages, CRect &rcSprites)
{
   HRESULT hr = S_OK;

   rcSprites.SetRectEmpty();

   if (m_bIsPspDesign)
   {
      if (rcSprites.IsRectEmpty())
      {
         rcSprites.left = 0;
         rcSprites.top = 0;
         rcSprites.right = PSP_MOVIE_WIDTH;
         rcSprites.bottom = PSP_MOVIE_HEIGHT;
      }
   }
   else // Normal design
   {
      // the x offset is the left border and the width of the video, if there is one
      if (!m_rcLogo.IsRectEmpty())
         rcSprites.UnionRect(rcSprites, m_rcLogo);
      if (m_bHasMetadata && !m_rcMetadata.IsRectEmpty())
         rcSprites.UnionRect(rcSprites, m_rcMetadata);
      if (m_bHasVideo && !m_rcVideo.IsRectEmpty())
         rcSprites.UnionRect(rcSprites, m_rcVideo);
      if (m_bHasPages && !m_rcPages.IsRectEmpty())
         rcSprites.UnionRect(rcSprites, m_rcPages);
      if (m_bHasControlBar && !m_rcControlBar.IsRectEmpty())
         rcSprites.UnionRect(rcSprites, m_rcControlBar);

      // No ControlBar --> add border at the bottom
      if (!m_bHasControlBar)
         rcSprites.InflateRect(0, 0, 0, FLASH_BORDERSIZE);

      // Add border/padding
      if (m_bHasMetadata && (!m_bHasVideo || m_docType == DOCUMENT_TYPE_DENVER))
      {
         rcSprites.InflateRect(FLASH_PADDING, FLASH_PADDING);
         // border at the right side must be added
         rcSprites.InflateRect(0, 0, FLASH_BORDERSIZE, 0);
      }
      else
         rcSprites.InflateRect(FLASH_BORDERSIZE, FLASH_BORDERSIZE);

///      // Consider global scale factor
///      rcSprites.right *= m_dGlobalScale;
///      rcSprites.bottom *= m_dGlobalScale;

   } // end if (!)m_bIsPspDesign

   if(m_bIsSlidestar)
   {
		float fVideoPageSpace = 40;
		float fControlBarSpace = 46;
		      if (!m_rcLogo.IsRectEmpty())
         rcSprites.UnionRect(rcSprites, m_rcLogo);
      if (m_bHasMetadata && !m_rcMetadata.IsRectEmpty())
         rcSprites.UnionRect(rcSprites, m_rcMetadata);
      if (m_bHasVideo && !m_rcVideo.IsRectEmpty())
         rcSprites.UnionRect(rcSprites, m_rcVideo);
      if (m_bHasPages && !m_rcPages.IsRectEmpty())
         rcSprites.UnionRect(rcSprites, m_rcPages);
      if (m_bHasControlBar && !m_rcControlBar.IsRectEmpty())
         rcSprites.UnionRect(rcSprites, m_rcControlBar);

      // No ControlBar --> add border at the bottom
      if (!m_bHasControlBar)
         rcSprites.InflateRect(0, 0, 0, FLASH_BORDERSIZE);

      // Add border/padding
      if (m_bHasMetadata && (!m_bHasVideo || m_docType == DOCUMENT_TYPE_DENVER))
      {
         rcSprites.InflateRect(0,0,FLASH_PADDING, FLASH_PADDING);
         // border at the right side must be added
         rcSprites.InflateRect(0, 0, FLASH_BORDERSIZE, 0);
      }
      else
         rcSprites.InflateRect(0,0,FLASH_BORDERSIZE, FLASH_BORDERSIZE);
	   if(m_bHasVideo && m_bHasPages)
		   rcSprites.InflateRect(0,0,fVideoPageSpace,0);
	   if(m_bHasControlBar)
		   rcSprites.InflateRect(0,0,0,fControlBarSpace);

   }

   return hr;
}


UINT CFlashEngine::CalculateInsets(RECT *rectInsets, float& fControlbarHeight, 
                                   bool bDisplayHeaderText, bool bIsControlbarVisible,
                                   const SIZE *pSizePages, const SIZE *pSizeVideo, const SIZE *pSizeLogo)
{
/*
   //
   // OBSOLETE !!!
   //

   //
   // Define some fix variables
   //
   // The height of the metadata text (equal to the font size used for the text)
   long lMetadataHeight = FLASH_METADATAHEIGHT;

   // Consider the border around the slides
   rectInsets->left   = FLASH_BORDERSIZE;
   rectInsets->top    = FLASH_BORDERSIZE;
   rectInsets->right  = FLASH_BORDERSIZE;
   rectInsets->bottom = 0L; // the bottom border is included in the control bar

   // Consider the metadata (text and/or logo image)
   if ( (bDisplayHeaderText)
      || ((pSizeLogo->cx > 0) && (pSizeLogo->cy > 0)) )
   {
      // Padding is added around all stuff
      rectInsets->left += (int)FLASH_PADDING;
      rectInsets->top += (int)FLASH_PADDING;
      rectInsets->right += (int)FLASH_PADDING;
      rectInsets->bottom += (int)FLASH_PADDING;

      // Metadata text height / logo image height
      // (assuming positive sizes if we have a logo image)
      if (pSizeLogo->cy > lMetadataHeight)
         lMetadataHeight = pSizeLogo->cy;

      rectInsets->top += lMetadataHeight;

      // Add another vertical padding between metadata and the slides area
      rectInsets->top += FLASH_PADDING;
   }

   // Consider the size of an accompanying video (if any)
   // The video is placed bottom left of the slides with padding added around it
   if ((pSizeVideo->cx > 0) && (pSizeVideo->cy > 0))
   {
      // Add padding around the video
      long extdVideoWidth  = pSizeVideo->cx;
      long extdVideoHeight = pSizeVideo->cy;
      if (pSizePages->cx > 0)
         extdVideoWidth += 2*FLASH_PADDING;
      if (pSizePages->cy > 0)
         extdVideoHeight += 2*FLASH_PADDING;

      // Does the video height exceed the whiteboard height?
      if (extdVideoHeight > pSizePages->cy)
      {
         rectInsets->top += (extdVideoHeight - pSizePages->cy);
      }

      rectInsets->left += extdVideoWidth;
   }


   // Consider the control bar
   // Calculate the height of the control bar in the dimension
   // Assumption: default slide size 720x540 px (PowerPoint) 
   // ==> 35 px height for the  control bar ==> ratio 35/540 in height (6.48%)
   fControlbarHeight = DEFAULT_CONTROLBAR_HEIGHT;
   if (pSizePages->cy > 0)
      fControlbarHeight *= ((float)(pSizePages->cy) / DEFAULT_SLIDES_HEIGHT);

   // Consider 'standard navigation'
   if (bIsControlbarVisible)
   {
      // Check, if the minimum control bar width is reached
      float fScaleCtrl = fControlbarHeight / DEFAULT_CONTROLBAR_HEIGHT;
      if (pSizePages->cx > 0)
      {
         if ((fScaleCtrl*MINIMUM_CONTROLBAR_WIDTH) > (float)(pSizePages->cx))
         {
            // Adapt the control bar height
            fControlbarHeight *= (((float)(pSizePages->cx)) / (fScaleCtrl*MINIMUM_CONTROLBAR_WIDTH));
         }
      }
      else if (pSizeVideo->cx > 0)
      {
         if ((fScaleCtrl*MINIMUM_CONTROLBAR_WIDTH) > (float)(pSizeVideo->cx))
         { 
            // Adapt the control bar height
            fControlbarHeight *= (((float)(pSizeVideo->cx)) / (fScaleCtrl*MINIMUM_CONTROLBAR_WIDTH));
         }
      }
      // Add the controlbar height to the inset
      // Note: insets members are long integers, while the controlbar height is calculated as a float
      rectInsets->bottom += (long)(0.5f + fControlbarHeight);
   }
   else
   {
      fControlbarHeight = 0.0f;
      rectInsets->bottom += FLASH_BORDERSIZE;
   }
*/


   HRESULT hr = S_OK;

   rectInsets->left   = 0L;
   rectInsets->top    = 0L;
   rectInsets->right  = 0L;
   rectInsets->bottom = 0L;

   // Set sizes of whiteboard, accompanying video and logo
   m_iWbInputWidth = pSizePages->cx;
   m_iWbInputHeight = pSizePages->cy;
   m_iAccVideoWidth = pSizeVideo->cx;
   m_iAccVideoHeight = pSizeVideo->cy;
   m_iLogoWidth = pSizeLogo->cx;
   m_iLogoHeight = pSizeLogo->cy;

   if ((m_iWbInputWidth > 0) && (m_iWbInputHeight > 0))
      m_bHasPages = true;
   if ((m_iAccVideoWidth > 0) && (m_iAccVideoHeight > 0))
      m_bHasVideo = true;

   bool bHasLogo = ((pSizeLogo->cx > 0) && (pSizeLogo->cy > 0));
   
   m_bHasMetadata = bDisplayHeaderText || bHasLogo;

   m_bHasControlBar = bIsControlbarVisible;

   // Calculate and get the movie size
   UINT iMovieWidth = 0; 
   UINT iMovieHeight = 0;

   hr = GetMovieSize(iMovieWidth, iMovieHeight);

   if (SUCCEEDED(hr))
   {
      // Calculate Insets from the whiteboard rectangle inside the movie rectangle
      if (!m_rcPages.IsRectEmpty())
      {
         rectInsets->left   = m_rcPages.left;
         rectInsets->top    = m_rcPages.top;
         rectInsets->right  = iMovieWidth - m_rcPages.right;
         rectInsets->bottom = iMovieHeight - m_rcPages.bottom;
      }
      else // "Denver" mode: use (accompanying) video instead
      {
         rectInsets->left   = m_rcVideo.left;
         rectInsets->top    = m_rcVideo.top;
         rectInsets->right  = iMovieWidth - m_rcVideo.right;
         rectInsets->bottom = iMovieHeight - m_rcVideo.bottom;
      }
   }

   // Debug info
   _tprintf(_T("- Insets: \n"));
   _tprintf(_T("  left: %d, top: %d, right: %d, bottom: %d\n"), rectInsets->left, rectInsets->top, rectInsets->right, rectInsets->bottom);

   return hr;
}

UINT CFlashEngine::CalculateSpritePositions(int iVideoWidth, int iVideoHeight,
                                            int iWbWidth, int iWbHeight,
                                            int iLogoWidth, int iLogoHeight,
                                            int iControlBarWidth, int iControlBarHeight,
                                            int iMetadataWidth, int iMetadataHeight)
{
   _tprintf(_T("- CalculateSpritePositions() Movie: %d,%d %dx%d\n"), m_rcMovie.left, m_rcMovie.top, m_rcMovie.Width(), m_rcMovie.Height());

   m_rcMetadata.SetRectEmpty();
   m_rcLogo.SetRectEmpty();
   if (m_bHasMetadata)
   {
      // Improvement Bug 3766: Metadata/Logo above Video
      if (m_bHasVideo && (m_docType != DOCUMENT_TYPE_DENVER))
      {
         m_rcMetadata.left = FLASH_BORDERSIZE; // + FLASH_SMALL_PADDING;
         m_rcMetadata.top = FLASH_BORDERSIZE; // + FLASH_SMALL_PADDING;

         if (iLogoWidth > 0 && iLogoHeight > 0)
         {
            m_rcLogo.left = FLASH_BORDERSIZE; // + FLASH_SMALL_PADDING;
            m_rcLogo.top = FLASH_BORDERSIZE; // + FLASH_SMALL_PADDING;
            m_rcLogo.right = m_rcLogo.left + m_iLogoWidth;
            m_rcLogo.bottom = m_rcLogo.top + m_iLogoHeight;

            //m_rcMetadata.left = m_rcLogo.right + FLASH_PADDING;
         }
      }
      else // Metadata above the WebPlayer
      {
         m_rcMetadata.left = FLASH_PADDING;
         m_rcMetadata.top = FLASH_PADDING;

         if (iLogoWidth > 0 && iLogoHeight > 0)
         {
            m_rcLogo.left = FLASH_PADDING;
            m_rcLogo.top = FLASH_PADDING;
            m_rcLogo.right = m_rcLogo.left + m_iLogoWidth;
            m_rcLogo.bottom = m_rcLogo.top + m_iLogoHeight;

            //m_rcMetadata.left = m_rcLogo.right + FLASH_PADDING;
         }
      }

      m_rcMetadata.right = m_rcMetadata.left + iMetadataWidth;
      m_rcMetadata.bottom = m_rcMetadata.top;
      m_rcMetadata.bottom += iMetadataHeight > iLogoHeight ? iMetadataHeight : iLogoHeight;
//      _tprintf(_T("# Metadata: %d,%d  %dx%d\n"), m_rcMetadata.left, m_rcMetadata.top, m_rcMetadata.Width(), m_rcMetadata.Height());
   }

   m_rcVideo.SetRectEmpty();
   if (m_bHasVideo)
   {
      m_rcVideo.left = FLASH_BORDERSIZE;
      if (m_bHasMetadata && m_docType == DOCUMENT_TYPE_DENVER)
         m_rcVideo.left = m_rcMetadata.left + FLASH_BORDERSIZE;

      m_rcVideo.top = FLASH_BORDERSIZE;
      if (m_bHasMetadata && m_docType == DOCUMENT_TYPE_DENVER)
         m_rcVideo.top = m_rcMetadata.bottom + FLASH_PADDING + FLASH_BORDERSIZE;

      m_rcVideo.right = m_rcVideo.left + iVideoWidth;
      m_rcVideo.bottom = m_rcVideo.top + iVideoHeight;
   }

   m_rcPages.SetRectEmpty();
   if (m_bHasPages)
   {
      m_rcPages.left = FLASH_BORDERSIZE;
      if (m_bHasVideo)
         m_rcPages.left = m_rcVideo.right + FLASH_SMALL_PADDING;
      else if (m_bHasMetadata)
         m_rcPages.left = m_rcMetadata.left + FLASH_BORDERSIZE;

      m_rcPages.top = FLASH_BORDERSIZE;
      if (m_bHasMetadata)
         m_rcPages.top = m_rcMetadata.bottom + FLASH_PADDING + FLASH_BORDERSIZE;

      if (m_bHasVideo && m_bHasMetadata)
      {
         m_rcPages.left = m_rcVideo.right + FLASH_SMALL_PADDING;
         m_rcPages.top = FLASH_BORDERSIZE;
      }

      m_rcPages.right = m_rcPages.left + iWbWidth;
      m_rcPages.bottom = m_rcPages.top + iWbHeight;
   }

   if (m_bHasPages && m_bHasVideo)
   {
      // Align video and pages at the bottom line
      if (m_rcPages.bottom > m_rcVideo.bottom)
         m_rcVideo.OffsetRect(0, (m_rcPages.bottom - m_rcVideo.bottom));
      else
         m_rcPages.OffsetRect(0, (m_rcVideo.bottom - m_rcPages.bottom));
   }

   m_rcControlBar.SetRectEmpty();
   if (m_bHasControlBar)
   {
      m_rcControlBar.left = FLASH_BORDERSIZE;
      if (m_bHasMetadata)
         m_rcControlBar.left = m_rcMetadata.left + FLASH_BORDERSIZE;;

      m_rcControlBar.top = FLASH_BORDERSIZE;
      if (m_bHasPages)
         m_rcControlBar.top = m_rcPages.bottom;
      else if (m_bHasVideo)
         m_rcControlBar.top = m_rcVideo.bottom;

      m_rcControlBar.right = m_rcControlBar.left + iControlBarWidth;
      m_rcControlBar.bottom = m_rcControlBar.top + iControlBarHeight;

      // Expand ControlBar in width, if necessary
      if (m_bHasPages && m_rcPages.right > m_rcControlBar.right)
         m_rcControlBar.right = m_rcPages.right;
      else if (!m_bHasPages && m_bHasVideo && m_rcVideo.right > m_rcControlBar.right)
         m_rcControlBar.right = m_rcVideo.right;

      // TODO: Behaviour for the case if ControlBar is shorter then the minimum width
      // --> Trivial solution: Resize the whole ControlBar
      m_fScaleControlBar = 1.0f;
      if (m_bHasPages && m_rcPages.right < m_rcControlBar.right)
      {
         m_rcControlBar.right = m_rcPages.right;
         int iShortenedLength = m_rcControlBar.right - m_rcControlBar.left;
         m_fScaleControlBar = (float)iShortenedLength / (float)iControlBarWidth;
         int iShortenedHeight = (int)(m_fScaleControlBar * iControlBarHeight + 0.5f);
         m_rcControlBar.bottom = m_rcControlBar.top + iShortenedHeight;
      }
      else if (!m_bHasPages && m_bHasVideo && m_rcVideo.right < m_rcControlBar.right)
      {
         m_rcControlBar.right = m_rcVideo.right;
         int iShortenedLength = m_rcControlBar.right - m_rcControlBar.left;
         m_fScaleControlBar = (float)iShortenedLength / (float)iControlBarWidth;
         int iShortenedHeight = (int)(m_fScaleControlBar * iControlBarHeight + 0.5f);
         m_rcControlBar.bottom = m_rcControlBar.top + iShortenedHeight;
      }
   }

   if (m_bHasMetadata)
   {
      if (m_bHasPages)
         m_rcMetadata.right = m_rcPages.right;
      else if (m_bHasVideo)
         m_rcMetadata.right = m_rcVideo.right;

      if (m_bHasPages && m_bHasVideo)
      {
         m_rcMetadata.right = m_rcVideo.right - FLASH_PADDING + FLASH_SMALL_PADDING;
         m_rcMetadata.bottom = m_rcVideo.top - FLASH_PADDING + FLASH_SMALL_PADDING;

         int minMetadataWidth = MINIMUM_METADATA_WIDTH;
         if (m_rcMetadata.Width() < minMetadataWidth)
         {
            int dShiftVideo = (int)(0.5f * (minMetadataWidth - m_rcVideo.Width()));
            int dShiftPages = minMetadataWidth - m_rcMetadata.Width();

            m_rcMetadata.right = m_rcMetadata.left + minMetadataWidth;

            m_rcVideo.OffsetRect(dShiftVideo, 0);
            m_rcPages.OffsetRect(dShiftPages, 0);

            if (m_bHasControlBar)
               m_rcControlBar.right = m_rcPages.right;
         }
      }
   }
   //_tprintf(_T("# Metadata: %d,%d  %dx%d\n"), m_rcMetadata.left, m_rcMetadata.top, m_rcMetadata.Width(), m_rcMetadata.Height());
   
   return S_OK;
}

UINT CFlashEngine::CalculateSpritePositionsNew(int iMovieWidth, int iMovieHeight, 
                                               int iVideoWidth, int iVideoHeight,
                                               int iWbWidth, int iWbHeight,
                                               int iLogoWidth, int iLogoHeight,
                                               int iControlBarWidth, int iControlBarHeight,
                                               int iMetadataWidth, int iMetadataHeight, 
                                               int &iNewWbWidth, int &iNewWbHeight, 
                                               bool bShowVideoOnPages)
{
    bool bHasNormalVideo     = (m_bHasVideo && !bShowVideoOnPages);
    bool bHasStructuredVideo = (m_bHasVideo && bShowVideoOnPages);
    // Bugfix 5598: 
    if (bHasStructuredVideo && m_bShowClipsInVideoArea) {
        bHasNormalVideo = true;
        bHasStructuredVideo = false;
    }

   m_rcMetadata.SetRectEmpty();
   m_rcLogo.SetRectEmpty();
   if (m_bHasMetadata)
   {
      // Improvement Bug 3766: Metadata/Logo above Video
      if (bHasNormalVideo && (m_docType != DOCUMENT_TYPE_DENVER))
      {
         m_rcMetadata.left = FLASH_BORDERSIZE;
         m_rcMetadata.top = FLASH_BORDERSIZE;

         if (iLogoWidth > 0 && iLogoHeight > 0)
         {
            m_rcLogo.left = FLASH_BORDERSIZE;
            m_rcLogo.top = FLASH_BORDERSIZE;
            m_rcLogo.right = m_rcLogo.left + m_iLogoWidth;
            m_rcLogo.bottom = m_rcLogo.top + m_iLogoHeight;
         }
      }
      else // Metadata above the WebPlayer
      {
         m_rcMetadata.left = FLASH_PADDING;
         m_rcMetadata.top = FLASH_PADDING;

         if (iLogoWidth > 0 && iLogoHeight > 0)
         {
            m_rcLogo.left = FLASH_PADDING;
            m_rcLogo.top = FLASH_PADDING;
            m_rcLogo.right = m_rcLogo.left + m_iLogoWidth;
            m_rcLogo.bottom = m_rcLogo.top + m_iLogoHeight;
         }
      }

      m_rcMetadata.right = m_rcMetadata.left + iMetadataWidth;
      m_rcMetadata.bottom = m_rcMetadata.top;
      m_rcMetadata.bottom += iMetadataHeight > iLogoHeight ? iMetadataHeight : iLogoHeight;
   }

   m_rcVideo.SetRectEmpty();
   if (bHasNormalVideo)
   {
      m_rcVideo.left = FLASH_BORDERSIZE;
      if (m_bHasMetadata && m_docType == DOCUMENT_TYPE_DENVER)
         m_rcVideo.left = m_rcMetadata.left + FLASH_BORDERSIZE;

      m_rcVideo.top = FLASH_BORDERSIZE;
      if (m_bHasMetadata && m_docType == DOCUMENT_TYPE_DENVER)
         m_rcVideo.top = m_rcMetadata.bottom + FLASH_PADDING + FLASH_BORDERSIZE;

      m_rcVideo.right = m_rcVideo.left + iVideoWidth;
      m_rcVideo.bottom = m_rcVideo.top + iVideoHeight;
   }

   m_rcPages.SetRectEmpty();
   if (m_bHasPages)
   {
      m_rcPages.left = FLASH_BORDERSIZE;
      if (bHasNormalVideo)
         m_rcPages.left = m_rcVideo.right + FLASH_SMALL_PADDING;
      else if (m_bHasMetadata)
         m_rcPages.left = m_rcMetadata.left + FLASH_BORDERSIZE;

      m_rcPages.top = FLASH_BORDERSIZE;
      if (m_bHasMetadata)
         m_rcPages.top = m_rcMetadata.bottom + FLASH_PADDING + FLASH_BORDERSIZE;

      if (bHasNormalVideo && m_bHasMetadata)
      {
         m_rcPages.left = m_rcVideo.right + FLASH_SMALL_PADDING;
         m_rcPages.top = FLASH_BORDERSIZE;
      }

      int iCalcPagesRight = iMovieWidth - FLASH_BORDERSIZE;
      if (m_bHasMetadata && (!bHasNormalVideo || m_docType == DOCUMENT_TYPE_DENVER))
         iCalcPagesRight -= FLASH_PADDING;

      int iCalcPagesBottom = iMovieHeight - DEFAULT_CONTROLBAR_HEIGHT; 
      // Note: The line above does not consider, if controlbar is visible 
      // or if controlbar is resized to a smaller size
      if (m_bHasMetadata && (bHasNormalVideo || m_docType == DOCUMENT_TYPE_DENVER))
         iCalcPagesBottom -= FLASH_PADDING;

      // Return error code in case of negative WB width or height
      // TODO: define error code
      if (iCalcPagesRight <= m_rcPages.left)
         return E_FAIL;
      if (iCalcPagesBottom <= m_rcPages.top)
         return E_FAIL;
   
      int iMaxWbWidth = iCalcPagesRight - m_rcPages.left;
      int iMaxWbHeight = iCalcPagesBottom - m_rcPages.top;
      float dScaleX = (double)((double)iMaxWbWidth / (double)iWbWidth);
      float dScaleY = (double)((double)iMaxWbHeight / (double)iWbHeight);

      m_dWhiteboardScale = (dScaleX < dScaleY) ? dScaleX : dScaleY;

      iNewWbWidth = (int)(0.5f + (m_dWhiteboardScale * (double)iWbWidth));
      iNewWbHeight = (int)(0.5f + (m_dWhiteboardScale * (double)iWbHeight));

      m_rcPages.right = m_rcPages.left + iNewWbWidth;
      m_rcPages.bottom = m_rcPages.top + iNewWbHeight;

      if (bHasStructuredVideo) {
          // The Structured SG video is displayed over the pages 
          // --> use m_rcPages for the video position & dimension
          m_rcVideo.left = m_rcPages.left;
          m_rcVideo.top = m_rcPages.top;
          m_rcVideo.right = m_rcPages.right;
          m_rcVideo.bottom = m_rcPages.bottom;
      }
   }

   if (m_bHasPages && bHasNormalVideo)
   {
      // Align video and pages at the bottom line
      if (m_rcPages.bottom > m_rcVideo.bottom)
         m_rcVideo.OffsetRect(0, (m_rcPages.bottom - m_rcVideo.bottom));
      else
         m_rcPages.OffsetRect(0, (m_rcVideo.bottom - m_rcPages.bottom));
   }

   m_rcControlBar.SetRectEmpty();
   if (m_bHasControlBar)
   {
      m_rcControlBar.left = FLASH_BORDERSIZE;
      if (m_bHasMetadata)
         m_rcControlBar.left = m_rcMetadata.left + FLASH_BORDERSIZE;;

      m_rcControlBar.top = FLASH_BORDERSIZE;
      if (m_bHasPages)
         m_rcControlBar.top = m_rcPages.bottom;
      else if (bHasNormalVideo)
         m_rcControlBar.top = m_rcVideo.bottom;

      m_rcControlBar.right = m_rcControlBar.left + iControlBarWidth;
      m_rcControlBar.bottom = m_rcControlBar.top + iControlBarHeight;

      // Expand ControlBar in width, if necessary
      if (m_bHasPages && m_rcPages.right > m_rcControlBar.right)
         m_rcControlBar.right = m_rcPages.right;
      else if (!m_bHasPages && bHasNormalVideo && m_rcVideo.right > m_rcControlBar.right)
         m_rcControlBar.right = m_rcVideo.right;

      // TODO: Behaviour for the case if ControlBar is shorter then the minimum width
      // --> Trivial solution: Resize the whole ControlBar
      m_fScaleControlBar = 1.0f;
      if (m_bHasPages && m_rcPages.right < m_rcControlBar.right)
      {
         m_rcControlBar.right = m_rcPages.right;
         int iShortenedLength = m_rcControlBar.right - m_rcControlBar.left;
         m_fScaleControlBar = (float)iShortenedLength / (float)iControlBarWidth;
         int iShortenedHeight = (int)(m_fScaleControlBar * iControlBarHeight + 0.5f);
         m_rcControlBar.bottom = m_rcControlBar.top + iShortenedHeight;
      }
      else if (!m_bHasPages && bHasNormalVideo && m_rcVideo.right < m_rcControlBar.right)
      {
         m_rcControlBar.right = m_rcVideo.right;
         int iShortenedLength = m_rcControlBar.right - m_rcControlBar.left;
         m_fScaleControlBar = (float)iShortenedLength / (float)iControlBarWidth;
         int iShortenedHeight = (int)(m_fScaleControlBar * iControlBarHeight + 0.5f);
         m_rcControlBar.bottom = m_rcControlBar.top + iShortenedHeight;
      }
   }

   if (m_bHasMetadata)
   {
      if (m_bHasPages)
         m_rcMetadata.right = m_rcPages.right;
      else if (bHasNormalVideo)
         m_rcMetadata.right = m_rcVideo.right;

      if (m_bHasPages && bHasNormalVideo)
      {
         m_rcMetadata.right = m_rcVideo.right - FLASH_PADDING + FLASH_SMALL_PADDING;
         m_rcMetadata.bottom = m_rcVideo.top +- FLASH_PADDING + FLASH_SMALL_PADDING;

         int minMetadataWidth = MINIMUM_METADATA_WIDTH;
         if (m_rcMetadata.Width() < minMetadataWidth)
         {
            int dShiftVideo = (int)(0.5f * (minMetadataWidth - m_rcVideo.Width()));
            int dShiftPages = minMetadataWidth - m_rcMetadata.Width();

            m_rcMetadata.right = m_rcMetadata.left + minMetadataWidth;

            m_rcVideo.OffsetRect(dShiftVideo, 0);
            m_rcPages.OffsetRect(dShiftPages, 0);

            if (m_bHasControlBar)
               m_rcControlBar.right = m_rcPages.right;
         }
      }
   }
   _tprintf(_T("# Pages (calc.): %d,%d  %dx%d\n"), m_rcPages.left, m_rcPages.top, m_rcPages.Width(), m_rcPages.Height());
   
   return S_OK;
}

UINT CFlashEngine::CalculateSpritePositionsSlidestarNew(int iMovieWidth, int iMovieHeight, 
                                               int iVideoWidth, int iVideoHeight,
                                               int iWbWidth, int iWbHeight,
                                               /*int iLogoWidth, int iLogoHeight,*/
                                               int iControlBarWidth, int iControlBarHeight,
                                               /*int iMetadataWidth, int iMetadataHeight, */
                                               int &iNewWbWidth, int &iNewWbHeight, 
                                               bool bShowVideoOnPages)
{
    bool bHasNormalVideo     = (m_bHasVideo && !bShowVideoOnPages);
    bool bHasStructuredVideo = (m_bHasVideo && bShowVideoOnPages);
    // Bugfix 5598: 
    if (bHasStructuredVideo && m_bShowClipsInVideoArea) {
        bHasNormalVideo = true;
        bHasStructuredVideo = false;
    }

   m_rcMetadata.SetRectEmpty();
   m_rcLogo.SetRectEmpty();
   float fVideoPageSpace = 40;
	float fCBWidth = 544;
	float fWidth = 0;

   m_rcVideo.SetRectEmpty();
   if (bHasNormalVideo)
   {
	   m_rcVideo.left = 0; //FLASH_BORDERSIZE;// + FLASH_PADDING;
	   m_rcVideo.top = 0; //FLASH_BORDERSIZE;
	   m_rcVideo.right = m_rcVideo.left + iVideoWidth;// + FLASH_BORDERSIZE;
	   m_rcVideo.bottom = m_rcVideo.top + iVideoHeight;// + FLASH_BORDERSIZE;
   }

   m_rcPages.SetRectEmpty();
   if (m_bHasPages)
   {
      m_rcPages.left = 0;//FLASH_BORDERSIZE;
      if (bHasNormalVideo)
         m_rcPages.left = fVideoPageSpace + m_rcVideo.right - FLASH_BORDERSIZE;
	  m_rcPages.top = 0;//FLASH_BORDERSIZE;// + FLASH_PADDING;

      int iCalcPagesRight = iMovieWidth /*- FLASH_BORDERSIZE*/ - FLASH_PADDING;
	  int iCalcPagesBottom = iMovieHeight - DEFAULT_CONTROLBAR_HEIGHT /*- FLASH_BORDERSIZE*/; 
      // Note: The line above does not consider, if controlbar is visible 
      // or if controlbar is resized to a smaller size

      // Return error code in case of negative WB width or height
      // TODO: define error code
      if (iCalcPagesRight <= m_rcPages.left)
         return E_FAIL;
      if (iCalcPagesBottom <= m_rcPages.top)
         return E_FAIL;
   
      //int iMaxWbWidth = iCalcPagesRight - m_rcPages.left;
      //int iMaxWbHeight = iCalcPagesBottom - m_rcPages.top;
      //float dScaleX = (double)((double)iMaxWbWidth / (double)iWbWidth);
      //float dScaleY = (double)((double)iMaxWbHeight / (double)iWbHeight);

      //m_dWhiteboardScale = (dScaleX < dScaleY) ? dScaleX : dScaleY;

      //iNewWbWidth = (int)(0.5f + (m_dWhiteboardScale * (double)iWbWidth));
      //iNewWbHeight = (int)(0.5f + (m_dWhiteboardScale * (double)iWbHeight));

      m_rcPages.right = m_rcPages.left + iWbWidth - 2*FLASH_BORDERSIZE;// + FLASH_PADDING;
      m_rcPages.bottom = m_rcPages.top + iWbHeight - 2*FLASH_BORDERSIZE;

      if (bHasStructuredVideo) {
          // The Structured SG video is displayed over the pages 
          // --> use m_rcPages for the video position & dimension
          m_rcVideo.left = m_rcPages.left;
          m_rcVideo.top = m_rcPages.top;
          m_rcVideo.right = m_rcPages.right;
          m_rcVideo.bottom = m_rcPages.bottom;
      }
   }

   if (m_bHasPages && bHasNormalVideo)
   {
      /*
      // Align video and pages at the bottom line
      if (m_rcPages.bottom > m_rcVideo.bottom)
         m_rcVideo.OffsetRect(0, (m_rcPages.bottom - m_rcVideo.bottom) + FLASH_BORDERSIZE);
      else
         m_rcPages.OffsetRect(0, (m_rcVideo.bottom - m_rcPages.bottom));
      */

      /*
      // Align video and pages in the middle
      if (m_rcPages.bottom > m_rcVideo.bottom)
         m_rcVideo.OffsetRect(0, ((m_rcPages.bottom - m_rcVideo.bottom)/2));
      else
         m_rcPages.OffsetRect(0, ((m_rcVideo.bottom - m_rcPages.bottom)/2));
      */

      ///*
      // Align video and pages at the top line
      if (m_rcPages.top < m_rcVideo.top)
         m_rcVideo.OffsetRect(0, (m_rcPages.top - m_rcVideo.top));
      else
         m_rcPages.OffsetRect(0, (m_rcVideo.top - m_rcPages.top));
      //*/

   }

   //   m_rcControlBar.SetRectEmpty();
   //if (m_bHasControlBar)
   //{
	  // float fCBWidth = 544;
	  // float fWidth = 0;
	  // if (m_bHasPages)
		 //  fWidth += m_rcPages.Width();
	  // if (bHasNormalVideo)
		 //  fWidth += m_rcVideo.Width();
	  // if (m_bHasPages && bHasNormalVideo)
		 //  fWidth += fVideoPageSpace;
   //    m_rcControlBar.left = (fWidth - fCBWidth) / 2;
	  // if (m_bHasPages)
   //      m_rcControlBar.top = m_rcPages.bottom;
   //    else if (bHasNormalVideo)
   //      m_rcControlBar.top = m_rcVideo.bottom;
	  // if (m_bHasPages && bHasNormalVideo)
		 //  if (m_rcPages.bottom > m_rcVideo.bottom)
			//   m_rcControlBar.top = m_rcPages.bottom;
		 //  else
			//   m_rcControlBar.top = m_rcVideo.bottom;
	  // m_rcControlBar.top += 46;
   //    m_rcControlBar.right = m_rcControlBar.left + fCBWidth ;
	  // m_rcControlBar.bottom = m_rcControlBar.top + DEFAULT_CONTROLBAR_HEIGHT;
   //}


   m_rcControlBar.SetRectEmpty();
   if (m_bHasControlBar)
   {
      m_rcControlBar.left = FLASH_BORDERSIZE;
      m_rcControlBar.top = FLASH_BORDERSIZE;
      if (m_bHasPages)
         fWidth += m_rcPages.Width();
      if (bHasNormalVideo)
         fWidth += m_rcVideo.Width();
	  if (m_bHasPages && bHasNormalVideo)
		 fWidth += fVideoPageSpace;
	  m_rcControlBar.left = (fWidth - fCBWidth) / 2;
		if (m_bHasPages)
         m_rcControlBar.top = m_rcPages.bottom;
       else if (bHasNormalVideo)
         m_rcControlBar.top = m_rcVideo.bottom;
	   if (m_bHasPages && bHasNormalVideo)
		   if (m_rcPages.bottom > m_rcVideo.bottom)
			   m_rcControlBar.top = m_rcPages.bottom;
		   else
			   m_rcControlBar.top = m_rcVideo.bottom;
   	   m_rcControlBar.top += 46;
       m_rcControlBar.right = m_rcControlBar.left + fCBWidth ;
	   m_rcControlBar.bottom = m_rcControlBar.top + DEFAULT_CONTROLBAR_HEIGHT;

      // TODO: Behaviour for the case if ControlBar is shorter then the minimum width
      // --> Trivial solution: Resize the whole ControlBar
      //m_fScaleControlBar = 1.0f;
      //if (m_bHasPages && m_rcPages.right < m_rcControlBar.right)
      //{
      //   m_rcControlBar.right = m_rcPages.right;
      //   int iShortenedLength = m_rcControlBar.right - m_rcControlBar.left;
      //   m_fScaleControlBar = (float)iShortenedLength / (float)iControlBarWidth;
      //   int iShortenedHeight = (int)(m_fScaleControlBar * iControlBarHeight + 0.5f);
      //   m_rcControlBar.bottom = m_rcControlBar.top + iShortenedHeight;
      //}
      //else if (!m_bHasPages && bHasNormalVideo && m_rcVideo.right < m_rcControlBar.right)
      //{
      //   m_rcControlBar.right = m_rcVideo.right;
      //   int iShortenedLength = m_rcControlBar.right - m_rcControlBar.left;
      //   m_fScaleControlBar = (float)iShortenedLength / (float)iControlBarWidth;
      //   int iShortenedHeight = (int)(m_fScaleControlBar * iControlBarHeight + 0.5f);
      //   m_rcControlBar.bottom = m_rcControlBar.top + iShortenedHeight;
      //}
   }

   _tprintf(_T("# Pages (calc.): %d,%d  %dx%d\n"), m_rcPages.left, m_rcPages.top, m_rcPages.Width(), m_rcPages.Height());
   
   return S_OK;
}

UINT CFlashEngine::CalculateWhiteboardScalingFromMovie(UINT nMovieWith, UINT nMovieSize)
{
   _tprintf(_T("- CalculateWhiteboardScalingFromMovie()\n"));

   UINT hr = S_OK;

   if (!m_bHasVideo)
   {
      m_dWhiteboardScale = 1.0;
      return S_OK;
   }

   hr = CalculateMovieDimension(m_bHasMetadata, m_bHasControlBar, m_bHasPages);

   int iVideoAreaWidth = m_rcVideo.Width();
   if (m_bHasMetadata && MINIMUM_METADATA_WIDTH > iVideoAreaWidth)
      iVideoAreaWidth = MINIMUM_METADATA_WIDTH;

   int iPagesWidth = m_rcMovie.Width() - 
                     (2*FLASH_BORDERSIZE) -   // border around all sprites
                     iVideoAreaWidth -         // video and metadata area
                     FLASH_SMALL_PADDING;      // space between video and whiteboard
 
   if(m_bIsSlidestar)
	   iPagesWidth = m_rcMovie.Width() - (2*FLASH_BORDERSIZE) - iVideoAreaWidth - FLASH_PADDING - 40;

   m_dWhiteboardScale = iPagesWidth / m_rcPages.Width();

   return S_OK;
}

UINT CFlashEngine::CalculateSpritePositionsPsp(int iVideoWidth, int iVideoHeight,
                                               int iWbWidth, int iWbHeight)
{
   if (m_rcMovie.IsRectEmpty())
   {
      m_rcMovie.left = 0;
      m_rcMovie.top = 0;
      m_rcMovie.right = PSP_MOVIE_WIDTH;
      m_rcMovie.bottom = PSP_MOVIE_HEIGHT;
   }

   _tprintf(_T("- CalculateSpritePositionsPsp() Movie: %d,%d %dx%d\n"), m_rcMovie.left, m_rcMovie.top, m_rcMovie.Width(), m_rcMovie.Height());
   _tprintf(_T("- CalculateSpritePositionsPsp() WB: %dx%d\n"), iWbWidth, iWbHeight);

   m_rcMetadata.SetRectEmpty();
   m_rcLogo.SetRectEmpty();

   m_rcVideo.SetRectEmpty();
   if (m_bHasVideo)
   {
      m_rcVideo.left = 0;
      m_rcVideo.top = 0;
 
      m_rcVideo.right = m_rcVideo.left + iVideoWidth;
      m_rcVideo.bottom = m_rcVideo.top + iVideoHeight;
   }

   m_rcPages.SetRectEmpty();
   if (m_bHasPages)
   {
      if (m_bHasVideo)
         m_rcPages.left = m_rcVideo.right + FLASH_SMALL_PADDING;
      else
         m_rcPages.left = 0;
      m_rcPages.top = 0;

      m_rcPages.right = m_rcPages.left + iWbWidth;
      m_rcPages.bottom = m_rcPages.top + iWbHeight;
   }

   if (m_bHasPages && m_bHasVideo)
   {
      if (m_rcPages.bottom > m_rcVideo.bottom)
         m_rcVideo.OffsetRect(0, (m_rcPages.bottom - m_rcVideo.bottom));
      else
         m_rcPages.OffsetRect(0, (m_rcVideo.bottom - m_rcPages.bottom));
   }

   CRect rcPspContent;
   rcPspContent.SetRectEmpty();
   if (m_bHasVideo)
      rcPspContent.UnionRect(rcPspContent, m_rcVideo);
   if (m_bHasPages)
      rcPspContent.UnionRect(rcPspContent, m_rcPages);

   float fScaleX = (float)m_rcMovie.Width() / (float)PSP_MOVIE_WIDTH;
   float fScaleY = (float)m_rcMovie.Height() / (float)PSP_MOVIE_HEIGHT;
   float fScale = fScaleX < fScaleY ? fScaleX : fScaleY;

   float pspContentWidth = m_bHasControlBar ? PSP_CONTENT_WIDTH : PSP_MOVIE_WIDTH;
   pspContentWidth *= fScale;
   float pspContentHeight = PSP_MOVIE_HEIGHT * fScale;

   float fContentScaleX = (float)pspContentWidth / (float)rcPspContent.Width();
   float fContentScaleY = (float)pspContentHeight / (float)rcPspContent.Height();
   float fContentScale = fContentScaleX < fContentScaleY ? fContentScaleX : fContentScaleY;

   float pspContentX = m_bHasControlBar ? PSP_OFFSET_X : 0;
   pspContentX *= fScale;
   pspContentX += 0.5f * (pspContentWidth - rcPspContent.Width()*fContentScale);

   float pspContentY = 0.5f * (pspContentHeight - rcPspContent.Height()*fContentScale);

//   m_rcVideo.left *= fContentScale;
//   m_rcVideo.top *= fContentScale;
//   m_rcVideo.right *= fContentScale;
//   m_rcVideo.bottom *= fContentScale;
   m_rcVideo.left = (int)(fContentScale * m_rcVideo.left + 0.5f);
   m_rcVideo.top = (int)(fContentScale * m_rcVideo.top + 0.5f);
   m_rcVideo.right = (int)(fContentScale * m_rcVideo.right + 0.5f);
   m_rcVideo.bottom = (int)(fContentScale * m_rcVideo.bottom + 0.5f);
   m_rcVideo.OffsetRect(pspContentX, pspContentY);

//   m_rcPages.left *= fContentScale;
//   m_rcPages.top *= fContentScale;
//   m_rcPages.right *= fContentScale;
//   m_rcPages.bottom *= fContentScale;
   m_rcPages.left = (int)(fContentScale * m_rcPages.left + 0.5f);
   m_rcPages.top = (int)(fContentScale * m_rcPages.top + 0.5f);
   m_rcPages.right = (int)(fContentScale * m_rcPages.right + 0.5f);
   m_rcPages.bottom = (int)(fContentScale * m_rcPages.bottom + 0.5f);
   m_rcPages.OffsetRect(pspContentX, pspContentY);

   m_rcControlBar.SetRectEmpty();
   if (m_bHasControlBar)
   {
      m_rcControlBar.left = 0;
      m_rcControlBar.top = 0;

      m_rcControlBar.right = PSP_MOVIE_WIDTH * fScale;
      m_rcControlBar.bottom = PSP_MOVIE_HEIGHT * fScale;
   }

   //CString csMsg;
   //csMsg.Format(_T("- Pages: %d,%d %dx%d"), m_rcPages.left, m_rcPages.top, m_rcPages.Width(), m_rcPages.Height());
   //MessageBox(NULL, csMsg, _T("Info"), MB_OK);
   _tprintf(_T("- Pages: %d,%d %dx%d   gs=%2.2f  s=%2.2f  cs=%2.2f\n"), m_rcPages.left, m_rcPages.top, m_rcPages.Width(), m_rcPages.Height(), m_dGlobalScale, fScale, fContentScale);

   m_dContentScale = fContentScale * fScale;
   //m_dGlobalScale *= fContentScale;

   return S_OK;
}

UINT CFlashEngine::CalculateSpritePositionsPspNew(int iMovieWidth, int iMovieHeight, 
                                                  int iVideoWidth, int iVideoHeight,
                                                  int iWbWidth, int iWbHeight, 
                                                  int &iNewWbWidth, int &iNewWbHeight, 
                                                  bool bShowVideoOnPages)
{
    bool bHasNormalVideo     = (m_bHasVideo && !bShowVideoOnPages);
    bool bHasStructuredVideo = (m_bHasVideo && bShowVideoOnPages);
    // Bugfix 5598: 
    if (bHasStructuredVideo && m_bShowClipsInVideoArea) {
        bHasNormalVideo = true;
        bHasStructuredVideo = false;
    }

   m_rcMovie.left = 0;
   m_rcMovie.top = 0;
   m_rcMovie.right = iMovieWidth;
   m_rcMovie.bottom = iMovieHeight;

   float fScaleX = (float)m_rcMovie.Width() / (float)PSP_MOVIE_WIDTH;
   float fScaleY = (float)m_rcMovie.Height() / (float)PSP_MOVIE_HEIGHT;
   float fScale = fScaleX < fScaleY ? fScaleX : fScaleY;

   float pspContentWidth = m_bHasControlBar ? PSP_CONTENT_WIDTH : PSP_MOVIE_WIDTH;
   pspContentWidth *= fScale;
   float pspContentHeight = PSP_MOVIE_HEIGHT * fScale;


   m_rcMetadata.SetRectEmpty();
   m_rcLogo.SetRectEmpty();

   m_rcVideo.SetRectEmpty();
   if (bHasNormalVideo)
   {
      m_rcVideo.left = 0;
      m_rcVideo.top = 0;
 
      if (m_bHasPages)
      {
         m_rcVideo.right = m_rcVideo.left + iVideoWidth;
         m_rcVideo.bottom = m_rcVideo.top + iVideoHeight;
      }
      else // Denver
      {
         int iCalcPagesRight = pspContentWidth;
         int iCalcPagesBottom = pspContentHeight;
         int iMaxSgClipWidth = iCalcPagesRight - m_rcVideo.left;
         int iMaxSgClipHeight = iCalcPagesBottom - m_rcVideo.top;
         double dScaleX = (double)((double)iMaxSgClipWidth / (double)iVideoWidth);
         double dScaleY = (double)((double)iMaxSgClipHeight / (double)iVideoHeight);
         double dScale = (dScaleX < dScaleY) ? dScaleX : dScaleY;
         int iNewSgClipWidth = (int)(0.5f + (dScale * (double)iVideoWidth));
         int iNewSgClipHeight = (int)(0.5f + (dScale * (double)iVideoHeight));

         m_rcVideo.right = m_rcVideo.left + iNewSgClipWidth;
         m_rcVideo.bottom = m_rcVideo.top + iNewSgClipHeight;
      }
   }

   m_rcPages.SetRectEmpty();
   if (m_bHasPages)
   {
      if (bHasNormalVideo)
         m_rcPages.left = m_rcVideo.right + FLASH_SMALL_PADDING;
      else
         m_rcPages.left = 0;
      m_rcPages.top = 0;

      int iCalcPagesRight = pspContentWidth;
      int iCalcPagesBottom = pspContentHeight;

      // Return error code in case of negative WB width or height
      // TODO: define error code
      if (iCalcPagesRight <= m_rcPages.left)
         return E_FAIL;
      if (iCalcPagesBottom <= m_rcPages.top)
         return E_FAIL;
   
      int iMaxWbWidth = iCalcPagesRight - m_rcPages.left;
      int iMaxWbHeight = iCalcPagesBottom - m_rcPages.top;
      double dScaleX = (double)((double)iMaxWbWidth / (double)iWbWidth);
      double dScaleY = (double)((double)iMaxWbHeight / (double)iWbHeight);

      m_dWhiteboardScale = (dScaleX < dScaleY) ? dScaleX : dScaleY;

      iNewWbWidth = (int)(0.5f + (m_dWhiteboardScale * (double)iWbWidth));
      iNewWbHeight = (int)(0.5f + (m_dWhiteboardScale * (double)iWbHeight));

      m_rcPages.right = m_rcPages.left + iNewWbWidth;
      m_rcPages.bottom = m_rcPages.top + iNewWbHeight;

      if (bHasStructuredVideo) {
          // The Structured SG video is displayed over the pages 
          // --> use m_rcPages for the video position & dimension
          m_rcVideo.left = m_rcPages.left;
          m_rcVideo.top = m_rcPages.top;
          m_rcVideo.right = m_rcPages.right;
          m_rcVideo.bottom = m_rcPages.bottom;
      }
   }

   if (m_bHasPages && bHasNormalVideo)
   {
      if (m_rcPages.bottom > m_rcVideo.bottom)
         m_rcVideo.OffsetRect(0, (m_rcPages.bottom - m_rcVideo.bottom));
      else
         m_rcPages.OffsetRect(0, (m_rcVideo.bottom - m_rcPages.bottom));
   }

   CRect rcPspContent;
   rcPspContent.SetRectEmpty();
   if (bHasNormalVideo)
      rcPspContent.UnionRect(rcPspContent, m_rcVideo);
   if (m_bHasPages)
      rcPspContent.UnionRect(rcPspContent, m_rcPages);

   float fContentScaleX = (float)pspContentWidth / (float)rcPspContent.Width();
   float fContentScaleY = (float)pspContentHeight / (float)rcPspContent.Height();
   float fContentScale = fContentScaleX < fContentScaleY ? fContentScaleX : fContentScaleY;

   float pspContentX = m_bHasControlBar ? PSP_OFFSET_X : 0;
   pspContentX *= fScale;
   pspContentX += 0.5f * (pspContentWidth - rcPspContent.Width()*fContentScale);

   float pspContentY = 0.5f * (pspContentHeight - rcPspContent.Height()*fContentScale);

//   m_rcVideo.left = (int)(fContentScale * m_rcVideo.left + 0.5f);
//   m_rcVideo.top = (int)(fContentScale * m_rcVideo.top + 0.5f);
//   m_rcVideo.right = (int)(fContentScale * m_rcVideo.right + 0.5f);
//   m_rcVideo.bottom = (int)(fContentScale * m_rcVideo.bottom + 0.5f);
   m_rcVideo.OffsetRect(pspContentX, pspContentY);

//   m_rcPages.left = (int)(fContentScale * m_rcPages.left + 0.5f);
//   m_rcPages.top = (int)(fContentScale * m_rcPages.top + 0.5f);
//   m_rcPages.right = (int)(fContentScale * m_rcPages.right + 0.5f);
//   m_rcPages.bottom = (int)(fContentScale * m_rcPages.bottom + 0.5f);
   m_rcPages.OffsetRect(pspContentX, pspContentY);

   m_rcControlBar.SetRectEmpty();
   if (m_bHasControlBar)
   {
      m_rcControlBar.left = 0;
      m_rcControlBar.top = 0;

      m_rcControlBar.right = PSP_MOVIE_WIDTH * fScale;
      m_rcControlBar.bottom = PSP_MOVIE_HEIGHT * fScale;
   }

   m_dContentScale = fScale;
   //m_dGlobalScale *= fContentScale;

   return S_OK;
}

UINT CFlashEngine::CalculateSpritePositionsSlidestar(int iVideoWidth, int iVideoHeight,
                                               int iWbWidth, int iWbHeight)
{
   float fVideoPageSpace = 40;

   m_rcMetadata.SetRectEmpty();
   m_rcLogo.SetRectEmpty();

   m_rcVideo.SetRectEmpty();
   if (m_bHasVideo)
   {
	   m_rcVideo.left = FLASH_BORDERSIZE + FLASH_PADDING;
	   m_rcVideo.top = FLASH_BORDERSIZE;
	   m_rcVideo.right = m_rcVideo.left + iVideoWidth + FLASH_BORDERSIZE;
	   m_rcVideo.bottom = m_rcVideo.top + iVideoHeight + FLASH_BORDERSIZE;
   }

   m_rcPages.SetRectEmpty();
   if (m_bHasPages)
   {
	   m_rcPages.left = FLASH_BORDERSIZE;
	   if (m_bHasVideo)
		   m_rcPages.left += fVideoPageSpace + m_rcVideo.right;
	   m_rcPages.top = FLASH_BORDERSIZE + FLASH_PADDING;
	   m_rcPages.right = m_rcPages.left + iWbWidth + FLASH_BORDERSIZE + FLASH_PADDING;
	   m_rcPages.bottom = m_rcPages.top + iWbHeight + FLASH_BORDERSIZE;
   }

   if (m_bHasPages && m_bHasVideo)
   {
      if (m_rcPages.bottom > m_rcVideo.bottom)
         m_rcVideo.OffsetRect(0, (m_rcPages.bottom - m_rcVideo.bottom));
      else
         m_rcPages.OffsetRect(0, (m_rcVideo.bottom - m_rcPages.bottom));
   }

   m_rcControlBar.SetRectEmpty();
   if (m_bHasControlBar)
   {
	   float fCBWidth = 544;
	   float fWidth = 0;
	   if (m_bHasPages)
		   fWidth += m_rcPages.Width();
	   if (m_bHasVideo)
		   fWidth += m_rcVideo.Width();
	   if (m_bHasPages && m_bHasVideo)
		   fWidth += fVideoPageSpace;
       m_rcControlBar.left = (fWidth - fCBWidth) / 2;
	   if (m_bHasPages)
         m_rcControlBar.top = m_rcPages.bottom;
       else if (m_bHasVideo)
         m_rcControlBar.top = m_rcVideo.bottom;
	   if (m_bHasPages && m_bHasVideo)
		   if (m_rcPages.bottom > m_rcVideo.bottom)
			   m_rcControlBar.top = m_rcPages.bottom;
		   else
			   m_rcControlBar.top = m_rcVideo.bottom;
	   m_rcControlBar.top += 46;
       m_rcControlBar.right = m_rcControlBar.left + fCBWidth ;
	   m_rcControlBar.bottom = m_rcControlBar.top + DEFAULT_CONTROLBAR_HEIGHT;
   }

   return S_OK;
}

UINT CFlashEngine::CalculateFlashPlayerDimension()
{
    // Start with Movie Dimension
    m_rcFlashPlayer.SetRect(m_rcMovie.TopLeft(), m_rcMovie.BottomRight());

    // Old LEC 3 Flash Player? --> Nothing has to be calculated here.
    if ( !m_bIsNewLecPlayer )
        return S_OK;


    int dWidth = 0; 
    int dHeight = 0;

    // Add Header height (35px)
    dHeight += 35;
    // Add navigation bar height (40px - or 0px if not visible
    if (m_iNavigationControlBar != 2) // 2 = hidden
        dHeight += 40;

    // Add left border width (5px)
    dWidth += 5;
    // Add right border width (5px)
    dWidth += 5;

    // Document structure and/or video? --> additional width
    bool bHasSideStructure = false;
    if ( m_bHasVideo || (m_iNavigationDocumentStructure != 2) ) // 2 = hidden
        bHasSideStructure = true;
    if (bHasSideStructure) {
        // Additional border width (5px)
        dWidth += 5;
        // Additional Document structure/video width
        int nDocumentStructureWidth = (m_iNavigationDocumentStructure != 2) ? 320 : 0;
        dWidth += max(nDocumentStructureWidth, m_rcVideo.Width());
    }

    // Now expand the Fley Player dimension
    m_rcFlashPlayer.right += dWidth;
    m_rcFlashPlayer.bottom += dHeight;

    return S_OK;
}

UINT CFlashEngine::GetMovieSize(UINT &iMovieWidth, UINT &iMovieHeight)
{
   HRESULT hr = S_OK;

   iMovieWidth = 0;
   iMovieHeight = 0;

   if (m_bIsPspDesign)
      hr = CalculateSpritePositionsPsp(m_iAccVideoWidth, m_iAccVideoHeight,
                                       m_iWbInputWidth, m_iWbInputHeight);
   else if(m_bIsSlidestar)
	   hr = CalculateSpritePositionsSlidestar(m_iAccVideoWidth, m_iAccVideoHeight,
                                                m_iWbOutputWidth, m_iWbOutputHeight);
   else
      hr = CalculateSpritePositions(m_iAccVideoWidth, m_iAccVideoHeight,
                                    m_iWbInputWidth, m_iWbInputHeight,
                                    m_iLogoWidth, m_iLogoHeight,
                                    MINIMUM_CONTROLBAR_WIDTH, DEFAULT_CONTROLBAR_HEIGHT,
                                    MINIMUM_CONTROLBAR_WIDTH, FLASH_METADATAHEIGHT);
   if (SUCCEEDED(hr))
   {
      // find out which components are in the recording
      m_bHasVideo = ((m_iAccVideoWidth > 0L) && (m_iAccVideoHeight > 0L)) ? true : false;

      m_bHasClips = (m_iNumOfClips > 0) ? true : false; 

      m_bHasMP3Audio = ((m_tszVideoFlvFile == NULL) && (m_tszMp3File != NULL)) ? true : false;

      //m_bHasPages = ((m_pWhiteboardStream != NULL) && (m_pPageStream != NULL)) ? true : false;
      m_bHasPages = ((m_iWbInputWidth > 0L) && (m_iWbInputHeight > 0L)) ? true : false;

      m_bHasControlBar = (m_iNavigationControlBar == 2) ? false : true;
   }

   if (SUCCEEDED(hr))
   {
      hr = CalculateMovieDimension(m_bHasMetadata, m_bHasControlBar, m_bHasPages);
      iMovieWidth = m_rcMovie.Width();
      iMovieHeight = m_rcMovie.Height();

      _tprintf(_T("- Movie Size: %dx%d\n"), iMovieWidth, iMovieHeight);
   }

   return hr;
}

UINT CFlashEngine::AddInteractionToMovie(SWFMovie *movie, SWFSprite *sprPages)
{
   HRESULT hr = S_OK;

   // After the 1st dry run we have the information 
   // to which frame each stop mark belongs to - and which stop marks are lost.

   // Add stop marks (ActionScript)
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      if (m_aFlashStopmarks.GetSize() > 0)
      {
         int iNumOfStopmarks = m_aFlashStopmarks.GetSize();
         int *anStopmarkFrames = new int[iNumOfStopmarks];
         int idxStopmarks = 0;
         bool bStopmarkLost = false;

         // Re-check the stopmark list for a special case
         ReCheckStopmarkList();

         for (int i = 0; i < iNumOfStopmarks; ++i)
         {
            int nMsTime = m_aFlashStopmarks.ElementAt(i).nMsTime;
            int nFrame = m_aFlashStopmarks.ElementAt(i).nFrame;
            // (Lost frame: nFrame == -1)
            if (nFrame >= 0)
            {
               anStopmarkFrames[idxStopmarks] = nFrame;
               _tprintf(_T("Stopmark %d: %d ms, Frame %d\n"), idxStopmarks, nMsTime, nFrame);
               idxStopmarks++;
            }
            else
            {
               bStopmarkLost = true;
               _tprintf(_T("!Stopmark lost: %d ms, Frame %d\n"), nMsTime, nFrame);
            }
         }
         _tprintf(_T("\n"));

         if (bStopmarkLost)
         {
            // --> Warning message
            ShowLostStopmarksWarningMessage();

            if ((m_hrPossibleWarning & imc_lecturnity_converter_LPLibs_WARN_STOPMARK_LOSS) 
                != imc_lecturnity_converter_LPLibs_WARN_STOPMARK_LOSS)
            {
               m_hrPossibleWarning += imc_lecturnity_converter_LPLibs_WARN_STOPMARK_LOSS;
            }
         }

         // Add ActionScript with paging info
         hr = AddActionScriptForStopmarksToMovie(movie, idxStopmarks, anStopmarkFrames);

         delete[] anStopmarkFrames;
         anStopmarkFrames = NULL;
      }
   }

   // Add target marks (ActionScript)
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      // TODO: Target marks ( - if they are used)
   }

   // Interaction objects (Areas/Buttons, RadioButtons/CheckBoxes)
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      if (m_aInteractionAreas.GetSize() > 0)
      {
         // Max index so far has to be increased to prevent 
         // side effects belonging to identical depths of objects
         m_iMaxInteractionIndex = m_iMaxIndexSoFar + 100;

         hr = CreateInteractionAreas(sprPages);

         if (SUCCEEDED(hr))
         {
            // BUGFIX BLECPUB-1864: Masking layer for interacive elements 
            hr = CreateInteractionMaskingLayer(sprPages);
         }
      }
   }

   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      // Add ActionScript for interaction objects
      if (m_aInteractionAreas.GetSize() > 0)
         hr = AddActionScriptForInteractionAreasToMovie(movie);
   }

   // Changeable text objects
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      if (m_aInteractionObjects.GetSize() > 0)
         hr = CreateChangeableTextObjects(sprPages);
   }

   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      // Add ActionScript for changeable texts
      if (m_aInteractionObjects.GetSize() > 0)
         hr = AddActionScriptForChangeableTextsToMovie(movie);
   }

   // Question text objects
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      if (m_aInteractionObjects.GetSize() > 0)
         hr = CreateQuestionTextObjects(sprPages);
   }

   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      // Add ActionScript for question texts
      if (m_aInteractionObjects.GetSize() > 0)
         hr = AddActionScriptForQuestionTextsToMovie(movie);
   }

   // Dragable images
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      if (m_aInteractionObjects.GetSize() > 0)
        hr = CreateDragableImages(sprPages);
   }

   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      // Add ActionScript for dragable images
      if (m_aInteractionObjects.GetSize() > 0)
         hr = AddActionScriptForDragableImagesToMovie(movie);
   }


   // Testing
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      // Add ActionScript for Testing
      if (m_aQuestionnaires.GetSize() > 0)
         hr = AddActionScriptForTestingToMovie(movie);
   }

   // Feedback
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      // Add ActionScript for Feedback
      if (m_aQuestionnaires.GetSize() > 0)
         hr = AddActionScriptForFeedbackToMovie(movie);
   }

   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      // Draw Testing Feedback 
      // (do this after 'AddActionScriptForFeedbackToMovie()' to have a 'OK' button)
      if (m_aQuestionnaires.GetSize() > 0)
         hr = DrawTestingFeedback(sprPages);
   }

   // Save and Load Learner Progress from Suspend Data
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      // Add ActionScript for Save Load Learner Data
         hr = AddActionScriptForSaveLoadSuspendData(movie);
   }
   
   // Load Testing Data from LMS
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      // Add ActionScript for Testing
      if (m_aQuestionnaires.GetSize() > 0)
         hr = AddActionScriptForLoadTestingData(movie);
   }

   return hr;
}

void CFlashEngine::ReCheckStopmarkList()
{
   // A special case must be considered:
   // If a stopmark has exactly the same timestamp as a paging frame 
   // then the frame of the paging must be used for the stopmark frame.

   // Get all paging timestamps
   CArray<CPage*, CPage*>aPages;
   aPages.RemoveAll();
   int nFirst, nLast;
   m_pPageStream->GetPages(aPages, 0, m_iMovieLengthMs, 0, nFirst, nLast);
   int numOfPages = aPages.GetSize();

   // Now check all stopmarks 
   for (int i = 0; i < m_aFlashStopmarks.GetCount(); ++i)
   {
      int iMsStopmark = m_aFlashStopmarks.ElementAt(i).nMsTime;

      for (int k = 0; k < numOfPages; ++k)
      {
         int iMsPageBegin = aPages[k]->GetBegin();

         if (iMsStopmark == iMsPageBegin)
         {
            // Set the frame of the corresponding page to the stopmark
            _tprintf(_T("ReCheckStopmarkList(): special timestamp %d ms; Frame %d --> %d\n"), iMsStopmark, m_aFlashStopmarks.ElementAt(i).nFrame, m_aiPagingFrames[k]);
            m_aFlashStopmarks.ElementAt(i).nFrame = m_aiPagingFrames[k];
         }
      }
   }
}

int CFlashEngine::GetPagingTimeInTimeInterval(int nFromMs, int nToMs)
{
   int nPagingTimeMs = -1;

   CArray<CPage*, CPage*>aPages;
   aPages.RemoveAll();
   int nFirst, nLast;
   m_pPageStream->GetPages(aPages, nFromMs, nToMs, 0, nFirst, nLast);
   // Normal case: 1 paging --> 2 pages (one before, one after the paging)
   if (aPages.GetSize() == 2)
   {
      // Paging found --> Get the begin time of the second page
      nPagingTimeMs = aPages.ElementAt(1)->GetBegin();
   }
   else
   {
      // Special case: first page
      int nFirstPagingTime = (m_pPageStream->GetFirstPage(0, m_iMovieLengthMs, 0))->GetBegin();
      if (aPages.GetSize() == 1)
      {
         if ((nFirstPagingTime >= nFromMs) && (nFirstPagingTime <= nToMs))
         {
            // First Page found --> Get the begin time of the first page
            nPagingTimeMs = aPages.ElementAt(0)->GetBegin();
         }
      }
   }

   return nPagingTimeMs;
}

UINT CFlashEngine::GetPageTimestamps(int nLength)
{
   HRESULT hr = S_OK;

   CArray<CPage*, CPage*>aPages;

   // Get number of pages and init ActionScript variables
   aPages.RemoveAll();

   int nFirst, nLast;
   m_pPageStream->GetPages(aPages, 0, nLength, 0, nFirst, nLast);

#ifdef _DEBUG
   _tprintf(_T("WhiteboardStream: %d Pages found.\n"), aPages.GetSize());
   _tprintf(_T("Frame rate: %g \n"), m_fFrameRate);
#endif

   // Add paging timestamps (ActionScript)
   // Used to jump to page
   // Get number of pages / paging timestamps
   // and check for double frames (Bugfix 3551: pages too short)
   int iFrameBefore = -1;
   int nNumOfPages = aPages.GetSize();
   for (int i = 0; i < nNumOfPages; ++i)
   {
      int pageBeginMs = aPages[i]->GetBegin();
      float fFrame = m_fFrameRate * ((float)pageBeginMs / 1000.0f);
      int iFrame = (int)(fFrame);
      // Paging frame is the next possible frame
      int msPerFrame = (int)(1000.0f / m_fFrameRate);
      if (pageBeginMs % msPerFrame != 0)
         iFrame += 1;
      // 1st Frame is 1 (not 0) --> add one frame
      iFrame += 1;

      // Bugfix 3551
      if (iFrame == iFrameBefore) {
         // Page before is too short
         // Bugfix 5302: Can it be simply ignored? 
         // (Only possible if it does not contain any interaction)
         int pageBeforeBeginMs = (i > 0) ? (aPages[i-1]->GetBegin()) : (0);
         int pageBeforeEndMs = (i > 0) ? (aPages[i-1]->GetEnd()) : pageBeginMs; 
         bool bHasInteraction = AreInteractionsInTimeInterval(pageBeforeBeginMs, pageBeforeEndMs);
         if (bHasInteraction) {
             // "Short" page can not be ignored --> return error code
             hr = imc_lecturnity_converter_LPLibs_INTERACTIVE_PAGE_TOO_SHORT;
         } else {
             // Ignore "short" page --> return warning code
             // (but only if not an error is detected before)
             if (hr >= 0)
                hr = imc_lecturnity_converter_LPLibs_WARN_PAGE_TOO_SHORT;
         }
         // Add timestamp to the list of "too short pages times"
         if (!m_csTooShortPagesTimes.IsEmpty())
            m_csTooShortPagesTimes.Append(_T(", "));
         m_csTooShortPagesTimes.Append(GetTimeStringFromMs(pageBeginMs));
      } else {
         // Add frame to paging frames list
         m_aiPagingFrames.Add(iFrame);
      }

      iFrameBefore = iFrame;

#ifdef _DEBUG
      _tprintf(_T("Page %d: %dms, Frame %d\n"), i, pageBeginMs, iFrame);
#endif
   }
#ifdef _DEBUG
   _tprintf(_T("\n"));
#endif

   // this stuff is not used any more
   for (int p = 0; p < aPages.GetSize(); ++p)
      delete aPages[p]; // the Page objects are copies (done in GetPages())

   aPages.RemoveAll();

   return hr;
}


UINT CFlashEngine::EmbedObjects(SWFSprite *movieClip, 
                                bool bDryRun, bool bConserveObjects, 
                                int nMsecPerFrame, DWORD *dwAddedObjects)
{
   HRESULT hr = S_OK;

   int iStartId = Ming_currentIdCount();
   
   *dwAddedObjects = 0;

   // Prepare Fulltext search
   m_aFlashFulltexts.RemoveAll();

   //int nLength = m_pWhiteboardStream->GetLength();
   int nLength = m_iMovieLengthMs;
   if (nLength < 0)
      nLength = m_pWhiteboardStream->GetLength();

   if (bDryRun)
      movieClip = new SWFSprite(); // dummy movie clip

   DWORD dwVisibleObjects = 0;
   
   CArray<SWFDisplayItem*, SWFDisplayItem*> aDisplayItems;
   aDisplayItems.SetSize(m_aObjects.GetSize());
   int item = 0;
   for (item = 0; item < aDisplayItems.GetSize(); ++item)
      aDisplayItems[item] = NULL;  
   CArray<bool, bool> aActiveItems;
   aActiveItems.SetSize(m_aObjects.GetSize());

   // do some preprocessing of the objects
   // in order to delete/ignore some of them lateron
   // (these are: "temporary" marker objects and line 
   //  objects leading to a polyline (freehand)
   CArray<bool, bool> aCanBeConserved;
   int iConserveCount = 0;
   if (bConserveObjects)
   {
      aCanBeConserved.SetSize(m_aObjects.GetSize());
      
      for (item = 0; item < aCanBeConserved.GetSize(); ++item)
      {
         aCanBeConserved[item] = false;

         if (m_aObjects[item]->GetType() == DrawSdk::FREEHAND)
         {
            // do a "backward" search to see which LINEs belong to this FREEHAND
            // and can thus be ignored
            
            // double code (see below)
            DrawSdk::Polygon *pPolygonObject = (DrawSdk::Polygon *)m_aObjects[item];
            int iNodeCount = pPolygonObject->GetPoints(NULL, 0);
            
            DrawSdk::DPoint *paPoints = NULL;
            paPoints = new DrawSdk::DPoint[iNodeCount];
            pPolygonObject->GetPoints(paPoints, iNodeCount);

            int nStartIndex = iNodeCount-1;
            for (int item2=item-1; item2 >= 0; --item2)
            {
               if (m_aObjects[item2]->GetType() == DrawSdk::LINE 
                  || m_bRectanglesCorrected && m_aObjects[item2]->GetType() == DrawSdk::RECTANGLE)
               {

                  if (m_aObjects[item2]->GetType() == DrawSdk::LINE)
                  {
                     // look for the start point of this line amongst the points of the FREEHAND
                     float x = (float)m_aObjects[item2]->GetX(), y = (float)m_aObjects[item2]->GetY();

                     bool bPointFound = false;
                     for (int p=nStartIndex; p>=0; --p)
                     {
                        if ((float)paPoints[p].x == x && (float)paPoints[p].y == y)
                        {
                           bPointFound = true;
                           nStartIndex = p;
                           break;
                        }
                     }

                     if (!bPointFound)
                        break; // breaks the "for all previous objects"
                     else
                     {
                        aCanBeConserved[item2] = true;
                        ++iConserveCount;
                     }
                  }
               }
               else
                  break; // only check preceeding lines (and maybe ignore rectangles
            }
          
            delete[] paPoints;

         } // if FREEHAND
     
         if (m_aObjects[item]->GetType() == DrawSdk::POLYGON)
         {
            // look if the next object is also a MARKER/POLYGON object 
            // starting with the same points (at least three of them)
            // if so: ignore this one
            
            if (item+1 < m_aObjects.GetSize())
            {
               if (m_aObjects[item+1]->GetType() == DrawSdk::POLYGON) 
               {
                  // double code (see below)
                  DrawSdk::Polygon *pPolygonObject1 = (DrawSdk::Polygon *)m_aObjects[item];
                  int iNodeCount1 = pPolygonObject1->GetPoints(NULL, 0);
                  
                  DrawSdk::Polygon *pPolygonObject2 = (DrawSdk::Polygon *)m_aObjects[item+1];
                  int iNodeCount2 = pPolygonObject2->GetPoints(NULL, 0);


                  
                  if (iNodeCount1 > 1 && iNodeCount2 > 1 && iNodeCount1 < iNodeCount2)
                  {
                     if (pPolygonObject1->GetIsFilled() == pPolygonObject2->GetIsFilled())
                     {
                        DrawSdk::DPoint *paPoints1 = NULL;
                        paPoints1 = new DrawSdk::DPoint[iNodeCount1];
                        pPolygonObject1->GetPoints(paPoints1, iNodeCount1);
                        
                        DrawSdk::DPoint *paPoints2 = NULL;
                        paPoints2 = new DrawSdk::DPoint[iNodeCount2];
                        pPolygonObject2->GetPoints(paPoints2, iNodeCount2);
                        
                        bool bTheSame = true;
                        for (int p=0; p<min(min(iNodeCount1, iNodeCount2), 3); ++p)
                        {
                           if (paPoints1[p].x != paPoints2[p].x
                              || paPoints1[p].y != paPoints2[p].y)
                           {
                              bTheSame = false;
                              break; // breaks the for
                           }
                        }
                        
                        if (bTheSame)
                        {
                           aCanBeConserved[item] = true;
                           ++iConserveCount;
                        }
                        
                        delete[] paPoints1;
                        delete[] paPoints2;
                     }
                     
                  }
                  
               }
            }
         } // if POLYGON
         
         
      } // for all items in aCanBeConserved

      //printf("EmbedObjects(): ConserveCount %d; objects in queue %d\n", iConserveCount, aCanBeConserved.GetSize());
   }
   


   // only generate font objects once for each font (ttf) name:
   /// --> use member variable here (because of "standard font" ARIAL)
   ///CMap<CString, LPCTSTR, SWFFont*, SWFFont*> mapSwfFonts;
   CMap<CString, LPCTSTR, SWFShape*, SWFShape*> mapImages;
   CMap<SWFBlock*, SWFBlock*, float, float> mapImageScalingX;
   CMap<SWFBlock*, SWFBlock*, float, float> mapImageScalingY;
   
   
   SWFDisplayItem *diPointer = NULL;
   
   // do a "search" for events; there seems to be no "GetEvents()"
   CWhiteboardEvent *pEventBefore = NULL;
   float fLastPointerX = 0.0f, fLastPointerY = 0.0f;
   int nLastPointerType = -1; // undefined
   bool bPointerTypeHasChanged = false;
   m_iAbsoluteFramesCount = 0;
   int nLastPageNumber = -1;
   int nLastStopmarkTime = -1;
   int nLastEventTime = -1;
   bool bIsNewPage = false;

   for (int timestamp = 0; SUCCEEDED(hr) && timestamp <= nLength+nMsecPerFrame; timestamp+=nMsecPerFrame )
   {
      // "+nMsecPerFrame" in the above line: make sure it finds the last event
      
      if (m_bCancelRequested)
         break;
      
      // Check for stopmarks
      bool bStopmarkFound = false;
      int timestampStopmark = 0;

      int nNumOfStopmarks = m_aFlashStopmarks.GetSize();
      for (int i = 0; i < nNumOfStopmarks; ++i)
      {
         // Check the interval [timestamp-nMsecPerFrame .. timestamp+nMsecPerFrame]
         // for stopmarks.
         // Only 1 or 2 stopmarks in this interval can be handled. 
         // More than 2 stopmarks in this intervall leads to a loss of last stopmark(s). 

         int nStopmarkTime = m_aFlashStopmarks.ElementAt(i).nMsTime;

         if ((nStopmarkTime >= timestamp-nMsecPerFrame) 
            && (nStopmarkTime < timestamp+nMsecPerFrame))
         {
            // Drop already set stopmark
            if (nStopmarkTime != nLastStopmarkTime)
            {
               // New stopmark found
               bStopmarkFound = true;
               timestampStopmark = nStopmarkTime;
               m_aFlashStopmarks.ElementAt(i).nFrame = (m_iAbsoluteFramesCount + 1); // 1st frame is 1 (not 0)
               nLastStopmarkTime = nStopmarkTime;
               break;
            }
         }
      }


      // Get the event at the given timestamp
      CWhiteboardEvent *pEvent = NULL;
      if (bStopmarkFound && (timestampStopmark > 0))
      {
         pEvent = m_pWhiteboardStream->GetEvent(timestampStopmark);
         m_aiFrameTimestampsMs.Add(timestampStopmark);
      }
      else
      {
         pEvent = m_pWhiteboardStream->GetEvent(timestamp);
         m_aiFrameTimestampsMs.Add(timestamp);
      }
      
      // Discard double events
      if (pEvent != NULL && pEvent != pEventBefore)
      {
         nLastEventTime = pEvent->GetTimestamp();

         if (pEvent->GetPageNumber() != nLastPageNumber)
         {
            bIsNewPage = true;
            nLastPageNumber = pEvent->GetPageNumber();
            //movieClip->resetDepth(0);
         }
         else
            bIsNewPage = false;
 

         // find out which objects currently are active:
         for (item=0; item<aActiveItems.GetSize(); ++item)
            aActiveItems[item] = false;
         
         bool bPointerActive = false;
         
         CPtrArray *pObjects = pEvent->GetObjectsPointer();

         CMap<int, int, bool, bool> mapNewToThisEvent;

         m_iMaxIndexSoFar = 0;
         int object = 0;
         for (object = 0; SUCCEEDED(hr) && object < pObjects->GetSize(); ++object)
         {
            DrawSdk::DrawObject* pDrawObject = (DrawSdk::DrawObject*)pObjects->ElementAt(object);
            
            int iObjectNumber = pDrawObject->GetObjectNumber();

            bool bIndexClash = false;
            if (iObjectNumber < m_iMaxIndexSoFar)
            {
               bool bMaxNewHere = false;
               mapNewToThisEvent.Lookup(m_iMaxIndexSoFar, bMaxNewHere);
               if (bMaxNewHere)
               {
                  bIndexClash = true; // an older object is below a newer one
               }
            }
            else
               m_iMaxIndexSoFar = iObjectNumber;
            
            if (aDisplayItems[iObjectNumber] != NULL && !bIndexClash)
            {
               // do not much: object is already visible and with the right z-index
               aActiveItems[iObjectNumber] = true;
             
               // only the depth for newly added objects can be changed 
               //aDisplayItems[iObjectNumber]->setDepth(iDepthValue++);
            }
            else
            {
               if (aDisplayItems[iObjectNumber] != NULL)
               {
                  if (bIndexClash)
                  {
                     // check for and
                     // correct depth ordering for appearances under other objects
                     // 
                     // this is already visible and most likely
                     // (iObjectNumber vs. aMaxIndexSoFar) it is below but should be above
                     // remove it now and re-add it later (below)
                     
                     movieClip->remove(aDisplayItems[iObjectNumber]); // movie clip can be a dummy
                     --dwVisibleObjects;
                     aDisplayItems[iObjectNumber] = NULL;
                  }
               }
               else
               {
                  mapNewToThisEvent.SetAt(iObjectNumber, true);
               }

                                       

               //
               // add an appropriate shape to the movie clip
               //
               
               
               //DrawSdk::DrawObject *pObject = aObjects[iObjectNumber];
               DrawSdk::DrawObject *pObject = pDrawObject; // they are the same
               
               SWFShape *pShape = new SWFShape();
               SWFBlock *pBlock = pShape;
                              
               int iType = pObject->GetType();
               
               float fLineWidth = 0.0f;
               bool bDashedLine = false;
               bool bDottedDash = false;
               
               if (!bDryRun)
               {
                  // set properties common to all ColorObjects
                  if (iType != DrawSdk::IMAGE && iType != DrawSdk::TELEPOINTER)
                  {
                     DrawSdk::ColorObject *pColorObject = (DrawSdk::ColorObject *)pObject;
                     Gdiplus::ARGB ocref = pColorObject->GetOutlineColor();
                     m_argbStd = ocref;
                     fLineWidth = pColorObject->GetLineWidth();

                     pShape->setLine(fLineWidth, (m_argbStd >> 16) & 0xff, 
                        (m_argbStd >> 8) & 0xff, (m_argbStd >> 0) & 0xff, (m_argbStd >> 24) & 0xff);

                     
                     if (pColorObject->GetIsFilled())
                     {
                        Gdiplus::ARGB fcref = pColorObject->GetFillColor();
                        SWFFill *pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
                           (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
                        pShape->setRightFill(pFill);
                     }

                     int nLineStyle = pColorObject->GetLineStyle();
                     if (nLineStyle != 0)
                     {
                        bDashedLine = true;
                        bDottedDash = nLineStyle == 1;
                     }
                  }
               }
               
               // shorthand declarations
               float x = (float)(pObject->GetX());
               float y = (float)(pObject->GetY());
               float width = (float)pObject->GetWidth(); 
               float height = (float)pObject->GetHeight();

               float fImageWidth = 0.0f, fImageHeight = 0.0f; // might be needed for scaling
               
               // handle the object type
               
               switch (iType)
               {
               case DrawSdk::RECTANGLE:
                  if (!bDryRun)
                  {
                     SWFSprite *pSprRect = new SWFSprite();
                     DrawPptRectangle(pSprRect, pShape, x, y, width, height, fLineWidth, bDashedLine, bDottedDash);
                     pSprRect->nextFrame();
                     pBlock = pSprRect;
                  }
                  break;
                  
               case DrawSdk::OVAL:
                  if (!bDryRun)
                  {
                     SWFSprite *pSprOval = new SWFSprite();
                     DrawPptOval(pSprOval, pShape, x, y, width, height, fLineWidth, bDashedLine, bDottedDash);
                     pSprOval->nextFrame();
                     pBlock = pSprOval;
                  }
                  break;
                  
               case DrawSdk::LINE:
                  if (bConserveObjects && aCanBeConserved[iObjectNumber])
                  {
                     pShape = NULL;
                     pBlock = NULL;
                     break;
                  }

                  if (!bDryRun)
                  {
                     DrawSdk::Line *pLineObject = (DrawSdk::Line *)pObject;

                     // For lines and arrow heads different shapes must be used 
                     // But they must be handled as one object 
                     // --> use a local SWFSprite as a container here
                     SWFSprite *pSpriteArrowLine = new SWFSprite();

                     //DrawLine(pShape, x, y, width, height, fLineWidth, bDashedLine, bDottedDash);
                     // If we have arow heads, we have to do some corrections before drawing the line
                     // --> DrawLine is called in "DrawLineAndArrowHeads()" below
                     
                     int nArrowStyle = pLineObject->GetArrowStyle();
                     int nArrowConfig = pLineObject->GetArrowConfig();

                     // Downwards compatibility to LECTURNITY versions < 1.7.0.p6: 
                     // Default values:
                     // arrow type = 1 (filled triangle), 
                     // arrow size = 5 (normal).
                     if ((nArrowStyle > 0) && (nArrowConfig == 0))
                     {
                        switch (nArrowStyle)
                        {
                           case 1:  nArrowConfig = 15;
                                    break;
                           case 2:  nArrowConfig = 1500;
                                    break;
                           case 3:  nArrowConfig = 1515;
                                    break;
                        }
                     }
                     
                     // Corrections for lines outside the whiteboard (Bugfix 3569)
                     float x1 = x;
                     float y1 = y;
                     float width1 = width;
                     float height1 = height;
                     float dw = 0.5f * fLineWidth;
                     if (x1 < dw) x1 = dw;
                     if (y1 < dw) y1 = dw;
                     if (x1 > (m_iWbInputWidth-dw)) x1 = (m_iWbInputWidth-dw);
                     if (y1 > (m_iWbInputHeight-dw)) y1 = (m_iWbInputHeight-dw);
                     float x2 = x + width;
                     float y2 = y + height;
                     if (x2 < dw) width1 = dw - x1;
                     if (y2 < dw) height1 = dw - y1;
                     if (x2 > (m_iWbInputWidth-dw)) width1 = (m_iWbInputWidth-dw) - x1;
                     if (y2 > (m_iWbInputHeight-dw)) height1 = (m_iWbInputHeight-dw) - y1;
                     if (sign(width1) == -sign(width)) width1 = 0.0f;
                     if (sign(height1) == -sign(height)) height1 = 0.0f;

                     // Draw line and add arrow heads (if any)
                     DrawLineAndArrowHeads(pSpriteArrowLine, nArrowStyle, nArrowConfig, x1, y1, width1, height1, 
                        fLineWidth, bDashedLine, bDottedDash, pLineObject->GetOutlineColor());

//                     // fortunately DrawSdk::Line() already 
//                     // did direction and ori handling for us
//                     if (nArrowStyle == 1 || nArrowStyle == 3)
//                        DrawArrowHead(pShape, nArrowConfig, x, y, width, height, 
//                           pLineObject->GetLineWidth(), pLineObject->GetOutlineColor());
//                     if (nArrowStyle == 2 || nArrowStyle == 3)
//                        DrawArrowHead(pShape, nArrowConfig, x+width, y+height, -width, -height, 
//                           pLineObject->GetLineWidth(), pLineObject->GetOutlineColor());
                     
                     pSpriteArrowLine->add(pShape);

                     if (m_pShpArrowEnd != NULL)
                     {
                        pSpriteArrowLine->add(m_pShpArrowEnd);
                        m_pShpArrowEnd = NULL;
                     }
                     if (m_pShpArrowBegin != NULL)
                     {
                        pSpriteArrowLine->add(m_pShpArrowBegin);
                        m_pShpArrowBegin = NULL;
                     }

                     pSpriteArrowLine->nextFrame();

                     if (pSpriteArrowLine != NULL)
                        pBlock = pSpriteArrowLine;
                     else
                        pBlock = pShape;
                     
                  }
                  break;
                  
               case DrawSdk::TEXT:
                  {
                     // Note: Text does not use a normal SWFShape
                     SWFText *pSwfText = new SWFText();
                     
                     // Bugfix 3617/4349 (Underlined text): 
                     // Text and underline must be handled as one object 
                     // --> use a local SWFSprite as a container here
                     SWFSprite *pSpriteText = NULL;
                     
                     if (!bDryRun)
                     {
                        DrawSdk::Text *pTextObject = (DrawSdk::Text *)pObject;

                        hr = MapFlashFontFromTextObject(pTextObject);

                        // Fulltext search: Get the correct timestamp
                        int timeMs = timestamp;
                        if (bIsNewPage)
                        {
                           timeMs = GetPagingTimeInTimeInterval((timestamp-nMsecPerFrame), timestamp);
                           if (timeMs < 0)
                              timeMs = timestamp;
                        }
                        // Add the text to the Fulltext search
                        FLASH_FULLTEXT flashFulltext;
                        flashFulltext.nMsTime = timeMs;
                        flashFulltext.pTextObject = pTextObject;
                        m_aFlashFulltexts.Add(flashFulltext);

                        LOGFONT lf = pTextObject->GetLogFont();
                        // Get font styles from LOGFONT
                        BOOL bLogFontIsItalic = lf.lfItalic; 
                        BOOL bLogFontIsBold = (lf.lfWeight >= 700) ? TRUE : FALSE;
                        BOOL bLogFontIsUnderlined = lf.lfUnderline;
                        if (bLogFontIsUnderlined)
                           pSpriteText = new SWFSprite();

                        CString csTtfIn = pTextObject->GetTtfName();
                        if (csTtfIn.IsEmpty())
                        {
                           // if no ttf is specified use the face name for the hash key
                           csTtfIn = lf.lfFaceName;
                        }
                        SWFFont *pSwfFont = GetFlashFontFromTextObject(pTextObject);

                        if (pSwfFont)
                        {
                           byte iFlags = pSwfFont->getFlags();
                           // Get font styles from SWFFont
                           BOOL bSwfFontIsItalic = ((iFlags & SWF_FONT_ISITALIC) != 0) ? TRUE : FALSE;
                           BOOL bSwfFontIsBold = ((iFlags & SWF_FONT_ISBOLD) != 0) ? TRUE : FALSE;

                           //_tprintf(_T("ooo %s LogFont: i%d b%d - SWFFont: i%d b%d\n"), lf.lfFaceName, bLogFontIsItalic, bLogFontIsBold, bSwfFontIsItalic, bSwfFontIsBold); 
                           if ( (bLogFontIsItalic != bSwfFontIsItalic) 
                              || (bLogFontIsBold != bSwfFontIsBold) )
                           {
                              // Add font name to font list
                              m_mapFontNamesStylesNotSupported.SetAt(lf.lfFaceName, NULL);

                              if ((m_hrPossibleWarning & imc_lecturnity_converter_LPLibs_WARN_FONTSTYLES_IN_TTF) 
                                  != imc_lecturnity_converter_LPLibs_WARN_FONTSTYLES_IN_TTF)
                              {
                                 m_hrPossibleWarning += imc_lecturnity_converter_LPLibs_WARN_FONTSTYLES_IN_TTF;
                              }
                           }

                           Gdiplus::ARGB crFontColor = pTextObject->GetOutlineColor();
                           CString csText = pTextObject->GetString();
                           float fFontSize = -1.0f * pTextObject->GetLogFont().lfHeight;
                           
                           if (fFontSize > 0)
                           {
                              DrawText(pSwfText, x, y, pSwfFont, csText, fFontSize, crFontColor);

                              if (bLogFontIsUnderlined)
                              {
                                 pSpriteText->add(pSwfText);

                                 float fLength = (float)pTextObject->GetWidth();
                                 DrawUnderline(pSpriteText, x, y, fLength, lf, crFontColor);

                                 pSpriteText->nextFrame();
                              }
                           }
                           // else negative font sizes lead to really big texts
                        }
                     }
                     
                     if (pSpriteText != NULL)
                        pBlock = pSpriteText;
                     else
                        pBlock = pSwfText;
                  }
                  break;

               case DrawSdk::MARKER:
                  if (bConserveObjects)
                  {
                     // there is no distinction (anymore) between MARKER
                     // and POLYGON; the type MARKER is never returned
                     // so type "MARKER" gets handled below
                  }
               case DrawSdk::POLYGON:
                  if (bConserveObjects && aCanBeConserved[iObjectNumber])
                  {
                     pBlock = NULL;
                     pShape = NULL;
                     break;
                  }
               case DrawSdk::FREEHAND:
                  if (!bDryRun)
                  {
                     SWFSprite *pSpritePoly = NULL;

                     // double code (see above)
                     DrawSdk::Polygon *pPolygonObject = (DrawSdk::Polygon *)pObject;
                     int iNodeCount = pPolygonObject->GetPoints(NULL, 0);
                     
                     DrawSdk::DPoint *paPoints = NULL;
                     paPoints = new DrawSdk::DPoint[iNodeCount];
                     pPolygonObject->GetPoints(paPoints, iNodeCount);

                     bool bPolylineIsSinglePoint = false;
                        
                     if (iNodeCount > 1)
                     {
                        pSpritePoly = new SWFSprite();
                        pSpritePoly->add(pShape);

                        // Check, if all points are in fact on the same point
                        int point = 1;
                        double x1 = paPoints[0].x;
                        double y1 = paPoints[0].y;
                        bPolylineIsSinglePoint = true;
                        while ((point < iNodeCount) && (bPolylineIsSinglePoint == true))
                        {
                           double x2 = paPoints[point].x;
                           double y2 = paPoints[point].y;

                           if ((x1 != x2) || (y1 != y2))
                              bPolylineIsSinglePoint = false;

                           point++;
                        }

                        if (bPolylineIsSinglePoint == false)
                        {
                           for (point=1; point<iNodeCount; ++point)
                           {
                              float x1 = (float)(paPoints[point-1].x);
                              float y1 = (float)(paPoints[point-1].y);
                              float x2 = (float)(paPoints[point].x);
                              float y2 = (float)(paPoints[point].y);

                              // Corrections for lines outside the whiteboard (Bugfix 3569)
                              float dw = 0.5f * fLineWidth;
                              if (x1 < dw) x1 = dw;
                              if (y1 < dw) y1 = dw;
                              if (x2 < dw) x2 = dw;
                              if (y2 < dw) y2 = dw;
                              if (x1 > (m_iWbInputWidth-dw)) x1 = (m_iWbInputWidth-dw);
                              if (y1 > (m_iWbInputHeight-dw)) y1 = (m_iWbInputHeight-dw);
                              if (x2 > (m_iWbInputWidth-dw)) x2 = (m_iWbInputWidth-dw);
                              if (y2 > (m_iWbInputHeight-dw)) y2 = (m_iWbInputHeight-dw);

                              DrawLineTo(pShape, x1, y1, x2, y2, 
                                         fLineWidth, bDashedLine, bDottedDash, false);
                           }
                        
                           if (pPolygonObject->GetIsFilled()) // make it a closed one
                           {
                              DrawLineTo(pShape,
                                         (float)(paPoints[iNodeCount-1].x), (float)(paPoints[iNodeCount-1].y),
                                         (float)(paPoints[0].x), (float)(paPoints[0].y), 
                                         fLineWidth, bDashedLine, bDottedDash, false);
                           }
                        
                           /*
                              //using Curves (at least these quadratic ones doesn't look better
                              for (int point=2; point<iNodeCount; point+=2)
                              pShape->drawCurveTo(
                              paPoints[point-1].x, paPoints[point-1].y,
                              paPoints[point].x, paPoints[point].y);
                           */
                        }

                        // Bugfix 5343 (correction of Bugfix 4361):
                        // PPT styled line for start and end point
                        // Exceptions: 
                        // - Filled polygons
                        // - Lines with transparency (e.g. Marker lines)
                        if ( (iNodeCount >= 2) && !pPolygonObject->GetIsFilled() && ((m_argbStd >> 24) == 255) ) {
                           // Start point:
                           // Draw the shape with a hairline
                           DrawLineTo(pShape,
                                      (float)(paPoints[0].x), (float)(paPoints[0].y),
                                      (float)(paPoints[1].x), (float)(paPoints[1].y), 
                                      fLineWidth, bDashedLine, bDottedDash, true);
                           // Draw the PPT styled line
                           DrawPptLineTo(pSpritePoly,
                                         (float)(paPoints[0].x), (float)(paPoints[0].y),
                                         (float)(paPoints[1].x), (float)(paPoints[1].y), 
                                         fLineWidth, bDashedLine, bDottedDash);
                           // End point:
                           // Draw the shape with a hairline
                           DrawLineTo(pShape,
                                      (float)(paPoints[iNodeCount-2].x), (float)(paPoints[iNodeCount-2].y),
                                      (float)(paPoints[iNodeCount-1].x), (float)(paPoints[iNodeCount-1].y), 
                                      fLineWidth, bDashedLine, bDottedDash, true);
                           // Draw the PPT styled line
                           DrawPptLineTo(pSpritePoly,
                                         (float)(paPoints[iNodeCount-2].x), (float)(paPoints[iNodeCount-2].y),
                                         (float)(paPoints[iNodeCount-1].x), (float)(paPoints[iNodeCount-1].y), 
                                         fLineWidth, bDashedLine, bDottedDash);
                        }

                        pSpritePoly->nextFrame();
                     }
                     
                     if ((iNodeCount == 1) || (bPolylineIsSinglePoint == true))
                     {
                        // Draw a filled square here sourrounded by a hairline 
                        Gdiplus::ARGB fcref = pPolygonObject->GetOutlineColor();
                        SWFFill *pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
                           (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
                        pShape->setRightFill(pFill);
                        pShape->setLine(0, (fcref >> 16)  & 0xff, (fcref >> 8) & 0xff, 
                           (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);

                        DrawRectangle(pShape, 
                        (float)(paPoints[0].x) - (0.5f*fLineWidth), 
                        (float)(paPoints[0].y) - (0.5f*fLineWidth), 
                        fLineWidth, fLineWidth,
                        0.0f, bDashedLine, bDottedDash);
                     }

                     delete[] paPoints;

                     if (pSpritePoly != NULL)
                        pBlock = pSpritePoly;
                     else
                        pBlock = pShape;
                  }
                  break;
                  
               case DrawSdk::TELEPOINTER:
                  bPointerActive = true;
                  
                  if (!bDryRun)
                  {
                     // Get the Pointer type before drawing the shape
                     DrawSdk::Pointer *pPointerObject = (DrawSdk::Pointer *)pObject;
                     int nPointerType = pPointerObject->IsMousePointer() ? 
                         imc_lecturnity_converter_LPLibs_TELEPOINTER_TYPE_CURSOR 
                         : imc_lecturnity_converter_LPLibs_TELEPOINTER_TYPE_POINTER;
                     bPointerTypeHasChanged = false;
                     if (nPointerType != nLastPointerType) {
                        bPointerTypeHasChanged = true;
                        // Reset diPointer
                        diPointer = NULL;
                     }

                     if (diPointer == NULL) {
                        DrawPointer(pShape, x, y, max(width, height), nPointerType);
                     } else {
                        // Move the Pointer only
                        diPointer->move(x-fLastPointerX, y-fLastPointerY);
                        // strange: moveTo seems to work like move; at least here
                     }
                     
                     fLastPointerX = x; fLastPointerY = y;
                     nLastPointerType = nPointerType;
                  }
                  break;
                  
               case DrawSdk::IMAGE:
                  if (!bDryRun)
                  {
                     DrawSdk::Image *pImageObject = (DrawSdk::Image *)pObject;

                     hr = MapFlashImageFromImageObject(pImageObject, &pShape, &pBlock, 
                        mapImages, mapImageScalingX, mapImageScalingY);
                  }
                  break;
                  
               default:
                  // do nothing
                  break;
               }
               
               
               
               if (SUCCEEDED(hr) && (pBlock))
               {
                  SWFDisplayItem *di = NULL;
                  {
                     if (pObject->GetType() != DrawSdk::TELEPOINTER || diPointer == NULL)
                     {
                        // must be done (even if bDryRun is set) in order to have object
                        // tracking (via the aDisplayItems array) working
                        (*dwAddedObjects)++;
                        di = movieClip->add(pBlock); // movie clip can be a dummy

                        if (di->item == NULL)
                        {
                           // cannot add object to movie:
                           hr = imc_lecturnity_converter_LPLibs_DOCUMENT_TOO_COMPLEX;
                           break;
                        }

                        di->setDepth(*dwAddedObjects); // ignore object depths of previously dry runs
                        ++dwVisibleObjects;
                     }
                     else
                     {
                        // diPointer only needs to be moved (done above)
                     }
                  }
                  
                  if (pObject->GetType() == DrawSdk::TELEPOINTER)
                  {
                     if (diPointer == NULL)
                     {
                        diPointer = di;
                        
                        // always have it on top of things (but not border and control)
                        diPointer->setDepth(15898);
                     }
                     else
                        di = diPointer;
                  }
                  
                  if (pObject->GetType() == DrawSdk::IMAGE)// || pObject->GetType() == DrawSdk::TEXT)
                  {
                     if (!bDryRun)
                     {
                        // Images have to be moved to their correct positions
                        // might be a bug in ming:
                        // if you make the DrawRectangle with an image fill style 
                        // with coordinates other than 0,0 then the image is misplaced (and tiled)
///                        di->moveTo(x, y);
                        di->moveTo(x, y);
                        
                        // it is possible (namely with images added in the Assistant)
                        // that the image has other dimensions than is
                        // specified in the object queue
///                        float fFactorX = 1.0f;
///                        float fFactorY = 1.0f;
///                        if (mapImageScalingX.Lookup(pBlock, fFactorX) && mapImageScalingY.Lookup(pBlock, fFactorY))
///                           di->scaleTo(fFactorX, fFactorY);
                        float fFactorX = 1.0;
                        float fFactorY = 1.0;
                        if (mapImageScalingX.Lookup(pBlock, fFactorX) && mapImageScalingY.Lookup(pBlock, fFactorY))
                           di->scaleTo(fFactorX, fFactorY);
                           
                     }
                  }
                  
                  
                  aDisplayItems[iObjectNumber] = di;
                  aActiveItems[iObjectNumber] = true;
               }
               
            } // aDisplayItems[iObjectNumber] == NULL // object is not already visible
            
            
         } // for all objects in current event
         
         // remove all objects which are not active in the current event
         for (object = 0; object < aDisplayItems.GetSize(); ++object)
         {
            if (aDisplayItems[object] != NULL && aActiveItems[object] == false)
            {
               if (m_aObjects[object]->GetType() != DrawSdk::TELEPOINTER)
               {
                  movieClip->remove(aDisplayItems[object]); // movie clip can be a dummy
                  --dwVisibleObjects;
                  //_tprintf(_T("r #%d t%d\n"), m_aObjects[object]->GetObjectNumber(), m_aObjects[object]->GetType());
               }
               // else will be handled below
               
               aDisplayItems[object] = NULL;
            }
         }
         if (diPointer != NULL && (!bPointerActive || bPointerTypeHasChanged))
         {
            movieClip->remove(diPointer); // movie clip can be a dummy
            --dwVisibleObjects;
            diPointer = NULL;
         }
         
         
         pEventBefore = pEvent;
      }
      // else pEvent == pEventBefore
      
      if ((pEvent != NULL) || (timestamp <= nLength))
      {
         movieClip->nextFrame(); // movie clip can be a dummy
         ++m_iAbsoluteFramesCount;
      }
   }

   // is needed in case of short(er) event queue files produced from the Editor
///   movieClip->setFrames(m_iAbsoluteFramesCount); 
   if (((m_iMovieLengthMs * m_fFrameRate) / 1000) > m_iAbsoluteFramesCount)
   {
      // Add some frames until the end of the recording
      int nDiffFrames = ((m_iMovieLengthMs * m_fFrameRate) / 1000) - m_iAbsoluteFramesCount;
      for (int i = 0; i < nDiffFrames; ++i)
         movieClip->nextFrame();
   }

   // Determine the number of frames: 
   // --> use the bigger value of a) record length or b) last event timestamp
   int nNewLength = (nLength > nLastEventTime) ? nLength : nLastEventTime;
   int nNewFrameCount = (int)(m_fFrameRate * (float)nNewLength  / 1000.0f);
   int msPerFrame = (int)(1000.0f / m_fFrameRate);
   if (nNewLength % msPerFrame != 0)
      nNewFrameCount += 1;
   // Note: first frame is frame 1 (not 0) --> add one frame here
   nNewFrameCount += 1;
   
   //movieClip->setFrames(nNewFrameCount); 

   if (bDryRun)
      Ming_reset(iStartId); // make sure the id of objects start with a low value

     
   if (SUCCEEDED(hr) && m_hrPossibleWarning != S_OK)
      return m_hrPossibleWarning;
   else
      return hr;
}

UINT CFlashEngine::MapFlashImageFromImageObject(DrawSdk::Image *pImageObject, SWFShape **pShape, SWFBlock **pBlock, 
                                                CMap<CString, LPCTSTR, SWFShape*, SWFShape*> &mapImages,
                                                CMap<SWFBlock*, SWFBlock*, float, float> &mapImageScalingX,
                                                CMap<SWFBlock*, SWFBlock*, float, float> &mapImageScalingY)
{
   HRESULT hr = S_OK;

   CString csImageIn, csImageOut;
   csImageIn = pImageObject->GetImageName();
   if (pImageObject->IsDndMoveable())
   {
      CString csNewImageName;
      GetNewDragableImageName(pImageObject, csNewImageName);
      csImageIn.Format(_T("%s"), csNewImageName);
   }

   SWFShape *pBitmapShape = NULL;
   if (!mapImages.Lookup(csImageIn, pBitmapShape))
   {
      if (m_mapImageNames.Lookup(csImageIn, csImageOut))
      {   
         FILE *pFile = NULL;
         SWFBitmap *pBitmap = NULL;
         // Bugfix 5324:
         bool bUseAlternativeBitmapping = false;

         if (m_mapBitmaps.Lookup(csImageOut, pBitmap) && (pBitmap != NULL))
         {
            // There already exists a SWFBitmap for this image
            // (and also an open file handler)
            // --> nothing to do here
         }
         else
         {
            //if (NULL == (pFile = _tfopen(csImageOut, _T("rb"))))
            // (Bugfix 5324:) Using _tfopen_s instead of _tfopen allows 
            // the determination of the error code, 
            // e.g. EMFILE (=24) for "too many open files". 
            // (512 open files is maximum, error message appears at 506 open images files)
            errno_t err;
            if( (err = _tfopen_s( &pFile, csImageOut, _T("rb") )) != 0 )
            {
                _tprintf(_T("\nError code: %d (after image number %d)\n"), err, mapImages.GetCount());
                if (err == EMFILE)
                    hr = imc_lecturnity_converter_LPLibs_TOO_MANY_OPEN_FILES;
                else
                    hr = imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
            
               _TCHAR tszMessage[1024];
               _stprintf(tszMessage, _T("Error: Cannot open image file! %s\n"), csImageOut);
               _tprintf(tszMessage);
            }
            if (SUCCEEDED(hr))
            {
               // Bugfix 5324: 
               // Maximum of 512 (506) open image files: 
               // Use an alternative way above 460 open image files
               int nCurentOpenImageFiles = m_mapBitmaps.GetSize();
               if (nCurentOpenImageFiles > 460)
                  bUseAlternativeBitmapping = true;
               
               if (!bUseAlternativeBitmapping) {
                  // Use the "normal" way: create SWFBitmap from the file pointer
                  pBitmap = new SWFBitmap(pFile, csImageOut.Right(4) == _T(".dbl"));
               } else {
                  // Alternative way: read data into buffer before putting it to SWFBitmap
                  pBitmap = new SWFBitmap(CT2CA(csImageOut), true);
               }

               m_mapBitmaps.SetAt(csImageOut, pBitmap);

               // Do not free the buffer - otherwise the images get lost
               //free(buf);
            }
         }
      
         if (SUCCEEDED(hr))
         {
         
            // Note: Image does not use a normal SWFShape
            DrawImage(*pShape, pBitmap);

            FILE *pDummy = NULL;
            if (!m_mapTempFiles.Lookup(csImageOut, pDummy))
            {
               // this file shouldn't be deleted, but the file handle 
               // should be closed
               // "instruct" Cleanup() to do so
               csImageOut = _T("!") + csImageOut;
            }
         
            m_mapTempFiles.SetAt(csImageOut, pFile);
         
            // Problem: maybe the same file can be opened n-times, so n-times handles are open
            CString csImageOutHandle;
            csImageOutHandle.Format(_T("!%s_0x%08x"), csImageOut, pFile),
            m_mapTempFiles.SetAt(csImageOutHandle, pFile);

            mapImages.SetAt(csImageIn, *pShape);

            float width = (float)pImageObject->GetWidth();
            float height = (float)pImageObject->GetHeight();
            float fImageWidth = pBitmap->getWidth();
            float fImageHeight = pBitmap->getHeight();
            mapImageScalingX.SetAt(*pShape, width/fImageWidth);
            mapImageScalingY.SetAt(*pShape, height/fImageHeight);
   
            //pBlock = pBitmapShape;

            if (bUseAlternativeBitmapping) {
               // Bugfix 5324:
               // Close the file handler
               if (pFile)
                  fclose(pFile);
            }
         }
      }
   }
   else
   {
      // only open and create images once
      *pBlock = pBitmapShape;
   }

   return hr;
}

UINT CFlashEngine::MapFlashFontFromTextObject(DrawSdk::Text *pTextObject)
{
   HRESULT hr = S_OK;

   LOGFONT lf = pTextObject->GetLogFont();
   CString csTtfIn = pTextObject->GetTtfName();
   if (csTtfIn.IsEmpty())
   {
      // if no ttf is specified use the face name for the hash key
      csTtfIn = lf.lfFaceName;
   }
   SWFFont *pSwfFont = NULL;

   if (!m_mapSwfFonts.Lookup(csTtfIn, pSwfFont))
   {
      CString csFdbOut;
      m_mapFontNames.Lookup(csTtfIn, csFdbOut);
      
      if (csFdbOut != _T(""))
      {
         FILE *pFile;
         if (NULL == (pFile = _tfopen(csFdbOut, _T("rb"))))
         {
            hr = imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
            
            _TCHAR tszMessage[1024];
            _stprintf(tszMessage, _T("Error: Cannot open font file! %s\n"), csFdbOut);
            _tprintf(tszMessage);
         }
         
         if (SUCCEEDED(hr))
         {
            pSwfFont = new SWFFont(pFile);
            m_mapSwfFonts.SetAt(csTtfIn, pSwfFont);
            m_mapTempFiles.SetAt(csFdbOut, pFile);
            
         }
      }
      else
      {
         // possible reasons:
         // * "ConvertFonts()" was not invoked; use standard font
         // * not ttf was specified; issue warning, use standard font
         // * the font conversion failed; then we shouldn't get here

         if (csTtfIn.Right(4).CompareNoCase(_T(".ttf")) != 0)
         {
            _tprintf(_T("Warning: Font %s not found! Maybe TTF was not specified\n"), csTtfIn);
            m_csErrorDetail += csTtfIn;
            m_csErrorDetail += _T("\n");

            // Add font name to font list
            m_mapFontNamesFontNotFound.SetAt(lf.lfFaceName, NULL);

            if ((m_hrPossibleWarning & imc_lecturnity_converter_LPLibs_WARN_FONT_NOT_FOUND) 
                != imc_lecturnity_converter_LPLibs_WARN_FONT_NOT_FOUND)
            {
               m_hrPossibleWarning += imc_lecturnity_converter_LPLibs_WARN_FONT_NOT_FOUND;
            }
         }

         pSwfFont = m_pFntStd;
         m_mapSwfFonts.SetAt(csTtfIn, pSwfFont);
      }
      // else: most likely 
   }

   return hr;
}

SWFFont* CFlashEngine::GetFlashFontFromTextObject(DrawSdk::Text *pTextObject)
{
   SWFFont *pSwfFont = NULL;

   CString csTtfIn = pTextObject->GetTtfName();
   if (csTtfIn.IsEmpty())
   {
      // if no ttf is specified use the face name for the hash key
      LOGFONT lf = pTextObject->GetLogFont();
      csTtfIn = lf.lfFaceName;
   }

   if (!m_mapSwfFonts.Lookup(csTtfIn, pSwfFont))
      return NULL;

   return pSwfFont;
}

CQuestionnaire* CFlashEngine::GetQuestionnaireFromStartMs(int nStartMs)
{
   CQuestionnaire *pQuestionnaire = NULL;

   for (int i = 0; i < m_aQuestionnaires.GetSize(); ++i)
   {
      CQuestionnaire *pQQ = m_aQuestionnaires.ElementAt(i);
      CPoint activityTime;
      HRESULT hr = pQQ->GetActivityTimes(&activityTime);
      if ((SUCCEEDED(hr)) && (nStartMs >= activityTime.x) && (nStartMs <= activityTime.y))
      {
         pQuestionnaire = pQQ;
         break;
      }
   }

   return pQuestionnaire;
}

UINT CFlashEngine::GetAudioInfos(CString csAudioFile, int *iSampleRate, int *iAudioLengthMs)
{
   if (iSampleRate == NULL ||
       iAudioLengthMs == NULL)
      return E_INVALIDARG;

   // The code below is some nice piece of DirectX code which
   // does what we want: Read out the sampling rate and finding the
   // length of the audio stream. But DirectX is not necessarily
   // supported by Windows NT, so we have to do it old-style.
   /*
   HRESULT hr = S_OK;

   ::CoInitialize(NULL);
   {
      CComPtr<IMediaDet> pDet = NULL;
      hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_ALL, 
         IID_IMediaDet, (void **) &pDet);
      if (SUCCEEDED(hr))
         hr = pDet->put_Filename(_bstr_t(csAudioFile));
      if (SUCCEEDED(hr))
         hr = pDet->put_CurrentStream(0);
      CMediaType streamType;
      if (SUCCEEDED(hr))
         hr = pDet->get_StreamMediaType(&streamType);
      if (SUCCEEDED(hr))
      {
         WAVEFORMATEX *pWfe = (WAVEFORMATEX *) streamType.Format();
         *iSampleRate = pWfe->nSamplesPerSec;
         _tprintf(_T("Audio sampling rate: %d Hz\n"), pWfe->nSamplesPerSec);
      }
      if (SUCCEEDED(hr))
      {
         double dLength = 0.0;
         hr = pDet->get_StreamLength(&dLength);
         *iAudioLengthMs = (int) (dLength * 1000.0 + 0.5);
         _tprintf(_T("Audio stream length: %d ms\n"), *iAudioLengthMs);
      }
   }
   ::CoUninitialize();
   */

   HRESULT hr = S_OK;
   ::AVIFileInit();

   PAVIFILE pAviFile = NULL;
   if (S_OK == hr)
      hr = ::AVIFileOpen(&pAviFile, csAudioFile, OF_READ | OF_SHARE_DENY_NONE, NULL);
   PAVISTREAM pAviStream = NULL;
   if (0 == hr)
      hr = ::AVIFileGetStream(pAviFile, &pAviStream, streamtypeAUDIO, 0);
   else
      hr = E_FAIL;
   if (0 == hr)
   {
      LONG lLength = ::AVIStreamLengthTime(pAviStream);
      if (lLength != -1)
         *iAudioLengthMs = lLength;
      else
         hr = E_FAIL;
   }
   else
      hr = E_FAIL;

   if (0 == hr)
   {
      AVISTREAMINFO aviStreamInfo;
      ZeroMemory(&aviStreamInfo, sizeof AVISTREAMINFO);
      hr = ::AVIStreamInfo(pAviStream, &aviStreamInfo, sizeof AVISTREAMINFO);
      if (0 == hr)
      {
         *iSampleRate = aviStreamInfo.dwRate / aviStreamInfo.dwScale;
      }
      else
         hr = E_FAIL;
   }

   if (pAviStream)
   {
      ::AVIStreamRelease(pAviStream);
      pAviStream = NULL;
   }

   if (pAviFile)
   {
      ::AVIFileRelease(pAviFile);
      pAviFile;
   }

   ::AVIFileExit();

   return hr;
}

UINT CFlashEngine::Cancel()
{
   m_bCancelRequested = true;

   // Cleanup() should be called by the calling thread (java) lateron
 
   return S_OK;
}


UINT CFlashEngine::Cleanup()
{
   m_bCleanupCalled = true;

   HRESULT hr = S_OK;

   CString csFileName;
   FILE *pFile = NULL;

   POSITION pos = m_mapTempFiles.GetStartPosition();
   // close all handles first
   while(pos != NULL)
   {
      m_mapTempFiles.GetNextAssoc(pos, csFileName, pFile);
     
      if (pFile)
         fclose(pFile);
   }

   pos = m_mapTempFiles.GetStartPosition();
   // then delete the files
   while(pos != NULL)
   {
      m_mapTempFiles.GetNextAssoc(pos, csFileName, pFile);

      if (!(csFileName.Find(_T("!")) == 0))
      {
         LFile lFile(csFileName);
         
         DWORD dwResult = 0;
         //if (lFile.Exists())
         dwResult = lFile.Delete();

         //if (lFile.Exists())
         //   hr = E_FAIL; // WARNING_FILE_NOT_DELETED?
      }
      // else the file is jpg and shouldn't be deleted (see above)
   }
   m_mapTempFiles.RemoveAll();

   SWFDestructable::CleanupAll();
  
  
   return hr;
}


DWORD CFlashEngine::CommandCall(_TCHAR* pszCommandLine) /* private */ 
{
   HRESULT hr = S_OK;

   // Debug info
   _tprintf(_T("- Command Call: %s\n"), pszCommandLine);

   STARTUPINFO StartupInfo;
   ZeroMemory(&StartupInfo, sizeof STARTUPINFO);
   StartupInfo.cb = sizeof STARTUPINFO;
   StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
   StartupInfo.wShowWindow = SW_HIDE;

   PROCESS_INFORMATION ProcessInformation;
   ZeroMemory(&ProcessInformation, sizeof PROCESS_INFORMATION);

   DWORD dwResult = ::CreateProcess(NULL, pszCommandLine, 
                                    NULL, NULL, FALSE, 0, NULL, NULL, 
                                    &StartupInfo, &ProcessInformation);


   if (dwResult == 0) // Failure
   {
      DWORD dwError = ::GetLastError();
      _TCHAR tszMessage[1024];
      _stprintf(tszMessage, _T("! Starting failed.\n  Error: 0x%08x\n"), dwError);
      _tprintf(tszMessage);
      
      hr = imc_lecturnity_converter_LPLibs_COMMAND_START_FAILED;
   }
   else
      ::WaitForSingleObject(ProcessInformation.hProcess, INFINITE);

   if (SUCCEEDED(hr))
   {
      // at least starting the program worked, did the program itself report no error?

      DWORD dwExitCode = 0;
      dwResult = ::GetExitCodeProcess(ProcessInformation.hProcess, &dwExitCode);

      if (dwResult == 0 || dwExitCode != 0) // Failure
      {
         DWORD dwError = ::GetLastError();

         _TCHAR tszMessage[1024];
         _stprintf(tszMessage, _T("! Program failed.\n  Error: 0x%08x  Exit code: 0x%08x\n"), dwError, dwExitCode);
         _tprintf(tszMessage);
      
         hr = imc_lecturnity_converter_LPLibs_COMMAND_EXECUTION_FAILED;
      }
   }

   //_TCHAR tszMessage[1024];
   //_stprintf(tszMessage, _T("Files gelöscht: %d"), nCounter);
   //::MessageBox(NULL, tszMessage, NULL, MB_OK);

   return hr;
}

UINT CFlashEngine::PrepareScorm()
{
   if (m_pWhiteboardStream == NULL)
      return E_FAIL;

   // Add text strings
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> drawObjects;
   m_pWhiteboardStream->GetObjectArray(drawObjects);

   int nCount = drawObjects.GetSize();
   for (int i=0; i<nCount; ++i)
   {
      DrawSdk::DrawObject *pObject = drawObjects[i];
      if (pObject->GetType() == DrawSdk::TEXT)
      {
         DrawSdk::Text *pText = (DrawSdk::Text *) pObject;
         Scorm_AddFullTextString(pText->GetString());
      }
   }

   // Add "openFile" related file names to the resource file list (--> imsmanifest)
   POSITION pos = m_mapInteractiveOpenedFileNames.GetStartPosition();
   while(pos != NULL)
   {
      CString csKey, csFileName;
      m_mapInteractiveOpenedFileNames.GetNextAssoc(pos, csKey, csFileName);
      Scorm_AddResourceFile(csFileName, csKey);
   }

   return S_OK;
}

int CFlashEngine::GetListOfInteractiveOpenedFiles(CString* acsFileList)
{
   // This method returns the number of interactive opened files
   // which can be used to initialize an CString Array with the correct size 
   // if the method is called with NULL as argument.

   int idx = 0;

   for (int i = 0; i < m_aInteractionAreas.GetSize(); ++i)
   {
      if (!m_aInteractionAreas.ElementAt(i)->IsSupport())
      {
         CString csAction = m_aInteractionAreas.ElementAt(i)->GetMouseClickAction();

         if ( (csAction.Find(_T("open-url")) == 0) || (csAction.Find(_T("open-file")) == 0))
         {
            if (acsFileList != NULL)
            {
               CString csArgument;
               if (csAction.Find(_T("open-url")) == 0)
                  csArgument= (csAction.GetLength() >= 8) ? csAction.Mid(9) : _T("");
               else if (csAction.Find(_T("open-file")) == 0)
                  csArgument= (csAction.GetLength() >= 9) ? csAction.Mid(10) : _T("");

               int nPosSeparator = csArgument.Find(_T(';'));
               if (nPosSeparator > -1)
                  csArgument = csArgument.Mid(0, nPosSeparator);
               csArgument = GetSlashSubstitutedString(csArgument);

               acsFileList[idx] = csArgument;
            }

            idx++;
         }
      }
   }

   return idx;
}

UINT CFlashEngine::PrepareCopiesOfInteractiveOpenedFiles()
{
   // This is a helper method for the LPD export 
   // (see Java class "imc.epresenter.converter.PresentationConverter")
   // Interaction events "open-file"/"open-url" need to copy the 
   // specified files to the target directory (if possible)

   int iNumOfFiles = GetListOfInteractiveOpenedFiles(NULL);
   if (iNumOfFiles > 0)
   {
      CString* acsFileList = new CString[iNumOfFiles];
      GetListOfInteractiveOpenedFiles(acsFileList);

      for (int i = 0; i < iNumOfFiles; ++i)
      {
         // Copy file to target directory (if possible)
         CString csSource = m_csInputPath + acsFileList[i];
         CString csTarget;
         csTarget.Format(_T("%s"), m_tszSwfFile);
         StringManipulation::GetPath(csTarget);
         csTarget += _T("\\");
         csTarget += acsFileList[i];

         BOOL bCanBeCopied = ::CopyFile(csSource, csTarget, FALSE);
         if (bCanBeCopied)
            _tprintf(_T("%s copied.\n"), acsFileList[i]);
      }
   }

   return S_OK;
}

UINT CFlashEngine::LastErrorDetail(_TCHAR *tszError, DWORD *dwSize)
{
   if (m_csErrorDetail.IsEmpty())
   {
      if (tszError)
         tszError[0] = _T('\0');
      else
         *dwSize = 0;
   }
   else
   {
      if (tszError)
      {
         int size = min(*dwSize-1, m_csErrorDetail.GetLength());
         _TCHAR *tszBuffer = m_csErrorDetail.GetBuffer(size);
         _tcsncpy(tszError, tszBuffer, size);
         tszError[size] = _T('\0');
         m_csErrorDetail.ReleaseBuffer();
      }
      else
         *dwSize = m_csErrorDetail.GetLength()+1;
   }

   return S_OK;
}

UINT CFlashEngine::GetFontListFontNotFound(_TCHAR *tszFontList, DWORD *dwSize) {
    return GetFontListFontOrGlyphNotFound(tszFontList, dwSize, true);
}

UINT CFlashEngine::GetFontListGlyphNotFound(_TCHAR *tszFontList, DWORD *dwSize) {
    return GetFontListFontOrGlyphNotFound(tszFontList, dwSize, false);
}

UINT CFlashEngine::GetFontListFontOrGlyphNotFound(_TCHAR *tszFontList, DWORD *dwSize, bool bIsFont)
{
   HRESULT hr = S_OK;

   CString csFontList;

   POSITION pos;
   if (bIsFont)
       pos = m_mapFontNamesFontNotFound.GetStartPosition();
   else
       pos = m_mapFontNamesGlyphNotFound.GetStartPosition();

   while(SUCCEEDED(hr) && pos != NULL)
   {
      CString csFontName;
      CString csDummy;
      if (bIsFont)
         m_mapFontNamesFontNotFound.GetNextAssoc(pos, csFontName, csDummy);
      else
         m_mapFontNamesGlyphNotFound.GetNextAssoc(pos, csFontName, csDummy);
      
      if (!csFontList.IsEmpty())
         csFontList.Append(_T("\n"));

      csFontList.Append(csFontName);
   }

   if (csFontList.IsEmpty())
   {
      if (tszFontList)
         tszFontList[0] = _T('\0');
      else
         *dwSize = 0;
   }
   else
   {
      if (tszFontList)
      {
         int size = min(*dwSize-1, csFontList.GetLength());
         _TCHAR *tszBuffer = csFontList.GetBuffer(size);
         _tcsncpy(tszFontList, tszBuffer, size);
         tszFontList[size] = _T('\0');
         csFontList.ReleaseBuffer();
      }
      else
         *dwSize = csFontList.GetLength()+1;
   }

   return S_OK;
}

UINT CFlashEngine::GetFontListStylesNotSupported(_TCHAR *tszFontList, DWORD *dwSize)
{
   HRESULT hr = S_OK;

   CString csFontList;

   POSITION pos = m_mapFontNamesStylesNotSupported.GetStartPosition();
   while(SUCCEEDED(hr) && pos != NULL)
   {
      CString csFontName;
      CString csDummy;
      m_mapFontNamesStylesNotSupported.GetNextAssoc(pos, csFontName, csDummy);
      
      if (!csFontList.IsEmpty())
         csFontList.Append(_T("\n"));

      csFontList.Append(csFontName);
   }

   if (csFontList.IsEmpty())
   {
      if (tszFontList)
         tszFontList[0] = _T('\0');
      else
         *dwSize = 0;
   }
   else
   {
      if (tszFontList)
      {
         int size = min(*dwSize-1, csFontList.GetLength());
         _TCHAR *tszBuffer = csFontList.GetBuffer(size);
         _tcsncpy(tszFontList, tszBuffer, size);
         tszFontList[size] = _T('\0');
         csFontList.ReleaseBuffer();
      }
      else
         *dwSize = csFontList.GetLength()+1;
   }

   return S_OK;
}

UINT CFlashEngine::GetPreloaderFilename(_TCHAR *tszPreloaderFilename, DWORD *dwSize)
{
   HRESULT hr = S_OK;

   if (tszPreloaderFilename)
   {
      int size = min(*dwSize-1, m_csPreloaderFilename.GetLength());
      _TCHAR *tszBuffer = m_csPreloaderFilename.GetBuffer(size);
      _tcsncpy(tszPreloaderFilename, tszBuffer, size);
      tszPreloaderFilename[size] = _T('\0');
      m_csPreloaderFilename.ReleaseBuffer();
   }
   else
      *dwSize = m_csPreloaderFilename.GetLength()+1;

   return S_OK;
}

UINT CFlashEngine::GetTooShortPagesTimes(_TCHAR *tszTooShortPagesTimes, DWORD *dwSize)
{
   HRESULT hr = S_OK;

   if (tszTooShortPagesTimes)
   {
      int size = min(*dwSize-1, m_csTooShortPagesTimes.GetLength());
      _TCHAR *tszBuffer = m_csTooShortPagesTimes.GetBuffer(size);
      _tcsncpy(tszTooShortPagesTimes, tszBuffer, size);
      tszTooShortPagesTimes[size] = _T('\0');
      m_csTooShortPagesTimes.ReleaseBuffer();
   }
   else
      *dwSize = m_csTooShortPagesTimes.GetLength()+1;

   return S_OK;
}

/*
 * Checks the file(s) in <app data dir>\Macromedia\Flash Player\#Security\FlashPlayerTrust\
 * if the given path must be enabled.
 * Otherwise local replay with the new Flash Player 2 will result in an
 * error message or "hanging" at "Initialize".
 */
UINT CFlashEngine::EnableOutputDir(const _TCHAR *tszEnableDir) {
    HRESULT hr = S_OK;

    if (tszEnableDir != NULL) {
        int iPathLength = _tcslen(tszEnableDir);

        if (iPathLength > 0) {
            // file or directory is specified

            CString csTodoDir = tszEnableDir;

            _TCHAR tszLecturnityHome[MAX_PATH];

            bool bHomeSuccess = LMisc::GetLecturnityHome(tszLecturnityHome);

            if (bHomeSuccess) {
                // check if it is a sub directory of LECTURNITY home
                // if so only the home dir itself must be regarded

                int iLecHomeLength = _tcslen(tszLecturnityHome);
                if (tszLecturnityHome[iLecHomeLength - 1] == _T('\\')) {
                    tszLecturnityHome[iLecHomeLength - 1] = 0;
                    iLecHomeLength -= 1;
                }

                bool bIsSubDir = false;
                if (iPathLength >= iLecHomeLength) {
                    bIsSubDir = true;
                    for (int i=0; i<iLecHomeLength; ++i) {
                        if (tszEnableDir[i] != tszLecturnityHome[i]) {
                            bIsSubDir = false;
                            break;
                        }
                    }
                }

                if (bIsSubDir)
                    csTodoDir = tszLecturnityHome;
            }

            csTodoDir.MakeLower();

            _TCHAR tszAppDataDir[MAX_PATH];
            bool bAppDirSuccess = LMisc::GetApplicationDataDir(tszAppDataDir);

            if (bAppDirSuccess) {
                _tcscat(tszAppDataDir, _T("\\Macromedia\\Flash Player\\#Security\\FlashPlayerTrust\\"));

                // make sure the security directory exists; also creates intermediate directories
                ::SHCreateDirectoryEx(NULL, tszAppDataDir, NULL);

                const _TCHAR *tszTodoDir = (const _TCHAR*)csTodoDir;
                ULONG lHashValue = 0;
                for (int i=0; i<csTodoDir.GetLength(); ++i)
                    lHashValue = (int)tszTodoDir[i] + (lHashValue << 6) + (lHashValue << 16) - lHashValue;

                _TCHAR tszSpecificFilename[MAX_PATH];
                _stprintf(tszSpecificFilename, _T("LecturnityEnable_%u.txt"), lHashValue);
                
                _tcscat(tszAppDataDir, tszSpecificFilename);

                bool bEnableFileExists = _taccess(tszAppDataDir, 00) == 0;
                bool bWriteFile = !bEnableFileExists;
                
                // Always creates/opens this file.
                HANDLE hAllowFile = ::CreateFile(tszAppDataDir, FILE_WRITE_DATA, FILE_SHARE_READ,
                    NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

                if (hAllowFile != INVALID_HANDLE_VALUE) {
                    DWORD dwFileSize = 0;
                    dwFileSize = ::GetFileSize(hAllowFile, NULL);

                    if (bEnableFileExists) {
                        DWORD dwExpectedSize = csTodoDir.GetLength() * sizeof _TCHAR;
                        if (sizeof _TCHAR == 2)
                            dwExpectedSize += 2;

                        if (dwExpectedSize != dwFileSize)
                            bWriteFile = true;

                        // This special case _might_ double entries; however
                        // it does this only if two different paths collide with one hash value
                    }

                    if (bWriteFile) {
                        // Normally only writes something if the file does not exist.

                        // Always append (see special case above).
                        ::SetFilePointer(hAllowFile, 0, NULL, FILE_END);

                        DWORD dwWrite = 0;
                        if (sizeof _TCHAR == 2 && !bEnableFileExists) {
                            // Unicode BOM
                            BYTE aBom[2] = { 0xff, 0xfe };
                            ::WriteFile(hAllowFile, aBom, 2, &dwWrite, NULL);
                        }

                        if (bEnableFileExists) {
                            ::WriteFile(hAllowFile, _T("\r\n"), 2 * sizeof _TCHAR, &dwWrite, NULL);
                        }

                        BOOL bWriteSuccess = ::WriteFile(
                            hAllowFile, tszTodoDir, csTodoDir.GetLength() * sizeof _TCHAR, &dwWrite, NULL);

                        if (!bWriteSuccess)
                            hr = E_FAIL;
                    }

                    ::CloseHandle(hAllowFile);
                } else {
                    hr = E_FAIL;
                    ::DeleteFile(tszAppDataDir);
                }
            }
        }
    }

    return hr;
}


UINT CFlashEngine::ShowLostStopmarksWarningMessage()
{
   int iNumOfStopmarks = m_aFlashStopmarks.GetSize();

   // Warning Message shows the corresponding page to the lost stop mark
   // Get the pages here
   CArray<CPage*, CPage*>aPages;
   aPages.RemoveAll();
   int nFirst, nLast;
   m_pPageStream->GetPages(aPages, 0, m_iMovieLengthMs, 0, nFirst, nLast);
   int nNumOfPages = aPages.GetSize();

   // Warning Mesage has 3 columns: "Slide (number)", "Stop mark number", "Stop mark time"
   CArray<CString, CString> aSlideNumbers;
   CArray<CString, CString> aStopmarkNumbers;
   CArray<CString, CString> aStopmarkTimes;
   aSlideNumbers.RemoveAll();
   aStopmarkNumbers.RemoveAll();
   aStopmarkTimes.RemoveAll();

   // Stop mark number: it is resetted after each paging;
   int lastPageIndex = -1;
   int idxStopmark = 0;

   int i = 0;
   for (i = 0; i < iNumOfStopmarks; ++i)
   {
      int nMsTime = m_aFlashStopmarks.ElementAt(i).nMsTime;
      int nFrame = m_aFlashStopmarks.ElementAt(i).nFrame;

      // Reset stop mark number after paging event
      CPage *actPage = NULL;
      actPage = m_pPageStream->FindPageAtTimestamp(nMsTime);
      if (actPage == NULL)
      {
         if (nMsTime < m_pPageStream->GetFirstPage(0, m_iMovieLengthMs, 0)->GetBegin() )
            actPage = m_pPageStream->GetFirstPage(0, m_iMovieLengthMs, 0);
         if (nMsTime > m_pPageStream->GetLastPage(0, m_iMovieLengthMs, 0)->GetEnd() )
            actPage = m_pPageStream->GetLastPage(0, m_iMovieLengthMs, 0);
      }
      int pageIndex = actPage->GetPageNumber();
      if (pageIndex != lastPageIndex)
      {
         idxStopmark = 0;
         lastPageIndex = pageIndex;
      }
      idxStopmark++;

      if (nFrame < 0)
      {
         // Lost stop mark found

         // Get the stop mark number 
         CString csStopmarkNumber;
         csStopmarkNumber.Format(_T("%d"), idxStopmark); 
         aStopmarkNumbers.Add(csStopmarkNumber);

         // Get the corresponding page
         for (int k = 0; k < nNumOfPages; ++k)
         {
            CString csPageNumber;
            if ((nMsTime >= aPages[k]->GetBegin()) && (nMsTime < aPages[k]->GetEnd()))
            {
               csPageNumber.Format(_T("%d"), aPages[k]->GetPageNumber());
               aSlideNumbers.Add(csPageNumber);
               break;
            }
            if (nMsTime < aPages[0]->GetBegin()) 
            {
               csPageNumber.Format(_T("%d"), aPages[0]->GetPageNumber());
               aSlideNumbers.Add(csPageNumber);
            }
            if (nMsTime >= aPages[nNumOfPages-1]->GetEnd()) 
            {
               csPageNumber.Format(_T("%d"), aPages[nNumOfPages-1]->GetPageNumber());
               aSlideNumbers.Add(csPageNumber);
            }
         }

         // Get the stop mark time
         CString csTime = GetTimeStringFromMs(nMsTime);
         aStopmarkTimes.Add(csTime);
      }
   }

   // Debug info
   for (i = 0; i < aStopmarkNumbers.GetSize(); ++i)
      _tprintf(_T("Lost Stopmark: Slide %s, Stopmark %s at %s\n"), aSlideNumbers.ElementAt(i), aStopmarkNumbers.ElementAt(i), aStopmarkTimes.ElementAt(i));

   // Show the warning dialog
   CFlashLostStopmarksDlg lostStopmarksDlg(&aSlideNumbers, &aStopmarkNumbers, &aStopmarkTimes, NULL);
   lostStopmarksDlg.DoModal();

   aSlideNumbers.RemoveAll();
   aStopmarkNumbers.RemoveAll();
   aStopmarkTimes.RemoveAll();

   return S_OK;
}

CString CFlashEngine::GetInstallDir()
{
   _TCHAR tszInstallDir[_MAX_PATH];
   DWORD dwSize = _MAX_PATH;
   bool success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
      _T("Software\\imc AG\\LECTURNITY"), _T("InstallDir"),
      tszInstallDir, &dwSize);
   if (success)
   {
      CString csInstallDir(tszInstallDir);
      if (csInstallDir[csInstallDir.GetLength() - 1] != '\\')
         csInstallDir += CString(_T("\\"));
      return csInstallDir;
   }
   else
   {
      // this should not happen, try relative path here
      return CString(_T(".\\"));
   }
}

CString CFlashEngine::GetFlashBinPath()
{
    CString csFlashBinDir = GetInstallDir() + CString(_T("Publisher\\Flash\\bin\\"));
    return csFlashBinDir;
}

CString CFlashEngine::GetFlashFdbPath()
{
    CString csFlashFdbDir = GetInstallDir() + CString(_T("Publisher\\Flash\\fdb\\"));
    return csFlashFdbDir;
}

CString CFlashEngine::GetOutputPath()
{
   // Get output path from output file
   CString csPathOut(m_tszSwfFile);
   StringManipulation::GetPath(csPathOut);
   return csPathOut;
}

CString CFlashEngine::GetTimeStringFromMs(int timeMs)
{
   CString csTime;

   int valueMs = timeMs;
   int hrs = (int)(valueMs / 3600000.0f);
   valueMs = valueMs - (hrs * 3600000);
   int min = (int)(valueMs / 60000.0f);
   valueMs = valueMs - (min * 60000);
   int sec = (int)(valueMs / 1000.0f);
   valueMs = valueMs - (sec * 1000);

   csTime.Format(_T("%d:%s%d:%s%d.%d"), 
      hrs, 
      ((min < 10) ? _T("0") : _T("")),
      min, 
      ((sec < 10) ? _T("0") : _T("")),
      sec,
      valueMs);

   return csTime;
}

CString CFlashEngine::GetScormEnabled()
{
   CString csScormEnabled;
   csScormEnabled.Format(_T("%s"), m_bScormEnabled ? _T("true") : _T("false"));
   return csScormEnabled;
}

void CFlashEngine::GetNewDragableImageName(DrawSdk::Image *pImageObject, CString &csNewDragableImageName)
{
   CString csImageName = pImageObject->GetImageName();
   // Get output path from output file
   CString csPathOut = GetOutputPath();
   // Get the image name without path
   CString csImage = csImageName;
   StringManipulation::GetFilename(csImage);
   // Get the suffix from the image
   CString csSuffix = csImage;
   StringManipulation::GetFileSuffix(csSuffix);
   // Cut the suffix from the image name
   StringManipulation::GetFilePrefix(csImage);
   // The filename of the copy includes width and height, e.g. "image_320x240":
   int width = (int)pImageObject->GetWidth();
   int height = (int)pImageObject->GetHeight();
   // Set the new dragable image name
   csNewDragableImageName.Format(_T("%s\\%s_%dx%d.%s"), csPathOut, csImage, width, height, csSuffix);
}

char* CFlashEngine::GetCharFromCString(CString csString)
{
   static char szMultibyte[4096];

#ifdef _UNICODE
   int iCreated = ::WideCharToMultiByte(CP_ACP, 0, csString, csString.GetLength(), szMultibyte, 4096, NULL, NULL);
   szMultibyte[iCreated] = 0;
#else
   strcpy( szMultibyte, csString.GetBuffer(0));
   csString.ReleaseBuffer();
#endif // _UNICODE

   return szMultibyte; 
}

char* CFlashEngine::GetUtf8CharFromCString(CString csString)
{
   static char szUtf8[4096];

#ifdef _UNICODE
   int iCreated = ::WideCharToMultiByte(CP_UTF8, 0, csString, csString.GetLength(), szUtf8, 4096, NULL, NULL);
   szUtf8[iCreated] = 0;
#else
   WCHAR wszDummy[4096];
   int iLength = ::MultiByteToWideChar(CP_ACP, 0, csString, csString.GetLength(), wszDummy, 4096);
   wszDummy[iLength] = 0;
   int iCreated = ::WideCharToMultiByte(CP_UTF8, 0, wszDummy, iLength, szUtf8, 4096, NULL, NULL);
   szUtf8[iCreated] = 0;
#endif

   return szUtf8; 
}

HRESULT CFlashEngine::InsertLogoImage(_TCHAR *tszLogoFile, _TCHAR *tszLogoUrl, 
                                      bool &bShowLogoImage, SWFBitmap **pBmpLogo)
{
   HRESULT hr = S_OK;

   CString csLogoImage(tszLogoFile); 
   bShowLogoImage = (csLogoImage.IsEmpty()) ? false : true;

   if (bShowLogoImage)
   {
      // Load the logo image (which can be rescaled)
      hr = LoadLogoImage(pBmpLogo, csLogoImage, 
                         m_iLogoWidth, m_iLogoHeight);

      if (FAILED(hr))
      {
         // the image can neither be loaded nor displayed
         hr = imc_lecturnity_converter_LPLibs_INPUT_FILE_MISSING;

         _TCHAR tszMessage[1024];
         _stprintf(tszMessage, _T("Error: Cannot open logo image file! %s\n"), csLogoImage);
         _tprintf(tszMessage);
         // TODO (?): further error handling
      }
   }

   return hr;
}

HRESULT CFlashEngine::CalculateFrameRate(int &nLength, int &nMsecPerFrame)
{
   HRESULT hr = S_OK;

   // Get the length of the document
   // - Next line may produce wrong result because of improper event queues produced by the Editor
   //int nLength = m_pWhiteboardStream->GetLength();
   // - Use the movie length here 
   nLength = m_iMovieLengthMs;

   // If the movie length determination failed above, we have -1 as movie length.
   // --> try to get the audio length from the Whiteboerd stream
   if (m_iMovieLengthMs < 0 && m_pWhiteboardStream != NULL)
      nLength = m_pWhiteboardStream->GetLength();

   // Find out the propper frame rate
   m_fFrameRate = 8.0f;
   float fLength = (float)nLength / 1000.0f;
   float fMaxFrameCount = 15500.0f;
   nMsecPerFrame = (int)(1000.0f/m_fFrameRate);

   if (fLength * m_fFrameRate > fMaxFrameCount)
   {
      m_fFrameRate = fMaxFrameCount/fLength;

      // Bugfix #1951
      // make sure m_fFrameRate is evenly divisable by 1000 
      if (m_fFrameRate > 4.0f)
         m_fFrameRate = 4.0f;
      else if (m_fFrameRate > 2.0f)
         m_fFrameRate = 2.0f;
      else if (m_fFrameRate > 1.0f)
         m_fFrameRate = 1.0f;
      else if (m_fFrameRate > 0.5f)
         m_fFrameRate = 0.5f;
      else if (m_fFrameRate > 0.25f)
         m_fFrameRate = 0.25f;
      // m_fFrameRate 0.25f is enough for 16 hours

      nMsecPerFrame = (int)(1000.0f/m_fFrameRate);
   }

   return hr;

}

UINT CFlashEngine::CreatePreloaderSwf(Gdiplus::ARGB refBackColor)
{
   HRESULT hr = S_OK;

   SWFMovie *movie = new SWFMovie();
   // Preloader must have the same dimension and frame rate as the main movie
   movie->setDimension(m_rcMovie.Width(), m_rcMovie.Height());
   movie->setRate(m_fFrameRate);

   // Calculate time interval depending on the frame rate
   int iTimeDeltaMs = (int)((1000.0f / m_fFrameRate) + 0.5f);

   // Set background color
   if (m_bIsSlidestar)
	   movie->setBackground(0,0,0);
   else
      movie->setBackground((refBackColor >> 16) & 0xff, (refBackColor >> 8) & 0xff, (refBackColor >> 0) & 0xff);

   SWFDisplayItem *di = NULL;

   //
   // 1st. frame (visible during preload)
   //

   // A simple text field for displaying e.g. "Loading... 23%"
   float txtWidth = 160.0f;
   float txtHeight = 20.0f;
   SWFTextField *txtField = new SWFTextField();
   txtField->setFont(m_pFntStd);
   txtField->setBounds(txtWidth, txtHeight);
   txtField->setHeight(16.0f);
   txtField->setIndentation(1.0f);
   if (m_bIsSlidestar)
      txtField->setColor(0xFF, 0xFF, 0xFF); // white text color on black background
   else
   {
      // Use black or white text color depending on the background lightness
      float rgbTmpIn[3];
      rgbTmpIn[0] = (float)((refBackColor >> 16) & 0xff);
      rgbTmpIn[1] = (float)((refBackColor >> 8) & 0xff);
      rgbTmpIn[2] = (float)((refBackColor >> 0) & 0xff);

      float *hlsTmp = RGBToHLS(rgbTmpIn, 255.0f);
      // hlsTmp[1] is the lightness factor (value range [0.0 .. 360.0]
      if (hlsTmp[1] < 180.0f)
         txtField->setColor(0xFF, 0xFF, 0xFF);
      else
         txtField->setColor(0x00, 0x00, 0x00);
   }
   txtField->setVariableName("text");
   txtField->setAlignment(SWFTEXTFIELD_ALIGN_CENTER);
   txtField->setFlags(SWFTEXTFIELD_USEFONT | SWFTEXTFIELD_NOEDIT | SWFTEXTFIELD_NOSELECT | SWFTEXTFIELD_HASLAYOUT);
   CString csInitialText;
   csInitialText.LoadString(IDS_FLASH_PRELOAD_SLIDESTAR);
   CString csText = csInitialText + _T("0123456789.% ");
   char *utfText = LString::TCharToUtf8(csText);
   char *utfInitialText = LString::TCharToUtf8(csInitialText);
   if (utfText)
   {
      txtField->addUTF8Chars(utfText);
      txtField->addUTF8String(utfInitialText);
      delete[] utfText;
      utfText = NULL;
      delete[] utfInitialText;
      utfInitialText = NULL;
   }
   di = movie->add(txtField);
   float centerX = 0.5f * (m_rcMovie.Width() - txtWidth);
   float centerY = 0.5f * (m_rcMovie.Height() - txtHeight);
   di->moveTo(centerX, centerY);
   di->setName("txtFieldLoader");

   // An empty sprite for the LECTURNITY movie
   SWFSprite *sprLecMovie = new SWFSprite();
   sprLecMovie->nextFrame();
   di = movie->add(sprLecMovie);
   di->setName("sprLecMovie");


   // LEC SWF file name
   CString csLecSwfName = m_tszSwfFile;
   StringManipulation::GetFilename(csLecSwfName);
   if (m_bIsSlidestar)
      csLecSwfName = _T("document.swf");
   char *utfLecSwfName = LString::TCharToUtf8(csLecSwfName);

   // ActionScript
   CString csScript;
   CString csScript1;
   CString csScript2;
   CString csSubScript;

   if (utfLecSwfName)
   {
#ifdef _UNICODE
      csScript1.Format(_T("\
         gotoAndStop(1);\
         var baseUrl;\
         var mainSwf;\
         var defaultSwf = \"%S\";\
         var streamUrl;\
         var streamStripFlvSuffix;\
         var autoStart;\
      "), utfLecSwfName);
#else
      csScript1.Format(_T("\
         gotoAndStop(1);\
         var baseUrl;\
         var mainSwf;\
         var defaultSwf = \"%s\";\
         var streamUrl;\
         var streamStripFlvSuffix;\
         var autoStart;\
      "), utfLecSwfName);
#endif //_UNICODE
      delete[] utfLecSwfName;
      utfLecSwfName = NULL;
   }
   else // utfLecSwfName == NULL
   {
      csScript1.Format(_T("\
         gotoAndStop(1);\
         var baseUrl;\
         var mainSwf;\
         var defaultSwf = \"%s\";\
         var streamUrl;\
         var streamStripFlvSuffix;\
         var autoStart;\
      "), csLecSwfName);
   }

   if (m_tszVideoFlvFile != NULL)
   {
      csSubScript.Format(_T(" var videoFlvName;"));
      csScript1 += csSubScript;
   }

   for (int i = 0; i < m_iNumOfClips; ++i)
   {
      csSubScript.Format(_T("var clip%dFlvName;"), i);
      csScript1 += csSubScript;
   }

   csScript2.Format(_T("\
      var counter = 0;\
      var vi = 0;\
      \
      sprLecMovie._visible = false;\
      \
      if (mainSwf == eval(\"undefined\"))\
         mainSwf = \"\";\
      if (mainSwf.length > 0)\
         defaultSwf = mainSwf;\
      loadMovie(defaultSwf, sprLecMovie);\
      sprLecMovie.stop();\
      vi = setInterval(updatePreload, %d);\
      \
      function updatePreload()\
      {\
         if (counter > 0 && sprLecMovie.getBytesTotal() > 0 && sprLecMovie.getBytesLoaded() >= sprLecMovie.getBytesTotal())\
         {\
            clearInterval(vi);\
            vi = 0;\
            gotoAndStop(3);\
         }\
         else\
         {\
            sprLecMovie.onLoad();\
            sprLecMovie._visible = false;\
            if (sprLecMovie.g_bIsAlreadyLoaded == true)\
               sprLecMovie._visible = true;\
            var percent = Math.round(100 * (sprLecMovie.getBytesLoaded() / sprLecMovie.getBytesTotal()));\
            txtFieldLoader.text = \"%s  \";\
            txtFieldLoader.text += percent;\
            txtFieldLoader.text += \"%%\";\
            counter = counter + 1;\
         }\
      }\
      \
   "), iTimeDeltaMs, csInitialText);
   csScript = csScript1 + csScript2;
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   movie->nextFrame();

   //
   // 2nd. frame ("buffer" frame: just to be sure that the main movie is not visible too early)
   //

   // ActionScript
   csScript.Format(_T("\
      gotoAndStop(1);\
   "));
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   movie->nextFrame();

   //
   // 3rd. frame (show the main movie)
   //

   // ActionScript
   csScript1.Format(_T("\
      txtFieldLoader._visible = false;\
      sprLecMovie.serverPrePath = baseUrl;\
      sprLecMovie.baseUrl = baseUrl;\
      sprLecMovie.streamUrl = streamUrl;\
      sprLecMovie.streamStripFlvSuffix = streamStripFlvSuffix;\
   "));

   if (m_tszVideoFlvFile != NULL)
   {
      csSubScript.Format(_T("sprLecMovie.videoFlvName = videoFlvName;"));
      csScript1 += csSubScript;
   }

   for (int i = 0; i < m_iNumOfClips; ++i)
   {
      csSubScript.Format(_T("sprLecMovie.clip%dFlvName = clip%dFlvName;"), i, i);
      csScript1 += csSubScript;
   }

   csScript2.Format(_T("\
      sprLecMovie.autoStart = autoStart;\
      sprLecMovie._visible = true;\
      sprLecMovie.onLoad();\
   "));
   csScript = csScript1 + csScript2;
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   movie->nextFrame();

   //
   // Save the preloader movie
   //
   CString csPreloaderSwf;
   csPreloaderSwf.Format(_T("%s\\%s"), GetOutputPath(), m_csPreloaderFilename);

   FILE *file = _tfopen(csPreloaderSwf, _T("wb"));
   movie->save(file);
   fclose(file);

   return hr;
}

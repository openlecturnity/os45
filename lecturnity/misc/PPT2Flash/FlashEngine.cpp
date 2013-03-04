#include "stdafx.h"

#include <Vfw.h>

#include "FlashEngine.h"
#include "FlashEngineHelper.h"

#include "mingpp.h"
#include "lutils.h" // Using LFile for temporary files
#include "MiscFunctions.h"
#include "mmsystem.h"
#include "PageObject.h"

// For getting audio length and sample rate ...
#include <qedit.h> // DirectShow
#include "streams.h" // BaseClasses DirectShow
#include <comdef.h> // _bstr_t
#include <atlbase.h> // CComPtr, CoInitialize...

#define INPUT_FILE_MISSING 111
#define COMMAND_EXECUTION_FAILED 112
#define IMAGE_CONVERSION_FAILED 113
#define WARN_FONT_NOT_FOUND 114
#define UDIO_CONVERSION_FAILED 115
#define FONT_CONVERSION_FAILED 116
#define AUDIO_CONVERSION_FAILED 117
#define EMBED_FILE_MISSING 118
#define DOCUMENT_TOO_COMPLEX 119
#define COMMAND_START_FAILED 120


#define PAGE_DURATION 5000

CFlashEngine::CFlashEngine()
{
   m_tszSwfFile = NULL;

   m_iFlashVersion = 7; // default: 5

   m_iMaxIndexSoFar = 0;
   m_iMovieLengthMs = -1;
   m_iInputWidth = -1;
   m_iInputHeight = -1;
   m_iOutputWidth = -1;
   m_iOutputHeight = -1;
   m_fMovieWidth = -1.0f;
   m_fMovieHeight = -1.0f;
   m_fSlideOffsetX = 0.0f;
   m_fSlideOffsetY = 0.0f;
   m_fControlbarHeight = -1.0f;
   m_fScale = 1.0f;
   m_dGlobalScale = (double)(1.0f);
   m_dFeedbackScale = (double)(1.0f);
   m_fFrameRate = 8.0f; // initial value

   m_bAutostartMode = false;
   m_bCancelRequested = false;
   m_bCleanupCalled = false;
   m_bScormEnabled = false;


   // this has to be done by a JNI call later on
   SetBasicColors(0xEA, 0xE7, 0xD7);

   // For debugging purposes
   //_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
   //_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
}

CFlashEngine::~CFlashEngine()
{
   if (m_tszSwfFile)
      delete[] m_tszSwfFile;

   for (int i = 0; i < m_aImageInformation.GetSize(); ++i)
   {
      if (m_aImageInformation[i] != NULL)
         delete m_aImageInformation[i];
   }
   m_aImageInformation.RemoveAll();

   m_mapFontNames.RemoveAll();

   if (!m_bCleanupCalled)
      Cleanup();
}


UINT CFlashEngine::ReadAndProcessPages(ASSISTANT::Page *pPage)
{
   HRESULT hr = S_OK;

   CArray<ASSISTANT::DrawObject *, ASSISTANT::DrawObject *> arObjects;
   pPage->GetObjects(arObjects);

   if (m_iInputWidth == -1 || m_iInputWidth < pPage->GetWidth())
      m_iInputWidth = pPage->GetWidth();
   
   if (m_iInputHeight == -1 || m_iInputHeight < pPage->GetHeight())
      m_iInputHeight = pPage->GetHeight();

   for (int i = 0; i < arObjects.GetSize(); ++i)
      FillHashTablesForDrawObject(arObjects[i]);

   arObjects.RemoveAll();

   return hr;
}

void CFlashEngine::FillHashTablesForDrawObject(ASSISTANT::DrawObject *pObject)
{
   // Detect the object type
   int type = pObject->GetType();

   switch ( type )
   {
      case ASSISTANT::IMAGE:
         {
            // Fill the hash table with the images
            ASSISTANT::Image *pImageObject = (ASSISTANT::Image *)pObject;
            CString csImageName = pImageObject->GetImageName();
            bool bAlreadyInserted = false;
            for (int i = 0; i < m_aImageInformation.GetSize() && !bAlreadyInserted; ++i)
            {
               if (m_aImageInformation[i]->IsEqual(pImageObject))
                  bAlreadyInserted = true;
            }
            if (!bAlreadyInserted)
            {
               ImageInformation *pImageInformation = new ImageInformation(pImageObject);
               m_aImageInformation.Add(pImageInformation);
            }
            break;
         }
      case ASSISTANT::TEXT:
         {
            // Fill the hash table with the fonts
            ASSISTANT::Text *pTextObject = (ASSISTANT::Text*)pObject;
            CString csTtfName = pTextObject->GetTtfName();
            if (csTtfName.IsEmpty())
            {
               LOGFONT logFont;
               pTextObject->GetLogFont(&logFont);
               csTtfName = GetTtfName(logFont);
               if (csTtfName.IsEmpty())
               {
                  csTtfName = logFont.lfFaceName;
                  csTtfName += _T(".ttf");
                  // this is a dummy value: it ends not on ".ttf"
                  // and can thus be detected lateron
               }
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
      m_tszSwfFile = new _TCHAR[_tcslen(tszSwfFile)+1];
      _tcscpy(m_tszSwfFile, tszSwfFile);
      m_tszSwfFile[_tcslen(tszSwfFile)] = '\0';

      return S_OK;
   }
   else
      return E_POINTER;
}


UINT CFlashEngine::SetInputDimensions(int iWidth, int iHeight)
{
   if (iWidth > 0 && iHeight > 0)
   {
      m_iInputWidth = iWidth;
      m_iInputHeight = iHeight;
      return S_OK;
   }
   else
      return E_INVALIDARG;
}   

UINT CFlashEngine::SetOutputDimensions(int iWidth, int iHeight)
{
   if (iWidth > 0 && iHeight > 0)
   {
      m_iOutputWidth = iWidth;
      m_iOutputHeight = iHeight;
      
      return S_OK;
   }
   else
      return E_INVALIDARG;
}

UINT CFlashEngine::SetAutostartMode(bool bAutostartMode)
{
   m_bAutostartMode = bAutostartMode;
   return S_OK;
}

UINT CFlashEngine::GetMovieWidth()
{
   //return m_iMovieWidth;

   // The embedded movie size can differ from the movie size 
   // if input and output slide size is not equal
   int iEmbeddedMovieWidth = (int)(0.5f + (m_dGlobalScale * m_fMovieWidth));
         
   // Debug info
   //printf("- movie width:  %g\n- movie embedded width:  %d\n", m_fMovieWidth, iEmbeddedMovieWidth);

   return iEmbeddedMovieWidth;
}

UINT CFlashEngine::GetMovieHeight()
{
   //return m_iMovieHeight;

   // The embedded movie size can differ from the movie size 
   // if input and output slide size is not equal
   int iEmbeddedMovieHeight = (int)(0.5f + (m_dGlobalScale * m_fMovieHeight));

   // Debug info
   //printf("- movie height: %g\n- movie embedded height: %d\n", m_fMovieHeight, iEmbeddedMovieHeight);
   return iEmbeddedMovieHeight;
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
   m_rgbUp.r =   0x8A; m_rgbUp.g =   0x82; m_rgbUp.b =   0x6D;
   m_rgbOver.r = 0xA6; m_rgbOver.g = 0xC7; m_rgbOver.b = 0xF5;
   m_rgbDown.r = 0x30; m_rgbDown.g = 0x84; m_rgbDown.b = 0xF7;

   
   // Fill gradients

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

   return S_OK;
}

UINT CFlashEngine::RescaleImages(float scaleX, float scaleY)
{
   HRESULT hr = S_OK;

   if (!m_tszSwfFile)
      hr = E_POINTER;


   if (SUCCEEDED(hr))
   {
      //printf("\nRescaling Images...");

      for (int i = 0; SUCCEEDED(hr) && i < m_aImageInformation.GetSize(); ++i)
      {
         hr = RescaleImageObject(m_aImageInformation[i], scaleX, scaleY);
      }

      //printf(" Done.\n\n");
   }

   return hr;
}

UINT CFlashEngine::RescaleImageObject(ImageInformation *pImageInformation, float scaleX, float scaleY)
{
   HRESULT hr = S_OK;
   
   if (!pImageInformation)
      hr = E_FAIL;

   if (SUCCEEDED(hr))
   {
      ASSISTANT::Image *pImageObject = pImageInformation->GetImage();
      CString csImageIn = pImageObject->GetImageName();
   
      CString csImageOut;
   
      hr = WriteRescaledImage(pImageObject, csImageOut, scaleX, scaleY);
   
      // set the (temporary) image output file name
      pImageInformation->SetOutputName(csImageOut);
   }

   return hr;
}

UINT CFlashEngine::WriteRescaledImage(ASSISTANT::Image *pImageObject, CString &csImageOut, 
                                      float scaleX, float scaleY)
{
   HRESULT hr = S_OK;

   // Get output path from output file
   CString csPathOut = GetOutputPath();

   // Get the image name without path
   CString csImageName = pImageObject->GetImageName();
   StringManipulation::GetFilename(csImageName);

   // Get the suffix from the image name --> conversion of GIF, PNG ,JPG only
   CString csImageSuffix = csImageName;
   StringManipulation::GetFileSuffix(csImageSuffix);
   csImageSuffix.MakeLower();

   if (csImageSuffix.Compare(_T("jpg")) == 0)
      csImageSuffix = _T("png");

   // Cut the suffix from the image name
   StringManipulation::GetFilePrefix(csImageName);

   // Set the output image name
   //csImageOut = csPathOut + _T("\\") + csImageName + _T("_tmp.") + csImageSuffix;
   if (csImageSuffix != _T("gif") && 
       csImageSuffix != _T("png") && 
       csImageSuffix != _T("jpg") && 
       csImageSuffix != _T("jpeg"))
      csImageOut.Format(_T("%s\\%s_tmp.%s"), csPathOut, csImageName, _T("png"));
   else
      csImageOut.Format(_T("%s\\%s_tmp.%s"), csPathOut, csImageName, csImageSuffix);

   int nLen = csImageOut.GetLength();
   WCHAR* wszImageOut = new WCHAR[nLen+1];
#ifdef _UNICODE
   wcscpy(wszImageOut, csImageOut);
#else
   ::MultiByteToWideChar(CP_ACP, 0, csImageOut, -1, wszImageOut, nLen+1);
#endif

///   printf("--> %d %S\n", nLen, wszImageOut);

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
      hr = pImageObject->WriteScaledImageFile(wszImageOut, scaleX, scaleY, L"image/png");
   }

   // Add path & name of temporary file to a container
   m_mapTempFiles.SetAt(csImageOut, NULL);

   if (wszImageOut)
      delete[] wszImageOut;

   return hr;
}

UINT CFlashEngine::ConvertImage(ImageInformation *pImageInformation, CString csImageIn, 
                                CString csImageTmp, CString &csImageOut, LPCTSTR tszProgramPath)
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
      _stprintf(szCommandLine, _T("%s\\gif2dbl.exe \"%s\" \"%s\""), tszProgramPath, csImageTmp, csImageOut);
   }
   else if (csImageSuffix.Compare(_T("png")) == 0)
   {
      _stprintf(szCommandLine, _T("%s\\png2dbl.exe \"%s\" \"%s\""), tszProgramPath, csImageTmp, csImageOut);
   }
   else if (csImageSuffix.Compare(_T("png")) == 0)
   {
      _stprintf(szCommandLine, _T("%s\\png2dbl.exe \"%s\" \"%s\""), tszProgramPath, csImageTmp, csImageOut);
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

            /*/
            _TCHAR tszMessage[1024];
            _stprintf(tszMessage, __T("! Copy failed.\n  Error: 0x%08x\n"), dwError);
            _tprintf(tszMessage);
            /*/
         
            hr = COMMAND_EXECUTION_FAILED;
         }
      }
      // else it can be the same input and output directory
      // then no copy needed for jpgs and the file shouldn't be deleted (twice later)
      
   }
   else
   {
      bIsSupportedImageFormat = false;
   }

   if (SUCCEEDED(hr) && bIsSupportedImageFormat)
   {
      if (_tcslen(szCommandLine) > 0)
         hr = CommandCall(szCommandLine);

      LFile lFile(csImageOut);

      if (SUCCEEDED(hr))
      {
         if (lFile.Exists())
         {
            // Update CMap: Set the corresponding output file
            pImageInformation->SetOutputName(csImageOut);
            
            if (csImageIn.Compare(csImageOut) != 0)
               m_mapTempFiles.SetAt(csImageOut, NULL);
         }
         else
         {
            m_csErrorDetail.Empty();
            m_csErrorDetail += csImageTmp;
  
            hr = IMAGE_CONVERSION_FAILED;
         }
      }
      else
      {
         m_csErrorDetail.Empty();
         m_csErrorDetail += csImageTmp;

         hr = IMAGE_CONVERSION_FAILED;
      }
   }

   return hr;
}

UINT CFlashEngine::ConvertImages(LPCTSTR tszProgramPath)
{
   HRESULT hr = S_OK;

   if (!m_tszSwfFile)
      hr = E_POINTER;

   if (m_iOutputWidth == -1)
   {
      m_iOutputWidth = m_iInputWidth;
      m_iOutputHeight = m_iInputHeight;
   }

   float fScaleX = (float)m_iOutputWidth/(float)m_iInputWidth;
   float fScaleY = (float)m_iOutputHeight/(float)m_iInputHeight;

   float fScale = fScaleX > fScaleY ? fScaleY : fScaleX;

   
   if (SUCCEEDED(hr) && m_aImageInformation.GetSize() > 0)
   {
      // We have to rescale the images first
      // This method also modifies the output file name in the image hash table
      hr = RescaleImages(fScale, fScale);

      for (int i = 0; i < m_aImageInformation.GetSize() && SUCCEEDED(hr); ++i)
      {
         if (m_bCancelRequested == true)
            break;

         CString csImageIn;
         CString csImageOut;
         ImageInformation *pImageInformation = m_aImageInformation[i];


         csImageIn = pImageInformation->GetInputName();

         // If the images are not rescaled in the step before
         // we have to use 'csImageOut' as input file name for conversion
         CString csImageTmp;
         if (pImageInformation->IsRescaled())
            csImageTmp = pImageInformation->GetOutputName();
         else
            csImageTmp = pImageInformation->GetInputName();

         LFile mFile(csImageIn);
         
         if (!mFile.Exists())
         {
            m_csErrorDetail.Empty();
            m_csErrorDetail += csImageIn;
        
            hr = INPUT_FILE_MISSING;
         }

         if (SUCCEEDED(hr))
         {
            hr = ConvertImage(pImageInformation, csImageIn, csImageTmp, csImageOut, tszProgramPath);
         }
      }
   }

   return hr;
}

UINT CFlashEngine::ConvertFonts(LPCTSTR tszProgramPath)
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

         // Get path from output swf file 
         // we need it to look for font files in it
         CString csPathIn = m_tszSwfFile;
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
         if (csTtfSuffix.Compare(_T("ttf")) == 0)
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
                       
                        m_csErrorDetail.Empty();
                        m_csErrorDetail += csTtfIn;

                        CString csMessage;
                        csMessage.Format(_T("Font %s not found for conversion!"), csTtfIn);
                        hr = WARN_FONT_NOT_FOUND;
                     }
                  }
               }
            }

            if (S_OK == hr)
            {
               // Conversion TTF --> FFT
               _stprintf(szCommandLine, _T("%s\\ttf2fft.exe \"%s\" -o \"%s\""), tszProgramPath, csTtfPathAndFile, csFftOut);
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
               _stprintf(szCommandLine, _T("%s\\makefdb.exe \"%s\" \"%s\""), tszProgramPath, csFftOut, csFdbOut);
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
                     
                     hr = FONT_CONVERSION_FAILED;
                  }
                  
               }
            }
 
         }
         // else no ttf specified; gets handled below

      } // for all font names
   }

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
      
      hr = INPUT_FILE_MISSING;
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

   //printf("- Standard navigation states: %d %d %d %d %d %d\n", m_iNavigationControlBar, m_iNavigationStandardButtons, m_iNavigationTimeLine, nNavigationTimeDisplay, nNavigationDocumentStructure, nNavigationPluginContextMenu);
   
   return hr;
}

UINT CFlashEngine::BuildMovie(CArray<ASSISTANT::Page *, ASSISTANT::Page *> &arPages,
                              Gdiplus::ARGB refBackColor, LPCTSTR tszProgramPath)
{
   HRESULT hr = S_OK;
   HRESULT hrPossibleWarning = S_OK;
 
   if (SUCCEEDED(hr))
   {
           
      // Ming init stuff
      Ming_init();
      // Ming_useSWFVersion(5); // is default
      Ming_useSWFVersion(m_iFlashVersion);

      // Look for metadata
      CString csKeyA = _T("author");
      CString csKeyT = _T("title");
      CString csAuthor;
      CString csTitle;


      // Calculate the movie dimension
      // Borders, metadata and logo image lead to a larger movie size than the slide size itself
      //
      // --> We have to calculate the real movie size considering the scale factor 
      // and an offset for the upper left corner of the slides

      // Calculate a scaling factor for the slides
      float fScaleX = (double)m_iOutputWidth / (double)m_iInputWidth;
      float fScaleY = (double)m_iOutputHeight / (double)m_iInputHeight;

      m_fScale = (fScaleX < fScaleY) ? fScaleX : fScaleY;
      double m_dGlobalScale = m_fScale;



      SIZE sizeWbInput;

      sizeWbInput.cx = m_iInputWidth;
      sizeWbInput.cy = m_iInputHeight;

      float fPadding        = (float)FLASH_PADDING;
      float fBorderSize     = (float)FLASH_BORDERSIZE;
      float fMetadataHeight = (float)FLASH_METADATAHEIGHT;
      m_fSlideOffsetX   = (float)0;
      m_fSlideOffsetY   = (float)0;


      // Set the movie dimension
      //
      // Consider offsets
      m_fMovieWidth = m_iOutputWidth;
      m_fMovieHeight = m_iOutputHeight;


      // Find out the propper frame rate
      m_fFrameRate = 0.25f;
      

      //
      // New Movie
      //
      SWFMovie *movie = new SWFMovie();
 

      // Now set the dimension, calculated a few steps before
      movie->setDimension(m_fMovieWidth, m_fMovieHeight);
      movie->setRate(m_fFrameRate);


      // setting the frame rate is done after all events are processed (and thus
      // the frame count is known)
      // movie->setFrames((int)(fLength * m_fFrameRate + 0.5f)); 



      // Set background color
      //movie->setBackground(0x2D, 0x33, 0x51);
      movie->setBackground((refBackColor >> 16) & 0xff, (refBackColor >> 8) & 0xff, (refBackColor >> 0) & 0xff);

            
      // Add Fonts 
      // - Standard font (for time display etc) 
      // - "Logo" font (for LECTURNITY webplayer)
      CString csFont;
      m_fntStd = NULL; 
      SWFFont *fntLogo = NULL;


      if (SUCCEEDED(hr))
      {
         // Use standard  font "ARIAL.fdb" which has been added manually to the 
         // font hash table in "SetInputFiles()" before.
         if (!m_mapSwfFonts.Lookup(_T("ARIAL.TTF"), m_fntStd))
         {
            CString csFdbOut;
            csFdbOut.Format(_T("%s\\ARIAL.fdb"), GetOutputPath());

            FILE *pFile;
            if (NULL == (pFile = _tfopen(csFdbOut, _T("rb"))))
            {
               hr = EMBED_FILE_MISSING;
               
               /*/
               _TCHAR tszMessage[1024];
               _stprintf(tszMessage, _T("Error: Cannot open font file! %s\n"), csFdbOut);
               _tprintf(tszMessage);
               /*/
            }
            
            if (SUCCEEDED(hr))
            {
               m_fntStd = new SWFFont(pFile);
               m_mapSwfFonts.SetAt(_T("ARIAL.TTF"), m_fntStd);
               m_mapTempFiles.SetAt(csFdbOut, pFile);
            }
         }
      }

      // Tooltips
      if (SUCCEEDED(hr))
      {
         //CreateTooltipField(movie);
      }
      


      //
      // Try to create the movie
      //

      // the boundary leaves some room (below 16000) for the replay controls et al.
      int nObjectBoundary = 15400; 
      DWORD dwAddedObjects2 = 0;

      DWORD anfTime = timeGetTime();


      // Now continue creating the movie
      DWORD dwAddedObjects = 0;
      for (int i = 0; i < arPages.GetSize() && !m_bCancelRequested; ++i)
      {
         if (SUCCEEDED(hr) && arPages[i] != NULL)
         {
            hr = EmbedObjects(movie, arPages[i], &dwAddedObjects);
            if (hr > 0)
               hrPossibleWarning = hr;
            bool bHasAnimatedObjects = arPages[i]->ShowNextObject();
            while (bHasAnimatedObjects)
            {
               hr = EmbedObjects(movie, arPages[i], &dwAddedObjects);
               if (hr > 0)
                  hrPossibleWarning = hr;
               bHasAnimatedObjects = arPages[i]->ShowNextObject();
            }
         }
      }


      //_TCHAR tszMessage[1024];
      //_stprintf(tszMessage, "Objekte ursprünglich hinzugefügt: %d %dms", dwAddedObjects, endTime-anfTime);
      //::MessageBox(NULL, tszMessage, NULL, MB_OK);
      //_TCHAR tszMessage[1024];
      //_stprintf(tszMessage, "Objekte schlußendlich hinzugefügt: %d %dms", dwAddedObjects2, endTime2-anfTime2);
      //::MessageBox(NULL, tszMessage, NULL, MB_OK);
      DWORD dwAddedObjects3 = (dwAddedObjects > nObjectBoundary) ? dwAddedObjects2 : dwAddedObjects;
      //printf("Objects originally added:  %d\n", dwAddedObjects);
      //printf("Objects finally added:     %d\n", dwAddedObjects3);

      //printf("- CMap Images: %d objects.\n", m_mapImageNames.GetCount());
      //printf("- CMap FNames: %d objects.\n", m_mapFontNames.GetCount());
      //printf("- CMap Fonts:  %d objects:\n", m_mapSwfFonts.GetCount());
      CString csFontName;
      SWFFont *pSwfFont = NULL;
      POSITION pos = m_mapSwfFonts.GetStartPosition();
      while(pos != NULL)
      {
         m_mapSwfFonts.GetNextAssoc(pos, csFontName, pSwfFont);
         //printf(" - %s 0x%08x\n", csFontName, pSwfFont);
      }

     
      if (SUCCEEDED(hr) && !m_bCancelRequested)
      {
         FILE *pSwfFile = _tfopen(m_tszSwfFile, _T("wb"));
         if (pSwfFile)
         {
            movie->save(pSwfFile);
            fclose(pSwfFile);
         }
      }
      
      //Ming_cleanupFonts(); // well, how about a "normal" Cleanup() ???
      // delete movie;
      // delete m_fntStd;
      // all deletes are handled by Cleanup (and the destructor)
   }

   
   //printf("Build movie ... Done(hr=%d)\n", hr);
   // Cleanup() should be called by the calling thread (java) lateron
   
   if (SUCCEEDED(hr) && hrPossibleWarning != S_OK)
      return hrPossibleWarning;
   else
      return hr;

}

UINT CFlashEngine::BuildMovie(ASSISTANT::Page * pPage,
                              Gdiplus::ARGB refBackColor, LPCTSTR tszProgramPath)
{
   HRESULT hr = S_OK;
   HRESULT hrPossibleWarning = S_OK;
 
   if (SUCCEEDED(hr))
   {
           
      // Ming init stuff
      Ming_init();
      // Ming_useSWFVersion(5); // is default
      Ming_useSWFVersion(m_iFlashVersion);

      // Look for metadata
      CString csKeyA = _T("author");
      CString csKeyT = _T("title");
      CString csAuthor;
      CString csTitle;


      // Calculate the movie dimension
      // Borders, metadata and logo image lead to a larger movie size than the slide size itself
      //
      // --> We have to calculate the real movie size considering the scale factor 
      // and an offset for the upper left corner of the slides

      // Calculate a scaling factor for the slides
      float fScaleX = (double)m_iOutputWidth / (double)m_iInputWidth;
      float fScaleY = (double)m_iOutputHeight / (double)m_iInputHeight;

      m_fScale = (fScaleX < fScaleY) ? fScaleX : fScaleY;
      double m_dGlobalScale = m_fScale;



      SIZE sizeWbInput;

      sizeWbInput.cx = m_iInputWidth;
      sizeWbInput.cy = m_iInputHeight;

      float fPadding        = (float)FLASH_PADDING;
      float fBorderSize     = (float)FLASH_BORDERSIZE;
      float fMetadataHeight = (float)FLASH_METADATAHEIGHT;
      m_fSlideOffsetX   = (float)0;
      m_fSlideOffsetY   = (float)0;


      // Set the movie dimension
      //
      // Consider offsets
      m_fMovieWidth = m_iOutputWidth;
      m_fMovieHeight = m_iOutputHeight;


      // Find out the propper frame rate
      m_fFrameRate = 0.25f;
      

      //
      // New Movie
      //
      SWFMovie *movie = new SWFMovie();
 

      // Now set the dimension, calculated a few steps before
      movie->setDimension(m_fMovieWidth, m_fMovieHeight);
      movie->setRate(m_fFrameRate);


      // setting the frame rate is done after all events are processed (and thus
      // the frame count is known)
      // movie->setFrames((int)(fLength * m_fFrameRate + 0.5f)); 



      // Set background color
      //movie->setBackground(0x2D, 0x33, 0x51);
      movie->setBackground((refBackColor >> 16) & 0xff, (refBackColor >> 8) & 0xff, (refBackColor >> 0) & 0xff);

            
      // Add Fonts 
      // - Standard font (for time display etc) 
      // - "Logo" font (for LECTURNITY webplayer)
      CString csFont;
      m_fntStd = NULL; 
      SWFFont *fntLogo = NULL;


      //
      // Try to create the movie
      //

      // the boundary leaves some room (below 16000) for the replay controls et al.
      int nObjectBoundary = 15400; 
      DWORD dwAddedObjects2 = 0;

      DWORD anfTime = timeGetTime();


      // Now continue creating the movie
      DWORD dwAddedObjects = 0;
      if (SUCCEEDED(hr) && pPage != NULL && !m_bCancelRequested)
      {
         hr = EmbedObjects(movie, pPage, &dwAddedObjects);
         if (hr > 0)
            hrPossibleWarning = hr;
      }


      //_TCHAR tszMessage[1024];
      //_stprintf(tszMessage, "Objekte ursprünglich hinzugefügt: %d %dms", dwAddedObjects, endTime-anfTime);
      //::MessageBox(NULL, tszMessage, NULL, MB_OK);
      //_TCHAR tszMessage[1024];
      //_stprintf(tszMessage, "Objekte schlußendlich hinzugefügt: %d %dms", dwAddedObjects2, endTime2-anfTime2);
      //::MessageBox(NULL, tszMessage, NULL, MB_OK);
      DWORD dwAddedObjects3 = (dwAddedObjects > nObjectBoundary) ? dwAddedObjects2 : dwAddedObjects;
      //printf("Objects originally added:  %d\n", dwAddedObjects);
      //printf("Objects finally added:     %d\n", dwAddedObjects3);

      //printf("- CMap Images: %d objects.\n", m_mapImageNames.GetCount());
      //printf("- CMap FNames: %d objects.\n", m_mapFontNames.GetCount());
      //printf("- CMap Fonts:  %d objects:\n", m_mapSwfFonts.GetCount());
      CString csFontName;
      SWFFont *pSwfFont = NULL;
      POSITION pos = m_mapSwfFonts.GetStartPosition();
      while(pos != NULL)
      {
         m_mapSwfFonts.GetNextAssoc(pos, csFontName, pSwfFont);
         //printf(" - %s 0x%08x\n", csFontName, pSwfFont);
      }

     
      if (SUCCEEDED(hr) && !m_bCancelRequested)
      {
         FILE *pSwfFile = _tfopen(m_tszSwfFile, _T("wb"));
         if (pSwfFile)
         {
            movie->save(pSwfFile);
            fclose(pSwfFile);
         }
      }
      
      //Ming_cleanupFonts(); // well, how about a "normal" Cleanup() ???
      // delete movie;
      // all deletes are handled by Cleanup (and the destructor)
      
      if (movie != NULL)
         delete movie;

   }

   
   //printf("Build movie ... Done(hr=%d)\n", hr);
   // Cleanup() should be called by the calling thread (java) lateron
   
   if (SUCCEEDED(hr) && hrPossibleWarning != S_OK)
      return hrPossibleWarning;
   else
      return hr;

}

UINT CFlashEngine::EmbedObjects(SWFMovie *movie, ASSISTANT::Page *pPage, DWORD *dwAddedObjects)
{
   HRESULT hr = S_OK;
   
   HRESULT hrPossibleWarning = S_OK;
   
   DWORD dwVisibleObjects = 0;
   
   CArray<ASSISTANT::DrawObject *, ASSISTANT::DrawObject *> arObjects;
   pPage->GetObjects(arObjects);
   
   CArray<SWFDisplayItem*, SWFDisplayItem*> aDisplayItems;
   aDisplayItems.SetSize(arObjects.GetSize());

   for (int item=0; item < arObjects.GetSize(); ++item)
      aDisplayItems[item] = NULL;  
   
   // do some preprocessing of the objects
   // in order to delete/ignore some of them lateron
   // (these are: "temporary" marker objects and line 
   //  objects leading to a polyline (freehand)
   CArray<bool, bool> aCanBeConserved;
   
   // only generate font objects once for each font (ttf) name:
   /// --> use member variable here (because of "standard font" ARIAL)
   ///CMap<CString, LPCSTR, SWFFont*, SWFFont*> mapSwfFonts;
   CMap<SWFBlock*, SWFBlock*, float, float> mapImageScalingX;
   CMap<SWFBlock*, SWFBlock*, float, float> mapImageScalingY;
   
   

   SWFDisplayItem *diPointer = NULL;
   
   // do a "search" for events; there seems to be no "GetEvents()"
   float fLastPointerX = 0.0f, fLastPointerY = 0.0f;
   int nFrameCounter = 0;
   int nLastPageNumber = -1;
   int nLastStopmarkTime = -1;
   ////   int nAllSwfText = 0;
   
   for (int i = 0; i < arObjects.GetSize(); ++i)
   {
      if (m_bCancelRequested)
         break;
      
      ASSISTANT::DrawObject* pObject = arObjects[i];
      
      SWFShape *pShape = new SWFShape();
      SWFBlock *pBlock = pShape;
      
      int iType = pObject->GetType();
      
      float fLineWidth = 0.0f;
      bool bDashedLine = false;
      bool bDottedDash = false;
      
      // set properties common to all ColorObjects
      if (iType != ASSISTANT::IMAGE)
      {
         ASSISTANT::ColoredObject *pColorObject = (ASSISTANT::ColoredObject *)pObject;
         Gdiplus::ARGB ocref = pColorObject->GetARGBLineColor();
         fLineWidth = pColorObject->GetLineWidth() * m_fScale;
         
         if (fLineWidth < 1)
            fLineWidth = 1.0;

         pShape->setLine(fLineWidth, (ocref >> 16) & 0xff, 
            (ocref >> 8) & 0xff, (ocref >> 0) & 0xff, (ocref >> 24) & 0xff);
         
         
         if (pColorObject->IsFilled())
         {
            Gdiplus::ARGB fcref = pColorObject->GetARGBFillColor();
            SWFFill *pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
               (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
            pShape->setRightFill(pFill);
         }
         
         int nLineStyle = pColorObject->GetGdiplusLineStyle();
         if (nLineStyle != 0)
         {
            bDashedLine = true;
            bDottedDash = nLineStyle == 1;
         }
      }
      
      // shorthand declarations
      float x = (float)(pObject->GetX()) + m_fSlideOffsetX;
      float y = (float)(pObject->GetY()) + m_fSlideOffsetY;
      float width = (float)pObject->GetWidth(), height = (float)pObject->GetHeight();
      
      float fImageWidth = 0.0f, fImageHeight = 0.0f; // might be needed for scaling
      
      // handle the object type
      
      switch (iType)
      {
      case ASSISTANT::RECTANGLE:
         DrawRectangle(pShape, x, y, width, height, fLineWidth, bDashedLine, bDottedDash);
         break;
         
      case ASSISTANT::OVAL:
         DrawOval(pShape, x, y, width, height, fLineWidth, bDashedLine, bDottedDash);
         break;
         
      case ASSISTANT::LINE:
         {  // prevent error C2360
            
            ASSISTANT::Line *pLineObject = (ASSISTANT::Line *)pObject;
            
            //DrawLine(pShape, x, y, width, height, fLineWidth, bDashedLine, bDottedDash);
            // If we have arow heads, we have to do some corrections before drawing the line
            // --> DrawLine is called in "DrawLineAndArrowHeads()" below
            
            int nArrowConfig = pLineObject->GetArrowConfig();
            
            // Downwards compatibility to LECTURNITY versions < 1.7.0.p6: 
            // Default values:
            // arrow type = 1 (filled triangle), 
            // arrow size = 5 (normal).
            
            int nArrowStyle = 0;
            if (nArrowConfig != 0)
            {
               if (nArrowConfig < 100)
                  nArrowStyle = 1;
               else if ((nArrowConfig % 100) == 0)
                  nArrowStyle = 2;
               else 
                  nArrowStyle = 3;
            }

            //fLineWidth = 1.0;
            // Draw line and add arrow heads (if any)
            DrawLineAndArrowHeads(pShape, nArrowStyle, nArrowConfig, x, y, width, height, 
               fLineWidth, bDashedLine, bDottedDash, pLineObject->GetARGBLineColor());
         }
         break;
      case ASSISTANT::TEXT:
         {  // prevent error C2360
            
            // Note: Text does not use a normal SWFShape
            SWFText *pSwfText = new SWFText();
            
            ASSISTANT::Text *pTextObject = (ASSISTANT::Text *)pObject;
            
            hr = MapFlashFontFromTextObject(pTextObject, hrPossibleWarning);
            
            CString csTtfIn = pTextObject->GetTtfName();
            if (csTtfIn.IsEmpty())
            {
               LOGFONT logFont;
               pTextObject->GetLogFont(&logFont);
               csTtfIn = GetTtfName(logFont);
               if (csTtfIn.IsEmpty())
               {
                  // if no ttf is specified use the face name for the hash key
                  csTtfIn = logFont.lfFaceName;
                  csTtfIn += _T(".ttf");
               }
            }
            SWFFont *pSwfFont = GetFlashFontFromTextObject(pTextObject);
            
            if (pSwfFont)
            {
               Gdiplus::ARGB crFontColor = pTextObject->GetARGBLineColor();
               CString csText = pTextObject->GetString();
               LOGFONT lf;
               pTextObject->GetLogFont(&lf);
               float fFontSize = -1.0f * lf.lfHeight;
               ////                           printf("csTtfIn: 0x%08x %s %d/%d --> %s\n", pSwfFont, csTtfIn, m_mapSwfFonts.GetCount(), m_mapFontNames.GetCount(), csText);
               
               if (fFontSize > 0)
                  DrawText(pSwfText, x, y, pSwfFont, csText, fFontSize, crFontColor);
               // else negative font sizes lead to really big texts
            }
            
            pBlock = pSwfText;
         }
         break;   
      case ASSISTANT::MARKER:
      case ASSISTANT::POLYGON:
      case ASSISTANT::POLYLINE:
      case ASSISTANT::FREEHAND:
         {  // prevent error C2360

            ASSISTANT::Polygon *pPolygonObject = (ASSISTANT::Polygon *)pObject;
            CArray<Gdiplus::PointF *, Gdiplus::PointF *> *pArPoints;
            pArPoints = pPolygonObject->GetPoints();
            
            bool bPolylineIsSinglePoint = false;
            
            if (pArPoints->GetSize() > 1)
            {
               // Check, if all points are in fact on the same point
               int point = 1;
               double x1 = pArPoints->GetAt(0)->X;
               double y1 = pArPoints->GetAt(0)->Y;
               bPolylineIsSinglePoint = true;
               while ((point < pArPoints->GetSize()) && (bPolylineIsSinglePoint == true))
               {
                  double x2 = pArPoints->GetAt(point)->X;
                  double y2 = pArPoints->GetAt(point)->Y;
                  
                  if ((x1 != x2) || (y1 != y2))
                     bPolylineIsSinglePoint = false;
                  
                  point++;
               }
               
               if (bPolylineIsSinglePoint == false)
               {
                  for (point=1; point<pArPoints->GetSize(); ++point)
                  {
                     DrawLineTo(pShape,
                                (float)(pArPoints->GetAt(point-1)->X) + m_fSlideOffsetX, 
                                (float)(pArPoints->GetAt(point-1)->Y) + m_fSlideOffsetY,
                                (float)(pArPoints->GetAt(point)->X) + m_fSlideOffsetX, 
                                (float)(pArPoints->GetAt(point)->Y) + m_fSlideOffsetY, 
                                fLineWidth, bDashedLine, bDottedDash);
                  }
                  
                  if (pPolygonObject->GetType() == ASSISTANT::POLYGON) // make it a closed one
                  {
                     int iLastPoint = pArPoints->GetSize() - 1;
                     DrawLineTo(pShape,
                                (float)(pArPoints->GetAt(iLastPoint)->X) + m_fSlideOffsetX, 
                                (float)(pArPoints->GetAt(iLastPoint)->Y) + m_fSlideOffsetY,
                                (float)(pArPoints->GetAt(0)->X) + m_fSlideOffsetX, 
                                (float)(pArPoints->GetAt(0)->Y) + m_fSlideOffsetY, 
                                fLineWidth, bDashedLine, bDottedDash);
                  }
                  
               }
            }
            
            if ((pArPoints->GetSize() == 1) || (bPolylineIsSinglePoint == true))
            {
               // Draw a filled square here sourrounded by a hairline 
               Gdiplus::ARGB fcref = pPolygonObject->GetARGBLineColor();
               SWFFill *pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
                  (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
               pShape->setRightFill(pFill);
               pShape->setLine(0, (fcref >> 16)  & 0xff, (fcref >> 8) & 0xff, 
                  (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
               
               DrawRectangle(pShape, 
                  (float)(pArPoints->GetAt(0)->X) + m_fSlideOffsetX - (0.5f*fLineWidth), 
                  (float)(pArPoints->GetAt(0)->Y) + m_fSlideOffsetY - (0.5f*fLineWidth), 
                  fLineWidth, fLineWidth,
                  0.0f, bDashedLine, bDottedDash);
            }
         }
         break;                 
      case ASSISTANT::IMAGE:
         {  // prevent error C2360
            ASSISTANT::Image *pImageObject = (ASSISTANT::Image *)pObject;
            
            hr = MapFlashImageFromImageObject(pImageObject, &pShape, &pBlock, mapImageScalingX, mapImageScalingY);
         }
         break;
      default:
         // do nothing
         break;
      }
      
      
      
      if (SUCCEEDED(hr) && (pBlock))
      {
         SWFDisplayItem *di = NULL;
         di = movie->add(pBlock); // movie can be a dummy
                        
         if (pObject->GetType() == ASSISTANT::IMAGE)// || pObject->GetType() == ASSISTANT::TEXT)
         {
            // Images have to be moved to their correct positions
            // might be a bug in ming:
            // if you make the DrawRectangle with an image fill style 
            // with coordinates other than 0,0 then the image is misplaced (and tiled)
            ///                        di->moveTo(x, y);
            di->moveTo(m_fScale*x, m_fScale*y);
            
            // it is possible (namely with images added in the Assistant)
            // that the image has other dimensions than is
            // specified in the object queue
            ///                        float fFactorX = 1.0f;
            ///                        float fFactorY = 1.0f;
            ///                        if (mapImageScalingX.Lookup(pBlock, fFactorX) && mapImageScalingY.Lookup(pBlock, fFactorY))
            ///                           di->scaleTo(fFactorX, fFactorY);
            float fFactorX = m_fScale;
            float fFactorY = m_fScale;
            if (mapImageScalingX.Lookup(pBlock, fFactorX) && mapImageScalingY.Lookup(pBlock, fFactorY))
               di->scaleTo(fFactorX, fFactorY);
            
         }
         
         
         aDisplayItems[i] = di;
      }
      
      
      
      
      // else pEvent == pEventBefore
      
   }
   
   
   movie->nextFrame(); // movie can be a dummy
   ++nFrameCounter;

   // remove all objects which are not active in the current event
   for (int object = 0; object < aDisplayItems.GetSize(); ++object)
   {
      if (aDisplayItems[object] != NULL)
      {
         movie->remove(aDisplayItems[object]);
         aDisplayItems[object] = NULL;
      }
   }
   aDisplayItems.RemoveAll();

   // is needed in case of short(er) event queue files produced from the Editor
   movie->setFrames(nFrameCounter); 
   
   
   
   
   if (SUCCEEDED(hr) && hrPossibleWarning != S_OK)
      return hrPossibleWarning;
   else
      return hr;
}

UINT CFlashEngine::MapFlashImageFromImageObject(ASSISTANT::Image *pImageObject, SWFShape **pShape, SWFBlock **pBlock, 
                                                CMap<SWFBlock*, SWFBlock*, float, float> &mapImageScalingX,
                                                CMap<SWFBlock*, SWFBlock*, float, float> &mapImageScalingY)
{
   HRESULT hr = S_OK;

   SWFShape *pBitmapShape = NULL;

   ImageInformation *pImageInformation = NULL;
   for (int i = 0; i < m_aImageInformation.GetSize(); ++i)
   {
      if (m_aImageInformation[i]->IsEqual(pImageObject))
      {
         pImageInformation = m_aImageInformation[i];
         break;
      }
   }
   

   if (pImageInformation != NULL)
   {
      CString csImageOut = pImageInformation->GetOutputName();
      if (pImageInformation->GetBitmapShape() == NULL)
      {   
         FILE *pFile = NULL;
         if (NULL == (pFile = _tfopen(csImageOut, _T("rb"))))
         {
            hr = EMBED_FILE_MISSING;
            
            /*/
            _TCHAR tszMessage[1024];
            _stprintf(tszMessage, _T("Error: Cannot open image file! %s\n"), csImageOut);
            _tprintf(tszMessage);
            /*/
         }
         
         if (SUCCEEDED(hr))
         {
            
            // Note: Image does not use a normal SWFShape
            SWFBitmap *pBitmap = new SWFBitmap(pFile, csImageOut.Right(4) == _T(".dbl"));
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
            csImageOutHandle.Format(_T("!%s_0x%08x"), csImageOut, pFile);
            m_mapTempFiles.SetAt(csImageOutHandle, pFile);
            
            pImageInformation->SetBitmapShape(*pShape);

            float width = (float)pImageObject->GetWidth();
            float height = (float)pImageObject->GetHeight();
            float fImageWidth = pBitmap->getWidth();
            float fImageHeight = pBitmap->getHeight();
            ///         mapImageScalingX.SetAt(pShape, width/fImageWidth)
            ///         mapImageScalingY.SetAt(pShape, height/fImageHeight);
            mapImageScalingX.SetAt(*pShape, (m_fScale*width)/fImageWidth);
            mapImageScalingY.SetAt(*pShape, (m_fScale*height)/fImageHeight);
            
            //pBlock = pShape;
         }
      }
      else
      {
         // only open and create images once
         *pBlock = pImageInformation->GetBitmapShape();
      }
   }

   return hr;
}

UINT CFlashEngine::MapFlashFontFromTextObject(ASSISTANT::Text *pTextObject, HRESULT &hrPossibleWarning)
{
   HRESULT hr = S_OK;

   CString csTtfIn = pTextObject->GetTtfName();
   if (csTtfIn.IsEmpty())
   {
      LOGFONT logFont;
      pTextObject->GetLogFont(&logFont);
      csTtfIn = GetTtfName(logFont);
      if (csTtfIn.IsEmpty())
      {
         // if no ttf is specified use the face name for the hash key
         csTtfIn = logFont.lfFaceName;
         csTtfIn += _T(".ttf");
      }
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
            hr = EMBED_FILE_MISSING;
            
            /*/
            _TCHAR tszMessage[1024];
            _stprintf(tszMessage, _T("Error: Cannot open font file! %s\n"), csFdbOut);
            _tprintf(tszMessage);
            /*/
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
            //printf("Warning: Font %s not found! Maybe TTF was not specified\n", csTtfIn);
            m_csErrorDetail += csTtfIn;
            m_csErrorDetail += _T("\n");

            hrPossibleWarning = WARN_FONT_NOT_FOUND;
         }

         pSwfFont = m_fntStd;
         m_mapSwfFonts.SetAt(csTtfIn, pSwfFont);
      }
      // else: most likely 
   }

   return hr;
}

SWFFont* CFlashEngine::GetFlashFontFromTextObject(ASSISTANT::Text *pTextObject)
{
   SWFFont *pSwfFont = NULL;

   CString csTtfIn = pTextObject->GetTtfName();
   if (csTtfIn.IsEmpty())
   {
      LOGFONT logFont;
      pTextObject->GetLogFont(&logFont);
      csTtfIn = GetTtfName(logFont);
      if (csTtfIn.IsEmpty())
      {
         // if no ttf is specified use the face name for the hash key
         csTtfIn = logFont.lfFaceName;
         csTtfIn += _T(".ttf");
      }
   }

   if (!m_mapSwfFonts.Lookup(csTtfIn, pSwfFont))
      return NULL;

   return pSwfFont;
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
   pos = m_mapTempFiles.GetStartPosition();

   // then delete the fonts
   SWFFont *pFont;
   CString csTtfIn;
   pos = m_mapSwfFonts.GetStartPosition();
   while (pos != NULL)
   {
      m_mapSwfFonts.GetNextAssoc(pos, csTtfIn, pFont);
      if (pFont)
         delete pFont;
   }
   m_mapSwfFonts.RemoveAll();

   //SWFDestructable::CleanupAll();
  
  
   return hr;
}


DWORD CFlashEngine::CommandCall(_TCHAR* pszCommandLine) /* private */ 
{
   HRESULT hr = S_OK;

   // Debug info
   //_tprintf(_T("- Command Call: %s\n"), pszCommandLine);

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
      /*/
      _TCHAR tszMessage[1024];
      _stprintf(tszMessage, _T("! Starting failed.\n  Error: 0x%08x\n"), dwError);
      _tprintf(tszMessage);
      /*/
      hr = COMMAND_START_FAILED;
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

         /*/
         _TCHAR tszMessage[1024];
         _stprintf(tszMessage, _T("! Program failed.\n  Error: 0x%08x  Exit code: 0x%08x\n"), dwError, dwExitCode);
         _tprintf(tszMessage);
         /*/
      
         hr = COMMAND_EXECUTION_FAILED;
      }
   }

   //_TCHAR tszMessage[1024];
   //_stprintf(tszMessage, "Files gelöscht: %d", nCounter);
   //::MessageBox(NULL, tszMessage, NULL, MB_OK);

   return hr;
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
      if (csInstallDir[csInstallDir.GetLength() - 1] != _T('\\'))
         csInstallDir += CString(_T("\\"));
      return csInstallDir;
   }
   else
   {
      // this should not happen, try relative path here
      return CString(_T(".\\"));
   }
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

LPTSTR CFlashEngine::GetTtfName(LOGFONT &lf)
{   
   static _TCHAR tszTtfName[512];


   HKEY hKeyMachine; 
   LONG regErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
                     _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), NULL,KEY_READ,&hKeyMachine); 

	if ( regErr != ERROR_SUCCESS )
   {
      LONG regErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
                        _T("Software\\Microsoft\\Windows\\CurrentVersion\\Fonts"), NULL,KEY_READ,&hKeyMachine); 
   }

   DWORD subValues;
   if (regErr == ERROR_SUCCESS)
   {
      regErr = RegQueryInfoKey(hKeyMachine, NULL, NULL, NULL, NULL, NULL, NULL, &subValues, NULL, NULL, NULL, NULL);
   }

   if (regErr != ERROR_SUCCESS)
   {
      LONG regErr = RegOpenKeyEx (HKEY_CURRENT_USER, 
         _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), NULL,KEY_READ,&hKeyMachine); 
      if ( regErr != ERROR_SUCCESS )
      {
         LONG regErr = RegOpenKeyEx (HKEY_CURRENT_USER, 
            _T("Software\\Microsoft\\Windows\\CurrentVersion\\Fonts"), NULL,KEY_READ,&hKeyMachine); 

      }

      if (regErr == ERROR_SUCCESS)
         regErr = RegQueryInfoKey(hKeyMachine, NULL, NULL, NULL, NULL, NULL, NULL, &subValues, NULL, NULL, NULL, NULL);
      if ( regErr != ERROR_SUCCESS )
      {
         return NULL;
      }
   }

   HRESULT hr = ::CoInitialize(NULL);
   ULONG_PTR token;
   Gdiplus::GdiplusStartupInput gdipsi;
   ZeroMemory(&gdipsi, sizeof Gdiplus::GdiplusStartupInput);
   gdipsi.GdiplusVersion = 1;
   Gdiplus::GdiplusStartup(&token, &gdipsi, NULL);
   HDC hdc = ::CreateCompatibleDC(NULL);

   DWORD dwType;
   _TCHAR tszEntryName[512];
   _TCHAR tszEntryValue[512];
   
   HFONT font = CreateFontIndirect(&lf);
   UINT cbData;       
   LPOUTLINETEXTMETRIC lpoltm;     
   SelectObject(hdc, font);
   cbData = GetOutlineTextMetrics (hdc, 0, NULL);
   DeleteObject(font);
   _TCHAR tszFontFace[512];
   if (cbData == 0) 
   {
      _tcscpy(tszFontFace, lf.lfFaceName);
   }
   else
   {
      lpoltm = (LPOUTLINETEXTMETRIC)LocalAlloc (LPTR, cbData);
      GetOutlineTextMetrics (hdc, cbData, lpoltm);  
      _tcscpy(tszFontFace, (_TCHAR *)((DWORD) lpoltm + lpoltm->otmpFaceName));
      //#ifdef _DEBUG
      //                     if (strstr(faceName.c_str(), "Arial") != NULL)
      //                        MessageBox(NULL, faceName.c_str(), _T("Face name"), MB_OK);
      //#endif
   }

   CString csFontFace = tszFontFace;
   int iStartPos = csFontFace.Find(_T("Bold"));
   if (iStartPos != -1)
   {
      csFontFace.Replace(_T("Bold"), _T("Fett"));
   }
   else
   {
      int iStartPos = csFontFace.Find(_T("Fett"));
      if (iStartPos != -1)
      {
         csFontFace.Replace(_T("Fett"), _T("Bold"));
      }
   }

   iStartPos = csFontFace.Find(_T("Italic"));
   if (iStartPos != -1)
   {
      csFontFace.Replace(_T("Italic"), _T("Kursiv"));
   }
   else
   {
      int iStartPos = csFontFace.Find(_T("Kursiv"));
      if (iStartPos != -1)
      {
         csFontFace.Replace(_T("Kursiv"), _T("Italic"));
      }
   }
   _TCHAR tszFontFace2[512];
   _tcscpy(tszFontFace2, csFontFace);



   bool bFoundFontInfo = false;
   for (int i = 0; i < subValues && !bFoundFontInfo; i++) 
   {       
      unsigned long entryNameLength = 512 * sizeof _TCHAR;
      unsigned long entryValueLength = 512 * sizeof _TCHAR;
      if (RegEnumValue(hKeyMachine, i, tszEntryName, &entryNameLength, 
         NULL, &dwType, (unsigned char *)tszEntryValue, &entryValueLength) != ERROR_SUCCESS)
      {
         continue;
      }
      
      _TCHAR tszFaceName[512];
      _tcscpy(tszFaceName, tszEntryName);
      _TCHAR *p = _tcsrchr(tszFaceName, '(');
      if (p != NULL)
         *p = '\0';

	  if (tszFaceName[_tcslen(tszFaceName) - 1] == _T(' '))
		  tszFaceName[_tcslen(tszFaceName) - 1] = _T('\0');

      if (_tcscmp(tszFontFace, tszFaceName) == 0 || _tcscmp(tszFontFace2, tszFaceName) == 0)
      {
         bFoundFontInfo = true;
         UINT nStringLength = _tcslen(tszEntryValue);
         _tcscpy(tszTtfName, tszEntryValue);
         tszTtfName[nStringLength] = '\0';
      }
      if (_tcscmp(tszFontFace, _T("Wingdings")) == 0 &&
          _tcscmp(tszFaceName, _T("WingDings")) == 0)
      {
         bFoundFontInfo = true;
         UINT nStringLength = _tcslen(tszEntryValue);
         _tcscpy(tszTtfName, tszEntryValue);
         tszTtfName[nStringLength] = '\0';
      }

   }

   ::DeleteDC(hdc);
   Gdiplus::GdiplusShutdown(token);
   ::CoUninitialize();

   RegCloseKey(hKeyMachine);

   if (bFoundFontInfo)
      return tszTtfName;
   else
      return NULL;

}

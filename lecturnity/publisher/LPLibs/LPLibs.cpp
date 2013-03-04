// LPLibs.cpp : Legt die Initialisierungsroutinen für die DLL fest.
//

#include "stdafx.h"
#include "LPLibs.h"
#include "PublisherLibs.h"
#include "Video2Flv.h"
#include "FlashEngine.h"
#include "Lrd2VideoConverter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RETRIEVE_STRING(function, tstring) \
   _TCHAR *##tstring;\
   { \
      DWORD dwTmp = 0; \
      ##function(NULL, &dwTmp); \
      ##tstring = new _TCHAR[dwTmp+1]; \
      dwTmp++; \
      ##function(##tstring, &dwTmp); \
   } \

//
//	Beachten Sie!
//
//		Wird diese DLL dynamisch an die MFC-DLLs gebunden,
//		muss bei allen von dieser DLL exportierten Funktionen,
//		die MFC-Aufrufe durchführen, das Makro AFX_MANAGE_STATE
//		direkt am Beginn der Funktion eingefügt sein.
//
//		Beispiel:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//		// Hier normaler Funktionsrumpf
//		}
//
//		Es ist sehr wichtig, dass dieses Makro in jeder Funktion
//		vor allen MFC-Aufrufen erscheint. Dies bedeutet, dass es
//		als erste Anweisung innerhalb der Funktion ausgeführt werden
//		muss, sogar vor jeglichen Deklarationen von Objektvariablen,
//		da ihre Konstruktoren Aufrufe in die MFC-DLL generieren
//		könnten.
//
//		Siehe MFC Technical Notes 33 und 58 für weitere
//		Details.
//

/////////////////////////////////////////////////////////////////////////////
// CLPLibsApp

BEGIN_MESSAGE_MAP(CLPLibsApp, CWinApp)
	//{{AFX_MSG_MAP(CLPLibsApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLPLibsApp Konstruktion

CLPLibsApp::CLPLibsApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
   m_hMingDll = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CLPLibsApp-Objekt

CLPLibsApp theApp;


UINT LPLibs_Init()
{
   Scorm_ReadSettings();
   // Bugfix 2867: Flash/iPod/LPD export do not use Templates 
   //Template_ReadSettings();

   return S_OK;
}

UINT LPLibs_CleanUp()
{
   return S_OK;
}

#include "lutils.h"

// entry point for invocation with rundll32.exe
void CALLBACK DirectCall()//HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
   bool bTestFlash = true;
   bool bTestFlashMp3 = false;
   bool bTestScorm = false;
   bool bTestTemplates = false;
   bool bTestFlv = false;
   bool bTestMp4 = true;

   
   /*
   C:\WINDOWS\system32\kundll.exe
   LPLibs.dll DirectCall
   C:\Sandbox\lecturnity\publisher\LPLibs\Debug
   */

   if (bTestFlash)
   {

      //_CrtSetBreakAlloc(2);


      CString strSourcePrefix = _T("D:\\documents\\support\\pantherMedia\\Pfister-ganz\\pfister-ga");
      CString strTargetPrefix = _T("D:\\documents\\support\\pantherMedia\\Pfister-ganz\\TargetDocuments\\Flash\\content\\pfister-ga");
      CString strTargetPath   = _T("D:\\documents\\support\\pantherMedia\\Pfister-ganz\\TargetDocuments\\Flash\\");


      HRESULT hr = Flash_SetInputFiles(
         strSourcePrefix+_T(".lrd"), strSourcePrefix+_T(".evq"), strSourcePrefix+_T(".obj"), strSourcePrefix+_T(".lad"));

      if (SUCCEEDED(hr))
         hr = Flash_SetOutputFile(strTargetPrefix+_T(".swf"));

      if (SUCCEEDED(hr))
         Flash_SetStandardNavigationStates(0, 0, 0, 0, 0, 0);

      if (SUCCEEDED(hr))
         hr = Flash_SetSgStandAloneMode(false);

      if (SUCCEEDED(hr))
         Flash_SetWbInputDimension(1024, 768);
         //Flash_SetWbInputDimension(720, 540);
         //Flash_SetWbInputDimension(842, 594);

      if (SUCCEEDED(hr))
         Flash_SetMovieDimension(1024, 768);
         //Flash_SetMovieDimension(720, 540);
         //Flash_SetWbOutputDimension(868, 682);

      if (SUCCEEDED(hr))
         hr = Flash_ConvertFonts();

      if (SUCCEEDED(hr))
         hr = Flash_ConvertImages();

      if (SUCCEEDED(hr))
      {
         CVideo2Flv *pFlvConverter = NULL;
         pFlvConverter = new CVideo2Flv();
         
         if (SUCCEEDED(hr))
            hr = Flash_SetIncreaseKeyframes(false);
         if (SUCCEEDED(hr))
            hr = pFlvConverter->SetInputFiles(NULL, 0, strSourcePrefix+_T(".lad"), 0);
         if (SUCCEEDED(hr))
            hr = pFlvConverter->SetOutputParams(strTargetPrefix+_T(".flv"), 0, 0, 0, 64, false, false);
         if (SUCCEEDED(hr))
            hr = pFlvConverter->PrepareInput();
         if (SUCCEEDED(hr))
            hr = pFlvConverter->PrepareConversion();
         if (SUCCEEDED(hr))
            hr = pFlvConverter->StartConversion();;

         if (FAILED(hr))
            hr = S_OK; // audio must not work
      }

      if (SUCCEEDED(hr))
         hr = Flash_CreateFlexFiles(_T(""), _T(""), false, false, true, false, false, true);

      if (SUCCEEDED(hr))
         hr = Flash_EnableOutputDir(strTargetPath);

      if (SUCCEEDED(hr))
         hr = Flash_BuildMovie(NULL, 0xffeae7d7, 0xff808080, 0xfffffff, NULL, NULL, 0, false);

      if (FAILED(hr))
      {
         _RPT1(_CRT_WARN, "!!! Export failed: 0x%08x\n", hr);
         ::Sleep(2000);
      }

      hr = Flash_Cleanup();

      //Sleep(10000);
   }

   if (bTestFlashMp3) {
       _TCHAR *tszLadFile = _T("D:\\LECTURNITY\\Recordings\\Video-Und-Clip-6-Teil-3\\6-Teil-3_17_01_2008_11_01_42.lad");
       _TCHAR *tszOutputFile = _T("c:\\dummy.mp3");
       CFlashEngine e;
       e.SetInputFiles(NULL, NULL, NULL, tszLadFile);
       e.ConvertAudioToMp3Ds(tszOutputFile, 32);
   }

   if (bTestScorm)
   {
      //_CrtSetBreakAlloc(106);

      HRESULT hr = S_OK;

      if (SUCCEEDED(hr))
         hr = Scorm_Init();
      if (SUCCEEDED(hr))
         hr = Scorm_ReadSettings();


      if (SUCCEEDED(hr))
      {
         // Do anything else here
      }

      if (SUCCEEDED(hr))
         hr = Scorm_WriteSettings();
      if (SUCCEEDED(hr))
         hr = Scorm_CleanUp();

      
      if (FAILED(hr))
      {
         _RPT1(_CRT_WARN, "!!! Export failed: 0x%08x\n", hr);
         ::Sleep(2000);
      }
   }

   if (bTestTemplates)
   {
      HRESULT hr = S_OK;

      if (SUCCEEDED(hr))
         hr = Template_Init();

      if (SUCCEEDED(hr))
         hr = Template_ReadSettings();

      DWORD dwSize = 0;
      if (SUCCEEDED(hr))
         hr = Template_GetSettings(NULL, &dwSize);

      _TCHAR *tszRep = new _TCHAR[dwSize];
      if (SUCCEEDED(hr))
         hr = Template_GetSettings(tszRep, &dwSize);

      if (SUCCEEDED(hr))
         hr = Template_SetSettings(tszRep);

      /*
      CScormSettings2 ss;
      ss.ReadRegistry();
      CString csRep = ss.GetRepresentation();
      ss.ParseFrom(csRep);
      */

      int iDummy = 0;

      //CString csSource = _T("c:\\Programme\\LECTURNITY\\Publisher\\Web Templates\\WebPlayer1\\normal\\__thumbs.html.tmpl");

      /*
      LTextBuffer ltbSource;
      bool success = ltbSource.LoadFile(csSource);

      if (success)
         success = ltbSource.SaveFile(_T("c:\\Temp\\delete.html"), LFILE_TYPE_TEXT_UTF8);
         */

      //LIo::CreateDirs(_T("c:\\Temp"), _T("dummy"));
/*
      if (SUCCEEDED(hr))
         hr = Template_ReadSettings();

      if (SUCCEEDED(hr))
      {
         int nMaxSlidesWidth = -1;
         hr = Template_GetMaxSlidesWidth(1280, 320, 0, &nMaxSlidesWidth);
      }
      if (SUCCEEDED(hr))
      {
         int nMaxSlidesHeight = -1;
         hr = Template_GetMaxSlidesHeight(1024, 320, 0, &nMaxSlidesHeight);
      }

      if (SUCCEEDED(hr))
         hr = Template_Init();
      if (SUCCEEDED(hr))
         hr = Template_SetScormEnabled(false);
      if (SUCCEEDED(hr))
         hr = Template_SetTargetDirectory(NULL);
      _TCHAR tszDirectory = NULL;
      if (SUCCEEDED(hr))
      {
         RETRIEVE_STRING(Template_GetTargetDirectory, tszDirectory);
         _tprintf(_T("tszDirectory: %s\n"), tszDirectory);
      }

      if (SUCCEEDED(hr))
         Template_CleanUp();
         */
   }

   if (bTestFlv) {

       // !! This code does not work with audio only as our FLV mux expects video as first input pin!
       // Furthermore there are two or three checks in this code which make sure that
       // video is present.

       HRESULT hr = S_OK;

       CVideo2Flv *pFlvConverter = new CVideo2Flv();

       //_TCHAR *tszLadFile = _T("D:\\LECTURNITY\\Support\\prog-rausch_30_03_2009_15_31_38.lad");
       //_TCHAR *tszAviFile = _T("D:\\LECTURNITY\\Support\\prog-rausch_30_03_2009_15_31_38.avi");
       //_TCHAR *tszFlvFile = _T("D:\\LECTURNITY\\Support\\prog-mono.flv");

       //_TCHAR *tszLadFile = _T("D:\\LECTURNITY\\Support\\Aufzeichnung5367.lad");
       //_TCHAR *tszAviFile = _T("D:\\LECTURNITY\\Support\\Aufzeichnung5367.avi");
       //_TCHAR *tszFlvFile = _T("D:\\LECTURNITY\\Support\\Aufzeichnung5367.flv");

       //_TCHAR *tszLadFile = _T("D:\\LECTURNITY\\Support\\4416\\20081211_Cop_ESCM.lad");
       //_TCHAR *tszAviFile = NULL;
       //_TCHAR *tszFlvFile = _T("D:\\LECTURNITY\\Support\\4416\\20081211_Cop_ESCM.flv");

       _TCHAR *tszLadFile = _T("D:\\LECTURNITY\\Recordings\\unbenannt_22_07_2009_15_29_45\\unbenannt_22_07_2009_15_29_45.lad");
       _TCHAR *tszAviFile = _T("D:\\LECTURNITY\\Recordings\\unbenannt_22_07_2009_15_29_45\\unbenannt_22_07_2009_15_29_45.avi");
       _TCHAR *tszFlvFile = _T("D:\\LECTURNITY\\Recordings\\unbenannt_22_07_2009_15_29_45\\unbenannt_22_07_2009_15_29_45.flv");

       if (SUCCEEDED(hr))
           hr = pFlvConverter->SetInputFiles(tszAviFile, 0, tszLadFile, 0);

       if (SUCCEEDED(hr)) {
           hr = pFlvConverter->SetOutputParams(
               tszFlvFile, 0, 0, 500, 32, true, false);
       }

       if (SUCCEEDED(hr))
           hr = pFlvConverter->PrepareInput();

       if (SUCCEEDED(hr)) {
           hr = pFlvConverter->PrepareConversion();

           if (FAILED(hr)) {
               if (hr == VFW_E_TYPE_NOT_ACCEPTED)
                   hr = imc_lecturnity_converter_LPLibs_AUDIO_8_16_KHZ_NOT_SUPPORTED;
               else if (hr == E_ACCESSDENIED)
                   hr = imc_lecturnity_converter_LPLibs_FLV_NO_OVERWRITE;
               else if (FAILED(pFlvConverter->CheckForFilters()))
                   hr = imc_lecturnity_converter_LPLibs_VIDEO_FILTERS_MISSING;
           }
       }

       if (SUCCEEDED(hr))
           hr = pFlvConverter->StartConversion();

       while (SUCCEEDED(hr) && pFlvConverter->GetProgress() >= 0 && pFlvConverter->GetProgress() < 10000)
       {
           // everything alright; still converting
           ::Sleep(100);
       }

       if (pFlvConverter->GetProgress() < 0)
       {
           int iFailureBreak = 0;
       }

       delete pFlvConverter;
   }

   if (bTestMp4) {
        HRESULT hr = S_OK;

        CLrd2VideoConverter mp4;
        // sehr kurz:
        //hr = mp4.SetInputFile(_T("D:\\LECTURNITY\\Recordings\\1000_doors_1_30_10_2009_13_13_39\\1000_doors_1_30_10_2009_13_13_39.lrd"));
        
        hr = mp4.SetInputFile(_T("D:\\LECTURNITY\\Recordings\\BrauchbareDreiSeiten_23_10_2008_10_42_27\\BrauchbareDreiSeiten_23_10_2008_10_42_27.lrd"));
        //hr = mp4.SetInputFile(_T("D:\\LECTURNITY\\Recordings\\6-Teil-3_17_01_2008_10_58_50\\6-Teil-3_17_01_2008_10_58_50.lrd"));

        if (SUCCEEDED(hr)) {
            hr = mp4.PrepareConversion(
                _T("D:\\LECTURNITY\\Support\\Main-c_1.mp4"), 
                true, 12.0f, 5, 64, 1020, 240, false, false, false);
        }

        if (SUCCEEDED(hr))
           hr = mp4.StartConversion();

       while (SUCCEEDED(hr) && mp4.GetProgress() >= 0 && mp4.GetProgress() < 10000)
       {
           // everything alright; still converting
           ::Sleep(100);
       }

   }
}

BOOL CLPLibsApp::InitInstance() 
{
   // This is... strange.
   m_hMingDll = NULL;
   m_hMingDll = ::LoadLibrary(_T("ming.dll"));
   bool success = (m_hMingDll != NULL);
   FARPROC pMingDllInit = NULL;
   if (success)
   {
      pMingDllInit = ::GetProcAddress(m_hMingDll, "MingDll_init");
      success = (pMingDllInit != NULL);
   }
   if (success)
   {
      DWORD dwRet = pMingDllInit();
      success = (dwRet == 760402L);
   }

   if (!success)
   {
      if (m_hMingDll)
         ::FreeLibrary(m_hMingDll);
      m_hMingDll = NULL;
      // Crash
      //_ftprintf(NULL, _T("blablubb"), 1, 2, 3, 4);
   }
   	
	return CWinApp::InitInstance();
}

int CLPLibsApp::ExitInstance() 
{
   if (m_hMingDll)
      ::FreeLibrary(m_hMingDll);
   m_hMingDll = NULL;
   
	return CWinApp::ExitInstance();
}

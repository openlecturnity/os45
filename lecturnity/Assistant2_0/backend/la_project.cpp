/*********************************************************************

  program  : la_project.cpp
  authors  : Gabriela Maass
  date     : 18.06.2002
  revision : $Id: la_project.cpp,v 1.264 2010-10-12 06:20:54 maass Exp $
  desc     : 
  
**********************************************************************/

#pragma warning( disable: 4786)


#include "StdAfx.h"
#include <io.h>
#include <exception>
#include "commctrl.h" 

#include "la_registry.h"
#include "..\MainFrm.h"
#include "..\DocumentManager.h"
#include "..\MissingFontsDlg.h"
#include "..\AssistantDoc.h"
#include "la_project.h"
#include "la_cliplist.h"
#include "la_registry.h"
#include "mlb_types.h"
#include "mlb_folder.h"
#include "mlb_page.h"
#include "mlb_misc.h"

#include "..\..\Studio\Studio.h"
#include "..\..\Studio\Resource.h"
#include "..\..\Editor\EditorDoc.h"
#include "..\..\Studio\MainFrm.h"

#include "lutils.h"             // lutils
#include "LanguageSupport.h"    // lsutl32
#include "MiscFunctions.h"      // filesdk
#include "LrdFile.h"            // filesdk
#include "securityutils.h"      // lsutl32


//#include "filesdkexport.h"

// PZI:
#include "ScreengrabbingExtendedExternal.h" // lsutl32

#import "..\..\pptimport\batconvcore.dll"

#define ASSISTANT_REGISTRY _T("Software\\imc AG\\LECTURNITY\\Assistant")

#ifdef _DEBUG
#define _DEBUGFILE
#endif

//// general global project variables
ASSISTANT::Project* ASSISTANT::Project::active = NULL;

//// global project variables for audio and video recording
AvGrabber *ASSISTANT::Project::avgrabber_       = NULL;

//// Array to store audio devices
AudioDevice **ASSISTANT::Project::audioDevices_ = NULL;
//// Size of audio device array
int ASSISTANT::Project::audioDeviceCount_       = 0;

//// Array to store video device names
TCHAR **ASSISTANT::Project::videoDeviceNames_    = NULL;
//// Size of video device array
int ASSISTANT::Project::videoDeviceCount_       = 0;


//// Array to store wdm device names
TCHAR **ASSISTANT::Project::wdmDeviceNames_ = NULL;
//// Size of wdm device array
int ASSISTANT::Project::wdmDeviceCount_           = 0;

int ASSISTANT::Project::audioIntervall    = 250;
int ASSISTANT::Project::videoIntervall    = 1100;
int ASSISTANT::Project::durationIntervall = 1000;

bool ASSISTANT::Project::audioIsInitialized    = false;
bool ASSISTANT::Project::volumeSliderIsActive  = false;
bool ASSISTANT::Project::optionsDialogIsActive = false;

//// global project variables for page metadata
char *ASSISTANT::Project::thumbData = NULL;

BITMAPINFO *ASSISTANT::Project::thumbInfo = NULL;

// Maximum AVI data size: 1900 MB
#define MAX_AVI_FILE_SIZE 1992294400

//// project konstructor
ASSISTANT::Project::Project(int doWithSG, bool bFreezeImages, 
                            bool bFreezeMaster, bool bFreezeDocument, const _TCHAR *tszFixedLrdName) 
{
   InitVariables();
   
   doWithSG_ = doWithSG;
   m_bFreezeImages = bFreezeImages;
   m_bFreezeMaster = bFreezeMaster;
   m_bFreezeDocument = bFreezeDocument;
   recording = false;

   m_bFixedTargetName = false;
   if (tszFixedLrdName != NULL && _tcslen(tszFixedLrdName) > 0) {
       m_bFixedTargetName = true;
       m_csLrdName = tszFixedLrdName;
   }
   
   ::CoInitialize(NULL);

   ActivateAudioAndVideo();

   m_bDoPureScreenGrabbing = false;
}

//// project destructor
ASSISTANT::Project::~Project() 
{
    CloseDocumentsAndPresentations();

    WriteAudioSettings();
    WriteVideoSettings();
    WriteSgRegistrySettings();
    WriteSgHotkeys();
    ActivateStartStopHotKey(false);
    WriteGridSettings();

    DeleteCodecInfo();
    DeleteSgCodecInfo();
    DeleteMixerSources();

    DeleteWdmInterfaces();

    mediaTypeSize_ = 0;
    if (mediaTypeData_)
        delete[] mediaTypeData_;
    mediaTypeData_ = NULL;

    videoHeaderSize_ = 0;
    if (videoHeaderData_)
        delete[] videoHeaderData_;
    videoHeaderData_ = NULL;

    ReleaseVariables();

    ::CoUninitialize();
}

//// initialiting project variables
void ASSISTANT::Project::InitVariables()
{ 
   projectName_.LoadString(IDS_DEFAULT_PROJECT);
   ChangeProjectName(projectName_);
   
   projectPath_.Empty();
   dataPath_.Empty();
   m_ssLastCopyPath.Empty();
   
   recordPath_.Empty();
   
   recCount = 0;
   isAudioDeviceSet_ = false;
   lastRecordedTime_ = -1;
   
   isEncrypted_ = true;
   presentationCount = 0;
   clipCount = 0;
   startTime.Empty();
   startDate.Empty();
   
   pasteBuffer_.RemoveAll();
   
   m_bFreezeImages = false;
   m_bFreezeMaster = false;
   m_bFreezeDocument = false;
   m_bWriteFinished = true;
   loadFinished_ = true;
   
   nodeName_.Empty();
   
   pageCount = 0;
   
   objectID_ = 0;
   SetChanged(false);
   
   documents_.RemoveAll();
   activeDocument_ = -1;
   
   presentations_.RemoveAll();
   activePresentation_ = NULL;
   
   imageList_      = new ImageList();
   
   fontList_       = new FontList();
   
   recording      = false;
   paused         = false;
   
   metadata_      = new ProjectMetadata();
   
   m_ssLastCursor = _T("none");
   
   lastDocumentPath.Empty();
   lastImagePath.Empty();
   
   lastActivePage = NULL;
   
   audioFramerate_   = 0;
   audioBitrate_     = 0;
   audioChannels_    = 1;
   audioDevice_      = 0;

   snapChecked_      = 0;
   spacingValue_     = 20;
   displayGrid_      = 0;

   
   useVfW_               = false;
   
   isVideoEnabled_       = false;
   isAdvancedSyncSet_    = false;
   hasAVOffset_           = false;
   avOffset_             = 0;
   isMonitorEnabled_     = false;
   isVideoDeviceSet_     = false;
   videoFramerate_       = 25.0;
   videoDevice_          = 0;
   hasFormatDialog_      = false;
   hasSourceDialog_      = false;
   hasDisplayDialog_     = false;
   
   isWdmDeviceSet_     = false;
   wdmDevice_          = _T("");
   wdmInterfaceCount_  = 0;
   wdmInterfaces_      = NULL;
   
   mediaTypeSize_      = 0;
   videoHeaderSize_    = 0;
   mediaTypeData_      = NULL;
   videoHeaderData_    = NULL;

   isCodecEnabled_     = false;
   codecInfo_          = NULL;
   codecCount_         = 0;
   codecNumber_        = -1;
   videoCodec_         = _T("iv50");
   videoQuality_       = 75;
   keyframeDist_       = 15;
   videoDatarate_      = 500;
   useDatarate_        = false;
   useKeyframes_       = true;
   bSupportsQuality_   = false;
   bSupportsKeyframes_ = false;
   bSupportsDatarate_  = false;
   hasAboutDialog_     = false;
   hasConfigureDialog_ = false;
   
   audioMixer_ = NULL;
   mixerSources_ = NULL;
   mixerSourceCount_ = 0;
   mixerSource_ = 0;
   audioVolume_ = 0;
   
   /*/
   useAppPlay = false;
   useJedas = false;
   /*/
   
   isScreenGrabbingActive_ = false;
   isSgThreadRunning_ = false;
   isAnnotationMode_ = false;
   
   isSgCodecEnabled_     = false;
   sgCodecInfo_          = NULL;
   sgCodecCount_         = 0;
   sgCodecNumber_        = -1;
   sgCodec_              = _T("lsgc");
   sgQuality_            = 75;
   sgKeyframeDist_       = 30;
   sgDatarate_           = 500;
   sgUseDatarate_        = false;
   sgUseKeyframes_       = true;
   bSgSupportsQuality_   = false;
   bSgSupportsKeyframes_ = false;
   bSgSupportsDatarate_  = false;
   hasSgAboutDialog_     = false;
   hasSgConfigureDialog_ = false;
   
   ASSISTANT::AudioEntry
      audioEntry;
   if (audioEntry.Read() == 0)
      isEncrypted_ = false;
   else
      isEncrypted_ = true;
   
   evqFp_ = NULL;
   objFp_ = NULL;
   
   hotKeyThread_ = -1;
   
   mouseAudioEffect_    = false;
   mouseVisualEffect_   = false;
   hasStaticFramerate_  = true;
   grabAtFixedPos_      = false;
   
   reportDroppedFrames_    = false;
   deleteAllOnInteraction_ = true;
   hideWhilePanning_       = false;   
   selectOnlyMainWindow_   = true;
   noHardwareAcceleration_ = false;
   quickCapture_           = false;
   pauseVideo_             = false;
   
   staticFramerate_   = 10;
   adaptiveFramerate_ = 10;
   startAtPosX_       = 0;
   startAtPosY_       = 0;
   startWithWidth_    = 320;
   startWithHeight_   = 240;
   m_rcSgRectangle.SetRect(0, 0, 320, 240);
   adaptWidth_        = 320;
   adaptHeight_       = 240;
   
   CString csNone;
   csNone.LoadString(IDS_SG_HOTKEYS_CMB_NONE);
   m_hkStartStop.vKeyString = csNone;
   m_hkStartStop.hasCtrl = false;
   m_hkStartStop.hasShift = false;
   m_hkStartStop.hasAlt = false;
   m_hkStartStop.id = ID_TOOL_SCREENGRABBING;

   isFullScreenAtStart_ = false;   
   FillLogFont(m_logFont, _T("Arial"), 12, _T("normal"), _T("roman"));
}

void ASSISTANT::Project::ReleaseVariables()
{ 
   if (fontList_)
      delete fontList_;
   fontList_ = NULL;
   
   projectName_.Empty();
   projectPath_.Empty();
   projectPrefix_.Empty();
   dataPath_.Empty();
   recordPath_.Empty();
   
   if (metadata_) 
       delete metadata_;
   metadata_ = NULL;

   ReleaseDocumentVariables();
}

void ASSISTANT::Project::ReleaseDocumentVariables()
{
   ClearPasteBuffer();

   if (objFp_) {
      objFp_->Flush();
      CloseHandle(m_hObjFile);
      delete objFp_;
   }
   objFp_ = NULL;

   if (evqFp_)
       fclose(evqFp_);
   evqFp_ = NULL;

   for (int i = 0; i < documents_.GetSize(); ++i)
      delete documents_[i];
   documents_.RemoveAll();
   
   activeDocument_ = -1;
   
   for (i = 0; i < presentations_.GetSize(); ++i)
      delete presentations_[i];
   presentations_.RemoveAll();

   activePresentation_ = NULL;
   
   thumbs_.Clear();
}

//// set project parameter

void ASSISTANT::Project::UpdateNewProject()
{
   
   ReadSgHotkeys();
   ReadSgRegistrySettings();
   SetSgCodec();
}

//// Initialize avgrabber-module (once only).
void ASSISTANT::Project::InitAvGrabber()
{
   if (avgrabber_)
      DeleteAvGrabber();
   
   try
   {
      avgrabber_ = new AvGrabber(&CStudioApp::GetLiveContext());
   }
   catch (exception &e)
   {
      audioDeviceCount_  = 0;
      audioDevices_      = NULL;
      videoDeviceCount_  = 0;
      if (avgrabber_)
         delete avgrabber_;
      avgrabber_ = NULL;
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
      return;
   }
   
   try 
   {
      audioDeviceCount_ = avgrabber_->getAudioDeviceCount();
      
      if (audioDeviceCount_ > 0)
      {
         audioDevices_ = new AudioDevice*[audioDeviceCount_];
         for (int i = 0; i < audioDeviceCount_; ++i)
            audioDevices_[i] = new AudioDevice;
         
         avgrabber_->getAudioDevices(audioDevices_);
      }
      else
      {
         CString csMessage;
         csMessage.LoadString(IDS_NO_AUDIO_DEVICE);
         MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      }
   }
   catch (exception &e)
   {
      audioDeviceCount_ = 0;
      audioDevices_     = NULL;
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
      return;
   }
   
   try 
   {
      videoDeviceCount_ = avgrabber_->getVideoDeviceCount();
      if (videoDeviceCount_ > 0)
      {
         videoDeviceNames_ = new TCHAR*[videoDeviceCount_];
         for (int i = 0; i < videoDeviceCount_; ++i)
            videoDeviceNames_[i] = new TCHAR[256];
         avgrabber_->getVideoDeviceList(videoDeviceNames_, 256);
      }
      else
      {
         videoDeviceCount_ = 0;
      }
   }
   catch (exception &e)
   {
      if (videoDeviceNames_)
      {
         for (int i = 0; i < videoDeviceCount_; ++i)
            delete[] videoDeviceNames_[i];
         delete[] videoDeviceNames_;
         videoDeviceNames_ = NULL;
      }
      videoDeviceCount_ = 0;
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
   }
   
   
   try 
   {
      if (avgrabber_->isWdmEnabled())
      {
         wdmDeviceCount_ = avgrabber_->getWdmDeviceCount();
         
         if (wdmDeviceCount_ > 0)
         {
            wdmDeviceNames_ = new TCHAR*[wdmDeviceCount_];
            for (int i = 0; i < wdmDeviceCount_; ++i)
               wdmDeviceNames_[i] = new TCHAR[256];
            avgrabber_->getWdmDeviceNames(wdmDeviceNames_, 256);
         }
         else
         {
            wdmDeviceCount_ = 0;
         }
      }
      else
      {
         wdmDeviceCount_ = 0;
      }
   }
   catch (exception &e)
   {
      if (wdmDeviceNames_)
      {
         for (int i = 0; i < wdmDeviceCount_; ++i)
            delete[] wdmDeviceNames_[i];
         delete[] wdmDeviceNames_;
         wdmDeviceNames_ = NULL;
      }
      wdmDeviceCount_ = 0;
      MessageBoxA(NULL, e.what(), "InitAvGrabber", MB_OK | MB_TOPMOST);
   }
   
}

//// delete avgrabber-module (once only).
void ASSISTANT::Project::DeleteAvGrabber()
{
   if (avgrabber_)
   {
      if (audioDevices_)
      {
         for (int i = 0; i < audioDeviceCount_; ++i)
            delete audioDevices_[i];
         delete[] audioDevices_;
      }
      audioDevices_     = NULL;
      audioDeviceCount_ = 0;
      
      if (videoDeviceNames_)
      {
         for (int i = 0; i < videoDeviceCount_; ++i)
            delete[] videoDeviceNames_[i];
         delete[] videoDeviceNames_;
         videoDeviceNames_ = NULL;
      }
      videoDeviceCount_ = 0;
      
      if (wdmDeviceNames_)
      {
         for (int i = 0; i < wdmDeviceCount_; ++i)
            delete[] wdmDeviceNames_[i];
         delete[] wdmDeviceNames_;
         wdmDeviceNames_ = NULL;
      }
      wdmDeviceCount_ = 0;
      
      delete avgrabber_;
      avgrabber_ = NULL;
   }
}

static LRESULT CALLBACK MixerControl(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   if (!ASSISTANT::Project::audioIsInitialized) 
      return 0;

   if (ASSISTANT::Project::active != NULL && !ASSISTANT::Project::active->recording)
   {  
      AudioMixer *audioMixer = ASSISTANT::Project::active->GetAudioMixer();
      if (audioMixer && audioMixer->isEnabled())
      {
         int mixerSource_ = audioMixer->getSource();
         if (mixerSource_ != ASSISTANT::Project::active->GetMixerSource())
         {
            ASSISTANT::Project::active->SetMixerSource(mixerSource_);
            CString csMessage;
            csMessage.LoadString(IDS_MIXER_SOURCE_CHANGED);
            CString csCaption;
            csCaption.LoadString(IDS_WARNING);
            MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
         }
      }
   }

   return 0;
}

void ASSISTANT::Project::InitAudioVideo()
{
   if (!avgrabber_)
      return;
   
   audioIsInitialized = false;

   ActivateAudioDevice();

   if (useVfW_)
      SetVideoDevice();
   else
      SetWdmDevice(true);

   audioIsInitialized = true;
}

void ASSISTANT::Project::ReleaseAudioVideo()
{
   if (!avgrabber_)
      return;
   
   audioIsInitialized = false;
   
   audioMixer_ = NULL;
   
   DeleteMixerSources();
   DeleteCodecInfo();
   
   isVideoDeviceSet_ = false;
   
}

void ASSISTANT::Project::ActivateAudioAndVideo()
{
   ReadAudioSettings();
   ReadVideoSettings(true);
   ReadGridSettings();

   InitAudioVideo();

   ReadSgHotkeys();
   ReadSgRegistrySettings();
   RetrieveSgCodecs();
   UpdateSgModul();

   OpenFromRegistry();
   ReadMetadata(NULL);
}

void ASSISTANT::Project::DeActivateAudioAndVideo()
{
   DeActivatePage();
   
   /*/
   if (avgrabber_)
      DestroyAudioControl();
   /*/
   
   ReleaseAudioVideo();

   for (int i = 0; i < documents_.GetSize(); ++i)
      delete documents_[i];
   documents_.RemoveAll();


   for (i = 0; i < presentations_.GetSize(); ++i)
      delete presentations_[i];
   presentations_.RemoveAll();
   
}
// Functions called from main.cpp concerning project


bool ASSISTANT::Project::LoadDocumentsAndPresentations()
{
    OpenFromRegistry();
    imageList_ = new ImageList();

    return true;
}

bool ASSISTANT::Project::CloseDocumentsAndPresentations()
{

    bool bReturn = SaveModified();

    if (bReturn) {
        // if bReturn is false, user clicked "Cancel" and the project
        // is not closed.

        lastActivePage = NULL;

        if (imageList_) {
            delete imageList_;
            imageList_ = NULL;
        }

        nodeName_.Empty();

        ReleaseDocumentVariables();

        SetChanged(false);
    }

    return bReturn;
}

bool ASSISTANT::Project::SaveModified()
{
    bool bIsRegistryProject = CAssistantDoc::IsDoRegistryProject();

    bool bReturn = true;

    if (IsRecording()) {
        CString csMessage;
        csMessage.LoadString(IDS_STOP_RECORDING);
        CString csCaption;
        csCaption.LoadString(IDS_WARNING);
        int iAnswer = MessageBox(NULL, csMessage, csCaption, MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);

        if (iAnswer == IDNO)
            return false;
        else
            StopRecording();
    }

    if (HasChanged()) {
        bReturn = SaveChangedDocuments();
        // Set all documents to unchaged
        if (bReturn) {
            // all the dialogs from SaveChangedDocuments are answered with
            // yes or no
            for (int i = 0; i < documents_.GetSize(); ++i) {
                if (documents_[i])
                    documents_[i]->SetAllUnchanged();
            }
            if (projectAction_ > 0 && !bIsRegistryProject) {
                // Saving (question for it) is disabled with a "registry project" (new Studio)

                CString csMessage;
                csMessage.Format(IDS_SAVE_PROJECT, GetProjectName());
                CString csCaption;
                csCaption.LoadString(IDS_CONFIRM_SAVE);
                int iRet = MessageBox(NULL, csMessage, csCaption, MB_YESNOCANCEL | MB_TOPMOST);
                if (iRet == IDYES)
                    Save(NULL, false);
                else if (iRet == IDCANCEL)
                    bReturn = false;
            }
        }
    }

    return bReturn;
}

HRESULT ASSISTANT::Project::GetMetadata(CString &csTitle, CString &csAuthor, 
                                        CString &csProducer, CString &csKeywords)
{
   if (metadata_ == NULL)
      return E_FAIL;

   csTitle = metadata_->GetTitle();
   csAuthor = metadata_->GetAuthor();
   csProducer = metadata_->GetCreator();
   csKeywords = metadata_->GetKeywords();

   return S_OK;
}

HRESULT ASSISTANT::Project::SetMetadata(CString &csTitle, CString &csAuthor, 
                                        CString &csProducer, CString &csKeywords)
{
   if (metadata_ == NULL)
      return E_FAIL;

   metadata_->SetTitle(csTitle);
   metadata_->SetAuthor(csAuthor);
   metadata_->SetCreator(csProducer);
   metadata_->SetKeywords(csKeywords);
   
   WriteMetadata();
   SetChanged(true);

   return S_OK;
}

HRESULT ASSISTANT::Project::Copy(LPCTSTR szCopyFilename, CProgressDialogA *pProgress)
{
   CString
      sgmlString,
      suffix,
      dialogTitle,
      actDocname,
      dialogMessage;
   bool 
      bShowProgress = false;
   
   // VerifyInternationalFilename is called in CheckNewFilename!
   CString csNewProjectName = szCopyFilename;

   CString csNewProjectPath = csNewProjectName;
   ASSISTANT::GetPath(csNewProjectPath);
   
   CString csNewProjectPrefix = csNewProjectName;
   ASSISTANT::GetName(csNewProjectPrefix);
   ASSISTANT::GetPrefix(csNewProjectPrefix);
   
   
   float fDocumentStep = 0;
   float fPresentationRange = 0;
   if (presentations_.GetSize() == 0 && documents_.GetSize() > 0)
      fDocumentStep = 100 / documents_.GetSize();
   else if (presentations_.GetSize() > 0 && documents_.GetSize() == 0)
      fPresentationRange = 100;
   else
   {
      fDocumentStep = 25 / documents_.GetSize();
      fPresentationRange = 75;
   }

   CString csNewDocumentDirectory = csNewProjectPath + _T("\\") + csNewProjectPrefix;

   if (!ASSISTANT::DirectoryExist(csNewDocumentDirectory))
      CreateDirectory(csNewDocumentDirectory, NULL);
   
   bool bOverwriteAll = false;
   bool bDoSave = true;
   bool bCancelCopy = false;
   for (int i = 0; i < documents_.GetSize() && !bCancelCopy; ++i)
   {
      // create new document name
      _TCHAR szDocumentDir[_MAX_PATH];
      _stprintf(szDocumentDir, _T("%s\\lsd_%d\\"), csNewDocumentDirectory, i);

      if (!ASSISTANT::DirectoryExist(szDocumentDir))
         CreateDirectory(szDocumentDir, NULL);
      
      CString csNewLsdName = szDocumentDir;
      csNewLsdName += documents_[i]->GetDocumentName();
      
      bDoSave = true;
      if (!bOverwriteAll && ASSISTANT::FileExist(csNewLsdName))
      {
         /*/ TODO /*/
         CString csMessage;
         csMessage.Format(IDS_OVERWRITE_FILE, csNewLsdName);
         CString csCaption;
         //csCaption.LoadString();
         int iAnswer = MessageBox(NULL, csMessage, csCaption, MB_YESNOCANCEL | MB_ICONQUESTION | MB_TOPMOST);

         if (iAnswer == IDYES)
            bDoSave = true;
         else if (iAnswer == IDNO)
            bDoSave = false;
         else if (iAnswer == IDCANCEL)
            bCancelCopy = true;
      }

      if (!bCancelCopy && (bDoSave || bOverwriteAll))
      {
         CString csProgressText;
         CString csLsdName = documents_[i]->GetDocumentName();
         ASSISTANT::GetName(csLsdName);
         
         if (!documents_[i]->WriteLSD(csNewLsdName, false))
         {
            CString csMessage;
            csMessage.Format(IDS_ERROR_SAVE_DOCUMENT, csNewLsdName);
            MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
         }
         else
         {
            OpenCloseMetadata(documents_[i], csNewLsdName);
         }
      }
      if (pProgress != NULL)
      {
         float fActualPosition = (i * fDocumentStep);
         pProgress->SetPos((int)fActualPosition);
      }
   }
   
   int iTotalFilesCount = 0;
   float fPrepareStep = (0.25 * fPresentationRange) / presentations_.GetSize();
   // parse object files for image names
   for (i = 0; i < presentations_.GetSize() && !bCancelCopy; ++i)
   { 
      PresentationInfo *pPresentationInfo = presentations_[i];
      
      CString csPresentationName = pPresentationInfo->GetFilename();
      ASSISTANT::GetName(csPresentationName);
      
      pPresentationInfo->FillFileList();
      iTotalFilesCount += pPresentationInfo->GetFileCount();
      if (pProgress != NULL)
      {  
         float fActualPosition = (documents_.GetSize() * fDocumentStep) + (i * fPrepareStep);
         pProgress->SetPos((int)fActualPosition);
      }
      
   }
   
   float fFileStep = (0.75 * fPresentationRange) / iTotalFilesCount;
   float fActualPosition = (documents_.GetSize() * fDocumentStep) + (0.25 * fPresentationRange);
   // copy all files belonging to prosentation
   for (i = 0; i < presentations_.GetSize() && !bCancelCopy; ++i)
   { 
      PresentationInfo *pPresentationInfo = presentations_[i];
      
      _TCHAR szPresentationDir[_MAX_PATH];
      _stprintf(szPresentationDir, _T("%s\\lrd_%d\\"), csNewDocumentDirectory, i);
      
      pPresentationInfo->CopyFiles(szPresentationDir, fFileStep, fActualPosition, bOverwriteAll, bCancelCopy, pProgress);
      pPresentationInfo->EmptyFileList();
   }
   
   if (!bCancelCopy)
   {
      WriteLapFile(csNewProjectName, true); 
   }
   
   return S_OK;
}

// Functions called from main.cpp concerning active document

void ASSISTANT::Project::NewDocument()
{
   Document *pDocument = new Document(GetObjectID());
   pDocument->SetChanged(true);

   CStringArray arDocumentNames;
   for (int i = 0; i < documents_.GetSize(); ++i)
       arDocumentNames.Add(documents_[i]->GetContainerName());

   CString csContainerName;
   csContainerName.LoadString(IDS_DOCUMENT);
   ASSISTANT::CalculateNextPrefix(csContainerName, arDocumentNames);

   pDocument->SetName(csContainerName);

   if (activeDocument_ != -1)
      ++activeDocument_;
   else
      activeDocument_ = documents_.GetSize();
   
   documents_.InsertAt(activeDocument_, pDocument);
   
   SaveToRegistry();
   SetChanged(true);
   
   InsertNewPage();

   ActivatePage();
   
   return;
}

void ASSISTANT::Project::AddDocument(LPCTSTR filename, int pos, bool showProgress, bool bDoLoad)
{

    if (!filename || (_tcslen(filename) == 0))
        return;

    // search for duplikate in document list
    Document *pDocument = NULL;
    bool bDocumentAlreadyInserted = false;
    for (int i = 0; i < documents_.GetSize(); ++i)
    {
        CString oldDocumentName = documents_[i]->GetDocumentPath();
        oldDocumentName += _T("\\");
        oldDocumentName += documents_[i]->GetDocumentName();
        if (oldDocumentName == filename)
        {
            if (!documents_[i]->IsLoaded()) {
                pDocument = documents_[i];
                bDocumentAlreadyInserted = true;
            }
            else if (bDoLoad) {
                CString csMessage;
                csMessage.LoadString(IDS_DOCUMENT_ALREADY_LOADED);
                CString csCaption;
                csCaption.LoadString(IDS_WARNING);
                MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
                return;
            }
            else
                return;
        }
    }

    // Open document
    if (pDocument == NULL)
        pDocument = new Document(GetObjectID(), filename);

    if (bDoLoad) {
        HRESULT hr = S_OK;
        if (showProgress)
            hr = pDocument->CreateLoadThread();
        else 
            hr = pDocument->Load();

        if (FAILED(hr)) {
            delete pDocument;
            return;
        }
    }
    else {
        HRESULT hr = S_OK;
        hr = pDocument->LoadInformation();
        if (FAILED(hr)) {
            delete pDocument;
            return;
        }
    }

    if (!bDocumentAlreadyInserted) {
        if (pos == BEGIN)
            activeDocument_ = 0;
        else if (pos == END)
            activeDocument_ = documents_.GetSize();
        else 
        {
            if (activeDocument_ != -1)
                ++activeDocument_;
            else
                activeDocument_ = 0;
        }
        documents_.InsertAt(activeDocument_, pDocument);
        lastDocumentPath = pDocument->GetDocumentPath();

        SaveToRegistry();
        ActivatePage();

        SetChanged(true);
    }

    if (fontList_->IncludeUnsupportedFonts())
    {
        ShowFontError();
    }

    return;
}

bool ASSISTANT::Project::CloseDocument(bool bAskForSave)
{
    bool bCloseSuccess = false;

    Document *document = GetActiveDocument();
    if (document) {

        // find out if the active page is part of the document
        bool bDocumentContainsActivePage = false;
        ASSISTANT::Page *pActiveDocumentPage = document->GetActivePage();
        if (lastActivePage && (lastActivePage == pActiveDocumentPage)) {
            lastActivePage->DeActivate();
            lastActivePage = NULL;
            bDocumentContainsActivePage = true;
        }

        // close document
        bCloseSuccess = document->Close(bAskForSave);

        // set the active page
        if (bCloseSuccess) {
            ActivatePage();
        }
        else if (bDocumentContainsActivePage) {
            lastActivePage = pActiveDocumentPage;
            lastActivePage->Activate();
        }
    }

    return bCloseSuccess;
}

UINT ASSISTANT::Project::WaitForInstallationFinished(LPVOID pParam) {
    ThreadParam *pThreadParam = (ThreadParam *)pParam;

    if (!pThreadParam)
        return 0;


    if (pThreadParam->pMainWnd && pThreadParam->pMainWnd->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
        ((CMainFrame *)pThreadParam->pMainWnd)->EnableWindow(FALSE);

    // SUCCESS: Wait until the process was finished
    DWORD dwExitCode;
    while (GetExitCodeProcess(pThreadParam->hProcess, &dwExitCode)) {
        if (dwExitCode != STILL_ACTIVE)
            break;
        else
            Sleep(100);
    }

    if (pThreadParam->pMainWnd && pThreadParam->pMainWnd->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
        ((CMainFrame *)pThreadParam->pMainWnd)->EnableWindow(TRUE);

    if (pThreadParam)
        delete pThreadParam;

    return 0;
}

UINT ASSISTANT::Project::WaitClickElementInfoThread(LPVOID pParam) {
    WaitForClickInfoStruct *pStruct = (WaitForClickInfoStruct *)pParam;

    if (pStruct == NULL)
        return 0;

    WaitForSingleObject(pStruct->hProgressEvent, 1000);

    CSingleProgressDialog *pProgress = pStruct->pProgress;
    AvGrabber *pAvGrabber = pStruct->pAvGrabber;

    if (pAvGrabber != NULL)
        pAvGrabber->WaitClickElementInfo(pProgress);

    if (pProgress != NULL && pProgress->GetSafeHwnd())
        PostMessage(pProgress->GetSafeHwnd(), WM_USER, CProgressDialogA::END_DIALOG, NULL);
    
    return 0;
}

bool ASSISTANT::Project::ImportPPT(LPCTSTR szPowerPointName, bool bAskForLsd, bool bDoRecord)
{
    bool bSuccessResult = true;

   // lsd filename is: source directory + ppt-prefix + ".lsd"
   CString lsdPath;
   ASSISTANT::GetSourceDirectory(lsdPath);
   if (lsdPath.IsEmpty())         // there is no lecturnity home directory
   {
      ASSISTANT::GetAppData(lsdPath);
      
      if (lsdPath.IsEmpty())      // if there is no application data directory uns directory of ppt file
      {
         lsdPath = szPowerPointName;
         ASSISTANT::GetPath(lsdPath);
      }
      else                          // use Lecturnity sub directory of application data directory
      {
         lsdPath += _T("\\Lecturnity");
      }
   }
   
   CString strTempName = szPowerPointName;
   ASSISTANT::GetPrefix(strTempName);
   ASSISTANT::GetName(strTempName);

   // delete blanks before "."
   strTempName.TrimRight();

   CString strLsdFilename = strTempName + _T(".lsd");
   

   if (bAskForLsd)
   {
      CString csFilename = strLsdFilename;
      ASSISTANT::GetSaveFilename(IDS_LSD_FILTER, csFilename, _T(".lsd"), _T("LSD"));
      if (csFilename.IsEmpty())
      {
         return false;
      }
      strLsdFilename = csFilename;
   }
   else
   {
      CString csLsdPath;
      if (_taccess(lsdPath,00) == -1)
         CreateDirectory(lsdPath, NULL);

      csLsdPath = lsdPath + _T("\\") + strTempName;
      if (_taccess(csLsdPath,00) == -1)
         CreateDirectory(csLsdPath, NULL);
      
      strLsdFilename = csLsdPath + _T("\\") + strTempName + _T(".lsd");
      
   }

   // delete blanks before "."
   int iPointPos = strLsdFilename.ReverseFind(_T('.'));
   CString csPrefix = strLsdFilename.Left(iPointPos);
   csPrefix.TrimRight();
   CString csSuffix = strLsdFilename.Right(strLsdFilename.GetLength() - iPointPos);
   strLsdFilename = csPrefix + csSuffix;
   
   HRESULT hresult = S_OK;
   CLSID clsid;
   
   // get component CLSID
   if (SUCCEEDED(hresult))
      hresult = CLSIDFromProgID(OLESTR("batch_ppt2lsd.CBatchConvCore"), &clsid);
   
   // the class we defined in dll
   batch_ppt2lsd::_CBatchConvCore *t;
   
   // create a class instantce
   if (SUCCEEDED(hresult))
      hresult = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER,
      __uuidof(batch_ppt2lsd::_CBatchConvCore), (LPVOID *) &t);
   
   if(FAILED(hresult))
   {
      MessageBoxA(NULL, "Component service initialize failed.", NULL, MB_OK | MB_TOPMOST);
   }
   
   if (SUCCEEDED(hresult))
   {
      // ppt filename should be in unicode
      WCHAR *wszText = NULL;
      int nLen = _tcslen(szPowerPointName);
      wszText = new WCHAR[nLen + 1];
#ifndef _UNICODE
      ::MultiByteToWideChar(CP_ACP, 0, szPowerPointName, -1, wszText, nLen + 1);
#else
      wcscpy(wszText, szPowerPointName);
#endif
   
      CString csParentHwnd;
      csParentHwnd.Format(_T("%d"), AfxGetMainWnd()->m_hWnd);
      int errID = t->Launch(wszText, (LPCTSTR)strLsdFilename, (LPCTSTR)csParentHwnd, bDoRecord);
   
      // Release
      t->Release();
   
      if (errID == 0)
      {
         this->AddDocument((LPCTSTR)strLsdFilename, END, true, true);
      }
      else if (errID == 1)
      {
          bSuccessResult = false;
         // nothing to do -> cancel pressed.
      }
      else if (errID == 21)   // PowerPoint is not installed
      {
         CString csMessage;
         csMessage.LoadString(IDS_ERROR_POWERPOINTNOTINSTALLED);
         CString csCaption;
         csCaption.LoadString(IDS_ERROR);
         MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONERROR | MB_TOPMOST);
         
         bSuccessResult = false;
      }
      else if (errID == 33)   // PowerPoint with wrong version is open
      {
         CString csMessage;
         csMessage.LoadString(IDS_ERROR_WRONGPOWERPOINT);
         CString csCaption;
         csCaption.LoadString(IDS_WARNING);
         MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
         
         bSuccessResult = false;
      }
      else if (errID == 34) {
          // There is no PowerPoint which can open pptx files (1) or
          // the default PowerPoint application is not able to open pptx files (2).

          // find the current reason (1) or (2)
          bool bPowerPoint2007IsInstalled = false;

          _TCHAR tszPpt12Dir[MAX_PATH];
          DWORD dwSize = MAX_PATH;
          BOOL bRet = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE, 
                          _T("Software\\Microsoft\\Office\\12.0\\PowerPoint\\InstallRoot"), 
                          _T("Path"), tszPpt12Dir, &dwSize); 
          CString csPptExecutable;
          csPptExecutable.Empty();
          if (bRet) {
              csPptExecutable = tszPpt12Dir;
              if (csPptExecutable[csPptExecutable.GetLength()-1] != _T('\\'))
                  csPptExecutable += _T("\\");
              csPptExecutable += _T("PowerPnt.exe");
              if (_taccess(csPptExecutable, 00) == 0)
                  bPowerPoint2007IsInstalled = true;
          }
          if (bPowerPoint2007IsInstalled) {
              // Reason (2): PowerPoint is installed but not the default.
              CString csMessage;
              csMessage.LoadString(IDS_ERROR_PPT2007_NOT_DEFAULT);
              int iAnswer = MessageBox(NULL, csMessage, NULL, MB_YESNO | MB_TOPMOST);
              if (iAnswer == IDYES) {

                  STARTUPINFO StartupInfo;
                  ZeroMemory(&StartupInfo, sizeof STARTUPINFO);
                  StartupInfo.cb = sizeof STARTUPINFO;
                  StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
                  StartupInfo.wShowWindow = SW_HIDE;

                  PROCESS_INFORMATION ProcessInformation;
                  ZeroMemory(&ProcessInformation, sizeof PROCESS_INFORMATION);

                  int iStringLength = csPptExecutable.GetLength() + 20;
                  TCHAR *tszCommand = new TCHAR[iStringLength];
                  _stprintf(tszCommand, _T("%s /regserver"), csPptExecutable);
                  BOOL bRet = CreateProcess(NULL, tszCommand, 
                                    NULL, NULL, FALSE, 0, NULL, NULL, 
                                    &StartupInfo, &ProcessInformation);

                  if (bRet != FALSE) {
                      ThreadParam *pParam = new ThreadParam;
                      pParam->hProcess = ProcessInformation.hProcess;
                      pParam->pMainWnd = AfxGetMainWnd();
                      CWinThread *thread = AfxBeginThread(WaitForInstallationFinished, (LPVOID)pParam);
                      // do not delete pParam, because it is used in the thread
                  }
                  delete[] tszCommand;
              }

          } else {
              // Reason (2): There is no PowerPoint working with pptx
              CString csMessage;
              csMessage.LoadString(IDS_ERROR_NO_PPT2007);
              MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
          }
          bSuccessResult = false;
      }
      else
      {
         CString csMessage;
         csMessage.Format(_T("Error: %d"), errID);
         MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
         
          bSuccessResult = false;
      }
      
      if (wszText)
         delete wszText;

   }

   return bSuccessResult;
}

bool ASSISTANT::Project::SaveDocument()
{
   Document
      *document;
   CString
      documentPath;
   CString
      docFilename;
   
   document = GetActiveDocument();
   
   if (document)
   {
      documentPath = document->GetDocumentPath();
      if (documentPath.IsEmpty())
      {
         CString docFilename;  
         docFilename = document->GetContainerName()+_T(".lsd");
         ASSISTANT::GetSaveFilename(IDS_LSD_FILTER, docFilename, _T(".lsd"), _T("LSD"));
         
         if (docFilename.IsEmpty())
            return false;
      
         document->SetDocumentFilename((LPCTSTR)docFilename);
      }
      else
      {
         docFilename = document->GetDocumentPath();
         docFilename += _T("\\");
         docFilename += document->GetDocumentName();
         
      }
   
      if (document->GetActivePage() != NULL) {
          if(document->GetActivePage()->HasSelectedObjects()) {
              document->GetActivePage()->UnSelectObjects();
          }
      }

      if (!document->Save())
      {
         CString csMessage;
         csMessage.Format(IDS_ERROR_SAVE_DOCUMENT, docFilename);
         MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      }
      else
      {
          OpenCloseMetadata(document, docFilename);
          // If this document was new it is not saved in registry yet
          SaveToRegistry();
      }

   }
   
   return true;
}

bool ASSISTANT::Project::SaveDocumentAs()
{
   Document *document = GetActiveDocument();
   
   if (document)
   {
      CString docFilename;  
      docFilename = document->GetContainerName()+_T(".lsd");
      ASSISTANT::GetSaveFilename(IDS_LSD_FILTER, docFilename, _T(".lsd"), _T("LSD"));
      
      if (docFilename.IsEmpty())
         return false;
      
      document->SetDocumentFilename((LPCTSTR)docFilename);

      if (document->GetActivePage() != NULL) {
          if(document->GetActivePage()->HasSelectedObjects()) {
              document->GetActivePage()->UnSelectObjects();
          }
      }
      
      if (!document->SaveAs((LPCTSTR)docFilename))
      {
         CString csMessage;
         csMessage.Format(IDS_ERROR_SAVE_DOCUMENT, docFilename);
         MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      }
      else
      {
         OpenCloseMetadata(document, docFilename);
         SetChanged(true);
          // If this document was new it is not saved in registry yet
          SaveToRegistry();
      }
   }
   
   
   return true;
}

void ASSISTANT::Project::GetLastSavedDocument(CString &csLastSavedDocument)
{
    Document *document = GetActiveDocument();
    if (document)
        document->GetLastSavedDocument(csLastSavedDocument);
}

void ASSISTANT::Project::GetSourceDocumentPaths(CArray<CString, CString>& aPaths) {
    for (int i=0; i<documents_.GetSize(); ++i) {
        Document *d = documents_[i];
        // Add only saved source documents
        if (d != NULL && !d->GetDocumentPath().IsEmpty()) {
            CString csPathName = d->GetDocumentPath();
            csPathName += _T("\\");
            csPathName += d->GetDocumentName();

            aPaths.Add(csPathName);
        }
    }
}

void ASSISTANT::Project::GetRecordingDocumentPaths(CArray<CString, CString>& aPaths) {
    for (int i=0; i<presentations_.GetSize(); ++i) {
        PresentationInfo *d = presentations_[i];
        if (d != NULL) {
            CString csPathName = d->GetFilename();
            //csPathName += _T("\\");
           // csPathName += d->GetDocumentName();

            aPaths.Add(csPathName);
        }
    }
}

void ASSISTANT::Project::AddRecording(CString &csFilename)
{
    PresentationInfo *pNewRecording = new PresentationInfo();
    pNewRecording->SetFilename(csFilename);
    pNewRecording->SetCount(presentationCount);
    pNewRecording->Initialize();
    ++presentationCount;
    presentations_.Add(pNewRecording);
}

ASSISTANT::PresentationInfo *ASSISTANT::Project::GetLastRecording() {
    // The last recorded document is at the first 
    // position in the presentations_ list.
    if (presentations_.GetSize() > 0)
        return presentations_[0];

    return NULL;
}

// Functions called from main.cpp concerning actual page

void ASSISTANT::Project::ChangeLineColor(COLORREF clrLine, int iTransparency)
{
   if (GetActivePage())
      GetActivePage()->ChangeLineColor(clrLine, iTransparency);
   
   if (!avgrabber_ || !recording || !isScreenGrabbingActive_) 
      return;
   
   Gdiplus::ARGB argbClrLine = 
       Gdiplus::Color::MakeARGB(iTransparency, 
                                GetRValue(clrLine), GetGValue(clrLine), GetBValue(clrLine));

   if (avgrabber_)
       avgrabber_->setSgAnnotationPenColor(argbClrLine);
}

void ASSISTANT::Project::ChangeFillColor(COLORREF clrFill, int iTransparency)
{
   if (GetActivePage())
      GetActivePage()->ChangeFillColor(clrFill, iTransparency);
   
   if (avgrabber_) 
   {
      if (clrFill != -1)
      {
         if (iTransparency != 255)
            clrFill = Gdiplus::Color::MakeARGB(iTransparency, 
            GetRValue(clrFill), GetGValue(clrFill), GetBValue(clrFill));
         avgrabber_->setSgAnnotationIsFilled(true);
         avgrabber_->setSgAnnotationFillColor(clrFill);
      }
      else
      {
         avgrabber_->setSgAnnotationIsFilled(false);
      }
   }
}

void ASSISTANT::Project::ChangeTextColor(COLORREF clrText, int iTransparency)
{
   if (GetActivePage())
      GetActivePage()->ChangeTextColor(clrText, iTransparency);
   
   if (!avgrabber_ || !recording || !isScreenGrabbingActive_) 
      return;

   Gdiplus::ARGB argbClrText = 
       Gdiplus::Color::MakeARGB(iTransparency, 
                                GetRValue(clrText), GetGValue(clrText), GetBValue(clrText));

   if (avgrabber_) 
      avgrabber_->setSgAnnotationPenColor(argbClrText);
}

void ASSISTANT::Project::ChangeFillColor()
{
   if (GetActivePage())
      GetActivePage()->ChangeFillColor();
   
   if (avgrabber_) 
   {
      avgrabber_->setSgAnnotationIsFilled(false);
   }
}

void ASSISTANT::Project::ChangeLineWidth(double dLineWidth)
{
   if (GetActivePage())
      GetActivePage()->ChangeLineWidth(dLineWidth);
   
   int iLineWidth= (int)dLineWidth;
   if (avgrabber_)
      avgrabber_->setSgAnnotationLineWidth(iLineWidth);
}

void ASSISTANT::Project::ChangeLineStyle(int iLineStyle)
{
   if (GetActivePage())
      GetActivePage()->ChangeLineStyle(iLineStyle);

   if (avgrabber_)
    avgrabber_->setSgAnnotationLineStyle(iLineStyle);
}

void ASSISTANT::Project::ChangeArrowStyle(int iArrowConfig, LPCTSTR szArrowStyle)
{
   if (GetActivePage())
      GetActivePage()->ChangeArrowStyle(iArrowConfig, szArrowStyle);
   
   int arrowStyle = ASSISTANT::GetIntArrowStyle(szArrowStyle);
   
   if (avgrabber_)
   {
      avgrabber_->setSgAnnotationArrowStyle(arrowStyle);
      avgrabber_->setSgAnnotationArrowConfig(iArrowConfig);
   }
}

void ASSISTANT::Project::ChangeFontFamily(LPCTSTR szFontFamily)
{
   if (GetActivePage())
      GetActivePage()->ChangeFontFamily(szFontFamily);

   if (avgrabber_)
   {
      _tcscpy(m_logFont.lfFaceName, szFontFamily);
      avgrabber_->setSgAnnotationFont(&m_logFont);
   }
}

void ASSISTANT::Project::ChangeFontSize(int iFontSize)
{
   if (GetActivePage())
      GetActivePage()->ChangeFontSize(iFontSize);
   
   if (avgrabber_)
   {
      m_logFont.lfHeight = -1 * abs(iFontSize);
      avgrabber_->setSgAnnotationFont(&m_logFont);
   }
}

void ASSISTANT::Project::ChangeFontWeight(bool bBold)
{
   if (GetActivePage())
      GetActivePage()->ChangeFontWeight(bBold);

   if (avgrabber_)
   {
      if (bBold)
         m_logFont.lfWeight = FW_BOLD;
      else
         m_logFont.lfWeight = FW_NORMAL;
      avgrabber_->setSgAnnotationFont(&m_logFont);
   }
}

void ASSISTANT::Project::ChangeFontSlant(bool bItalic)
{
   if (GetActivePage())
      GetActivePage()->ChangeFontSlant(bItalic);

   if (avgrabber_)
   {
      m_logFont.lfItalic = bItalic;
      avgrabber_->setSgAnnotationFont(&m_logFont);
   }
}

void ASSISTANT::Project::CutObjects()
{
   for (int i = 0; i < documents_.GetSize(); ++i)
      documents_[i]->ResetFirstInserted();
   
   if (GetActiveDocument())
      GetActiveDocument()->CutObjects();
}

void ASSISTANT::Project::DeleteObjects(bool bDoUndo)
{
   if (GetActivePage())
      GetActivePage()->DeleteObjects(bDoUndo);
}

void ASSISTANT::Project::CopyObjects()
{
   for (int i = 0; i < documents_.GetSize(); ++i)
      documents_[i]->ResetFirstInserted();
   
   if (GetActiveDocument())
      GetActiveDocument()->CopyObjects();
}

void ASSISTANT::Project::PasteObjects()
{
   if (GetActiveDocument())
      GetActiveDocument()->PasteObjects();
}

void ASSISTANT::Project::SelectAllObjects()
{
   if (GetActivePage())
      GetActivePage()->SelectAllObjects();
}

void ASSISTANT::Project::Undo()
{
   if (recording)
   {
      int lastSgUndo = -1;
      int lastWbUndo = -1;
      if (avgrabber_)
         lastSgUndo = avgrabber_->getSgAnnotationLastUndoTimeStamp();

      if (GetActivePage())
         lastWbUndo = GetActivePage()->GetLastUndoTime();
      
      if ((lastSgUndo != -1) && (lastSgUndo > lastWbUndo))
      {
         avgrabber_->sgAnnotationUndo();
      }
      else
      {
         if (GetActivePage())
            GetActivePage()->Undo();
      }
   }
   else
   {
      if (GetActivePage())
         GetActivePage()->Undo();
   }
}

void ASSISTANT::Project::SelectObjects(CRect &rcSelection)
{
   if (GetActivePage())
      GetActivePage()->SelectObjects(rcSelection);
}

bool ASSISTANT::Project::HasSelectedObjects()
{
   if (GetActivePage())
      return GetActivePage()->HasSelectedObjects();

   return false;
}

bool ASSISTANT::Project::HasLoadedDocuments()
{
    if (documents_.IsEmpty())
        return false;

    for (int i = 0; i < documents_.GetSize(); ++i)
        if (documents_[i]->IsLoaded())
            return true;

    return false;
}

bool ASSISTANT::Project::IsSgOnlyDocument()
{
    return !HasLoadedDocuments() || IsPureScreenGrabbing();
}

bool ASSISTANT::Project::PageIsChanged()
{
   if (GetActivePage())
      return GetActivePage()->HasChanged();

   return false;
}

void ASSISTANT::Project::SelectSingleObject(CRect &rcSelection)
{
   if (GetActivePage())
      GetActivePage()->SelectSingleObject(rcSelection);
}

void ASSISTANT::Project::UnSelectObjects()
{
   if (GetActivePage())
      GetActivePage()->UnSelectObjects();
}

void ASSISTANT::Project::RaiseObjects()
{
   if (GetActiveDocument())
      GetActiveDocument()->RaiseObjects();
}

void ASSISTANT::Project::LowerObjects()
{
   if (GetActiveDocument())
      GetActiveDocument()->LowerObjects();
}

void ASSISTANT::Project::DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC)
{
   if (GetActivePage())
      GetActivePage()->DrawWhiteboard(rcWhiteboard, pDC);
}

void ASSISTANT::Project::SetFullScreenOn(bool bState)
{
	if(GetActivePage())
		GetActivePage()->SetFullScreenOn(bState);
}

void ASSISTANT::Project::ReBuildStructureTree(CDocumentStructureView *pDocStructTree)
{
    CDocumentStructureView *pCurrentStructureTree = pDocStructTree;

    if (pCurrentStructureTree == NULL) {
        CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();
        pCurrentStructureTree = pMainWnd->GetStructureTree();
    }

   if (pCurrentStructureTree == NULL)
      return;

   pCurrentStructureTree->RemoveAll();
   
   for (int i = 0; i < documents_.GetSize(); ++i)
      documents_[i]->AddToStructureTree(pCurrentStructureTree, 0);
   
   if (GetActivePage())
      pCurrentStructureTree->SetSelected(GetActivePage()->GetID());
   else
	   if(GetActiveFolder())
			pCurrentStructureTree->SetSelected(GetActiveFolder()->GetID());
	   else
		   if(GetActiveDocument())
				pCurrentStructureTree->SetSelected(GetActiveDocument()->GetID());
   
}

void ASSISTANT::Project::ReBuildRecordingTree(CRecordingsView *pRecordingTree)
{
   if (pRecordingTree == NULL)
      return;

   pRecordingTree->RemoveAll();

   SortRecordingsList(1 /*by date*/, TRUE /*Ascending*/);
   
   for (int i = 0 ; i < presentations_.GetSize(); i++)
      pRecordingTree->InsertPresentation(presentations_[i],i);

   // Selecting recordings leads to side effect 
   // (selection in document structure).
   //pRecordingTree->SetSelected(0);
}

void ASSISTANT::Project::ActivatePage(bool rebuild)
{
   if (!GetActiveDocument())
   {
      if (lastActivePage)
         lastActivePage->DeActivate();
      lastActivePage = NULL;
      return;
   }
   
   if (lastActivePage && (GetActivePage() != lastActivePage))
   {
      lastActivePage->DeActivate();
      lastActivePage = NULL;
   }
   
   if (rebuild || !lastActivePage || (GetActivePage() != lastActivePage))
   {
      GetActiveDocument()->Activate();
      lastActivePage = GetActivePage();
   }

   if (recording && !paused)
   {
      int time = GetRecordingTime();
      SaveImages();
      UpdateProjectFiles(time);
   }
}

void ASSISTANT::Project::DeActivatePage()
{
   if (lastActivePage)
      lastActivePage->DeActivate();
   lastActivePage = NULL;
   
   if (GetActiveDocument())
      GetActiveDocument()->DeActivate();
   
   
}

bool ASSISTANT::Project::GetPageProperties(UINT &nPageWidth, UINT &nPageHeight, COLORREF &clrBackground)
{
   Page *pPage = GetActivePage();

   if (pPage == NULL)
      return false;

   nPageWidth = pPage->GetWidth();
   nPageHeight = pPage->GetHeight();
   clrBackground = pPage->GetColor();

   return true;
}

bool ASSISTANT::Project::GetPagePropertiesEx(CRect rcW, UINT &nPageWidth, UINT &nPageHeight, UINT &nOffX, UINT &nOffY)
{
	Page *pPage = GetActivePage();

   if (pPage == NULL)
      return false;


   float fZoomFactor = pPage->CalculateZoomFactor(rcW);
   pPage->CalculateOffset(rcW, fZoomFactor, nOffX, nOffY);

   nPageWidth = rcW.Width() - (2 * nOffX);
   nPageHeight = rcW.Height() - (2 * nOffY);

   return true;
}

bool ASSISTANT::Project::SetPageProperties(UINT nPageWidth, UINT nPageHeight, COLORREF clrBackground)
{
   Page *pPage = GetActivePage();

   if (pPage == NULL)
      return false;

   pPage->SetPageDimension(nPageWidth, nPageHeight);
   pPage->SetColor(clrBackground);

   return true;
}
//

void ASSISTANT::Project::OpenFromRegistry() {
    CDocumentManager docManage(AfxGetApp());
    // it is necessary to read first all recordings
    // because in AddDocument SaveToRegistry will be called
    docManage.LoadRecordingDocumentsIn(this);  
    docManage.LoadSourceDocumentsIn(this);  
}

void ASSISTANT::Project::SaveToRegistry() {
    // Do nothing related to media library (LC has its own)
    if (CStudioApp::IsLiveContextMode())
        return;

    CDocumentManager docManage(AfxGetApp());
    docManage.StoreSourceDocumentsFrom(this);
    docManage.StoreRecordingDocumentsFrom(this);
}

void ASSISTANT::Project::WriteLapFile(LPCTSTR filename, bool bCopyProject)
{
   HANDLE hLapFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hLapFile == INVALID_HANDLE_VALUE)
   {
      CString csMessage;
      csMessage.Format(IDS_ERROR_FILE_OPEN, filename);
      MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      return;
   }

   CFileOutput lapOutput;
   lapOutput.Init(hLapFile, 0, true);
   
   CString csLine;
   CString csSgmlLine;

   csLine = _T("<lap>\n");
   lapOutput.Append(csLine);
   
   
   // write metafile infotmation
   csLine = _T("   <metainfo>\n");
   lapOutput.Append(csLine);

   metadata_->Write(&lapOutput);

   csLine = _T("   </metainfo>\n");
   lapOutput.Append(csLine);
   
   
   // write file entries
   csLine = _T("   <files>\n");
   lapOutput.Append(csLine);

   for (int i = 0; i < documents_.GetSize(); ++i)
   {
      if (bCopyProject)
      {
         CString ssNewProjectPrefix;
         ssNewProjectPrefix = filename;
         ASSISTANT::GetName(ssNewProjectPrefix);
         ASSISTANT::GetPrefix(ssNewProjectPrefix);
         
         csLine.Format(_T("%s\\lsd_%d\\"), ssNewProjectPrefix, i);
         csLine += documents_[i]->GetDocumentName();
      }
      else
      {
         CString ssLsdPath;
         ssLsdPath = documents_[i]->GetDocumentPath();
         ssLsdPath += _T("\\");
         ssLsdPath += documents_[i]->GetDocumentName();

         CString ssProjectPath = projectPath_ + _T("\\");

         _TCHAR szRelative[MAX_PATH];
         LIo::MakeRelativePath(szRelative, ssProjectPath, ssLsdPath);

         csLine = szRelative;
      }
      StringManipulation::TransformForSgml(csLine, csSgmlLine);
      
      csLine.Format(_T("      <source>%s</source>\n"), csSgmlLine);
      lapOutput.Append(csLine);
   }

   csLine = _T("      <apps></apps>\n");
   lapOutput.Append(csLine);

   csLine = _T("      <clips></clips>\n");
   lapOutput.Append(csLine);

   if (!bCopyProject)
   {
      csLine = recordPath_;
      StringManipulation::TransformForSgml(csLine, csSgmlLine);

      csLine.Format(_T("      <recorddir>%s</recorddir>\n"), csSgmlLine);
      lapOutput.Append(csLine);
   }

   csLine = _T("      <presentations>\n");
   lapOutput.Append(csLine);

   for (int j = 0; j < presentations_.GetSize(); ++j)
   {  
      if (bCopyProject)
      {
         CString ssNewProjectPrefix = filename;
         ASSISTANT::GetName(ssNewProjectPrefix);
         ASSISTANT::GetPrefix(ssNewProjectPrefix);
         
         CString csLrdName = presentations_[j]->GetFilename();
         ASSISTANT::GetName(csLrdName);

         csLine.Format(_T("%s\\lrd_%d\\%s"), ssNewProjectPrefix, j, csLrdName);
      }
      else
      {
         CString ssLrdPath = presentations_[j]->GetFilename();
         CString ssProjectPath = projectPath_ + "\\";
         _TCHAR szRelative[MAX_PATH];
         LIo::MakeRelativePath(szRelative, ssProjectPath, ssLrdPath);
         csLine = szRelative;
      }
      
      StringManipulation::TransformForSgml(csLine, csSgmlLine);
      csLine.Format(_T("        <file>%s</file>\n"), csSgmlLine);
      lapOutput.Append(csLine);
   }

   csLine = _T("      </presentations>\n");
   lapOutput.Append(csLine);

   csLine = _T("   </files>\n");
   lapOutput.Append(csLine);

   csLine = _T("</lap>\n");
   lapOutput.Append(csLine);
   
   
   lapOutput.Flush();
   CloseHandle(hLapFile);
}

bool ASSISTANT::Project::SaveChangedDocuments()
{
   CString
      documentPath,
      message,
      title;
   CString
      docFilename;
   bool 
      docsChanged;
   
   docsChanged = false;
   for (int i = 0; i < documents_.GetSize(); ++i)
   {
      documentPath = documents_[i]->GetDocumentPath();
      if (documentPath.IsEmpty() || documents_[i]->HasChanged())
      {
         docsChanged = true;
         break;
      }
   }
   
   if (docsChanged)
   {
      for (i = 0; i < documents_.GetSize(); ++i)
      {
         documentPath = documents_[i]->GetDocumentPath();
         if (documentPath.IsEmpty() || documents_[i]->HasChanged())
         {
            CString csCaption;
            csCaption.LoadString(IDS_CONFIRM_SAVE);
            CString csMessage;
            csMessage.Format(IDS_SAVE_DOCUMENT, documents_[i]->GetContainerName());
            int iAnswer = MessageBox(NULL, csMessage, csCaption, MB_YESNOCANCEL | MB_TOPMOST);

            if (iAnswer == IDCANCEL) 
               return false;
            else if (iAnswer == IDYES)
            {
               if (documentPath.IsEmpty())
               {
                  docFilename = documents_[i]->GetContainerName()+_T(".lsd");
                  ASSISTANT::GetSaveFilename(IDS_LSD_FILTER, docFilename, _T(".lsd"), _T("LSD"));
                 
                  if (docFilename.IsEmpty())
                     return false;
                  
                  documents_[i]->SetDocumentFilename((LPCTSTR)docFilename);
               }
               else
               {
                  docFilename = documents_[i]->GetDocumentPath();
                  docFilename += _T("\\");
                  docFilename += documents_[i]->GetDocumentName();
                  if (documents_[i]->GetActivePage()->HasSelectedObjects()) {
                      documents_[i]->GetActivePage()->UnSelectObjects();
                  }
               }
               
               if (!documents_[i]->Save())
               {
                  CString csMessage;
                  csMessage.Format(IDS_ERROR_SAVE_DOCUMENT, docFilename);
                  MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
               }
               else
               {
                   OpenCloseMetadata(documents_[i], docFilename);
                   // If this document was new it is not saved in registry yet
                   SaveToRegistry();
               }
            }
         }
      }
   }
   
   return true;
}

bool ASSISTANT::Project::Save(LPCTSTR szProjectName, bool bSaveDocuments)
{
   bool
      ret = true;
   
   if (bSaveDocuments)
      ret = SaveChangedDocuments();

   if (ret)
   {
      CString csProjectName;
      if (szProjectName == NULL)
      {
         if (projectPath_.IsEmpty())
         {
            ASSISTANT::GetSourceDirectory(csProjectName);
            csProjectName += _T("\\");
            csProjectName += projectName_;
            
            ASSISTANT::GetSaveFilename(IDS_LAP_FILTER, csProjectName, _T(".lap"), _T("LAP"));
            
            if (csProjectName.IsEmpty())
               ret = false;
         }
         else
         {
            csProjectName = projectPath_ + _T("\\");
            
            ASSISTANT::GetSourceDirectory(csProjectName);
            csProjectName += _T("\\");
            csProjectName += projectName_;
         }
      }
      else
      {
         csProjectName = szProjectName;
      }

      if (ret)
      {
         bool changeMetadata = false;
         if (projectPrefix_ == metadata_->GetTitle())
            changeMetadata = true;
         
         ChangeProjectName(csProjectName, changeMetadata);
      }

      if (ret)
      {
         WriteLapFile(csProjectName);
         SetChanged(false);
      }
   }
   
   return ret;
}


// Changing project options

bool ASSISTANT::Project::ActivateAudioDevice()
{
   if (audioDevice_ >= audioDeviceCount_)
      audioDevice_ = 0;

   if (audioDeviceCount_ == 0)
      return false;

   if (audioFramerate_ == 0)
      audioFramerate_ = 22050;
   
   bool bFound = false;
   for (int i = 0; i < audioDevices_[audioDevice_]->nSampleRates && !bFound; ++i)
   {
      if (audioFramerate_ == audioDevices_[audioDevice_]->sampleRates[i])
         bFound = true;
   }

   if (!bFound)
   {
      if (audioDevices_[audioDevice_]->nSampleRates != 0)
         audioFramerate_ = audioDevices_[audioDevice_]->sampleRates[0];
      else
         audioFramerate_ = 0;
   }
   
   
   bFound = false;
   if (audioBitrate_ == 0)
      audioBitrate_ = 16;
   
   for (i = 0; i < audioDevices_[audioDevice_]->nBitRates && !bFound; ++i)
   {
      if (audioBitrate_ == audioDevices_[audioDevice_]->bitRates[i])
         bFound = true;
   }
   
   if (!bFound)
   {
      if (audioDevices_[audioDevice_]->nBitRates != 0)
         audioBitrate_ = audioDevices_[audioDevice_]->bitRates[0];
      else
         audioBitrate_ = 0;
   }
   
   if ((audioFramerate_ == 0) || (audioBitrate_ == 0))
   {
      CString csMessage;
      csMessage.LoadString(IDS_ERROR_AUDIODEVICE);
      MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      audioMixer_ = NULL;
      audioVolume_ = 0;
      return false;
   }

   try
   {
      audioMixer_ = avgrabber_->setAudioDevice(audioDevice_, audioFramerate_, audioBitrate_, MixerControl);
      isAudioDeviceSet_ = true;

#ifndef _UNICODE
      ASSERT(false); // the below does not exist for Ansi?
#endif

      CString csLine = ::GetCommandLine();
      int iArgCount = 0;
      LPWSTR *paArgs = ::CommandLineToArgvW(csLine, &iArgCount);
      if (iArgCount > 0) {
          csLine = paArgs[0];

          CString csClickPath = csLine;
          StringManipulation::GetPath(csClickPath);
          csClickPath += _T("\\click.wav");

          bool bLoadClickNoise = true;
#ifdef _DEBUG
          if (_taccess(csClickPath, 04) != 0)
              bLoadClickNoise = false;
              // click.wav cannot be found: ignore this only in debug mode
#endif

          if (bLoadClickNoise)
              avgrabber_->loadSgClickNoise((_TCHAR *)(LPCTSTR)csClickPath);

          ::LocalFree(paArgs);
      }
      paArgs = NULL;
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), "InitAudioVideo", MB_OK | MB_TOPMOST);
      audioMixer_  = NULL;
      audioVolume_ = 0;
      isAudioDeviceSet_ = false;
      return false;
   }
   
   DeleteMixerSources();
   try 
   {
      if (audioMixer_) 
      {
         if (audioMixer_->isEnabled())
         {
            audioVolume_ = audioMixer_->getVolume();
            mixerSourceCount_ = audioMixer_->getSourceCount();
            CreateMixerSources();
            mixerSource_ = audioMixer_->getSource();
         }
         else
         {
            audioMixer_ = NULL;
            audioVolume_ = 0;
         }
      }
      else 
      {
         audioVolume_ = 0;
      }
   }
   catch (exception &e)
   {
      DeleteMixerSources();
      audioMixer_  = NULL;
      audioVolume_ = 0;
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
   }

   return true;
}

bool ASSISTANT::Project::SetAudioDevice(UINT nAudioDevice)
{
   isAudioDeviceSet_ = false;
   
   audioDevice_ = nAudioDevice;

   if (audioDevice_ >= audioDeviceCount_)
      audioDevice_ = 0;

   return true;
}

bool ASSISTANT::Project::SetSampleWidth(UINT nSampleWidth)
{
   if (audioDeviceCount_ == 0)
      return true;

   if (audioDevice_ >= audioDeviceCount_)
      audioDevice_ = 0;

   if (audioDevices_[audioDevice_]->nBitRates == 0)
      return true;

   if (nSampleWidth >= audioDevices_[audioDevice_]->nBitRates)
      nSampleWidth = 0;

   audioBitrate_ = audioDevices_[audioDevice_]->bitRates[nSampleWidth];

   return true;
}

bool ASSISTANT::Project::SetSampleRate(UINT nSampleRate)
{
   if (audioDeviceCount_ == 0)
      return true;

   if (audioDevice_ >= audioDeviceCount_)
      audioDevice_ = 0;

   if (audioDevices_[audioDevice_]->nSampleRates == 0)
      return true;

   if (nSampleRate >= audioDevices_[audioDevice_]->nSampleRates)
      nSampleRate = 0;

   audioFramerate_ = audioDevices_[audioDevice_]->sampleRates[nSampleRate];
   
   return true;   
}

bool ASSISTANT::Project::SetAudioSource(UINT nAudioSource)
{
   if (mixerSourceCount_ == 0)
      return true;

   if (nAudioSource >= mixerSourceCount_)
      mixerSource_ = 0;
   
   mixerSource_ = nAudioSource;
   
   audioMixer_->setSource(mixerSource_);
   audioVolume_ = audioMixer_->getVolume();
   
   return true;
}

UINT ASSISTANT::Project::GetAudioSource()
{
   UINT nSource = 0;

   if (mixerSourceCount_ != 0)
   {
      AudioMixer *audioMixer = ASSISTANT::Project::active->GetAudioMixer();
      if (audioMixer && audioMixer->isEnabled())
      {
         nSource = audioMixer->getSource();
      }
   }

   return nSource;
}

bool ASSISTANT::Project::SetVideoDevice()
{
   if (videoDeviceCount_ > 0)
   {
      avgrabber_->deleteWdmDevice();
      isWdmDeviceSet_ = false;
      
      if (videoDevice_ >= videoDeviceCount_)
      {
         videoDevice_ = 0;
         CString csMessage;
         csMessage.Format(IDS_ERROR_SET_VIDEODEVICE, videoDeviceNames_[videoDevice_]);
         
         MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      }
      
      if (videoFramerate_ == 0)
      {
         videoFramerate_ = 25.0;
      }
      
      hasFormatDialog_ = false;
      hasSourceDialog_ = false;
      hasDisplayDialog_ = false;
      
      try 
      {
         if (isVideoEnabled_)
         {
            avgrabber_->setVideoDevice(videoDevice_);
            avgrabber_->setUseAdvancedAudioSync(isAdvancedSyncSet_);
            avgrabber_->setUseFixedAudioVideoOffset(hasAVOffset_, avOffset_);
            
            VideoDevice videoDevice;
            avgrabber_->getVideoDeviceParameters(videoDevice);
            hasFormatDialog_  = videoDevice.bHasFormatDlg;
            hasSourceDialog_  = videoDevice.bHasSourceDlg;
            hasDisplayDialog_ = videoDevice.bHasDisplayDlg;
            
            RetrieveCodecs(true);
            if (videoFramerate_ > 0)
               avgrabber_->setVideoFrameRate(videoFramerate_);
            avgrabber_->setVideoMonitor(isMonitorEnabled_);
         }
         
      }
      catch (exception &e)
      {
         hasFormatDialog_      = false;
         hasSourceDialog_      = false;
         hasDisplayDialog_     = false;
         videoFramerate_       = 0;
         isVideoEnabled_       = false;
         isCodecEnabled_       = false;
         MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
      }
      
   }
   else 
   {
      if (isVideoEnabled_)
      {
         isVideoEnabled_ = false;
         CString csMessage;
         csMessage.LoadString(IDS_NO_VIDEO_DEVICE);

         MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      }
      isCodecEnabled_ = false;
   }
   
   avgrabber_->setEnableVideo(isVideoEnabled_);
   return true;
}

void ASSISTANT::Project::SetCodecNumber(bool bShowWarning)
{
   if (codecCount_ > 0)
      codecNumber_ = 0;
   else
      codecNumber_ = -1;
   
   for (int i = 0; i < codecCount_; ++i)
   {
      if (videoCodec_ == codecInfo_[i]->fccString)
      {
         codecNumber_ = i;
         break;
      }
   }
   
   if (codecNumber_ == 0)
   {
      VideoSettings videoSettings;
      avgrabber_->getVideoSettings(videoSettings);
      double dFramesPerSec = 1000000.0 / ((double) avgrabber_->getVideoUSecsPerFrame());
      int nBytesPerSec = (int) (dFramesPerSec * ((double) videoSettings.height * videoSettings.width * videoSettings.colorDepth / 8));
      
      int nSecsMax = MAX_AVI_FILE_SIZE / nBytesPerSec; // Max 1900 MB.
      
      int nMins = nSecsMax / 60;
      nSecsMax = nSecsMax % 60;
      _TCHAR tszMins[256];
      _stprintf(tszMins, _T("%d:%02d"), nMins, nSecsMax);

      if (bShowWarning) {
          CString csMessage;
          csMessage.Format(IDS_UNCOMPRESSED_CODEC, tszMins);
          CString csCaption;
          csCaption.LoadString(IDS_WARNING);
          MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
      }
   }
}

void ASSISTANT::Project::CreateCodecInfo()
{
   if (codecCount_ > 0)
   {
      codecInfo_ = new CodecInfo*[codecCount_];
      for (int i = 0; i < codecCount_; ++i)
         codecInfo_[i] = new CodecInfo();
      avgrabber_->getCodecs(codecInfo_);
   }
}

void ASSISTANT::Project::DeleteCodecInfo()
{
   if (codecInfo_)
   {
      for (int i = 0; i < codecCount_; ++i)
      {
         delete codecInfo_[i];
         codecInfo_[i] = NULL;
      }
      delete[] codecInfo_;
      codecInfo_ = NULL;
   }
   
   codecInfo_  = NULL;
   codecCount_ = 0;
   codecNumber_ = -1;
}

void ASSISTANT::Project::ShowCodecConfigure()
{
   if (!avgrabber_)
      return;

   HWND hwnd = AfxGetMainWnd()->m_hWnd;
   try
   {
      avgrabber_->displayCodecConfigureDialog(hwnd);
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), "ShowCodecConfigure", MB_OK | MB_TOPMOST);
   }
}

void ASSISTANT::Project::ShowCodecAbout()
{
   HWND
      hwnd;

   if (!avgrabber_)
      return;

   hwnd = AfxGetMainWnd()->m_hWnd;
   try
   {
      avgrabber_->displayCodecAboutDialog(hwnd);
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), "ShowCodecAbout", MB_OK | MB_TOPMOST);
   }
}


void ASSISTANT::Project::CreateMixerSources()
{
   if (!audioMixer_)
      return;
   
   if (mixerSourceCount_ > 0)
   {
      mixerSources_ = new AudioSource*[mixerSourceCount_];
      for (int i = 0; i < mixerSourceCount_; ++i)
         mixerSources_[i] = new AudioSource();
      audioMixer_->getSources(mixerSources_);
   }
}

void ASSISTANT::Project::DeleteMixerSources()
{
   if (mixerSources_)
   {
      for (int i = 0; i < mixerSourceCount_; ++i)
         delete mixerSources_[i];
      delete[] mixerSources_;
   }
   
   mixerSources_     = NULL;
   mixerSourceCount_ = 0;
   mixerSource_      = 0;
}

void ASSISTANT::Project::SetProjectName(LPCTSTR prefix)
{
   projectName_ = prefix;
   projectName_ += _T(".lap");
   
   projectPrefix_ = prefix;
   
   /*/
   GUI::Toplevel::SetTitle(projectPrefix_);
   /*/
   if (_tcslen(metadata_->GetTitle()) == 0)
      metadata_->SetTitle(projectPrefix_);
}

void ASSISTANT::Project::ChangeProjectName(LPCTSTR newProjectName, bool changeMetadata) {
    projectName_ = newProjectName;
    ASSISTANT::GetName(projectName_);

    projectPrefix_ = projectName_;
    ASSISTANT::GetPrefix(projectPrefix_);

    projectPath_ = newProjectName;
    ASSISTANT::GetPath(projectPath_);

    if (changeMetadata)
        metadata_->SetTitle(projectPrefix_);
}

// video functions
HRESULT ASSISTANT::Project::GetVideoSettings(bool &bRecordVideo, bool &bShowMonitor, bool &bUseAlternativeSync,
                                             bool &bSetAvOffset, int &nAvOffset)
{
   bRecordVideo = isVideoEnabled_;
   bShowMonitor = isMonitorEnabled_;
   bUseAlternativeSync = isAdvancedSyncSet_;
   bSetAvOffset = hasAVOffset_;
   nAvOffset = avOffset_;

   return S_OK;
}

HRESULT ASSISTANT::Project::GetVideoParameter(bool &bUseVFW, CStringArray &arVFWSources, UINT &nVFWIndex,
                                              CStringArray &arWDMSources, UINT &nWDMIndex,
                                              UINT &nXResolution, UINT &nYResolution, UINT &nBitPerColor, 
                                              CArray<float, float> &arFramerates, UINT &nFrameRateIndex)
{ 
   if (avgrabber_)
   {
      bUseVFW = useVfW_;
      
      nVFWIndex = 0;
      arVFWSources.RemoveAll();
      nVFWIndex = videoDevice_;
      for (int i = 0; i < videoDeviceCount_; ++i)
      {
         arVFWSources.Add(videoDeviceNames_[i]);
      }

      nWDMIndex = 0;
      arWDMSources.RemoveAll();
      for (i = 0; i < wdmDeviceCount_; ++i)
      {
         if (wdmDeviceNames_[i] == wdmDevice_)
            nWDMIndex = i;
         arWDMSources.Add(wdmDeviceNames_[i]);
      }
      
      nXResolution = 0;
      nYResolution = 0;
      nBitPerColor = 0;
      arFramerates.RemoveAll();
      nFrameRateIndex = 0;
      if (isVideoEnabled_)
      {
         VideoSettings videoSettings;
         avgrabber_->getVideoSettings(videoSettings);
         nXResolution = videoSettings.width;
         nYResolution = videoSettings.height;
         nBitPerColor = videoSettings.colorDepth;
         if (1) //!useVfW_)
         {
            float wdmFramerate = avgrabber_->getVideoUSecsPerFrame();
            wdmFramerate = (float)(1000000.0 / wdmFramerate);
            videoFramerate_ = wdmFramerate;
         }
         arFramerates.Add(5);
         arFramerates.Add(7.5);
         arFramerates.Add(10);
         arFramerates.Add(12.5);
         arFramerates.Add(15);
         arFramerates.Add(25);
         arFramerates.Add((float)29.97);
         arFramerates.Add(30);
         
         bool bFramerateFound = false;
         for (i = 0; i < arFramerates.GetSize() && !bFramerateFound; ++i)
         {
            if (videoFramerate_ == arFramerates[i])
            {
               nFrameRateIndex = i;
               bFramerateFound = true;
            }
         }
         if (!bFramerateFound)
         {
            nFrameRateIndex = arFramerates.GetSize();
            arFramerates.Add(videoFramerate_);
         }
         
      }
   }

   return S_OK;
}

HRESULT ASSISTANT::Project::GetVideoResolution(UINT &nXResolution, UINT &nYResolution)
{
   nXResolution = 0;
   nYResolution = 0;
   if (isVideoEnabled_)
   {
      VideoSettings videoSettings;
      avgrabber_->getVideoSettings(videoSettings);
      nXResolution = videoSettings.width;
      nYResolution = videoSettings.height;
      
   }

   return S_OK;
}

HRESULT ASSISTANT::Project::GetColorDepth(UINT &nBitPerColor)
{
   nBitPerColor = 0;
   if (isVideoEnabled_)
   {
      VideoSettings videoSettings;
      avgrabber_->getVideoSettings(videoSettings);
      nBitPerColor = videoSettings.colorDepth;
   }

   return S_OK;
}

HRESULT ASSISTANT::Project::EnableVideoRecording(bool bEnable, bool bCheckCodecs)
{
   if (!avgrabber_)
      return E_FAIL;
   
   isVideoEnabled_ = bEnable;
   avgrabber_->setEnableVideo(isVideoEnabled_);
   
   if (isVideoEnabled_)
   {
      if (useVfW_)
         SetVideoDevice();
      else
         SetWdmDevice(true);

      // Check for useable video codecs
      if (bCheckCodecs)
         CheckInstalledVideoCodecs();

   }

   return S_OK;
}

HRESULT ASSISTANT::Project::ShowVideoMonitor(bool bShow)
{
   if (!avgrabber_)
      return E_FAIL;
   
   isMonitorEnabled_ = bShow;
   try 
   {
      if (isVideoEnabled_)
         avgrabber_->setVideoMonitor(isMonitorEnabled_);
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), "ShowVideoMonitor", MB_OK | MB_TOPMOST);
   }

   return S_OK;
}

HRESULT ASSISTANT::Project::CheckInstalledVideoCodecs()
{
   // if video recording is enabled, the codecs should be tested 
   // if they are usable for video recording or not.
   if (isVideoEnabled_)
   {
      int iNotUsableCodecCount = 0;
      for (int i = 0; i < codecCount_; ++i)
      {
         // the following codecs are not really usable for video recording
         if (_tcsnicmp(codecInfo_[i]->fccString, _T("cvid"), 4) == 0 ||
            _tcsnicmp(codecInfo_[i]->fccString, _T("msvc"), 4) == 0 ||
            _tcsnicmp(codecInfo_[i]->fccString, _T("cram"), 4) == 0 ||
            _tcsnicmp(codecInfo_[i]->fccString, _T("dib"), 3) == 0 ||
            _tcsnicmp(codecInfo_[i]->fccString, _T("iyuv"), 4) == 0 ||
            _tcsnicmp(codecInfo_[i]->fccString, _T("i420"), 4) == 0 ||
            _tcsnicmp(codecInfo_[i]->fccString, _T("lsgc"), 4) == 0)
         {
            ++iNotUsableCodecCount;
         }
      }

      if (iNotUsableCodecCount == codecCount_)
      {
         CString csMessage;
         csMessage.LoadString(IDS_WARNING_NOUSABLECODEC);
         CString csCaption;
         csCaption.LoadString(IDS_WARNING);
         MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
      }
   }

   return S_OK;
}

HRESULT ASSISTANT::Project::SetAlternativeAvSync(bool bUseAlternativeSync)
{
   isAdvancedSyncSet_ = bUseAlternativeSync; 
   
   return S_OK;
}

void ASSISTANT::Project::SetVideoFramerate(float fVideoFramerate)
{
   float newVideoFramerate = fVideoFramerate;
   if (videoFramerate_ != newVideoFramerate)
   {
      videoFramerate_ = newVideoFramerate;
      
      try
      {
         if (videoFramerate_ > 0)
            avgrabber_->setVideoFrameRate(videoFramerate_);
      }
      catch (exception &e)
      {
         MessageBoxA(NULL, e.what(), "SetVideoFramerate", MB_OK | MB_TOPMOST);
      }
   }
}

HRESULT ASSISTANT::Project::SetUseAvOffset(bool bUseAvOffset)
{
   hasAVOffset_ = bUseAvOffset;

   if (avgrabber_)
      avgrabber_->setUseFixedAudioVideoOffset(hasAVOffset_, avOffset_);
   
   return S_OK;
}

HRESULT ASSISTANT::Project::SetAvOffset(int iAvOffset)
{
   avOffset_ = iAvOffset;
   
   if (avgrabber_)
      avgrabber_->setUseFixedAudioVideoOffset(hasAVOffset_, avOffset_);
   
   return S_OK;
}

HRESULT ASSISTANT::Project::SetUseVFW(bool bUseVFW)
{
   useVfW_ = bUseVFW;
   
   if (useVfW_)
      SetVideoDevice();
   else
      SetWdmDevice(true);
   
   return S_OK;
}

HRESULT ASSISTANT::Project::ChangeWdmDevice(CString &csWdmDevice)
{
   wdmDevice_ = csWdmDevice;

   if (SetWdmDevice(true))
      return S_OK;
   else
      return E_FAIL;
}

HRESULT ASSISTANT::Project::ShowVideoFormat()
{
   if (!avgrabber_)
      return E_FAIL;
   
   try
   {
      avgrabber_->displayVideoFormatDialog();
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), "ShowVideoFormat", MB_OK | MB_TOPMOST);
      return E_FAIL;
   }
   
   RetrieveCodecs(true);

   return S_OK;
}

HRESULT ASSISTANT::Project::ShowVideoSource()
{
   if (!avgrabber_)
      return E_FAIL;
   
   try
   {
      avgrabber_->displayVideoSourceDialog();
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), "ShowVideoSource", MB_OK | MB_TOPMOST);
      return E_FAIL;
   }
   
   RetrieveCodecs(true);

   return S_OK;
}

HRESULT ASSISTANT::Project::ShowVideoDisplay()
{
   if (!avgrabber_)
      return E_FAIL;
   
   try
   {
      avgrabber_->displayVideoDisplayDialog();
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), "ShowVideoDisplay", MB_OK | MB_TOPMOST);
      return E_FAIL;
   }
   
   RetrieveCodecs(true);

   return S_OK;
}

HRESULT ASSISTANT::Project::GetCodecSettings(CStringArray &arCodecs, UINT &nCodecIndex, UINT &nQuality, 
                                             bool &bUseKeyframes, UINT &nKeyframeDistance,
                                             bool &bUseDatarateAdaption, UINT &nDatarate,
                                             bool &bHasAbout, bool &bHasConfigure)
{
   arCodecs.RemoveAll();
   nCodecIndex = 0;
   for (int i = 0; i < codecCount_; ++i)
   {
      if (videoCodec_ == codecInfo_[i]->fccString)
         nCodecIndex = i;
      CString csCodec = codecInfo_[i]->fccString;
      csCodec += _T(" : "); 
      csCodec += codecInfo_[i]->tszDescription;
      arCodecs.Add(csCodec);
   }

   nQuality = videoQuality_;

   bUseKeyframes = useKeyframes_;
   nKeyframeDistance = keyframeDist_;

   bUseDatarateAdaption = useDatarate_;
   nDatarate = videoDatarate_;

   bHasAbout = hasAboutDialog_;
   bHasConfigure = hasConfigureDialog_;

   return S_OK;
}

HRESULT ASSISTANT::Project::SetCodec(CString &csCodec, bool bShowWarning)
{
   if (!avgrabber_ || codecCount_ <= 0)
      return E_FAIL;
   
   CString csFccString = csCodec;
   int iPos = csFccString.Find(_T(":"));
   if (iPos > 0)
      csFccString = csFccString.Left(iPos-1);

   videoCodec_ = csFccString;
   SetVideoCodec(bShowWarning);

   return S_OK;
}

HRESULT ASSISTANT::Project::SetUseKeyframes(bool bUseKeyframes)
{
   if (!bSupportsKeyframes_)
      return E_FAIL;

   useKeyframes_ = bUseKeyframes;

   return S_OK;
}

HRESULT ASSISTANT::Project::SetKeyframes(UINT nKeyframes)
{
   if (!bSupportsKeyframes_)
      return E_FAIL;

   keyframeDist_ = nKeyframes;

   return S_OK;
}

HRESULT ASSISTANT::Project::SetUseDatarate(bool bUseDatarate)
{
   if (!bSupportsDatarate_)
      return E_FAIL;

   useDatarate_ = bUseDatarate;

   return S_OK;
}

HRESULT ASSISTANT::Project::SetDatarate(UINT nDatarate)
{
   if (!bSupportsDatarate_)
      return E_FAIL;

   videoDatarate_ = nDatarate;

   return S_OK;
}


HRESULT ASSISTANT::Project::SetCodecQuality(UINT nQuality)
{
   if (!bSupportsQuality_)
      return E_FAIL;

   videoQuality_ = nQuality;

   return S_OK;
}

bool ASSISTANT::Project::QualityPossible()
{
   return bSupportsQuality_;
}

bool ASSISTANT::Project::KeyframesPossible()
{
   return bSupportsKeyframes_;
}

bool ASSISTANT::Project::DataratePossible()
{
   return bSupportsDatarate_;
}

bool ASSISTANT::Project::CodecHasAbout()
{
   return hasAboutDialog_;
}

bool ASSISTANT::Project::CodecCanBeConfigured()
{
   return hasConfigureDialog_;
}

void ASSISTANT::Project::ReadVideoSettings(bool bShowWarning, bool bSet)
{
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
   DWORD
      subValues;
   TCHAR
      entryName[100];
   TCHAR
      entryValue[512];
   DWORD
      entryNameLength = 100,
      entryValueLength = 512,
      dwType;

   // Read SG options
   CString csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\Video");

   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_READ, &hKeyUser);
   if (regErr == ERROR_SUCCESS )
   {
      regErr = RegQueryInfoKey(hKeyUser, NULL, NULL, NULL, NULL, NULL, NULL, &subValues, NULL, NULL, NULL, NULL);
      
      if ( regErr == ERROR_SUCCESS )
      {
         for (int i = 0; i < subValues; i++) 
         {             
            entryNameLength = 100;
            entryValueLength = 512;
            if (RegEnumValue(hKeyUser, i, entryName, &entryNameLength, NULL, &dwType, (unsigned char *)entryValue, &entryValueLength) == ERROR_SUCCESS)
            {
               CString csEntry = entryName;
               if (dwType == REG_SZ)
               {
                  if (csEntry == _T("DEVICE"))
                  {
                     wdmDevice_ = (TCHAR *)entryValue;
                  }
                  else if (csEntry == _T("FRAMERATE"))
                  {
                     TCHAR *stopstring;
                     videoFramerate_ = _tcstod((TCHAR *)entryValue, &stopstring);
                  }
                  else if (csEntry == _T("RECORDVIDEO"))
                  {
                     isVideoEnabled_ = _ttoi((TCHAR *)entryValue) == 0 ? false : true;
                  }
                  else if (csEntry == _T("CODEC"))
                  {
                     videoCodec_ = (TCHAR *)entryValue;
                  }
                  else if (csEntry == _T("USEQUALITY"))
                  {
                     bSupportsQuality_ = _ttoi((TCHAR *)entryValue) == 0 ? false : true;
                  }
                  else if (csEntry == _T("QUALITY"))
                  {
                     videoQuality_ = _ttoi((TCHAR *)entryValue);
                  }
                  else if (csEntry == _T("USEKEYFRAMES"))
                  {
                     useKeyframes_ = _ttoi((TCHAR *)entryValue) == 0 ? false : true;
                  }
                  else if (csEntry == _T("KEYFRAMES"))
                  {
                     keyframeDist_ = _ttoi((TCHAR *)entryValue);
                  }
                  else if (csEntry == _T("USEDATARATE"))
                  {
                     useDatarate_ = _ttoi((TCHAR *)entryValue) == 0 ? false : true;
                  }
                  else if (csEntry == _T("DATARATE"))
                  {
                     videoDatarate_ = _ttoi((TCHAR *)entryValue);
                  }
                  else if (csEntry == _T("USEADVANCEDAVSYNC"))
                  {
                     isAdvancedSyncSet_ = _ttoi((TCHAR *)entryValue) == 0 ? false : true;
                  }
                  else if (csEntry == _T("AVOFFSET"))
                  {
                     avOffset_ = _ttoi((TCHAR *)entryValue);
                  }
                  else if (csEntry == _T("USEAVOFFSET"))
                  {
                     hasAVOffset_ = _ttoi((TCHAR *)entryValue) == 0 ? false : true;;
                  }
                  else if (csEntry == _T("CONVERTDVDATA"))
                  {
                     if (avgrabber_)
                     {
                        if (entryValueLength > 0)
                        {
                           int zw = _ttoi((TCHAR *)entryValue);
                           if (zw == 0)
                              avgrabber_->setWdmConvertDvDataToRgb(false);
                           else
                              avgrabber_->setWdmConvertDvDataToRgb(true);
                        }
                     }
                  }
                  else if (csEntry == _T("DVRESOLUTION"))
                  {
                     if (avgrabber_)
                     {
                        if (entryValueLength > 0)
                        {
                           int zw = _ttoi((TCHAR *)entryValue);
                           avgrabber_->setWdmDvResolution((DVRES)zw);
                        }
                     }
                  }
               }
               else if (dwType == REG_BINARY)
               {
                  if (csEntry == _T("MEDIATYPE"))
                  {
                      // TODO enable this; memory leak; not enabled because 1h before patch release
                      //if (mediaTypeData_ != NULL)
                      //    delete[] mediaTypeData_;
                      // TODO why is this an instance variable anyway?

                     mediaTypeSize_ = entryValueLength;
                     mediaTypeData_ = new char[mediaTypeSize_ + 1];
                     strncpy(mediaTypeData_, (char *)entryValue, mediaTypeSize_);
                     mediaTypeData_[mediaTypeSize_] = '\000';
                  }
                  else if (csEntry == _T("VIDEOHEADER"))
                  {
                      // TODO enable this; memory leak; not enabled because 1h before patch release
                      //if (videoHeaderData_ != NULL)
                      //    delete[] videoHeaderData_;

                     videoHeaderSize_ = entryValueLength;
                     videoHeaderData_ = new char[videoHeaderSize_ + 1];
                     strncpy(videoHeaderData_, (char *)entryValue, videoHeaderSize_);
                     videoHeaderData_[videoHeaderSize_] = '\000';
                  }
               }
            }
         }
      }
      RegCloseKey(hKeyUser);
   }
   
   if (bSet)
      SetWdmDevice(bShowWarning);
}

HRESULT ASSISTANT::Project::WriteVideoSettings()
{
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
   DWORD
      dwDummy;
   DWORD
      entryNameLength = 100,
      entryValueLength = 512;

   // Read Audio settings
   CString csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\Video");
   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_ALL_ACCESS, &hKeyUser);
   
   if ( regErr != ERROR_SUCCESS )
   {
      regErr = RegCreateKeyEx(HKEY_CURRENT_USER, csSubKey,
                              NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyUser, &dwDummy); 
      
      if ( regErr != ERROR_SUCCESS )
         return E_FAIL;
   }

   if (regErr == ERROR_SUCCESS )
   {
      TCHAR value[512];

      regErr = RegSetValueEx(hKeyUser, _T("DEVICE"), 0, REG_SZ, 
                             (BYTE *)(LPCTSTR)wdmDevice_, wdmDevice_.GetLength()*sizeof(TCHAR));
      
      _stprintf(value, _T("%2.2f"), videoFramerate_);
      regErr = RegSetValueEx(hKeyUser, _T("FRAMERATE"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));

      _stprintf(value, _T("%d"), isVideoEnabled_);
      regErr = RegSetValueEx(hKeyUser, _T("RECORDVIDEO"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));

      _stprintf(value, _T("%s"), videoCodec_);
      regErr = RegSetValueEx(hKeyUser, _T("CODEC"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));

      _stprintf(value, _T("%d"), bSupportsQuality_);
      regErr = RegSetValueEx(hKeyUser, _T("USEQUALITY"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), videoQuality_);
      regErr = RegSetValueEx(hKeyUser, _T("QUALITY"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), useKeyframes_);
      regErr = RegSetValueEx(hKeyUser, _T("USEKEYFRAMES"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), keyframeDist_);
      regErr = RegSetValueEx(hKeyUser, _T("KEYFRAMES"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), useDatarate_);
      regErr = RegSetValueEx(hKeyUser, _T("USEDATARATE"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), videoDatarate_);
      regErr = RegSetValueEx(hKeyUser, _T("DATARATE"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), isAdvancedSyncSet_);
      regErr = RegSetValueEx(hKeyUser, _T("USEADVANCEDAVSYNC"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));

      _stprintf(value, _T("%d"), avOffset_);
      regErr = RegSetValueEx(hKeyUser, _T("AVOFFSET"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));

      _stprintf(value, _T("%d"), hasAVOffset_);
      regErr = RegSetValueEx(hKeyUser, _T("USEAVOFFSET"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));

      if (avgrabber_)
      {
         
         try {
            int mediaTypeSize = avgrabber_->getWdmStreamConfigMediaTypeSize(); 
            int videoHeaderSize = avgrabber_->getWdmStreamConfigVideoHeaderSize(); 
            if ((mediaTypeSize >= 0) && (videoHeaderSize >= 0))
            {
               char *mediaTypeData = new char[mediaTypeSize+1];
               char *videoHeaderData = new char[videoHeaderSize+1];
               avgrabber_->getWdmStreamConfigData(mediaTypeData, mediaTypeSize,
                  videoHeaderData, videoHeaderSize);
               mediaTypeData[mediaTypeSize] = 0;
               videoHeaderData[videoHeaderSize] = 0;

               regErr = RegSetValueEx(hKeyUser, _T("MEDIATYPE"), 0, REG_BINARY , 
                                      (BYTE *)mediaTypeData, strlen(mediaTypeData));
               
               if (videoHeaderSize != 0)
               {
                  regErr = RegSetValueEx(hKeyUser, _T("VIDEOHEADER"), 0, REG_BINARY , 
                                         (BYTE *)videoHeaderData, strlen(videoHeaderData));
               }
               delete[] mediaTypeData;
               delete[] videoHeaderData;
            }
         }
         catch (exception &e)
         {
            MessageBoxA(NULL, e.what(), "WriteVideoSettings", MB_OK | MB_TOPMOST);
         }
         
         _stprintf(value, _T("%d"), avgrabber_->getWdmConvertDvDataToRgb());
         regErr = RegSetValueEx(hKeyUser, _T("CONVERTDVDATA"), 0, REG_SZ, 
                                (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
         
         _stprintf(value, _T("%d"), avgrabber_->getWdmDvResolution());
         regErr = RegSetValueEx(hKeyUser, _T("DVRESOLUTION"), 0, REG_SZ, 
                                (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      }


      RegCloseKey(hKeyUser);
   }

   return S_OK;
}

void ASSISTANT::Project::RetrieveCodecs(bool bShowWarning)
{
   if (!avgrabber_)
      return;
   
   DeleteCodecInfo();
   
   try 
   {
      codecCount_ = avgrabber_->getCodecCount();
      isCodecEnabled_ = true;
      if (codecCount_ > 0)
      {
         CreateCodecInfo();
         SetVideoCodec(bShowWarning);
      } 
      else 
      {
         isCodecEnabled_ = false;
      }
   }
   catch (exception &e)
   {
      DeleteCodecInfo();
      isVideoEnabled_ = false;
      MessageBoxA(NULL, e.what(), "RetrieveCodecs", MB_OK | MB_TOPMOST);
   }
}

void ASSISTANT::Project::InitWdmDevices()
{
   wdmDeviceCount_ = avgrabber_->getWdmDeviceCount();
   
   if (wdmDeviceCount_ > 0)
   {
      wdmDeviceNames_ = new TCHAR*[wdmDeviceCount_];
      for (int i = 0; i < wdmDeviceCount_; ++i)
         wdmDeviceNames_[i] = new TCHAR[256];
      avgrabber_->getWdmDeviceNames(wdmDeviceNames_, 256);
   }
   else
   {
      wdmDeviceCount_ = 0;
      wdmDeviceNames_ = NULL;
   }
}

void ASSISTANT::Project::DeleteWdmDevices()
{
   if (wdmDeviceNames_)
   {
      for (int i = 0; i < wdmDeviceCount_; ++i)
         delete[] wdmDeviceNames_[i];
      delete[] wdmDeviceNames_;
      wdmDeviceNames_ = NULL;
   }
   wdmDeviceCount_ = 0;
}

void ASSISTANT::Project::InitWdmInterfaces()
{
   if (wdmInterfaces_)
      DeleteWdmInterfaces();
   
   wdmInterfaceCount_ = avgrabber_->getWdmInterfaceCount();
   
   if (wdmInterfaceCount_ > 0)
   {
      wdmInterfaces_ = new WDM_INTERFACE[wdmInterfaceCount_];
      avgrabber_->getWdmInterfaces(wdmInterfaces_);
   }
   else
   {
      wdmInterfaceCount_ = 0;
      wdmInterfaces_ = NULL;
   }
}

void ASSISTANT::Project::DeleteWdmInterfaces()
{
   if (wdmInterfaces_)
   {
      delete[] wdmInterfaces_;
      wdmInterfaces_ = NULL;
   }
   wdmInterfaceCount_ = 0;
}

bool ASSISTANT::Project::IsWdmDeviceInList()
{
   bool retValue = false;
   
   if (wdmDevice_.IsEmpty() && wdmDeviceCount_ > 0)
      wdmDevice_ = wdmDeviceNames_[0];

   for (int i = 0; i < wdmDeviceCount_; ++i)
   {
      if (wdmDevice_ == wdmDeviceNames_[i])
      {
         retValue = true;
         break;
      }
   }
   
   return retValue;
}

bool ASSISTANT::Project::SetWdmDevice(bool bShowWarning)
{
   if (wdmDeviceCount_ > 0)
   {
    isWdmDeviceSet_ = false;
        avgrabber_->deleteVideoDevice();
      
      if (!IsWdmDeviceInList())
      {
            wdmDevice_ = wdmDeviceNames_[0];
            CString csMessage;
            csMessage.Format(IDS_ERROR_SET_VIDEODEVICE);
            MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
            isVideoEnabled_ = false;
        }
      
        // This code made it impossible for a WDM device
        // to have a default frame rate other than 25 fps.
        // By not setting the default frame rate, the frame
        // rate is read out in the UpdateVideoPage() method.
        // If no frame rate has been specified up to now (e.g.
        // in a project file), the default frame rate is taken,
        // otherwise the frame rate is set by the
        // setWdmStreamConfigData() method call below.
      
        //if (videoFramerate_ == 0)
        //{
        //   videoFramerate_ = 25;
        //}
      
      
      try 
      {
         if (isVideoEnabled_)
         {
                avgrabber_->setWdmVideoDevice((TCHAR *)(LPCTSTR)wdmDevice_);
                avgrabber_->setUseAdvancedAudioSync(isAdvancedSyncSet_);
                avgrabber_->setUseFixedAudioVideoOffset(hasAVOffset_, avOffset_);
            
            if (mediaTypeData_ != NULL)
            {
               try
               {
                        avgrabber_->setWdmStreamConfigData(mediaTypeData_, mediaTypeSize_,
                            videoHeaderData_, videoHeaderSize_);
                    }
               catch (exception &e)
               {
                  char message[256];
                  sprintf(message, "Note: Could not set WDM stream config data.\nPlease check WDM settings\n%s", e.what());
                  ::MessageBoxA(NULL, message, "Warning", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
               }
               
                    delete[] mediaTypeData_;
                    mediaTypeData_ = NULL;
                    mediaTypeSize_ = 0;
                    if (videoHeaderData_)
                        delete[] videoHeaderData_;
                    videoHeaderData_ = NULL;
                    videoHeaderSize_ = 0;
                }
            
                InitWdmInterfaces();
                RetrieveCodecs(bShowWarning);
                // See above: Either the frame rate was set with
                // the WDM stream config data, or the default frame
                // rate of the WDM device is used.
                //if (videoFramerate_ > 0)
                //   avgrabber_->setVideoFrameRate(videoFramerate_);
                avgrabber_->setVideoMonitor(isMonitorEnabled_);
                isWdmDeviceSet_ = true;
            }
      }
      catch (exception &e)
      {
            videoFramerate_       = 0;
            isVideoEnabled_       = false;
         MessageBoxA(NULL, e.what(), "SetWdmDevice", MB_OK | MB_TOPMOST);
        }
        }
   else 
   {
      if (isVideoEnabled_)
      {
            isVideoEnabled_ = false;
            CString csMessage;
            csMessage.LoadString(IDS_NO_VIDEO_DEVICE);
            MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
        }
        isCodecEnabled_ = false;
    }
   
    avgrabber_->setEnableVideo(isVideoEnabled_);
   
   return true;
}

bool ASSISTANT::Project::HasWdmSettings()
{
   if (!avgrabber_)
      return false;
   
   for (int i = 0; i < wdmInterfaceCount_; ++i)
   {
      if (wdmInterfaces_[i].widId == ID_CAPTUREFILTER)
         return true;
   }

   return false;
}

bool ASSISTANT::Project::HasWdmSource()
{
   if (!avgrabber_)
      return false;
   
   for (int i = 0; i < wdmInterfaceCount_; ++i)
   {
      if (wdmInterfaces_[i].widId == ID_CROSSBAR)
         return true;
   }

   return false;
}

bool ASSISTANT::Project::HasWdmFormat()
{
   if (!avgrabber_)
      return false;
   
   for (int i = 0; i < wdmInterfaceCount_; ++i)
   {
      if (wdmInterfaces_[i].widId == ID_STREAMCONFIG)
         return true;
   }

   return false;
}

bool ASSISTANT::Project::HasWdmDv()
{
   if (!avgrabber_)
      return false;
   
   for (int i = 0; i < wdmInterfaceCount_; ++i)
   {
      if (wdmInterfaces_[i].widId == ID_DV_DECODER)
         return true;
   }

   return false;
}



void ASSISTANT::Project::ShowWdmSettings()
{
   if (!avgrabber_)
      return;
   
   for (int i = 0; i < wdmInterfaceCount_; ++i)
   {
      if (wdmInterfaces_[i].widId == ID_CAPTUREFILTER)
      {
         avgrabber_->displayWdmConfigurationDialog(wdmInterfaces_[i].widId);
         RetrieveCodecs(true);
         break;
      }
   }

   return;
}

void ASSISTANT::Project::ShowWdmSource()
{
   if (!avgrabber_)
      return;
   
   for (int i = 0; i < wdmInterfaceCount_; ++i)
   {
      if (wdmInterfaces_[i].widId == ID_CROSSBAR)
      {
         avgrabber_->displayWdmConfigurationDialog(wdmInterfaces_[i].widId);
         RetrieveCodecs(true);
         break;
      }
   }

   return;
}

void ASSISTANT::Project::ShowWdmFormat()
{
   if (!avgrabber_)
      return;
   
   for (int i = 0; i < wdmInterfaceCount_; ++i)
   {
      if (wdmInterfaces_[i].widId == ID_STREAMCONFIG)
      {
         avgrabber_->displayWdmConfigurationDialog(wdmInterfaces_[i].widId);
         RetrieveCodecs(true);
         break;
      }
   }

   return;
}

void ASSISTANT::Project::ShowWdmDv()
{
   if (!avgrabber_)
      return;
   
   for (int i = 0; i < wdmInterfaceCount_; ++i)
   {
      if (wdmInterfaces_[i].widId == ID_DV_DECODER)
      {
         avgrabber_->displayWdmConfigurationDialog(wdmInterfaces_[i].widId);
         RetrieveCodecs(true);
         break;
      }
   }

   return;
}

void ASSISTANT::Project::SetVideoCodec(bool bShowWarning)
{
   if (!avgrabber_)
      return;
   
   SetCodecNumber(bShowWarning);

   if (codecCount_ <= 0 || codecNumber_ < 0)
   {
      isCodecEnabled_     = false;
      hasAboutDialog_     = false;
      hasConfigureDialog_ = false;
      bSupportsQuality_   = false;
      bSupportsKeyframes_ = false;
      bSupportsDatarate_  = false;
      return;
   }
   
   videoCodec_ = codecInfo_[codecNumber_]->fccString;
   
   try 
   {
      isCodecEnabled_ = true;
      bSupportsKeyframes_ = codecInfo_[codecNumber_]->bSupportsKeyframes;
      if (bSupportsKeyframes_)
      {
         codecInfo_[codecNumber_]->bUseKeyframes = useKeyframes_;
         codecInfo_[codecNumber_]->iKeyframeRate = keyframeDist_;
      }
      
      bSupportsDatarate_ = codecInfo_[codecNumber_]->bSupportsDatarate;
      if (bSupportsDatarate_)
      {
         codecInfo_[codecNumber_]->bUseDatarate = useDatarate_;
         codecInfo_[codecNumber_]->iDatarate    = videoDatarate_;
      }
      
      bSupportsQuality_ = codecInfo_[codecNumber_]->bSupportsQuality;
      if (bSupportsQuality_)
      {
         codecInfo_[codecNumber_]->iQuality = videoQuality_;
      }
      
      avgrabber_->setVideoCodec(codecInfo_[codecNumber_]);
      
      hasAboutDialog_     = codecInfo_[codecNumber_]->bHasInfoDialog;
      hasConfigureDialog_ = codecInfo_[codecNumber_]->bHasConfigDialog;
   }
   catch (exception &e) 
   {
      isCodecEnabled_ = false;
      MessageBoxA(NULL, e.what(), "SetVideoCodec", MB_OK | MB_TOPMOST);
   }
   
   return;
}

// Screen grabbing

void ASSISTANT::Project::ReadSgHotkeys()
{
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
   DWORD
      subValues;
   TCHAR
      entryName[100];
   TCHAR
      entryValue[512];
   DWORD
      entryNameLength = 100,
      entryValueLength = 512,
      dwType;

   // Read SG options
   CString csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\Hotkeys");

   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_READ, &hKeyUser);
   if (regErr == ERROR_SUCCESS )
   {
      regErr = RegQueryInfoKey(hKeyUser, NULL, NULL, NULL, NULL, NULL, NULL, &subValues, NULL, NULL, NULL, NULL);
      
      if ( regErr == ERROR_SUCCESS )
      {
         
         for (int i = 0; i < subValues; i++) 
         {             
            entryNameLength = 100;
            entryValueLength = 512;
            if (RegEnumValue(hKeyUser, i, entryName, &entryNameLength, NULL, &dwType, (BYTE *)entryValue, &entryValueLength) == ERROR_SUCCESS)
            {
               if (dwType == REG_SZ)
               {
                  CString csEntry = entryName;
                  if (csEntry == _T("STARTSTOP"))
                  {
                     CString csHotkey = entryValue;
                     if (csHotkey[0] == _T('1'))
                        m_hkStartStop.hasCtrl = true;
                     else
                        m_hkStartStop.hasCtrl = false;
                     
                     if (csHotkey[2] == _T('1'))
                        m_hkStartStop.hasShift = true;
                     else
                        m_hkStartStop.hasShift = false;
                     
                     if (csHotkey[4] == _T('1'))
                        m_hkStartStop.hasAlt = true;
                     else
                        m_hkStartStop.hasAlt = false;

					 if(csHotkey.GetLength() < 10)
						m_hkStartStop.vKeyString = csHotkey.Right(csHotkey.GetLength() - 6);
                  }
               }
            }
         }
      }
      RegCloseKey(hKeyUser);
   }
   
}

HRESULT ASSISTANT::Project::WriteSgHotkeys()
{
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
   DWORD
      dwDummy;
   DWORD
      entryNameLength = 100,
      entryValueLength = 512;

   CString csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\Hotkeys");
   // Read SG options
   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_ALL_ACCESS, &hKeyUser);
   
   if ( regErr != ERROR_SUCCESS )
   {
      regErr = RegCreateKeyEx(HKEY_CURRENT_USER, csSubKey,
                              NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyUser, &dwDummy); 
      
      if ( regErr != ERROR_SUCCESS )
         return E_FAIL;
   }

   if (regErr == ERROR_SUCCESS )
   {
      CString csNone;
      csNone.LoadString(IDS_SG_HOTKEYS_CMB_NONE);
      TCHAR szHotkey[256];
      //if (m_hkStartStop.vKeyString != csNone)
      {
         _stprintf(szHotkey, _T("%d %d %d %s"), 
                             m_hkStartStop.hasCtrl, m_hkStartStop.hasShift, 
                             m_hkStartStop.hasAlt, m_hkStartStop.vKeyString);
         
         regErr = RegSetValueEx(hKeyUser, _T("STARTSTOP"), 0, REG_SZ, 
                                (BYTE *)szHotkey, _tcslen(szHotkey)*sizeof(TCHAR));
      }

      RegCloseKey(hKeyUser);
   }

   return S_OK;
   
}

void ASSISTANT::Project::ReadSgRegistrySettings()
{
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
   DWORD
      subValues;
   TCHAR
      entryName[100];
   TCHAR
      entryValue[512];
   DWORD
      entryNameLength = 100,
      entryValueLength = 512,
      dwType;

   CString csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\SgOptions");
   // Read SG options
   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_READ, &hKeyUser);
   if (regErr == ERROR_SUCCESS )
   {
      regErr = RegQueryInfoKey(hKeyUser, NULL, NULL, NULL, NULL, NULL, NULL, &subValues, NULL, NULL, NULL, NULL);
      
      if ( regErr == ERROR_SUCCESS )
      {
         
         for (int i = 0; i < subValues; i++) 
         {             
            entryNameLength = 100;
            entryValueLength = 512;
            if (RegEnumValue(hKeyUser, i, entryName, &entryNameLength, NULL, &dwType, (unsigned char *)entryValue, &entryValueLength) == ERROR_SUCCESS)
            {
               if (dwType == REG_SZ)
               {
                  CString csEntry = entryName;
                  if (csEntry == _T("ADAPTIVEFRAMERATE"))
                  {
                     adaptiveFramerate_ = _ttoi(entryValue);
                  }
                  else if (csEntry == _T("STATICFRAMERATE"))
                  {
                     staticFramerate_ = _ttoi(entryValue);
                  }
                  else if (csEntry == _T("FIXEDPOS"))
                  {
                     grabAtFixedPos_ = _ttoi(entryValue) == 1 ? true : false;
                  }
                  else if (csEntry == _T("MOUSEAUDIO"))
                  {
                     mouseAudioEffect_ = _ttoi(entryValue) == 1 ? true : false;
                  }
                  else if (csEntry == _T("MOUSEVISUAL"))
                  {
                     mouseVisualEffect_ = _ttoi(entryValue) == 1 ? true : false;
                  }
                  else if (csEntry == _T("POSX"))
                  {
                     startAtPosX_ = _ttoi(entryValue);
                     m_rcSgRectangle.left = _ttoi(entryValue);
                  }
                  else if (csEntry == _T("POSY"))
                  {
                     startAtPosY_ = _ttoi(entryValue);
                     m_rcSgRectangle.top = _ttoi(entryValue);
                  }
                  else if (csEntry == _T("POSWIDTH"))
                  {
                     startWithWidth_ = _ttoi(entryValue);
                     m_rcSgRectangle.right = m_rcSgRectangle.left + _ttoi(entryValue);
                  }
                  else if (csEntry == _T("POSHEIGHT"))
                  {
                     startWithHeight_ = _ttoi(entryValue);
                     m_rcSgRectangle.bottom = m_rcSgRectangle.top + _ttoi(entryValue);
                  }
                  else if (csEntry == _T("STATIC"))
                  {
                     hasStaticFramerate_ = true;
                     CString csValue = entryValue;
                     if (csValue == _T("adaptive"))
                        hasStaticFramerate_ = false;
                  }
                  else if (csEntry == _T("WINHEIGHT"))
                  {
                     adaptHeight_ = _ttoi(entryValue);
                  }
                  else if (csEntry == _T("WINWIDTH"))
                  {
                     adaptWidth_ = _ttoi(entryValue);
                  }
                  else if (csEntry == _T("CODEC"))
                  {
                     sgCodec_ = entryValue;
                  }
                  else if (csEntry == _T("QUALITY"))
                  {
                     sgQuality_ = _ttoi(entryValue);
                  }
                  else if (csEntry == _T("USEKEYFRAMES"))
                  {
                     sgUseKeyframes_ = _ttoi(entryValue) == 1 ? true : false;
                  }
                  else if (csEntry == _T("KEYFRAMES"))
                  {
                     sgKeyframeDist_ = _ttoi(entryValue);
                  }
                  else if (csEntry == _T("USEDATARATE"))
                  {
                     sgUseDatarate_ = _ttoi(entryValue) == 1 ? true : false;
                  }
                  else if (csEntry == _T("DATARATE"))
                  {
                     sgDatarate_ = _ttoi(entryValue);
                  }
               }
            }
         }
      }
      RegCloseKey(hKeyUser);
   }
   
   csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\SgSettings");

   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_READ, &hKeyUser);
   if (regErr == ERROR_SUCCESS )
   {
      regErr = RegQueryInfoKey(hKeyUser, NULL, NULL, NULL, NULL, NULL, NULL, &subValues, NULL, NULL, NULL, NULL);
      
      if ( regErr == ERROR_SUCCESS )
      {
         
         for (int i = 0; i < subValues; i++) 
         {             
            entryNameLength = 100;
            entryValueLength = 512;
            if (RegEnumValue(hKeyUser, i, entryName, &entryNameLength, NULL, &dwType, (unsigned char *)entryValue, &entryValueLength) == ERROR_SUCCESS)
            {
               if (dwType == REG_SZ)
               {
                  CString csEntry = entryName;
                  if (csEntry == _T("DELANNO"))
                  {
                     deleteAllOnInteraction_ = _ttoi(entryValue) == 1 ? true : false;
                  }
                  else if (csEntry == _T("NOHWACCEL"))
                  {
                     noHardwareAcceleration_ = _ttoi(entryValue) == 1 ? true : false;
                  }
                  else if (csEntry == _T("NOTHIDEWHILE"))
                  {
                     hideWhilePanning_ = _ttoi(entryValue) == 0 ? true : false;
                  }
                  else if (csEntry == _T("PAUSEVIDEO"))
                  {
                     pauseVideo_ = _ttoi(entryValue) == 1 ? true : false;
                  }
                  else if (csEntry == _T("QUICKCAPTURE"))
                  {
                     quickCapture_ = _ttoi(entryValue) == 1 ? true : false;
                  }
                  else if (csEntry == _T("REPORTDROPPED"))
                  {
                     reportDroppedFrames_ = _ttoi(entryValue) == 1 ? true : false;
                  }
               }
            }
         }
      }
      RegCloseKey(hKeyUser);
   }
}

HRESULT ASSISTANT::Project::WriteSgRegistrySettings()
{
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
   DWORD
      dwDummy;
   DWORD
      entryNameLength = 100,
      entryValueLength = 512;

   // Read SG options
   CString csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\SgOptions");

   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_ALL_ACCESS, &hKeyUser);
   
   if ( regErr != ERROR_SUCCESS )
   {
      regErr = RegCreateKeyEx(HKEY_CURRENT_USER, csSubKey,
                              NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyUser, &dwDummy); 
      
      if ( regErr != ERROR_SUCCESS )
         return E_FAIL;
   }

   if (regErr == ERROR_SUCCESS )
   {
      
      TCHAR value[512];

      _stprintf(value, _T("%d"), (int)adaptiveFramerate_);
      regErr = RegSetValueEx(hKeyUser, _T("ADAPTIVEFRAMERATE"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      _stprintf(value, _T("%d"), (int)staticFramerate_);
      regErr = RegSetValueEx(hKeyUser, _T("STATICFRAMERATE"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      _stprintf(value, _T("%d"), grabAtFixedPos_);
      regErr = RegSetValueEx(hKeyUser, _T("FIXEDPOS"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      _stprintf(value, _T("%d"), mouseAudioEffect_);
      regErr = RegSetValueEx(hKeyUser, _T("MOUSEAUDIO"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      _stprintf(value, _T("%d"), mouseVisualEffect_);
      regErr = RegSetValueEx(hKeyUser, _T("MOUSEVISUAL"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      _stprintf(value, _T("%d"), m_rcSgRectangle.left); //startAtPosX_);
      regErr = RegSetValueEx(hKeyUser, _T("POSX"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      _stprintf(value, _T("%d"), m_rcSgRectangle.top); //startAtPosY_);
      regErr = RegSetValueEx(hKeyUser, _T("POSY"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      _stprintf(value, _T("%d"), m_rcSgRectangle.Width()); //startWithWidth_);
      regErr = RegSetValueEx(hKeyUser, _T("POSWIDTH"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      _stprintf(value, _T("%d"), m_rcSgRectangle.Height()); //startWithHeight_);
      regErr = RegSetValueEx(hKeyUser, _T("POSHEIGHT"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      if (hasStaticFramerate_)
         _stprintf(value, _T("%s"), _T("static"));
      else
         _stprintf(value, _T("%s"), _T("adaptive"));
      regErr = RegSetValueEx(hKeyUser, _T("STATIC"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      _stprintf(value, _T("%d"), adaptHeight_);
      regErr = RegSetValueEx(hKeyUser, _T("WINHEIGHT"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
   
      _stprintf(value, _T("%d"), adaptWidth_);
      regErr = RegSetValueEx(hKeyUser, _T("WINWIDTH"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      regErr = RegSetValueEx(hKeyUser, _T("CODEC"), 0, REG_SZ, (BYTE *)(LPCTSTR)sgCodec_, sgCodec_.GetLength()*sizeof(TCHAR));

      _stprintf(value, _T("%d"), sgQuality_);
      regErr = RegSetValueEx(hKeyUser, _T("QUALITY"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), sgUseKeyframes_);
      regErr = RegSetValueEx(hKeyUser, _T("USEKEYFRAMES"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), sgKeyframeDist_);
      regErr = RegSetValueEx(hKeyUser, _T("KEYFRAMES"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), sgUseDatarate_);
      regErr = RegSetValueEx(hKeyUser, _T("USEDATARATE"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), sgDatarate_);
      regErr = RegSetValueEx(hKeyUser, _T("DATARATE"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
   }

   RegCloseKey(hKeyUser);
   


   csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\SgSettings");
   
   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_ALL_ACCESS, &hKeyUser);
   
   if ( regErr != ERROR_SUCCESS )
   {
      regErr = RegCreateKeyEx(HKEY_CURRENT_USER, csSubKey,
         NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyUser, &dwDummy); 
      
      if ( regErr != ERROR_SUCCESS )
         return E_FAIL;
   }
   
   if (regErr == ERROR_SUCCESS )
   {
      TCHAR value[512];
      
      _stprintf(value, _T("%d"), deleteAllOnInteraction_);
      regErr = RegSetValueEx(hKeyUser, _T("DELANNO"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), noHardwareAcceleration_);
      regErr = RegSetValueEx(hKeyUser, _T("NOHWACCEL"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), !noHardwareAcceleration_);
      regErr = RegSetValueEx(hKeyUser, _T("NOTHIDEWHILE"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), pauseVideo_);
      regErr = RegSetValueEx(hKeyUser, _T("PAUSEVIDEO"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), quickCapture_);
      regErr = RegSetValueEx(hKeyUser, _T("QUICKCAPTURE"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), reportDroppedFrames_);
      regErr = RegSetValueEx(hKeyUser, _T("REPORTDROPPED"), 0, REG_SZ, (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
   }
   RegCloseKey(hKeyUser);

   return S_OK;

}

HRESULT ASSISTANT::Project::GetMouseEffects(bool &bMouseVisual, bool &bMouseAcoustic)
{
   bMouseVisual = mouseVisualEffect_;
   bMouseAcoustic = mouseAudioEffect_;


   return S_OK;
}

HRESULT ASSISTANT::Project::SetMouseEffects(bool bMouseVisual, bool bMouseAcoustic)
{
   mouseVisualEffect_ = bMouseVisual;
   mouseAudioEffect_ = bMouseAcoustic;

   return S_OK;
}

HRESULT ASSISTANT::Project::GetFramerateSettings(bool &bStaticFramerate, 
                                                 float &fStaticFramerate, float &fAdaptiveFramerate,
                                                 UINT &nWidth, UINT &nHeight)
{
   bStaticFramerate = hasStaticFramerate_;
   fStaticFramerate = staticFramerate_;
   fAdaptiveFramerate = adaptiveFramerate_;
   nWidth = adaptWidth_;
   nHeight = adaptHeight_;

   return S_OK;
}

HRESULT ASSISTANT::Project::SetFramerateSettings(bool bStaticFramerate, 
                                                 float fStaticFramerate, float fAdaptiveFramerate,
                                                 UINT nWidth, UINT nHeight)
{
   hasStaticFramerate_ = bStaticFramerate;
   staticFramerate_ = fStaticFramerate;
   adaptiveFramerate_ = fAdaptiveFramerate;
   adaptWidth_ = nWidth;
   adaptHeight_ = nHeight;

   WriteSgRegistrySettings();
   return S_OK;
}


HRESULT ASSISTANT::Project::GetRegionSettings(bool &bRegionFixed, UINT &nFixedX, UINT &nFixedY, 
                                              UINT &nFixedWidth, UINT &nFixedHeight)
{
   bRegionFixed = grabAtFixedPos_;
   nFixedX = m_rcSgRectangle.left;
   nFixedY = m_rcSgRectangle.top;
   nFixedWidth = m_rcSgRectangle.Width();
   nFixedHeight = m_rcSgRectangle.Height();

   WriteSgRegistrySettings();
   return S_OK;
}

HRESULT ASSISTANT::Project::SetRegionSettings(bool bRegionFixed, UINT nFixedX, UINT nFixedY, 
                                              UINT nFixedWidth, UINT nFixedHeight)
{
   grabAtFixedPos_ = bRegionFixed;
   m_rcSgRectangle.left = nFixedX;
   m_rcSgRectangle.top = nFixedY;
   m_rcSgRectangle.right = m_rcSgRectangle.left + nFixedWidth;
   m_rcSgRectangle.bottom = m_rcSgRectangle.top + nFixedHeight;

   WriteSgRegistrySettings();
   return S_OK;
}

HRESULT ASSISTANT::Project::GetSgHandling(bool &bClearAnnotation, bool &bReportDroppedFrames, bool &bBorderPanning,
                                          bool &bDisableAcceleration, bool &bQuickCapture, bool &bSuspendVideo)
{
   deleteAllOnInteraction_ = bClearAnnotation;
   bReportDroppedFrames = reportDroppedFrames_;
   bBorderPanning = !hideWhilePanning_;
   bDisableAcceleration = noHardwareAcceleration_;
   bQuickCapture = quickCapture_;
   bSuspendVideo = pauseVideo_;


   return S_OK;
}

HRESULT ASSISTANT::Project::SetSgHandling(bool bClearAnnotation, bool bReportDroppedFrames, bool bBorderPanning,
                                          bool bDisableAcceleration, bool bQuickCapture, bool bSuspendVideo)
{
   bClearAnnotation = deleteAllOnInteraction_;
   reportDroppedFrames_ = bReportDroppedFrames;
   hideWhilePanning_ = !bBorderPanning;
   
   // If the HKLM key is not writable, it is not possible to 
   // disable the hardware acceleration
   noHardwareAcceleration_ = bDisableAcceleration;
   if (bDisableAcceleration)
   {
      if (!LMisc::IsUserAdmin()) // TODO this (disabling acceleration) doesn't work anyway...
      {
         noHardwareAcceleration_ = false;

         CString csMessage;
         csMessage.LoadString(IDS_ERROR_DISABLE_HARDWAREACCELERATION);
         CString csCaption;
         csCaption.LoadString(IDS_WARNING);
         MessageBox(NULL, csMessage, csCaption, MB_OK | MB_TOPMOST);
      }
   }

   quickCapture_ = bQuickCapture;
   pauseVideo_ = bSuspendVideo;

   UpdateSgModul();
   WriteSgRegistrySettings();

   return S_OK;
}

HRESULT ASSISTANT::Project::GetSgHotkeys(AVGRABBER::HotKey &hkStartStop, AVGRABBER::HotKey &hkReduce, AVGRABBER::HotKey &hkMinimize)
{
   hkStartStop.vKeyString = m_hkStartStop.vKeyString;
   hkStartStop.hasCtrl = m_hkStartStop.hasCtrl;
   hkStartStop.hasShift = m_hkStartStop.hasShift;
   hkStartStop.hasAlt = m_hkStartStop.hasAlt;

   WriteSgHotkeys();

   return S_OK;
}

HRESULT ASSISTANT::Project::SetSgHotkeys(AVGRABBER::HotKey &hkStartStop, AVGRABBER::HotKey &hkReduce, AVGRABBER::HotKey &hkMinimize)
{
   
   m_hkStartStop.vKeyString = hkStartStop.vKeyString;
   m_hkStartStop.hasCtrl = hkStartStop.hasCtrl;
   m_hkStartStop.hasShift = hkStartStop.hasShift;
   m_hkStartStop.hasAlt = hkStartStop.hasAlt;

   ActivateStartStopHotKey(true);

   return S_OK;
}

HRESULT ASSISTANT::Project::GetSgStartStopHotKey(AVGRABBER::HotKey &hkStartStop)
{
   hkStartStop.vKeyString = m_hkStartStop.vKeyString;
   hkStartStop.hasCtrl = m_hkStartStop.hasCtrl;
   hkStartStop.hasShift = m_hkStartStop.hasShift;
   hkStartStop.hasAlt = m_hkStartStop.hasAlt;
   hkStartStop.id = m_hkStartStop.id;

   return S_OK;
}

HRESULT ASSISTANT::Project::SetSgStartStopHotKey(AVGRABBER::HotKey &hkStartStop)
{
   m_hkStartStop.vKeyString = hkStartStop.vKeyString;
   m_hkStartStop.hasCtrl = hkStartStop.hasCtrl;
   m_hkStartStop.hasShift = hkStartStop.hasShift;
   m_hkStartStop.hasAlt = hkStartStop.hasAlt;

   ActivateStartStopHotKey(true);

   return S_OK;
}

HRESULT ASSISTANT::Project::TestHotKey(AVGRABBER::HotKey &hkToTest)
{
    HRESULT hr = S_OK;

    CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();
    if (pMainWnd == NULL)   // TODO: Error message
        return E_FAIL;

    HWND hWnd = pMainWnd->m_hWnd;

    CString csNone;
    csNone.LoadString(IDS_SG_HOTKEYS_CMB_NONE);

    if (hkToTest.vKeyString != csNone) {
        unsigned int modifier = 0;
        if (hkToTest.hasCtrl)
            modifier |= MOD_CONTROL;
        if (hkToTest.hasShift)
            modifier |= MOD_SHIFT;
        if (hkToTest.hasAlt)
            modifier |= MOD_ALT;

        if (hkToTest.vKeyString == _T("F9"))
            hkToTest.vKey = VK_F9;
        else if (hkToTest.vKeyString == _T("F10"))
            hkToTest.vKey = VK_F10;
        else if (hkToTest.vKeyString == _T("F11"))
            hkToTest.vKey = VK_F11;
        else if (hkToTest.vKeyString == _T("F12"))
            hkToTest.vKey = VK_F12;
        else
            hkToTest.vKey = hkToTest.vKeyString[0];

        BOOL bResult = RegisterHotKey(hWnd, hkToTest.id, modifier, hkToTest.vKey);
        if (bResult == FALSE) 
        {
            CString csHotkey;
            csHotkey = _T("\"");
            if (hkToTest.hasCtrl)
                csHotkey += _T("Ctrl-");
            if (hkToTest.hasShift)
                csHotkey += _T("Shift-");
            if (hkToTest.hasAlt)
                csHotkey += _T("Alt-");
            csHotkey += hkToTest.vKeyString;
            csHotkey += _T("\" ");
            CString csMessage;
            csMessage.Format(IDS_ERROR_REGISTER_HOTKEY, csHotkey);
            MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
            hr = E_FAIL;
        }

        UnregisterHotKey(hWnd, hkToTest.id);
    }

    return hr;
}

HRESULT ASSISTANT::Project::ActivateStartStopHotKey(bool bActivate)
{
    HRESULT hr = S_OK;

    CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();
    if (pMainWnd == NULL)   // TODO: Error message
        return E_FAIL;

    HWND hWnd = pMainWnd->m_hWnd;

    BOOL bUnregisterResult = UnregisterHotKey(hWnd, m_hkStartStop.id);

    if (bActivate) {
        CString csNone;
        csNone.LoadString(IDS_SG_HOTKEYS_CMB_NONE);

        if (m_hkStartStop.vKeyString != csNone) {
            unsigned int modifier = 0;
            if (m_hkStartStop.hasCtrl)
                modifier |= MOD_CONTROL;
            if (m_hkStartStop.hasShift)
                modifier |= MOD_SHIFT;
            if (m_hkStartStop.hasAlt)
                modifier |= MOD_ALT;

            if (m_hkStartStop.vKeyString == _T("F9"))
                m_hkStartStop.vKey = VK_F9;
            else if (m_hkStartStop.vKeyString == _T("F10"))
                m_hkStartStop.vKey = VK_F10;
            else if (m_hkStartStop.vKeyString == _T("F11"))
                m_hkStartStop.vKey = VK_F11;
            else if (m_hkStartStop.vKeyString == _T("F12"))
                m_hkStartStop.vKey = VK_F12;
            else
                m_hkStartStop.vKey = m_hkStartStop.vKeyString[0];

            BOOL bResult = RegisterHotKey(hWnd, m_hkStartStop.id, modifier, m_hkStartStop.vKey);
            if (bResult == FALSE) {
                CString csHotkey;
                csHotkey = _T("\"");
                if (m_hkStartStop.hasCtrl)
                    csHotkey += _T("Ctrl-");
                if (m_hkStartStop.hasShift)
                    csHotkey += _T("Shift-");
                if (m_hkStartStop.hasAlt)
                    csHotkey += _T("Alt-");
                csHotkey += m_hkStartStop.vKeyString;
                csHotkey += _T("\" ");
                CString csMessage;
                csMessage.Format(IDS_ERROR_REGISTER_HOTKEY, csHotkey);
                MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
                hr = E_FAIL;
            }
        }
    }

    return hr;
}

HRESULT ASSISTANT::Project::GetCodecSettings(CStringArray &arCodecList, UINT &nCodecIndex, bool &bHasAbout, bool &bHasConfigure,
                                             bool &bSupportsQuality, UINT &nCodecQuality, 
                                             bool &bSupportsKeyframes, bool &bUseKeyframes, UINT &nKeyframeDistance, 
                                             bool &bSupportsDatarate, bool &bUseDatarte, UINT &nDatarate)
{
   for (int i = 0; i < sgCodecCount_; ++i)
   {
      CString csCodec = sgCodecInfo_[i]->fccString;
      csCodec += _T(" : ");
      csCodec += sgCodecInfo_[i]->tszDescription;
      arCodecList.Add(csCodec);
   }
   nCodecIndex = sgCodecNumber_;
   
   bHasAbout = hasSgAboutDialog_;
   bHasConfigure = hasSgConfigureDialog_;
   
   bSupportsQuality= bSgSupportsQuality_;
   nCodecQuality = sgQuality_;
   
   bSupportsKeyframes = bSupportsKeyframes_;
   bUseKeyframes = sgUseKeyframes_;
   nKeyframeDistance = sgKeyframeDist_;
   
   bSupportsDatarate = bSupportsDatarate_;
   bUseDatarte = sgUseDatarate_;
   nDatarate = sgDatarate_;

   return S_OK;
}

HRESULT ASSISTANT::Project::SetCodecSettings(CString &csCodec, UINT nCodecQuality, 
                                             bool bUseKeyframes, UINT nKeyframeDistance, bool &bUseDatarate, UINT nDatarate)
{
   CString csFccString = csCodec;
   int iPos = csFccString.Find(_T(":"));
   if (iPos > 0)
      csFccString = csFccString.Left(iPos-1);

   sgCodec_ = csFccString;
   
   sgQuality_ = nCodecQuality;

   sgUseKeyframes_ = bUseKeyframes;
   sgKeyframeDist_ = nKeyframeDistance;

   sgUseDatarate_ = bUseDatarate;
   sgDatarate_ = nDatarate;


   SetSgCodec();

   return S_OK;
}

HRESULT ASSISTANT::Project::SetSgCodec(CString &csCodec, bool bDisplayWarning)
{
   if (!avgrabber_ || sgCodecCount_ <= 0)
      return E_FAIL;
   
   CString csFccString = csCodec;
   int iPos = csFccString.Find(_T(":"));
   if (iPos > 0)
      csFccString = csFccString.Left(iPos-1);

   sgCodec_ = csFccString;

   SetSgCodec(bDisplayWarning);

   return S_OK;
}

bool ASSISTANT::Project::SgCodecHasAbout()
{
   return hasSgAboutDialog_;
}

bool ASSISTANT::Project::SgCodecCanBeConfigured()
{
   return hasSgConfigureDialog_;
}

HRESULT ASSISTANT::Project::ShowSgCodecAbout()
{
   HWND
      hwnd;
   
   if (!avgrabber_)
      return E_FAIL;
   
   hwnd = AfxGetMainWnd()->m_hWnd;
   try
   {
      avgrabber_->displaySgCodecAboutDialog(hwnd);
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
      return E_FAIL;
   }
   
   return S_OK;
}

HRESULT ASSISTANT::Project::ShowSgCodecConfigure()
{
   HWND
      hwnd;
   
   if (!avgrabber_)
      return E_FAIL;
   
   hwnd = AfxGetMainWnd()->m_hWnd;
   try
   {
      avgrabber_->displaySgCodecConfigureDialog(hwnd);
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
      return E_FAIL;
   }

   return S_OK;
}

// Record and Replay functions

HRESULT ASSISTANT::Project::StartRecording()
{
   CString sAudioFileName;
   CString sLadChunk;

   HRESULT hr = StartRecording(sAudioFileName, sLadChunk);

   return hr;
}

HRESULT ASSISTANT::Project::StartRecording(CString &sAudioFileName, CString &sLadChunk) {
    static int startInProgress = false;

    HRESULT hr = S_OK;

    if (!avgrabber_)
        return E_FAIL;

    if (audioDeviceCount_ == 0) {
        CString csMessage;
        csMessage.LoadString(IDS_NO_AUDIO_DEVICE);
        MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
        return E_FAIL;
    }

    if (!m_bFixedTargetName)
        m_csLrdName.Empty();

    bool bIsSgOnly = IsSgOnlyDocument();

    if (!startInProgress) {
        startInProgress = true;

        // TODO create recordPath_ also conditional to m_bFixedTargetName
        if (recordPath_.IsEmpty())
            ASSISTANT::GetRecordingDirectory(recordPath_);

        if (_taccess(recordPath_, 00) == -1) {
            CreateDirectory(recordPath_, NULL);
            if (_taccess(recordPath_, 00) == -1) {
                CString csMessage;
                csMessage.Format(IDS_ERROR_CREATE_DIR, recordPath_);
                MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
                hr = E_ACCESSDENIED;
            }
        }


        // Find out the prefix of the new recording
        CString csRecordPrefix;
        if (SUCCEEDED(hr)) {
            if (!m_bFixedTargetName) {
                // Specify the prefix for the new recording
                if (IsSgOnlyDocument())
                    // If the recording is a screen recording the prefix 
                    // Recording/Aufzeichnung sould be used.
                    csRecordPrefix.LoadString(IDS_RECORDING);
                else if (GetActiveDocument())
                    // The name of the container should be used
                    csRecordPrefix = GetActiveDocument()->GetContainerName();

            } else {
                // The name for the recording is given, the 
                // recording prefix is the prefix of this file
                csRecordPrefix = m_csLrdName;
                StringManipulation::GetFilePrefix(csRecordPrefix);
            }
        }

        CString csRecordingTitle = csRecordPrefix;
        GenerateUniqueRecordingTitle(csRecordingTitle);

        // update lmd-file
        if (SUCCEEDED(hr)) {
            if (_tcslen(metadata_->GetTitle()) == 0) {
                metadata_->SetTitle(csRecordingTitle);
            }
        }

        CString csRecordFileName;
        if (SUCCEEDED(hr)) {
            hr = CreateRecordingNameFromPrefix(csRecordPrefix, csRecordFileName);
        }

        if (SUCCEEDED(hr)) {
            int mbLeft = 0;
            SystemInfo::CalculateDiskSpace(dataPath_, mbLeft);

            if ((mbLeft <= 20) ||
                (mbLeft <= 50 && avgrabber_->getEnableVideo())) {
                hr = E_OUTOFMEMORY;
                // TODO: Error message
            }
        }

        char *szLadInfo = NULL;
        if (SUCCEEDED(hr)) {
            szLadInfo = (char *)malloc(128);
            hr = CreateLadInfo(&szLadInfo);
            sLadChunk = szLadInfo;
        }

        if (SUCCEEDED(hr) && !bIsSgOnly) {
                try {
                    // if isEncrypted is true, write lad-File, else write wav-File
                    if (isEncrypted_)
                        avgrabber_->start((TCHAR *)(LPCTSTR)csRecordFileName, szLadInfo);
                    else
                        avgrabber_->start((TCHAR *)(LPCTSTR)csRecordFileName);
                }
         catch (exception &e)
                {
                hr = E_FAIL;
            MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
            }
        }

        if (szLadInfo != NULL)
            delete szLadInfo;

        if (SUCCEEDED(hr)) {
            sAudioFileName = csRecordFileName;

            recording = true;

            thumbs_.Clear(); 
            thumbs_.SetName(csRecordPrefix, dataPath_);

            if (!bIsSgOnly) {
                CString 
                    evqName,
                    objName;

                evqName = csRecordFileName;
                evqName += _T(".evq");

                objName = csRecordFileName;
                objName += _T(".obj");

                evqFp_ = _tfopen(evqName, _T("wb"));

                m_hObjFile = CreateFile(objName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (m_hObjFile == INVALID_HANDLE_VALUE) {
                    CString csMessage;
                    csMessage.Format(IDS_ERROR_FILE_OPEN, objName);
                    MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
                } else {
                    objFp_ = new CFileOutput;
                    objFp_->Init(m_hObjFile, 0, true);
                    CString line = _T("<WB orient=bottomleft>\n");
                    objFp_->Append(line);
                }

                if (GetActivePage()) {
                    int time = GetRecordingTime();
                    thumbs_.AddElement(GetActivePage(), time);
                    GetActivePage()->SaveAsAOF(time);
                }
                else {
                    int recID = recCount;
                    recCount++;
                    CString line = _T("<FILLEDRECT x=0 y=0 width=1 height=1 rgb=0xffffff></FILLEDRECT>\n");
                    objFp_->Append(line);
                    fprintf(evqFp_, "0 0 7 1 %d\n", recID);
                }
            }

            // Create lrd file
            m_csLrdName = csRecordFileName;
            m_csLrdName += _T(".lrd");
            ASSISTANT::GetDateString(startDate, 0);
            ASSISTANT::GetTimeString(startTime);

            TCHAR buf[20];
            PresentationInfo *newPresentation = new PresentationInfo();
            newPresentation->SetFilename(m_csLrdName);
            newPresentation->SetCount(presentationCount);
            unsigned int iCodepage;
            if (GetActiveDocument())
                iCodepage = GetActiveDocument()->GetCodepage();
            else
                iCodepage = GetACP();
            newPresentation->SetCodepage(iCodepage);
            newPresentation->SetTitle(csRecordingTitle);
            newPresentation->SetAuthor(metadata_->GetAuthor());
            newPresentation->SetCreator(metadata_->GetCreator());
            newPresentation->SetKeywords(metadata_->GetKeywords());
            newPresentation->SetRecorddate(startDate);
            newPresentation->SetRecordtime(startTime);
            newPresentation->SetAudioSamplerate(_itot(audioFramerate_, buf, 10));
            newPresentation->SetAudioSamplewidth(_itot(audioBitrate_, buf, 10));
            if (isVideoEnabled_)
            {
                newPresentation->SetVideoFramerate(videoFramerate_);
                newPresentation->SetVideoCodec(videoCodec_);
                if (bSupportsQuality_)
                    newPresentation->SetVideoQuality(_itot(videoQuality_, buf, 10));
                if (bSupportsKeyframes_ && useKeyframes_)
                    newPresentation->SetVideoKeyframes(_itot(keyframeDist_, buf, 10));
                if (bSupportsDatarate_ && useDatarate_)
                    newPresentation->SetVideoDatarate(_itot(videoDatarate_, buf, 10));
                newPresentation->SetVideoEnabled(true);
            }
            newPresentation->SetRecordlength(_T("??"));
            ++presentationCount;

            activePresentation_ = newPresentation;
            presentations_.InsertAt(0, newPresentation);

            SaveToRegistry();
        }

        // Bugfix 4418: Immedeately init and write a LRD file by activating the current page
        if (!bIsSgOnly)
            ActivatePage();

        startInProgress = false;
    }

    return hr;
}

int ASSISTANT::Project::PauseRecording()
{
   if (!avgrabber_)
      return false;
   
   if (!recording) return 0;
   
   paused = true;
   
   try 
   {
      avgrabber_->pause();
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), "PauseRecording", MB_OK | MB_TOPMOST);
   }
   
   return 1;
}

int ASSISTANT::Project::UnPauseRecording()
{
   if (!avgrabber_)
      return false;
   
   if (!recording) return 0;
   
   paused = false;
   
   try 
   {
      avgrabber_->unPause();
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), "UnpauseRecording", MB_OK | MB_TOPMOST);
   }

   if (GetActivePage())
       GetActivePage()->Activate();
   
   return 1;
}

HRESULT ASSISTANT::Project::StopRecording()
{
   int
      time = 0;
   int 
      progressMax;
   static int
      stopInProgress = false;
   
   if (!avgrabber_)
      return false;
   
   if (!recording) return 0;
   
   if (!stopInProgress && recording) 
   {
      if (isScreenGrabbingActive_)
      {
         this->StopSg();
      }
      // stop audio recording
      stopInProgress = true;
      time = GetRecordingTime();
      
      recording = false; 
      paused    = false;
      
      if (imageList_)
         progressMax = 3 + imageList_->GetUsedImageCount();
      else
         progressMax = 4;
      
      if (GetActivePage())
         GetActivePage()->DeActivateOnlinePointer();
      
      int nVideoSize = 0;
      if (avgrabber_)
      {
         if (isVideoEnabled_)
            nVideoSize = avgrabber_->getVideoFileSize();
         try 
         {
            avgrabber_->stop(); 
         }
         catch (exception &e)
         {
            MessageBoxA(NULL, e.what(), "StopRecording", MB_OK | MB_TOPMOST);
         }
      }
      
      bool bIsSgOnly = IsSgOnlyDocument();

      // save last page
      if (GetActivePage() && bIsSgOnly == false)
         GetActivePage()->SaveAsAOF(time, true);
      
      thumbs_.CloseThumbList(time);
      
      // close eventqueue and objectfile
      if (!bIsSgOnly)
      {
         fclose(evqFp_);
         evqFp_ = NULL;
         
         CString line = _T("</WB>\n");
         objFp_->Append(line);
         objFp_->Flush();

         CloseHandle(m_hObjFile);
         delete objFp_;
         objFp_ = NULL;
      }
      // Update entry in presentation history
      
      CLepSgml *pLepFile = NULL;
      if (activePresentation_)
      {
         // write lrd-file
         pLepFile = UpdateProjectFiles(time, true);
         
         //Move thumbs structure to presentation, after this thumbs_ only contains empty root.
         activePresentation_->MoveThumbs(&thumbs_);
         
         // set presentation record metadata
         CString lengthbuf;
         ASSISTANT::TranslateMsecToTimeString(time, lengthbuf);
         activePresentation_->SetRecordlength(lengthbuf);
         
      }
      
      SaveImages(false);
      // Write used pictures and clean data structure for new Recording
      ResetRecording();
  
      // if the video file is larger tha 1GB a warning should be displayed
      if (nVideoSize > (1000 * 1024 * 1024))
      {
         CString csMessage;
         csMessage.LoadString(IDS_ONEGB_WARNING);
         CString csCaption;
         csCaption.LoadString(IDS_WARNING);

         MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
      }
      
      SetChanged(true);
      
      stopInProgress = false;
      
      // TODO use hr more widely (currently only used for cancel detection)
      HRESULT hr = S_OK;

      // call automated post-processing
      // LEC5: always call structuring
      if(bIsSgOnly) {
          if (CStudioApp::IsLiveContextMode()) {
              HANDLE hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
              CSingleProgressDialog *pProgressDlg = new CSingleProgressDialog(AfxGetMainWnd(), hDialogInitEvent);
              
              WaitForClickInfoStruct threadStruct;
              threadStruct.pAvGrabber = avgrabber_;
              threadStruct.pProgress = pProgressDlg;
              threadStruct.hProgressEvent = hDialogInitEvent;

              CWinThread *thread = AfxBeginThread(WaitClickElementInfoThread, &threadStruct);

              if (pProgressDlg != NULL) {
                  pProgressDlg->DoModal();
              }

              CloseHandle(hDialogInitEvent);

              if (pProgressDlg != NULL)
                  delete pProgressDlg;
              pProgressDlg = NULL;
          }


         // currently only pure screen grabbing is supported
         // TODO: error handling
         hr = CScreengrabbingExtendedExternal::RunPostProcessing(m_csLrdName, true, CStudioApp::IsLiveContextMode(), pLepFile);
         if (pLepFile) {
             delete pLepFile;
             pLepFile = NULL;
         }

         if (hr == E_ABORT && CStudioApp::IsLiveContextMode()) {
             CStudioApp::QuitLiveContextWithCode(EXIT_CODE_LC_NO_RECORDING_DONE);
         }

         if(activePresentation_) {
            if( hr == S_OK )
               // structuring successful
               // TODO: handle "Continue later"
               activePresentation_->SetClipSearchable(true);
            else
               // structuring failed or canceled
               activePresentation_->SetDenverDocument(true);
         }
      }

      if(activePresentation_)
         activePresentation_ = NULL;

      return hr;
   }
   
   return E_FAIL;
}

UINT ASSISTANT::Project::StartDefineAreaThread(LPVOID pParam)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();
   //ASSISTANT::Project::active->isSgThreadRunning_ = true;
   bool bIsSgOnly = ASSISTANT::Project::active->GetActiveDocument() == 0;
   try 
   {
      if (avgrabber_->chooseSgCaptureArea(bIsSgOnly))
      {
			avgrabber_->GetScreenCapturer()->SetIsCustomSet(true);
		}
	}
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
      Sleep(100);
   }
   
   Sleep(100);
   //ASSISTANT::Project::active->isSgThreadRunning_ = false;
	return 0;
}

HRESULT ASSISTANT::Project::StartDefineArea()
{
   if (!avgrabber_ || !doWithSG_)
      return E_FAIL;
   
   if (isScreenGrabbingActive_)
      return E_FAIL;
   
   if (isSgThreadRunning_)
      return E_FAIL;
   
   HRESULT hr = S_OK;

   CWinThread *thread = AfxBeginThread(StartDefineAreaThread, (LPVOID&) avgrabber_);

	return hr;
}

HRESULT ASSISTANT::Project::StartSg(CRect &rcSelection)
{
   if (!avgrabber_ || !doWithSG_)
      return E_FAIL;
   
   if (isScreenGrabbingActive_)
      return E_FAIL;
   
   if (isSgThreadRunning_)
      return E_FAIL;
   
   HRESULT hr = S_OK;

   CString sAudioFileName;
   CString sLadChunk;

   bool bIsSgOnly = IsSgOnlyDocument();
   if (bIsSgOnly)
   {
      avgrabber_->setEnableVideo(false);
      
      hr = StartRecording(sAudioFileName, sLadChunk);
   }
   
   
   if (SUCCEEDED(hr))
   {
      isAnnotationMode_ = false;
   
      static CString clipPath;
   
      if (activePresentation_)
      {
         int time = GetRecordingTime();
         // Bugfix 4418: In case of pure Screengrabbing "GetRecordingTime()" returns 
         // no meaningful value. Until no better solution is found then the initial time 
         // written to LRD/LMD file is "0" (to have a valid time if the Assistant crashes).
         if (bIsSgOnly)
            time = 0L;
         LPCTSTR clipName = activePresentation_->GetClipList()->AddClip(projectPrefix_, time);

         SaveImages();
         UpdateProjectFiles(time);
      
         clipPath = dataPath_;
         clipPath += _T("\\");
         clipPath += clipName;
      
         static SGInfo sgInfo;
         sgInfo.avgrabber = avgrabber_;
         sgInfo.clipName = clipPath;

         try 
         {
             avgrabber_->setSgCaptureRect(&rcSelection);
             if (bIsSgOnly)
             {
                 int iLength = sLadChunk.GetLength();
                 char szLadInfo[130];
#ifdef _UNICODE
                 ::WideCharToMultiByte(CP_ACP, 0, sLadChunk, iLength, szLadInfo, 130, NULL, NULL);
#else
                 strncpy(szLadInfo, sLadChunk);
#endif
                 szLadInfo[iLength] = 0;

                 avgrabber_->startScreenGrabbingClip((TCHAR *)(LPCTSTR)clipPath, true, (TCHAR *)(LPCTSTR)sAudioFileName,  szLadInfo);
             }
             else
                 avgrabber_->startScreenGrabbingClip((TCHAR *)(LPCTSTR)clipPath, false);
             isScreenGrabbingActive_ = true;
         }
         catch (exception &e)
         {
             MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
         }
      }
      else
      {
      }
   }
   
   EscHotkey(true);
   return hr;
}


HRESULT ASSISTANT::Project::StopSg()
{
   int timeStamp = -1;
   int clipLength = -1;
   int droppedFrames = 0;
   int totalFrames = 0;
   bool isSgEmpty = false;
   
   
   if (!avgrabber_ || !recording || !doWithSG_)
      return 0;
   
   if (!isScreenGrabbingActive_)
   {
      if (isSgThreadRunning_)
      {
         return E_FAIL;
      }
   }
   
   try 
   {
      avgrabber_->stopScreenGrabbingClip();
      isScreenGrabbingActive_ = false;
   } 
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
      return E_FAIL;
   }
   
   try 
   {
      timeStamp = avgrabber_->getScreenGrabbingClipStartTime();
      if (timeStamp > 0)
        clipLength = avgrabber_->getSgClipLengthMs();
      else if (IsSgOnlyDocument()) // BUGFIX BLECS-962 
      {
          // 0 or negative value returned from avgrabber_->getScreenGrabbingClipStartTime();
          // negative value caused by system load
          // in this case  clip length will br -1
          // no structure is created and no replay is possible
          int iRecLength = avgrabber_->getRecordingTime();
          int iClipLength = avgrabber_->getSgClipLengthMs();
          // check if actually a recording was made and a clip was created
          if (iRecLength > 0 && iClipLength > 0)
          {
              // set audio/video offset to 1 because MediaSegment::IntersectsRange doesn't support negative offset
              // set length to clip's length
              timeStamp = 1;
              clipLength = iClipLength;
              // structure will be created and replay works
          }
      }
   } 
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
      return E_FAIL;
   }
   
   // screen grabbing started and stopped in the same pause
   if (clipLength == 0)
   {
      SgClip *sgClip = activePresentation_->GetClipList()->GetActualClip();
      activePresentation_->GetClipList()->RemoveClip(sgClip);
      isSgEmpty = true;
   }
   
   if (reportDroppedFrames_)
   {
      droppedFrames = avgrabber_->getSgDroppedFramesCount();
      totalFrames = avgrabber_->getSgTotalFramesCount();
   }
   
   if (!isSgEmpty && activePresentation_)
   {
      SgClip *sgClip = activePresentation_->GetClipList()->GetActualClip();
      if (sgClip)
      {
         sgClip->SetStartTime(timeStamp);
         sgClip->SetLength(clipLength);
      }
   }

   // TODO use hr more widely (currently only used for cancel detection)
   HRESULT hr = S_OK;
   
   bool bIsSgOnly = IsSgOnlyDocument();
   if (bIsSgOnly)
   {
      hr = StopRecording();
      avgrabber_->setEnableVideo(isVideoEnabled_);
   }
   
   if (!isSgEmpty && reportDroppedFrames_)
   {
      /*/ TODO
      if (sgModus_)
         sgModus_->ReportDroppedFrames(droppedFrames, totalFrames);
      /*/
   }
   
   EscHotkey(false);
   
   return hr;
}

void ASSISTANT::Project::RetrieveSgCodecs()
{
   if (!avgrabber_)
      return;
   
   DeleteSgCodecInfo();
   
   try 
   {
      sgCodecCount_ = avgrabber_->getSgCodecCount();
      isSgCodecEnabled_ = true;
      if (sgCodecCount_ > 0)
      {
         CreateSgCodecInfo();
         SetSgCodec();
      } 
      else 
      {
         isSgCodecEnabled_ = false;
      }
   }
   catch (exception &e)
   {
      DeleteSgCodecInfo();
      isSgCodecEnabled_ = false;
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
   }
   
}

void ASSISTANT::Project::CreateSgCodecInfo()
{
   
   if (sgCodecCount_ > 0)
   {
      sgCodecInfo_ = new CodecInfo*[sgCodecCount_];
      for (int i = 0; i < sgCodecCount_; ++i)
         sgCodecInfo_[i] = new CodecInfo();
      avgrabber_->getSgCodecs(sgCodecInfo_);
   }
   
}

void ASSISTANT::Project::DeleteSgCodecInfo()
{
   
   if (sgCodecInfo_)
   {
      for (int i = 0; i < sgCodecCount_; ++i)
         delete sgCodecInfo_[i];
      delete[] sgCodecInfo_;
   }
   
   sgCodecInfo_  = NULL;
   sgCodecCount_ = 0;
   sgCodecNumber_ = -1;
   
}


void ASSISTANT::Project::SetSgCodecNumber(bool bDisplayWarning)
{
   if (sgCodecCount_ > 0)
      sgCodecNumber_ = 0;
   else
      sgCodecNumber_ = -1;
   
   for (int i = 0; i < sgCodecCount_; ++i)
   {
      if (sgCodec_ == sgCodecInfo_[i]->fccString)
      {
         sgCodecNumber_ = i;
         break;
      }
   }
   
   if (bDisplayWarning && sgCodecNumber_ == 0)
   {
      CString csMessage;
      csMessage.LoadString(IDS_UNCOMPRESSED_SG_CODEC);
      CString csCaption;
      csCaption.LoadString(IDS_WARNING);
      MessageBox(NULL, csMessage, csCaption, MB_OK | MB_TOPMOST);
   }
   
}

void ASSISTANT::Project::SetSgCodec(bool bDisplayWarning)
{
   
   if (!avgrabber_)
      return;
   
   SetSgCodecNumber(bDisplayWarning);
   
   if (sgCodecCount_ <= 0 || sgCodecNumber_ < 0)
   {
      isSgCodecEnabled_     = false;
      hasSgAboutDialog_     = false;
      hasSgConfigureDialog_ = false;
      bSgSupportsQuality_   = false;
      bSgSupportsKeyframes_ = false;
      bSgSupportsDatarate_  = false;
      return;
   }
   
   sgCodec_ = sgCodecInfo_[sgCodecNumber_]->fccString;
   
   try 
   {
      isSgCodecEnabled_ = true;
      bSgSupportsKeyframes_ = sgCodecInfo_[sgCodecNumber_]->bSupportsKeyframes;
      if (bSgSupportsKeyframes_)
      {
         sgCodecInfo_[sgCodecNumber_]->bUseKeyframes = sgUseKeyframes_;
         sgCodecInfo_[sgCodecNumber_]->iKeyframeRate = sgKeyframeDist_;
      }
      
      bSgSupportsDatarate_ = sgCodecInfo_[sgCodecNumber_]->bSupportsDatarate;
      if (bSgSupportsDatarate_)
      {
         sgCodecInfo_[sgCodecNumber_]->bUseDatarate = sgUseDatarate_;
         sgCodecInfo_[sgCodecNumber_]->iDatarate    = sgDatarate_;
      }
      
      bSgSupportsQuality_ = sgCodecInfo_[sgCodecNumber_]->bSupportsQuality;
      if (bSgSupportsQuality_)
      {
         sgCodecInfo_[sgCodecNumber_]->iQuality = sgQuality_;
      }
      
      avgrabber_->setScreenGrabberCodec(sgCodecInfo_[sgCodecNumber_]);
      
      hasSgAboutDialog_     = sgCodecInfo_[sgCodecNumber_]->bHasInfoDialog;
      hasSgConfigureDialog_ = sgCodecInfo_[sgCodecNumber_]->bHasConfigDialog;
   }
   catch (exception &e) 
   {
      isSgCodecEnabled_ = false;
      MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
   }
   
   
   return;
}


int ASSISTANT::Project::SetAnnotationMode(UINT nCurrentTool, double dLineWidth, int iLineStyle, int iArrowStyle,
                                          COLORREF clrLine, COLORREF clrFill, bool bIsFilled, COLORREF clrText)
{
   if (!avgrabber_)
      return 0;
   
   // Only important if we're doing some
   // screen grabbing at the moment: Pass on the tool
   // to the AvGrabber's Screengrabbing module.
   if (!recording || !isScreenGrabbingActive_) 
      return 0;
   
   
   if (!isAnnotationMode_)
   {
      isAnnotationMode_ = true;
      avgrabber_->setSgAnnotationMode();
   }

   if ((nCurrentTool == ID_TOOL_PENRED) || (nCurrentTool == ID_TOOL_PENGREEN) ||
      (nCurrentTool == ID_TOOL_PENBLUE) || (nCurrentTool == ID_TOOL_PENWHITE) ||
      (nCurrentTool == ID_TOOL_PENCYAN) || (nCurrentTool == ID_TOOL_PENMAGENTA) ||
      (nCurrentTool == ID_TOOL_PENBLACK) || (nCurrentTool == ID_TOOL_PENYELLOW) )
   {
      int penWidth = 2;
      avgrabber_->setSgAnnotationLineWidth(penWidth);
      avgrabber_->setSgAnnotationTool(GenericObject::FREEHAND);
      if (nCurrentTool == ID_TOOL_PENRED)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(255, 255, 0, 0));
      else if (nCurrentTool == ID_TOOL_PENGREEN)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(255, 0, 255, 0));
      else if (nCurrentTool == ID_TOOL_PENBLUE)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(255, 0, 0, 255));
      else if (nCurrentTool == ID_TOOL_PENWHITE)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(255, 255, 255, 255));
      else if (nCurrentTool == ID_TOOL_PENCYAN)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(255, 0, 255, 255));
      else if (nCurrentTool == ID_TOOL_PENMAGENTA)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(255, 255, 0, 255));
      else if (nCurrentTool == ID_TOOL_PENBLACK)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(255, 0, 0, 0));
      else if (nCurrentTool == ID_TOOL_PENYELLOW)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(255, 255, 255, 0));
   }
   else if ((nCurrentTool == ID_TOOL_MARKERYELLOW) || (nCurrentTool == ID_TOOL_MARKERRED) ||
            (nCurrentTool == ID_TOOL_MARKERGREEN) || (nCurrentTool == ID_TOOL_MARKERMAGENTA) ||
            (nCurrentTool == ID_TOOL_MARKERBLUE) || (nCurrentTool == ID_TOOL_MARKERORANGE))
   {
      int markerWidth = 20;
      avgrabber_->setSgAnnotationLineWidth(markerWidth);
      avgrabber_->setSgAnnotationTool(GenericObject::MARKER);
      if (nCurrentTool == ID_TOOL_MARKERYELLOW)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(127, 255, 255, 0));
      else if (nCurrentTool == ID_TOOL_MARKERRED)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(127, 255, 0, 0));
      else if (nCurrentTool == ID_TOOL_MARKERGREEN)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(127, 0, 255, 0));
      else if (nCurrentTool == ID_TOOL_MARKERMAGENTA)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(127, 255, 0, 255));
      else if (nCurrentTool == ID_TOOL_MARKERBLUE)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(127, 0, 255, 255));
      else if (nCurrentTool == ID_TOOL_MARKERORANGE)
         avgrabber_->setSgAnnotationPenColor(Gdiplus::Color::MakeARGB(127, 255, 128, 0));
   }
   else
   {
      if (nCurrentTool == ID_TOOL_TEXT)
      {
          Gdiplus::ARGB argbClrText = 
              Gdiplus::Color::MakeARGB(255, GetRValue(clrText), GetGValue(clrText), GetBValue(clrText));
         avgrabber_->setSgAnnotationPenColor(argbClrText);
         avgrabber_->setSgAnnotationFont(&m_logFont);
         avgrabber_->setSgAnnotationTool(GenericObject::ASCIITEXT);
      }
      else if (nCurrentTool == ID_TOOL_TELEPOINTER)
      {
         avgrabber_->setSgAnnotationTool(GenericObject::TELEPOINTER);
      }
      else 
      { 
         Gdiplus::ARGB argbClrLine = 
             Gdiplus::Color::MakeARGB(255, GetRValue(clrLine), GetGValue(clrLine), GetBValue(clrLine));
         Gdiplus::ARGB argbClrFill = 
             Gdiplus::Color::MakeARGB(255, GetRValue(clrFill), GetGValue(clrFill), GetBValue(clrFill));
         if (nCurrentTool == ID_TOOL_FREEHAND)
         {
            avgrabber_->setSgAnnotationLineWidth(dLineWidth);
            avgrabber_->setSgAnnotationPenColor(argbClrLine);
            avgrabber_->setSgAnnotationTool(GenericObject::FREEHAND);
         }
         else if (nCurrentTool == ID_TOOL_LINE)
         {
            CString csArrowStyle;
            int iArrowConfig;
            ASSISTANT::GetArrowConfig(iArrowStyle, iArrowConfig, csArrowStyle);
            avgrabber_->setSgAnnotationArrowConfig(iArrowConfig);
            avgrabber_->setSgAnnotationArrowStyle(iArrowStyle);
            avgrabber_->setSgAnnotationLineWidth(dLineWidth);
            avgrabber_->setSgAnnotationPenColor(argbClrLine);
            avgrabber_->setSgAnnotationLineStyle(iLineStyle);
            avgrabber_->setSgAnnotationTool(GenericObject::LINE);
         }
         else if (nCurrentTool == ID_TOOL_RECTANGLE)
         {
            avgrabber_->setSgAnnotationLineWidth(dLineWidth);
            avgrabber_->setSgAnnotationPenColor(argbClrLine);
            if (bIsFilled)
               avgrabber_->setSgAnnotationFillColor(argbClrFill);
            avgrabber_->setSgAnnotationIsFilled(bIsFilled);
            avgrabber_->setSgAnnotationLineStyle(iLineStyle);
            avgrabber_->setSgAnnotationTool(GenericObject::RECTANGLE);
         }
         else if (nCurrentTool == ID_TOOL_OVAL)
         {
            avgrabber_->setSgAnnotationLineWidth(dLineWidth);
            avgrabber_->setSgAnnotationPenColor(argbClrLine);
            if (bIsFilled)
               avgrabber_->setSgAnnotationFillColor(argbClrFill);
            avgrabber_->setSgAnnotationIsFilled(bIsFilled);
            avgrabber_->setSgAnnotationLineStyle(iLineStyle);
            avgrabber_->setSgAnnotationTool(GenericObject::OVAL);
         }
         else if (nCurrentTool == ID_TOOL_POLYLINE)
         {
            CString csArrowStyle;
            int iArrowConfig;
            ASSISTANT::GetArrowConfig(iArrowStyle, iArrowConfig, csArrowStyle);
            avgrabber_->setSgAnnotationArrowConfig(iArrowConfig);
            avgrabber_->setSgAnnotationArrowStyle(iArrowStyle);
            avgrabber_->setSgAnnotationLineWidth(dLineWidth);
            avgrabber_->setSgAnnotationPenColor(argbClrLine);
            avgrabber_->setSgAnnotationIsFilled(false);
            avgrabber_->setSgAnnotationLineStyle(iLineStyle);
            avgrabber_->setSgAnnotationIsClosed(false);
            avgrabber_->setSgAnnotationTool(GenericObject::POLYGON);
         }
         else if (nCurrentTool == ID_TOOL_POLYGON)
         {
            avgrabber_->setSgAnnotationLineWidth(dLineWidth);
            avgrabber_->setSgAnnotationPenColor(argbClrLine);
            if (bIsFilled)
               avgrabber_->setSgAnnotationFillColor(argbClrFill);
            avgrabber_->setSgAnnotationIsFilled(bIsFilled);
            avgrabber_->setSgAnnotationLineStyle(iLineStyle);
            avgrabber_->setSgAnnotationIsClosed(true);
            avgrabber_->setSgAnnotationTool(GenericObject::POLYGON);
         }
      }
   }
   
   return 1;
}

int ASSISTANT::Project::ResetAnnotationMode()
{
   if (!avgrabber_)
      return 0;
   
   // Only important if we're doing some
   // screen grabbing at the moment: Pass on the tool
   // to the AvGrabber's Screengrabbing module.
   if (!recording || !isScreenGrabbingActive_) 
      return 0;
   
   
   isAnnotationMode_ = false;
   avgrabber_->setSgInteractionMode();

   return 1;
}

CString &ASSISTANT::Project::GetRecordPath()
{
   return recordPath_;
}

bool ASSISTANT::Project::SetRecordPath(CString &path)
{  
   if (_taccess(path, 06) != 0)
   {
      CString csMessage;
      csMessage.Format(IDS_PERMISSION_DENIED, path);
      MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      return false;
   }
   
   if (!ASSISTANT::TryNextDataPath(path, projectPrefix_))
   {
      CString csMessage;
      csMessage.Format(IDS_PERMISSION_DENIED, path);
      MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      return false;
   }
   
   recordPath_ = path;
   if (recordPath_[recordPath_.GetLength() -1] == _T('\\'))
      recordPath_.Delete(recordPath_.GetLength()-1, 1);
   
   SetChanged(true);
   
   return true;
}

CString &ASSISTANT::Project::GetDataPath()
{
   return dataPath_;
}

CString &ASSISTANT::Project::GetPrefix()
{
   return projectPrefix_;
}

void ASSISTANT::Project::CheckDiskSpace(bool bReset)
{
   static int
      lastRecordingTime = 0,
      lastColor = 0;
   static int
      minLeft = 0;
   static int 
      countdown = -1;
   static bool
      warningActiv = false;
   static int i2GBWarningLevel = 0;
   static int i2GBCountdown = -1;
   
   if (bReset)
   {
      lastRecordingTime = 0;
      lastColor = 0;
      i2GBWarningLevel = 0;
      i2GBCountdown = -1;
      
      HideTwoGBCountdown();
      HideTwoGBWarning();
      HideDiscSpaceCountdown();
      HideDiscSpaceWarning();
      return;
   }

   try 
   {
      int recordingTime = avgrabber_->getRecordingTime();
      if (recordingTime < lastRecordingTime)
      {
         lastRecordingTime = 0;
         lastColor = 0;
      }

      if ((lastRecordingTime == 0) || (recordingTime - lastRecordingTime) > 5000)
      {
         minLeft = 0;
         int mbLeft = 0;
         SystemInfo::CalculateDiskSpace(ASSISTANT::Project::active->GetRecordPath(), mbLeft);
         int datarate = avgrabber_->getEstimatedDataRate();
         mbLeft = mbLeft - 10;
         if (mbLeft < 0)
            mbLeft = 0;
         minLeft = (int)(((double)mbLeft * (1024.0*1024.0)) / ((double)datarate * 60));

         int nVideoDatarate = avgrabber_->getVideoAverageDataRate();

         if (nVideoDatarate > 0)
         {
            int nVideoSize = avgrabber_->getVideoFileSize();
            // shouldn't be larger than this:
            int nMaxSize = MAX_AVI_FILE_SIZE; // for testing: 100 MB
            int nVideoMBLeft = (nMaxSize - nVideoSize) / (1024*1024);

            int nVideoSecsLeft = (nMaxSize - nVideoSize) / nVideoDatarate;

            if (i2GBWarningLevel <= 2 && nVideoSecsLeft <= 60)
            {
               ShowTwoGBCountdown(nVideoSecsLeft);
               i2GBCountdown = nVideoSecsLeft > 0 ? nVideoSecsLeft : 1;
               i2GBWarningLevel = 3;
            }
            else if (i2GBWarningLevel <= 1 && nVideoSecsLeft <= 300)
            {
               ShowTwoGBWarning(5);
               i2GBWarningLevel = 2;
            }
            else if (i2GBWarningLevel == 0 && nVideoSecsLeft <= 600)
            {
               ShowTwoGBWarning(10);
               i2GBWarningLevel = 1;
            }
         }
         
         lastRecordingTime = recordingTime;
         if (minLeft <= 1)
         {
            if (countdown == -1)
            {
               int secondsLeft = (int)(((double)(mbLeft) * (1024.0*1024.0)) / ((double)datarate));
               countdown = 60;
               if (countdown > secondsLeft)
                  countdown = secondsLeft;
               ShowDiscSpaceCountdown(countdown);
            }
         }
         else if (minLeft <= 5)
         {
            if (!warningActiv)
            {
               ShowDiscSpaceWarning(5);
               warningActiv = true;
            }
            if (countdown != -1)
            {
               HideDiscSpaceCountdown();
               countdown = -1;
            }
         }
         else
         {
            if (countdown)
               HideDiscSpaceCountdown();
            if (warningActiv)
               HideDiscSpaceWarning();
            countdown = -1;
            warningActiv = false;
         }
      }
      
      CString color;
      if (minLeft < 10)
      {
         if (lastColor == 1)
         {
            lastColor = 0;
            color = _T("#000000"); 
         }
         else 
         {
            lastColor = 1;
            color = _T("#ff0000");
         }
      }
      else 
      {
         lastColor = 0;
         color = _T("#000000");
      }
      

      if (i2GBCountdown != -1 || countdown != -1)
      {
         if (countdown != -1 && countdown > 0)
            --countdown;
         if (i2GBCountdown != -1 && i2GBCountdown > 0)
            --i2GBCountdown;

         if (i2GBCountdown == 0 || countdown == 0)
         {
            HideTwoGBCountdown();
            HideTwoGBWarning();
            HideDiscSpaceCountdown();
            HideDiscSpaceWarning();
            CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
            if (pMainFrame)
            {
               SendMessage(pMainFrame->m_hWnd, WM_COMMAND, ID_STOP_RECORDING, NULL);
            }
            countdown = -1;
            i2GBCountdown = -1;
            i2GBWarningLevel = 0;
            warningActiv = false;
            lastRecordingTime = 0;
            return;
         }
         else
         {
            if (countdown != -1)
               ShowDiscSpaceCountdown(countdown);
              
            if (i2GBCountdown != -1)
               ShowTwoGBCountdown(i2GBCountdown);
         }
      }
   }
   catch (exception &e)
   {
      MessageBoxA(NULL, e.what(), "CheckDiskSpace", MB_OK | MB_TOPMOST);
   }
   
}

void ASSISTANT::Project::ShowTwoGBCountdown(int iSecondsLeft)
{
   if (m_dlgTwoGBCountdown.m_hWnd == NULL)
      m_dlgTwoGBCountdown.Create(CTwoGBCountdown::IDD);
   
   if (!m_dlgTwoGBCountdown.IsWindowVisible())
      m_dlgTwoGBCountdown.ShowWindow(SW_SHOWNOACTIVATE);

   m_dlgTwoGBCountdown.Update(iSecondsLeft);         
}

void ASSISTANT::Project::HideTwoGBCountdown()
{
   if (m_dlgTwoGBCountdown.m_hWnd == NULL)
      return;
   
   if (m_dlgTwoGBCountdown.IsWindowVisible())
      m_dlgTwoGBCountdown.ShowWindow(SW_HIDE);
}

void ASSISTANT::Project::ShowTwoGBWarning(int iMinutesLeft)
{
   if (m_dlgTwoGBWarning.m_hWnd == NULL)
      m_dlgTwoGBWarning.Create(CTwoGBWarning::IDD);
   
   if (!m_dlgTwoGBWarning.IsWindowVisible())
      m_dlgTwoGBWarning.ShowWindow(SW_SHOW);
   
   m_dlgTwoGBWarning.Update(iMinutesLeft);
   
}

void ASSISTANT::Project::HideTwoGBWarning()
{
   if (m_dlgTwoGBCountdown.m_hWnd == NULL)
      return;
   
   if (m_dlgTwoGBCountdown.IsWindowVisible())
      m_dlgTwoGBCountdown.ShowWindow(SW_HIDE);
}

void ASSISTANT::Project::ShowDiscSpaceCountdown(int iSecondsLeft)
{
   if (m_dlgDiscSpaceCountdown.m_hWnd == NULL)
      m_dlgDiscSpaceCountdown.Create(CDiscSpaceCountdown::IDD);
   
   if (!m_dlgDiscSpaceCountdown.IsWindowVisible())
      m_dlgDiscSpaceCountdown.ShowWindow(SW_SHOW);
   
   CString csRecordPath = GetRecordPath();
   ASSISTANT::GetDrive(csRecordPath);
   m_dlgDiscSpaceCountdown.Update(csRecordPath, iSecondsLeft);
   
}

void ASSISTANT::Project::HideDiscSpaceCountdown()
{
   if (m_dlgDiscSpaceCountdown.m_hWnd == NULL)
      return;
   
   if (m_dlgDiscSpaceCountdown.IsWindowVisible())
      m_dlgDiscSpaceCountdown.ShowWindow(SW_HIDE);
      
}

void ASSISTANT::Project::ShowDiscSpaceWarning(int iMinutesLeft)
{
   if (m_dlgDiscSpaceWarning.m_hWnd == NULL)
      m_dlgDiscSpaceWarning.Create(CDiscSpaceWarning::IDD);
   
   if (!m_dlgDiscSpaceWarning.IsWindowVisible())
      m_dlgDiscSpaceWarning.ShowWindow(SW_SHOW);
   
   CString csRecordPath = GetRecordPath();
   ASSISTANT::GetDrive(csRecordPath);
   m_dlgDiscSpaceWarning.Update(csRecordPath, iMinutesLeft);
   
}

void ASSISTANT::Project::HideDiscSpaceWarning()
{
   if (m_dlgDiscSpaceWarning.m_hWnd == NULL)
      return;
   
   if (m_dlgDiscSpaceWarning.IsWindowVisible())
      m_dlgDiscSpaceWarning.ShowWindow(SW_HIDE);
}

void ASSISTANT::Project::SetNewActiveDocument(UINT nIndex)
{
    if (documents_.GetSize() > 0) {
        activeDocument_ = nIndex;
        if (activeDocument_ >= documents_.GetSize())
            activeDocument_ = documents_.GetSize() - 1;
    }
    else
        activeDocument_ = -1;
}

// TODO: The creation of the lmd file should be made in CLmdSgml (analog Lep file)
void ASSISTANT::Project::CreateLmdFile(int _time) {  
    CString csLrdPrefix = m_csLrdName;
    StringManipulation::GetPathFilePrefix(csLrdPrefix);
    CString csLmdName = csLrdPrefix + _T(".lmd");
   
   HANDLE hLmdFile = CreateFile(csLmdName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hLmdFile == INVALID_HANDLE_VALUE)
   {
      CString csMessage;
      csMessage.Format(IDS_ERROR_FILE_OPEN, csLmdName);
      MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      return;
   }

   CFileOutput lmdOutput;
   lmdOutput.Init(hLmdFile, 0, true);
   
   CString csLine;
   CString csSgmlLine;

   csLine = _T("<docinfo>\n");
   lmdOutput.Append(csLine);

   csLine = _T("  <metainfo>\n");
   lmdOutput.Append(csLine);

   if (activePresentation_ != NULL)
       StringManipulation::TransformForSgml(activePresentation_->GetTitle(), csSgmlLine);
   else
       StringManipulation::TransformForSgml(metadata_->GetTitle(), csSgmlLine);
   csLine.Format(_T("    <title>%s</title>\n"), csSgmlLine);
   lmdOutput.Append(csLine);
   
   csLine = metadata_->GetAuthor();
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   csLine.Format(_T("    <author>%s</author>\n"), csSgmlLine);
   lmdOutput.Append(csLine);
   
   csLine = metadata_->GetCreator();
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   csLine.Format(_T("    <creator>%s</creator>\n"), csSgmlLine);
   lmdOutput.Append(csLine);

   time_t ldate;
   time(&ldate);    
   struct tm *today = localtime( &ldate );
   
   CString csDate;
   csLine.Format(_T("    <recorddate>%02d.%02d.%04d</recorddate>\n"), 
                 today->tm_mday, today->tm_mon+1, 1900+today->tm_year);
   lmdOutput.Append(csLine);
   
   
   // Keywords in list are not SGML strings; they have to be
   // converted like everyone else (with the TransformForSgml function)
   CString csKeywords = metadata_->GetKeywords();
   CStringArray keywordList;
   ASSISTANT::CreateListFromString(csKeywords, keywordList);
   for (int i = 0; i < keywordList.GetSize(); ++i)
   {
      csLine = keywordList[i];
      StringManipulation::TransformForSgml(csLine, csSgmlLine);
      csLine.Format(_T("    <keyword>%s</keyword>\n"), csSgmlLine);
      lmdOutput.Append(csLine);
   }
   keywordList.RemoveAll();
   
   csLine = _T("  </metainfo>\n");
   lmdOutput.Append(csLine);

   csLine = _T("  <structure>\n");
   lmdOutput.Append(csLine);
   
   bool bIsSgOnly = IsSgOnlyDocument();
   if (!bIsSgOnly)
   {
      if (activePresentation_)
         thumbs_.WriteRootEntry(&lmdOutput, activePresentation_->GetClipList());
      else
         thumbs_.WriteRootEntry(&lmdOutput, NULL);
   }
   else
   {
      if (activePresentation_)
         activePresentation_->GetClipList()->WriteLmdEntry(&lmdOutput, 0, _time, 0);
   }
   
   csLine = _T("  </structure>\n");
   lmdOutput.Append(csLine);
   
   csLine = _T("  <fileinfo>\n");
   lmdOutput.Append(csLine);

   csLine = _T("    <audio>\n");
   lmdOutput.Append(csLine);

   csLine.Format(_T("      <samplerate>%d</samplerate>\n"), audioFramerate_);
   lmdOutput.Append(csLine);

   csLine.Format(_T("      <bitrate>%d</bitrate>\n"), audioBitrate_);
   lmdOutput.Append(csLine);

   csLine = _T("    </audio>\n");
   lmdOutput.Append(csLine);
   
   csLine = _T("    <video>\n");
   lmdOutput.Append(csLine);
   
   if (isVideoEnabled_ && !bIsSgOnly)
   {
      csLine.Format(_T("      <framerate>%f</framerate>\n"), videoFramerate_);
      lmdOutput.Append(csLine);
      
      csLine.Format(_T("      <codec>%s</codec>\n"), videoCodec_);
      lmdOutput.Append(csLine);
      
      if (bSupportsQuality_)
      {
         csLine.Format(_T("      <quality>%d</quality>\n"), videoQuality_);
         lmdOutput.Append(csLine);
      }

      if (useKeyframes_)
      {
         csLine.Format(_T("      <keyframes>%d</keyframes>\n"), keyframeDist_);
         lmdOutput.Append(csLine);
      }

      if (useDatarate_)
      {
         csLine.Format(_T("      <datarate>%d</datarate>\n"), videoDatarate_);
         lmdOutput.Append(csLine);
      }
   }
   
   csLine = _T("    </video>\n");
   lmdOutput.Append(csLine);

   csLine = _T("  </fileinfo>\n");
   lmdOutput.Append(csLine);
   
   csLine = _T("</docinfo>\n");
   lmdOutput.Append(csLine);
   
   lmdOutput.Flush();
   CloseHandle(hLmdFile);
}

// TODO: The creation of the lrd file should be made in CLrdSgml (analog Lep file)
void ASSISTANT::Project::CreateLrdFile(int iSlideWidth, int iSlideHeight, 
                                      CMetaInformation &metaInfo, CString csSgmlPrefix) {

    HANDLE hLrdFile = CreateFile(m_csLrdName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hLrdFile == INVALID_HANDLE_VALUE) {
        CString csMessage;
        csMessage.Format(IDS_ERROR_FILE_OPEN, m_csLrdName);
        MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
        return;
    }

    CFileOutput lrdOutput;
    lrdOutput.Init(hLrdFile, 0, true);

    CString csLine;

    csLine = _T("<DOCUMENT>\n");
    lrdOutput.Append(csLine);

    // Metadata

    csLine = _T("  <INFO>\n");
    lrdOutput.Append(csLine);

    csLine.Format(_T("    <CODEPAGE>utf-8</CODEPAGE>\n"));
    lrdOutput.Append(csLine);
    csLine.Format(_T("    <VERSION>1.1</VERSION>\n"));
    lrdOutput.Append(csLine);   
    csLine.Format(_T("    <TITLE>%s</TITLE>\n"), metaInfo.GetTitle());
    lrdOutput.Append(csLine);
    csLine.Format(_T("    <AUTHOR>%s</AUTHOR>\n"), metaInfo.GetAuthor());
    lrdOutput.Append(csLine);
    csLine.Format(_T("    <COMMENTS></COMMENTS>\n"));
    lrdOutput.Append(csLine);
    csLine.Format(_T("    <LENGTH>%s</LENGTH>\n"), metaInfo.GetLength());
    lrdOutput.Append(csLine);
    csLine.Format(_T("    <RECORDDATE>%s - %s</RECORDDATE>\n"), startDate, startTime);
    lrdOutput.Append(csLine);

    bool bIsSgOnlyDocument = IsSgOnlyDocument();
    if (bIsSgOnlyDocument) {
        csLine.Format(_T("    <TYPE>denver</TYPE>\n"));
        lrdOutput.Append(csLine);
    }

    csLine = _T("  </INFO>\n");
    lrdOutput.Append(csLine);

    // Files and Helper
    csLine = _T("  <FILES>\n");
    lrdOutput.Append(csLine);

    if (isEncrypted_)
        csLine.Format(_T("    <AUDIO>%s.lad</AUDIO>\n"), csSgmlPrefix);
    else
        csLine.Format(_T("    <AUDIO>%s.wav</AUDIO>\n"), csSgmlPrefix);
    lrdOutput.Append(csLine);

    csLine = _T("    <HELPERS>\n");
    lrdOutput.Append(csLine);

    if (!bIsSgOnlyDocument) {
        csLine = _T("      <H>\n");
        lrdOutput.Append(csLine);
        csLine.Format(_T("        <OPT>wbPlay</OPT>\n"));
        lrdOutput.Append(csLine);
        csLine.Format(_T("        <OPT>-G</OPT>\n"));
        lrdOutput.Append(csLine);
        csLine.Format(_T("        <OPT>%dx%d</OPT>\n"), iSlideWidth, iSlideHeight);
        lrdOutput.Append(csLine);
        csLine.Format(_T("        <OPT>%s.evq</OPT>\n"), csSgmlPrefix);
        lrdOutput.Append(csLine);
        csLine.Format(_T("        <OPT>%s.obj</OPT>\n"), csSgmlPrefix);
        lrdOutput.Append(csLine);
        csLine = _T("      </H>\n");
        lrdOutput.Append(csLine);
    }

    if ((isVideoEnabled_ && !bIsSgOnlyDocument) || 
        (activePresentation_ && !activePresentation_->IsClipListEmpty())) {
        csLine = _T("      <H>\n");
        lrdOutput.Append(csLine);
        csLine.Format(_T("        <OPT>java</OPT>\n"));
        lrdOutput.Append(csLine);
        csLine.Format(_T("        <OPT>imc.epresenter.player.JMFVideoPlayer</OPT>\n"));
        lrdOutput.Append(csLine);
        if (isVideoEnabled_ && !bIsSgOnlyDocument) {
            csLine.Format(_T("        <OPT>-video</OPT>\n"));
            lrdOutput.Append(csLine);
            csLine.Format(_T("        <OPT>%s.avi</OPT>\n"), csSgmlPrefix);
            lrdOutput.Append(csLine);
        }
        if (activePresentation_ && !activePresentation_->IsClipListEmpty()) {
            csLine.Format(_T("        <OPT>-multi</OPT>\n"));
            lrdOutput.Append(csLine);
            activePresentation_->GetClipList()->WriteToLrd(&lrdOutput);
        }

        csLine = _T("      </H>\n");
        lrdOutput.Append(csLine);
    }

    csLine = _T("    </HELPERS>\n");
    lrdOutput.Append(csLine);

    if (isVideoEnabled_ && !bIsSgOnlyDocument) {
        int videoOffset = 0;
        try {
            // videoOffset = (-1) * avgrabber_->getVideoOffset();
            videoOffset = avgrabber_->getVideoOffset();
        } catch (exception &e) {
            MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
            videoOffset = 0;
        }

        csLine = _T("    <OFFSETS>\n");
        lrdOutput.Append(csLine);

        csLine = _T("      <O>0</O>\n");
        lrdOutput.Append(csLine);

        csLine.Format(_T("      <O>%d</O>\n"), videoOffset);
        lrdOutput.Append(csLine);

        csLine = _T("    </OFFSETS>\n");
        lrdOutput.Append(csLine);
    }

    csLine = _T("    <SLIDES>\n");
    lrdOutput.Append(csLine);
    csLine = _T("    </SLIDES>\n");
    lrdOutput.Append(csLine);

    csLine = _T("    <APPS>\n");
    lrdOutput.Append(csLine);
    csLine = _T("    </APPS>\n");
    lrdOutput.Append(csLine);

    if (bIsSgOnlyDocument || (thumbs_.GetElementCount() > 0)) {
        csLine.Format(_T("    <METADATA>%s.lmd</METADATA>\n"), csSgmlPrefix);
        lrdOutput.Append(csLine);
    }

    csLine = _T("  </FILES>\n");
    lrdOutput.Append(csLine);

    // configuration infos
    csLine = _T("  <CONFIG>\n");
    lrdOutput.Append(csLine);

    csLine = _T("  <SLIDES>\n");
    lrdOutput.Append(csLine);
    csLine = _T("  </SLIDES>\n");
    lrdOutput.Append(csLine);

    csLine = _T("  <APPS>\n");
    lrdOutput.Append(csLine);
    csLine = _T("  </APPS>\n");
    lrdOutput.Append(csLine);

    csLine = _T("  </CONFIG>\n");
    lrdOutput.Append(csLine);

    csLine = _T("</DOCUMENT>\n");
    lrdOutput.Append(csLine);

    lrdOutput.Flush();
    CloseHandle(hLrdFile);
}

CLepSgml *ASSISTANT::Project::CreateLepFile(int iTotalRecordingTime, 
                                      int iSlideWidth, int iSlideHeight, 
                                      CMetaInformation &metaInfo,
                                      CString &csSgmlFilePrefix) {

    bool bHasClips = false;
    if (activePresentation_ != NULL && !activePresentation_->IsClipListEmpty())
        bHasClips = true;
    
    bool bVideoRecorded = false;
    if (isVideoEnabled_ && !IsSgOnlyDocument())
        bVideoRecorded = true;

    CLepSgml *pNewLepSgml = CLepSgml::CreateDefaultLep(bVideoRecorded, bHasClips, CStudioApp::s_sCompatibleLepVersion, CStudioApp::s_sCurrentLepVersion);

    // Metadata
    pNewLepSgml->SetMetaInfo(_T("title"), metaInfo.GetTitle());
    pNewLepSgml->SetMetaInfo(_T("author"), metaInfo.GetAuthor());
    pNewLepSgml->SetMetaInfo(_T("creator"), metaInfo.GetCreator());
    pNewLepSgml->SetMetaInfo(_T("length"), metaInfo.GetLength());
    pNewLepSgml->SetMetaInfo(_T("recorddate"), metaInfo.GetRecordDate());
    pNewLepSgml->SetMetaInfo(_T("recordtime"), metaInfo.GetRecordTime());

    // lrd file
    CString csLrdFile = csSgmlFilePrefix + _T(".lrd");
    pNewLepSgml->AddLrdName(csLrdFile);

    // streams
    CString csAudioName = csSgmlFilePrefix + _T(".lad");
    pNewLepSgml->AddAudioStreamInfo(csAudioName, 0, iTotalRecordingTime);
    
    if (bVideoRecorded) {
        CString csVideoName = csSgmlFilePrefix + _T(".avi");
        int iVideoOffset = 0;
        if (avgrabber_ != NULL)
            iVideoOffset = avgrabber_->getVideoOffset();
        pNewLepSgml->AddVideoStreamInfo(csVideoName, 0, iTotalRecordingTime, iVideoOffset);
    }

        pNewLepSgml->AddWhiteboardStreamInfo(csSgmlFilePrefix, iSlideWidth, iSlideHeight, 
                                         0, iTotalRecordingTime);

    // Read page structure and copy to a new array (ThumbPage is only available in Assistant code)
    // TODO:: Replace ThumbPage, ThumbChapter, ... with CPageInformation, CChapterInformation, ...
    CArray<PLAYER::ThumbElement *, PLAYER::ThumbElement *> aThumbs;
    thumbs_.GetSlides(aThumbs);
    CArray<CPageInformation *, CPageInformation *> aSlides;
    for (int i = 0; i < aThumbs.GetCount(); ++i) {
        PLAYER::ThumbPage *pThumbPage = (PLAYER::ThumbPage *)aThumbs[i];
        if (pThumbPage != NULL) {
            CPageInformation *pNewSlide = new CPageInformation();
            pNewSlide->SetBegin(pThumbPage->GetBegin());
            pNewSlide->SetEnd(pThumbPage->GetEnd());
            pNewSlide->SetTitle(pThumbPage->GetTitle());
            pNewSlide->SetKeywords(pThumbPage->GetKeywords());
            pNewSlide->SetNumber(pThumbPage->GetNumber());
            pNewSlide->SetInternalNumber(i+1);
            aSlides.Add(pNewSlide);
        }
    }
    pNewLepSgml->AddSlidesStreamInfo(aSlides, 0, iTotalRecordingTime);

    for (int i = 0; i < aSlides.GetCount(); ++i)
        if (aSlides[i] != NULL)
            delete aSlides[i];
    aSlides.RemoveAll();
    aThumbs.RemoveAll();
    
    CClipInfo *pClipInfo = NULL;
    if (activePresentation_ != NULL && activePresentation_->GetClipList() != NULL &&
        activePresentation_->GetClipList()->GetClip(0) != NULL) {
        pClipInfo = new CClipInfo();
        pClipInfo->SetFilename(activePresentation_->GetClipList()->GetClip(0)->GetFileName());
        pClipInfo->SetLength(activePresentation_->GetClipList()->GetClip(0)->GetLength());
        pClipInfo->SetTimestamp(activePresentation_->GetClipList()->GetClip(0)->GetStartTime());
    }
    pNewLepSgml->AddClipsStreamInfo(pClipInfo, 0, iTotalRecordingTime);
    if (pClipInfo != NULL)
        delete pClipInfo;

    pNewLepSgml->UpdateInteractionsTag(iTotalRecordingTime);

    if (pNewLepSgml != NULL) {
        CString csLrdPrefix = m_csLrdName;
        StringManipulation::GetPathFilePrefix(csLrdPrefix);
        CString csLepFilename = csLrdPrefix + _T(".lep");
        pNewLepSgml->Write(csLepFilename);
    }
    
    return pNewLepSgml;
}

HRESULT ASSISTANT::Project::GenerateUniqueRecordingTitle(CString &csRecordTitle) {
    HRESULT hr = S_OK;

    // The title should be unique (not existent in the list of actual recordings)
    CStringArray arRecordingNames;
    for (int i = 0; i < presentations_.GetSize(); ++i)
        arRecordingNames.Add(presentations_[i]->GetTitle());

    ASSISTANT::CalculateNextPrefix(csRecordTitle, arRecordingNames);

    arRecordingNames.RemoveAll();

    return hr;
}

HRESULT ASSISTANT::Project::CreateRecordingNameFromPrefix(CString csRecordPrefix, CString &csRecordName) {
    HRESULT hr = S_OK;

    csRecordName.Empty();

    if (!m_bFixedTargetName) {
        CTime time = CTime::GetCurrentTime();
        CString csDateTime;
        csDateTime.Format(_T("%02d%02d%02d_%02d%02d"), 
            time.GetYear(), time.GetMonth(), time.GetDay(),
            time.GetHour(), time.GetMinute());

        // There may be problems in the export format if the filename 
        // include " "
        // remove any filename reserved characters from the new recording file
        CString csModifiedRecordPrefix = csRecordPrefix;

        // The prefix should not be longer than 20 characters
        if (csRecordPrefix.GetLength() > 20)
            csModifiedRecordPrefix = csRecordPrefix.Left(20);

        StringManipulation::RemoveReservedFilenameChars(csModifiedRecordPrefix);
        csModifiedRecordPrefix.Replace(_T(" "), _T("_"));


        ASSISTANT::CalculateNextDataPath(csDateTime, recordPath_, csModifiedRecordPrefix, dataPath_);

        csRecordName = dataPath_;
        csRecordName += _T("\\");
        csRecordName += csModifiedRecordPrefix;
        csRecordName += _T("_");
        csRecordName += csDateTime;
    } else {
        csRecordName = m_csLrdName;
        StringManipulation::GetPathFilePrefix(csRecordName);

        dataPath_ = m_csLrdName;
        StringManipulation::GetPath(dataPath_);
    }

    if (_taccess(dataPath_, 00) == -1)
    {
        CreateDirectory(dataPath_, NULL);
        if (_taccess(dataPath_, 00) == -1)
        {
            CString csMessage;
            csMessage.Format(IDS_ERROR_CREATE_DIR, dataPath_);
            MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
            hr = E_ACCESSDENIED;
        }
    }

    return hr;
}

HRESULT ASSISTANT::Project::CreateLadInfo(char **pszLadInfo) {
    HRESULT hr = S_OK;

    char date[20];
    _strdate(date);

    char *q;
    q = date;
    char *p = strchr(q,'/');
    *p = '\0';
    int month =  atoi(date);
    p++;
    q = p;
    p = strchr(q,'/');
    *p = '\0';
    int day =  atoi(q);
    p++;
    q = p;
    int year =  atoi(q)+2000;

    int iVersionType = FULL_VERSION;

    // Get Application version type
    CSecurityUtils securityUtils;
    CString csOwnVersion;
    csOwnVersion.LoadString(IDS_VERSION);
    iVersionType = securityUtils.GetVersionType(csOwnVersion);

    char szVersionType[2];
    szVersionType[0] = 'f';
    szVersionType[1] = '\0';
    if (iVersionType == EVALUATION_VERSION)
        szVersionType[0] = 'e';
    else if (iVersionType == UNIVERSITY_VERSION)
        szVersionType[0] = 'u';

    struct tm timeStruct;
    time_t now;
    time(&now);    
    _localtime64_s( &timeStruct, &now );

    sprintf(*pszLadInfo, "%s;%s;%02d:%02d:%02d;%d.%d.%d\0",
            szVersionType, "2.0", 
            timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec, 
            year, month, day);

    return hr;
}

void ASSISTANT::Project::CheckAudioLevel(bool bReset)
{
    static bool
        isAudioWarning = false;

    if (m_dlgAudioLevelWarning.m_hWnd == NULL) {
        CWnd *pDesktop = CWnd::FromHandle(GetDesktopWindow());
        m_dlgAudioLevelWarning.Create(IDD_AUDIO_LEVEL_WARNING, pDesktop);
        m_dlgAudioLevelWarning.ModifyStyleEx(0, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
    }

    if (bReset)
    {
        isAudioWarning = false;
        m_dlgAudioLevelWarning.ShowWindow(SW_HIDE);
        return;
    }

    if (!avgrabber_)
        return;

    if (isAudioDeviceSet_ && recording)
    {
        try
        {
            if (avgrabber_->getAudioState() != AUDIO_NORMAL)
            {
                m_dlgAudioLevelWarning.ShowWindow(SW_SHOW);
                m_dlgAudioLevelWarning.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
                isAudioWarning = true;
            }
            else if (isAudioWarning)
            {
                m_dlgAudioLevelWarning.ShowWindow(SW_HIDE);
                isAudioWarning = false;
            }
        }
        catch (exception &e)
        {
            MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
        }

    }
    else
    {
        if (isAudioWarning)
        {
            m_dlgAudioLevelWarning.ShowWindow(SW_HIDE);
            isAudioWarning = false;
        }
    }
}

int ASSISTANT::Project::GetRecordingTime()
{
   int 
      rTime;
   
   if (!avgrabber_)
      return -1;
   
   if (recording)
   {
      try 
      {
         rTime =  avgrabber_->getRecordingTime();
      }
      catch (exception &e)
      {
         rTime = -1;
         MessageBoxA(NULL, e.what(), NULL, MB_OK | MB_TOPMOST);
      }
   }
   else
   {
      rTime = -1;
   }
   return rTime;
}

void ASSISTANT::Project::ResetRecording()
{
   for (int i = 0; i < documents_.GetSize(); ++i)
      documents_[i]->ResetRecording();
   
   if (imageList_)
      imageList_->ResetRecording();
   
   recCount = 0;
   lastRecordedTime_ = -1;
}

void ASSISTANT::Project::SortRecordingsList(int nColumn, BOOL bAscending) 
{
   CArray<PresentationInfo *, PresentationInfo *> aTmpPresentations;
   aTmpPresentations.Copy(presentations_);

   if (nColumn == 0)          // title
   {
      presentations_.RemoveAll();
      for (int i = 0; i < aTmpPresentations.GetSize(); ++i)
      {
         CString csTitle = aTmpPresentations[i]->GetTitle();
         bool bInserted = false;
         for (int j = 0; j < presentations_.GetSize() && !bInserted; ++j)
         {
            CString csTitle2 = presentations_[j]->GetTitle();

            if (bAscending && (csTitle > csTitle2))
            {
               presentations_.InsertAt(j, aTmpPresentations[i]);
               bInserted = true;
            }
            else if (!bAscending && (csTitle < csTitle2))
            {
               presentations_.InsertAt(j, aTmpPresentations[i]);
               bInserted = true;
            }
         }
         if (!bInserted)
            presentations_.Add(aTmpPresentations[i]);
      }
   }
   else if (nColumn == 1)      // date
   {
      presentations_.RemoveAll();
      for (int i = 0; i < aTmpPresentations.GetSize(); ++i)
      {
         CString csDate = aTmpPresentations[i]->GetRecorddate(); 
         SYSTEMTIME sTime;
         bool bUseTimeComparison = ASSISTANT::GetDateFromString(sTime, csDate);
         CString csTime = aTmpPresentations[i]->GetRecordtime();
         bUseTimeComparison = bUseTimeComparison && ASSISTANT::GetTimeFromString(sTime, csTime);
         CTime cTimeDate;
         if ( bUseTimeComparison == false )
            csDate += _T(" ") + csTime;
         else
            cTimeDate = CTime(sTime.wYear, sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, 0);

         bool bInserted = false;
         for (int j = 0; j < presentations_.GetSize() && !bInserted; ++j)
         {
            CString csDate2 = presentations_[j]->GetRecorddate(); 
            SYSTEMTIME sTime2;
            bool bUseTimeComparison2 = ASSISTANT::GetDateFromString(sTime2, csDate2);
            CString csTime2 = presentations_[j]->GetRecordtime();
            bUseTimeComparison2 = bUseTimeComparison2 && ASSISTANT::GetTimeFromString(sTime2, csTime2);
            CTime cTimeDate2;
            if ( bUseTimeComparison2 == false )
               csDate2 += _T(" ") + csTime2;
            else
               cTimeDate2 = CTime(sTime2.wYear, sTime2.wMonth, sTime2.wDay, sTime2.wHour, sTime2.wMinute, 0);

            // Compare by Time just if both parses completed with succes
            if ( bUseTimeComparison && bUseTimeComparison2 )
            {
               if ((bAscending && (cTimeDate > cTimeDate2) )
                  || (!bAscending && (cTimeDate < cTimeDate2)))
               {
                  presentations_.InsertAt(j, aTmpPresentations[i]);
                  bInserted = true;
               }
            }
            else // make comparison using strings
            {
               if ( (bAscending && (csDate > csDate2))
                  || (!bAscending && (csDate < csDate2)))
               {
                  presentations_.InsertAt(j, aTmpPresentations[i]);
                  bInserted = true;
               }
            }
         }
         if (!bInserted)
            presentations_.Add(aTmpPresentations[i]);
      }
   }
   else if (nColumn == 2)      // length
   {
      presentations_.RemoveAll();
      for (int i = 0; i < aTmpPresentations.GetSize(); ++i)
      {
         CString csLength = aTmpPresentations[i]->GetRecordlength(); 
         bool bInserted = false;
         for (int j = 0; j < presentations_.GetSize() && !bInserted; ++j)
         {
            CString csLength2 = presentations_[j]->GetRecordlength(); 
            
            if (bAscending && (csLength > csLength2))
            {
               presentations_.InsertAt(j, aTmpPresentations[i]);
               bInserted = true;
            }
            else if (!bAscending && (csLength < csLength2))
            {
               presentations_.InsertAt(j, aTmpPresentations[i]);
               bInserted = true;
            }
         }
         if (!bInserted)
            presentations_.Add(aTmpPresentations[i]);
      }
   }

   aTmpPresentations.RemoveAll();

   SaveToRegistry();
}

void ASSISTANT::Project::ReadAudioSettings(bool bSet)
{
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
   DWORD
      subValues;
   TCHAR
      entryName[100];
   TCHAR
      entryValue[512];
   DWORD
      entryNameLength = 100,
      entryValueLength = 512,
      dwType;

   // Read SG options
   CString csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\Audio");

   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_READ, &hKeyUser);
   if (regErr == ERROR_SUCCESS )
   {
      regErr = RegQueryInfoKey(hKeyUser, NULL, NULL, NULL, NULL, NULL, NULL, &subValues, NULL, NULL, NULL, NULL);
      
      if ( regErr == ERROR_SUCCESS )
      {
         for (int i = 0; i < subValues; i++) 
         {             
            entryNameLength = 100;
            entryValueLength = 512;
            if (RegEnumValue(hKeyUser, i, entryName, &entryNameLength, NULL, &dwType, (BYTE *)entryValue, &entryValueLength) == ERROR_SUCCESS)
            {
               if (dwType == REG_SZ)
               {
                  CString csEntry = entryName;
                  if (csEntry == _T("DEVICE"))
                  {
                     audioDevice_ = _ttoi((TCHAR *)entryValue);
                  }
                  else if (csEntry == _T("FRAMERATE"))
                  {
                     audioFramerate_ = _ttoi((TCHAR *)entryValue);
                  }
                  else if (csEntry == _T("BITRATE"))
                  {
                     audioBitrate_ = _ttoi((TCHAR *)entryValue);
                  }
               }
            }
         }
      }
      RegCloseKey(hKeyUser);
   }
   
   if (bSet)
      ActivateAudioDevice();
}

HRESULT ASSISTANT::Project::WriteAudioSettings()
{
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
   DWORD
      dwDummy;
   DWORD
      entryNameLength = 100,
      entryValueLength = 512;

   // Read Audio settings
   CString csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\Audio");

   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_ALL_ACCESS, &hKeyUser);
   
   if ( regErr != ERROR_SUCCESS )
   {
      regErr = RegCreateKeyEx(HKEY_CURRENT_USER, csSubKey,
                              NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyUser, &dwDummy); 
      
      if ( regErr != ERROR_SUCCESS )
         return E_FAIL;
   }

   if (regErr == ERROR_SUCCESS )
   {
      TCHAR value[512];

      _stprintf(value, _T("%d"), audioDevice_);
      regErr = RegSetValueEx(hKeyUser, _T("DEVICE"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), audioFramerate_);
      regErr = RegSetValueEx(hKeyUser, _T("FRAMERATE"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));

      _stprintf(value, _T("%d"), audioBitrate_);
      regErr = RegSetValueEx(hKeyUser, _T("BITRATE"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));

      RegCloseKey(hKeyUser);
   }

   return S_OK;
}

HRESULT ASSISTANT::Project::GetAudioDevices(CStringArray &arDevices, UINT &nDevice)
{
   arDevices.RemoveAll();
   nDevice = 0;

   if (!avgrabber_)
      return E_FAIL;

   if (audioDeviceCount_ != 0)
   {
      for (int i = 0; i < audioDeviceCount_; ++i)
         arDevices.Add(audioDevices_[i]->tszName);

      nDevice = audioDevice_;
   }

   return S_OK;
}

HRESULT ASSISTANT::Project::GetSampleWidths(UINT nAudioDevice, CUIntArray &arSampleWidths, UINT &nSampleWidth)
{
   arSampleWidths.RemoveAll();
   nSampleWidth = 0;

   if (!avgrabber_)
      return E_FAIL;

   if (nAudioDevice >= audioDeviceCount_)
      return E_FAIL;

   if (audioDeviceCount_ != 0)
   {
      AudioDevice *pAudioDevice = audioDevices_[nAudioDevice];

      for (int i = 0; i < pAudioDevice->nBitRates; ++i)
      {
         if (pAudioDevice->bitRates[i] == audioBitrate_)
            nSampleWidth = i;
         arSampleWidths.Add(pAudioDevice->bitRates[i]);
      }
   }

   return S_OK;
}

HRESULT ASSISTANT::Project::GetSampleRates(UINT nAudioDevice, CUIntArray &arSampleRates, UINT &nSampleRate)
{
   arSampleRates.RemoveAll();
   nSampleRate = 0;

   if (!avgrabber_)
      return E_FAIL;

   if (nAudioDevice >= audioDeviceCount_)
      return E_FAIL;

   if (audioDeviceCount_ != 0)
   {
      AudioDevice *pAudioDevice = audioDevices_[nAudioDevice];

      for (int i = 0; i < pAudioDevice->nSampleRates; ++i)
      {
         if (audioFramerate_ == pAudioDevice->sampleRates[i])
            nSampleRate = i;
         arSampleRates.Add(pAudioDevice->sampleRates[i]);
      }
   }

   return S_OK;
}

HRESULT ASSISTANT::Project::GetAudioSources(CStringArray &arSources, UINT &nSource)
{
   arSources.RemoveAll();
   nSource = 0;

   if (!avgrabber_)
      return E_FAIL;

   if (mixerSourceCount_ != 0)
   {
      for (int i = 0; i < mixerSourceCount_; ++i)
         arSources.Add(mixerSources_[i]->tszName);

      nSource = mixerSource_;
   }

   return S_OK;
}

bool ASSISTANT::Project::RecordingIsPossible()
{
   if (avgrabber_ == NULL || audioDeviceCount_ <= 0)
      return false;
   else
      return true;
}

double ASSISTANT::Project::GetAudioLevel()
{
   if (!avgrabber_)
      return 0;

   int iAudioLevel = avgrabber_->getAudioLevel();
   double dAudioLevel = (double)iAudioLevel / 100;


   return dAudioLevel;
}

UINT ASSISTANT::Project::GetMixerVolume()
{
   if (!avgrabber_)
      return 0;

   if (!audioMixer_)
      return 0;

   return audioMixer_->getVolume();
}

HRESULT ASSISTANT::Project::SetMixerVolume(UINT nMixerVolume)
{
   if (!avgrabber_)
      return E_FAIL;

   if (!audioMixer_)
      return E_FAIL;

   audioMixer_->setVolume(nMixerVolume);

   return S_OK;
}

// Functions concerning document


void ASSISTANT::Project::SetActiveDocument(uint32 id) 
{
   for (int i = 0; i < documents_.GetSize(); ++i) 
   {
      if (documents_[i]->GetID() == id) 
      {
         activeDocument_ = i;
         break;
      }
   }
   
   return;
}

void ASSISTANT::Project::SetNoActiveDocument() 
{
    activeDocument_ = -1;
   
   return;
}

bool ASSISTANT::Project::RemoveDocumentFromList(uint32 id) 
{
    bool bRemoveSuccess = true;

    for (int i = 0; i < documents_.GetSize(); ++i) {
        if (documents_[i]->GetID() == id) {
            bool bCloseSuccess = true;
            if (documents_[i]->IsLoaded()) {
                SetActiveDocument(id);
                bCloseSuccess = CloseDocument();
            }
            if (bCloseSuccess) {
                delete documents_[i];
                documents_.RemoveAt(i);
                SaveToRegistry();

                SetNewActiveDocument(i);
            }
            else
                bRemoveSuccess = false;
        }
    }

   return bRemoveSuccess;
}

ASSISTANT::Document* ASSISTANT::Project::GetActiveDocument() 
{
   // no documents yet
   if (documents_.GetSize() == 0)
      return NULL;
   
   // no active document set
   if (activeDocument_ == -1) 
      return NULL; 
   
   return (Document *)documents_[activeDocument_];
}


//
void ASSISTANT::Project::ReadMetadata(SgmlElement *root) {
    CString csTitle = 
        AfxGetApp()->GetProfileString(_T("Metadata"), _T("Title"), _T(""));
    if (!csTitle.IsEmpty())
        metadata_->SetTitle(csTitle);

    CString csAuthor = 
        AfxGetApp()->GetProfileString(_T("Metadata"), _T("Author"), _T(""));
    if (!csAuthor.IsEmpty())
        metadata_->SetAuthor(csAuthor);

    CString csCreator = 
        AfxGetApp()->GetProfileString(_T("Metadata"), _T("Creator"), _T(""));
    if (!csCreator.IsEmpty())
        metadata_->SetCreator(csCreator);

    CString csKeywords = 
        AfxGetApp()->GetProfileString(_T("Metadata"), _T("Keywords"), _T(""));
    if (!csKeywords.IsEmpty())
        metadata_->SetKeywords(csKeywords);

    return;
}

void ASSISTANT::Project::WriteMetadata()
{
    AfxGetApp()->WriteProfileString(_T("Metadata"), _T("Title"), metadata_->GetTitle());

    AfxGetApp()->WriteProfileString(_T("Metadata"), _T("Author"), metadata_->GetAuthor());

    AfxGetApp()->WriteProfileString(_T("Metadata"), _T("Creator"), metadata_->GetCreator());

    AfxGetApp()->WriteProfileString(_T("Metadata"), _T("Keywords"), metadata_->GetKeywords());
}

HRESULT  ASSISTANT::Project::RemovePresentation(PresentationInfo *pPresentation)
{
   HRESULT hr = E_FAIL;
   for (int i = 0; i < presentations_.GetSize(); ++i)
   {
      if (presentations_[i] == pPresentation)
      {
         if (presentations_[i] == activePresentation_)
         {
            if (i < presentations_.GetSize() - 1)
               activePresentation_ = presentations_[i+1];
            else if (i > 0)
               activePresentation_ = presentations_[i-1];
            else 
               activePresentation_ = NULL;
         }
         delete presentations_[i];
         presentations_.RemoveAt(i);
 
         SaveToRegistry();
         SetChanged(true);
         hr = S_OK;
         break;
      }
   }

   return hr;
}

HRESULT  ASSISTANT::Project::DeletePresentation(PresentationInfo *pPresentation)
{
   HRESULT hr = E_FAIL;
   for (int i = 0; i < presentations_.GetSize(); ++i)
   {
      if (presentations_[i] == pPresentation)
      {
         if (presentations_[i] == activePresentation_)
         {
            if (i < presentations_.GetSize() - 1)
               activePresentation_ = presentations_[i+1];
            else if (i > 0)
               activePresentation_ = presentations_[i-1];
            else 
               activePresentation_ = NULL;
         }
         
         CString csLrdName = presentations_[i]->GetFilename();

         // Delete lep file if exists
         CString csLepPath = CEditorDoc::GetCorrespondingLep(csLrdName);
         if (!csLepPath.IsEmpty())
             ::DeleteFile(csLepPath);

         // Delete all files belonging to presentation (lrd, audio, video, ...)
         CLrdFile lrdFile;
         lrdFile.Open(csLrdName);
         lrdFile.DeleteFiles();

         // Delete directory if empty
         CString csLrdPath = csLrdName;
         ASSISTANT::GetPath(csLrdPath);
         ::RemoveDirectory(csLrdPath);
        

         delete presentations_[i];
         presentations_.RemoveAt(i);
         SaveToRegistry();
         SetChanged(true);
         hr = S_OK;
         break;
      }
   }

   return hr;
}

ASSISTANT::Page *ASSISTANT::Project::GetActivePage() 
{
   Page *pActivePage = NULL;
   
   if (GetActiveDocument())
      pActivePage = GetActiveDocument()->GetActivePage();   
   
   return pActivePage;
}

ASSISTANT::Folder *ASSISTANT::Project::GetActiveFolder() 
{
   Folder *pActiveFolder = NULL;
   
   if (GetActiveDocument())
      pActiveFolder = GetActiveDocument()->GetActiveFolder();   
   
   return pActiveFolder;
}

ASSISTANT::GenericObject* ASSISTANT::Project::GetObject(UINT nId) 
{
   GenericObject *pContainer = NULL;
   
   for (int i = 0; i < documents_.GetSize() && pContainer == NULL; ++i) 
   {
      Document *pDocument = (Document *)documents_[i];
      if (pDocument)
      {
         if (pDocument->GetID() == nId) 
            pContainer = pDocument;
         
         if (pContainer == NULL)
            pContainer = pDocument->GetObject(nId);
      }
   }
   
   return pContainer;
}

ASSISTANT::GenericContainer* ASSISTANT::Project::GetParentContainer(uint32 id) 
{
   GenericContainer* container = NULL;
   for (int i =0; i < documents_.GetSize(); ++i) {
      container = documents_[i]->GetContainer(id);
      if (container) return container;
   }
   
   return NULL;
}


ASSISTANT::Document* ASSISTANT::Project::GetParentDocument(ASSISTANT::GenericObject* obj) 
{
   if (obj->GetType() == GenericObject::DOCUMENT) 
      return (Document*)obj;
   
   GenericContainer* container = NULL;
   for (int i =0; i < documents_.GetSize(); ++i) 
   {
      container =  documents_[i]->GetContainer(obj->GetID());
      if (container) 
         return documents_[i];
   }
   
   return NULL;
}

bool ASSISTANT::Project::IsDocumentInProject(LPCTSTR documentName)
{
   
   for (int i = 0; i < documents_.GetSize(); ++i)
   {
      CString existingDocumentName;
      existingDocumentName = documents_[i]->GetDocumentPath();
      existingDocumentName += _T("\\");
      existingDocumentName += documents_[i]->GetDocumentName();
      if (existingDocumentName == documentName)
         return true;
   }
   
   return false;
}

void ASSISTANT::Project::InsertNewFolder() 
{
   if (GetActiveDocument())
   {
      GetActiveDocument()->InsertEmptyFolder();
      
      ActivatePage();
      
      GetActiveDocument()->SetChanged(true);
   }
   
   return;
}

void ASSISTANT::Project::InsertNewPage() 
{    
   if (GetActiveDocument())
   {
      Page *pActivePage = GetActivePage();
      int iWidth = PAGE_WIDTH;
      int iHeight = PAGE_HEIGHT;
      COLORREF clrBackground = BG_COLOR;
      if (pActivePage)
      {
         iWidth = pActivePage->GetWidth();
         iHeight = pActivePage->GetHeight();
		 clrBackground = pActivePage->GetBackground();
		 clrBackground = Gdiplus::Color::MakeARGB(255, GetRValue(clrBackground), GetGValue(clrBackground), GetBValue(clrBackground)); 
      }
      GetActiveDocument()->InsertEmptyPage(iWidth, iHeight, clrBackground);
      
      ActivatePage();
      
      GetActiveDocument()->SetChanged(true);
   }
   
   return;
} 

void ASSISTANT::Project::InsertNewPage(ASSISTANT::Page *_page) 
{
   if (GetActiveDocument()) 
   {
      if (GetActiveFolder())
      {
         GetActiveFolder()->InsertContainerCurrent(_page);
         
         ActivatePage();
         
         GetActiveDocument()->SetChanged(true);
      }
   }
}

void ASSISTANT::Project::DuplicatePage() 
{    
   if (GetActiveDocument())
   {  
      Page *pActivePage = GetActivePage();

      GetActiveDocument()->DuplicatePage(pActivePage);
      
      ActivatePage();
      
      GetActiveDocument()->SetChanged(true);
   }
   
   return;
} 

void ASSISTANT::Project::NextPage()
{
   ActivateNextPage();
}

void ASSISTANT::Project::PreviousPage()
{
   ActivatePreviousPage();
}

void ASSISTANT::Project::FirstPage()
{
   if (GetActiveDocument() == NULL)
      return;

   GetActiveDocument()->GoToFirstPage();
}

bool ASSISTANT::Project::SetKeywords(UINT nContainerId, LPCTSTR szKeywords)
{
   GenericContainer *pContainer = (GenericContainer*)GetObject(nContainerId);
   if (pContainer) 
   {
      pContainer->SetKeywords(szKeywords);
      if (GetActiveDocument())
          GetActiveDocument()->SetChanged(true);

      return true;
   }
   
   return false;
}

HRESULT ASSISTANT::Project::GetKeywords(UINT nContainerId, CString &csKeywords)
{
   GenericContainer *pContainer = (GenericContainer*)GetObject(nContainerId);
   if (pContainer) 
   {
      pContainer->GetKeywords(csKeywords);
      return S_OK;
   }

   return E_FAIL;
}

void ASSISTANT::Project::EscHotkey(bool set)
{
   if (doWithSG_)
   {
    CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();
    if (pMainWnd == NULL)   // TODO: Error message
        return;
      
      HWND hWnd = pMainWnd->m_hWnd;
      if (set)
      {
         BOOL bResult = RegisterHotKey(hWnd, ID_RESET_SG_TOOL, 0, VK_ESCAPE);
         if (bResult == FALSE)
         {
            CString csMessage;
            csMessage.Format(IDS_ERROR_REGISTER_HOTKEY, _T("\"Esc\" "));
            CString csCaption;
            csCaption.LoadString(IDS_WARNING);

            MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
         }
      }
      else
      {
         UnregisterHotKey(hWnd, ID_RESET_SG_TOOL);
      }
   }
}

void ASSISTANT::Project::UpdateSgModul()
{
   if (avgrabber_ == NULL)
      return;

   avgrabber_->setSgDeleteAllOnInteractionMode(deleteAllOnInteraction_);
   avgrabber_->setSgHideWhilePanning(hideWhilePanning_);
   avgrabber_->setSgEnableClickEffects(mouseAudioEffect_, mouseVisualEffect_);
   avgrabber_->setSgQuickCaptureMode(quickCapture_);
   if (pauseVideo_)
      avgrabber_->setSgVideoMode(AV_NULL_ON_SG);
   else
      avgrabber_->setSgVideoMode(AV_CONTINUE_ON_SG);
   avgrabber_->setSgDisableVideoAcceleration(noHardwareAcceleration_);
   if (hasStaticFramerate_)
   {
      avgrabber_->setSgAutoFrameRate(false);
      avgrabber_->setSgFrameRate(staticFramerate_);
   }
   else
   {
      avgrabber_->setSgAutoFrameRate(true, adaptWidth_, adaptHeight_, adaptiveFramerate_);
   }
}

int ASSISTANT::Project::GetMaxWidth()
{
   int iActMax = 0;
   
   for (int i = 0; i < documents_.GetSize(); ++i) 
      documents_[i]->GetMaxWidth(iActMax);
   
   return iActMax;
}

int ASSISTANT::Project::GetMaxHeight()
{
   int iActMax = 0;
   
   for (int i = 0; i < documents_.GetSize(); ++i) 
      documents_[i]->GetMaxHeight(iActMax);
   
   return iActMax;
}

void ASSISTANT::Project::OpenCloseMetadata(GenericContainer *container, LPCTSTR filename)
{
   CString csMetaFilename = filename;
   ASSISTANT::GetPrefix(csMetaFilename);
   csMetaFilename += _T(".lmd");
   
   HANDLE hLmdFile = CreateFile(csMetaFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hLmdFile == INVALID_HANDLE_VALUE)
   {
      CString csMessage;
      csMessage.Format(IDS_ERROR_FILE_OPEN, csMetaFilename);
      MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      return;
   }

   CFileOutput lmdOutput;
   lmdOutput.Init(hLmdFile, 0, true);
   
   CString csLine;

   csLine = _T("<docinfo>\n");
   lmdOutput.Append(csLine);

   csLine = _T("  <structure>\n");
   lmdOutput.Append(csLine);

   container->SaveMetadata(&lmdOutput);

   csLine = _T("  </structure>\n");
   lmdOutput.Append(csLine);

   csLine = _T("</docinfo>\n");
   lmdOutput.Append(csLine);

   lmdOutput.Flush();
   CloseHandle(hLmdFile);
}

void ASSISTANT::Project::FreezeImages(bool bFreezeImages) 
{ 
   m_bFreezeImages = bFreezeImages;

   if (GetActivePage() && m_bFreezeImages)
      GetActivePage()->UnSelectImages();
} 

void ASSISTANT::Project::FreezeMaster(bool bFreezeMaster) 
{ 
   m_bFreezeMaster = bFreezeMaster;

   if (GetActivePage() && m_bFreezeMaster)
      GetActivePage()->UnSelectMasterObjects();
} 

void ASSISTANT::Project::FreezeDocument(bool bFreezeDocument) 
{ 
   m_bFreezeDocument = bFreezeDocument;

   if (GetActivePage() && m_bFreezeDocument)
      GetActivePage()->UnSelectDocumentObjects();
} 

void CALLBACK GoToFirstPage(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
   ASSISTANT::Page
      *firstPage;
   
   if (!ASSISTANT::Project::active->GetActiveDocument()) 
      return;
   
   firstPage = ASSISTANT::Project::active->GetActiveDocument()->GoToFirstPage();
   if (firstPage) 
   {
      ASSISTANT::Project::active->ActivatePage();
   } 
   
   return;
}

bool ASSISTANT::Project::HasChanged() {
    if (documents_.GetSize() == 0) 
        return false;

    for (int i = 0; i < documents_.GetSize(); ++i) {
        if (documents_[i]->HasChanged())
            return true;
    }

    return false;
}

bool ASSISTANT::Project::ContainsDocument(LPCTSTR documentName)
{
   CString
      documentFile;
   
   for (int i = 0; i < documents_.GetSize(); ++i)
   {
      documentFile = documents_[i]->GetDocumentPath();
      documentFile += _T("\\");
      documentFile += documents_[i]->GetDocumentName();
      if (documentFile == documentName)
         return true;
   }
   
   return false;
}

// Functions concern subContainer


// Functions concern document

void ASSISTANT::Project::ActivatePreviousPage(bool _jump) 
{
   Page
      *prevPage;
   bool
      found;
   
   if (!GetActiveDocument())
      return;
   
   int iActiveDocument = activeDocument_;
   found = false;
   prevPage = documents_[iActiveDocument]->GoToPrevPage(NULL, &found, _jump);
   if (!prevPage) 
   {
      prevPage = NULL;
      while (!prevPage && (iActiveDocument > 0))
      {
         --iActiveDocument;
         prevPage = documents_[iActiveDocument]->GoToLastPage();
      }
      if (prevPage) 
      {
         activeDocument_ = iActiveDocument;
      }
   }
   
   ActivatePage(true);
   
   return;
}

void ASSISTANT::Project::ActivateNextPage(bool _jump) 
{
   if (!GetActiveDocument())
      return;

   int iActiveDocument = activeDocument_;
   bool bFound = false;
   Page *nextPage = documents_[iActiveDocument]->GoToNextPage(NULL, &bFound, _jump);
   if (!nextPage) 
   {
      nextPage = NULL;
      while (!nextPage && (iActiveDocument < documents_.GetUpperBound()))
      {
         ++iActiveDocument;
         nextPage = documents_[iActiveDocument]->GoToFirstPage();
         if (nextPage) 
         {
            activeDocument_ = iActiveDocument;
            break;
         }
      }
   }
   
   ActivatePage(true);
   
   return;
}

bool ASSISTANT::Project::NextPageAvailable() {
   if (!GetActiveDocument())
      return false;

   int iActiveDocument = activeDocument_;
   bool bHasNextPage = documents_[iActiveDocument]->NextPageAvailable();
   while (!bHasNextPage && (iActiveDocument < documents_.GetUpperBound())) {
       ++iActiveDocument;
       bHasNextPage = documents_[iActiveDocument]->ContainPages();
   }

   return bHasNextPage;
}

bool ASSISTANT::Project::PreviousPageAvailable() {
    if (!GetActiveDocument())
        return false;

    int iActiveDocument = activeDocument_;
    bool bHasPreviousPage = documents_[iActiveDocument]->PreviousPageAvailable();
    if (!bHasPreviousPage) {
        while (!bHasPreviousPage && iActiveDocument > 0) {
            --iActiveDocument;
            bHasPreviousPage = documents_[iActiveDocument]->ContainPages();
        }
    }


    return bHasPreviousPage;
}

bool ASSISTANT::Project::LoadDocument(UINT nDocumentId)
{
    bool bRenamed = false;
    for (int i = 0; i < documents_.GetSize() && !bRenamed; ++i)
    {
        if (documents_[i]->GetID() == nDocumentId) {
            HRESULT hr = documents_[i]->CreateLoadThread();
            if (SUCCEEDED(hr))
                return true;
            break;
        }
    }

    return false;
}


bool ASSISTANT::Project::RenameContainer(UINT nContainerId, LPCTSTR szContainerName)
{
   if (nContainerId == 0)
   {
      projectName_ = szContainerName;
      return true;
   }

   bool bRenamed = false;
   for (int i = 0; i < documents_.GetSize() && !bRenamed; ++i)
   {
      bRenamed = documents_[i]->RenameContainer(nContainerId, szContainerName);
   }

   return bRenamed;
}

bool ASSISTANT::Project::SelectContainer(UINT nContainerId) 
{
   bool bSelected = false;
   for (int i = 0; i < documents_.GetSize() && !bSelected; ++i)
   {
      bSelected = documents_[i]->SelectContainer(nContainerId);
      if (bSelected)
         activeDocument_ = i;
   }
   
   if (bSelected)
      ActivatePage();

   return bSelected;
}

bool ASSISTANT::Project::DeleteContainer(UINT nContainerId) 
{
   if (lastActivePage)
   {
      lastActivePage->DeActivate();
      lastActivePage = NULL;
   }
   
   bool bDeleted = false;

   GenericContainer *pDeleteContainer = (GenericContainer *)GetObject(nContainerId);
   if (pDeleteContainer) 
   {
      SelectContainer(pDeleteContainer->GetID());

      if (pDeleteContainer->GetType() == GenericObject::DOCUMENT) 
      {
         if (RemoveDocumentFromList(pDeleteContainer->GetID()))
         {
            SetChanged(true);
            bDeleted = true;
         }
      }
      else 
      {
         Document *document = GetActiveDocument();
         // delete all sub container or objects
         pDeleteContainer->DeleteAll();
         if (pDeleteContainer->GetType() == GenericObject::PAGE) 
         {
            if (GetActivePage() == pDeleteContainer) 
            {
               ActivateNextPage(true);
               // if there is no next page
               if (GetActivePage() == pDeleteContainer)
               {
                  ((Page*)pDeleteContainer)->DeActivate();
                  lastActivePage = NULL;
               }
            }
         }
         else 
         {
            lastActivePage = NULL;
            Folder *pParent = (Folder*)GetParentContainer(pDeleteContainer->GetID());
            bool bNewActiveSet = false;
            if (pParent)
            {
               GenericContainer *pNextContainer = pParent->GetNextContainer(pDeleteContainer);
               if(pNextContainer)
               {
                  UINT nNewActiveContainerId = pNextContainer->GetID();
                  SelectContainer(nNewActiveContainerId);
                  bNewActiveSet = true;//false;
               }
            }
            if (!bNewActiveSet)
               activeDocument_ = -1;
         }

         ((GenericContainer*)(document))->DeleteObject(pDeleteContainer->GetID());
         delete pDeleteContainer;
         (document)->SetChanged(true);
         bDeleted = true;
      }
      
   }
   
   ActivatePage(true);
   
   return bDeleted;
}

bool ASSISTANT::Project::SaveContainer(UINT nContainerId, LPCTSTR szFileName)
{
   /*/
   if (nContainerId == 0)
   {
      SaveAs(szFileName);
      return true;
   }
   /*/

   bool bSaved = false;
   for (int i = 0; i < documents_.GetSize() && !bSaved; ++i)
   {
      bSaved = ((GenericContainer *)documents_[i])->SaveContainer(nContainerId, szFileName);
   }

   return bSaved;
}

bool ASSISTANT::Project::MoveContainer(UINT nMoveContainerId, UINT nContainerId, bool bInsertInChapter, bool bInsertFirst)
{
   bool bDeleted = false;

   // The active page should not be changed, so it is necessary to save it
   Page *pActivePage = GetActivePage();

   GenericContainer *pMoveContainer = (GenericContainer *)GetObject(nMoveContainerId);
   if (pMoveContainer) 
   {
      SelectContainer(pMoveContainer->GetID());
      if (pMoveContainer->GetType() == GenericObject::DOCUMENT) 
      {
         int iDeleteIndex = -1;
         for (int i = 0; i < documents_.GetSize(); ++i)
         {
            if (documents_[i] == pMoveContainer)
            {
               iDeleteIndex = i;
               break;
            }
         }
         
         if (iDeleteIndex >= 0 && iDeleteIndex < documents_.GetSize())
         {
            SelectContainer(nContainerId);

            if (GetActiveDocument())
            {
               int nActiveDocumentID = GetActiveDocument()->GetID();

               if (activeDocument_ != iDeleteIndex)
               {
                  documents_.RemoveAt(iDeleteIndex);
                  
                  for (int i = 0; i < documents_.GetSize(); ++i)
                  {
                     if (documents_[i]->GetID() == nActiveDocumentID && !bInsertFirst)
                     {
                        documents_.InsertAt(i+1, (Document *)pMoveContainer);
                        activeDocument_ = i+1;
                        break;
                     }
					 else
					 if (documents_[i]->GetID() == nActiveDocumentID && bInsertFirst)
                     {
                        documents_.InsertAt(0, (Document *)pMoveContainer);
                        activeDocument_ = 0;
                        break;
                     }

                  }
               }
               if (activeDocument_ < 0 && documents_.GetSize() > 0)
                  activeDocument_ = 0;
               if (activeDocument_ >= documents_.GetSize())
                  activeDocument_ = documents_.GetSize() - 1;
            }
            else
               activeDocument_ = -1;
            bDeleted = true;
         }

         SaveToRegistry();
         return bDeleted;     
      }
      else 
      {
         Document *document = GetActiveDocument();
         if (document)
             ((GenericContainer*)(document))->DeleteObject(pMoveContainer->GetID());
         bDeleted = true;
      }
      
   }
   
   if (bDeleted && pMoveContainer)
   {  
      GenericContainer *pDestContainer = (GenericContainer *)GetObject(nContainerId);
      if (pDestContainer && pDestContainer->GetType() == GenericObject::FOLDER)
      {
         if (bInsertInChapter && 
            (pMoveContainer->GetType() == GenericObject::FOLDER || 
             pMoveContainer->GetType() == GenericObject::PAGE))
         {
            pDestContainer->InsertContainerBegin(pMoveContainer);
         }
         else
         {
             SelectContainer(nContainerId);
             if (GetActiveDocument()) {
                 pDestContainer = GetActiveDocument()->GetParent(nContainerId);
                 if (pDestContainer)
                     pDestContainer->InsertContainerCurrent(pMoveContainer);
             }
         }
         SelectContainer(nMoveContainerId);
      }
      else
      {
          SelectContainer(nContainerId);
          if (GetActiveDocument()) {
              pDestContainer = GetActiveDocument()->GetParent(nContainerId);
              if (pDestContainer)
                  pDestContainer->InsertContainerCurrent(pMoveContainer);
              else
                  GetActiveDocument()->InsertContainerCurrent(pMoveContainer);
         }
         SelectContainer(nMoveContainerId);
      }
      if (GetActiveDocument())
          GetActiveDocument()->SetChanged(true);
   }
   
   // The active page should not be changed, so if there was one, activate it.
   if (pActivePage)
      SelectContainer(pActivePage->GetID());
   
   
   return bDeleted;
}

// Functions concern active document

int ASSISTANT::Project::GetNbrOfPages() 
{
   int 
      count = 0;
   
   for (int i = 0; i < documents_.GetSize(); ++i)
   {
      count += documents_[i]->GetNbrOfPages();
   }
   
   return count;
}

int ASSISTANT::Project::GetPageNumber(UINT nId, bool &bFound) 
{
   int 
      count = 0;
   bFound = false;
   for (int i = 0; i < documents_.GetSize(); ++i)
   {
      count += documents_[i]->GetPageNumber(nId, bFound);
      if(bFound)
         return count;
   }
   
   return count;
}

CLepSgml *ASSISTANT::Project::UpdateProjectFiles(int _time, bool bReturnLep) {

    CreateLmdFile(_time);

    int iSlideWidth = 720;
    int iSlideHeight = 540;
    if (GetActivePage()) {
        iSlideWidth  = GetMaxWidth();
        iSlideHeight = GetMaxHeight();
    } else if (IsSgOnlyDocument()) {
        if (avgrabber_ != NULL) {
            CRect rcSgRectangle;
            bool bSuccess = avgrabber_->getSgCaptureRect(&rcSgRectangle);
            if (bSuccess) {
                iSlideWidth = rcSgRectangle.Width();
                iSlideHeight = rcSgRectangle.Height();
            }
        }
    }

    if (m_csLrdName.IsEmpty()) {
        m_csLrdName = dataPath_;
        m_csLrdName += _T("\\");
        m_csLrdName += projectPrefix_;
        // remove any filename reserved characters from the new recording file
        StringManipulation::RemoveReservedFilenameChars(m_csLrdName);
        m_csLrdName += _T(".lrd");
    }

    // Copy metaInformation: TODO Use CMetaInformation instead of ASSISTANT::ProjectMetadata
    CString csSgmlString;
    CMetaInformation metaInfo;
    if (activePresentation_ != NULL)
        StringManipulation::TransformForSgml(activePresentation_->GetTitle(), csSgmlString);
    else
        StringManipulation::TransformForSgml(metadata_->GetTitle(), csSgmlString);
    metaInfo.SetTitle(csSgmlString);
    StringManipulation::TransformForSgml(metadata_->GetAuthor(), csSgmlString);
    metaInfo.SetAuthor(csSgmlString);
    StringManipulation::TransformForSgml(metadata_->GetCreator(), csSgmlString);
    metaInfo.SetCreator(csSgmlString);
    StringManipulation::TransformForSgml(metadata_->GetKeywords(), csSgmlString);
    metaInfo.SetKeywords(csSgmlString);
    CString csRecordLength;
    ASSISTANT::TranslateMsecToTimeString(_time, csRecordLength);
    metaInfo.SetLength(csRecordLength);
    metaInfo.SetRecordTime(startTime);
    metaInfo.SetRecordDate(startDate);

    CString csProjectPrefix = m_csLrdName;
    ASSISTANT::GetName(csProjectPrefix);
    ASSISTANT::GetPrefix(csProjectPrefix);
    CString csSgmlPrefix;
    StringManipulation::TransformForSgml(csProjectPrefix, csSgmlPrefix);

    // Create Lrd file
    CreateLrdFile(iSlideWidth, iSlideHeight, metaInfo, csSgmlPrefix);

    // Create Lep file
    if (CStudioApp::IsLiveContextMode()) {
        CLepSgml *pLepFile = CreateLepFile(_time, iSlideWidth, iSlideHeight, metaInfo, csSgmlPrefix);
        if (bReturnLep)
            return pLepFile;
        else {
            if (pLepFile != NULL)
                delete pLepFile;
            return NULL;
        }
    }

    return NULL;
}

void ASSISTANT::Project::SaveImages(bool bUseThread)
{  
   // If the SaveImages is called by StopRecording 
   // no thread should be used to save images
   if (!bUseThread)
   {
      // If there is a thread already running, wait up to 
      // 10 seconds for his determinstion
      if (!m_bWriteFinished)
      {
         int iCount = 0;
         while (iCount < 100 && !m_bWriteFinished)
         {
            Sleep(100);
            ++iCount;
         }
      }

      while (!m_imageWriteQueue.IsEmpty())
      {
         WindowsImage *pImage = m_imageWriteQueue.RemoveHead();
         if (pImage)
         {
            pImage->WriteImage();
         }
      }
      m_bWriteFinished = true;
   }
   else if (m_bWriteFinished)
   {
      // There is no running thread.
      m_bWriteFinished = false;
      CWinThread *pWinThread = AfxBeginThread(SaveImagesThread, this, THREAD_PRIORITY_BELOW_NORMAL);
   }
   // else: the thread is already running
}


UINT ASSISTANT::Project::SaveImagesThread(LPVOID pParam)
{
   ASSISTANT::Project *pProject = (ASSISTANT::Project *)pParam;

   while (!pProject->m_imageWriteQueue.IsEmpty())
   {
      WindowsImage *pImage = pProject->m_imageWriteQueue.RemoveHead();
      if (pImage)
         pImage->WriteImage();
   }

   pProject->m_bWriteFinished = true;

   return 1;
}

void ASSISTANT::Project::AppendImageToWriteQueue(WindowsImage *pImage)
{
   m_imageWriteQueue.AddTail(pImage);
}

// Functions used by pages

void ASSISTANT::Project::ClearPasteBuffer()
{
   if (pasteBuffer_.GetSize() > 0) 
   {
      for (int i = 0; i < pasteBuffer_.GetSize(); ++i) 
      {
         delete pasteBuffer_[i];
      }
      pasteBuffer_.RemoveAll();
   }
}

void ASSISTANT::Project::AddObjectToPasteBuffer(DrawObject *object)
{
   pasteBuffer_.Add(object);
}

void ASSISTANT::Project::GetPasteBuffer(CArray<DrawObject *, DrawObject *> &pasteBuffer)
{
   pasteBuffer.RemoveAll();
   pasteBuffer.Append(pasteBuffer_);
}

bool ASSISTANT::Project::IsPasteBufferEmpty()
{
   return (pasteBuffer_.GetSize() == 0);
}

void ASSISTANT::Project::UpdateVideoThumb()
{
   ThumbnailData
      thumb;
   
   if (!recording || !isVideoEnabled_)
   {
      ASSISTANT::Project::thumbInfo = NULL;
      ASSISTANT::Project::thumbData = NULL;
   }
   else
   {
      try 
      {
         if (avgrabber_->getEnableVideoThumbnail())
         {
            avgrabber_->getVideoThumbnailData(&thumb);
            ASSISTANT::Project::thumbInfo = (BITMAPINFO *)thumb.info;
            ASSISTANT::Project::thumbData = thumb.data;
         }
      }
      catch (exception &e)
      {
         MessageBoxA(NULL, e.what(), "UpdateVideoThumb", MB_OK | MB_TOPMOST);
      }
   }
}



/*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************/

ASSISTANT::ProjectMetadata::ProjectMetadata()
{
   TCHAR
      buf[100];
   unsigned long
      bufSize = 100;
   
   title_ = _T("");
   if (GetUserName(buf, &bufSize))
   {
      author_ = buf;
      creator_ = buf;
   }
   else
   {
      author_ = _T("");
      creator_ = _T("");
   }
}

ASSISTANT::ProjectMetadata::~ProjectMetadata()
{
   title_.Empty();
   author_.Empty();
   creator_.Empty();
   keywords_.Empty();
}


void ASSISTANT::ProjectMetadata::SetTitle(LPCTSTR title)
{
   title_ = title;
}

void ASSISTANT::ProjectMetadata::SetAuthor(LPCTSTR author)
{
   author_ = author;
}

void ASSISTANT::ProjectMetadata::SetCreator(LPCTSTR creator)
{
   creator_ = creator;
}

void ASSISTANT::ProjectMetadata::SetKeywords(LPCTSTR szKeywords)
{
   keywords_ = szKeywords;
}

void ASSISTANT::ProjectMetadata::AddKeyword(LPCTSTR szKeyword)
{
   if (!keywords_.IsEmpty())
      keywords_ += _T(";");
   keywords_ += szKeyword;
}

void ASSISTANT::ProjectMetadata::Write(CFileOutput *pLapFile)
{
   CString csSgmlLine;
   CString csLine;

   csLine = title_;
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   csLine.Format(_T("      <title>%s</title>\n"), csSgmlLine);
   pLapFile->Append(csLine);

   csLine = author_;
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   csLine.Format(_T("      <author>%s</author>\n"), csSgmlLine);
   pLapFile->Append(csLine);

   csLine = creator_;
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   csLine.Format(_T("      <creator>%s</creator>\n"), csSgmlLine);
   pLapFile->Append(csLine);

   CStringArray arKeywords;
   ASSISTANT::CreateListFromString(keywords_, arKeywords);
   for (int i = 0; i < arKeywords.GetSize(); ++i)
   {
      csLine = arKeywords[i];
      StringManipulation::TransformForSgml(csLine, csSgmlLine);
      csLine.Format(_T("      <keyword>%s</keyword>\n"), csSgmlLine);
      pLapFile->Append(csLine);
   }
   arKeywords.RemoveAll();
}


void ASSISTANT::Project::ShowFontError() {
    bool bShowFontWarning = true;
    CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
    if (pMainFrame)
        bShowFontWarning = pMainFrame->GetShowMissingFontsWarning();

    if (bShowFontWarning) {
        CWnd *pMainWnd = CMainFrameA::GetCurrentInstance(); //AfxGetMainWnd();

        CMissingFontsDlg dlgMissingFonts(pMainWnd);

        CStringArray arFontNames;
        CStringArray arErrorPages;
        CStringArray arErrorTypes;
        fontList_->FillErrorList(arFontNames, arErrorPages, arErrorTypes);

        for (int i = 0; i < arFontNames.GetSize(); ++i)
            dlgMissingFonts.AddMissingFont(arFontNames[i], arErrorPages[i], arErrorTypes[i]);

        dlgMissingFonts.DoModal();

        fontList_->DeleteErrorPages();
    }
}

HRESULT ASSISTANT::Project::GetGridSettings(UINT &snapChecked, UINT &spacingValue, UINT &displayGrid)
{
   snapChecked = snapChecked_;
   spacingValue = spacingValue_;
   displayGrid = displayGrid_;

   return S_OK;
}

bool ASSISTANT::Project::SetGridSettings(UINT snapChecked, UINT spacingValue, UINT displayGrid)
{
   snapChecked_ = snapChecked;
   spacingValue_ = spacingValue;
   displayGrid_ = displayGrid;
   
   return true;
}

void ASSISTANT::Project::ReadGridSettings()
{
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
   DWORD
      subValues;
   TCHAR
      entryName[100];
   TCHAR
      entryValue[512];
   DWORD
      entryNameLength = 100,
      entryValueLength = 512,
      dwType;

   // Read Grid options
   CString csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\Grid");

   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_READ, &hKeyUser);
   if (regErr == ERROR_SUCCESS )
   {
      regErr = RegQueryInfoKey(hKeyUser, NULL, NULL, NULL, NULL, NULL, NULL, &subValues, NULL, NULL, NULL, NULL);
      
      if ( regErr == ERROR_SUCCESS )
      {
         for (int i = 0; i < subValues; i++) 
         {             
            entryNameLength = 100;
            entryValueLength = 512;
            if (RegEnumValue(hKeyUser, i, entryName, &entryNameLength, NULL, &dwType, (BYTE *)entryValue, &entryValueLength) == ERROR_SUCCESS)
            {
               if (dwType == REG_SZ)
               {
                  CString csEntry = entryName;
                  if (csEntry == _T("SNAP"))
                  {
                     snapChecked_ = _ttoi((TCHAR *)entryValue);
                  }
                  else if (csEntry == _T("SPACING"))
                  {
                     spacingValue_= _ttoi((TCHAR *)entryValue);
                  }
                  else if (csEntry == _T("DISPLAY"))
                  {
                     displayGrid_ = _ttoi((TCHAR *)entryValue);
                  }
               }
            }
         }
      }
      RegCloseKey(hKeyUser);
   }
}

HRESULT ASSISTANT::Project::WriteGridSettings()
{
   HKEY 
      hKeyUser; 
	LONG		
      regErr;
   DWORD
      dwDummy;
   DWORD
      entryNameLength = 100,
      entryValueLength = 512;

   // Read Grid settings
   CString csSubKey = ASSISTANT_REGISTRY;
   csSubKey += _T("\\Grid");

   regErr = RegOpenKeyEx(HKEY_CURRENT_USER, csSubKey, NULL, KEY_ALL_ACCESS, &hKeyUser);
   
   if ( regErr != ERROR_SUCCESS )
   {
      regErr = RegCreateKeyEx(HKEY_CURRENT_USER, csSubKey,
                              NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyUser, &dwDummy); 
      
      if ( regErr != ERROR_SUCCESS )
         return E_FAIL;
   }

   if (regErr == ERROR_SUCCESS )
   {
      TCHAR value[512];

      _stprintf(value, _T("%d"), snapChecked_);
      regErr = RegSetValueEx(hKeyUser, _T("SNAP"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));
      
      _stprintf(value, _T("%d"), spacingValue_);
      regErr = RegSetValueEx(hKeyUser, _T("SPACING"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));

      _stprintf(value, _T("%d"), displayGrid_);
      regErr = RegSetValueEx(hKeyUser, _T("DISPLAY"), 0, REG_SZ, 
                             (BYTE *)value, _tcslen(value)*sizeof(TCHAR));

      RegCloseKey(hKeyUser);
   }

   return S_OK;
}

void ASSISTANT::Project::DoPureScreenGrabbing(bool bDoPureScreenGrabbing)
{
   m_bDoPureScreenGrabbing = bDoPureScreenGrabbing;
};

bool ASSISTANT::Project::IsPureScreenGrabbing()
{
   return m_bDoPureScreenGrabbing;
};
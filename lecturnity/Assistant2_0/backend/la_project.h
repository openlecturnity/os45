/*********************************************************************

 program : la_project.h
 authors : Gabriela Maass
 date    : 18.06.2002
 desc    : 

 **********************************************************************/

#ifndef __ASSISTANT_PROJECT__
#define __ASSISTANT_PROJECT__

#ifdef WIN32
#  pragma warning( disable: 4786)
#endif

#include <gdiplus.h>
#include "la_imagelist.h"

#include "la_document.h"
#include "la_thumbs.h"
#include "la_font.h"
#include "la_presentationInfo.h"

#include "avgrabber.h"
#include "NewCaptureRectChooser.h"
#include "..\RecordingsView.h"
#include "..\AudioLevelWarning.h"
#include "..\TwoGBCountdown.h"
#include "..\TwoGBWarning.h"
#include "..\DiscSpaceCountdown.h"
#include "..\DiscSpaceWarning.h"

#include "LmdFile.h"    // filesdk
#include "SingleProgressDialog.h" // lsutl32

namespace ASSISTANT {
   
   // Structure to transmit data to SG-Thread
   typedef struct 
   {
      const _TCHAR *clipName;
      CString audioName;
      CString ladInfo;
      AvGrabber *avgrabber;
   } SGInfo; 

   typedef struct {
       HANDLE hProcess;
       CWnd *pMainWnd;
   } ThreadParam; 

   struct WaitForClickInfoStruct {
       AvGrabber *pAvGrabber;
       CSingleProgressDialog *pProgress;
       HANDLE hProgressEvent;
   };

   // Class to manage project metadata
   class ProjectMetadata
   {
      public:
         ProjectMetadata();
         ~ProjectMetadata();

         void SetTitle(LPCTSTR title);
         void SetAuthor(LPCTSTR author);
         void SetCreator(LPCTSTR creator);
         void SetKeywords(LPCTSTR szKeywords);
         void AddKeyword(LPCTSTR szKeyword);

         CString &GetTitle() {return title_;}
         CString &GetAuthor() {return author_;}
         CString &GetCreator() {return creator_;}
         CString &GetKeywords() {return keywords_;}

         void Write(CFileOutput *pLapFile);

      protected:
         CString
            title_,
            author_,
            creator_,
            keywords_;

   };

   class Project 
   {
      public:

      enum closeType 
      {
         CLOSE = 0,
         EXIT  = 1,
         NEW   = 2,
         OPEN  = 3
      };

      enum insertType
      {
         BEGIN   = 0,
         CURRENT = 1,
         END     = 2
      };

      enum type 
      {
         Record = 0,
         Pause  = 1,
         Stop   = 2
      };

      enum threadStatus
      {
         STOPPED  = 0,
         RUNNING  = 1,
         PENDING  = 2
      };


      Project(int doWithSG, bool bFreezeImages, 
          bool bFreezeMaster, bool bFreezeDocument,
          const _TCHAR *tszFixedLrdName);

      ~Project();

      // Initialize project
      void InitVariables();
      void ReleaseVariables();
      void ReleaseDocumentVariables();
      void UpdateNewProject();

      // Activate and deactivate project
      void ActivateAudioAndVideo();
      void DeActivateAudioAndVideo();


      /*****************************************************
       ********** Functions called from main.cpp ***********
       *****************************************************/

      // Functions called from main.cpp concerning project
#ifndef _STUDIO
      bool Open(LPCTSTR filename, CProgressDialogA *pProgress);
#endif
      bool LoadDocumentsAndPresentations();
      bool CloseDocumentsAndPresentations();
      bool SaveModified();
      bool Save(LPCTSTR szProjectName, bool bSaveDocuments=true);
      HRESULT Copy(LPCTSTR szCopyFilename, CProgressDialogA *pProgress);
	   void ShowAudioWarning(bool bShowAudioWarning);
		 
      // Functions called from main.cpp concerning active document
      void NewDocument();
      bool CloseDocument(bool bAskForSave = true);
      void AddDocument(LPCTSTR filename, int pos = CURRENT, bool showProgress = true, bool bDoLoad = false);
      bool SaveDocument();
      bool SaveDocumentAs();
      bool ImportPPT(LPCTSTR szPowerPointName, bool bAskForLsd, bool bDoRecord);
      void GetLastSavedDocument(CString &csLastSavedDocument);
      void GetSourceDocumentPaths(CArray<CString, CString>& aPaths);
      void GetRecordingDocumentPaths(CArray<CString, CString>& aPaths);
      void AddRecording(CString &csFilename);
      PresentationInfo *GetLastRecording();

      // Functions called from main.cpp concerning actual page
      void ChangeLineColor(COLORREF clrLine, int iTransparency=255);
      void ChangeFillColor(COLORREF clrFill, int iTransparency=255);
      void ChangeTextColor(COLORREF clrText, int iTransparency=255);
      void ChangeFillColor();
      void ChangeLineWidth(double dLineWidth);
      void ChangeLineStyle(int iLineStyle);
      void ChangeArrowStyle(int iArrowConfig, LPCTSTR szArrowStyle);
      void ChangeFontFamily(LPCTSTR szFontFamily);
      void ChangeFontSize(int iFontSize);
      void ChangeFontWeight(bool bBold);
      void ChangeFontSlant(bool bItalic);
      void CutObjects();
      void DeleteObjects(bool bDoUndo = true);
      void CopyObjects();
      void PasteObjects();
      void SelectAllObjects();
      void Undo();
      void SelectObjects(CRect &rcSelection);
      bool HasSelectedObjects();
      bool HasLoadedDocuments();
      bool IsSgOnlyDocument();
      bool HasChanged();
      void SelectSingleObject(CRect &rcSelection);
      void UnSelectObjects();
      void RaiseObjects();
      void LowerObjects();
      void ActivatePage(bool rebuild=false);
      void DeActivatePage();

      
      bool GetPageProperties(UINT &nPageWidth, UINT &nPageHeight, COLORREF &clrBackground);
      bool SetPageProperties(UINT nPageWidth, UINT nPageHeight, COLORREF clrBackground);

	  bool GetPagePropertiesEx(CRect rcW, UINT &nPageWidth, UINT &nPageHeight, UINT &nOffX, UINT &nOffY);

	  void SetFullScreenOn(bool bState);
      // Container functions (called from document structure)
      void EscHotkey(bool set);  
      void UpdateSgModul();
      void ExtractSgOptions(CString &settingString);

      bool SetKeywords(UINT nContainerId, LPCTSTR szKeywords);
      HRESULT GetKeywords(UINT nContainerId, CString &csKeywords);

      // Folder functions
      void InsertNewFolder();

      // Page functions
      void InsertNewPage();
      void DuplicatePage();
      void NextPage();
      void PreviousPage();
      void FirstPage();


      bool IsRecording() {return recording;}
      bool IsRecordingPaused() {return paused;}
      bool GrabAtFixedPos() {return grabAtFixedPos_;}
      CRect &GetSGRectangle() {return m_rcSgRectangle;}


      // video functions
      void ReadVideoSettings(bool bShowWarning, bool bSet = false);
      HRESULT WriteVideoSettings();

      void RetrieveCodecs(bool bShowWarning);
      
      HRESULT GetVideoSettings(bool &bRecordVideo, bool &bShowMonitor, bool &bUseAlternativeSync,
                               bool &bSetAvOffset, int &nAvOffset);
      HRESULT GetVideoParameter(bool &bUseVFW, CStringArray &arVFWSources, UINT &nVFWIndex,
                                CStringArray &arWDMSources, UINT &nWDMIndex, 
                                UINT &nXResolution, UINT &nYResolution, UINT &nBitPerColor, 
                                CArray<float, float> &arFramerates, UINT &nFrameRateIndex);
      HRESULT GetVideoResolution(UINT &nXResolution, UINT &nYResolution);
      HRESULT GetColorDepth(UINT &nBitPerColor);
      HRESULT EnableVideoRecording(bool bEnable, bool bCheckCodecs);
      HRESULT ShowVideoMonitor(bool bShow);
      HRESULT CheckInstalledVideoCodecs();
      HRESULT SetAlternativeAvSync(bool bUseAlternativeSync);
      HRESULT SetUseAvOffset(bool bUseAvOffset);
      HRESULT SetAvOffset(int iAvOffset);
      HRESULT SetUseVFW(bool bUseVFW);
      HRESULT ChangeWdmDevice(CString &csWdmDevice);
      
      HRESULT ShowVideoFormat();
      HRESULT ShowVideoSource();
      HRESULT ShowVideoDisplay();

      void SetVideoFramerate(float fVideoFramerate);


      // wdm functions
      void InitWdmDevices();
      void DeleteWdmDevices();
      void InitWdmInterfaces();
      void DeleteWdmInterfaces();
      int GetVideoDeviceNumber();
      bool IsWdmDeviceInList();
      bool SetWdmDevice(bool bShowWarning); 

      bool HasWdmSettings();
      bool HasWdmSource();
      bool HasWdmFormat();
      bool HasWdmDv();

      void ShowWdmSettings();
      void ShowWdmSource();
      void ShowWdmFormat();
      void ShowWdmDv();

      //void RetrieveCodecs();
      
      // codec functions
      HRESULT GetCodecSettings(CStringArray &arCodecs, UINT &nCodecIndex, UINT &nQuality, 
                               bool &bUseKeyframes, UINT &nKeyframeDistance,
                               bool &bUseDatarateAdaption, UINT &nDatarate,
                               bool &bHasAbout, bool &bHasConfigure);
      HRESULT SetCodec(CString &csCodec, bool bShowWarning);
      HRESULT SetUseKeyframes(bool bUseKeyframes);
      HRESULT SetKeyframes(UINT nKeyframes);
      HRESULT SetUseDatarate(bool bUseDatarate);
      HRESULT SetDatarate(UINT nDatarate);
      HRESULT SetCodecQuality(UINT nQuality);
      bool QualityPossible();
      bool KeyframesPossible();
      bool DataratePossible();
      bool CodecHasAbout();
      bool CodecCanBeConfigured();

      void SetVideoCodec(bool bShowWarning);
      
      void ReadSgHotkeys();
      HRESULT WriteSgHotkeys();
      void ReadSgRegistrySettings();
      HRESULT WriteSgRegistrySettings();
      HRESULT GetMouseEffects(bool &bMouseVisual, bool &bMouseAcoustic);
      HRESULT SetMouseEffects(bool bMouseVisual, bool bMouseAcoustic);
      HRESULT GetFramerateSettings(bool &bStaticFramerate, 
                                   float &fStaticFramerate, float &fAdaptiveFramerate,
                                   UINT &nWidth, UINT &nHeight);
      HRESULT SetFramerateSettings(bool bStaticFramerate, 
                                   float fStaticFramerate, float fAdaptiveFramerate,
                                   UINT nWidth, UINT nHeight);
      HRESULT GetRegionSettings(bool &bRegionFixed, UINT &nFixedX, UINT &nFixedY, 
                                UINT &nFixedWidth, UINT &nFixedHeight);

      HRESULT SetRegionSettings(bool bRegionFixed, UINT nFixedX, UINT nFixedY, 
                                UINT nFixedWidth, UINT nFixedHeight);
      
      HRESULT GetSgHandling(bool &bClearAnnotation, bool &bReportDroppedFrames, bool &bBorderPanning,
                            bool &bDisableAcceleration, bool &bQuickCapture, bool &bSuspendVideo);
      
      HRESULT SetSgHandling(bool bClearAnnotation, bool bReportDroppedFrames, bool bBorderPanning,
                            bool bDisableAcceleration, bool bQuickCapture, bool bSuspendVideo);
      
      HRESULT GetSgHotkeys(AVGRABBER::HotKey &hkStartStop, AVGRABBER::HotKey &hkReduce, AVGRABBER::HotKey &hkMinimize);
      HRESULT SetSgHotkeys(AVGRABBER::HotKey &hkStartStop, AVGRABBER::HotKey &hkReduce, AVGRABBER::HotKey &hkMinimize);

      HRESULT GetSgStartStopHotKey(AVGRABBER::HotKey &hkStartStop);
      HRESULT SetSgStartStopHotKey(AVGRABBER::HotKey &hkStartStop);

      HRESULT TestHotKey(AVGRABBER::HotKey &hkToTest);
      HRESULT ActivateStartStopHotKey(bool bActivate=true);

      HRESULT GetCodecSettings(CStringArray &arCodecList, UINT &nCodecIndex, bool &bHasAbout, bool &bHasConfigure,
                               bool &bSupportsQuality, UINT &nCodecQuality, 
                               bool &bSupportsKeyframes, bool &bUseKeyframes, UINT &nKeyframeDistance, 
                               bool &bSupportsDatarate, bool &bUseDatarte, UINT &nDatarate);
      HRESULT SetCodecSettings(CString &csCodec, UINT nCodecQuality, 
                               bool bUseKeyframes, UINT nKeyframeDistance, bool &bUseDatarte, UINT nDatarate);

      
      HRESULT SetSgCodec(CString &csCodec, bool bDisplayWarning);
      bool SgCodecHasAbout();
      bool SgCodecCanBeConfigured();

      HRESULT ShowSgCodecAbout();
      HRESULT ShowSgCodecConfigure();

      // sg functions
      HRESULT StartSg(CRect &rcSelection);
      HRESULT StopSg();
      HRESULT StartDefineArea();

      int SetAnnotationMode(UINT nCurrentTool, double dLineWidth, int iLineStyle, int iArrowStyle, 
                            COLORREF clrLine, COLORREF clrFill, bool bIsFilled, COLORREF clrText);
      int ResetAnnotationMode();
      
      void SetSgCodecNumber(bool bDisplayWarning);
      void SetSgCodec(bool bDisplayWarning=true);
      void GetSgCodecList(CString &codecList);
      void GetSgCodecName(CString &codecName);
      void RetrieveSgCodecs(); 
      
      void CreateSgCodecInfo();
      void DeleteSgCodecInfo();

      /*****************************************************
       *****************************************************
       *****************************************************/
      
      // metadata functions
      HRESULT GetMetadata(CString &csTitle, CString &csAuthor, 
         CString &csProducer, CString &csKeywords);
      HRESULT SetMetadata(CString &csTitle, CString &csAuthor, 
         CString &csProducer, CString &csKeywords);

      // Project functions
      void WriteLapFile(LPCTSTR filename, bool bCopyProject=false);
      void ChangeProjectName(const _TCHAR *newProjectName, bool changeMetadata = false);
      void SetProgressCancelled() {m_bProgressCancelled = true;}
      bool GetProgressCancelled() {return m_bProgressCancelled;}
      void SetProjectName(const _TCHAR *name);
      CString &GetProjectName() {return projectName_;}
      CString &GetPrefix();
      CString &GetProjectPath() {return projectPath_;}
#ifndef _STUDIO
      void ReadFilesInfo(SgmlElement *root, CProgressDialogA *pProgress);
#endif
      bool PageIsChanged();
      void SetChanged(int changed) {projectAction_ = changed;}
      virtual int GetNbrOfPages();
      virtual int GetPageNumber(UINT nId, bool &bFound); 
      void OpenFromRegistry();
      void SaveToRegistry();

      // container functions 
      GenericObject    *GetObject(uint32 id);
      GenericContainer *GetParentContainer(uint32 id);
      uint32 GetObjectID() {return ++objectID_;}

      void ActivatePreviousPage(bool _jump=false);
      void ActivateNextPage(bool _jump=false);
      bool NextPageAvailable();
      bool PreviousPageAvailable();

      bool LoadDocument(UINT nDocumentId);
      bool RenameContainer(UINT nContainerId, LPCTSTR szContainerName);
      bool SelectContainer(UINT nContainerId);
      bool DeleteContainer(UINT nContainerId);
      bool SaveContainer(UINT nContainerId, LPCTSTR szFileName);
      bool MoveContainer(UINT nMoveContainerId, UINT nContainerId, bool bInsertInChapter, bool bInsertFirst);

      // Metadata functions
      void ReadMetadata(SgmlElement *root);
      void WriteMetadata();
      void OpenCloseMetadata(GenericContainer *container, const _TCHAR *filename);

      // document functions
      bool SaveChangedDocuments();
      Document *GetActiveDocument();
      void SetActiveDocument(uint32 id);
      void SetNoActiveDocument();
      bool RemoveDocumentFromList(uint32 id);
      bool IsDocumentInProject(LPCTSTR documentName);
      Document *GetParentDocument(GenericObject* obj);
      bool ContainsDocument(LPCTSTR documentName);

      // page functions
      Page *GetActivePage();
      void InsertNewPage(Page* page);
      int  GetMaxWidth();
      int  GetMaxHeight();

      // folder functions
      Folder   *GetActiveFolder();

      // Drawing and update whiteboard
      virtual void DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC);
      virtual void DrawThumbWhiteboard(CRect rcWhiteboard, CDC *pDC){return;};

      void ReBuildStructureTree(CDocumentStructureView *pDocStructTree);
      void ReBuildRecordingTree(CRecordingsView *pRecordingTree);

      // common record functions
      HRESULT  StartRecording();
      HRESULT  StopRecording();
      int  PauseRecording();
      int  UnPauseRecording();
      void ResetRecording();
      void SortRecordingsList(int nColumn, BOOL bAscending);      
      
      void ReadAudioSettings(bool bSet = false);
      HRESULT WriteAudioSettings();
      HRESULT GetAudioDevices(CStringArray &arDevices, UINT &nDevice);
      HRESULT GetSampleWidths(UINT nAudioDevice, CUIntArray &arSampleWidths, UINT &nSampleWidth);
      HRESULT GetSampleRates(UINT nAudioDevice, CUIntArray &arSampleRates, UINT &nSampleRate);
      HRESULT GetAudioSources(CStringArray &arSources, UINT &nSource);
      
      bool RecordingIsPossible();
      double GetAudioLevel();
      UINT GetMixerVolume();
      HRESULT SetMixerVolume(UINT nMixerVolume);

      void CheckDiskSpace(bool bReset);
      void CheckAudioLevel(bool bReset);

      int GetRecordingTime();
      bool SetRecordPath(CString &path);
      CString &GetRecordPath();
      CString &GetDataPath();
      CFileOutput *GetObjFp() {return objFp_;}
      FILE *GetEvqFp() {return evqFp_;}
      CLepSgml *UpdateProjectFiles(int _time, bool bReturnLep=false);
      void SaveImages(bool bUseThread = true);
      void AppendImageToWriteQueue(WindowsImage *pImage);

      // Common audio and video functions
      void InitAudioVideo();
      void ReleaseAudioVideo();

      // audio functions
      bool ActivateAudioDevice();
      bool SetAudioDevice(UINT nAudioDevice);
      bool SetSampleWidth(UINT nSampleWidth);
      bool SetSampleRate(UINT nSampleRate);
      bool SetAudioSource(UINT nAudioSource);
      UINT GetAudioSource();

      void AdjustAudioParameter();
      char *GetAudioDevice();
      bool IsAudioDeviceSet() {return isAudioDeviceSet_;}
      int GetAudioBitRate() {return audioBitrate_;}
      int GetAudioSampleRate() {return audioFramerate_;}

	  //Grid functions

	  bool SetGridSettings(UINT snapChecked, UINT spacingValue, UINT displayGrid);
	  HRESULT GetGridSettings(UINT &snapChecked, UINT &spacingValue, UINT &displayGrid);
	  HRESULT WriteGridSettings();
	  void ReadGridSettings();

      // Audio functions
      AvGrabber *GetAvGrabber() {return avgrabber_;}

      // mixer functions
      int GetMixerSource() {return mixerSource_;}
      void GetMixerSourceName(CString &mixerSourceName);
      AudioMixer *GetAudioMixer() {return audioMixer_;}
      void SetMixerSource(int mixerSource) {mixerSource_ = mixerSource;}
      int GetAudioVolume() {return audioVolume_;}
      void SetAudioVolume(int audioVolume) {audioVolume_ = audioVolume;}
      void GetMixerSourceList(CString &mixerSourceList);
      void CreateMixerSources();
      void DeleteMixerSources();

      // video functions
      bool SetVideoDevice(); 
      void UpdateVideoPage();
      void UpdateVideoHelp();
      void GetVideoDeviceList(CString &videoDeviceList);
      void GetVideoDeviceName(CString &videoDeviceName);

      // codec functions
      void AdjustCodecParameter();
      void UpdateCodecPage();
      void SetCodecNumber(bool bShowWarning);
      void GetCodecList(CString &codecList);
      void GetCodecName(CString &codecName);
      void CreateCodecInfo();
      void DeleteCodecInfo();
      void ShowCodecConfigure();
      void ShowCodecAbout();

      // Presentation-list functions
      PresentationInfo *GetActivePresentation() {return activePresentation_;}
      HRESULT RemovePresentation(PresentationInfo *pPresentation);
      HRESULT DeletePresentation(PresentationInfo *pPresentation);

      // image functions
      ImageList *GetImageList() {return imageList_;}
      void FreezeImages(bool bFreezeImages);
      bool DoFreezeImages() {return m_bFreezeImages;}
      void FreezeMaster(bool bFreezeMaster);
      bool DoFreezeMaster() {return m_bFreezeMaster;}
      void FreezeDocument(bool bFreezeDocument);
      bool DoFreezeDocument() {return m_bFreezeDocument;}

      FontList *GetFontList() {return fontList_;}

      // Functions only needed for AOF features
      void UseAppPlay() {useAppPlay = true;}
      void UseJedas() {useJedas = true;}

      // Functions to handle cutted/copied objects for paste
      void ClearPasteBuffer();
      void AddObjectToPasteBuffer(DrawObject *object);
      void GetPasteBuffer(CArray<DrawObject *, DrawObject *> &pasteBuffer);
      bool IsPasteBufferEmpty();

      void UpdateVideoThumb();
      
      bool IsAudioDeviceAvailable() {return audioDeviceCount_ > 0;}
      

      // static functions
      
      // Initialize audio and video settings
      static void InitAvGrabber();
      static void DeleteAvGrabber();

      // static variables
      static Project 
         *active;

      /*/
      static Tcl_TimerToken
         audioTimer,
         videoTimer,
         durationTimer;
         /*/

      static int
         audioIntervall,
         videoIntervall,
         durationIntervall;

      static char
         *thumbData;
      static BITMAPINFO
         *thumbInfo;
      static bool 
         audioIsInitialized,
         volumeSliderIsActive,
         optionsDialogIsActive;

      int pageCount;
      
      AVGRABBER::HotKey m_hkStartStop;
      AVGRABBER::HotKey m_hkReduce;
      AVGRABBER::HotKey m_hkMinimize;

   private:      
      void ShowFontError();
      HRESULT  StartRecording(CString &ssAudioName, CString &ssLadInfo);
	  static UINT StartDefineAreaThread(LPVOID pParam);
      static UINT WaitForInstallationFinished(LPVOID pParam);
      static UINT WaitClickElementInfoThread(LPVOID pParam);

      void ShowTwoGBCountdown(int iSecondsLeft);
      void HideTwoGBCountdown();
      void ShowTwoGBWarning(int iMinutesLeft);
      void HideTwoGBWarning();
      void ShowDiscSpaceCountdown(int iSecondsLeft);
      void HideDiscSpaceCountdown();
      void ShowDiscSpaceWarning(int iMinutesLeft);
      void HideDiscSpaceWarning();

      void SetNewActiveDocument(UINT nIndex);

      void CreateLmdFile(int _time);
      void CreateLrdFile(int iSlideWidth, int iSlideHeight, 
                        CMetaInformation &metaInfo,
                        CString csSgmlPrefix);
      CLepSgml *CreateLepFile(int iTotalRecordingTime, 
                             int iSlideWidth, int iSlideHeight, 
                             CMetaInformation &metaInfo,
                             CString &csSgmlFilePrefix);

      static UINT SaveImagesThread(LPVOID pParam);

      HRESULT GenerateUniqueRecordingTitle(CString &csRecordTitle);
      HRESULT CreateRecordingNameFromPrefix(CString csRecordPrefix, CString &csRecordName);
      HRESULT CreateLadInfo(char **pszLadInfo);

   private:
      enum
      {
         PAGE_WIDTH = 720,
         PAGE_HEIGHT = 540,
         BG_COLOR = RGB(255, 255, 255)
      };

      static AvGrabber 
         *avgrabber_;

      static AudioDevice
         **audioDevices_;
      static int
         audioDeviceCount_;

      static TCHAR
         **videoDeviceNames_;
      static int
         videoDeviceCount_;
      
      static TCHAR
         **wdmDeviceNames_;
      static int
         wdmDeviceCount_;

      
      int mediaTypeSize_;
      int videoHeaderSize_;
      char *mediaTypeData_;
      char *videoHeaderData_;

      friend class Document;

      // buffer to store cutted or copied objects 
      CArray<DrawObject *, DrawObject *>
         pasteBuffer_;
      
      // Common information

      int
         doWithSG_;

      // project information
      ProjectMetadata
         *metadata_;

      CString
         projectName_,
         projectPath_,
         projectPrefix_,
         dataPath_,
         recordPath_;
      int 
         projectAction_;

      // Project substructures
      CArray<Document *, Document *> 
         documents_;
      int
         activeDocument_;

      CArray<PresentationInfo *, PresentationInfo *>
         presentations_;
      PresentationInfo
         *activePresentation_;

      CString 
         toolList_;
      CString 
         guiList_;
      unsigned long hotKeyThread_;

      // Variables needed for whiteboard recording
      // Pointer to eventqueue and object file
      CFileOutput
         *objFp_;
      HANDLE 
         m_hObjFile;
      FILE
         *evqFp_;
      // Write lad or wav?
      bool 
         isEncrypted_;
      bool
         isFullScreenAtStart_;

      // Variables to store audio properties
      int 
         audioDevice_;
      int
         audioFramerate_,
         audioBitrate_,
         audioChannels_;
      bool
         isAudioDeviceSet_;

	  // Variables to store grid properties

	  int
		  snapChecked_;
      int
		  spacingValue_;
      int
		  displayGrid_;


      // Variables to store mixer properties
      AudioSource
         **mixerSources_;
      int
         mixerSourceCount_;
      int
         mixerSource_;
      AudioMixer
         *audioMixer_;
      int
         audioVolume_;

      // Variable to find out which kind of video is used
      bool useVfW_;

      // Variables to store video properties
      int
         videoDevice_;
      bool 
         isVideoDeviceSet_;
      bool
         isVideoEnabled_,
         isAdvancedSyncSet_,
         hasAVOffset_,
         isMonitorEnabled_;
      long
         avOffset_;
      bool
         hasFormatDialog_,
         hasSourceDialog_,
         hasDisplayDialog_;
      float
         videoFramerate_;
      
      // Variables to store WDM properties
     
      bool
         isWdmDeviceSet_;
      CString
         wdmDevice_;
      int
         wdmInterfaceCount_;
      WDM_INTERFACE 
         *wdmInterfaces_;

      // Variables to store codec properties

      CodecInfo
         **codecInfo_;
      int 
         codecCount_;
      int 
         codecNumber_;
      CString
         videoCodec_;
      bool 
         isCodecEnabled_,
         hasAboutDialog_,
         hasConfigureDialog_,
         useKeyframes_,
         useDatarate_,
         bSupportsQuality_, 
         bSupportsKeyframes_, 
         bSupportsDatarate_;
      int
         videoQuality_,
         keyframeDist_,
         videoDatarate_;


      // Variables to store SG codec properties

      bool isSgInPackage_;
      bool isSgEnabled_;

      CodecInfo
         **sgCodecInfo_;
      int 
         sgCodecCount_;
      int 
         sgCodecNumber_;
      CString
         sgCodec_;
      bool 
         isSgCodecEnabled_,
         hasSgAboutDialog_,
         hasSgConfigureDialog_,
         sgUseKeyframes_,
         sgUseDatarate_,
         bSgSupportsQuality_, 
         bSgSupportsKeyframes_, 
         bSgSupportsDatarate_;
      int
         sgQuality_,
         sgKeyframeDist_,
         sgDatarate_;


      CString
         lastDocumentPath,
         lastImagePath;
      Page
         *lastActivePage;

      CString
         wbGeometry;
      int
         isToolbar,
         isDocview;
      bool
         m_bFreezeImages,
         m_bFreezeMaster,
         m_bFreezeDocument;
      int
         presentationCount,
         clipCount;
      CString
         startTime,
         startDate;
      float
         staticFramerate_,
         adaptiveFramerate_;
      bool 
         hasStaticFramerate_;
      bool
         reportDroppedFrames_;
      bool
         grabAtFixedPos_;
      bool
         deleteAllOnInteraction_;
      bool
         hideWhilePanning_;
      bool
         selectOnlyMainWindow_;
      bool
         noHardwareAcceleration_;
      bool
         quickCapture_;
      bool
         pauseVideo_;
      bool
         mouseAudioEffect_,
         mouseVisualEffect_;
      int
         startAtPosX_,
         startAtPosY_,
         startWithWidth_,
         startWithHeight_;
      CRect m_rcSgRectangle;
      int
         adaptWidth_,
         adaptHeight_;
      //
      ImageList 
         *imageList_;
      bool 
         isAnnotationMode_;
      bool 
         showImportWarnings_;

      FontList *fontList_;

      // ???
      bool
         loadFinished_;
      int 
         objectID_;

      CString 
         nodeName_;
      CString 
         m_ssLastCursor;

      bool 
         m_bProgressCancelled;

      CString
         m_ssLastCopyPath;

      HINSTANCE m_hSmartBoardDll;

      CAudioLevelWarning m_dlgAudioLevelWarning;
      
      CTwoGBCountdown m_dlgTwoGBCountdown;
      CTwoGBWarning m_dlgTwoGBWarning;
      CDiscSpaceCountdown m_dlgDiscSpaceCountdown;
      CDiscSpaceWarning m_dlgDiscSpaceWarning;

      CString m_csLrdName;
      bool m_bFixedTargetName;

      CList<WindowsImage *, WindowsImage *>  
         m_imageWriteQueue;
      bool 
        m_bWriteFinished;
      bool 
         m_bDoPureScreenGrabbing;
   public:
      // list to keep pasted objects
      int
         recCount;
      int 
         lastRecordedTime_;
      bool
         recording,
         paused,
         useAppPlay,
         isScreenGrabbingActive_,
         isSgThreadRunning_;
      PLAYER::Thumbs 
         thumbs_;

      // Variable only needed for AOF features
      bool
         useJedas;   

      LOGFONT
         m_logFont;

      void DoPureScreenGrabbing(bool bDoPureScreenGrabbing = true);
      bool IsPureScreenGrabbing();

   };
}

#endif

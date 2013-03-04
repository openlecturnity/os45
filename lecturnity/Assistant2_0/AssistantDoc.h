// AssistantDoc.h : interface of the CAssistantDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASSISTANTDOC_H__E41E9329_B2BC_4017_BEBA_6918B8FC1EC7__INCLUDED_)
#define AFX_ASSISTANTDOC_H__E41E9329_B2BC_4017_BEBA_6918B8FC1EC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProgressDialog.h"
#include "AssistantView.h"
#include "DocumentStructureView.h"
#include "RecordingsView.h"
#include "backend\la_project.h"

namespace ASSISTANT {

class Project;

}

struct OpenProjectStruct
{
    CAssistantDoc *pDoc;
    ASSISTANT::Project *pProject;
    LPCTSTR   szProjectName;
    CProgressDialogA *pProgress;
    HRESULT hr;
};

class CAssistantDoc : public CDocument
{
public:
    static bool IsDoRegistryProject();

protected: // create from serialization only
	CAssistantDoc();
	DECLARE_DYNCREATE(CAssistantDoc)

public:
   bool HasOpenProject();
   ASSISTANT::Project *GetOpenProject();
   bool HasLoadedDocuments();
   bool ActiveDocumentIsLoaded();
   bool IsSgOnlyDocument();
   bool RecordingIsPossible();
   bool HasActivePage();
   bool PageIsChanged();
   bool PasteBufferIsEmpty();
   void SortRecordingsList(int nColumn, BOOL bAscending); 

// Operations
public:
   HRESULT DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssistantDoc)
	public:
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
   virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
   bool IsFullScreenOn();
	void SetFullScreenOn(bool bState);
	virtual ~CAssistantDoc();
	bool IsGridVisible();
	bool IsSnapEnabled();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

   HRESULT SetCurrentTool(UINT nToolId);
   HRESULT AddNewObject();
   HRESULT ModifyObject();

   HRESULT CutObjects();
   HRESULT DeleteObjects(bool bDoUndo = true);
   HRESULT CopyObjects();
   HRESULT PasteObjects();

   UINT GoToNextPage();
   UINT GoToPreviousPage();
   UINT GoToFirstPage();
   HRESULT SelectContainer(UINT nPadeId);

   HRESULT LoadDocument(UINT nDocumentId);
   HRESULT CloseDocument(bool bAskFor = true);
   HRESULT CloseDocument(UINT nDocumentId);
   HRESULT RemoveDocumentFromList(UINT nDocumentId);
   HRESULT RenameContainer(UINT nContainerId, LPCTSTR szNewContainerName);
   HRESULT DeleteContainer(UINT nContainerId);
   HRESULT SaveContainer(UINT nContainerId, LPCTSTR szFileName);
   HRESULT MoveContainer(CDocumentStructureView *pStructureTree, UINT nMoveContainerId, UINT nContainerId, bool bInsertInChapter, bool bInsertFirst);
   
   HRESULT SelectObjectsInRange(CRect &rcObject);
   HRESULT SelectObjectContainsRange(CRect &rcObject);
   HRESULT SelectAllObjects();
   HRESULT UnSelectObjects();
   bool HasSelectedObjects();

   HRESULT InsertNewDocument(CDocumentStructureView *pStructureTree);
   HRESULT InsertNewChapter(CDocumentStructureView *pStructureTree);
   HRESULT InsertNewPage(CDocumentStructureView *pStructureTree);
   HRESULT DuplicatePage(CDocumentStructureView *pStructureTree);

   HRESULT SetKeywords(UINT nContainerId, LPCTSTR szKeywords);
   HRESULT OpenSourceDocument(LPCTSTR szDocumentName);
   HRESULT SaveSourceDocument();
   HRESULT SaveSourceDocumentAs();

   bool Is64BitPowerPointInstalled();
   HRESULT ImportPowerPoint(LPCTSTR szPowerPointName, bool bAskForLsd, bool bDoRecord);
   HRESULT GetLastSavedDocument(CString &csLastSavedDocument);

   HRESULT ChangeLineColor(COLORREF clrLine, int iTransparency=255);
   HRESULT ChangeFillColor(COLORREF clrFill, int iTransparency=255);
   HRESULT ChangeFillColor();
   HRESULT ChangeLineWidth(double dLineWidth);
   HRESULT ChangeLineStyle(int iLineStyle);
   HRESULT ChangeArrowStyle(int iArrowConfig, LPCTSTR szArrowStyle);
   HRESULT ChangeFontFamily(LPCTSTR szFontFamily);
   HRESULT ChangeFontSize(int iFontSize);
   HRESULT ChangeTextColor(COLORREF clrText);

   HRESULT Undo();

   HRESULT StartPackProject(LPCTSTR csProjectName);
   static UINT PackThread(LPVOID pParam);

   HRESULT GetPageProperties(UINT &nPageWidth, UINT &nPageHeight, COLORREF &clrBackground);
   HRESULT GetPagePropertiesEx(CRect rcW, UINT &nPageWidth, UINT &nPageHeight, UINT &nOffX, UINT &nOffY);

   HRESULT StartRecording();
   HRESULT PauseRecording();
   HRESULT UnPauseRecording();
   HRESULT StopRecording();
   HRESULT StartScreengrabbing(CRect &rcSelection);
   HRESULT StopScreengrabbing();
   HRESULT StartScreengrabbingSelection();


   // audio functions
   HRESULT GetAudioDevices(CStringArray &arDevices, UINT &nDevice);
   HRESULT GetSampleWidths(UINT nAudioDevice, CUIntArray &arSampleWidths, UINT &nSampleWidth);
   HRESULT GetSampleRates(UINT nAudioDevice, CUIntArray &arSampleRates, UINT &nSampleRate);
   HRESULT GetAudioSources(CStringArray &arSources, UINT &nSource);
   HRESULT ActivateAudioDevice();
   HRESULT SetAudioDevice(UINT nDevice);
   HRESULT SetSampleWidth(UINT nSampleWidth);
   HRESULT SetSampleRate(UINT nSampleRate);
   HRESULT SetAudioSource(UINT nSource);
   UINT GetAudioSource();

   double GetAudioLevel();
   UINT GetMixerVolume();
   HRESULT SetMixerVolume(UINT nMixerVolume);

   HRESULT WriteAudioSettings();
   HRESULT ReadAudioSettings();

   // video functions
   HRESULT GetVideoSettings(bool &bRecordVideo, bool &bShowMonitor, bool &bUseAlternativeSync,
                            bool &bSetAvOffset, int &nAvOffset);
   HRESULT GetVideoParameter(bool &bUseVFW, CStringArray &arVFWSources, UINT &nVFWIndex,
                             CStringArray &arWDMSources, UINT &nWDMIndex, 
                             UINT &nXResolution, UINT &nYResolution, UINT &nBitPerColor, 
                             CArray<float, float> &arFramerates, UINT &nFrameRateIndex);
   HRESULT EnableVideoRecording(bool bEnable, bool bCheckCodecs);
   HRESULT ShowVideoMonitor(bool bShow);
   HRESULT SetAlternativeAvSync(bool bUseAlternativeSync);
   HRESULT SetUseAvOffset(bool bUseAvOffset);
   HRESULT SetAvOffset(int iAvOffset);
   HRESULT SetUseVFW(bool bUseVFW);
   HRESULT ChangeWdmDevice(CString &csWdmDevice);
   HRESULT ShowVideoFormat();
   HRESULT ShowVideoSource();
   HRESULT ShowVideoDisplay();
   HRESULT WriteVideoSettings();
   HRESULT ReadVideoSettings(bool bShowWarning);

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

   // metadata functions
   HRESULT GetMetadata(CString &csTitle, CString &csAuthor, 
                       CString &csProducer, CString &csKeywords);
   HRESULT SetMetadata(CString &csTitle, CString &csAuthor, 
                       CString &csProducer, CString &csKeywords);

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
   HRESULT TestHotKey(AVGRABBER::HotKey &hkStartStop);
   HRESULT ActivateStartStopHotKey(bool bActivate=true);

   void GetLastSelectionRectangle(CRect &rcLastSelection, bool &bRecordFullScreen);
   void SetLastSelectionRectangle(CRect &rcLastSelection, bool bRecordFullScreen);

   HRESULT GetCodecSettings(CStringArray &arCodecList, UINT &nCodecIndex, bool &bHasAbout, bool &bHasConfigure,
                            bool &bSupportsQuality, UINT &nCodecQuality, 
                            bool &bSupportsKeyframes, bool &bUseKeyframes, UINT &nKeyframeDistance, 
                            bool &bSupportsDatarate, bool &bUseDatarte, UINT &nDatarate);
   HRESULT SetCodecSettings(CString &csCodec, UINT nCodecQuality, 
                            bool bUseKeyframes, UINT nKeyframeDistance, bool &bUseDatarte, UINT nDatarate);

   HRESULT ShowSgCodecAbout();
   HRESULT ShowSgCodecConfigure();

   // structure functions
   HRESULT RebuildStructureTree(CDocumentStructureView *pStructureTree = NULL);
   HRESULT RebuildRecordingTree(CRecordingsView *pRecordingList = NULL);

   HRESULT RemovePresentation(ASSISTANT::PresentationInfo *pPresentation);
   HRESULT DeletePresentation(ASSISTANT::PresentationInfo *pPresentation);

   HRESULT TranslateScreenToObjectCoordinates(CRect &rcWhiteboard,CPoint &ptScreen, Gdiplus::PointF &ptObject);
   HRESULT TranslateScreenToObjectCoordinates(CRect &rcWhiteboard,CRect &rcScreen, CRect &rcObject);

   ASSISTANT::DrawObject *GetObject(Gdiplus::PointF &ptMouse);
   HRESULT GetSelectedObjects(CArray<ASSISTANT::DrawObject *, ASSISTANT::DrawObject *> &arSelectedObjects);

   bool PointIsInObject(Gdiplus::PointF &ptMouse);

   void SetShowImportWarning(bool bShowImportWarning);
   void SetShowMissingFontsWarning(bool bShowMissingFontsWarning);
   void SetShowAudioWarning(bool bShowAudioWarning);
   void SetShowUseSmartBoard(bool bShowUseSmartBoard);
   void SetFreezeImages(bool bFreezeImage);
   void SetFreezeMaster(bool bFreezeMaster);
   void SetFreezeDocument(bool bFreezeDocument);
   void SetShowMouseCursor(bool bShowMouseCursor);
   void SetShowStatusBar(bool bShowStatusBar);
   void SetShowStartupPage(bool bShowStartupPage);
   void SetSwitchAutomaticallyToEdit(bool bSwitchAutomatically);
   void SetShowAVMonitor(bool bShowAvMonitor);
   // Audio monitor is visible just on pure screen recording
   void SetShowAudioMonitor(bool bShowAudioMonitor);

   bool GetShowImportWarning() {return m_bShowImportWarning;}
   bool GetShowMissingFontsWarning() {return m_bShowMissingFontsWarning;}
   bool GetShowAudioWarning() {return m_bShowAudioWarning;}
   bool GetShowUseSmartBoard() {return m_bShowUseSmartBoard;}
   bool GetFreezeImages() {return m_bFreezeImages;}
   bool GetFreezeMaster() {return m_bFreezeMaster;}
   bool GetFreezeDocument() {return m_bFreezeDocument;}
   bool GetShowMouseCursor(){return m_bShowMouseCursor;}
   bool GetShowStatusBar(){return m_bShowStatusBar;}
   bool GetShowStartupPage() {return m_bShowStartupPage;}
   bool GetSwitchAutomaticallyToEdit() {return m_bSwitchAutomatically;}
   bool GetShowAVMonitor() {return m_bShowAVMonitor;}
   // Audio monitor is visible just on pure screen recording
   bool GetShowAudioMonitor() {return m_bShowAudioMonitor;};
   

   bool IsRecording();
   bool IsRecordingPaused();

   CAssistantView *GetWhiteboardView();

   
   int GetGridWidth();
   HRESULT GetGridSettings(UINT &snapChecked, UINT &spacingValue, UINT &displayGrid);
   HRESULT SetGridSettings(UINT snapChecked, UINT spacingValue, UINT displayGrid);


protected:
    virtual BOOL SaveModified();

private:
   void Init();
   void Clear();
   
   void ReadSettings();
   void SaveSettings();

#ifndef _STUDIO
   bool OpenProject(LPCTSTR szFilename);
#endif
#ifndef _STUDIO
   bool StartOpen(LPCTSTR szFilename);
#endif

#ifndef _STUDIO
   static UINT OpenThread(LPVOID pParam);
#endif

   void UpdateAfterStructureChange();

// Generated message map functions
protected:
	//{{AFX_MSG(CAssistantDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CProgressDialogA *m_pProgress;
   UINT m_nToolId;
   COLORREF m_clrFill;
   COLORREF m_clrLine;
   COLORREF m_clrText;

   LOGFONT m_lf;
   HANDLE  m_hDialogInitEvent;

   bool m_bShowImportWarning;
   bool m_bShowMissingFontsWarning;
   bool m_bShowAudioWarning;
   bool m_bShowUseSmartBoard;
   bool m_bFreezeImages;
   bool m_bFreezeMaster;
   bool m_bFreezeDocument;
   bool m_bShowMouseCursor;
   bool m_bShowStatusBar;
   bool m_bShowStartupPage;
   bool m_bSwitchAutomatically;
   bool m_bShowAVMonitor;
   bool m_bShowAudioMonitor;

   bool m_bIsFullScreenOn;

   UINT m_snapChecked;
   UINT m_spacingValue;
   UINT m_displayGrid;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASSISTANTDOC_H__E41E9329_B2BC_4017_BEBA_6918B8FC1EC7__INCLUDED_)

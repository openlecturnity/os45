#pragma once

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif

//#include "LrdFile.h"
//#include "LanguageSupport.h"
#include "ScreengrabbingExtendedExternal.h" // lsutl32
#include "ScreengrabbingExtendedProgressDialog.h"
#include "ScreengrabbingExtendedHelper.h"

// ABBYY FineReader Engine 9.0
// #include "FREngineLoader.h"
// also add to include path \ABBYY FineReader Engine 9.0\Inc
// and add to linker path \ABBYY FineReader Engine 9.0\Bin
// maybe paths must be extended for ASSISTANT and EDITOR projects, too

class LSUTL32_EXPORT CscreengrabbingDoc : public CDocument
{
protected:
   DECLARE_DYNCREATE(CscreengrabbingDoc)

// Attribute
private:
   // the recording to be structure
   CLrdFile *m_pLrdFile;
   // the structure to be extended (will be done on SGML level)
   CLmdSgml *m_pLmdFile;
   // the structure to be extended (will be done on SGML level)
   CLepSgml *m_pLepFile;

   // stream/colection of clips
   CClipStream *m_pClipStream;
   CWhiteboardStream *m_pWhiteboardStream;
   CPageStream *m_pPageStream;
   // list of clips with added structutue
   CList<CClipInfoStructured*, CClipInfoStructured*> m_listStructuredClips;
   int m_iClipIndexCounter;
   bool m_bWriteEvqAndObj;
   bool m_bWriteLrd;
   bool m_bWriteLmd;
   bool m_bCanceled;
   bool m_bBackupOriginal;

   // progress dialog
   CScreengrabbingExtendedProgressDialog *m_pExportProgressDialog;
   bool m_bShowLaterButton;
   HANDLE  m_hDialogInitEvent;



// Vorgänge
public:
   CscreengrabbingDoc(bool bShowLaterButton = true);
	virtual ~CscreengrabbingDoc();

   virtual HRESULT Open(CString filename);
   virtual HRESULT Save(CString csLrdFilename);
   virtual HRESULT DoStructuring( bool bForce = false );
   virtual HRESULT ShowStructureEditor( bool bOnlyShowIfModified = false , bool bIsLiveContextMode=false);

   void SetLepFile(CLepSgml *pLepFile) {m_pLepFile = pLepFile;}
   // page number starting at 1
   int GetNumberOfPages();
   CString GetChapterTitle();
   CString GetPageTitle(int iPageNr);
   CString GetThumbnailFilename(int iPageNr);
   void DeleteThumbnailFiles(int iPageNr);
   int GetPageBeginTime(int iPageNr);
   bool SetChapterTitle(CString csTitle);
   bool SetPageTitle(int iPageNr, CString csTitle);
   bool RemovePage(int iPageNr);
   // support different page types (used by DEMO MODE)
   bool IsClickPage(int iPageNr);
   bool IsShowClickPages();
   void ShowClickPages( bool bShow);

   //// TODO: is this used??
   //bool doWriteOnAbort() { return m_pExportProgressDialog && m_pExportProgressDialog->isWriteOnCancel(); }

private:
   void Clear();

   HRESULT RunPostProcessingThread();
   static UINT __cdecl PostProcessingThreadLauncher(LPVOID pParam);
   void PostProcessingThread(LPVOID pParam);

   HRESULT PostProcessing(LPCTSTR lpszLrdFilename = NULL);

   // find all clips and create index structure (replacing clip tag)
   HRESULT CreateClipStructuresForAllClips();
   HRESULT CreateClipStructure(SgmlElement *pClipTag);
   // returns timestamps relative to clip
   HRESULT CreateClipStructure(CClipInfoStructured *clipInfoExtended);
   bool CreateScreenshots();
   bool CreateScreenshots(CClipInfoStructured *clipInfoExtended);
   bool CreateFulltext();
   bool CreateFulltext(CClipInfoStructured *clipInfoStructured);

   CArray<LMisc::StructuringValues>* ComputeStructuringValues(CClipInfoStructured *clipInfoStructured);


   // helper
   bool CreateEmptyObj(CString objName);
   bool CreateObj(CString objName, int timestamps[], int numberOfIndices);
   HRESULT UpdateEvqAndObj();
   bool WriteEvqAndObj(bool saveAs=false);
   bool CreateEvq(CString evqName, int timestamps[], int numberOfIndices);
   // BUGFIX 5505: "_internal_thumbinfo" no required anymore
   // bool WriteInternalThumbinfo();

   SgmlElement *FindPageBeginsAt(int timestamp);
   
   // reload previously stored structuring information of recording
   HRESULT ReloadStructure();

   // update progress dialog with label and position; also checks if canceled 
   inline void UpdateProgressTotal(HRESULT &hr, const int pos, UINT nResourceID = 0);

   // update progress dialog with progress of sub-task; also checks if canceled 
   inline void UpdateProgressSubtask(HRESULT &hr, const int pos);
	 
   // BUGFIX 5823: update progress & handle cancel while grabbing poster frames
   // helper used as callback
   static CscreengrabbingDoc *pScreengrabbingExtended;
   static HRESULT updateProgress();

// Überschreibungen
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
   virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

// Implementierung
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};

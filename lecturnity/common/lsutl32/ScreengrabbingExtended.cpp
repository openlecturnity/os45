// screengrabbingDoc.cpp : Implementierung der Klasse CscreengrabbingDoc
//

#include "stdafx.h"
//#include <afxstr.h>
#include <atlimage.h>

#include "ScreengrabbingExtended.h" // lsutl32
#include "StructureEditor.h"
#include "imc_lecturnity_converter_ScreengrabbingExtended.h" // lsutl32

#include "JNISupport.h"

#include "LanguageSupport.h"
#include "KeyGenerator.h"

#include "LepSgml.h"        // filesdk;

#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//static
HRESULT CScreengrabbingExtendedExternal::RunPostProcessing(CString csLrdFilename, bool bShowLaterButton, 
                                                           bool bIsInLiveContextMode, CLepSgml *pLepFile) {
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   CscreengrabbingDoc screengrabbingDoc(bShowLaterButton);
   if(SUCCEEDED(hr))
      hr = screengrabbingDoc.Open(csLrdFilename);

   if(SUCCEEDED(hr) && pLepFile != NULL)
      screengrabbingDoc.SetLepFile(pLepFile);

   if(SUCCEEDED(hr))
      hr = screengrabbingDoc.DoStructuring();
 
   // BUGFIX 5762: do not show Structure Editor for LiveContext
   if(SUCCEEDED(hr) && !bIsInLiveContextMode) {
      hr = screengrabbingDoc.ShowStructureEditor( true, bIsInLiveContextMode );
   }

   // unlike EditStructure() it is ok to call save if Structure Editor was canceled here
   // there might be other modifications made
   if(SUCCEEDED(hr))
      hr = screengrabbingDoc.Save(csLrdFilename);

   return hr;
}

//static
HRESULT CScreengrabbingExtendedExternal::EditStructure(CString csLrdFilename) {
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   CscreengrabbingDoc screengrabbingDoc(true);
   if(SUCCEEDED(hr))
      hr = screengrabbingDoc.Open(csLrdFilename);

   if(SUCCEEDED(hr))
      // "true" required for reading structure
      hr = screengrabbingDoc.DoStructuring( true );

   if(SUCCEEDED(hr)) {
      hr = screengrabbingDoc.ShowStructureEditor( false );
   }

   // BUGFIX 5134: do not call save if Structure Editor was canceled
   if(SUCCEEDED(hr) && hr == S_OK)
      hr = screengrabbingDoc.Save(csLrdFilename);

   return hr;
}

// CscreengrabbingDoc
IMPLEMENT_DYNCREATE(CscreengrabbingDoc, CDocument)

BEGIN_MESSAGE_MAP(CscreengrabbingDoc, CDocument)
END_MESSAGE_MAP()

// CscreengrabbingDoc-Erstellung/Zerstörung
CscreengrabbingDoc::CscreengrabbingDoc(bool bShowLaterButton)
{
   // TODO: Hier Code für One-Time-Konstruktion einfügen
   m_pExportProgressDialog = NULL;
   m_pClipStream = NULL;
   m_pWhiteboardStream = NULL;
   m_pPageStream = NULL;
   m_pLrdFile = NULL;
   m_pLmdFile = NULL;
   m_pLepFile = NULL;
   m_listStructuredClips.RemoveAll();
   m_iClipIndexCounter = 0;
   m_bWriteEvqAndObj = false;
   m_bWriteLrd = false;
   m_bWriteLmd = false;
   m_bCanceled = false;
   m_bShowLaterButton = bShowLaterButton;
   m_bBackupOriginal = false;
}

CscreengrabbingDoc::~CscreengrabbingDoc()
{
   TRACE("############### CscreengrabbingDoc::~CscreengrabbingDoc ##############\n");
   Clear();
}

void CscreengrabbingDoc::Clear() 
{
   // Delete m_listStructuredClips content if any
   POSITION position = m_listStructuredClips.GetHeadPosition();
   while (position != NULL) {   
      delete m_listStructuredClips.GetNext(position);
   }
   m_listStructuredClips.RemoveAll();

   SAFE_DELETE(m_pExportProgressDialog);
   SAFE_DELETE(m_pClipStream);
   SAFE_DELETE(m_pWhiteboardStream);
   SAFE_DELETE(m_pPageStream);
   SAFE_DELETE(m_pLrdFile);
   SAFE_DELETE(m_pLmdFile);

   m_iClipIndexCounter = 0;
}

BOOL CscreengrabbingDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;

   // TODO: Hier Code zur Reinitialisierung einfügen
   // (SDI-Dokumente verwenden dieses Dokument)
   TRACE("############### CscreengrabbingDoc::OnNewDocument ##############\n");


   return true;
}


// CscreengrabbingDoc-Diagnose
#ifdef _DEBUG
void CscreengrabbingDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void CscreengrabbingDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG

// CscreengrabbingDoc-Serialisierung
void CscreengrabbingDoc::Serialize(CArchive& ar)
{
   if (ar.IsStoring())
   {
      // TODO: Hier Code zum Speichern einfügen
   }
   else
   {
      // TODO: Hier Code zum Laden einfügen
      //CString filePath = ar.GetFile()->GetFilePath();
      //Open(filePath);
   }
}
BOOL CscreengrabbingDoc::OnSaveDocument(LPCTSTR lpszPathName) {
   return Save(lpszPathName);
}

HRESULT CscreengrabbingDoc::Save(CString csLrdFilename){
   bool bSuccess = true;

   TRACE("OnSaveDocument %ls\n",csLrdFilename);

   if(m_listStructuredClips.IsEmpty()) {
      TRACE("NOT STRUCTURED - WILL NOT WRITE RECORDING\n");
      return S_OK;
   }

   // check if SAVE or SAVE_AS
   //TODO: copy files (if save_as)
   CString csPath = csLrdFilename;
   StringManipulation::GetPath(csPath);
   CString csFilename = csLrdFilename;
   StringManipulation::GetFilename(csFilename);
   bool bSaveAs =(_tcscmp(m_pLrdFile->GetRecordPath(), csPath) != 0) 
      || (_tcscmp(m_pLrdFile->GetFilename(), csFilename) != 0);
   if(bSaveAs) {
      TRACE("!!!!!!!!!!!!!! SAVE AS is currently NOT FULLY SUPPORTED !!!!!!!!\n");
      /*
      CString csPathFilePrefix = csLrdFilename;
      StringManipulation::GetPathFilePrefix(csPathFilePrefix);
      CString csFilePrefix = csLrdFilename;
      StringManipulation::GetFilePrefix(csFilePrefix);

      CString csEvqFilename;
      csEvqFilename.Format(_T("%ls.evq"),csPathFilePrefix);
      CString csObjFilename;
      csObjFilename.Format(_T("%ls.obj"),csPathFilePrefix);
      CString csLmdFilename;
      csLmdFilename.Format(_T("%ls.lmd"),csPathFilePrefix);

      // update LRD
      m_pLrdFile->SetRecordPath(csPath);
      m_pLrdFile->SetFilename(csFilename);
      StringManipulation::GetFilename(csLmdFilename);
      m_pLrdFile->SetMetadataFilename(csLmdFilename);

      // TODO: update thumbnail filenames in LMD

      // update whiteboard stream (EVQ/OBJ)
      StringManipulation::GetFilename(csEvqFilename);
      m_pLrdFile->SetEvqFilename(csEvqFilename);
      StringManipulation::GetFilename(csObjFilename);
      m_pLrdFile->SetObjFilename(csObjFilename);

      if(m_pWhiteboardStream) {
         m_pWhiteboardStream->SetEvqFilename(csEvqFilename);
         m_pWhiteboardStream->SetObjFilename(csObjFilename);
      }

      /////////////////////////////////////////////////////
      // audio
      /////////////////////////////////////////////////////
      CString csAudioFilename = m_pLrdFile->GetAudioFilename();
      CString csAudioFilenameNew;
      csAudioFilenameNew.Format(_T("%ls.lad"), csPathFilePrefix);

      TRACE("  Copy audio %ls\n", csAudioFilename);
      TRACE("          to %ls\n", csAudioFilenameNew);

      // TODO: error handling
      bSuccess = (CopyFile(csAudioFilename, csAudioFilenameNew, FALSE) != 0);

      // set audio name without path in LRD
      StringManipulation::GetFilename(csAudioFilenameNew);
      m_pLrdFile->SetAudioFilename(csAudioFilenameNew);


      /////////////////////////////////////////////////////
      // video
      /////////////////////////////////////////////////////
      CString csVideoFilename = m_pLrdFile->GetVideoFilename();
      if(!csVideoFilename.IsEmpty()) {
         CString csVideoFilenameNew;
         // TODO: file ending???
         csVideoFilenameNew.Format(_T("%ls.avi"), csPathFilePrefix);

         TRACE("  Copy video %ls\n", csVideoFilename);
         TRACE("          to %ls\n", csVideoFilenameNew);

         // TODO: error handling
         bSuccess = (CopyFile(csVideoFilename, csVideoFilenameNew, FALSE) != 0);

         // set video name without path in LRD
         StringManipulation::GetFilename(csVideoFilenameNew);
         m_pLrdFile->SetVideoFilename(csVideoFilenameNew);
      }

      /////////////////////////////////////////////////////
      // still images
      /////////////////////////////////////////////////////
      // TODO: copy still image

      /////////////////////////////////////////////////////
      // audio
      /////////////////////////////////////////////////////
      if(m_pClipStream && m_pClipStream->HasClips()) {
         for(int i=0; i<m_pClipStream->GetSize(); i++) {
            CString csClipFilename = m_pClipStream->m_arrClip.GetAt(i)->GetFilename();
            CString csClipFilenameNew;
            csClipFilenameNew.Format(_T("%ls_clip_%i.avi"), csPathFilePrefix, (i+1));

            TRACE("  Copy clip %ls\n", csClipFilename);
            TRACE("         to %ls\n", csClipFilenameNew);
            //TRACE("    Copy clip: %ls\n", m_pClipStream->m_arrClip.GetAt(i)->GetFilename());

            // TODO: error handling
            bSuccess = (CopyFile(csClipFilename, csClipFilenameNew, FALSE) != 0);

            // set clip name without path in LRD
            // TODO: set
            StringManipulation::GetFilename(csClipFilenameNew);
            m_pLrdFile->SetClipFilename(i+1, csClipFilenameNew);
         }
      }
      */
   } else {
      TRACE("  SAVE\n");

      // Bugfix 5163: inconsistancy if removing structure after export
      // only backup original and not if no "*.orig" files are available
      if(m_bBackupOriginal) {
         TRACE("  BACKUP ORIGINAL\n");

         CString csBackupFile;

         if( bSuccess ) {
            // backup LRD
            csBackupFile.Format(_T("%ls.orig"),csLrdFilename);
            // TODO: error handling
            // NOTE: TRUE says FailIfExists and NOT overwrite

            // BUGFIX 5134: error message on canceling structure editor
            // caused by error on creating backup
            // must not set bSuccess to false if copy fails
            // because copying backup is best effort and fails if already
            // bSuccess = (CopyFile(csLrdFilename, csBackupFile, TRUE) != 0);
            CopyFile(csLrdFilename, csBackupFile, TRUE);
         }

         if(bSuccess) {
            // backup LMD
            CString csLmdFilename = m_pLrdFile->GetMetadataFilename();
            csBackupFile.Format(_T("%ls.orig"),csLmdFilename);
            // TODO: error handling
            // BUGFIX 5134: (see above)
            // bSuccess = (CopyFile(csLmdFilename, csBackupFile, TRUE) != 0);
            CopyFile(csLmdFilename, csBackupFile, TRUE);
         }

         // TODO: backup OBJ/EVQ/... for non-denver recordings (currently not supported)
      }
   }


   // write whiteboard stream (OBJ/EVQ)
   // NOTE: condition is necessary to avoid overwriting of previously stored fulltext
   //       (may be removed if loading of EVQ/OBJ is supported)
   if(bSuccess || m_bWriteEvqAndObj)
      bSuccess = WriteEvqAndObj(bSaveAs);
   
   // write metadata (LMD)
   if(bSuccess || m_bWriteLmd)
      bSuccess = m_pLmdFile->Write(m_pLrdFile->GetMetadataFilename());

   // write LRD
   if(bSuccess || m_bWriteLrd)
      bSuccess = m_pLrdFile->Write(csLrdFilename);

   if(!bSuccess) {
      // BUGFIX 5108: show meaningfull error message
      // something went wrong while writing files
      CString cError;
      cError.Format(IDS_SG_WRITE_ERROR);
      MessageBox(NULL, cError, NULL, MB_OK);
      return E_FAIL;
   }

   return S_OK;
}

// CscreengrabbingDoc-Befehle
HRESULT CscreengrabbingDoc::Open(CString csLrdFilename){
   HRESULT hr = S_OK;

   TRACE("\n--------------------------------\n\n");

   TRACE("LOADING LRD %ls\n",csLrdFilename);

   m_pLrdFile = new CLrdFile();

   if (!m_pLrdFile->Open(csLrdFilename))
   {
      TRACE("LOADING LRD FAILED - %ls\n",csLrdFilename);
      delete m_pLrdFile;
      m_pLrdFile = NULL;
      hr = E_FAIL;
   }

   return hr;
}

HRESULT CscreengrabbingDoc::DoStructuring( bool bForce ) {
   HRESULT hr = S_OK;

   //// currently structuring is only supported for pure SG clips:
   // only call structuring for pure SG clips (denver)
   bool bPerformPostProcessing = m_pLrdFile->IsDenverDocument();

   // TODO: when to create a backup??
   m_bBackupOriginal = m_pLrdFile->IsDenverDocument();

   // or to continue structuring (structure available but Images and/or Fulltext is missing)
   if(!bPerformPostProcessing && m_pLrdFile->GetClipStructureAvailable(1))
      bPerformPostProcessing = !m_pLrdFile->GetClipImagesAvailable(1) 
                            || !m_pLrdFile->GetClipFulltextAvailable(1);

   //// or start (WB with SG)
   //for(int i=1; !bPerformPostProcessing && i<=m_pLrdFile->GetNumberOfClips(); i++)
   //   bPerformPostProcessing = !m_pLrdFile->GetClipStructureAvailable(i)
   //                         || !m_pLrdFile->GetClipImagesAvailable(i) 
   //                         || !m_pLrdFile->GetClipFulltextAvailable(i);

   if(bPerformPostProcessing || bForce) {
      //PostProcessing();
      hr = RunPostProcessingThread();
   }

   return hr;
}


HRESULT CscreengrabbingDoc::ShowStructureEditor( bool bOnlyShowIfModified , bool bIsLiveContextMode) {
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   // only call editor if structure is available
   bool bShowStructureEditor = m_pLrdFile->GetClipStructureAvailable(1) && !m_bCanceled;
   if(bShowStructureEditor && bOnlyShowIfModified)
      // only call editor if structure is available and was added/modified lately
      bShowStructureEditor = m_bWriteLrd;

      // && m_pLrdFile->GetClipImagesAvailable(1) && m_pLrdFile->GetClipFulltextAvailable(1);
   if(bShowStructureEditor) {
       CWnd *pParent = NULL;
       if (bIsLiveContextMode) {
           HWND hwndDesktop = GetDesktopWindow();
           pParent = CWnd::FromHandle(hwndDesktop);
       }
      CStructureEditor pStructureEditorDialog(this, pParent);// = new CStructureEditor(this);
      if(pStructureEditorDialog.DoModal() == IDOK) {
         // write modified structure
         m_bWriteLmd = true;
         m_bWriteEvqAndObj = true;
      } 
	  // BUGFIX 5134: handle cancel
	  else
         hr = S_FALSE;
   }

   return hr;
}

#define PROGRESS_INITIALIZATION 10
#define PROGRESS_STRUCTURING 25
#define PROGRESS_IMAGING 50
#define PROGRESS_FULLTEXT 75
#define PROGRESS_FINALIZATION 90
#define PROGRESS_COMPLETED 100


/////////////////////////////////////////////////////////////////////////
// main processing method
/////////////////////////////////////////////////////////////////////////

HRESULT CscreengrabbingDoc::PostProcessing(LPCTSTR lpszLrdFilename) {
   CString csLrdFilename = lpszLrdFilename;

   HRESULT hr = S_OK;

   /////////////////////////////////////////////////////////////////////////
   // initialization
   /////////////////////////////////////////////////////////////////////////
   UpdateProgressTotal(hr, PROGRESS_INITIALIZATION, IDC_EP_PARTICULAR);

   // load LRD file if specified; check if loaded otherwise
   if(SUCCEEDED(hr)) {            
      if(!csLrdFilename.IsEmpty()) {
         TRACE("LOADING LRD %ls\n",csLrdFilename);
         if(m_pLrdFile)
            delete m_pLrdFile;
         m_pLrdFile = new CLrdFile();

         if (!m_pLrdFile->Open(csLrdFilename))
         {
            TRACE("LOADING LRD FAILED - %ls\n",csLrdFilename);
            delete m_pLrdFile;
            m_pLrdFile = NULL;
            hr = E_FAIL;
         }
      }
      if(!m_pLrdFile)
         hr = E_FAIL;
   }

   // read LMD file
   if (SUCCEEDED(hr)) {            
       CString csLmdFilename = m_pLrdFile->GetMetadataFilename();
       if (m_pLmdFile == NULL)
           m_pLmdFile = new CLmdSgml();
       if (!m_pLmdFile->Read(csLmdFilename)) {
           TRACE("  Error reading LMD: %ls\n", csLmdFilename);
           hr = E_FAIL;
       }
   }

   // reload previously stored structuring information
   if(SUCCEEDED(hr)) {            
      m_pClipStream = m_pLrdFile->CreateClipStream();
      hr = ReloadStructure();
   }

   /////////////////////////////////////////////////////////////////////////
   // structuring
   /////////////////////////////////////////////////////////////////////////
   UpdateProgressTotal(hr, PROGRESS_STRUCTURING, IDC_STRUCTURING);

   // handle clips
   if(SUCCEEDED(hr)) {            
      hr = CreateClipStructuresForAllClips();
   }

   /////////////////////////////////////////////////////////////////////////
   // imaging / thumbnails
   /////////////////////////////////////////////////////////////////////////
   UpdateProgressTotal(hr, PROGRESS_IMAGING, IDC_IMAGING);

   // screenshots and thumbnails
   if(SUCCEEDED(hr)) {
      hr = CreateScreenshots();
   }

   /////////////////////////////////////////////////////////////////////////
   // full text search / character recognition
   /////////////////////////////////////////////////////////////////////////
   UpdateProgressTotal(hr, PROGRESS_FULLTEXT, IDC_FULLTEXT);

   // text recognition for full text search
   if(SUCCEEDED(hr)) {            
      hr = CreateFulltext();
   }

   UpdateProgressTotal(hr, PROGRESS_FINALIZATION);
   UpdateProgressSubtask(hr, 100);

   /////////////////////////////////////////////////////////////////////////
   // finalization
   /////////////////////////////////////////////////////////////////////////
   // add images and full text to whiteboard stream
   if(SUCCEEDED(hr))
      hr = UpdateEvqAndObj();

   UpdateProgressTotal(hr, PROGRESS_COMPLETED, IDC_COMPLETED);
   UpdateProgressSubtask(hr, 100);

   if (hr == E_ABORT && m_pExportProgressDialog && m_pExportProgressDialog->IsWriteOnCancel()) {
      // write partial structuring data to continue later
      hr = UpdateEvqAndObj();
      if(SUCCEEDED(hr))
         // set status to S_FALSE as notification of incomplete structure 
         hr = S_FALSE;
   } else if (hr == E_ABORT) {
      // reset to pure SG document
      CString csLrdFilename;
      csLrdFilename.Format(_T("%ls\\%ls"), m_pLrdFile->GetRecordPath(), m_pLrdFile->GetFilename());
      CScreengrabbingExtendedExternal::RemoveStructure(csLrdFilename);
   }
   return hr;
}


/////////////////////////////////////////////////////////////////////////
// structuring methods
/////////////////////////////////////////////////////////////////////////

int GetNumber(char *szToken, int iValidCharacters)
{
   szToken[iValidCharacters] = 0;
   return atoi(szToken);
}

// computes index timestamps relative to clip
HRESULT CscreengrabbingDoc::CreateClipStructure(CClipInfoStructured *clipInfoStructured){
   if(!clipInfoStructured)
      return E_FAIL;

   HRESULT hr = S_OK;

   // read structuring data if available
   CString csStructuringValuesFilename = clipInfoStructured->clipInfo->GetFilename();
   int iLength = csStructuringValuesFilename.GetLength();
   csStructuringValuesFilename.SetAt(iLength - 3,'s');
   csStructuringValuesFilename.SetAt(iLength - 2,'g');
   csStructuringValuesFilename.SetAt(iLength - 1,'e');
   CArray<LMisc::StructuringValues> *paStructuringValues = ReadStructuringValues(csStructuringValuesFilename);

   // compute and write structuring data if required
   if(!paStructuringValues) {
      // compute
      paStructuringValues = ComputeStructuringValues(clipInfoStructured);

      // write to file
      WriteStructuringValues(csStructuringValuesFilename, paStructuringValues);
   }

   // read user events if available
   csStructuringValuesFilename.SetAt(iLength - 1,'a');
   CArray<UserAction> *paUserActions = ReadUserActions(csStructuringValuesFilename);

   // read Live Context data if available
   CArray<CLiveContextElementInfo *,CLiveContextElementInfo*> *paElementInfos = ReadLiveContextElementInfos(csStructuringValuesFilename);

   // determine framerate
   double dFrameRate = 0;
   VideoInformation::GetVideoFramerate(clipInfoStructured->clipInfo->GetFilename(), dFrameRate);

   DWORD dwStarttime = GetTickCount();
   if(SUCCEEDED(hr)) {
      if(paStructuringValues) {
         TRACE(_T("\nCOMPUTE STRUCTURE:\n"));
         // first frame updates all pixels (same as width x height)
         int iMaxPixel = paStructuringValues->GetAt(0).iPixelCount;

         bool setIndex = false;

         //int lastFrame = 0;

         for( int i = 0; i < paStructuringValues->GetSize(); i++ ) {
            LMisc::StructuringValues pStructuringValues = paStructuringValues->GetAt(i);
            //TRACE(_T("frame %i pixel %i area %i\n"), pStructuringValues.iFrameNr, 
            //   pStructuringValues.iPixelCount, pStructuringValues.iAreaCount);

            int msec = (pStructuringValues.iFrameNr-1) * 1000 / dFrameRate;
            double percentagePixelChanges = pStructuringValues.iPixelCount * 100.0 / iMaxPixel;
            double percentageRectChanges = pStructuringValues.iAreaCount * 100.0 / iMaxPixel;

            // workaround bug 5338 (missing structure)
            if (pStructuringValues.iFrameNr == 1) {
               // first frame should always cover 100 percent
               percentagePixelChanges = 100.0;
               percentageRectChanges = 100.0;
            }

            //for(int j = lastFrame + 1; j < pStructuringValues.iFrameNr; j++) 
            //   TRACE("-");
            //TRACE("\n");
            //lastFrame = pStructuringValues.iFrameNr;

            //TRACE("## frame %7i time %7i area %3.1f ", pStructuringValues.iFrameNr, msec, percentageRectChanges);
            //for(int j=0; j<percentageRectChanges; j++)
            //   TRACE("#");
            //TRACE("\n");


            if(percentageRectChanges>15.0) {
               setIndex = true;

               // include slightly delayed updates
               if(pStructuringValues.iFrameNr > 1) {
                  int iFrameNr = pStructuringValues.iFrameNr;
                  int iFramesToBeTested = dFrameRate/2; // every frame within 500 msec
                  for( int j = i+1; j < paStructuringValues->GetSize(); j++ ) {
                     pStructuringValues = paStructuringValues->GetAt(j);
                     if(pStructuringValues.iFrameNr <= iFrameNr+iFramesToBeTested)
                        msec = (pStructuringValues.iFrameNr-1) * 1000 / dFrameRate;
                     else
                        break;
                  }
               }

               if(!clipInfoStructured->structure.IsEmpty()) {
                  // compare with previous index
                  CStructureElement *prevElement = clipInfoStructured->structure.GetTail();
                  if(msec - prevElement->timestamp < 1500) {
                     if ( clipInfoStructured->structure.GetCount()>1 ) {
                        // overwrite page timestamp (except first page)
                        prevElement->timestamp = msec;
                        TRACE("### INDEX %i RESET TO %i\n", clipInfoStructured->structure.GetCount(), clipInfoStructured->structure.GetTail()->timestamp);
                     }
                     setIndex = false;
                  }
               }
               if(setIndex) { 
                  // set additional index
                  clipInfoStructured->structure.AddTail(new CStructureElement(msec));
                  TRACE("### INDEX %i SET TO %i\n", clipInfoStructured->structure.GetCount(), clipInfoStructured->structure.GetTail()->timestamp);
               }
            }
         }
      } else
         hr = E_FAIL;  
   }
   if(SUCCEEDED(hr)) {
      if(paUserActions) {
         POSITION position = clipInfoStructured->structure.GetHeadPosition();

         int iLastActionIndex = -1;
         for( int i = 0; i < paUserActions->GetSize(); i++ ) {
            UserAction pUserAction = paUserActions->GetAt(i);
            if ( pUserAction.iTimeMsec > iLastActionIndex &&
                 pUserAction.dwEventType != MH_POINTER &&  pUserAction.dwEventType != MH_TELE &&
               //( pUserAction.dwAction == MH_BUTTON_DOWN || pUserAction.dwAction == MH_BUTTON_DBLCLICK) ) {
               ( pUserAction.dwAction == MH_BUTTON_UP || pUserAction.dwAction == MH_BUTTON_DBLCLICK) ) {
                  // mouse pressed
                  //int msec = (pUserAction.iFrameNr-1) * 1000 / dFrameRate;
                  int msec = pUserAction.iTimeMsec;
                  
                  // include slightly delayed updates (within 500 msec)
                  for( int i = 0; i < paStructuringValues->GetSize(); i++ ) {
                     int msecFrame = (paStructuringValues->GetAt(i).iFrameNr-1) * 1000 / dFrameRate;
                     if ( msecFrame > pUserAction.iTimeMsec + 500 )
                        // too late: end loop
                        break;
                     if ( msecFrame > msec )
                        // shift
                        msec = msecFrame;
                  }
                  iLastActionIndex = msec;

                  // search matching LiveContext data
                  pUserAction.pElementInfo = NULL;
                  for ( int j=0; paElementInfos != NULL && j<paElementInfos->GetSize(); j++ ) {
                     CLiveContextElementInfo *pElementInfo = paElementInfos->GetAt(j);
                     if ( pElementInfo != NULL && pElementInfo->GetID() == pUserAction.iTimeMsec ) {
                        // add element info
                        pUserAction.pElementInfo = pElementInfo;

                        TRACE("\nLIVE CONTEXT INFO AT %i FOR EVENT AT %i :\n---> ", msec, pUserAction.iTimeMsec);
                        TRACE(pElementInfo->ToJsonString());

                        break;
                     }
                  }

                  // insert at correct position accoring to timestamp
                  while( position && clipInfoStructured->structure.GetAt(position)->timestamp < msec )
                     clipInfoStructured->structure.GetNext(position);
                  if ( position ) {
                     // use existing index if within 1000 msec after mouse click
                     // otherwise insert new click page index
                     if ( clipInfoStructured->structure.GetAt(position)->timestamp > pUserAction.iTimeMsec + 1000 )
                        clipInfoStructured->structure.InsertBefore(position, new CStructureElement(msec, true, &pUserAction));
                     else if (!clipInfoStructured->structure.GetAt(position)->hasAction())
                        // page has no click info -> add current click info
                        clipInfoStructured->structure.GetAt(position)->m_action = pUserAction;
                  } else {
                     // later than any existing index -> add at end
                     clipInfoStructured->structure.AddTail(new CStructureElement(msec, true, &pUserAction));
                  }
            }
         }
      }
   }


   SAFE_DELETE(paStructuringValues);
   /* TODO - see if array content should be deleted.
   // NOTE: some pElementInfo are set in clipInfoStructured (see above)
   // and used later for writing LEP in CreateClipStructure
   // not deleting leads to memory leaks  
   // TODO: clipInfoStructured should use copy of required pElementInfo and delet pElementInfos here
   for ( int j=0; paElementInfos != NULL && j<paElementInfos->GetSize(); j++ ) {
       CLiveContextElementInfo *pElementInfo = paElementInfos->GetAt(j);
       SAFE_DELETE(pElementInfo);
   }*/
   SAFE_DELETE(paElementInfos);
   SAFE_DELETE(paUserActions);
   DWORD dwEndtime = GetTickCount();
   TRACE("------------------ INDEXING TOOK %i msec\n", dwEndtime - dwStarttime);


   return hr;
}
   
// computes values used for structuring
CArray<LMisc::StructuringValues>* CscreengrabbingDoc::ComputeStructuringValues(CClipInfoStructured *clipInfoStructured) {
   HRESULT hr = S_OK;

   //int iDuration = 0;
   //VideoInformation::GetVideoDuration(clipInfoStructured->clipInfo->GetFilename(), iDuration);
   //TRACE("        video duration %i msec\n", iDuration);
   double dFrameRate = 0;
   VideoInformation::GetVideoFramerate(clipInfoStructured->clipInfo->GetFilename(), dFrameRate);
   //TRACE("        video frame rate %3.1f fps\n", dFrameRate);
   //int iTotalFrames = iDuration * dFrameRate / 1000;
   //TRACE("        total frames %i \n", iTotalFrames);
   ////TRACE("        estimated processing time %i sec\n", totalFrames/25);
   //TRACE("        estimated processing time %2.1f min\n", (double)(iTotalFrames/25)/60);

   // create and start AVI filtering
   // initialze renderer
   hr = CoInitialize(0);

   IGraphBuilder*  graph = NULL;
   IMediaControl*  ctrl = NULL; 
   IPin*           rnd  = NULL; 
   IBaseFilter*    vid  = NULL; 
   IPin*           avi  = NULL; 

   if(SUCCEEDED(hr))
      hr = CoCreateInstance( CLSID_FilterGraph, 0, CLSCTX_INPROC,IID_IGraphBuilder, (void **)&graph );
   if(SUCCEEDED(hr)) 
      hr = graph->QueryInterface( IID_IMediaControl, (void **)&ctrl );

   Sampler*        sampler      = new Sampler(0,&hr); 
   if(SUCCEEDED(hr)) 
      hr = sampler->FindPin(L"In", &rnd);
   if(SUCCEEDED(hr)) 
      hr = graph->AddFilter((IBaseFilter*)sampler, L"Sampler");

   if(SUCCEEDED(hr)) 
      hr = graph->AddSourceFilter (clipInfoStructured->clipInfo->GetFilename(), L"File Source", &vid);
   if(SUCCEEDED(hr)) 
      hr = vid->FindPin(L"Output", &avi);
   if(SUCCEEDED(hr)) 
      hr = graph->Connect(avi,rnd);
   if(SUCCEEDED(hr)) 
      hr = graph->Render( avi );
   if(hr == VFW_E_ALREADY_CONNECTED) 
      hr=S_OK;

   // run renderer
   if(SUCCEEDED(hr)) 
      do{
         // wait until ready
         Sleep(50);
         hr = ctrl->Run();
      }while(hr == S_FALSE);

   // wait until renderer has completed
   int iDuration = clipInfoStructured->clipInfo->GetLength();
   int iCount=0;
   // waiting for end of stream, i.e. video has been parsed
   while(SUCCEEDED(hr) && !sampler->IsEndOfStream()) {
      int iMsec = sampler->m_iFrameCounter * 1000 / dFrameRate;
      UpdateProgressSubtask(hr, 5 + (90 * iMsec) / iDuration);
      Sleep(50);
   }

   // get result
   CArray<LMisc::StructuringValues> *paStructuringValues = sampler->m_paStructuringValues;

   // close graph and cleanup
   if(SUCCEEDED(hr) || (hr == E_ABORT)) {
      graph->Disconnect(rnd);
      graph->Disconnect(avi);
      graph->RemoveFilter(sampler);
   }
   SafeRelease(&rnd);
   SafeRelease(&avi);
   SafeRelease(&vid);
   SafeRelease(&ctrl);
   SafeRelease(&graph);
   
   CoUninitialize();

   // return result
   if(SUCCEEDED(hr) && paStructuringValues && !paStructuringValues->IsEmpty())
      return paStructuringValues;
   else
      return NULL;
}


HRESULT CscreengrabbingDoc::CreateClipStructuresForAllClips() {
    HRESULT hr = S_OK;

    // init and checks
    if (m_pExportProgressDialog && m_pExportProgressDialog->IsCanceled())
        // recursive call was canceled
        hr = E_ABORT;
    else if (m_pClipStream == NULL) {
        hr = E_FAIL;
        /*
        // TODO: still required?? should never occur since clipstream and structure should be loaded already
        // get screengrabbing clips
        m_pClipStream = m_pLrdFile->CreateClipStream();

        if (!m_pClipStream || !m_pClipStream->HasClips()) {
            // no clips - no structure to be computed - done
            TRACE("  no Screengrabbing Clip found\n");
            hr = E_FAIL;
        } else if (m_pClipStream->GetSize()!=1) {
            //// TODO: non denver support
            TRACE("  currently only pure Screengrabbing is supported, but %i clips found\n", m_pClipStream->GetSize());
            hr = E_FAIL;
        }
        */
    }

    if ( SUCCEEDED(hr) ) {
       CArray<SgmlElement *, SgmlElement *> aClipTags;
       hr = m_pLmdFile->GetAllClips(aClipTags);

       // Replace each clip with a page list.
       for (int i = 0; SUCCEEDED(hr) && i < aClipTags.GetCount(); ++i) {
           hr = CreateClipStructure(aClipTags[i]); 
       }
    }

    return hr;
}


// computes clip structure and replaces clip tag by new clip structure
// the clip structure is a sequence of pages
// an additional chapter is added (according to DTD) for pure screengrabbing clip
HRESULT CscreengrabbingDoc::CreateClipStructure(SgmlElement *pClipTag) {
    HRESULT hr = S_OK;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    TRACE("\ncreateIndexForClip:\n");

    UpdateProgressSubtask(hr, 2);

    if (pClipTag == NULL || _tcscmp(pClipTag->GetName(),_T("clip")) != 0) {
        // clip tag not valid
        TRACE("FAILED - NO VALID CLIP TAG");
        hr = E_FAIL;
    }

    /////////////////////////////////////////////////////////
    // init and checks
    /////////////////////////////////////////////////////////
    CClipInfoStructured *pClipInfoStructured = NULL;
    if (SUCCEEDED(hr)) {
        // init clip info
        TRACE("  Replacing Clip:\n");
        TRACE("     Title: %ls\n",  pClipTag->Find(_T("title"))->GetParameter());
        TRACE("     Nr: %ls\n",  pClipTag->Find(_T("nr"))->GetParameter());
        TRACE("     Begin: %ls\n",  pClipTag->Find(_T("begin"))->GetParameter());
        TRACE("     End: %ls\n",  pClipTag->Find(_T("end"))->GetParameter());
        TRACE("     Thumb: %ls\n",  pClipTag->Find(_T("thumb"))->GetParameter());   

        // find corresponding clip by timestamp
        CString csBegin;
        pClipTag->GetElementParameter(_T("begin"), csBegin);
        int iBegin = _ttoi(csBegin);

        int iClipId;
        CClipInfo *pClipInfo = m_pClipStream->FindClipBeginsAt(iBegin, &iClipId);
        if (pClipInfo == NULL) {
            TRACE("NO CORRESPONDING CLIP FOUND AT %i - TRY AGAIN\n", iBegin);
            // try to find clip, whose starttime does not match exactly (e.g. after Editor Export)
            pClipInfo = m_pClipStream->FindClipAt(iBegin, &iClipId);
        }

        if (pClipInfo != NULL) {
            // extended ClipInfo including structuring timestamps
            pClipInfoStructured = new CClipInfoStructured(pClipInfo, iClipId);

            // check if clip duration is set correctly because we will rely on that information later
            // NOTE: clipInfoStructured.clipInfo->GetEndTime()/GetLength() are probably not initialized
            int iDuration = pClipInfoStructured->clipInfo->GetLength();
            if (iDuration <= 0) {
                // TODO: is LMD time reliable?? use more complex GetVideoDuration instead??
                // read endtime from LMD
                CString csEnd;
                pClipTag->GetElementParameter(_T("end"), csEnd);
                int iEnd = _ttoi(csEnd);
                iDuration = iEnd - pClipInfoStructured->clipInfo->GetTimestamp();
                if (iDuration <= 0)
                    // read duration from clip
                    VideoInformation::GetVideoDuration(pClipInfoStructured->clipInfo->GetFilename(), iDuration);
                // set length (will implicitly set endtime)
                pClipInfoStructured->clipInfo->SetLength(iDuration);
            }
        } else {
            TRACE("NO CORRESPONDING CLIP FOUND AT %i\n", iBegin);
            hr = E_FAIL;
        }
    } 

    UpdateProgressSubtask(hr, 5);

   /////////////////////////////////////////////////////////
   // compute clip structure (timestamps)
   /////////////////////////////////////////////////////////
   if(SUCCEEDED(hr))
      hr = CreateClipStructure(pClipInfoStructured);

   UpdateProgressSubtask(hr, 95);


   /////////////////////////////////////////////////////////
   // set structure in LMD
   /////////////////////////////////////////////////////////

   // prepare current structure to add clip structure; adding an additional chapter if required
   if (SUCCEEDED(hr)) {
       SgmlElement *pParentTag = pClipTag->GetParent();
       if(pParentTag != NULL && _tcscmp(pParentTag->GetName(),_T("structure")) == 0) {
           // pure screengrabbing clip - the clipTag is the only element of the structure
           // according to DTD we must add an additional chapterTag before we can add the clip structure 

           // create new chapter tag
           SgmlElement *pChapterTag = new SgmlElement();
           if (pChapterTag != NULL) {
               // default title
               CString csChapter;
               csChapter.LoadString(IDC_CHAPTER);
               CString csChapterTitle;
               csChapterTitle.Format(_T("%s 1"), csChapter);

               // set chapter title to title of recording (analog to surrounding chaper of WB recordings)
               SgmlElement *pTitleTag = m_pLmdFile->Find(_T("title"), _T("metainfo"));
               if (pTitleTag)
                   csChapterTitle.Format(_T("%s"), pTitleTag->GetParameter());

               pChapterTag->SetName(_T("chapter"));
               // chapterTag->SetAttribute(_T("title"),clipTag->Find(_T("title"))->GetParameter());
               pChapterTag->SetAttribute(_T("title"), csChapterTitle);
               pChapterTag->SetAttribute(_T("nr"), pClipTag->Find(_T("nr"))->GetParameter());

               // attach chapter tag to structure tag
               CArray<SgmlElement *, SgmlElement *> aParentElements;
               pParentTag->GetElements(aParentElements);
               pChapterTag->SetElements(aParentElements);
               pParentTag->RemoveAllElements(false);
               pParentTag->Append(pChapterTag);
           } else {
               TRACE("  out of memory: cannot add indices\n");
               hr = E_OUTOFMEMORY;
           }
       }
   }

   // add new clip structure, i.e. clip tag will be replaced by sequence of page tags
   CArray<SgmlElement *, SgmlElement *> aPages;
   if (SUCCEEDED(hr)) {
       CString csSgmlPrefix = m_pLrdFile->GetFilenamePrefix();
       int iOffset = pClipInfoStructured->clipInfo->GetTimestamp();

       POSITION position = pClipInfoStructured->structure.GetHeadPosition();
       while (position != NULL && SUCCEEDED(hr)) {
           // get page type (create as result of mouse click or analysis)
           CStructureElement *pStructureElement = pClipInfoStructured->structure.GetAt(position);
           CString csType;
           if (pStructureElement != NULL && pStructureElement->m_bClickPage )
               csType = _T("click");
           else
               csType = _T("analysis");

           // add action with click info
           UserAction *pAction = NULL;
           if (pStructureElement != NULL && pStructureElement->hasAction()) {
               pAction = &(pStructureElement->m_action);
               // adjust timestamp
               pAction->iTimeMsec += iOffset;
           }

           // get start and end time (with offset in relation to beginning of recording)
           int iStartTime = iOffset;
           if (pStructureElement != NULL)
               iStartTime += pStructureElement->timestamp;
           
           // The start timestamp of the next element is needed (if exists)
           pClipInfoStructured->structure.GetNext(position);
           int iEndTime = iOffset;
           if (position != NULL) {
               // endtime is beginning of next index
               CStructureElement *pNextStructureElement = pClipInfoStructured->structure.GetAt(position);
               iEndTime += pNextStructureElement->timestamp;
           } else
               // last index: endtime is end of clip
               iEndTime += pClipInfoStructured->clipInfo->GetLength(); // GetEndTime();

           // create and initialize page tag
           ++m_iClipIndexCounter;

           CString csPage;
           csPage.LoadString(IDC_PAGE);
           CString csPageTitle;
           csPageTitle.Format(_T("%ls %i"), csPage, m_iClipIndexCounter);

           bool bHasAction = false;
           int iXcoord = 0;
           int iYcoord = 0;
           CString csButtonType;
           if (pAction != NULL) {
               bHasAction = true;
               iXcoord = pAction->iXcoord;
               iYcoord = pAction->iYcoord;
               if (pAction->dwEventType == MH_LEFT_BUTTON)
                   csButtonType = _T("left");
               else if (pAction->dwEventType == MH_MIDDLE_BUTTON)
                   csButtonType = _T("middle");
               else if (pAction->dwEventType == MH_RIGHT_BUTTON)
                   csButtonType = _T("right");
           }
           SgmlElement *pPageTag = CLmdSgml::CreatePageTag(csPageTitle, m_iClipIndexCounter, iStartTime, iEndTime, csSgmlPrefix, 
                                                           bHasAction, iXcoord, iYcoord, csButtonType, csType);
           
           if (pPageTag != NULL)
               aPages.Add(pPageTag);
           else    {
               TRACE("  out of memory: cannot add indices\n");
               hr = E_OUTOFMEMORY;
           }

           // Create lep metadata flag for each slide.
           // These tags are only used if the lep file should be completed.
           if (m_pLepFile != NULL) {
               m_pLepFile->AddSlidesMetadata(csPageTitle, m_iClipIndexCounter, iStartTime, iEndTime);

               if (pStructureElement != NULL && bHasAction &&
                   pStructureElement->m_action.pElementInfo != NULL) {
                   CRect rcBounds;
                   pStructureElement->m_action.pElementInfo->GetBounds(rcBounds);
                   bool bBoundsDefined = rcBounds.Height() > 0 && rcBounds.Width() > 0;

                   // LiveContext delivers absolute coordinates
                   // translate to relative coordinates (relative within recorded area)
                   if ( bBoundsDefined )
                       rcBounds.OffsetRect( pStructureElement->m_action.iLiveContextXoffset, 
                                            pStructureElement->m_action.iLiveContextYoffset );

                   // add CLICK to LEP
                   CLiveContextElementInfo *pElementInfo = pStructureElement->m_action.pElementInfo;
                   m_pLepFile->AddClickInformation(pStructureElement->m_action.iXcoord, pStructureElement->m_action.iYcoord, 
                                                   pStructureElement->m_action.iTimeMsec,
                                                   pElementInfo->GetAccID(), pElementInfo->GetUiAutoID(),
                                                   pElementInfo->GetUiAutoTreeID(), pElementInfo->GetSapID(),
                                                   //pElementInfo->GetTimestamp(), 
                                                   rcBounds,
                                                   pElementInfo->GetName(), pElementInfo->GetProcessName(),
                                                   pElementInfo->GetAccType(),
                                                   pElementInfo->GetSapType());
               }
           }
       }

       if (aPages.IsEmpty())
           hr = E_FAIL;
   }

   if (SUCCEEDED(hr)&& m_pLepFile != NULL)
       m_pLepFile->Write();

   if (SUCCEEDED(hr)) {
       // moved csBegin into method
       //CString csBegin;
       //csBegin.Format(_T("%d"), pClipInfoStructured->clipInfo->GetTimestamp());
       //m_pLmdFile->ReplaceClipWithPages(pClipTag, aPages, csBegin);
       m_pLmdFile->ReplaceClipWithPages(pClipTag, aPages);
   }

   if (SUCCEEDED(hr)) {
       // structure is available
       m_pLrdFile->SetClipStructureAvailable(pClipInfoStructured->GetId(),true);
       pClipInfoStructured->clipInfo->SetStructureAvailable(true);

       m_bWriteLrd = true;
       m_bWriteLmd = true;
       m_bWriteEvqAndObj = true;

       // add to list of structured clips
       m_listStructuredClips.AddTail(pClipInfoStructured);
       pClipInfoStructured = NULL;
   }

   // If there occurs an error the clipInfoStructured is created but not added => delete it 
   if (pClipInfoStructured != NULL)
       delete pClipInfoStructured;

   // set progress to completed but ignore cancelation because everything is done
   HRESULT dummy;
   UpdateProgressSubtask(dummy, 100);

   return hr;
}



// static
HRESULT CScreengrabbingExtendedExternal::RemoveStructure(CString csLrdFilename) {
   // TODO: error handling
   bool bSuccess = true;

   // load LRD file if specified; check if loaded otherwise
   CLrdFile *pLrdFile;
   if(bSuccess) {
      if(!csLrdFilename.IsEmpty()) {
         //TRACE("  LOADING LRD %ls\n",csLrdFilename);
         pLrdFile = new CLrdFile();

         if (!pLrdFile->Open(csLrdFilename))
         {
            //TRACE("  LOADING LRD FAILED - %ls\n",csLrdFilename);
            delete pLrdFile;
            bSuccess = false;
         }
      }
      if(!pLrdFile)
         bSuccess = false;
   }

   // determine file names
   csLrdFilename;
   csLrdFilename.Format(_T("%ls\\%ls"), pLrdFile->GetRecordPath(), pLrdFile->GetFilename());
   CString csLrdBackupFile;
   csLrdBackupFile.Format(_T("%ls.orig"),csLrdFilename);

   CString csLmdFilename;
   csLmdFilename.Format(_T("%ls"), pLrdFile->GetMetadataFilename());
   CString csLmdBackupFile;
   csLmdBackupFile.Format(_T("%ls.orig"), csLmdFilename);

   // Bugfix 5163: inconsistancy if removing structure after export
   // only restore if "*.orig" files are available
   if(bSuccess) {
      WIN32_FIND_DATA FindFileData;
      HANDLE hFind = FindFirstFile(csLrdBackupFile, &FindFileData);
      bSuccess = (hFind != INVALID_HANDLE_VALUE);

      // Bugfix 5512: structure not removable if renamed after creation
      // try to use prefix of lmd (which is not renamed)
      if(!bSuccess) {
         csLrdBackupFile = csLmdFilename;
         StringManipulation::GetPathFilePrefix(csLrdBackupFile);
         csLrdBackupFile += _T(".lrd.orig");
         hFind = FindFirstFile(csLrdBackupFile, &FindFileData);
         bSuccess = (hFind != INVALID_HANDLE_VALUE);
      }
   }
   if(bSuccess) {
      WIN32_FIND_DATA FindFileData;
      HANDLE hFind = FindFirstFile(csLmdBackupFile, &FindFileData);
      bSuccess = (hFind != INVALID_HANDLE_VALUE);
   }


      if(bSuccess) {
         // restore LRD from backup 
         WIN32_FIND_DATA FindFileData;
         HANDLE hFind = FindFirstFile(csLrdBackupFile, &FindFileData);
         if (hFind != INVALID_HANDLE_VALUE)  {
            // NOTE: TRUE says FailIfExists and NOT overwrite
            bSuccess = (CopyFile(csLrdBackupFile, csLrdFilename, FALSE) != 0);
            if(bSuccess)
               DeleteFile(csLrdBackupFile);
         } else 
            bSuccess = false;
      }

      // Bugfix 5512: PNGs not removed if renamed after creation
      CString csPngPrefix;
      if(bSuccess) {
         // determine png prefix from lmd
         // note: maybe not reliable under certain circumstances
         SgmlFile lmdFile;
         bool bResult = lmdFile.Read(pLrdFile->GetMetadataFilename(), _T("<DOC"));
         if ( bResult && lmdFile.GetRoot() ) {
            SgmlElement *thumbNode = lmdFile.GetRoot()->Find(_T("thumb"), _T("page"));
            if ( thumbNode ) {
               csPngPrefix = thumbNode->GetParameter();
               if(csPngPrefix.Right(4) == _T(".png"))
                  csPngPrefix.Delete(csPngPrefix.GetLength()-4, 4);
               if(csPngPrefix.Right(4) == _T("_thumb"))
                  csPngPrefix.Delete(csPngPrefix.GetLength()-6, 6);
            }
         }
      }

      if(bSuccess) {
         // restore LMD from backup 
         WIN32_FIND_DATA FindFileData;
         HANDLE hFind = FindFirstFile(csLmdBackupFile, &FindFileData);
         if (hFind != INVALID_HANDLE_VALUE)  {
            bSuccess = (CopyFile(csLmdBackupFile, csLmdFilename, FALSE) != 0);
            if(bSuccess)
               DeleteFile(csLmdBackupFile);
         } else 
            bSuccess = false;
      }
      if(bSuccess) {
         // TODO: restore OBJ/EVQ (if WB with CLIP)
         // remove OBJ/EVQ files
         //TRACE("  delete %ls\n",pLrdFile->GetEvqFilename());
         //TRACE("  delete %ls\n",pLrdFile->GetObjFilename());
         DeleteFile(pLrdFile->GetEvqFilename());
         DeleteFile(pLrdFile->GetObjFilename());
      }
      if(bSuccess) {
         // remove thumbnail info file
         CString csDeleteFile;
         csDeleteFile.Format(_T("%ls\\_internal_thumbinfo"), pLrdFile->GetRecordPath());
         DeleteFile(csDeleteFile);

         // check all files in recording path and remove keyframe/thumbnail files
         CString csDirectory;
         csDirectory.Format(_T("%ls\\*"), pLrdFile->GetRecordPath());
         WIN32_FIND_DATA FindFileData;
         HANDLE hFind = FindFirstFile(csDirectory, &FindFileData);

         if (hFind != INVALID_HANDLE_VALUE) 
         {
            // check all files in recording path
            if ( csPngPrefix.IsEmpty() )
               csPngPrefix = pLrdFile->GetFilenamePrefix();

            while (FindNextFile(hFind, &FindFileData) != 0) 
            {
               CString csFile(FindFileData.cFileName);
               if(csFile.Right(4) == _T(".png")) {
                  // PNG image found
                  CString csString = csFile.Left(4);
                  // remove thumbnail header in file name
                  if(csString == _T("100_") || csString == _T("150_") || csString == _T("200_"))
                     csFile.Delete(0,4);
                  // test if document file prefix matches
                  if(csFile.Left(csPngPrefix.GetLength()) == csPngPrefix) {
                     // delete matching PNG file
                     csDeleteFile.Format(_T("%ls\\%ls"), pLrdFile->GetRecordPath(), FindFileData.cFileName);
                     DeleteFile(csDeleteFile);
                  }
               }
            }
            // close handle
            FindClose(hFind);
         }
      }
      SAFE_DELETE(pLrdFile);
      return bSuccess ? S_OK : E_FAIL;
}

////////////////////////////////////////////////////
// IMAGING / SCREENSHOTS / THUMBNAILS
////////////////////////////////////////////////////

// create screenshots for all structured clips
bool CscreengrabbingDoc::CreateScreenshots() {
   // TODO: should return HRESULT
   bool result = true;

   // get number of images to be computed
   int count = 0;
   POSITION position = m_listStructuredClips.GetHeadPosition();
   while (position != NULL) {   
      //CList<CStructureElement,CStructureElement> structure = m_listStructuredClips.GetNext(position)->structure;
      ////if(structure.IsEmpty())
      //   count += structure.GetCount();
      count += m_listStructuredClips.GetNext(position)->structure.GetCount();
   }

   // update progress dialog
   TRACE("COMPUTING SCREENSHOTS OF %i INDICES\n", count);
   if(m_pExportProgressDialog) {
      m_pExportProgressDialog->SetParticularSteps(2*count);
      m_pExportProgressDialog->SetParticularProgress(0);
   }


   // compute images for indices of structured clips
   position = m_listStructuredClips.GetHeadPosition();
   while (position != NULL) {   
      if(m_pExportProgressDialog && m_pExportProgressDialog->IsCanceled())
         break;
      result &= CreateScreenshots(m_listStructuredClips.GetNext(position));
   }

   // update progress dialog
   if(m_pExportProgressDialog)
      m_pExportProgressDialog->SetParticularProgress(2*count);

   return result;
}

// BUGFIX 5823: update progress & handle cancel
// Helper to update progress and check cancel from outside
CscreengrabbingDoc *CscreengrabbingDoc::pScreengrabbingExtended = NULL;
HRESULT CscreengrabbingDoc::updateProgress() {   
   HRESULT hr = S_OK;
   if ( pScreengrabbingExtended != NULL && pScreengrabbingExtended->m_pExportProgressDialog != NULL ) {
      if ( pScreengrabbingExtended->m_bCanceled || pScreengrabbingExtended->m_pExportProgressDialog->IsCanceled() ) {
         pScreengrabbingExtended->m_bCanceled = true;
         hr = E_ABORT;
      } else {
         pScreengrabbingExtended->m_pExportProgressDialog->IncParticularProgress();
      }
   }
   return hr;
}

bool CscreengrabbingDoc::CreateScreenshots(CClipInfoStructured *clipInfoStructured) {
   // TODO: improve performance of VideoInformation::GetPosterFrame (to compute all images at once)
   // TODO: use AVI Filter instead 

   //// get clip resolution
   //SIZE size;
   //VideoInformation::GetVideoSize(clipInfoStructured.clipInfo->GetFilename(), size);
   //TRACE("  clip resolution: %i x %i\n", size.cx, size.cy);
   //double framerate;
   //VideoInformation::GetVideoFramerate(clipInfoStructured.clipInfo->GetFilename(), framerate);
   //TRACE("  clip framerate: %f\n", framerate);
   //TRACE("  time offset %i msec\n", offset);

   if(clipInfoStructured->clipInfo->GetImagesAvailable()) {
      TRACE("  skipping structured clip with images\n");
      return true;
   }

   bool result = true;

   // get filename prefix for thumbnail filenames
   CString csSgmlPrefix = m_pLrdFile->GetFilenamePrefix();
   CString csPath = m_pLrdFile->GetRecordPath();
   CString csImageFilepath = clipInfoStructured->clipInfo->GetFilename();
   StringManipulation::GetPathFilePrefix(csImageFilepath);

   int offset = clipInfoStructured->clipInfo->GetTimestamp();

   int count = 0;

   // determine framerate
   double dFrameRate = 0;
   VideoInformation::GetVideoFramerate(clipInfoStructured->clipInfo->GetFilename(), dFrameRate);

   // get encoder fpr PNG images
   CLSID pngClsid;
   VideoInformation::GetEncoderClsid(L"image/png", &pngClsid);


   // BUGFIX 5684: incorrect thumbnails:
   // using alternative implementation to grab poster frames from video
   // first get timestamps
   CList<int> timestamps;
   POSITION position = clipInfoStructured->structure.GetHeadPosition();
   while (position != NULL)
      timestamps.AddTail(clipInfoStructured->structure.GetNext(position)->timestamp);  

   // BUGFIX 5823: update progress & handle cancel
   // a little hack to initialize callback helper
   pScreengrabbingExtended = this;

   // now compute all images at once
   CString imageFilenamePrefix;
   imageFilenamePrefix.Format(_T("%ls\\%ls_"), csPath, csSgmlPrefix);
   HRESULT hr = WritePosterFrames(clipInfoStructured->clipInfo->GetFilename(), imageFilenamePrefix, &timestamps, offset, &updateProgress);
   if ( hr != S_OK )
      result = false;

   pScreengrabbingExtended = NULL;

   position = clipInfoStructured->structure.GetHeadPosition();
   while (result && position != NULL) {
      if(m_pExportProgressDialog && m_pExportProgressDialog->IsCanceled())
         break;
      int streamTime = clipInfoStructured->structure.GetNext(position)->timestamp;

      // set image file name
      CString imageFilename;
      imageFilename.Format(_T("%ls\\%ls_%i.png"), csPath, csSgmlPrefix, streamTime+offset);

      // update progress dialog
      if(m_pExportProgressDialog) {
         m_pExportProgressDialog->IncParticularProgress();
      }

      // BUGFIX 5684: incorrect thumbnails:
      // using alternative implementation to grab poster frames from video (see above outside loop)
      /*
      // write image
      // TODO: instead use: GetPosterFrame(tszVideoFileName, width, height, &bitmap, streamTimeMsec)){
      if(VideoInformation::WritePosterFrame(imageFilename, clipInfoStructured->clipInfo->GetFilename(), streamTime)){
         TRACE("  screenshot at %i msec created: %ls\n", streamTime+offset, imageFilename);
      } else {
         TRACE("  screenshot at %i msec FAILED\n", streamTime+offset);
         result = false;
      }
      */


      /* removed: not fully working because not fully synchronized
      ///////////////////////////////////////////////////////////////////
      // restore screenshot without mouse pointer (required in DEMO MODE)
      ///////////////////////////////////////////////////////////////////
      // get original background behind mouse pointer
      CString csPointerBackgroundPath = clipInfoStructured->clipInfo->GetFilename();
      StringManipulation::GetPathFilePrefix(csPointerBackgroundPath);
      CString csPointerBackground;
      // TODO: is this precise? rounding errors?
      int iFrameNr = streamTime * dFrameRate / 1000;
      csPointerBackground.Format(_T("%s_%d.png"), csPointerBackgroundPath, iFrameNr);
      Image *pPointerBackgroundImage = Image::FromFile(csPointerBackground);

      // get current screenshot
      Image *pImage = Image::FromFile(imageFilename);

      // paint mouse pointer background (everything except pointer area is transparent)
      Graphics *newGraphics = Graphics::FromImage(pImage);
      newGraphics->DrawImage( pPointerBackgroundImage, 0, 0 );

      // write temp image (Image cannot write to same file)
	  // TODO: avoid temp file if possible
      CString imageFilenameTemp = imageFilename+".png";
      pImage->Save(imageFilenameTemp, &pngClsid, NULL);
      delete newGraphics;
      delete pImage;
	  
      // copy temp
      pImage = Image::FromFile(imageFilenameTemp);
      pImage->Save(imageFilename, &pngClsid, NULL);
      // TRACE("--------> pointer restore %i msec; frame %i; %ls\n", streamTime+offset, iFrameNr, imageFilename);
      */


      //////////////////////////////////////////////////////////////
      // create thumbnails in three sizes: max 100, 150 & 200 pixel
      //////////////////////////////////////////////////////////////

      // get current screenshot
      Image *pImage = Image::FromFile(imageFilename);

      for ( int width = 100; width <=200; width += 50 ) {
         // set thumbnail size
         int height = width;
         int iNewImageWidth;
         int iNewImageHeight;
         // set thumbnail image file name
         imageFilename.Format(_T("%ls\\%i_%ls_%i_thumb.png"), csPath, width, csSgmlPrefix, streamTime+offset);
         // get scaled thumbnail image
         Calculator::FitRectInRect(width, height, pImage->GetWidth(), pImage->GetHeight(), &iNewImageWidth, &iNewImageHeight);
         Image *pThumbnail = pImage->GetThumbnailImage(iNewImageWidth, iNewImageHeight);
         // draw border
         Graphics *newGraphics = Graphics::FromImage(pThumbnail);
         Pen pen(Color::Black);
         newGraphics->DrawRectangle(&pen, 0, 0, pThumbnail->GetWidth()-1, pThumbnail->GetHeight()-1);
         // write thumbnail image
         pThumbnail->Save(imageFilename, &pngClsid, NULL);
         delete newGraphics;
         delete pThumbnail;
      }

      // free image memory
      delete pImage;
      //DeleteFile(imageFilenameTemp);

      // BUGFIX 5505: "_internal_thumbinfo" no required anymore
      /*
      // create additional info file to support multiple thumnbnail sizes (_internal_thumbinfo)
      WriteInternalThumbinfo();
      */
   }

   // images for thumbnail creation are available
   if (result && (m_pExportProgressDialog==NULL || !m_pExportProgressDialog->IsCanceled())) {
      m_pLrdFile->SetClipImagesAvailable(clipInfoStructured->GetId(),true);
      clipInfoStructured->clipInfo->SetImagesAvailable(true);

      m_bWriteLrd = true;
      m_bWriteEvqAndObj = true;
   }

   // TODO: error handling
   return result;
}

// BUGFIX 5505: "_internal_thumbinfo" no required anymore
/*
bool CscreengrabbingDoc::WriteInternalThumbinfo() {
   // create _internal_thumbinfo to support multiple thumbnail sizes
   // TODO: may be transfered to FileSDK WhiteboardStream
   // TODO: error handling

   CString fileName;
   fileName.Format(_T("%ls\\_internal_thumbinfo"), m_pLrdFile->GetRecordPath());

   CFileOutput *fp;
   HANDLE m_hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (m_hFile == INVALID_HANDLE_VALUE)
   {
      TRACE("ERROR WRITING EMPTY _internal_thumbinfo FILE\n");
      return false;
   } 
   else 
   {
      fp = new CFileOutput;
      fp->Init(m_hFile, 0, false);
   }

   // write content
   fp->Append(_T("<thumbs>\r\n"));
   fp->Append(_T("  <size>100</size>\r\n"));
   fp->Append(_T("  <size>150</size>\r\n"));
   fp->Append(_T("  <size>200</size>\r\n"));
   fp->Append(_T("</thumbs>\r\n"));
   fp->Flush();

   // close file handles
   CloseHandle(m_hFile);
   delete fp;
   fp = NULL;

   return true;
}
*/


////////////////////////////////////////////////////
// CHARACTER RECOGNITION / FULL TEXT SEARCH
////////////////////////////////////////////////////


// create fulltext for all structured clips
bool CscreengrabbingDoc::CreateFulltext() {
   bool result = true;

   // count number of images to be computed to adjust progress dialog
   int count = 0;
   POSITION position = m_listStructuredClips.GetHeadPosition();
   while (position != NULL) {   
      //CList<CStructureElement,CStructureElement> structure = m_listStructuredClips.GetNext(position)->structure;
      ////if(structure.IsEmpty())
      //   count += structure.GetCount();
      count += m_listStructuredClips.GetNext(position)->structure.GetCount();
   }

   // update progress dialog
   TRACE("COMPUTING FULLTEXT OF %i INDICES\n", count);
   if(m_pExportProgressDialog) {
      m_pExportProgressDialog->SetParticularSteps(count);
      m_pExportProgressDialog->SetParticularProgress(0);
   }


   // compute fulltext for indices of structured clips
   position = m_listStructuredClips.GetHeadPosition();
   while (position != NULL) {   
      if(m_pExportProgressDialog && m_pExportProgressDialog->IsCanceled())
         break;
      result &= CreateFulltext(m_listStructuredClips.GetNext(position));
   }

   // update progress dialog
   if(m_pExportProgressDialog)
      m_pExportProgressDialog->SetParticularProgress(count);

   return result;
}

bool CscreengrabbingDoc::CreateFulltext(CClipInfoStructured *clipInfoStructured) {
   // TOD: error handling

   if(clipInfoStructured->clipInfo->GetFulltextAvailable()) {
      TRACE("  skipping structured clip with fulltext\n");
      return true;
   }

   HRESULT hr = S_OK;

   // get filename prefix for thumbnail filenames
   CString csSgmlPrefix = m_pLrdFile->GetFilenamePrefix();

   bool result = false;

   int offset = clipInfoStructured->clipInfo->GetTimestamp();

   int count = 0;

   // TODO: select TopOCR or other OCR
   bool topOcr = true;
   bool abbyyOcr = !true;

   CString topOcrWorkingDirectory(m_pLrdFile->GetRecordPath());

   CString topOcrExecutable;
   if(topOcr) {
      // test if TopOCR is available
      // TODO: error handling

      // install path
      CString csInstallPath = JNISupport::GetInstallDir();
      topOcrExecutable.Format(_T("%lsBackend\\Helper\\scan.exe"), csInstallPath);

      TRACE("searching for OCR : %ls\n", topOcrExecutable);

      // try to get file attributes
      struct _stat64i32 stFileInfo;
      if(_wstat(topOcrExecutable.GetBuffer(),&stFileInfo) != 0) {
         TRACE("OCR not found: %ls\n", topOcrExecutable);
         return false;

      }else{
         TRACE("OCR found: %ls\n", topOcrExecutable);
      }
   }

   /////////////////////////////////
   // loop to process all images 
   /////////////////////////////////
   POSITION position = clipInfoStructured->structure.GetHeadPosition();
   while (position != NULL) {
      if(m_pExportProgressDialog && m_pExportProgressDialog->IsCanceled())
         break;

      // update progress dialog
      if(m_pExportProgressDialog) {
         m_pExportProgressDialog->IncParticularProgress();
      }

      /////////////////
      // read image 
      /////////////////
      CStructureElement *structureElement = clipInfoStructured->structure.GetNext(position);
      int streamTime = structureElement->timestamp;
      // set image file name
      // TODO: filename fails after Editor Export because timestamps are removed from filename
      CString imageFilename;
      imageFilename.Format(_T("%ls\\%ls_%i.png"), topOcrWorkingDirectory, csSgmlPrefix, streamTime+offset);
      // load image
      //TRACE("LOAD: %ls\n", imageFilename);
      CImage image;
      hr = image.Load(imageFilename);


      /////////////////
      // call OCR
      /////////////////
      if(SUCCEEDED(hr)) {
         if(abbyyOcr) {            
            //TRACE("FR OCR : %ls\n", imageFilename);
            //CList<DrawSdk::Text*,DrawSdk::Text*> *textObjects = GetTextFromImage(imageFilename);
            //if(!structureElement->textObjects)
            //   structureElement->textObjects = new CList<DrawSdk::Text*,DrawSdk::Text*>();
            //if(structureElement->textObjects)
            //   structureElement->textObjects->AddTail(textObjects);
            //result = true;

         }else

         ///////////////////////
         // TopOCR
         ///////////////////////
         if(topOcr) {
            //TRACE("OCR : %ls\n", imageFilename);
            // TODO: test if OCR is available
            // TODO: set OCR path
            // TODO: error handling

            CString topOcrInputFileName;
            topOcrInputFileName.Format(_T("%ls\\ocr.bmp"),topOcrWorkingDirectory);
            CString topOcrOutputFileName;
            topOcrOutputFileName.Format(_T("%ls\\ocr.txt"),topOcrWorkingDirectory);

            ///////////////////////
            // convert image
            ///////////////////////
            // TopOCR requires 24 bit BMP files, hence we must convert PNG files
            CImage imageBmp;
            if(image.GetBPP() == 24) {
               // bit per pixel are fine
               imageBmp = image;
            } else {
               // convert to 24 bits per pixel
               imageBmp.Create(image.GetWidth(),image.GetHeight(),24);
               image.Draw(imageBmp.GetDC(),0,0);
               imageBmp.ReleaseDC();
            }
            // write temp ocr input
            hr = imageBmp.Save(topOcrInputFileName);

            ///////////////////////
            // call TopOCR
            ///////////////////////
            if(SUCCEEDED(hr)) {
               // create parameter
               //
               // "TopOCR inputimage.ext [options\parameters] textoutput.ext"
               //
               // -THRESH Param
               // -RESENHANCE
               // -SHARPEN Param
               // -DESPECKLE Param
               // -LANGUAGE Param
               // The default LANGUAGE value is English, otherwise this needs to be set to one of the following:
               // ENG, FRN, GRM, ITL, SPN, POR, SWD, DAN, NOR, DCH, FIN
               //
               // default parameters used in TopOCR application: -RESENHANCE -THRESH 6
               // additionally set language parameters if required (based on LECTURNITY language version)
               _TCHAR tszLanguageCode[128];
               bool ret = KerberokHandler::GetLanguageCode(tszLanguageCode);
               CString csLanguage = tszLanguageCode;
               CString topOcrParameters;
               if (ret && csLanguage == "de") // german
                  topOcrParameters = "ocr.bmp -RESENHANCE -THRESH 6 -LANGUAGE GRM ocr.txt";
               else // default english
                  topOcrParameters = "ocr.bmp -RESENHANCE -THRESH 6 ocr.txt";


               //TRACE("OCR: %ls %ls\n", topOcrExecutable, topOcrParameters);
               //TRACE(" in: %ls\n", topOcrWorkingDirectory);

               // remove ocr text file because TopOCR appends results
               DeleteFile(topOcrOutputFileName);

               // prepare execution
               HINSTANCE h = 0;
               SHELLEXECUTEINFO ExecInfo;

               ExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
               ExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
               ExecInfo.hwnd = GetActiveWindow();
               ExecInfo.lpVerb = _T("open");
               ExecInfo.lpFile = topOcrExecutable;
               ExecInfo.lpParameters = topOcrParameters;
               ExecInfo.lpDirectory = topOcrWorkingDirectory;
               ExecInfo.nShow = SW_SHOWNORMAL;
               ExecInfo.hInstApp = h;

               // execute TopOCR
               ShellExecuteEx(&ExecInfo);

               // wait until finished
               // TODO: what is a valid timeout
               WaitForSingleObject(ExecInfo.hProcess, INFINITE /*3600*/);
            }
            ///////////////////////
            // read results (ASCII file)
            ///////////////////////
            if(SUCCEEDED(hr)) {

               //TRACE("\n-------------------------------------\n");

               wchar_t *filename = topOcrOutputFileName.GetBuffer();

               FILE *pfile;
               //unsigned char buffer[80];
               int count = 0;
               int lines = 0;
               int i = 0;

               if((pfile = _wfopen(filename, _T("rb"))) == NULL){
                  TRACE("Sorry, can't open %ls", filename);
                  //return -1;
               }else{
                  result = true;

                  // read chars and create words
                  CString text;
                  bool lineEnd = true;
                  while(!feof(pfile))
                  {
                     //if(count < sizeof buffer) /* If the buffer is not full  */
                     unsigned char ch = (unsigned char)fgetc(pfile);
                     //if(ch!=' ' && ch!='\n' && ch!='\r' && ch!='\t') {
                     if(ch!='\n' && ch!='\r' && ch!='\t') {
                        // append character
                        text += ch;
                        lineEnd = false;
                     } else if(!lineEnd) {
                        if(!text.IsEmpty()) {                     
                           // end of word - add word   
                           //TRACE(text);
                           //TRACE("\n");

                           DrawSdk::Text *textObject = CreateTextObject(text);
                           if(!structureElement->textObjects)
                              structureElement->textObjects = new CList<DrawSdk::Text*,DrawSdk::Text*>();
                           if(structureElement->textObjects)
                              structureElement->textObjects->AddTail(textObject);

                           text.Empty();
                        }
                        lineEnd = true;
                     }

                  }
                  if(!text.IsEmpty()) {
                     // add last word                  
                     //TRACE(text);
                     //TRACE("\n");

                     DrawSdk::Text *textObject = CreateTextObject(text);
                     if(!structureElement->textObjects)
                        structureElement->textObjects = new CList<DrawSdk::Text*,DrawSdk::Text*>();
                     if(structureElement->textObjects)
                        structureElement->textObjects->AddTail(textObject);

                     text.Empty();
                  }

                  fclose(pfile);

                  //TRACE(text);
                  // remove temp files
                  DeleteFile(topOcrOutputFileName);
                  DeleteFile(topOcrInputFileName);

                  //TRACE("\n-------------------------------------\n");
               }
            }
         } else {
            // TODO:
            TRACE("OCR : %ls\n", imageFilename);
            TRACE("  currently not supported!!!!!!!!\n");

            result = false;
         }
      }else
         TRACE("LOAD: %ls FAILED\n", imageFilename);
   }

   // fulltext is available
   if (SUCCEEDED(hr) && result && (m_pExportProgressDialog==NULL || !m_pExportProgressDialog->IsCanceled())) {
      m_pLrdFile->SetClipFulltextAvailable(clipInfoStructured->GetId(),true);
      clipInfoStructured->clipInfo->SetFulltextAvailable(true);
      
      m_bWriteLrd = true;
      m_bWriteEvqAndObj = true;
   }

   // TODO: error handling
   return result;
}

HRESULT CscreengrabbingDoc::UpdateEvqAndObj() {
   HRESULT hr = S_OK;

   // TODO: error handling

   // TODO: add events and objects to existing EVQ/OBJ - DONE??

   // clear previous whiteboard
   SAFE_DELETE(m_pWhiteboardStream);

   // file names
   CString csPath = m_pLrdFile->GetRecordPath();
   CString csFilePrefix = m_pLrdFile->GetFilename();
   StringManipulation::GetFilePrefix(csFilePrefix);
   CString csEvqFilename = "";
   CString csObjFilename = "";

   SIZE size;

   // TODO: think about first event
   boolean bAddEventAtZero = false;
   boolean bAddEventAtEnd = false;

   // read exististing whiteboard stream (if available)
   m_pWhiteboardStream = m_pLrdFile->CreateWhiteboardStream();
   if(m_pWhiteboardStream) {
      csEvqFilename = m_pWhiteboardStream->GetEvqFilename();
      StringManipulation::GetFilename(csEvqFilename);
      m_pWhiteboardStream->SetEvqFilename(csEvqFilename);
      csObjFilename = m_pWhiteboardStream->GetObjFilename();
      StringManipulation::GetFilename(csObjFilename);
      m_pWhiteboardStream->SetObjFilename(csObjFilename);
      CSize dimPage;
      if(m_pWhiteboardStream->GetPageDimension(dimPage)) {
         size.cx = dimPage.cx;
         size.cy = dimPage.cy;
      }
   } else {
      // create new whiteboard
      m_pWhiteboardStream = new CWhiteboardStream();

      // get clip resolution
      if(m_pClipStream && m_pClipStream->HasClips()) {
         // TODO: check if all clips have the same resolution
         VideoInformation::GetVideoSize(m_pClipStream->m_arrClip[0]->GetFilename(), size);
         //TRACE("  clip resolution: %i x %i\n", size.cx, size.cy);
      } else {
         //TRACE("  clip resolution unknown - set to 1024 x 768\n");
         size.cx = 1024; 
         size.cy = 768;
      }

      // set whiteboard dimensions
      CString csDimensions;
      csDimensions.Format(_T("%ix%i"), size.cx, size.cy);
      TRACE("   Whiteboard dimensions set to: %ls\n", csDimensions);
      m_pWhiteboardStream->SetPageDimension(csDimensions); 

      // set file names
      csEvqFilename.Format(_T("%ls.evq"), csFilePrefix);
      csObjFilename.Format(_T("%ls.obj"), csFilePrefix);

      m_pWhiteboardStream->SetObjFilename(csObjFilename);
      m_pWhiteboardStream->SetEvqFilename(csEvqFilename);

      // TODO: think about first event
      bAddEventAtZero = true;
      bAddEventAtEnd = true;
   }



   // add events for each structured clip
   // initiate according to existing pages

   // TODO: is this required?
   SAFE_DELETE(m_pPageStream);
   m_pPageStream = m_pWhiteboardStream->CreatePageStreamFromEvents();
   int iPageCount = m_pWhiteboardStream->m_pageCounter;

   int iClipCount = 0;
   POSITION clipPos = m_listStructuredClips.GetHeadPosition();
   while(SUCCEEDED(hr) && clipPos != NULL) {
      CClipInfoStructured *clipInfoStructured = m_listStructuredClips.GetNext(clipPos);
      iClipCount++;
      TRACE("  CLIP #%i at %i msec:\n", iClipCount, clipInfoStructured->clipInfo->GetTimestamp());

      bool bWriteImages = clipInfoStructured->clipInfo->GetImagesAvailable();
      bool bWriteFulltext = clipInfoStructured->clipInfo->GetFulltextAvailable();
      bool bWriteUserActions = bWriteImages & bWriteFulltext;

      int iOffset = clipInfoStructured->GetOffset();

      // get last event before clip (required to reset WB page status behind clip structure)
      // will be NULL for pure SG recordings
      CWhiteboardEvent *pLastEvent = m_pWhiteboardStream->GetEvent(iOffset);

      // add event at timestamp zero if required
      // TODO: think about first event
      // TODO: maybe not required if adding Events to existing EVQ - NOT ADDED
      if(bAddEventAtZero && iOffset!= 0) {
         iPageCount++;
         CWhiteboardEvent *pWhiteboardEvent = new CWhiteboardEvent(m_pWhiteboardStream);
         pWhiteboardEvent->SetTimestamp(0);
         pWhiteboardEvent->SetBgColor(7);
         pWhiteboardEvent->SetPageNumber(iPageCount);
         pWhiteboardEvent->SetExportPageNumber(iPageCount);
         m_pWhiteboardStream->AddEvent(pWhiteboardEvent);
         // decrease to ensure same page number as next entry, which is the first real page
         iPageCount--;
      }
      bAddEventAtZero = false;


      // add events for each structured clip entry
      POSITION structurePos = clipInfoStructured->structure.GetHeadPosition();
      while(SUCCEEDED(hr) && structurePos!= NULL) {
         CStructureElement *structureElement = clipInfoStructured->structure.GetNext(structurePos);
         iPageCount++;
         TRACE("     INDEX #%i: %i\n", iPageCount, structureElement->timestamp + iOffset);
         int iTimestamp = structureElement->timestamp + iOffset;

         // Bugfix 5621: inconsistent EVQ
         // try to read existing event
         CWhiteboardEvent *pWhiteboardEvent = m_pWhiteboardStream->GetEvent(iTimestamp);
         if ( pWhiteboardEvent==NULL || pWhiteboardEvent->GetTimestamp() < iTimestamp ) {
            // no matching event found -> create new one
            // TODO: maybe do not add empty event but copy of first real event (cmp. bug 5622)
            pWhiteboardEvent = new CWhiteboardEvent(m_pWhiteboardStream);
            pWhiteboardEvent->SetTimestamp(iTimestamp);
            pWhiteboardEvent->SetBgColor(7);
            pWhiteboardEvent->SetPageNumber(iPageCount);
            pWhiteboardEvent->SetExportPageNumber(iPageCount);
            m_pWhiteboardStream->AddEvent(pWhiteboardEvent);
         }

         // add text objects for full text search
         if(bWriteFulltext && structureElement->textObjects) {
            POSITION pos = structureElement->textObjects->GetHeadPosition();
            while(pos)
               pWhiteboardEvent->AddObject(structureElement->textObjects->GetNext(pos));
         }

         // add images of index keyframes for automated thumbnail computation in Editor/Publisher
         if ( bWriteImages ) {
            // set image file name
            CString csImageFilename;
            csImageFilename.Format(_T("%ls\\%ls_%i.png"), csPath, csFilePrefix, pWhiteboardEvent->GetTimestamp());
            // Bugfix 5621: inconsistent EVQ
            // check if image exists
            bool bImageExists = false;
            CPtrArray *pObjects = pWhiteboardEvent->GetObjectsPointer();
            for ( int i=0; !bImageExists && i<pObjects->GetCount(); i++ ) {
               DrawSdk::DrawObject *pDrawObject = (DrawSdk::DrawObject*)pObjects->GetAt(i);
               if ( pDrawObject->GetType() == DrawSdk::IMAGE ) {
                  DrawSdk::Image *pImageObject = (DrawSdk::Image*)pDrawObject;
                  bImageExists = csImageFilename.Compare(pImageObject->GetImageName()) == 0 
                     && pImageObject->GetX() == 0 && pImageObject->GetY() == 0 
                     && pImageObject->GetWidth() == size.cx && pImageObject->GetHeight() == size.cy;
               }
            }
            if ( !bImageExists ) {
               DrawSdk::Image *image = new DrawSdk::Image(0, 0, size.cx, size.cy, (LPCTSTR)csImageFilename);
               if ( image ) {
                  // adds to object array to ensure memory cleanup (to avoid memory leaks)
                  // add to event
                  pWhiteboardEvent->AddObject(image, TRUE);
               }
            }
         }
      }

      // Bugfix 5621: inconsistent EVQ
      // QUICKFIX do not copy previous event/page (will be required for bugfix 5297)
      // TODO: rework for bugfix 5297 (currently disabled)
      /*
      if(SUCCEEDED(hr) && pLastEvent) 
      {
         // add event to restore WB status
         CWhiteboardEvent *pWhiteboardEvent = pLastEvent->Copy();
         pWhiteboardEvent->SetTimestamp(clipInfoStructured->clipInfo->GetEndTime());
         m_pWhiteboardStream->AddEvent(pWhiteboardEvent);
      }
      */

      // DEMO MODE: add telepointer from user actions
      // BUGFIX 5763: empty thumbnails after using "continue later"
      if ( bWriteUserActions ) {
         // read user events if available
         CString csUserActionsFilename = clipInfoStructured->clipInfo->GetFilename();
         int iLength = csUserActionsFilename.GetLength();
         csUserActionsFilename.SetAt(iLength - 3,'s');
         csUserActionsFilename.SetAt(iLength - 2,'g');
         csUserActionsFilename.SetAt(iLength - 1,'a');
         CArray<UserAction> *paUserActions = ReadUserActions(csUserActionsFilename);
         if(SUCCEEDED(hr)) {
            if(paUserActions) {
               // insert telepointer events
               for( int i = 0; i < paUserActions->GetSize(); i++ ) {
                  UserAction pUserAction = paUserActions->GetAt(i);
                  // BUGFIX 5622 empty page with telepointer
			         // clip offset must be added to timestamp
                  int iTimestamp = pUserAction.iTimeMsec + iOffset;

                  CWhiteboardEvent *pWhiteboardEvent = m_pWhiteboardStream->GetEvent(iTimestamp);
                  if (pWhiteboardEvent->GetTimestamp() != iTimestamp) {
                     // add copy with new timestamp
                     // note: GetEvent delivers EventBefore
                     pWhiteboardEvent = pWhiteboardEvent->Copy();
                     pWhiteboardEvent->SetTimestamp(iTimestamp);
                     m_pWhiteboardStream->AddEvent(pWhiteboardEvent);
                  }
                  // remove previous telepointer
				  if(pUserAction.dwEventType == MH_TELE )
					  pWhiteboardEvent->RemovePointerObjects();
				  else
					  pWhiteboardEvent->RemoveMousePointerObjects();
                  // add telepointer
				  pWhiteboardEvent->AddObject(new DrawSdk::Pointer(pUserAction.iXcoord, pUserAction.iYcoord, pUserAction.dwEventType!=MH_TELE || (pUserAction.dwEventType==MH_TELE && pUserAction.dwAction==MH_REM)), TRUE);
               }
            }
         }
	  }

	  //join the clip evq/obj files (for annotations)
	  //create temporary whiteboard stream for the clip
	  CString csObjFilename = clipInfoStructured->clipInfo->GetFilename();
	  CString csEvqFilename = clipInfoStructured->clipInfo->GetFilename();
	  csObjFilename += ".obj";
	  csEvqFilename += ".evq";
	  CWhiteboardStream *clipWbs = new CWhiteboardStream();
	  clipWbs->SetObjFilename(csObjFilename);
	  clipWbs->SetEvqFilename(csEvqFilename);
	  clipWbs->Open();
	  clipWbs->ResetObjects();
	  CArray<CWhiteboardEvent*,CWhiteboardEvent*> wbEvents;
	  m_pWhiteboardStream->GetEventArray(wbEvents);
	  CWhiteboardEvent *clipEvent;
	  CPtrArray obj;
	  int iTimestamp;
	  for(int i=0;i<wbEvents.GetCount();i++)
	  {
		  if(wbEvents[i]->GetTimestamp() < clipInfoStructured->clipInfo->GetTimestamp())
			  continue;
		  if(wbEvents[i]->GetTimestamp() > clipInfoStructured->clipInfo->GetEndTime())
			  break;
		  iTimestamp = wbEvents[i]->GetTimestamp()-iOffset;
		  clipEvent = clipWbs->GetEvent(iTimestamp);
		  if(clipEvent != NULL && clipEvent->GetTimestamp() <= iTimestamp)
		  {
			  obj.Copy(*wbEvents[i]->GetObjectsPointer());
			  obj.Append(*clipEvent->GetObjectsPointer());
			  wbEvents[i]->AddObjects(obj);
		  }
	  }
   }

   if(SUCCEEDED(hr) && bAddEventAtEnd) {
      // add last event at end of recording
      CWhiteboardEvent *pWhiteboardEvent = new CWhiteboardEvent(m_pWhiteboardStream);
      int iDuration = m_pLrdFile->RetrieveAudioLength() + m_pLrdFile->GetAudioOffset();
      pWhiteboardEvent->SetTimestamp(iDuration);
      pWhiteboardEvent->SetBgColor(7);
      pWhiteboardEvent->SetPageNumber(iPageCount);
      pWhiteboardEvent->SetExportPageNumber(iPageCount);
      m_pWhiteboardStream->AddEvent(pWhiteboardEvent);
   }

   ///////////////////////////////////////
   // update LRD 
   ///////////////////////////////////////

   if(SUCCEEDED(hr)) {
      // adjust whiteboard helper in LRD
      StringManipulation::GetFilename(csEvqFilename);
      StringManipulation::GetFilename(csObjFilename);
      m_pLrdFile->SetWhiteboardHelper(csEvqFilename, csObjFilename, size.cx, size.cy);
   }

   // TODO: error handling
   return hr;
}

bool CscreengrabbingDoc::WriteEvqAndObj(bool bSaveAs) {
   // TODO: may be transfered to FileSDK WhiteboardStream

   // create OBJ / EVQ file handles

   // TODO: error handling
   if(!m_pWhiteboardStream || !m_pLrdFile)
      return false;

   CString csEvqFilename;
   csEvqFilename.Format(_T("%ls\\%ls"), m_pLrdFile->GetRecordPath(), m_pWhiteboardStream->GetEvqFilename());
   CString csObjFilename;
   csObjFilename.Format(_T("%ls\\%ls"), m_pLrdFile->GetRecordPath(), m_pWhiteboardStream->GetObjFilename());

   TRACE("  write EVQ: %ls\n", csEvqFilename);
   TRACE("  write OBJ: %ls\n", csObjFilename);

   LPCTSTR tszRecordPath = m_pLrdFile->GetRecordPath();
   CString filePrefix = m_pLrdFile->GetFilename();
   StringManipulation::GetFilePrefix(filePrefix);

   CFileOutput *objFp;
   HANDLE hObjFile = CreateFile(csObjFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hObjFile == INVALID_HANDLE_VALUE)
   {
      TRACE("ERROR WRITING EMPTY OBJ FILE: %ls\n",csObjFilename);
      return false;
   } 
   else 
   {
      objFp = new CFileOutput;
      objFp->Init(hObjFile, 0, true);
   }
   HANDLE hEvqFile = CreateFile(csEvqFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   CFileOutput *evqFp;
   if (hEvqFile == INVALID_HANDLE_VALUE)
   {
      TRACE("ERROR WRITING EMPTY EVQ FILE: %ls\n",csEvqFilename);
      return false;
   } 
   else 
   {
      evqFp = new CFileOutput;
      evqFp->Init(hEvqFile, 0, false); // EVQ files are not written in UTF-8 format
   }

   // write OBJ/EVQ files

   // write OBJ header
   objFp->Append(_T("<WB orient=bottomleft>\n"));

   // reset object counter and object numbers to ensure sequential numbering
   m_pWhiteboardStream->ResetObjects();
   CWhiteboardStream::m_objectCounter = 0;
   // set page numbers for writing (which are different than normal page numbers)
   SAFE_DELETE(m_pPageStream);
   m_pWhiteboardStream->m_pageCounter = 0;
   m_pPageStream = m_pWhiteboardStream->CreatePageStreamFromEvents();
   m_pWhiteboardStream->UpdateExportPageNumbers(m_pPageStream);

   // write events and objects
   m_pWhiteboardStream->Write(evqFp, objFp, tszRecordPath, filePrefix, 0, m_pWhiteboardStream->GetLength()+1, 0, bSaveAs, m_pPageStream);

   // write OBJ tail
   objFp->Append(_T("</WB>"));
   objFp->Flush();

   // close file handles
   CloseHandle(hObjFile);
   delete objFp;
   objFp = NULL;

   evqFp->Flush();
   CloseHandle(hEvqFile);
   delete evqFp;
   evqFp = NULL;

   TRACE("------------------------DONE---------------------------------\n");
   return true;
}



// data structure for the thread
struct PostProcessingStruct
{
   CscreengrabbingDoc *pScreengrabbingDoc;
   CScreengrabbingExtendedProgressDialog *pExportProgressDialog;
   HRESULT hr;
};



// prepare thread
HRESULT CscreengrabbingDoc::RunPostProcessingThread()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // create progress dialog
   m_hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

   m_pExportProgressDialog = new CScreengrabbingExtendedProgressDialog(AfxGetMainWnd(), m_hDialogInitEvent, m_bShowLaterButton);

   // create processing struct
   PostProcessingStruct packStruct;
   packStruct.pScreengrabbingDoc = this;
   packStruct.pExportProgressDialog = m_pExportProgressDialog;
   packStruct.hr = S_OK;

   AfxBeginThread(PostProcessingThreadLauncher, &packStruct);

   int result = m_pExportProgressDialog->DoModal();
   if(result<0)       
      TRACE(_T("CscreengrabbingDoc::RunPostProcessingThread()"));   

   delete m_pExportProgressDialog;
   m_pExportProgressDialog = NULL;

   CloseHandle(m_hDialogInitEvent);
   m_hDialogInitEvent = NULL;

   return packStruct.hr;
}

// thread starter calls this static method; now let's call the member function
// static
UINT __cdecl CscreengrabbingDoc::PostProcessingThreadLauncher(LPVOID pParam)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   PostProcessingStruct *pPackStruct = (PostProcessingStruct *)pParam;
   CScreengrabbingExtendedProgressDialog *pExportProgressDialog = pPackStruct->pExportProgressDialog;
   CscreengrabbingDoc *pScreengrabbingDoc = pPackStruct->pScreengrabbingDoc;

   // As we need the dialog, wait for it to initialize
   DWORD res = ::WaitForSingleObject(pScreengrabbingDoc->m_hDialogInitEvent, INFINITE);
   // TODO: required??   
   hr = ::CoInitialize(NULL);

   hr = pScreengrabbingDoc->PostProcessing();
   pPackStruct->hr = hr;

   ::PostMessage(pExportProgressDialog->GetSafeHwnd(), WM_USER, CScreengrabbingExtendedProgressDialog::END_DIALOG, NULL);
   // TODO: required??
   ::CoUninitialize();

   return 0;
}


// update progress dialog with label and overall progress
inline void CscreengrabbingDoc::UpdateProgressTotal(HRESULT &hr, const int pos, UINT nResourceID ) {
   if(SUCCEEDED(hr) && m_pExportProgressDialog) {            
      if(m_pExportProgressDialog->IsCanceled()) {
         m_bCanceled = true;
         hr = E_ABORT;
      } else {
         m_pExportProgressDialog->SetOverallProgress(pos);
         m_pExportProgressDialog->SetParticularProgress(0);
         if(nResourceID>0) 
            m_pExportProgressDialog->SetParticularLabel(nResourceID);         
      }
   }
}

// update progress dialog with progress of sub-task; also checks if canceled 
inline void CscreengrabbingDoc::UpdateProgressSubtask(HRESULT &hr, const int pos) {
   if(SUCCEEDED(hr) && m_pExportProgressDialog) {            
      if(m_pExportProgressDialog->IsCanceled()) {
         m_bCanceled = true;
         hr = E_ABORT;
      } else {
         m_pExportProgressDialog->SetParticularProgress(pos);
      }
   }
}

static SgmlElement *FindPageBeginsAt(SgmlElement *sgmlElement, int timestamp) {
    if (sgmlElement) {
        if(_tcscmp(sgmlElement->GetName(),_T("page")) == 0) {
            SgmlElement *beginTag = sgmlElement->Find(_T("begin"));
            if (beginTag && _ttoi(beginTag->GetParameter()) == timestamp)
                return sgmlElement;
        } else if (_tcscmp(sgmlElement->GetName(),_T("chapter")) == 0 || 
                   _tcscmp(sgmlElement->GetName(),_T("structure")) == 0) {
            CArray<SgmlElement *, SgmlElement *> aElements;
            sgmlElement->GetElements(aElements);
            for (int i = 0; i < aElements.GetCount(); ++i) {
                SgmlElement *result = FindPageBeginsAt(aElements[i], timestamp);
                if (result != NULL)
                    return result;
            }
        }
    }

    return NULL;
}


SgmlElement *CscreengrabbingDoc::FindPageBeginsAt(int timestamp) {
   if(m_pLmdFile)
      return ::FindPageBeginsAt(m_pLmdFile->Find(_T("structure"),_T("docinfo")), timestamp);
   return NULL;
}

// reload previously stored structuring information of recording
HRESULT CscreengrabbingDoc::ReloadStructure() {
    // TODO: error handling
    HRESULT hr = S_OK;

    if(m_pClipStream) {
        m_bWriteEvqAndObj = true;
        for (int clipNr=0; clipNr < m_pClipStream->GetSize(); clipNr++) {
            CClipInfo *clipInfo = m_pClipStream->FindClip(clipNr);
            int duration;
            if(VideoInformation::GetVideoDuration(clipInfo->GetFilename(), duration))
                clipInfo->SetLength(duration);

            CClipInfoStructured *clipInfoStructured = new CClipInfoStructured(clipInfo, clipNr+1);

            TRACE("  Clip Nr. %i\n",clipNr+1);
            TRACE("     Title: %ls\n",clipInfo->GetTitle());
            TRACE("     Start: %i\n",clipInfo->GetTimestamp());
            TRACE("     End  : %i\n",clipInfo->GetEndTime());
            if(m_pLrdFile->GetClipStructureAvailable(clipNr+1)) {
                TRACE("     Structure: available\n");
                clipInfo->SetStructureAvailable(true);
            } else
                TRACE("     Structure: not available\n");
            if(m_pLrdFile->GetClipImagesAvailable(clipNr+1)) {
                TRACE("     Images:    available\n");
                clipInfo->SetImagesAvailable(true);
            } else
                TRACE("     Images:    not available\n");
            if(m_pLrdFile->GetClipFulltextAvailable(clipNr+1)) {
                TRACE("     Full Text: available\n");
                clipInfo->SetFulltextAvailable(true);

                // hack to avoid overwriting of previously stored fulltext
                // TODO: currently works only for single SG clips
                m_bWriteEvqAndObj = false;
            } else
                TRACE("     Full Text: not available\n");
            if(m_pLrdFile->GetClipStructureAvailable(clipNr+1)) {
                // TODO: reading structure may not be required if images and fulltext are available
                // TODO: can be removed if existing EVQ/OBJ can be read
                TRACE("     Structure:\n");
                int iBeginClip = clipInfo->GetTimestamp();
                int iEndClip = clipInfo->GetEndTime();
                SgmlElement* pPageTag = FindPageBeginsAt(iBeginClip);

                // BUGFIX 6381: after export the first page no longer starts at clip start time but at 0
                if(pPageTag == NULL && iBeginClip <1000)
                   pPageTag = FindPageBeginsAt(0);

                SgmlElement *pParentTag = (pPageTag!=NULL) ? pPageTag->GetParent() : NULL;
                //SgmlElement *pParentTag = pPageTag->GetParent();
                if (pParentTag != NULL) {
                    CArray<SgmlElement *, SgmlElement *> aElements;
                    pParentTag->GetElements(aElements);
                    bool bIsAfterCurrentPage = false;
                    for (int i = 0; i < aElements.GetCount(); ++i) {
                        SgmlElement *pElement = aElements[i];
                        if (pElement != NULL) {
                            if (pElement == pPageTag)
                                bIsAfterCurrentPage = true;
                            if (bIsAfterCurrentPage) {
                                // TODO: error check - relies on valid structure
                                int iBeginPage = _ttoi(pElement->Find(_T("begin"))->GetParameter());
                                int iEndPage = _ttoi(pElement->Find(_T("end"))->GetParameter());

                                // add index mark
                                clipInfoStructured->structure.AddTail(new CStructureElement(iBeginPage-iBeginClip));
                                TRACE("       Index: %i\n", iBeginPage);

                                // leave loop if end is reached
                                if (iEndPage == iEndClip)
                                    break;
                            }
                        }
                    }
                }
                // add to list of structured clips
                m_listStructuredClips.AddTail(clipInfoStructured);
            } else {
                // if the clipInfoStructured is not added to the list, delete it
                delete clipInfoStructured;
            }

        }
    }
    return hr;
}

int CscreengrabbingDoc::GetNumberOfPages() {
    // TODO: handle subchapters (not required for pure SG)
    int iPageNr = 0;
    if (m_pLmdFile) {
        // skip pages until specified page is found
        SgmlElement *pChapter = m_pLmdFile->Find(_T("chapter"),_T("structure"));
        iPageNr = pChapter->GetElementCount();
    }
    return iPageNr;
}

CString CscreengrabbingDoc::GetChapterTitle()
{
   // TODO: handle subchapters (not required for pure SG)
   if(m_pLmdFile) {
      // skip pages until specified page is found
      SgmlElement *pChapter = m_pLmdFile->Find(_T("chapter"), _T("structure"));

      // get title of chapter
      if(pChapter) {
         return pChapter->GetValue(_T("title"));
      }
   }
   return "";
}

bool CscreengrabbingDoc::SetChapterTitle(CString csTitle)
{
   if(m_pLmdFile) {
      // skip pages until specified page is found
      SgmlElement *pChapter = m_pLmdFile->Find(_T("chapter"), _T("structure"));

      // set title of chapter
      if(pChapter) {
         pChapter->SetAttribute(_T("title"), csTitle);
         return true;
      }
   }
   return false;
}

// iPageNr starting at 1
CString CscreengrabbingDoc::GetPageTitle(int iPageNr) {
    // iPageNr starting at 1
    --iPageNr;
    if (m_pLmdFile && iPageNr >= 0) {
        // skip pages until specified page is found
        SgmlElement *pChapter = m_pLmdFile->Find(_T("chapter"), _T("structure"));
        CArray<SgmlElement *, SgmlElement *> aElements;
        pChapter->GetElements(aElements);
        SgmlElement *pPageTag = NULL;
        if (iPageNr < aElements.GetCount())
            pPageTag = aElements[iPageNr];
        // get title of page
        if (pPageTag != NULL) {
            SgmlElement *pTitle = pPageTag->Find(_T("title"));
            if (pTitle) 
                return (pTitle->GetParameter()); 
        }
    }
    return "";
}

// returns whether the specified page was created by muose clicks or not
bool CscreengrabbingDoc::IsClickPage(int iPageNr) {
    // iPageNr starting at 1
    --iPageNr;
    if (m_pLmdFile && iPageNr >= 0) {
        // skip pages until specified page is found
        SgmlElement *pChapter = m_pLmdFile->Find(_T("chapter"), _T("structure"));
        CArray<SgmlElement *, SgmlElement *> aElements;
        pChapter->GetElements(aElements);
        SgmlElement *pPageTag = NULL;
        if (iPageNr < aElements.GetCount())
            pPageTag = aElements[iPageNr];

        // get type of page
        if (pPageTag && pPageTag->Find(_T("type")) )
            return _tcscmp(pPageTag->Find(_T("type"))->GetParameter(), _T("click")) == 0;
    }
    return false;
}

// returns whether pages created by muose clicks should be shown or not
bool CscreengrabbingDoc::IsShowClickPages() {
   if ( m_pLmdFile ) {
      SgmlElement *pStructure = m_pLmdFile->Find(_T("showClickPages"), _T("fileinfo"));
      if ( pStructure )
         // read if available
         return _tcscmp(pStructure->GetParameter(), _T("true"))==0;
   }
   // default "show all"
   return true;
}

// specifies whether pages created by muose clicks should be shown or not
void CscreengrabbingDoc::ShowClickPages( bool bShow) {
    if (m_pLmdFile != NULL) {
        SgmlElement *pFileInfoTag = m_pLmdFile->Find(_T("fileinfo"));
        if (pFileInfoTag == NULL)
            return;
        SgmlElement *pShowClickStructureTag = pFileInfoTag->Find(_T("showClickPages"));
        if (pShowClickStructureTag == NULL) {
            // add missing <showClickPages> tag
            pShowClickStructureTag = new SgmlElement();
            if (pShowClickStructureTag != NULL) {
                pShowClickStructureTag->SetName(_T("showClickPages"));
                pFileInfoTag->Append(pShowClickStructureTag);
            }
        }
        CString csType = bShow ? _T("true") : _T("false");
        if (pShowClickStructureTag != NULL)
            pShowClickStructureTag->SetParameter(csType);
    }
}


// prefixes of thumbnail file names in testing order
static const CString s_acsThumbPrefixes[] = {"200_", "150_", "100_", ""};

// iPageNr starting at 1
CString CscreengrabbingDoc::GetThumbnailFilename(int iPageNr) {
    if (m_pLmdFile != NULL) {
        CArray<SgmlElement *, SgmlElement *> aPages;
        m_pLmdFile->GetAllPages(aPages);

        SgmlElement *pPage = NULL; 
        if (iPageNr > 0 && iPageNr <= aPages.GetCount())
            pPage = aPages[iPageNr-1];

        // get title of page
        if (pPage != NULL) {
            SgmlElement *pThumb = pPage->Find(_T("thumb"));
            if (pThumb != NULL) {
                // determine filename and test if file exists
                // supporting different preview sizes
                for (int iPrefixCount = 0; iPrefixCount < 4; iPrefixCount++) {
                    CString csFilename;
                    csFilename.Format(_T("%ls\\%ls%ls"), m_pLrdFile->GetRecordPath(), 
                        s_acsThumbPrefixes[iPrefixCount], pThumb->GetParameter());
                    CFileStatus status;
                    if( CFile::GetStatus( csFilename, status ) )
                        return csFilename; 
                }
            }
        }
    }

    return "";
}

// iPageNr starting at 1
void CscreengrabbingDoc::DeleteThumbnailFiles(int iPageNr) {
    if (m_pLmdFile != NULL) {
        CArray<SgmlElement *, SgmlElement *> aPages;
        m_pLmdFile->GetAllPages(aPages);

        SgmlElement *pPage = NULL; 
        if (iPageNr > 0 && iPageNr <= aPages.GetCount())
            pPage = aPages[iPageNr-1];

        // get title of page
        if (pPage != NULL) {
            SgmlElement *pThumb = pPage->Find(_T("thumb"));
            if(pThumb) {
                // determine filename and test if file exists
                // supporting different preview sizes
                for (int iPrefixCount = 0; iPrefixCount < 4; iPrefixCount++) {
                    CString csFilename;
                    csFilename.Format(_T("%ls\\%ls%ls"), m_pLrdFile->GetRecordPath(), 
                        s_acsThumbPrefixes[iPrefixCount], pThumb->GetParameter());
                    CFileStatus status;
                    if( CFile::GetStatus( csFilename, status ) )
                        CFile::Remove( csFilename );
                }
            }
        }
    }
}

// iPageNr starting at 1
int CscreengrabbingDoc::GetPageBeginTime(int iPageNr) {
    if (m_pLmdFile != NULL) {
        CArray<SgmlElement *, SgmlElement *> aPages;
        m_pLmdFile->GetAllPages(aPages);

        SgmlElement *pPage = NULL; 
        if (iPageNr > 0 && iPageNr <= aPages.GetCount())
            pPage = aPages[iPageNr-1];

        // get begin time of page
        if (pPage != NULL) {
            SgmlElement *pTitle = pPage->Find(_T("begin"));
            if(pTitle) 
                return atoiW(pTitle->GetParameter()); 
        }
    }
    return 0;
}

// iPageNr starting at 1
bool CscreengrabbingDoc::SetPageTitle(int iPageNr, CString csTitle) {
    if (m_pLmdFile != NULL && !csTitle.IsEmpty()) {
        CArray<SgmlElement *, SgmlElement *> aPages;
        m_pLmdFile->GetAllPages(aPages);

        SgmlElement *pPage = NULL; 
        if (iPageNr > 0 && iPageNr <= aPages.GetCount())
            pPage = aPages[iPageNr-1];

        // set title of page
        if (pPage != NULL) {
            SgmlElement *pTitle = pPage->Find(_T("title"));
            if(pTitle) {
                pTitle->SetParameter(csTitle);
                return true;
            }
        }
    }
    return false;
}

// iPageNr starting at 1
bool CscreengrabbingDoc::RemovePage(int iPageNr) {
    if (m_pLmdFile != NULL) {
        CArray<SgmlElement *, SgmlElement *> aPages;
        m_pLmdFile->GetAllPages(aPages);

        SgmlElement *pPage = NULL; 
        if (iPageNr > 0 && iPageNr <= aPages.GetCount())
            pPage = aPages[iPageNr-1];

        // remove page from LMD
        if (pPage != NULL) {

            // remove page within sequence
            // Note: does not remove first page
            if (iPageNr > 1) {
                SgmlElement *pPreviousPage = aPages[iPageNr-2];
                if (pPreviousPage != NULL) {

                    SgmlElement *pParent = pPage->GetParent();
                    CString csBegin;
                    pPage->GetElementParameter(_T("begin"), csBegin);
                    CString csEnd;
                    pPage->GetElementParameter(_T("end"), csEnd);
                    CString csNr;
                    pPage->GetElementParameter(_T("nr"), csNr);

                    if (pParent != NULL) {
                        pParent->RemoveElement(pPage);
                    }
                    // adjust end time of previous page             
                    pPreviousPage->SetElementParameter(_T("end"), csEnd);

                    // Bugfix 5668
                    // update pages in event queue according to LMD structure
                    CString csNewBegin;
                    pPreviousPage->GetElementParameter(_T("begin"), csNewBegin);
                    int iBeginTimestamp = atoiW(csNewBegin);
                    CString csNewEnd;
                    pPreviousPage->GetElementParameter(_T("end"), csNewEnd);
                    int iEndTimestamp = atoiW(csNewEnd);
                    CString csNewNr;
                    pPreviousPage->GetElementParameter(_T("nr"), csNewNr);
                    int iPageNr = atoiW(csNewNr);
                    int iOldPageNr = atoiW(csNr);
                    CArray<CWhiteboardEvent *, CWhiteboardEvent *> lstEvents;
                    m_pWhiteboardStream->GetEventArray(lstEvents);
                    for (int i=0; i<lstEvents.GetSize(); ++i) {
                        CWhiteboardEvent *pEvent = lstEvents.GetAt(i);
                        if (!pEvent || pEvent->GetTimestamp() > iEndTimestamp)
                            break;
                        if (pEvent->GetTimestamp() >= iBeginTimestamp) {
                            if (pEvent->GetPageNumber() == iOldPageNr)
                                pEvent->SetPageNumber(iPageNr);
                        }
                    }

                    // remove from EVQ
                    // TODO: should remove static image (screenshot) only
                    // (cmpare similar code in UpdateEvqAndObj())
                    if (m_pWhiteboardStream != NULL) {
                        int iTimestamp = atoiW(csBegin);
                        m_pWhiteboardStream->RemoveEvent(iTimestamp);
                    }
                }

                return true;
            }
        }
    }
    return false;
}


JNIEXPORT jboolean JNICALL Java_imc_lecturnity_converter_ScreengrabbingExtended_runPostProcessing
  (JNIEnv *env, jclass, jstring jsLrdFile)
{
   _TCHAR *tszLrdFile = JNISupport::GetTCharFromString(env, jsLrdFile);

   // only called by Publisher: disable "Continue later" button
   HRESULT hr = CScreengrabbingExtendedExternal::RunPostProcessing(tszLrdFile, false);

   jboolean jbResult;

   if(SUCCEEDED(hr))
      jbResult = JNI_TRUE;
   else
      jbResult = JNI_FALSE;

   return jbResult;
}

JNIEXPORT jboolean JNICALL Java_imc_lecturnity_converter_ScreengrabbingExtended_removeStructure
  (JNIEnv *env, jclass, jstring jsLrdFile)
{
   _TCHAR *tszLrdFile = JNISupport::GetTCharFromString(env, jsLrdFile);

   HRESULT hr = CScreengrabbingExtendedExternal::RemoveStructure(tszLrdFile);

   jboolean jbResult;

   if(SUCCEEDED(hr))
      jbResult = JNI_TRUE;
   else
      jbResult = JNI_FALSE;

   return jbResult;
}

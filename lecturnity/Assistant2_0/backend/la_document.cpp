/*********************************************************************

 program  : la_document.cpp
 authors  : Gabriela Maass
 date     : 18.06.2002
 revision : $Id: la_document.cpp,v 1.35 2010-04-21 15:33:04 kuhn Exp $
 desc     : 

 **********************************************************************/
#include "StdAfx.h"
#include "..\..\Studio\Resource.h"

/*
#ifndef WIN32
#  include <sys/errno.h>
#  include <sys/wait.h>
#  include <unistd.h>
#endif


#include <vector>

#include <math.h>
*/

//#include "tcl.h"
//#include "tk.h"
#include "mlb_types.h"
#include "la_document.h"
#include "la_project.h" 
#include "mlb_folder.h"
#include "mlb_page.h"
#include "mlb_misc.h"
#include "la_metadatamerger.h"

#include "LanguageSupport.h"    // lsutl32

#undef DEBUG

struct LoadDocumentStruct
{
    ASSISTANT::Document *pDocument;
    CProgressDialogA *pProgress;
    HRESULT hr;
};


ASSISTANT::Document::Document(uint32 _id, float _zoomFakt) 
: ASSISTANT::Folder(_id)
{
   m_csDocumentName.LoadString(IDS_DEFAULT_DOCUMENT_NAME);
   m_csDocumentPath.Empty();

   containerName = m_csDocumentName;
   ASSISTANT::GetPrefix(containerName);

   Init();

   SetChanged(false);
   lastSavedDocument.Empty();

   // empty document is always loaded
   m_bIsLoaded = true;
   m_bIsEmptyDocument = true;
}

ASSISTANT::Document::Document(uint32 _id, LPCTSTR _filename, float _zoomFakt) 
  : Folder(_id)
{
   m_csDocumentName = _filename;
   ASSISTANT::GetName(m_csDocumentName);
  
   m_csDocumentPath = _filename;
   ASSISTANT::GetPath(m_csDocumentPath);
   
   containerName = m_csDocumentName;
   ASSISTANT::GetPrefix(containerName);

   Init();

   SetChanged(false);
   lastSavedDocument.Empty();
   
   m_bIsLoaded = false;
   m_bIsEmptyDocument = true;
}

ASSISTANT::Document::~Document()
{
    UnLoad();

    m_csDocumentName.Empty(); 
    m_csDocumentPath.Empty();
    lastSavedDocument.Empty();
}

void ASSISTANT::Document::SetName(LPCTSTR _name) 
{
   CString
      zw;

   containerName.Empty();

   if (_name == NULL) 
   {
      if (!m_csDocumentName.IsEmpty()) 
      {
         containerName = m_csDocumentName;
         int iDotPos = containerName.ReverseFind(_T('.'));
         if (iDotPos != -1) 
         {
            containerName.Left(iDotPos);
         }
      }
      else
		   containerName.LoadString(IDS_DEFAULT_DOCUMENT_NAME);
   }
   else   
      containerName = _name;

   scorm.SetTitle(containerName);
}

void ASSISTANT::Document::SetDocumentFilename(LPCTSTR filename)
{
   CString documentPrefix = m_csDocumentName;
   GetPrefix(documentPrefix);

   m_csDocumentName = filename;
   ASSISTANT::GetName(m_csDocumentName);
      
   m_csDocumentPath = filename;
   ASSISTANT::GetPath(m_csDocumentPath);

   if (containerName == documentPrefix)
   {
      containerName = m_csDocumentName;
      ASSISTANT::GetPrefix(containerName);
   }
}

void  ASSISTANT::Document::SaveContainer(FILE *fp, LPCTSTR path, int level, bool all)
{
   for (int i = 0; i < subContainer.GetSize(); ++i)
      subContainer[i]->SaveContainer(fp, path, (level+1), all);

}

HRESULT ASSISTANT::Document::Load(CProgressDialogA *pProgress) {
    HRESULT hr = S_OK;

    CString csProgressText;
    csProgressText.Format(IDS_LOADING_DOCUMENT, m_csDocumentName);
    if (pProgress != NULL) {
        pProgress->SetRange(0, 100);
        pProgress->SetLabel(csProgressText);
    }

    CString csFilename = m_csDocumentPath;
    csFilename += _T("\\");
    csFilename += m_csDocumentName;

    int iPageCount = 0;
    ASSISTANT::SGMLTree *pSgmlTree = new ASSISTANT::SGMLTree(csFilename);
    if (pSgmlTree == NULL || pSgmlTree->Buildtree(iPageCount, pProgress) < 0)
        return E_FAIL;

    containerName.Empty();

    ASSISTANT::SGMLElement *pRoot = pSgmlTree->root;
    if (pRoot == NULL || ((pRoot->Name != _T("ASSISTANT")) && (pRoot->Name != _T("MLB")))) {
        delete pSgmlTree;
        return E_FAIL;
    }

    SetName(pRoot->GetAttribute("name"));

    LPTSTR tszAttribute;
    tszAttribute = pRoot->GetAttribute("author");
    if (tszAttribute != NULL) 
        scorm.SetAuthor(tszAttribute);
    else     
        scorm.SetAuthor(_T(""));   

    tszAttribute = pRoot->GetAttribute("creator");
    if (tszAttribute != NULL) 
        scorm.SetCreator(tszAttribute);
    else     
        scorm.SetCreator(_T(""));   

    tszAttribute = pRoot->GetAttribute("keywords");
    if (tszAttribute) 
        scorm.SetKeywords(tszAttribute);
    else     
        scorm.SetKeywords(_T(""));   

    tszAttribute = pRoot->GetAttribute("codepage");
    if (tszAttribute) {
        unsigned int iCodePage = _ttoi(tszAttribute);
        SetCodepage(iCodePage);
        unsigned int iActualCodepage = GetACP();
        if (iActualCodepage != iCodePage) {
            CString csMessage;
            csMessage.Format(IDS_WARNING_CODEPAGE, iCodePage, iActualCodepage);
            CString csCaption;
            csCaption.LoadString(IDS_WARNING);
            MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
        }
    }

    float fProgressTab = (float)50 / iPageCount;

    int iCount = 0;
    int iContainerNumber = 0;
    // If the document is selected the loaded objects are inserted at the begin
    // -> wrong order, this flag is only necessary for draeg&drop
    m_bSelected = false;
    ASSISTANT::SGMLElement *pTag = pRoot->son;
    while (pTag != NULL) {
        // 50% for Buildtree, 50% for page creation
        int iActualProgressValue = (int)(50 + iCount * fProgressTab);

        if (pProgress)
            pProgress->SetPos(iActualProgressValue);

        if (pTag->Name == _T("FOLDER")) {
            ++iContainerNumber;
            _TCHAR tszTmpFolderNumber[256];
            _stprintf(tszTmpFolderNumber, _T("%d") ,iContainerNumber);
            ASSISTANT::Folder *pFolder = new ASSISTANT::Folder(Project::active->GetObjectID(), tszTmpFolderNumber, this, pTag);
            InsertContainerCurrent(pFolder);
            pFolder->SetCodepage(GetCodepage());
        } else if (pTag->Name == _T("PAGE")) { 
            ++iContainerNumber;
            _TCHAR tszTmpPageNumber[256];
            _stprintf(tszTmpPageNumber, _T("%d"), iContainerNumber);
            ASSISTANT::Page *pPage = new ASSISTANT::Page(Project::active->GetObjectID(), tszTmpPageNumber, this, pTag);
            InsertContainerCurrent(pPage);
            pPage->SetCodepage(GetCodepage());
            ++iCount;
        } else {
            CString csError;
            csError.Format(IDS_SGML_TYPE_ERROR, pTag->Name);
            MessageBox(NULL, csError, NULL, MB_OK | MB_TOPMOST);
        }

        pTag = pTag->next;
    }  

    current = subContainer.GetSize() > 0 ? 0 : -1;

    delete pSgmlTree;

      
   CString metadataName = csFilename;
   GetPrefix(metadataName);
   metadataName += _T(".lmd");
   
   MetadataMerger metadataMerger;
   metadataMerger.DoMerge(this, metadataName);

    //if ((50 - lastValue) > 0)
    //   GUI::Progress::IncreaseProgress(50-lastValue);

    m_bIsLoaded = true;

    return hr;
}

HRESULT ASSISTANT::Document::LoadInformation()
{
    HRESULT hr = S_OK;

    CString csFilename = m_csDocumentPath;
    csFilename += _T("\\");
    csFilename += m_csDocumentName;

    ASSISTANT::SGMLTree *pSgmlTree = new SGMLTree(csFilename);
    int iPages;
    if (pSgmlTree->Buildtree(iPages, NULL, true) < 0)
        hr = E_FAIL;

    if (iPages == 0)
        m_bIsEmptyDocument = true;
    else
        m_bIsEmptyDocument = false;

    containerName.Empty();
    ASSISTANT::SGMLElement *pTreeRoot = NULL;
    if (SUCCEEDED(hr)) {
        pTreeRoot = pSgmlTree->root;
        if (!pTreeRoot || 
            ((pTreeRoot->Name != _T("ASSISTANT")) && (pTreeRoot->Name != _T("MLB"))))
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr))
        SetName(pTreeRoot->GetAttribute("name"));

    delete pSgmlTree;

    return hr;
}

HRESULT ASSISTANT::Document::CreateLoadThread()
{
   HANDLE hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   
   CProgressDialogA *pProgress = new CProgressDialogA(AfxGetMainWnd(), hDialogInitEvent);
   
   LoadDocumentStruct loadStruct;
   loadStruct.pDocument = this;
   loadStruct.pProgress = pProgress;
   loadStruct.hr = S_OK;

   CWinThread *thread = AfxBeginThread(LoadThread, (LPVOID)&loadStruct);

   if (pProgress)
      pProgress->DoModal();
   
   if (pProgress)
      delete pProgress;
   pProgress = NULL;

   CloseHandle(hDialogInitEvent);
   hDialogInitEvent = NULL;
   
   return loadStruct.hr;
}

UINT ASSISTANT::Document::LoadThread(LPVOID pParam)
{
   HRESULT hr = S_OK;

   hr = CLanguageSupport::SetThreadLanguage();

   // necessary to prevent progress window with hwnd==NULL
   Sleep(100);

   LoadDocumentStruct *pLoadStruct = (LoadDocumentStruct *)pParam;

   ASSISTANT::Document *pDocument = pLoadStruct->pDocument;
   CProgressDialogA *pProgress = pLoadStruct->pProgress;

   pLoadStruct->hr = pDocument->Load(pProgress);

   if (pProgress != NULL)
      PostMessage(pProgress->GetSafeHwnd(), WM_USER, CProgressDialogA::END_DIALOG, NULL);

   return 0;
}

void ASSISTANT::Document::UnLoad()
{
    if (subContainer.GetSize() > 0) {
        for (int i = 0; i < subContainer.GetSize(); ++i) 
            delete(subContainer[i]);
        subContainer.RemoveAll();
    }

    m_bIsLoaded = false;
}


/* Functions to write PostScript */


/* Functions to open or save document */
/*** own functions called by main.cpp ***/
bool ASSISTANT::Document::Save()
{
   CString
      csFilename;
   
   csFilename = m_csDocumentPath;
   csFilename += _T("\\");
   csFilename += m_csDocumentName;
   
   bool success = WriteLSD(csFilename);
   
   if (success)
   {
      SetChanged(false);
      lastSavedDocument = csFilename;
   }

   return success;
}

bool ASSISTANT::Document::SaveAs(LPCTSTR filename)
{
   // store temporarily old document name
   CString documentPrefix = m_csDocumentName;
   GetPrefix(documentPrefix);
   
   // update document name
   m_csDocumentName = filename;
   ASSISTANT::GetName(m_csDocumentName);
   
   // update document path
   m_csDocumentPath = filename;
   ASSISTANT::GetPath(m_csDocumentPath);
   
   if (containerName == documentPrefix)
   {
      bool changeTitle = false;
      if (containerName == scorm.GetTitle())
         changeTitle = true;
      containerName = m_csDocumentName;
      ASSISTANT::GetPrefix(containerName);
      if (changeTitle)
         scorm.SetTitle(containerName);
   }
   
   bool success = WriteLSD(filename);
   if (success)
   {
      SetChanged(false);
      lastSavedDocument = filename;
   }
   
   return success;
}

void ASSISTANT::Document::GetLastSavedDocument(CString &csLastSavedDocument)
{
   csLastSavedDocument = lastSavedDocument;
}

bool  ASSISTANT::Document::WriteLSD(LPCTSTR filename, bool bChangeFilename)
{
   WCHAR *wszFilename = ASSISTANT::ConvertTCharToWChar(filename);

   FILE *datei = _wfopen(wszFilename, L"wb");

   if (wszFilename)
      delete wszFilename;

   if (datei) 
   {
      // Write Unicode BOMs
      BYTE bom1 = 0xFF;
      BYTE bom2 = 0xFE;
      fwrite(&bom1, 1, 1, datei);
      fwrite(&bom2, 1, 1, datei);

      CString csSgmlString;

      CString csName = containerName;
      StringManipulation::TransformForSgml(csName, csSgmlString);
      WCHAR *wszName = ASSISTANT::ConvertTCharToWChar(csSgmlString);

      CString csKeywords = scorm.GetKeywords();
      StringManipulation::TransformForSgml(csKeywords, csSgmlString);
      WCHAR *wszKeywords = ASSISTANT::ConvertTCharToWChar(csSgmlString);

      CString csAuthor = scorm.GetAuthor();
      StringManipulation::TransformForSgml(csAuthor, csSgmlString);
      WCHAR *wszAuthor = ASSISTANT::ConvertTCharToWChar(csSgmlString);

      CString csCreator = scorm.GetCreator();
      StringManipulation::TransformForSgml(csCreator, csSgmlString);
      WCHAR *wszCreator = ASSISTANT::ConvertTCharToWChar(csSgmlString);
      
      fwprintf(datei, L"<ASSISTANT codepage=\"%d\" name=\"%s\" author=\"%s\" creator=\"%s\" keywords=\"%s\">\n",
                    GetCodepage(), wszName, wszAuthor, wszCreator, wszKeywords);
      
      if (wszName)
         delete wszName;

      if (wszAuthor)
         delete wszAuthor;

      if (wszCreator)
         delete wszCreator;

      if (wszKeywords)
         delete wszKeywords;

      CString ssDocumentPath = filename;
      ASSISTANT::GetPath(ssDocumentPath);

      SaveContainer(datei, ssDocumentPath, 1, true);

      fwprintf(datei, L"</ASSISTANT>\n");
      fclose(datei);
      
      if (bChangeFilename)
         m_csDocumentPath = ssDocumentPath;

      return true;
   }

   return false;
}

#ifndef _STUDIO
bool ASSISTANT::Document::Open(const _TCHAR *_filename, bool bDoLoad)
{
   m_csDocumentName = _filename;
   ASSISTANT::GetName(m_csDocumentName);
  
   m_csDocumentPath = _filename;
   ASSISTANT::GetPath(m_csDocumentPath);

   if (bDoLoad) {
       HRESULT hr = Load();
       if (FAILED(hr))
            return false;
   }

  return true;
}
#endif

bool ASSISTANT::Document::Close(bool bAskForSave)
{
    if (HasChanged() && bAskForSave) {
        CString csMessage;
        csMessage.Format(IDS_SAVE_DOCUMENT, m_csDocumentName);
        CString csCaption;
        csCaption.LoadString(IDS_CONFIRM_CLOSE);

        int iAnswer = MessageBox(NULL, csMessage, csCaption, MB_YESNOCANCEL | MB_TOPMOST);
        if (iAnswer == IDCANCEL) 
            return false;

        if (iAnswer == IDYES)
        {
            CString docFilename;
            docFilename = m_csDocumentPath;
            docFilename += _T("\\");
            docFilename += m_csDocumentName;

            if (!Save())
            {
                CString csError;
                csError.Format(IDS_ERROR_SAVE_DOCUMENT, m_csDocumentName);
                MessageBox(NULL, csError, NULL, MB_OK | MB_TOPMOST);
                return false;
            }
        }
    }

    UnLoad();

    return true;
}

bool ASSISTANT::Document::HasChanged()
{
   if (m_bContainerChanged)
      return true;

   if (subContainer.GetSize() > 0)  
   {
      for (int i = 0;i < subContainer.GetSize(); ++i)
      {
         if (subContainer[i]->HasChanged())
            return true;
      }
   }

   return false;
}

void ASSISTANT::Document::SetAllUnchanged()
{
    SetChanged(false);

    if (subContainer.GetSize() > 0) {
        for (int i = 0; i < subContainer.GetSize(); ++i) {
            if (subContainer[i] != NULL)
                subContainer[i]->SetChanged(false);
        }
    }

   return;
}

/*** functions concerned actual page ***/


void ASSISTANT::Document::RaiseObjects()
{
  if (GetActivePage()) 
    GetActivePage()->RaiseObjects();
}


void ASSISTANT::Document::LowerObjects()
{
  if (GetActivePage()) 
    GetActivePage()->LowerObjects();

  return;
}


void ASSISTANT::Document::Undo()
{
  if (GetActivePage()) 
    GetActivePage()->Undo();

  return;
}

void ASSISTANT::Document::CutObjects()
{
  if (GetActivePage()) 
    GetActivePage()->CutObjects();

  return;
}

void ASSISTANT::Document::CopyObjects()
{
  if (GetActivePage()) 
    GetActivePage()->CopyObjects();

  return;
}


void ASSISTANT::Document::PasteObjects()
{
  if (GetActivePage()) {
    GetActivePage()->PasteObjects();
  }

  return;
}

void ASSISTANT::Document::AddTo(Document *dest)
{
   if (subContainer.GetSize() > 0) 
   {
      for (int i = 0; i < subContainer.GetSize(); ++i)
      {
         dest->InsertContainerCurrent(subContainer[i]);
      }
   }

   subContainer.RemoveAll();
}




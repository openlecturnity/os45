// StudioDocTemplate.cpp: implementation of the CMultiDocTemplateEx class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "StudioDocTemplate.h"
#include "MainFrm.h"
#include "..\Assistant2_0\MainFrm.h"
#include "..\Editor\MainFrm.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStudioDocTemplate::CStudioDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
   m_bRetFromEditMode = false;
}

CStudioDocTemplate::~CStudioDocTemplate()
{

}

CDocument* CStudioDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible)
{
    if (m_docList.IsEmpty())
        CMultiDocTemplate::OpenDocumentFile(NULL, bMakeVisible);

    if (m_docList.GetCount() == 1)
    {	
        CDocument* pDocument = NULL;
        BOOL bWasModified = FALSE;

        // already have a document - reinit it
        POSITION docPos = m_docList.GetHeadPosition();
        pDocument = (CDocument*)m_docList.GetNext(docPos);

        if (pDocument == NULL)
        {
            AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
            return NULL;
        }

        CFrameWnd *pFrameWnd = NULL;
        if (pDocument) {
            POSITION viewPos = pDocument->GetFirstViewPosition();
            while (viewPos) {
                CView *pView = pDocument->GetNextView(viewPos);
                if (pView != NULL) {
                    CFrameWnd *pChildFrame = pView->GetParentFrame();
                    if (pChildFrame != NULL && 
                        (pChildFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrameA) ||
                         pChildFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrameE))) {
                        pFrameWnd = pChildFrame;
                        break;
                    }
                }
            }
        }

        CWnd *pWnd = AfxGetMainWnd();
        if (pFrameWnd && 
            (pWnd && pWnd->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))) {
               CMainFrameA* pChild = NULL;
            if (pFrameWnd->GetRuntimeClass() == RUNTIME_CLASS(CMainFrameA)) {
                ((CMDIFrameWnd *)pWnd)->MDIActivate(pFrameWnd);
                pChild = (CMainFrameA*)((CMDIFrameWnd *)pWnd)->MDIGetActive();
            }
            if(m_bRetFromEditMode)
            {
               if(pChild && pChild->GetStartupView())
                  pChild->GetStartupView()->SetReturnFromEditMode(true);
               m_bRetFromEditMode = false;
            }
            pFrameWnd->ShowWindow(SW_SHOWMAXIMIZED);
        }

        if (lpszPathName == NULL)
        {
            // create a new document
            SetDefaultTitle(pDocument);

            // avoid creating temporary compound file when starting up invisible
            if (!bMakeVisible)
                pDocument->m_bEmbedded = TRUE;

            if (!pDocument->OnNewDocument())
            {
                // user has been alerted to what failed in OnNewDocument
                TRACE(traceAppMsg, 0, "CDocument::OnNewDocument returned FALSE.\n");
                return NULL;
            }
        }
        else
        {
            CWaitCursor wait;

            // open an existing document
            bWasModified = pDocument->IsModified();
            pDocument->SetModifiedFlag(FALSE);  // not dirty for open

            if (!pDocument->OnOpenDocument(lpszPathName))
            {
                // user has been alerted to what failed in OnOpenDocument
                TRACE(traceAppMsg, 0, "CDocument::OnOpenDocument returned FALSE.\n");
                if (!pDocument->IsModified())
                {
                    // original document is untouched
                    pDocument->SetModifiedFlag(bWasModified);
                }
                else
                {
                    // we corrupted the original document
                    SetDefaultTitle(pDocument);

                    if (!pDocument->OnNewDocument())
                    {
                        TRACE(traceAppMsg, 0, "Error: OnNewDocument failed after trying "
                            "to open a document - trying to continue.\n");
                        // assume we can continue
                    }
                }
                return NULL;        // open failed
            }
            pDocument->SetPathName(lpszPathName);
        }

        return pDocument;
    }

    return NULL;
}

bool CStudioDocTemplate::CloseDocumentFile()
{
    if (m_docList.GetSize() == 0)
        return true;

    CDocument* pDocument = NULL;
    BOOL bWasModified = FALSE;

    // already have a document - reinit it
    POSITION docPos = m_docList.GetHeadPosition();
    pDocument = (CDocument*)m_docList.GetNext(docPos);

    if (!pDocument->SaveModified())
        return false;        // leave the original one

    if (!pDocument->OnNewDocument())
    {
        // user has been alerted to what failed in OnNewDocument
        TRACE(traceAppMsg, 0, "CDocument::OnNewDocument returned FALSE.\n");
        return false;
    }

    return true;
}

bool CStudioDocTemplate::Save()
{
    if (m_docList.GetSize() == 0)
        return true;

    CDocument* pDocument = NULL;
    BOOL bWasModified = FALSE;

    // already have a document - reinit it
    POSITION docPos = m_docList.GetHeadPosition();
    pDocument = (CDocument*)m_docList.GetNext(docPos);

    if (!pDocument->SaveModified())
        return false;        // leave the original one

    return true;
}
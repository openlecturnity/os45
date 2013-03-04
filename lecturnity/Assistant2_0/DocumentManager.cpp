#include "stdafx.h"

#include "DocumentManager.h"
#include "..\Studio\Studio.h"
#include "AssistantDoc.h"
#include "backend\la_project.h"

IMPLEMENT_DYNCREATE(CDocumentManager, CObject)

CDocumentManager::CDocumentManager() {
    // dummy for extending CObject (??)
}

CDocumentManager::CDocumentManager(CWinApp *pWinApp) {
    m_pWinApp = pWinApp;

}

CDocumentManager::~CDocumentManager(void) {
}

bool CDocumentManager::LoadSourceDocumentsIn(ASSISTANT::Project *pProject) {

    if (pProject == NULL) // one of them can be NULL
        return false;

    m_aFilenames.RemoveAll();

    ReadFileNames(_T("Lsd"));

    bool bSuccess = true;

    for (int i = 0; i < m_aFilenames.GetSize(); ++i) {
        // check if file exists, add only existing files
        if (_taccess(m_aFilenames[i], 00) == 0)
            pProject->AddDocument(m_aFilenames[i]);
    }

    return bSuccess;
}

bool CDocumentManager::StoreSourceDocumentsFrom(ASSISTANT::Project *pProject) {
    if (m_pWinApp == NULL)
        return false;

    if (pProject == NULL)
        return false;

    CArray<CString, CString> aPaths;
    pProject->GetSourceDocumentPaths(aPaths);

    m_aFilenames.RemoveAll();

    CString csFileLocation;
    GetFileNodeName(csFileLocation);

    CString csSourceId;
    for (int i=0; i<aPaths.GetSize(); ++i) {
        if (!aPaths[i].IsEmpty()) {
            csSourceId.Format(_T("Lsd%d"), i);
            m_pWinApp->WriteProfileString(csFileLocation, csSourceId, aPaths[i]);
            m_aFilenames.Add(aPaths[i]);
        }
    }
    // Write stop entry; entries are not deleted, only overwritten
    csSourceId.Format(_T("Lsd%d"), m_aFilenames.GetSize());
    m_pWinApp->WriteProfileString(csFileLocation, csSourceId, _T(""));

    return true;
}

bool CDocumentManager::LoadRecordingDocumentsIn(ASSISTANT::Project *pProject) {

    m_aFilenames.RemoveAll();

    ReadFileNames(_T("Lrd"));

    bool bSuccess = true;

    // if pProject == NULL the document should only be added to m_aFilenames
    // this is called from editor to add opened documents to list
    if (pProject != NULL) {
        for (int i = 0; i < m_aFilenames.GetSize(); ++i) {
            // check if file exists, add only existing files
            if (_taccess(m_aFilenames[i], 00) == 0)
                pProject->AddRecording(m_aFilenames[i]);
        }
    }

    return bSuccess;
}

bool CDocumentManager::StoreRecordingDocumentsFrom(ASSISTANT::Project *pProject) {
    if (m_pWinApp == NULL)
        return false;

    CArray<CString, CString> aPaths;

    // if pProject == NULL write all entries in m_aFilenames
    // this is called from editor to add opened documents to list
    if (pProject != NULL) {
        pProject->GetRecordingDocumentPaths(aPaths);
    }
    else {
        aPaths.Append(m_aFilenames);
    }

    m_aFilenames.RemoveAll();

    CString csFileLocation;
    GetFileNodeName(csFileLocation);

    CString csSourceId;
    for (int i=0; i<aPaths.GetSize(); ++i) {
        if (!aPaths[i].IsEmpty()) {
            bool bDoSave = true;
            // test for duplicates
            for (int j = 0; j < i && bDoSave; ++j) {
                if (aPaths[i] == aPaths[j])
                    bDoSave = false;
            }
            if (bDoSave) {
                csSourceId.Format(_T("Lrd%d"), i);
                m_pWinApp->WriteProfileString(csFileLocation, csSourceId, aPaths[i]);
                m_aFilenames.Add(aPaths[i]);
            }
        }
    }
    // Write stop entry; entries are not deleted, only overwritten
    csSourceId.Format(_T("Lrd%d"), m_aFilenames.GetSize());
    m_pWinApp->WriteProfileString(csFileLocation, csSourceId, _T(""));

    return true;
}

bool CDocumentManager::AddRecordingToRegistry(LPCTSTR lpszRecordingFilename) {
    // This is called by editor to add a recording to the registry list
    bool bSuccess = false;

    bSuccess = LoadRecordingDocumentsIn(NULL);

    if (bSuccess) {
        m_aFilenames.Add(lpszRecordingFilename);
        bSuccess = StoreRecordingDocumentsFrom(NULL);
    }

    return bSuccess;
}

// private
void CDocumentManager::ReadFileNames(TCHAR *tszFileSuffix) {

    if (m_pWinApp != NULL) {
        CString csSourceId;
        int iSourceDocumentCount = 0;

        do {
            csSourceId.Format(_T("%s%d"), tszFileSuffix, iSourceDocumentCount);
            CString csFileLocation;
            GetFileNodeName(csFileLocation);
            CString csDocPath = m_pWinApp->GetProfileString(csFileLocation, csSourceId);

            if (!csDocPath.IsEmpty()) {
                m_aFilenames.Add(csDocPath);

                ++iSourceDocumentCount;
            } else
                break;
        } while (true);
    }
}

void CDocumentManager::GetFileNodeName(CString &csFileNodeName) {
    csFileNodeName = _T("Files");

    if (CStudioApp::IsPowerTrainerMode())
        csFileNodeName += _T("\\") + CStudioApp::GetPowerTrainerProjectName();
}
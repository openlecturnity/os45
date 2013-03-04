#pragma once

class CAssistantDoc; // avoid circular includes
namespace ASSISTANT {
    class Project;
}

class CDocumentManager : public CObject
{
protected:
    CDocumentManager(); // only for extending CObject

    DECLARE_DYNCREATE(CDocumentManager)

public:
    CDocumentManager(CWinApp *pWinApp);
    ~CDocumentManager(void);

    bool LoadSourceDocumentsIn(ASSISTANT::Project *pProject);
    bool StoreSourceDocumentsFrom(ASSISTANT::Project *pProject);
    
    bool LoadRecordingDocumentsIn(ASSISTANT::Project *pProject);
    bool StoreRecordingDocumentsFrom(ASSISTANT::Project *pProject);

    bool AddRecordingToRegistry(LPCTSTR lpszRecordingFilename);

private:
    void ReadFileNames(TCHAR *tszFileSuffix);
    void GetFileNodeName(CString &csFileNodeName);

private:
    CWinApp *m_pWinApp;
    CArray<CString, CString> m_aFilenames;
};

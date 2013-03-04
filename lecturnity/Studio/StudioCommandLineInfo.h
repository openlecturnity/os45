#pragma once
class CLiveContext;

class CStudioCommandLineInfo : public CCommandLineInfo {
public:
    /* Pointer to a CLiveContext instance that will hold command line information */
	CStudioCommandLineInfo(CLiveContext *pLiveContext);
	virtual ~CStudioCommandLineInfo();

    bool IsDoFinalizeRecording() { return m_bDoFinalize; }
    CString &GetLrdToFinalizeName() { return m_csLrdToFinalize; }

    bool IsPowerTrainerMode() { return m_bIsPowerTrainerMode; }
    CString &GetPowerTrainerProjectName() { return m_csPowerTrainerProjectName; }

    bool IsShowFormatDialog() { return m_bIsShowFormatDialog; }
    CString &GetFormatLrdFileName() { return m_csFormatLrdName; }

private:
    virtual void ParseParam(const TCHAR* pszParam,  BOOL bFlag, BOOL bLast);

private:
    bool m_bDoFinalize;
    CString m_csLrdToFinalize;

    bool m_bIsPowerTrainerMode;
    CString m_csPowerTrainerProjectName;

    bool m_bIsShowFormatDialog;
    CString m_csFormatLrdName;

    /* Currently it hold just the reference to the CStudioApp::m_cLiveContext instance*/
    CLiveContext *m_pLiveContext;
};



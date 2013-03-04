// StudioCommandLineInfo.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Studio.h"
#include "StudioCommandLineInfo.h"


// CStudioCommandLineInfo

CStudioCommandLineInfo::CStudioCommandLineInfo(CLiveContext *pLiveContext) {
    m_bDoFinalize = false;
    m_bIsPowerTrainerMode = false;
    m_bIsShowFormatDialog = false;
    m_pLiveContext = pLiveContext;
    ASSERT(m_pLiveContext); // must be valid pointer
}

CStudioCommandLineInfo::~CStudioCommandLineInfo() {
}


// CStudioCommandLineInfo-Memberfunktionen

void CStudioCommandLineInfo::ParseParam(const TCHAR* pszParam,  BOOL bFlag, BOOL bLast) {
    static int iLastParam = -1;

    enum paramIDs {
        UNKNOWN = -1,
        FINALIZE = 0,
        PT_MODE = 1,
        SHOW_FORMAT = 2,
        LC_MODE = 3,
        LC_TARGET = 4,
        LC_PROFILE = 5, 
        LC_PIPE_NAME = 6,
        LC_TITLE = 7
    };


    if (bFlag && _tcscmp(pszParam, _T("finalize")) == 0) {
        m_bDoFinalize = true;
        iLastParam = FINALIZE;

        ParseLast(bLast);
    } else if (bFlag && _tcscmp(pszParam, _T("dpt")) == 0) {
        m_bIsPowerTrainerMode = true;
        iLastParam = PT_MODE;

        ParseLast(bLast);
    } else if (bFlag && _tcscmp(pszParam, _T("format")) == 0) {
        m_bIsShowFormatDialog = true;
        iLastParam = SHOW_FORMAT;

        ParseLast(bLast);
    } else if (bFlag && _tcscmp(pszParam, _T("lc")) == 0) {
        m_pLiveContext->SetLiveContextMode(true);
        iLastParam = LC_MODE;

        ParseLast(bLast);
    } else if (bFlag && _tcscmp(pszParam, _T("target")) == 0) {
        iLastParam = LC_TARGET;

        ParseLast(bLast);
    } else if (bFlag && _tcscmp(pszParam, _T("title")) == 0) {
        iLastParam = LC_TITLE;

        ParseLast(bLast);
    } else if (bFlag && _tcscmp(pszParam, _T("profile")) == 0) {
        iLastParam = LC_PROFILE;

        ParseLast(bLast);
    }else if (bFlag && _tcscmp(pszParam, _T("pipename")) == 0) {
        iLastParam = LC_PIPE_NAME;

        ParseLast(bLast);
    } else if (!bFlag && iLastParam == FINALIZE) {
        m_csLrdToFinalize = pszParam;

        ParseLast(bLast);
    } else if (!bFlag && iLastParam == PT_MODE) {
        m_csPowerTrainerProjectName = pszParam;

        ParseLast(bLast);
    } else if (!bFlag && iLastParam == SHOW_FORMAT) {
        m_csFormatLrdName = pszParam;

        ParseLast(bLast);
    } else if (!bFlag && iLastParam == LC_MODE) {
        // Is used for two (following) parameters

        CString csParam = pszParam;

        if (m_pLiveContext->GetModeType()== LcUnknown) {
            csParam.MakeUpper();

            if (csParam == _T("RECORD"))
                m_pLiveContext->SetModeType(LcRecord);
            else if (csParam == _T("PREVIEW"))
                m_pLiveContext->SetModeType(LcPreview);
            else if (csParam == _T("EDIT"))
                m_pLiveContext->SetModeType(LcEdit);
            else if (csParam == _T("PUBLISH"))
                m_pLiveContext->SetModeType(LcPublish);
            else if (csParam == _T("DEMODOC"))
                m_pLiveContext->SetModeType(LcCreateDemoDoc);
            else if (csParam == _T("MANUAL"))
                m_pLiveContext->SetModeType(LcCreateManual);
        } else {
            m_pLiveContext->SetInput(pszParam);
        }

        ParseLast(bLast);
    } else if (!bFlag && iLastParam == LC_TARGET) {
        m_pLiveContext->SetOutput(pszParam);

        ParseLast(bLast);
    } else if (!bFlag && iLastParam == LC_TITLE) {
        m_pLiveContext->SetTitle(pszParam);

        ParseLast(bLast);
    } else if (!bFlag && iLastParam == LC_PROFILE) {
        m_pLiveContext->SetProfile(pszParam);

        ParseLast(bLast);
    } else if (!bFlag && iLastParam == LC_PIPE_NAME) {
        m_pLiveContext->SetPipeName(pszParam);

        ParseLast(bLast);
    } 
    else {
        iLastParam = -1;
        CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
    }  
}

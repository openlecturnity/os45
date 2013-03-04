#pragma once

#include "stdafx.h"


enum LcModeType {
    LcRecord = 0,
    LcPreview = 1,
    LcEdit = 2,
    LcPublish = 3,
    LcCreateManual = 4,
    LcCreateDemoDoc = 5,
    LcUnknown = -1
};

class LSUTL32_EXPORT CLiveContext {
public:
    CLiveContext();
    virtual ~CLiveContext();

    void SetLiveContextMode(bool bLiveContextMode) {m_bIsLiveContextMode = bLiveContextMode;}
    bool IsLiveContextMode() { return m_bIsLiveContextMode; }

    LcModeType GetModeType() { return m_iModeType; }
    void SetModeType(LcModeType iModeType) {m_iModeType = iModeType;}

    CString &GetInput() { return m_csLrd; }
    void SetInput(CString csLrd) {m_csLrd = csLrd;}

    CString &GetPipeName() { return m_csPipeName; }
    void SetPipeName(CString csPipeName) {m_csPipeName = csPipeName;}

    CString &GetOutput() { return m_csHtml; }
    void SetOutput(CString csHtml) {m_csHtml = csHtml;}

    CString &GetTitle() { return m_csTitle; }
    void SetTitle(CString csTitle) {m_csTitle = csTitle;}

    CString &GetProfile() { return m_csProfile; }
    void SetProfile(CString csProfile) {m_csProfile = csProfile;}

    bool &IsPreview() { return m_bIsLiveContextPreview; }
    void SetPreview(bool bIsLiveContextPreview) {m_bIsLiveContextPreview = bIsLiveContextPreview;}

    bool CheckIntegrity();

private:
    bool m_bIsLiveContextMode;
    bool m_bIsLiveContextPreview;
    LcModeType m_iModeType;
    CString m_csLrd;
    CString m_csPipeName;
    CString m_csHtml;
    CString m_csProfile;
    CString m_csTitle;
};
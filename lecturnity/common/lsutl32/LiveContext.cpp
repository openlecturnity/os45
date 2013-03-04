#include "stdafx.h"
#include "LiveContext.h"

CLiveContext::CLiveContext() {
    m_bIsLiveContextMode = false;
    m_iModeType = LcUnknown;
}

CLiveContext::~CLiveContext() {
}

bool CLiveContext::CheckIntegrity() {
    bool bCheckSuccess = true;

    if (m_bIsLiveContextMode) {
        if (m_iModeType == LcUnknown)
            bCheckSuccess = false;
        if (bCheckSuccess && m_csLrd.IsEmpty())
            bCheckSuccess = false;
        if (bCheckSuccess && m_iModeType == LcPublish 
            && (m_csHtml.IsEmpty() || m_csProfile.IsEmpty()))
            bCheckSuccess = false;
    }

    return bCheckSuccess;
};
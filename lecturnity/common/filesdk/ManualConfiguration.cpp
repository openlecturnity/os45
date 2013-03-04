#include "stdafx.h"
#include "ManualConfiguration.h"

CManualConfiguration::CManualConfiguration(void) {
    m_iSlidesNumberPerPage = 2;
    m_bShowHeader = true;
    m_bShowFooter = true;
    m_bShowCoverSheet = true;
    m_bShowTableOfContent = true;
}

CManualConfiguration::~CManualConfiguration(void) {
}

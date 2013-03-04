#include "StdAfx.h"
#include "announcement.h"

CAnnouncement::CAnnouncement()
{
   m_csTitle = "";
   m_csUrl   = "";
   m_bReleaseNote = false;
   m_bViewed = false;
}

CAnnouncement::CAnnouncement(CString csTitle, CString csUrl, bool bReleaseNote)
{
   m_csTitle = csTitle;
   m_csUrl = csUrl;
   m_bReleaseNote = bReleaseNote;
   m_bViewed = false;
}

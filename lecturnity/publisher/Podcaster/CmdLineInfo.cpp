// CmdLineInfo.cpp: implementation of the CCmdLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Podcaster.h"
#include "CmdLineInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdLineInfo::CCmdLineInfo()
{
   m_dwFlags = 0;
   m_csMediaFileName.Empty();
   m_csTitle.Empty();
   m_csAuthor.Empty();
}

CCmdLineInfo::~CCmdLineInfo()
{

}

void CCmdLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bSwitch, BOOL bLast)
{

   CString m_csFlag;
   CString m_csParam = CString(lpszParam);

   if(bSwitch)
   {
      // settings
      m_csFlag = _T("settings");
      if(m_csFlag.CompareNoCase(m_csParam.Left(m_csFlag.GetLength())) == 0)
      {
         m_dwFlags |= CMD_SETTINGS;
         return;
      }
      // transfer
      m_csFlag = _T("transfer:");
      if(m_csFlag.CompareNoCase(m_csParam.Left(m_csFlag.GetLength())) == 0)
      {
         m_dwFlags |= CMD_UPLOAD;
         m_csMediaFileName = m_csParam.Right(m_csParam.GetLength() - m_csFlag.GetLength());
         return;
      }
      // title
      m_csFlag = _T("title:");
      if(m_csFlag.CompareNoCase(m_csParam.Left(m_csFlag.GetLength())) == 0)
      {
         m_dwFlags |= CMD_TITLE;
         m_csTitle = m_csParam.Right(m_csParam.GetLength() - m_csFlag.GetLength());
         return;
      }
      // author
      m_csFlag = _T("author:");
      if(m_csFlag.CompareNoCase(m_csParam.Left(m_csFlag.GetLength())) == 0)
      {
         m_dwFlags |= CMD_AUTHOR;
         m_csAuthor = m_csParam.Right(m_csParam.GetLength() - m_csFlag.GetLength());
         return;
      }
   }
   
   // unknown
   m_csUnknown = m_csParam;
   m_dwFlags |= CMD_UNKNOWN;
}

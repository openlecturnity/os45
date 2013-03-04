// CmdLineInfo.h: interface for the CCmdLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDLINEINFO_H__149D647F_A422_40B0_B815_F54ECE5004B2__INCLUDED_)
#define AFX_CMDLINEINFO_H__149D647F_A422_40B0_B815_F54ECE5004B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h>

#define CMD_SETTINGS 1
#define CMD_UPLOAD   2
#define CMD_TITLE    4
#define CMD_AUTHOR   8
#define CMD_TRANSFER (CMD_UPLOAD | CMD_AUTHOR | CMD_TITLE) 
#define CMD_TRANSFER2 (CMD_UPLOAD | CMD_TITLE) 
#define CMD_TRANSFER3 (CMD_UPLOAD | CMD_AUTHOR) 
#define CMD_UNKNOWN  0x8000000

class CCmdLineInfo : public CCommandLineInfo
{
public:
   CCmdLineInfo();
   virtual ~CCmdLineInfo();
// Overrides
public:
   DWORD m_dwFlags;
   CString m_csMediaFileName;
   CString m_csTitle;
   CString m_csAuthor;
   CString m_csUnknown;
   virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
};

#endif // !defined(AFX_CMDLINEINFO_H__149D647F_A422_40B0_B815_F54ECE5004B2__INCLUDED_)

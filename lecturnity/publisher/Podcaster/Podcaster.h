// Podcaster.h : main header file for the PODCASTER application
//

#if !defined(AFX_PODCASTER_H__86E548D5_6C10_4A06_BE66_BD7A0BC1EE45__INCLUDED_)
#define AFX_PODCASTER_H__86E548D5_6C10_4A06_BE66_BD7A0BC1EE45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
   #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"      // main symbols
#include "SingleInstanceApp.h"
#include "CmdLineInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CPodcasterApp:
// See Podcaster.cpp for the implementation of this class
//

class CPodcasterApp : public /* CWinApp */ CSingleInstanceApp
{
   TCHAR tstrTmpDirectory[MAX_PATH + 1];

   CString csAuthor;
   CString csTitle;

   BOOL FindLrdFile();
   //CString GetActiveChannel();
   void SystemError();
public:
   CCmdLineInfo m_cmdInfo;
   CString csTmpDirectory;

public:
   CPodcasterApp();

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CPodcasterApp)
   public:
   virtual BOOL InitInstance();
   //}}AFX_VIRTUAL

// Implementation

   //{{AFX_MSG(CPodcasterApp)
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PODCASTER_H__86E548D5_6C10_4A06_BE66_BD7A0BC1EE45__INCLUDED_)

// LmdTree.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "LmdManip.h"
#include "LmdManipDlg.h"
#include "LmdTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLmdTree

CLmdTree::CLmdTree()
{
}

CLmdTree::~CLmdTree()
{
}


BEGIN_MESSAGE_MAP(CLmdTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CLmdTree)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CLmdTree 

void CLmdTree::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
   CString csFileName;
   DWORD dwSize = ::DragQueryFile(hDropInfo, 0, NULL, 0);
   _TCHAR *tszFileName = new _TCHAR[dwSize+1];
   ::DragQueryFile(hDropInfo, 0, tszFileName, dwSize+1);
   csFileName = tszFileName;
   delete[] tszFileName;

   //MessageBox(csFileName);
   ((CLmdManipDlg *) GetParent())->LoadLmdFile(csFileName);
	
	CTreeCtrl::OnDropFiles(hDropInfo);
}

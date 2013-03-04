// ConfirmDelete.cpp : implementation file
//

#include "stdafx.h"
#include "assistant.h"
#include "ConfirmDelete.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfirmDelete dialog


CConfirmDelete::CConfirmDelete(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmDelete::IDD, pParent)
{
   //{{AFX_DATA_INIT(CConfirmDelete)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConfirmDelete::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmDelete)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmDelete, CDialog)
   ON_COMMAND(IDYES, OnYes)
   ON_COMMAND(IDNO, OnNo)
//{{AFX_MSG_MAP(CConfirmDelete)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CConfirmDelete::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

   DWORD dwIds[] = {IDC_STATIC_CONFIRM_DELETE_TEXT,
                    IDYES,
                    IDNO,
                   -1};
   MfcUtils::Localize(this, dwIds);
   
   CString csTitle;
   csTitle.LoadString(IDS_CONFIRM_DELETE_TITLE);
   SetWindowText(csTitle);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfirmDelete::OnYes()
{
   EndDialog(IDYES);
}

void CConfirmDelete::OnNo()
{
   EndDialog(IDNO);
}


/////////////////////////////////////////////////////////////////////////////
// CConfirmDelete message handlers

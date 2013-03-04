// ConfigureDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lsupdt32.h"
#include "ConfigureDialog.h"

#include "MfcUtils.h"     // MftUtils

// CConfigureDialog

IMPLEMENT_DYNAMIC(CConfigureDialog, CPropertySheet)

CConfigureDialog::CConfigureDialog(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CConfigureDialog::CConfigureDialog(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

CConfigureDialog::~CConfigureDialog()
{
}


BEGIN_MESSAGE_MAP(CConfigureDialog, CPropertySheet)
END_MESSAGE_MAP()


// CConfigureDialog-Meldungshandler

BOOL CConfigureDialog::OnInitDialog()
{
   BOOL bResult = CPropertySheet::OnInitDialog();

   DWORD dwIds[] = {IDOK,
                    IDCANCEL,
                    -1};

   MfcUtils::Localize(this, dwIds);

   CWnd *pApply = GetDlgItem(ID_APPLY_NOW);
   if(pApply != NULL)
      pApply->DestroyWindow();
   
   CWnd *pHelp = GetDlgItem(IDHELP);
   if(pHelp != NULL)
      pHelp->DestroyWindow();
   
   CRect rectBtn;
   int nSpacing = 6;        // space between two buttons...

   int ids [] = {IDOK, IDCANCEL};
   for( int i =0; i < sizeof(ids)/sizeof(int); i++)
   {
      GetDlgItem (ids [i])->GetWindowRect (rectBtn);
      
      ScreenToClient (&rectBtn);
      int btnWidth = rectBtn.Width();
      rectBtn.left = rectBtn.left + (btnWidth + nSpacing);
      rectBtn.right = rectBtn.right + (btnWidth + nSpacing);
      
      GetDlgItem (ids [i])->MoveWindow(rectBtn);
   }
   return bResult;
}

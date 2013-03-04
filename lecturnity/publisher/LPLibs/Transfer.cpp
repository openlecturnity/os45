#include "StdAfx.h"
#include "resource.h"
#include "TransferConfigure.h"

#include "MySheet.h"

UINT Transfer_Configure(DWORD dwIndex, HWND hWndParent)
{
   // TODO: sollte in Transfer-Engine verlagert werden

   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CMySheet mySheet(_T("Datei-Übertragung"));
   //CPropertyPage p1(IDD_TRANSFER_CONFIGURE2);
   CTransferConfigure p1;
   mySheet.AddPage(&p1);
   mySheet.DoModal();

   return S_OK;
}
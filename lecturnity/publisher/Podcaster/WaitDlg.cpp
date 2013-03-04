// WaitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Podcaster.h"
#include "WaitDlg.h"


// CWaitDlg dialog

IMPLEMENT_DYNAMIC(CWaitDlg, CDialog)

CWaitDlg::CWaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDlg::IDD, pParent)
{

}


CWaitDlg::~CWaitDlg()
{
}

void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WAIT_MESSAGE, m_Message);
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATE()
//	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


// CWaitDlg message handlers

void CWaitDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	CWnd *pParent = GetParent();
	if(bShow)
	{
		//disable parent
		if(pParent)
			pParent->EnableWindow(false);
		UpdateWindow();
	}
	else
	{
		//enable parent
		if(pParent)
			pParent->EnableWindow(true);
	}

	// TODO: Add your message handler code here
}

void CWaitDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here

		UpdateWindow();

}

//int CWaitDlg::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	UpdateWindow();
//
//	return CDialog::OnMouseActivate(pDesktopWnd, nHitTest, message);
//}

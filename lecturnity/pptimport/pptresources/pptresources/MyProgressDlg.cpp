// MyProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pptresources.h"
#include "MfcUtils.h"
#include "MyProgressDlg.h"


// CMyProgressDlg dialog

IMPLEMENT_DYNAMIC(CMyProgressDlg, CDialog)

CMyProgressDlg::CMyProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyProgressDlg::IDD, pParent)
{
	aiProgressVal[TITLE_MASTER] = 3;
	aiProgressVal[MASTER]		= 12;
	aiProgressVal[TEMPLATE]		= 15;
	aiProgressVal[SLIDE]		= 70;
	iSelectedCall				= TITLE_MASTER;
	m_iDurationVal				= 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

CMyProgressDlg::~CMyProgressDlg()
{
	
}

void CMyProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_pctProgress);
	DDX_Control(pDX, ID_WIZBACK, m_btnBack);
	DDX_Control(pDX, ID_WIZNEXT, m_btnNext);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CMyProgressDlg, CDialog)
	ON_MESSAGE(WM_SELECT_CALL, OnSelectCall)
	ON_MESSAGE(WM_CHANGE_PROGRESS, OnChangeProgress)
	ON_MESSAGE(WM_END_PROGRESS, OnEndProgress)
	ON_MESSAGE(WM_CHANGE_PROG_TEXT, OnChangeProgText)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMyProgressDlg message handlers

BOOL CMyProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetIcon(m_hIcon, false);
	
	*m_pHwnd = GetSafeHwnd();
	if(m_pEvent != NULL)
		m_pEvent->SetEvent();

	DWORD dwIds [] = {IDC_PROGRESS_TITLE_LABEL,
		ID_WIZBACK,
		ID_WIZNEXT,
		IDCANCEL,
					-1
					};

	MfcUtils::Localize(this, dwIds);

	CString csDlgTitle;
	csDlgTitle.LoadString(IDS_DIALOG_TITLE);

	SetWindowText(csDlgTitle);

	m_btnBack.EnableWindow(false);
	m_btnNext.EnableWindow(false);
	if(!m_bIsWizard)
	{
		m_btnBack.ShowWindow(SW_HIDE);
		m_btnNext.ShowWindow(SW_HIDE);
	}
   else
   {
      CRect btnRect;
		m_btnCancel.GetWindowRect(&btnRect);
      
		CRect client;
		GetClientRect(&client);
   
      CRect rectNext;
      m_btnNext.GetWindowRect(rectNext);

      int x = rectNext.right + 9;
		btnRect.SetRect(x, btnRect.top, x + btnRect.Width(), btnRect.top  + btnRect.Height());
		ScreenToClient(&btnRect);
		m_btnCancel.MoveWindow(&btnRect);
   }

	CString labelText;
	labelText.LoadString(IDS_TIME_LEFT);

	CWnd* label = GetDlgItem(IDC_LABEL_TIME);
	label->SetWindowText(labelText);

	label = GetDlgItem(IDC_LABEL_DURATION);
	labelText.LoadString(IDS_DURATION);
	labelText += _T("0:00");
	label->SetWindowText(labelText);

	m_iTimer = SetTimer(1, 1000, 0);
	m_pctProgress.SetRange(0,100);
	m_pctProgress.SetPos(0);
	
	SetForegroundWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMyProgressDlg::Init(HWND *pHwnd, CEvent * pEvent, bool bIsWizard)
{
	m_pHwnd		= pHwnd;
	m_pEvent	= pEvent;
	m_bIsWizard	= bIsWizard;
}

LRESULT CMyProgressDlg::OnSelectCall(WPARAM wParam, LPARAM lParam)
{
	iSelectedCall = (int) wParam;
	
	return TRUE;
}

LRESULT CMyProgressDlg::OnChangeProgress(WPARAM wParam, LPARAM lParam)
{
  
	static float iProgressPos = 0.0;
	int nCurrSlide = -1;
	int nSlideCount = 0;
	if((iSelectedCall == TITLE_MASTER) || ((int)wParam == 0))
	{
		iProgressPos = aiProgressVal[iSelectedCall];
	}
	else
	{
		float pace = ((float)aiProgressVal[iSelectedCall] / (float)wParam);
		iProgressPos += pace;
	}
	m_pctProgress.SetPos(iProgressPos);

	CString csWText;
	csWText.LoadString(IDS_DIALOG_TITLE);

	CString csPercent;
	csPercent.Format(_T("%d"),m_pctProgress.GetPos());
	csPercent += _T("% ");
	
	csWText = csPercent + csWText;
	SetWindowText(csWText);

	CString csText;
	if(iSelectedCall == TEMPLATE)
	{
		csText.LoadString(IDS_PROGRESS_TEMPLATES);
	}
	if(iSelectedCall == SLIDE)
	{
		
		CString csSlide;
		csSlide.LoadString(IDS_PROGRESS_TEXT);
		CString csFrom;
		csFrom.LoadString(IDS_PROGRESS_TEXT1);
		nCurrSlide = (int) lParam;
		nSlideCount = (int) wParam;
		
		csText.Format(_T("%s %d %s %d"),csSlide, (int)lParam, csFrom, (int)wParam);
	}
	
	GetDlgItem(IDC_PROGRESS_TEXT)->SetWindowText(csText);


	////CString cc;
	////cc.Format(_T("--curr pos = %d"), m_pctProgress.GetPos());
	//if((m_pctProgress.GetPos() >= 99) /*|| (nCurrSlide >= nSlideCount)*/)
	//{
	//	Sleep(1000);
	//	KillTimer(m_iTimer);
	//	//MessageBox(_T("Soare:Se Inchide!!!!!"  + cc));
	//	EndDialog(IDOK);
	//	//SendMessage(WM_CLOSE);
	//}

   return TRUE;
}

LRESULT CMyProgressDlg::OnEndProgress(WPARAM wParam, LPARAM lParam)
{
   KillTimer(m_iTimer);
   EndDialog(IDOK);

   return TRUE;
}

LRESULT CMyProgressDlg::OnChangeProgText(WPARAM wParam, LPARAM lParam)
{
	CString csText;
	csText.LoadString(IDS_TIME_LEFT);

	CString * pStr = (CString*)lParam;

	csText += (_T(" ") + *pStr);

	GetDlgItem(IDC_LABEL_TIME)->SetWindowText(csText);

	return TRUE;
}

HBRUSH CMyProgressDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(pWnd->GetDlgCtrlID() == IDC_PROGRESS_TITLE_LABEL)
	{
		pDC->SetBkColor(RGB(0xFF, 0xFF, 0xFF));
		hbr = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	}
	else
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CMyProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	m_iDurationVal ++;
	CString csDuration;
	csDuration.LoadString(IDS_DURATION);

	int iMinutes = m_iDurationVal / 60;
	int iSeconds = m_iDurationVal % 60;

	CString csTime;

	if(iSeconds < 10)
	{
		csTime.Format(_T(" %d:0%d"), iMinutes, iSeconds); 
	}
	else
	{
		csTime.Format(_T(" %d:%d"), iMinutes, iSeconds);
	}

	csDuration += csTime;
	CWnd* durationLabel = GetDlgItem(IDC_LABEL_DURATION);
	durationLabel->SetWindowText(csDuration);

	CDialog::OnTimer(nIDEvent);
}

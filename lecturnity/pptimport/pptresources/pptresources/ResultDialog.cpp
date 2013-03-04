// ResultDialog.cpp : implementation file
//

#include "stdafx.h"
#include "pptresources.h"
#include "ResultDialog.h"
#include "MfcUtils.h"
//#include "lregistry.h"


// CResultDialog dialog

IMPLEMENT_DYNAMIC(CResultDialog, CDialog)

CResultDialog::CResultDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CResultDialog::IDD, pParent)
{
	m_bIsListVisible = false;
	m_ppRestrictedObjects = NULL;
	m_nSize = 0;
	m_bPPTCall = true;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
    m_bDoRecord = false;
}

CResultDialog::~CResultDialog()
{
}

void CResultDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DETAILS, m_listBox);
	DDX_Control(pDX, IDC_SEPARATOR, m_staticSeparator);
	DDX_Control(pDX, IDC_OPEN, m_buttonOpen);
	DDX_Control(pDX, IDC_PREVIOUS, m_buttonPrevious);
	DDX_Control(pDX, IDC_QUIT, m_buttonQuit);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_BUTTON_DETAILS, m_buttonDetails);
	DDX_Control(pDX, IDC_REMARK, m_staticRemark);
}


BEGIN_MESSAGE_MAP(CResultDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DETAILS, &CResultDialog::OnBnClickedButtonDetails)
	
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_QUIT, &CResultDialog::OnBnClickedQuit)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_OPEN, &CResultDialog::OnBnClickedOpen)
END_MESSAGE_MAP()


// CResultDialog message handlers

BOOL CResultDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, false);

	DWORD dwIds [] = {IDC_QUIT,
		              IDC_BUTTON_DETAILS,
		              IDC_OPEN,
					  IDC_REMARK,
					  IDC_IMPORT_STATUS,
					  IDC_SUMMARY_TITLE_LABEL,
		              IDCANCEL,
					  -1
					 };

	MfcUtils::Localize(this, dwIds);


	CString csString;
    csString.LoadString(ID_WIZBACK);
	SetDlgItemTextW(IDC_PREVIOUS,csString);
	csString.LoadString(IDS_DIALOG_TITLE);
	SetWindowText(csString);

	GetWindowRect(&m_rectDialog);
    CRect rcClient;
    GetClientRect(&rcClient);
    int iYBorder = GetSystemMetrics(SM_CYBORDER);
    m_iYOffset = m_rectDialog.Height() - rcClient.Height() - 2*iYBorder;


	/*m_staticSeparator.GetWindowRect(&m_rectStaticSeparator);
	m_buttonOpen.GetWindowRect(&m_rectButtonOpen);
	m_buttonPrevious.GetWindowRect(&m_rectButtonPrevious);
	m_buttonQuit.GetWindowRect(&m_rectButtonQuit);
	m_buttonCancel.GetWindowRect(&m_rectButtonCancel);*/


	if(!m_bPPTCall)
	{
		m_buttonOpen.ShowWindow(SW_HIDE);
		m_buttonPrevious.ShowWindow(SW_HIDE);

		CString cs;
        if (m_bDoRecord)
		    cs.LoadString(IDS_RECORD);
        else
		    cs.LoadString(IDC_OPEN);

		m_buttonQuit.SetWindowText(cs);
	}

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	 
	CString csColumnName;
	csColumnName.LoadString(IDS_DETAILS_PAGE);
	_TCHAR szColumnName[256];
	_tcscpy(szColumnName, csColumnName);
		   
	lvc.iSubItem = 0;
	lvc.pszText  = szColumnName;
	lvc.cx       = 50;
	lvc.fmt      = LVCFMT_LEFT;
	m_listBox.InsertColumn(0,&lvc);

	csColumnName.LoadString(IDS_DETAILS_OBJECT);
    _tcscpy(szColumnName, csColumnName);

	lvc.iSubItem = 0;
    lvc.pszText  = szColumnName;
    lvc.cx       = 100;
    lvc.fmt      = LVCFMT_LEFT;
    m_listBox.InsertColumn(1,&lvc);

	csColumnName.LoadString(IDS_DETAILS_COUNT);
    _tcscpy(szColumnName, csColumnName);

	lvc.iSubItem = 0;
    lvc.pszText  = szColumnName;
    lvc.cx       = 50;
    lvc.fmt      = LVCFMT_LEFT;
    m_listBox.InsertColumn(2,&lvc);

	csColumnName.LoadString(IDS_DETAILS_DESCRIPTION);
    _tcscpy(szColumnName, csColumnName);

	lvc.iSubItem = 0;
    lvc.pszText  = szColumnName;
    lvc.cx       = 350;
    lvc.fmt      = LVCFMT_LEFT;
    m_listBox.InsertColumn(3,&lvc);

	//if(m_ppRestrictedObjects != NULL)
	if(m_aObjects.GetSize() != 0)
	{
		InsertRestrictedObjects(/*m_ppRestrictedObjects, m_nSize*/);
		m_buttonDetails.ShowWindow(true);
		m_staticRemark.ShowWindow(true);
	}
	else
	{
		m_buttonDetails.ShowWindow(false);
		m_staticRemark.ShowWindow(false);
	}
	SetForegroundWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CResultDialog::SetAssistantCall()
{
	m_bPPTCall = false;
}

void CResultDialog::OnBnClickedButtonDetails()
{
	// TODO: Add your control notification handler code here
	CRect rcCurrent;
    GetWindowRect(&rcCurrent);

	if(m_bIsListVisible)
	{
		m_listBox.ShowWindow(false);
		m_bIsListVisible = false;

		SetWindowPos(NULL, rcCurrent.left, rcCurrent.TopLeft().y, m_rectDialog.Width(), m_rectDialog.Height(),SWP_NOZORDER);		

		m_staticSeparator.SetWindowPos(NULL, 3, 240-m_iYOffset, 437, 2, SWP_NOZORDER);
		m_buttonOpen.SetWindowPos(NULL, 18, 257-m_iYOffset, 75, 23, SWP_NOZORDER);
		m_buttonPrevious.SetWindowPos(NULL, 195, 257-m_iYOffset, 75, 23, SWP_NOZORDER);
		m_buttonQuit.SetWindowPos(NULL, 270, 257-m_iYOffset, 75, 23, SWP_NOZORDER);
		m_buttonCancel.SetWindowPos(NULL, 354, 257-m_iYOffset, 75, 23, SWP_NOZORDER);
				
		/*SetWindowPos(NULL, rectDialog.left, rectDialog.TopLeft().y, m_rectDialog.Width(), m_rectDialog.Height(),SWP_NOZORDER);		
		m_staticSeparator.SetWindowPos(NULL, m_rectStaticSeparator.left, m_rectStaticSeparator.TopLeft().y-29, m_rectStaticSeparator.Width(),m_rectStaticSeparator.Height(), SWP_NOZORDER);
		m_buttonOpen.SetWindowPos(NULL, m_rectButtonOpen.left, m_rectButtonOpen.TopLeft().y-29, m_rectButtonOpen.Width(), m_rectButtonOpen.Height(), SWP_NOZORDER);
		m_buttonPrevious.SetWindowPos(NULL, m_rectButtonPrevious.left, m_rectButtonPrevious.TopLeft().y-29, m_rectButtonPrevious.Width(), m_rectButtonPrevious.Height(), SWP_NOZORDER);
		m_buttonQuit.SetWindowPos(NULL, m_rectButtonQuit.left, m_rectButtonQuit.TopLeft().y-29, m_rectButtonQuit.Width(), m_rectButtonQuit.Height(), SWP_NOZORDER);
		m_buttonCancel.SetWindowPos(NULL, m_rectButtonCancel.left, m_rectButtonCancel.TopLeft().y-29, m_rectButtonCancel.Width(), m_rectButtonCancel.Height(), SWP_NOZORDER);
        */	

		CString csDetails;
        csDetails.LoadString(IDC_BUTTON_DETAILS);
		SetDlgItemTextW(IDC_BUTTON_DETAILS,csDetails);
	}
	else
	{
		m_bIsListVisible = true;
		m_listBox.ShowWindow(true);
		
		SetWindowPos(NULL, rcCurrent.left, rcCurrent.TopLeft().y, m_rectDialog.Width(), m_rectDialog.Height() + 200, SWP_NOZORDER);		
		m_listBox.SetWindowPos(NULL, 12, 230-m_iYOffset, 415, 200, SWP_NOZORDER);
		m_staticSeparator.SetWindowPos(NULL, 3, 440-m_iYOffset, 437, 2, SWP_NOZORDER);
		m_buttonOpen.SetWindowPos(NULL, 18, 457-m_iYOffset, 75, 23, SWP_NOZORDER);
		m_buttonPrevious.SetWindowPos(NULL, 195, 457-m_iYOffset, 75, 23, SWP_NOZORDER);
		m_buttonQuit.SetWindowPos(NULL, 270, 457-m_iYOffset, 75, 23, SWP_NOZORDER);
		m_buttonCancel.SetWindowPos(NULL, 354, 457-m_iYOffset, 75, 23, SWP_NOZORDER);
		
		/*m_staticSeparator.SetWindowPos(NULL, m_rectStaticSeparator.left, 440, m_rectStaticSeparator.Width(), m_rectStaticSeparator.Height(), SWP_NOZORDER);
		m_buttonOpen.SetWindowPos(NULL, m_rectButtonOpen.left, 455, m_rectButtonOpen.Width(), m_rectButtonOpen.Height(), SWP_NOZORDER);
		m_buttonPrevious.SetWindowPos(NULL, m_rectButtonPrevious.left, 455, m_rectButtonPrevious.Width(), m_rectButtonPrevious.Height(), SWP_NOZORDER);
		m_buttonQuit.SetWindowPos(NULL, m_rectButtonQuit.left, 455, m_rectButtonQuit.Width(), m_rectButtonQuit.Height(), SWP_NOZORDER);
		m_buttonCancel.SetWindowPos(NULL, m_rectButtonCancel.left, 455, m_rectButtonCancel.Width(), m_rectButtonCancel.Height(), SWP_NOZORDER);
        */

		CString csDetails;
        csDetails.LoadString(IDS_BUTTON_DETAILS_PRESSED);
		SetDlgItemTextW(IDC_BUTTON_DETAILS,csDetails);	  
	}
}



HBRUSH CResultDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	
    HBRUSH hbr = NULL;

	// TODO:  Change any attributes of the DC here
	if(pWnd->GetDlgCtrlID() == IDC_SUMMARY_TITLE_LABEL)
	{
		pDC->SetBkColor(RGB(0xFF, 0xFF, 0xFF));
		hbr = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	}
	else
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	return hbr;
}

void CResultDialog::InsertRestrictedObjects(/*WCHAR **ppObjects, UINT nSize*/)
{
	CString csPage, csObject, csCount, csDescription;
	CString resToken;
	UINT nIdObject, nIdDescription, nObject;
	_TCHAR szBuff[MAX_PATH];
	int nCount = 0;
	int curPos = 0;
	
	LV_ITEM lvi1;
	LV_ITEM lvi2;
	LV_ITEM lvi3;
	LV_ITEM lvi4;

	//CString *csRestricted = new CString[nSize];
	/*for(int i=0; i<nSize; i++)
		csRestricted[i] = ppObjects[i];*/

	//for(int i=0; i < nSize; i++)
	for(int i=0; i < m_aObjects.GetSize(); i++)
	{
		//resToken = csRestricted[i].Tokenize(_T(","), curPos);
		resToken = m_aObjects.GetAt(i).Tokenize(_T(","), curPos);
		while (resToken != "")
		{
			_tcscpy(szBuff, resToken);

			lvi1.mask      = LVIF_TEXT;
			lvi1.iItem     = nCount;
			lvi1.iSubItem  = 0;
			lvi1.pszText   = szBuff;

			m_listBox.InsertItem(&lvi1);

			//resToken = csRestricted[i].Tokenize(_T(","),curPos);
			resToken = m_aObjects.GetAt(i).Tokenize(_T(","), curPos);
			_tcscpy(szBuff, resToken);

			nObject = _tcstod(szBuff, NULL);
			nIdObject = nObject + 700;
			csObject.LoadString(nIdObject);
			_tcscpy(szBuff, csObject);

			lvi2.mask      = LVIF_TEXT;
			lvi2.iItem     = nCount;
			lvi2.iSubItem  = 1;
			lvi2.pszText   = szBuff;

			m_listBox.SetItem(&lvi2);

			//resToken = csRestricted[i].Tokenize(_T(","),curPos);
			resToken = m_aObjects.GetAt(i).Tokenize(_T(","), curPos);
			_tcscpy(szBuff, resToken);

			lvi3.mask      = LVIF_TEXT;
			lvi3.iItem     = nCount;
			lvi3.iSubItem  = 2;
			lvi3.pszText   = szBuff;

			m_listBox.SetItem(&lvi3);

			//resToken = csRestricted[i].Tokenize(_T(","), curPos);
			resToken = m_aObjects.GetAt(i).Tokenize(_T(","), curPos);

			nIdDescription = nObject + 730;
			csDescription.LoadString(nIdDescription);
			_tcscpy(szBuff, csDescription);

			lvi4.mask      = LVIF_TEXT;
			lvi4.iItem     = nCount;
			lvi4.iSubItem  = 3;
			lvi4.pszText   = szBuff;

			m_listBox.SetItem(&lvi4);

			nCount ++;
		}
		curPos = 0;
	}
}

void CResultDialog::OnBnClickedQuit()
{
	EndDialog(IDC_QUIT);
	// TODO: Add your control notification handler code here
}

void CResultDialog::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	EndDialog(IDCLOSE);
}

void CResultDialog::OnBnClickedOpen()
{
	// TODO: Add your control notification handler code here
	
    CString csInstallPath;
	 ReadProgramPath(csInstallPath);

	if (!csInstallPath.IsEmpty()) 
	{
		csInstallPath += _T("\\Backend");

		CFile file;
		CFileException e;
		if(file.Open(m_csPath, CFile::modeRead, &e))
		{
			file.Close();
			ShellExecute(NULL, _T("open"), m_csPath, NULL, csInstallPath, SW_SHOWNORMAL);
			EndDialog(IDC_OPEN);
		}
		else
		{
			TCHAR msg[MAX_PATH];
			e.GetErrorMessage(msg, MAX_PATH);
			MessageBox( msg);
		}

	}

}

bool CResultDialog::ReadProgramPath(CString &programPath)
{
   _TCHAR tszInstallDir[MAX_PATH];
   DWORD dwSize = MAX_PATH;
   bool success = 
	   ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
                                         _T("SOFTWARE\\imc AG\\LECTURNITY"), _T("InstallDir"),
                                         tszInstallDir, &dwSize);
   if (success)
      programPath = tszInstallDir;
   else
      programPath = _T("..\\");

   return success;
}

bool CResultDialog::ReadRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue,
                                              DWORD dwType, char *pData, DWORD *pcbSize)
{
   HKEY hkKey   = NULL;
   LONG res = ::RegOpenKeyEx(hkHive, szKey, 0, KEY_READ, &hkKey);
   if (res == ERROR_SUCCESS)
   {
      res = ::RegQueryValueEx(hkKey, szValue, NULL, &dwType, (LPBYTE) pData, pcbSize);
   }
   
   return (res == ERROR_SUCCESS);
}

bool CResultDialog::ReadStringRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, 
                                        _TCHAR *tszString, DWORD *pcbSize)
{
   bool success = ReadRegistryEntry(hkHive, szKey, szValue, REG_SZ, (char *)tszString, pcbSize);
   if (!success)
      success = ReadRegistryEntry(hkHive, szKey, szValue, REG_EXPAND_SZ, (char *)tszString, pcbSize);
   return success;
}

void CResultDialog::InsertObjects(WCHAR **ppObjects, UINT nSize)
{
	m_ppRestrictedObjects = ppObjects;
	m_nSize = nSize; 
}

void CResultDialog::InsertObjects(CArray<CString, CString>&aObjects)
{
	for(int i = 0; i < aObjects.GetSize(); i++)
	{
		m_aObjects.Add(aObjects.GetAt(i));
	}
}

// DlgMetadata.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "DlgMetadata.h"
#include "MfcUtils.h"
#include "XPGroupBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BOTTOM_POSITION  (145)
const int EDIT_RECORD_PATH_WIDTH = 200;
/////////////////////////////////////////////////////////////////////////////
// CDlgMetadata dialog


CDlgMetadata::CDlgMetadata(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMetadata::IDD, pParent)
{
	_TCHAR nUser[100];
   unsigned long nUserSize = 100;
   GetUserName(nUser, &nUserSize);

   if(pParent!=NULL)
   {
      pParent->EnableWindow(false);
      pParent->EnableWindow(true);
   }
      //{{AFX_DATA_INIT(CDlgMetadata)
	 
   m_csTitle = _T("");
   m_csAuthor = nUser;
	m_csProducer = nUser;
	m_csKeywords = _T("");	
   m_bTitleVisibility = true;
   m_gbCustomGroupBox = new CXPGroupBox();
	//}}AFX_DATA_INIT
}


CDlgMetadata::~CDlgMetadata()
{
   SAFE_DELETE(m_gbCustomGroupBox);
}
void CDlgMetadata::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMetadata)
   DDX_Text(pDX, IDC_TITLE, m_csTitle);
	DDX_Text(pDX, IDC_AUTHOR, m_csAuthor);
	DDX_Text(pDX, IDC_PRODUCER, m_csProducer);
	DDX_Text(pDX, IDC_METADATA_KEYWORDS, m_csKeywords);
   
   DDX_Text(pDX, IDC_METADATA_RECORD_DATE, m_csRecordDate);
   DDX_Text(pDX, IDC_METADATA_RECORD_TIME, m_csRecordTime);
   DDX_Text(pDX, IDC_METADATA_RECORD_LENGTH, m_csRecordLength);
   DDX_Text(pDX, IDC_METADATA_RECORD_PATH, m_csRecordPath);
   
   DDX_Control(pDX, IDC_STATIC_METADATA, *m_gbCustomGroupBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMetadata, CDialog)
	//{{AFX_MSG_MAP(CDlgMetadata)
   ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDOK, &CDlgMetadata::OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMetadata message handlers


BOOL CDlgMetadata::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   DWORD dwIds[] = {IDC_STATIC_PROP_TITLE,
                    IDC_STATIC_PROP_AUTHOR, 
                    IDC_STATIC_PROP_PRODUCER,
                    IDC_STATIC_PROP_KEYWORDS,
                    IDC_STATIC_PROP_RECORD_DATE,
                    IDC_STATIC_PROP_RECORD_TIME,
                    IDC_STATIC_PROP_RECORD_LENGTH,
                    IDC_STATIC_PROP_RECORD_PATH,
                    IDOK,
                    IDCANCEL,
                   -1};
   MfcUtils::Localize(this, dwIds);
   
   CString csTitle;
   csTitle.LoadString(ID_PROPERTIES);
   SetWindowText(csTitle);

   if(m_bTitleVisibility == false)
   {
      SetNewPosition();

      HideTitle();
      
      HideLabel(IDC_STATIC_PROP_RECORD_DATE);
      HideLabel(IDC_STATIC_PROP_RECORD_TIME);
      HideLabel(IDC_STATIC_PROP_RECORD_LENGTH);
      HideLabel(IDC_STATIC_PROP_RECORD_PATH);

      HideEditBox(IDC_METADATA_RECORD_DATE);
      HideEditBox(IDC_METADATA_RECORD_TIME);
      HideEditBox(IDC_METADATA_RECORD_LENGTH);
      HideEditBox(IDC_METADATA_RECORD_PATH);
   }
   ProcessRecordPath(m_csRecordPath);
   //UpdateData();
   CEdit* pEdtPath = (CEdit*)GetDlgItem(IDC_METADATA_RECORD_PATH);
   pEdtPath->SetWindowText(m_csRecordPath);
   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMetadata::Init(CString &csTitle, CString &csAuthor, 
                        CString &csProducer, CString &csKeywords)
{
   m_csTitle = csTitle;
   m_csAuthor = csAuthor;
   m_csProducer = csProducer;
   m_csKeywords = csKeywords;
}

void CDlgMetadata::SetRecordInfo(CString &csRecordDate, CString &csRecordTime, 
                        CString &csRecordLength, CString &csRecordPath)
{
   m_csRecordDate = csRecordDate;
   m_csRecordTime = csRecordTime;
   m_csRecordLength = csRecordLength;
   m_csRecordPath = csRecordPath;
}

HBRUSH CDlgMetadata::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) 
{
   
   HBRUSH hbr = NULL;

   if ( nCtlColor == CTLCOLOR_STATIC )
    {
      pDC->SetBkColor(RGB(255, 255, 255));
      hbr = CreateSolidBrush(RGB(255, 255, 255));
    }
   else
      hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);  

return hbr;
}
void CDlgMetadata::OnBnClickedOk()
{
   // TODO: Add your control notification handler code here
   OnOK();
}
void CDlgMetadata::SetTitleVisibility(bool bVisibility)
{
   m_bTitleVisibility = bVisibility;
}

void CDlgMetadata::HideTitle()
{
  HideEditBox(IDC_TITLE);
  HideLabel(IDC_STATIC_PROP_TITLE);
}

void CDlgMetadata::HideEditBox(UINT nID)
{
  CEdit* pEditBox = (CEdit*)GetDlgItem(nID);
  pEditBox->ShowWindow(SW_HIDE);
}
void CDlgMetadata::HideLabel(UINT nID)
{
  CStatic* pStaticLabel = (CStatic*)GetDlgItem(nID);
  pStaticLabel->ShowWindow(SW_HIDE);
}
void CDlgMetadata::SetNewPosition()
{
  WINDOWPLACEMENT wndTitlePlacement;
  WINDOWPLACEMENT wndAuthorPlacement;
  WINDOWPLACEMENT wndProducerPlacement;

  WINDOWPLACEMENT wndStaticTitlePlacement;
  WINDOWPLACEMENT wndStaticAuthorPlacement;
  WINDOWPLACEMENT wndStaticProducerPlacement;
 
  WINDOWPLACEMENT wndGroupBoxPlacement;

  WINDOWPLACEMENT wndButtonOkPlacement;
  WINDOWPLACEMENT wndButtonCancelPlacement;

  WINDOWPLACEMENT wndDialogPlacement;

  CRect rcGroupBox;
  
  CEdit* pTitleEditBox = (CEdit*)GetDlgItem(IDC_TITLE);
  pTitleEditBox->GetWindowPlacement(&wndTitlePlacement);

  CStatic* pTitleLabel = (CStatic*)GetDlgItem(IDC_STATIC_PROP_TITLE);
  pTitleLabel->GetWindowPlacement(&wndStaticTitlePlacement);

  CEdit* pAuthorEditBox = (CEdit*)GetDlgItem( IDC_AUTHOR);
  pAuthorEditBox->GetWindowPlacement(&wndAuthorPlacement);
  pAuthorEditBox->SetWindowPlacement(&wndTitlePlacement);

  CEdit* pProducerEditBox = (CEdit*)GetDlgItem( IDC_PRODUCER);
  pProducerEditBox->GetWindowPlacement(&wndProducerPlacement);
  pProducerEditBox->SetWindowPlacement(&wndAuthorPlacement);

  CEdit* pKeywordsEditBox = (CEdit*)GetDlgItem(IDC_METADATA_KEYWORDS);
  pKeywordsEditBox->SetWindowPlacement(&wndProducerPlacement);


  CStatic* pAuthorLabel = (CStatic*)GetDlgItem( IDC_STATIC_PROP_AUTHOR);
  pAuthorLabel->GetWindowPlacement(&wndStaticAuthorPlacement);
  pAuthorLabel->SetWindowPlacement(&wndStaticTitlePlacement);

  CStatic* pProducerLabel  = (CStatic*)GetDlgItem( IDC_STATIC_PROP_PRODUCER);
  pProducerLabel->GetWindowPlacement(&wndStaticProducerPlacement);
  pProducerLabel->SetWindowPlacement(&wndStaticAuthorPlacement);

  CStatic* pKeywordsLabel = (CStatic*)GetDlgItem(IDC_STATIC_PROP_KEYWORDS);
  pKeywordsLabel->SetWindowPlacement(&wndStaticProducerPlacement);
 
  m_gbCustomGroupBox->GetClientRect(&rcGroupBox);
  rcGroupBox.bottom -= 50;
  m_gbCustomGroupBox->GetWindowPlacement(&wndGroupBoxPlacement);
  wndGroupBoxPlacement.rcNormalPosition.bottom -= BOTTOM_POSITION;
  m_gbCustomGroupBox->SetWindowPlacement(&wndGroupBoxPlacement);

  CButton* pButtonOk = (CButton*)GetDlgItem( IDOK);
  pButtonOk->GetWindowPlacement(&wndButtonOkPlacement);
  wndButtonOkPlacement.rcNormalPosition.top -=BOTTOM_POSITION;
  wndButtonOkPlacement.rcNormalPosition.bottom -=BOTTOM_POSITION;
  pButtonOk->SetWindowPlacement(&wndButtonOkPlacement);

  CButton* pButtonCancel = (CButton*)GetDlgItem( IDCANCEL);
  pButtonCancel->GetWindowPlacement(&wndButtonCancelPlacement);
  wndButtonCancelPlacement.rcNormalPosition.top -=BOTTOM_POSITION;
  wndButtonCancelPlacement.rcNormalPosition.bottom -=BOTTOM_POSITION;
  pButtonCancel->SetWindowPlacement(&wndButtonCancelPlacement);

  CRect rcDesktop;
  CWnd * wndDesktop = GetDesktopWindow();
  wndDesktop->GetWindowRect(&rcDesktop);
  CPoint cpDesktopCenterPoint = rcDesktop.CenterPoint();
  int iNewOffset;
  GetWindowPlacement(&wndDialogPlacement); 
  if(wndDialogPlacement.rcNormalPosition.top != 0)
  {
    iNewOffset = cpDesktopCenterPoint.x/2;
    wndDialogPlacement.rcNormalPosition.top +=  iNewOffset ;
    wndDialogPlacement.rcNormalPosition.right += iNewOffset ;
    wndDialogPlacement.rcNormalPosition.left += iNewOffset ;
    wndDialogPlacement.rcNormalPosition.bottom += iNewOffset;
    wndDialogPlacement.rcNormalPosition.bottom -= BOTTOM_POSITION;
  }
  else
  {
    iNewOffset = cpDesktopCenterPoint.x/2;
    wndDialogPlacement.rcNormalPosition.top +=  iNewOffset + 80 ;
    wndDialogPlacement.rcNormalPosition.right += iNewOffset+ 80 ;
    wndDialogPlacement.rcNormalPosition.left += iNewOffset + 80;
    wndDialogPlacement.rcNormalPosition.bottom += iNewOffset+ 80;
    wndDialogPlacement.rcNormalPosition.bottom -= BOTTOM_POSITION;
  }
  SetWindowPlacement(&wndDialogPlacement);
  
}

void CDlgMetadata::ProcessRecordPath(CString& csRecordPath) {
    
    CFont* pFont = GetFont();
    CWindowDC dc(NULL);
    CXTPFontDC font(&dc, pFont);
    csRecordPath.Trim();
    int iOrigTextWidth = dc.GetTextExtent(csRecordPath, csRecordPath.GetLength()).cx;
    int iPos = 0;
    CArray<CString, CString>aParts;
    /*int iFindPos = csRecordPath.Find(_T("\\"), iPos);
    while (iFindPos != -1) {
        aSlashPos.Add(iFindPos);
        iPos = iFindPos + 1;
        iFindPos = csRecordPath.Find(_T("\\"), iPos);
    }*/
    CArray<int, int>aiWidths;
    CString csTok = csRecordPath.Tokenize(_T("\\"), iPos);
    while (csTok != _T("")) {
        aParts.Add(csTok);
        csTok = csRecordPath.Tokenize(_T("\\"), iPos);
    }
    for (int i = 0; i< aParts.GetCount() ; i++) {
        if (i < aParts.GetCount() -1) {
            aParts.GetAt(i) += _T("\\");
        }
        CString cs = aParts.GetAt(i);
        int iW = dc.GetTextExtent(aParts.GetAt(i), aParts.GetAt(i).GetLength()).cx;
        aiWidths.Add(iW);//(dc.GetTextExtent(aParts.GetAt(i), aParts.GetAt(i).GetLength()).cx);
    }

    CArray<CString, CString>acsEditLines;
    int iWidthsSum = 0;
    for (int i = 0; i < aiWidths.GetCount(); i++) {
        iWidthsSum += aiWidths.GetAt(i);
        if (iWidthsSum > EDIT_RECORD_PATH_WIDTH) {
            acsEditLines.Add(aParts.GetAt(i));
            iWidthsSum = 0;
        } else { 
            if (acsEditLines.GetCount() > 0) {
                acsEditLines.GetAt(acsEditLines.GetCount() - 1) += aParts.GetAt(i);
            } else {
                acsEditLines.Add(aParts.GetAt(i));
            }
        }    
    }

    csRecordPath.Empty();
    for (int i = 0; i < acsEditLines.GetCount(); i++) {
        CString csNewLine = (i < acsEditLines.GetCount() - 1)? _T("\r\n"):_T("");
        csRecordPath += acsEditLines.GetAt(i) + csNewLine;
    }
}
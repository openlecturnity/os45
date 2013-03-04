// DlgRecordRename.cpp : implementation file
//

#include "stdafx.h"
#include "..\Studio\Resource.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "DlgRecordRename.h"
#include "MfcUtils.h"
#include "MiscFunctions.h"
#include "XPGroupBox.h"

#include "LepSgml.h"     // filesdk

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRecordRename dialog

BEGIN_MESSAGE_MAP(CDlgRecordRename, CDialog)
    //{{AFX_MSG_MAP(CDlgRecordRename)
    ON_WM_PAINT()
    ON_BN_CLICKED(IDOK, &CDlgRecordRename::OnBnClickedOk)
    //}}AFX_MSG_MAP
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


CDlgRecordRename::CDlgRecordRename(CWnd* pParent /*=NULL*/, CString csName)
: CDialog(CDlgRecordRename::IDD, pParent)
{
    if (pParent!=NULL){
        pParent->EnableWindow(false);
        pParent->EnableWindow(true);
    }
    //{{AFX_DATA_INIT(CDlgRecordRename)

    m_csPath = csName;
    StringManipulation::GetPath(m_csPath);
    m_csPath += _T("\\"); 

    m_csName = csName;
    StringManipulation::GetFilePrefix(m_csName);

    m_hbrBg = NULL;
    m_bIsInit = false;
    //}}AFX_DATA_INIT
}

CDlgRecordRename::~CDlgRecordRename()
{
    if (m_hbrBg)
        ::DeleteObject(m_hbrBg);
}

void CDlgRecordRename::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgRecordRename)
    DDX_Text(pDX, IDC_RECORD_NEW_LRDNAME, m_csName);
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDOK, m_btnOK);
    DDX_Control(pDX, IDCANCEL, m_btnCancel);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgRecordRename message handlers


BOOL CDlgRecordRename::OnInitDialog() 
{
    CDialog::OnInitDialog();


    DWORD dwIDS[] = {IDOK,
                    IDCANCEL,
                    -1
    };
    MfcUtils::Localize(this, dwIDS);
    CString m_csName;
    m_csName.LoadString(ID_RENAME_RECORD);
    SetWindowText(m_csName);

    m_bIsInit = true;
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRecordRename::OnPaint()
{
    CPaintDC dc(this); 

    CRect rcDialog;
    GetClientRect(&rcDialog);

    CRect rcTitleBg(0, 0, rcDialog.Width(), rcDialog.Height());
    CBrush whiteBrush;
    whiteBrush.CreateSolidBrush(RGB(255, 255, 255));
    dc.FillRect(rcTitleBg, &whiteBrush);
    whiteBrush.DeleteObject();

    CFont *pWindowFont = XTPPaintManager()->GetRegularFont();
    if (pWindowFont != NULL) {
        LOGFONT lf;
        pWindowFont->GetLogFont(&lf);
        HFONT hFontNew, hFontOld;
        lf.lfHeight = 20;
        hFontNew = CreateFontIndirect(&lf);
        if (hFontNew != NULL ) {
            hFontOld = (HFONT) SelectObject(dc.m_hDC, hFontNew);
            CRect rcTitle(MARGIN_OFFSET/*8*/, 8 + 6, rcDialog.Width(), 28 + 6);
            CString csTitle;
            csTitle.LoadString(IDC_RECORD_NEW_LRDNAME_TITLE);

            COLORREF oldColor = dc.SetTextColor(RGB(0, 51, 153));
            dc.DrawText(csTitle, csTitle.GetLength(), rcTitle, DT_LEFT);

            if (hFontOld != NULL) {
                SelectObject(dc.m_hDC, hFontOld);
                DeleteObject(hFontNew);
            }
            dc.SetTextColor(oldColor);
        }

        lf.lfHeight = 14;
        hFontNew = CreateFontIndirect(&lf);
        if (hFontNew != NULL ) {
            hFontOld = (HFONT) SelectObject(dc.m_hDC, hFontNew);

            CRect rcSubTitle(MARGIN_OFFSET/*8*/, 45 + 6, rcDialog.Width(), 130 + 6);
            CString csSubTitle;
            csSubTitle.LoadString(IDC_RECORD_NEW_LRDNAME_HINT);
            COLORREF oldColor = dc.SetTextColor(RGB(0, 0, 0));
            dc.DrawText(csSubTitle, csSubTitle.GetLength(), rcSubTitle, DT_LEFT);

            if (hFontOld != NULL) {
                SelectObject(dc.m_hDC, hFontOld);
                DeleteObject(hFontNew);
            }
            dc.SetTextColor(oldColor);

        }
        else
            DeleteObject(hFontNew);
    }
}

void CDlgRecordRename::OnBnClickedOk() {
    CString csOldName = m_csName;
    UpdateData();

    // There is no change
    if (csOldName.Trim() == m_csName.Trim())
        return CDialog::OnCancel();

    // Test if the entered name contains path information.
    // There should be no path information.
    if (ValidateFileName(m_csName) == false) {
        m_csName = csOldName;
        UpdateData(FALSE);
        return;
    }

    CString csNewName = m_csPath + m_csName;

    // Test if the extension is ".lrd", if not append ".lrd"
    CString csExtension = m_csName;
    StringManipulation::GetFileSuffix(csExtension);
    csExtension.MakeLower();
    if (csExtension != _T("lrd"))
        csNewName = m_csPath + m_csName + _T(".lrd");

    // Test the length of the ne file name
    if (csNewName.GetLength() > 255){
        AfxMessageBox(IDS_FILENAME_TOO_LONG, MB_OK | MB_ICONERROR);
        m_csName = csOldName;
        UpdateData(FALSE);
        return;
    }

    // If the file already exist, show error message
    if (FileExists(csNewName)) {
        AfxMessageBox(IDS_CANNOT_OVERRITE_FILE, MB_OK | MB_ICONERROR);
        m_csName = csOldName;
        UpdateData(FALSE);
        return;
    }

    // Test if there exist a project file with the new name
    // If yes, show error message
    if (FileExists(m_csPath + m_csName + _T(".lep"))){
        AfxMessageBox(IDS_CANNOT_OVERRITE_FILE, MB_OK | MB_ICONERROR);
        m_csName = csOldName;
        UpdateData(FALSE);
        return;
    }

    // Try to rename
    try {
        CFile::Rename(m_csPath + csOldName + _T(".lrd"), csNewName);
    }
    catch (CFileException* pEx){
        pEx->ReportError();
        pEx->Delete();
        m_csName = csOldName;
        UpdateData(FALSE);
        return;
    }

    // If there is a lep file, rename it
    if (FileExists(m_csPath + csOldName + _T(".lep"))) {
        CString csNewName = m_csName;
        CString csNewNameSuff = csNewName;//m_csName.Right(4);
        StringManipulation::GetFileSuffix(csNewNameSuff);
        if(csNewNameSuff.CompareNoCase(_T("lrd")) == 0)
            csNewName = csNewName.Left(csNewName.GetLength() - 4);
        bool bWriteSuccess = true;
        CString csNewLepName = m_csPath + csNewName + _T(".lep");
        try {
            CFile::Rename(m_csPath + csOldName + _T(".lep"), csNewLepName);
        }
        catch (CFileException* pEx){
            //pEx->ReportError();
            pEx->Delete();
            bWriteSuccess = false;
        }

        // All occurences of the renamed lrd file in the lep file has to be changed
        if (bWriteSuccess) {
            CLepSgml projectFile;
            bool bSuccess = projectFile.Read(csNewLepName, _T("<edi"));
            if (bSuccess){
                projectFile.ChangeLrdName(csOldName, m_csName);
                projectFile.Write(csNewLepName);
            }
        }
    }

    CDialog::OnOK();
}

bool CDlgRecordRename::ValidateFileName(CString csText)
{
    //validate text entered.
    for (int i = 0; i< csText.GetLength(); i++){
        _TCHAR c = csText.GetAt(i);
        if ((c == 92)              // "\"
            || (c == 47)){           // "/"
            AfxMessageBox (IDS_FILENAME_WITHOUT_PATH, MB_OK | MB_ICONERROR);
            return false;
        }
    }
    return true;
}

bool CDlgRecordRename::FileExists(CString csFile)
{
    try{
        CFile file;
        if (file.Open(csFile, CFile::modeRead) == TRUE){
            file.Close();
            return true;
        }
    }
    catch (CFileException* pEx){
        pEx->Delete();
    }

    return false;
}
void CDlgRecordRename::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    /*CWnd* pControl;

    pControl = GetDlgItem(IDOK);

    CRect rcControl;
    pControl->GetClientRect(&rcControl);
    pControl->MoveWindow(rcControl.left + cx, rcControl.right + cy, rcControl.Width(), rcControl.Height());*/

    if(m_bIsInit){
        CRect rcClient;
        GetClientRect(&rcClient);
        
        CRect rcControl;
        m_btnCancel.GetClientRect(&rcControl);

        int x = rcClient.right - MARGIN_OFFSET - rcControl.Width();
        int y = rcClient.bottom - MARGIN_OFFSET - rcControl.Height();
        int iButtonHeight = rcControl.Height();
        m_btnCancel.MoveWindow(x, y, rcControl.Width(), rcControl.Height());
        //m_btnCancel.SetWindowPos(NULL, x, y, x + rcControl.Width(), y + rcControl.Height(), SWP_NOZORDER);

        m_btnOK.GetClientRect(&rcControl);
        x = rcClient.right - MARGIN_OFFSET - 2* rcControl.Width() - BUTTONS_DISTANCE;
        y = rcClient.bottom - MARGIN_OFFSET - rcControl.Height();
        m_btnOK.MoveWindow(x, y, rcControl.Width(), rcControl.Height());

        CEdit* pEditCtrl = (CEdit*)GetDlgItem(IDC_RECORD_NEW_LRDNAME);

        if(pEditCtrl){
            pEditCtrl->GetClientRect(&rcControl);
            x = rcClient.left + MARGIN_OFFSET;
            y = TEXT_BOTTOM - 4 + (rcClient.bottom - MARGIN_OFFSET - iButtonHeight - TEXT_BOTTOM - EDIT_HEIGHT) / 2;
            pEditCtrl->MoveWindow(x, y, rcClient.Width() - 2 * MARGIN_OFFSET, EDIT_HEIGHT/*rcControl.Height()*/); 
            
        }

        Invalidate();
    }
}

void CDlgRecordRename::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: Add your message handler code here and/or call default
    lpMMI->ptMinTrackSize.x = MIN_WIDTH;
    lpMMI->ptMinTrackSize.y = MIN_HEIGHT;

    CDialog::OnGetMinMaxInfo(lpMMI);
}

#if !defined(AFX_DLGRECORDRERENAME_H__595CA7C8_2ACA_469A_2F6E_3EB128E9E599__INCLUDED_)
#define AFX_DLGRECORDRERENAME_H__595CA7C8_2ACA_469A_2F6E_3EB128E9E599__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRecordRename.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRecordRename dialog

/**
Shows a dialog to change the file name of a specified recording document (by recording file path).
Pressing OK will validate for:
1. text contain only 0-9, a-zA-Z and _
2. new files must not exists on disk
When dialog closes, some extra steps must occur
1. Update new file name in the internal structures
2. Update registry with new file value (HKCU\Software\imc AG\LECTURNITY\Studio\Files\Lrd*).
*/
class CDlgRecordRename : public CDialog
{
    // Attributes
private:
    CString m_csName;       // file name without path and extension
    CString m_csPath;       // path of the file without file name.
    HBRUSH m_hbrBg;
    // Dialog Data
    enum { IDD = IDD_RECORD_RENAME };

    // Construction
public:
    CDlgRecordRename(CWnd* pParent = NULL, CString csName = _T(""));   // standard constructor
    virtual ~CDlgRecordRename();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    // Implementation
protected:

    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg void OnBnClickedOk();

    bool ValidateFileName(CString csText);
    bool FileExists(CString csFile);

public:
    inline CString GetName(){return m_csName;};
    inline CString SetName(CString csName){m_csName = csName;};
    inline CString GetPath(){return m_csPath;};
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    CButton m_btnOK;
    CButton m_btnCancel;
private:
    bool m_bIsInit;
protected:
    enum DlgSizes{
        MIN_WIDTH = 355,
        MIN_HEIGHT = 250,
        TEXT_BOTTOM = 136,
        EDIT_HEIGHT = 23,
        MARGIN_OFFSET = 20,
        BUTTONS_DISTANCE = 15
    };
public:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};

#endif // !defined(AFX_DLGRECORDRERENAME_H__595CA7C8_2ACA_469A_2F6E_3EB128E9E599__INCLUDED_)

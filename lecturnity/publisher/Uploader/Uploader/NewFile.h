#pragma once


// CNewFile dialog
#include "Resource.h"
#include "afxwin.h"

typedef enum {
    TYPE_MP4,
    TYPE_LZP
}Filetype /*FILETYPE*/;

class CNewFile : public CDialog {
    DECLARE_DYNAMIC(CNewFile)

public:

    CNewFile(CWnd* pParent = NULL);   // standard constructor
    CNewFile(Filetype eType, CWnd* pParent = NULL);
    virtual ~CNewFile();

    // Dialog Data
    enum { IDD = IDD_NEWMP4 };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CStatic m_csLzp;
    CStatic m_csMp4;
    CString GetNewFileName();
private:
    bool m_bFileType;
    afx_msg void OnEnChangeEdit1();
    CEdit m_edtFile;
    CButton m_btnOK;
    CString m_csNewFileName;
    afx_msg void OnBnClickedOk();
};

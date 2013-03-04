#pragma once

// CResultDialog dialog


class CResultDialog : public CDialog
{
	DECLARE_DYNAMIC(CResultDialog)

public:
	CResultDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CResultDialog();

// Dialog Data
	enum { IDD = IDD_RESULT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	bool m_bIsListVisible;
	WCHAR **m_ppRestrictedObjects;
	UINT m_nSize;
	CString m_csPath;

	CListCtrl m_listBox;
	CStatic m_staticSeparator;
	CButton m_buttonOpen;
	CButton m_buttonPrevious;
	CButton m_buttonQuit;
	CButton m_buttonCancel;
	CButton m_buttonDetails;
	CStatic m_staticRemark;
	
	CRect m_rectDialog;
    int m_iYOffset;
	CArray<CString, CString>m_aObjects;
	
	bool m_bPPTCall;
	HICON m_hIcon;

    bool m_bDoRecord;

private:
	afx_msg void OnBnClickedButtonDetails();
	afx_msg void OnBnClickedQuit();
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOpen();
	virtual BOOL OnInitDialog();	
	
private:
	static bool ReadProgramPath(CString &playerPath);
	static bool ReadRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, DWORD dwType, char *pData, DWORD *pcbSize);
    static bool ReadStringRegistryEntry(HKEY hkHive, LPCTSTR szKey, LPCTSTR szValue, _TCHAR *tszString, DWORD *pcbSize);
	void InsertRestrictedObjects(/*WCHAR **ppObjects, UINT nSize*/);

public:
	void InsertObjects(WCHAR **ppObjects, UINT nSize);
	void InsertObjects(CArray<CString, CString>&aObjects);
	void SetAssistantCall();
	void SetPath(CString csPath){m_csPath = csPath;}
	void SetDoRecord(bool bDoRecord) {m_bDoRecord = bDoRecord;}

	

	
	
};




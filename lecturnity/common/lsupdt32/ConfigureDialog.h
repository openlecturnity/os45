#pragma once



// CConfigureDialog

class CConfigureDialog : public CPropertySheet
{
	DECLARE_DYNAMIC(CConfigureDialog)

public:
	CConfigureDialog(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CConfigureDialog(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CConfigureDialog();

protected:
	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
};



#pragma once
//#include "afxwin.h"
#include "Resource.h"


// CProfileNameDialog-Dialogfeld

class CProfileNameDialog : public CDialog
{
	DECLARE_DYNAMIC(CProfileNameDialog)

public:
	CProfileNameDialog(CString csProfileName, CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CProfileNameDialog();

// Dialogfelddaten
	enum { IDD = IDD_PROFILE_SETNAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();

public:
   CString &GetProfileName() {return m_csProfileName;}

private:
   CString m_csProfileName;
protected:
   virtual void OnOK();
public:
   CEdit m_editName;
};

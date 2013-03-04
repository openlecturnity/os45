#pragma once

// Instead of including "StdAfx.h" (like in the other header files):
#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif

#include "resource.h"
#include "afxwin.h"
#include "controls\xtpbutton.h"
#include "lutils.h" // PipeInputListener

class LSUTL32_EXPORT CEnterLicenseDialog : public CDialog//, PipeInputListener
{
	DECLARE_DYNAMIC(CEnterLicenseDialog)

public:
    // Returns if a key has been successfully entered and written.
    static bool ShowItModal(CWnd* pParent = NULL);

	CEnterLicenseDialog(CWnd* pParent = NULL); 
	virtual ~CEnterLicenseDialog();

	enum { IDD = IDD_DLG_ENTER_LICENSE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

    virtual BOOL OnInitDialog();

public:
    afx_msg void OnBnClickedBtnEnterLicense();

    bool HasKeyEntered() { return m_bKeyEntered; }

private:
    CEdit m_edtKey;
    CXTPButton m_btnEnter;
    bool m_bKeyEntered;
};

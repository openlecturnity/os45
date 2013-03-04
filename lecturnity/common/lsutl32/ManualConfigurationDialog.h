#pragma once
#include "resource.h"

class CManualConfiguration;
// CManualConfigurationDialog dialog

class LSUTL32_EXPORT CManualConfigurationDialog : public CDialog {
    DECLARE_DYNAMIC(CManualConfigurationDialog)
private:
// Dialog Data
    enum { IDD = IDD_MANUALCON_FIGURATION };

    // pointer to external instance. This instance will be modified just on OnBnClickedOk() function.
    CManualConfiguration *m_pManualConfiguration;

    CEdit m_ceSlidesNumber;
    CSpinButtonCtrl m_sbSlidesNumber;
    CStatic	m_stSlidesNumber;
    CButton	m_chkHeader;
    CButton	m_chkFooter;
    CButton	m_chkCoverSheet;
    CButton	m_chkTableOfContent;

public:
    /**
    * Creates a dialog that manages manual settings.
    *
    * @param pManualConfiguration: Pointer to a CManualConfiguration instance. 
    *        This instance will be modified just if user clicks OK button.
    */
    CManualConfigurationDialog(CManualConfiguration* pManualConfiguration);   // standard constructor
    virtual ~CManualConfigurationDialog();

    static UINT ShowItModal(CManualConfiguration* pManualConfiguration);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnUpdateEditSlidesNumber();
};

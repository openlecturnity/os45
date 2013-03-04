#pragma once

#include "InteractionButtonDialog.h"
#include "InteractionAreaEx.h"
#include "..\Studio\resource.h"
#include "afxwin.h"
// CIButtonDialog dialog

class CIButtonDialog : public CDialog
{
	DECLARE_DYNAMIC(CIButtonDialog)

public:
	CIButtonDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIButtonDialog();
    void Init(CInteractionAreaEx *pInteractionButton, CRect &rcArea, CEditorProject *pProject);
    void SetUserEntries(CRect &rcArea, CInteractionAreaEx *pInteraction);

// Dialog Data
	enum { IDD = IDD_IBUTTON_CONTAINER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    CInteractionButtonDialog m_dlgInside;
    void InitChildWin(CDialog* pDlg, UINT iWinID);
public:
    virtual BOOL ContinueModal();
public:
    CButton m_btnCancel;
public:
    CButton m_btnOK;
	CStatic m_Divider;
protected:
    CSize m_szInitial;
    bool m_bIsInitialized;
    int m_iButtonVOffset;
    void RepositionControls();
public:
    afx_msg void OnBnClickedOk();
};

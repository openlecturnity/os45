#pragma once

#include "InteractionAreaDialog.h"
#include "InteractionAreaEx.h"
#include "..\Studio\resource.h"
#include "afxwin.h"


// CIAreaDialog dialog

class CIAreaDialog : public CDialog
{
	DECLARE_DYNAMIC(CIAreaDialog)

public:
	CIAreaDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIAreaDialog();
    void Init(CInteractionAreaEx *pInteractionButton, CEditorDoc *pEditorDoc);
    void SetUserEntries(CRect &rcArea, CInteractionAreaEx *pInteraction);

// Dialog Data
	enum { IDD = IDD_IAREA_CONTAINER };

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
public:
    virtual BOOL ContinueModal();
public:
    afx_msg void OnBnClickedOk();
    CInteractionAreaDialog m_dlgInside;
    void InitChildWin(CDialog* pDlg, UINT iWinID);
public:
    CButton m_btnOK;
public:
    CButton m_btnCancel;
	CStatic m_Divider;
protected:
    CSize m_szInitial;
    bool m_bIsInitialized;
    int m_iButtonVOffset;
    void RepositionControls();
};

#pragma once

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
    #define LSUTL32_EXPORT __declspec(dllexport)
#else 
    #ifdef _LSUTL32_STATIC
        #define LSUTL32_EXPORT 
    #else
        #define LSUTL32_EXPORT __declspec(dllimport)
    #endif
#endif

#include "Resource.h"

class LSUTL32_EXPORT CSingleProgressDialog : public CDialog
{
public:
    CSingleProgressDialog(CWnd* pParent, HANDLE hDialogInitEvent);

    //{{AFX_DATA(CSingleProgressDialog)
    enum { IDD = IDD_SINGLE_PROGRESS_DIALOG };
    //}}AFX_DATA

public:
    void SetRange(int lower, int upper);
    void SetLabel(LPCTSTR lpText);
    void SetLabel(UINT nResourceID);
    void SetStep(int step);
    void SetPos(int pos);
    void OffsetPos(int offset);

    //{{AFX_VIRTUAL(CSingleProgressDialog)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CSingleProgressDialog)
    virtual BOOL OnInitDialog();
    afx_msg LRESULT OnCheckEndDialog(WPARAM wParam, LPARAM lParam);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
    enum {
        END_DIALOG = 99
    };

private:
    CStatic	m_progressLabel;
    CProgressCtrl m_progress;
    CString m_labelString;
    HANDLE m_hDialogInitEvent;
};
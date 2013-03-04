#if !defined(AFX_FLASHLOSTSTOPMARKSDLG_H__CB68D2E9_61CD_4AE1_964F_2A268894E9B4__INCLUDED_)
#define AFX_FLASHLOSTSTOPMARKSDLG_H__CB68D2E9_61CD_4AE1_964F_2A268894E9B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlashLostStopmarksDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFlashLostStopmarksDlg 

class CFlashLostStopmarksDlg : public CDialog
{
// Konstruktion
public:
	CFlashLostStopmarksDlg(CArray<CString, CString> *aSlideNumbers, 
      CArray<CString, CString> *aStopmarkNumbers, 
      CArray<CString, CString> *aStopmarkTimes, 
      CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CFlashLostStopmarksDlg)
	enum { IDD = IDD_FLASH_STOPPMARKS_LOST_DLG };
   CStatic	   m_lbFlashStopmarkLoss;
	CListCtrl	m_lstFlashLostStopmarks;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFlashLostStopmarksDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

public:
   // Fill the CListCtrl element with data
   void FillLostStopmarkList(CArray<CString, CString> *aSlideNumbers, 
      CArray<CString, CString> *aStopmarkNumbers, 
      CArray<CString, CString> *aStopmarkTimes);

private:
   CArray<CString, CString> *m_aSlideNumbers;
   CArray<CString, CString> *m_aStopmarkNumbers;
   CArray<CString, CString> *m_aStopmarkTimes;

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CFlashLostStopmarksDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_FLASHLOSTSTOPMARKSDLG_H__CB68D2E9_61CD_4AE1_964F_2A268894E9B4__INCLUDED_

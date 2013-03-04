#if !defined(AFX_UPDATECONFIGURE_H__0D710509_99BB_443D_9848_C917A5CE476E__INCLUDED_)
#define AFX_UPDATECONFIGURE_H__0D710509_99BB_443D_9848_C917A5CE476E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// updateconfigure.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CUpdateConfigure 

class CUpdateConfigure : public CPropertyPage
{
	DECLARE_DYNCREATE(CUpdateConfigure)

// Konstruktion
public:
	CUpdateConfigure();
	~CUpdateConfigure();

// Dialogfelddaten
	//{{AFX_DATA(CUpdateConfigure)
	enum { IDD = IDD_CONFIGURE };
	CComboBox	m_cbbDays;
	CButton	m_cbAnnouncements;
	CButton	m_cbAutoUpdate;
	//}}AFX_DATA

   UINT GetReturnValue() { return m_nReturnValue; }
   // Reads the settings from the current DLL settings,
   // NOT FROM REGISTRY!
   void ReadSettings();

// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CUpdateConfigure)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
   HICON m_hIcon;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CUpdateConfigure)
	afx_msg void OnSearchUpdates();
	afx_msg void OnInstallPatch();
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   UINT m_nReturnValue;
   CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_UPDATECONFIGURE_H__0D710509_99BB_443D_9848_C917A5CE476E__INCLUDED_

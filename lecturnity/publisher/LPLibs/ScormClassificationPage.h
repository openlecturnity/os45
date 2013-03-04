#if !defined(AFX_SCORMCLASSIFICATIONPAGE_H__F7ACA930_9A83_4021_BFC7_9C8864C8F815__INCLUDED_)
#define AFX_SCORMCLASSIFICATIONPAGE_H__F7ACA930_9A83_4021_BFC7_9C8864C8F815__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScormClassificationPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CScormClassificationPage 

class CScormClassificationPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CScormClassificationPage)

// Konstruktion
public:
	CScormClassificationPage();
	~CScormClassificationPage();

// Dialogfelddaten
	//{{AFX_DATA(CScormClassificationPage)
	enum { IDD = IDD_SCORM_CLASSIFICATION };
	CEdit	m_edClsfKeyword;
	CEdit	m_edClsfDescription;
	CComboBox	m_cbbClsfPurpose;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CScormClassificationPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CScormClassificationPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeAnything();
	afx_msg void OnSelectChangeScormPurpose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CString m_csCaption;

   int m_nActualPurposeIndex;
   CScormSettings2 m_scormSettingsTemp;

private:
   void StoreValuesTemporarily(int nActualIndex);
   void RestoreValuesTemporarily(int nActualIndex);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SCORMCLASSIFICATIONPAGE_H__F7ACA930_9A83_4021_BFC7_9C8864C8F815__INCLUDED_

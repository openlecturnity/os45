#if !defined(AFX_AUDIOPROPS_H__0F54E671_AE2B_4294_B1A9_06378468D615__INCLUDED_)
#define AFX_AUDIOPROPS_H__0F54E671_AE2B_4294_B1A9_06378468D615__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AudioProps.h : Header-Datei
//

#include "editorDoc.h"
//class CEditorDoc;

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CAudioProps 

class CAudioProps : public CPropertyPage
{
	DECLARE_DYNCREATE(CAudioProps)

// Konstruktion
public:
	CAudioProps();
	~CAudioProps();

// Dialogfelddaten
	//{{AFX_DATA(CAudioProps)
	enum { IDD = IDD_AUDIO };
	CComboBox	m_sampleSizeCombo;
	CComboBox	m_monoStereoCombo;
	CComboBox	m_sampleRateCombo;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CAudioProps)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CAudioProps)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeAudioSetting();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CEditorDoc *m_pDoc;

   void ReadDefaultSettings();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_AUDIOPROPS_H__0F54E671_AE2B_4294_B1A9_06378468D615__INCLUDED_

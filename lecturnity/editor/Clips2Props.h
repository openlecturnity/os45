#if !defined(AFX_CLIPS2PROPS_H__038BA04B_E792_4687_9CCE_27CB9D9C21C2__INCLUDED_)
#define AFX_CLIPS2PROPS_H__038BA04B_E792_4687_9CCE_27CB9D9C21C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Clips2Props.h : Header-Datei
//

#include "EditorDoc.h"
#include "ClipCodecWnd.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CClips2Props 

class CClips2Props : public CPropertyPage
{
// Konstruktion
public:
	CClips2Props(const _TCHAR *tszClipToDisplay=NULL);   // Standardkonstruktor
   virtual ~CClips2Props();

// Dialogfelddaten
	//{{AFX_DATA(CClips2Props)
	enum { IDD = IDD_CLIPS2 };
	CStatic	m_lbPlaceholder;
	CEdit	m_edClip;
	CComboBox	m_cbClip;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CClips2Props)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CClips2Props)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectClip();
	afx_msg void OnChangeClipTitle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CEditorDoc *m_pDoc;
   CArray<CClipCodecWnd *, CClipCodecWnd *> m_aClipCodecWnds;

   CString m_csClipToDisplay;
   CString m_csCaption;

public:
   void ApplyCodecSettingsForAllClips();
   void ApplyAudioSettingsForAllClips();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CLIPS2PROPS_H__038BA04B_E792_4687_9CCE_27CB9D9C21C2__INCLUDED_

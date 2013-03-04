#if !defined(AFX_TEMPLATEVARIMAGE_H__B8383E44_2979_4874_BFA5_19C85509E48E__INCLUDED_)
#define AFX_TEMPLATEVARIMAGE_H__B8383E44_2979_4874_BFA5_19C85509E48E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateVarImage.h : Header-Datei
//

#include "ImageStatic.h" // MfcUtils
#include "TemplateEngine.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarImage 

class CTemplateVarImage : public CDialog
{
// Konstruktion
public:
	CTemplateVarImage(CTemplateKeyword *pKeyword);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTemplateVarImage)
	enum { IDD = IDD_TEMPLATE_VAR_IMAGE };
	CButton	      m_btnCheckBox;
	CEdit	         m_edVarName;
	CImageStatic	m_imgStatic;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTemplateVarImage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTemplateVarImage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectImage();
	afx_msg void OnRemoveImage();
	afx_msg void OnAdaptImageSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CTemplateImageKeyword *m_pKeyword;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEMPLATEVARIMAGE_H__B8383E44_2979_4874_BFA5_19C85509E48E__INCLUDED_

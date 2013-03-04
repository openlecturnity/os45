#if !defined(AFX_EXPORTSETTINGS_H__4158AF37_A70B_4412_A85C_6E04E19E6D7D__INCLUDED_)
#define AFX_EXPORTSETTINGS_H__4158AF37_A70B_4412_A85C_6E04E19E6D7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// exportsettings.h : Header-Datei
//
#include "videoprops.h"
#include "metadataprops.h"
#include "Clips2Props.h"
#include "DenverCodecPage.h"
#include "audioprops.h"

/////////////////////////////////////////////////////////////////////////////
// CExportSettings

class CExportSettings : public CPropertySheet
{
	DECLARE_DYNAMIC(CExportSettings)

// Konstruktion
public:
	CExportSettings(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, const _TCHAR *tszClipFile=NULL);
	CExportSettings(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, const _TCHAR *tszClipFile=NULL);

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CExportSettings)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CExportSettings();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CExportSettings)
		// HINWEIS - Der Klassen-Assistent fügt hier Member-Funktionen ein und entfernt diese.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   void Init();

   CVideoProps    *m_pVideoProps;
   CMetadataProps *m_pMetadataProps;
   CDenverCodecPage    *m_pClipsProps;
   CAudioProps    *m_pAudioProps;
   CClips2Props   *m_pClips2Props;

   CString m_csClipCodecsToDisplay;
   //CArray<CVideoProps *, CVideoProps *> m_aClipProps;
}; 

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EXPORTSETTINGS_H__4158AF37_A70B_4412_A85C_6E04E19E6D7D__INCLUDED_

// filesdk.h : Haupt-Header-Datei für die DLL FILESDK
//

#if !defined(AFX_FILESDK_H__46E1D88A_140F_4871_B6F4_22822AF62B4C__INCLUDED_)
#define AFX_FILESDK_H__46E1D88A_140F_4871_B6F4_22822AF62B4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole


// Double code from MfcUtils.cpp in lsutl32 (didn't want to move the whole Dialg including resources)
static void Localize(CWnd *pWnd, DWORD *adwIds)
{
   int nIndex = 0;
   DWORD dwId = adwIds[nIndex];
   while (dwId != -1)
   {
      CString csString;
      if (FALSE != csString.LoadString(dwId))
      {
         CWnd *pChild = pWnd->GetDlgItem(dwId);
         if (pChild)
            pWnd->SetDlgItemText(dwId, csString);
      }

      dwId = adwIds[++nIndex];
   }
}


/////////////////////////////////////////////////////////////////////////////
// CFilesdkApp
// Siehe filesdk.cpp für die Implementierung dieser Klasse
//

class CFilesdkApp : public CWinApp
{
public:
	CFilesdkApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CFilesdkApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CFilesdkApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_FILESDK_H__46E1D88A_140F_4871_B6F4_22822AF62B4C__INCLUDED_)

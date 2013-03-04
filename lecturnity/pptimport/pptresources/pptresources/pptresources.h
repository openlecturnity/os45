// pptresources.h : Hauptheaderdatei für die pptresources-DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CpptresourcesApp
// Siehe pptresources.cpp für die Implementierung dieser Klasse
//

class CpptresourcesApp : public CWinApp
{
public:
	CpptresourcesApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
public:
   HRESULT ShowImportWizards(const WCHAR * wszPPTName, int iSlideCount, int iCurrentSlide, WCHAR * wszSlideSelection);
};

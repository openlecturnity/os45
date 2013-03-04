// cdStart_doItDlg.h : Header-Datei
//

#if !defined(AFX_CDSTART_DOITDLG_H__FBF22E16_95E5_4D4D_A012_A002F2D36168__INCLUDED_)
#define AFX_CDSTART_DOITDLG_H__FBF22E16_95E5_4D4D_A012_A002F2D36168__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxctl.h>

/////////////////////////////////////////////////////////////////////////////
// CCdStart_doItDlg Dialogfeld

#include "ImageButton.h"
#include "MCIWnd.h"
class CCdStart_doItDlg : public CDialog
{
// Konstruktion
public:
	CCdStart_doItDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CCdStart_doItDlg)
	enum { IDD = IDD_CDSTART_DOIT_DIALOG };
		// HINWEIS: der Klassenassistent fügt an dieser Stelle Datenelemente (Members) ein
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CCdStart_doItDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	CMCIWnd m_mciWnd;
   bool m_bIntroStopped;
   long m_introEnd;
   CImageButton m_skipButton;
   CMenu m_tutorialPopup;
   bool m_bUse4Buttons;
   int m_iPressedButton;
   
   CPictureHolder m_backgroundPicture;

   //CArray<CImageButton *, CImageButton *> m_buttonList;
   CImageButton m_button[6];

   CMultiBitmapCtrl m_title[4];

   CMultiBitmapCtrl m_explanationText;

   CStatic m_hideRectangle;
   CBitmap m_hideBitmap;

   CString m_szProgramPath;
   UINT m_wDeviceID;

   BOOL InitButtons();

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CCdStart_doItDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSysReq();
	afx_msg void OnGuidedTour();
	afx_msg void OnLecturnityTutorial();
	afx_msg void OnPdfTutorial();
	afx_msg void OnInstall();
	afx_msg void OnCdDemo();
	afx_msg void OnOrder();
	afx_msg void OnCancel();
	afx_msg void OnStopIntro();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   void StopIntro();

   bool CheckAudioReplay(LPCTSTR szAudioFilename);
   bool ReplayAudio(LPCTSTR szAudioFilename);
   void StopAudio();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_CDSTART_DOITDLG_H__FBF22E16_95E5_4D4D_A012_A002F2D36168__INCLUDED_)

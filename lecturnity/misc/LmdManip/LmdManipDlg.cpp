// LmdManipDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "LmdManip.h"
#include "LmdManipDlg.h"
#include "lutils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLmdManipDlg Dialogfeld

CLmdManipDlg::CLmdManipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLmdManipDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLmdManipDlg)
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

   m_pLmdFile = NULL;
   m_pagePopup.LoadMenu(IDR_CONTEXT);
   m_bHasChanged = false;
}

void CLmdManipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLmdManipDlg)
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDC_LMDTREE, m_lmdTree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLmdManipDlg, CDialog)
	//{{AFX_MSG_MAP(CLmdManipDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
   ON_COMMAND(ID_PAGE_REMOVEFROMSTRUCTURE, RemoveFromStructure)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLmdManipDlg Nachrichten-Handler

BOOL CLmdManipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	
	// ZU ERLEDIGEN: Hier zusätzliche Initialisierung einfügen
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CLmdManipDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CLmdManipDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CLmdManipDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

CString FindTitle(SgmlElement *pNode)
{
   SgmlElement *pTitle = pNode->Find(_T("title"));
   if (pTitle)
      return CString(pTitle->GetParameter());
   else
      return CString(_T("<unknown>"));
}

bool CLmdManipDlg::BuildTree(SgmlElement *pRoot)
{
   m_lmdTree.DeleteAllItems();

   SgmlElement *pStru = NULL;
   pStru = pRoot->Find(_T("structure"));
   bool success = (pStru != NULL);

   if (success)
   {
      HTREEITEM hRoot = m_lmdTree.InsertItem(_T("Document structure"), TVI_ROOT);
      HTREEITEM hTemp = hRoot;
      SgmlElement *pTemp = pStru->GetSon();
      bool bInsertAsSon = true;
      while (pTemp)
      {
         HTREEITEM hInsertHere = hTemp;
         if (!bInsertAsSon)
            hInsertHere = m_lmdTree.GetParentItem(hTemp);

         CString csCaption;
         bool bExpand = false;
         if (_tcscmp(pTemp->GetName(), _T("chapter")) == 0)
         {
            csCaption.Format(_T("Chapter: %s"), pTemp->GetValue(_T("title")));
            bExpand = true;
         }
         else if (_tcscmp(pTemp->GetName(), _T("page")) == 0)
         {
            csCaption.Format(_T("Page: %s"), FindTitle(pTemp));
            bExpand = true;
         }
         else if (_tcscmp(pTemp->GetName(), _T("clip")) == 0)
         {
            csCaption.Format(_T("Clip: %s"), FindTitle(pTemp));
            bExpand = true;
         }
         else if (_tcscmp(pTemp->GetName(), _T("title")) == 0)
            csCaption.Format(_T("Title: %s"), pTemp->GetParameter());
         else if (_tcscmp(pTemp->GetName(), _T("nr")) == 0)
            csCaption.Format(_T("Number: %d"), _ttoi(pTemp->GetParameter()));
         else if (_tcscmp(pTemp->GetName(), _T("begin")) == 0)
            csCaption.Format(_T("Begin (ms): %d"), _ttoi(pTemp->GetParameter()));
         else if (_tcscmp(pTemp->GetName(), _T("end")) == 0)
            csCaption.Format(_T("End (ms): %d"), _ttoi(pTemp->GetParameter()));
         else
            csCaption.Format(_T("%s (\"%s\")"), pTemp->GetName(), pTemp->GetParameter());

         hTemp = m_lmdTree.InsertItem(csCaption, hInsertHere, TVI_LAST);
         m_lmdTree.SetItemData(hTemp, (DWORD) pTemp);
         if (bExpand)
            m_lmdTree.EnsureVisible(hTemp);

         if (pTemp->GetSon() != NULL)
         {
            pTemp = pTemp->GetSon();
            bInsertAsSon = true;
         }
         else if (pTemp->GetNext() != NULL)
         {
            pTemp = pTemp->GetNext();
            bInsertAsSon = false;
         }
         else
         {
            while (_tcscmp(pTemp->GetParent()->GetName(), _T("structure")) != 0 && pTemp->GetParent()->GetNext() == NULL)
            {
               pTemp = pTemp->GetParent();
               hTemp = m_lmdTree.GetParentItem(hTemp);
            }

            if (_tcscmp(pTemp->GetParent()->GetName(), _T("structure")) != 0)
            {
               pTemp = pTemp->GetParent()->GetNext();
               hTemp = m_lmdTree.GetParentItem(hTemp);
               bInsertAsSon = false;
            }
            else
               pTemp = NULL;
         }
      }

      m_lmdTree.EnsureVisible(hRoot);
   }

   return success;
}

void CLmdManipDlg::LoadLmdFile(const _TCHAR *tszFile)
{
   if (m_pLmdFile)
      delete m_pLmdFile;
   m_pLmdFile = NULL;

   m_pLmdFile = new SgmlFile();
   bool success = m_pLmdFile->Read(tszFile, "<docinfo");

   SgmlElement *pRoot = NULL;
   if (success)
   {
      pRoot = m_pLmdFile->GetRoot();
      success = (pRoot != NULL);
   }
   if (success)
      success = BuildTree(pRoot);

   if (!success)
   {
      MessageBox(_T("Could not read file."), _T("Error"), MB_OK | MB_ICONERROR);
   }
   else
   {
      m_csLmdFileName = tszFile;
      m_bHasChanged = false;
   }
}

void CLmdManipDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
   HTREEITEM hItem = m_lmdTree.GetSelectedItem();
   if (hItem)
   {
      SgmlElement *pTemp = (SgmlElement *) m_lmdTree.GetItemData(hItem);
      if (_tcscmp(pTemp->GetName(), _T("page")) == 0 ||
          _tcscmp(pTemp->GetName(), _T("clip")) == 0)
      {
         CMenu *pTemp = m_pagePopup.GetSubMenu(0);
         pTemp->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
      }
   }
}

void CLmdManipDlg::RemoveFromStructure()
{
   HTREEITEM hItem = m_lmdTree.GetSelectedItem();
   bool success = (hItem != NULL);

   SgmlElement *pTemp = NULL;
   if (success)
      pTemp = (SgmlElement *) m_lmdTree.GetItemData(hItem);

   if (success)
   {
      if (_tcscmp(pTemp->GetName(), _T("page")) == 0)
      {
         if (pTemp->GetPrev() != NULL)
         {
            SgmlElement *pPrevEnd = pTemp->GetPrev()->Find(_T("end"));
            SgmlElement *pThisEnd = pTemp->Find(_T("end"));
            pPrevEnd->SetParameter(pThisEnd->GetParameter());
         }
         else if (pTemp->GetNext() != NULL)
         {
            // No prev, but has next
            SgmlElement *pThisBegin = pTemp->Find(_T("begin"));
            SgmlElement *pNextBegin = pTemp->GetNext()->Find(_T("begin"));
            pNextBegin->SetParameter(pThisBegin->GetParameter());
         }
      }
      
      if (pTemp->GetPrev() != NULL)
      {
         SgmlElement *pPrev = pTemp->GetPrev();
         pPrev->SetNext(pTemp->GetNext());
      }
      else
      {
         // Prev is NULL
         SgmlElement *pParent = pTemp->GetParent();
         pParent->SetSon(pTemp->GetNext());
         if (pTemp->GetNext() != NULL)
            pTemp->GetNext()->SetParent(pParent);
      }

      if (pTemp->GetNext() != NULL)
      {
         SgmlElement *pNext = pTemp->GetNext();
         pNext->SetPrev(pTemp->GetPrev());
      }

      delete pTemp;
      pTemp = NULL;

      BuildTree(m_pLmdFile->GetRoot());

      m_bHasChanged = true;
   }
}

void CLmdManipDlg::OnOK() 
{
   if (m_bHasChanged)
   {
      int nResult = MessageBox(_T("The document structure has been changed.\n\nDo you want to save the changes?"),
         _T("Confirm"), MB_OKCANCEL | MB_ICONQUESTION);
      if (nResult == IDOK)
      {
         CString csSafetyCopy = m_csLmdFileName;
         csSafetyCopy += _T(".orig");
         LFile lSafetyFile(csSafetyCopy);
         bool success = true;
         if (!lSafetyFile.Exists())
            success = (0 != ::CopyFile(m_csLmdFileName, csSafetyCopy, TRUE));
         else
            success = false;

         if (success)
            success = m_pLmdFile->Write(m_csLmdFileName);

         if (success)
            CDialog::OnOK();
         else
            MessageBox(_T("A problem ocurred either while creating a safety copy of the old LMD file, or while saving the new LMD file.\n\nOperation cancelled."), _T("Error"), MB_OK | MB_ICONERROR);
      }
   }
   else
   {
      CDialog::OnOK();
   }
}

void CLmdManipDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
/*	
   if (m_lmdTree.GetSafeHwnd() != NULL)
   {
      CRect clientRect;
      GetClientRect(&clientRect);
	   CRect treeRect;
      m_lmdTree.GetWindowRect(&treeRect);
      CRect okRect;
      m_btOk.GetWindowRect(&okRect);
      //int btOkWidth = m_btOk.Width();

      treeRect.right = okRect.left - 10;
      treeRect.bottom = clientRect.Height() - 10;

      m_lmdTree.SetWindowPos(NULL, 0, 0, treeRect.Width(), treeRect.Height(), SWP_NOZORDER | SWP_NOMOVE);
   }
   */
}

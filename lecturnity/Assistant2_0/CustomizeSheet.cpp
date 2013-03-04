// CustomizeSheet.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#else
#include "MainFrm.h"   // Assistant
#include "..\Studio\MainFrm.h"   // Studio
#include "..\Studio\RibbonBars.h"   // Studio
#include "..\Studio\Studio.h"
#endif
#include "CustomizeSheet.h"
#include "MfcUtils.h"


// CCustomizeSheet dialog

IMPLEMENT_DYNAMIC(CCustomizeSheet, CXTPPropertyPage)

CCustomizeSheet::CCustomizeSheet()
	: CXTPPropertyPage(CCustomizeSheet::IDD)
{
   m_bSomethingChanged = false;
}

CCustomizeSheet::CCustomizeSheet(CXTPCommandBars * pCommandBars)
	: CXTPPropertyPage(CCustomizeSheet::IDD)
{
   m_pCommandBars = pCommandBars;/*pSheet->GetCommandBars();*/

   m_bShowQuickAccessBelow = FALSE;
   m_bSomethingChanged = false;
}

CCustomizeSheet::~CCustomizeSheet()
{
   for (int i = 0; i < m_arrCategories.GetSize(); i++)
	{
		delete m_arrCategories[i];
	}
}

void CCustomizeSheet::DoDataExchange(CDataExchange* pDX)
{
	CXTPPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_CUST_COMBO_CATEGORIES, m_lstCategories);
   DDX_Control(pDX, IDC_CUST_LIST_QUICK_ACCESS, m_lstQuickAccess);
   DDX_Control(pDX, IDC_CUST_LIST_COMMANDS, m_lstCommands);
	DDX_Check(pDX, IDC_CUST_CHECK_SHOW_BELOW, m_bShowQuickAccessBelow);
}


BEGIN_MESSAGE_MAP(CCustomizeSheet, CXTPPropertyPage)
   ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
   ON_BN_CLICKED(IDC_CUST_CHECK_SHOW_BELOW, OnCheckShowQuickAccessBelow)
	ON_LBN_SELCHANGE(IDC_CUST_COMBO_CATEGORIES, OnCategoriesSelectionChanged)
	ON_LBN_DBLCLK(IDC_CUST_LIST_QUICK_ACCESS, OnDblclkListQuickAccess)
	ON_LBN_DBLCLK(IDC_CUST_LIST_COMMANDS, OnDblclkListCommands)
   ON_BN_CLICKED(IDC_CUST_BUTTON_ADD, OnButtonAdd)
   ON_BN_CLICKED(IDC_CUST_BUTTON_REMOVE, OnButtonRemove)
   ON_BN_CLICKED(IDC_CUST_BUTTON_RESET, OnButtonReset)
	ON_LBN_SELCHANGE(IDC_CUST_LIST_COMMANDS, OnCommandsSelectionChanged)
	ON_LBN_SELCHANGE(IDC_CUST_LIST_QUICK_ACCESS, OnQuickAccessSelectionChanged)
   ON_BN_CLICKED(IDC_BUTTON_UP, &CCustomizeSheet::OnBnClickedButtonCqaUp)
   ON_BN_CLICKED(IDC_BUTTON_DOWN, &CCustomizeSheet::OnBnClickedButtonCqaDown)
END_MESSAGE_MAP()


// CCustomizeSheet message handlers
void CCustomizeSheet::OnKickIdle()
{
   UpdateDialogControls(this, false);
}

CXTPRibbonBar* CCustomizeSheet::GetRibbonBar()
{
	CXTPRibbonBar* pRibbonBar = DYNAMIC_DOWNCAST(CXTPRibbonBar, m_pCommandBars->GetMenuBar());
	ASSERT(pRibbonBar);

	return pRibbonBar;
}

BOOL CCustomizeSheet::OnInitDialog()
{
   CXTPPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here
   DWORD dwIds[] = {IDS_CUST_STATIC_TITLE,
                    IDC_CUST_STATIC_CHOOSE,
                    IDC_CUST_BUTTON_ADD,
                    IDC_CUST_BUTTON_REMOVE,
                    IDC_CUST_BUTTON_RESET,
                    IDC_CUST_CHECK_SHOW_BELOW,
                   -1};

   MfcUtils::Localize(this, dwIds);

   for (int i = 0; i < m_arrCategories.GetSize(); i++)
	{
		int nIndex = m_lstCategories.AddString(m_arrCategories[i]->strCategory);
		m_lstCategories.SetItemData(nIndex, i);

		if (i == 0) m_lstCategories.SetCurSel(0);
	}

	CSize sz = m_pCommandBars->GetPaintManager()->DrawListBoxControl(NULL, NULL, CRect(0, 0, 0, 0), FALSE, FALSE, m_pCommandBars);
	m_lstCommands.SetItemHeight(-1, sz.cy);
	m_lstQuickAccess.SetItemHeight(-1, sz.cy);

	m_lstCommands.m_pCommandBars = m_pCommandBars;
	m_lstQuickAccess.m_pCommandBars = m_pCommandBars;

	OnCategoriesSelectionChanged();
	RefreshQuickAccessList();

	m_bShowQuickAccessBelow = GetRibbonBar()->IsQuickAccessBelowRibbon();
	UpdateData(FALSE);

	if (m_pCommandBars->IsLayoutRTL())
	{
		m_lstCommands.ModifyStyleEx(0, WS_EX_LAYOUTRTL);
		m_lstQuickAccess.ModifyStyleEx(0, WS_EX_LAYOUTRTL);
		m_lstCategories.ModifyStyleEx(0, WS_EX_RIGHT | WS_EX_LEFTSCROLLBAR | WS_EX_RTLREADING);
	}

   m_caption.SubclassDlgItem(IDS_CUST_STATIC_TITLE, this);

   SetResize(IDC_CUST_COMBO_CATEGORIES, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
   SetResize(IDC_CUST_LIST_COMMANDS, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
   SetResize(IDC_CUST_LIST_QUICK_ACCESS, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
   SetResize(IDC_CUST_BUTTON_ADD, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
   SetResize(IDC_CUST_BUTTON_REMOVE, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
   SetResize(IDC_CUST_BUTTON_RESET, SZ_BOTTOM_CENTER, SZ_BOTTOM_CENTER);
   SetResize(IDC_CUST_CHECK_SHOW_BELOW, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
   SetResize(IDS_CUST_STATIC_TITLE, SZ_TOP_LEFT, SZ_TOP_RIGHT);
   SetResize(IDC_BUTTON_UP, SZ_MIDDLE_RIGHT, SZ_MIDDLE_RIGHT);
   SetResize(IDC_BUTTON_DOWN, SZ_MIDDLE_RIGHT, SZ_MIDDLE_RIGHT);

#ifdef _STUDIO
   CStudioApp* pApp = (CStudioApp*)AfxGetApp();
#else
   CAssistantApp* pApp = (CAssistantApp*)AfxGetApp();
#endif
   UINT nColorScheme = pApp->GetCurrentColorScheme();

   m_caption.SetScheme(nColorScheme);
   m_caption.SetTitleAlign(xtpTitleAlignCustom);
   m_caption.SetCaptionType(xtpCaptionHeader);
   m_caption.SetIconID(xtpCustomize);
   m_caption.SetBold();
   
   m_font.CreateFont(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, SYMBOL_CHARSET, 0, 0, 0, 0, _T("Marlett"));
   CWnd *pWnd = GetDlgItem(IDC_BUTTON_UP);
   pWnd->SetFont(&m_font);
   pWnd->SetWindowText(_T("5"));

   pWnd = GetDlgItem(IDC_BUTTON_DOWN);
   pWnd->SetFont(&m_font);
   pWnd->SetWindowText(_T("6"));

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomizeSheet::OnCheckShowQuickAccessBelow()
{
	UpdateData();

	GetRibbonBar()->ShowQuickAccessBelowRibbon(m_bShowQuickAccessBelow);
}

void CCustomizeSheet::RefreshQuickAccessList()
{
   bool bIsStartupMode=false;
	m_lstQuickAccess.SetRedraw(FALSE);
	int nTopIndex = m_lstQuickAccess.GetTopIndex();
	m_lstQuickAccess.ResetContent();

	CXTPControls* pControls = GetRibbonBar()->GetQuickAccessControls();
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
   if (pMainFrame)
   {
     bIsStartupMode = pMainFrame->IsInStartupMode();
   }
	for (int i = 0; i < pControls->GetCount(); i++)
   {
      CXTPControl* pControl = pControls->GetAt(i);
      pControl->SetIconSize(CSize(16,16));

      m_lstQuickAccess.SendMessage(LB_INSERTSTRING, m_lstQuickAccess.GetCount(), (LPARAM)pControl);
      
   }
   m_lstQuickAccess.SetTopIndex(nTopIndex);
   m_lstQuickAccess.SetRedraw(TRUE);

   OnQuickAccessSelectionChanged();
   OnCommandsSelectionChanged();
}

void CCustomizeSheet::OnCategoriesSelectionChanged()
{
   bool bIsStartupMode=false;
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
   if (pMainFrame)
   {
     bIsStartupMode = pMainFrame->IsInStartupMode();
   }
	m_lstCommands.ResetContent();

	int nIndex = m_lstCategories.GetCurSel();

	if (nIndex == LB_ERR)
		return;

	XTP_COMMANDBARS_CATEGORYINFO* pInfo = GetCategoryInfo((int)m_lstCategories.GetItemData(nIndex));

	if (pInfo == NULL)
		return;

	for (int i = 0; i < pInfo->pControls->GetCount(); i++)
	{
		CXTPControl* pControl = pInfo->pControls->GetAt(i);
		m_lstCommands.SendMessage(LB_INSERTSTRING, m_lstCommands.GetCount(), (LPARAM)pControl);
	}
	OnCommandsSelectionChanged();
}

XTP_COMMANDBARS_CATEGORYINFO* CCustomizeSheet::FindCategory(LPCTSTR strCategory) const
{
	for (int i = 0; i < m_arrCategories.GetSize(); i++)
	{
		if (m_arrCategories[i]->strCategory.Compare(strCategory) == 0)
			return m_arrCategories[i];
	}
	return NULL;
}

XTP_COMMANDBARS_CATEGORYINFO* CCustomizeSheet::GetCategoryInfo(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_arrCategories.GetSize())
		return m_arrCategories[nIndex];
	return NULL;
}
bool CCustomizeSheet::IsAssistantMenuID(UINT nID)
{
   UINT uiAssistantMenuEntries[] = {ID_MENU_OPEN, ID_SUBMENU_STUDIO_OPEN,ID_LSD_SAVE, 
      ID_LSD_SAVE_AS, ID_MENU_REVERT, ID_FILE_PACK,
      ID_MENU_IMPORT, ID_MENU_PROJECT_SETTINGS,ID_EDIT_UNDO,
      ID_EDIT_DELETE,ID_EDIT_SELECT_ALL,ID_DELETE_PAGE_CHAPTER,ID_PAGE_DUPLICATE,
      ID_HELP,ID_SHOW_IMC,IDS_SHOW_IMC_WEBPAGE_TUTORIALS,
      IDS_SHOW_IMC_WEBPAGE_GUIDED_TOUR,IDS_SHOW_IMC_WEBPAGE_COMUNITY,
      IDS_SHOW_IMC_WEBPAGE_BUYNOW,ID_APP_ABOUT,ID_LSD_OPEN,ID_LRD_OPEN};
   for(int j = 0; j < _countof(uiAssistantMenuEntries); ++j)
   {
      if (nID == uiAssistantMenuEntries[j]) 
      {
         return true;
      }
   }
   return false;
}

bool CCustomizeSheet::IsEditorMenuID(UINT nID)
{
   UINT uiEditorMenuEntries[] = {ID_FILE_OPEN, ID_FILE_SAVE, 
      ID_FILE_SAVE_AS, ID_FILE_IMPORT, ID_FILE_EXPORT_SETTINGS,
      ID_MENU_AUDIO, ID_MENU_VIDEO, ID_CLOSE,IDC_UNDO,IDC_REDO,
      IDC_EDIT_DELETE,IDC_MARK_SETMARK_LEFT,IDC_MARK_SETMARK_RIGHT,
      IDC_MARK_ALL,IDC_MARK_NONE,
      ID_HELP,ID_SHOW_IMC,IDS_SHOW_IMC_WEBPAGE_TUTORIALS,
      IDS_SHOW_IMC_WEBPAGE_GUIDED_TOUR,IDS_SHOW_IMC_WEBPAGE_COMUNITY,
      IDS_SHOW_IMC_WEBPAGE_BUYNOW,ID_APP_ABOUT,ID_LRD_OPEN};
   for(int j = 0; j < _countof(uiEditorMenuEntries); ++j)
   {
      if (nID == uiEditorMenuEntries[j]) 
      {
         return true;
      }
   }
   return false;
}

BOOL CCustomizeSheet::AddCategory(LPCTSTR strCategory, CMenu* pMenu, BOOL bListSubItems)
{
   CXTPControlPopup* pControl = NULL;
   CXTPControls* pCategoryControls = InsertCategory(strCategory);
   int nCount = pMenu->GetMenuItemCount();
   UINT nCurrentMode = ((CStudioApp *)AfxGetApp())->GetCurrentMode();
   for (int i = 0; i < nCount; i++)
   {
      if (pMenu->GetMenuItemID(i) > 0)
      {
         if((IsEditorMenuID(pMenu->GetMenuItemID(i)) == true ) && (nCurrentMode == CStudioApp::MODE_EDITOR))
         {
            pControl = DYNAMIC_DOWNCAST(CXTPControlPopup, pCategoryControls->AddMenuItem(pMenu, i));
            if(pMenu->GetMenuItemID(i)==ID_MENU_AUDIO)
            {
               CMenu mnSubmenu;
               if (!XTPResourceManager()->LoadMenu(&mnSubmenu, ID_SUBMENU_AUDIO))
                  return FALSE;
               CMenu* pSubMenu = mnSubmenu.GetSubMenu(0);
               int nCount = pSubMenu->GetMenuItemCount();
               if (pSubMenu)
               {
                  for (int j = 0; j < nCount; j++)
                  {
                     CString strCategory;
                     pControl = DYNAMIC_DOWNCAST(CXTPControlPopup, pCategoryControls->AddMenuItem(pSubMenu, j));
                  }
               }
            }
            else  if(pMenu->GetMenuItemID(i)==ID_MENU_VIDEO)
            {
               CMenu mnSubmenu;
               if (!XTPResourceManager()->LoadMenu(&mnSubmenu, ID_SUBMENU_VIDEO))
                  return FALSE;
               CMenu* pSubMenu = mnSubmenu.GetSubMenu(0);
               int nCount = pSubMenu->GetMenuItemCount();
               if (pSubMenu)
               {
                  for (int j = 0; j < nCount; j++)
                  {
                     CString strCategory;
                     pControl = DYNAMIC_DOWNCAST(CXTPControlPopup, pCategoryControls->AddMenuItem(pSubMenu, j));
                  }
               }
            }
            /*else  if(pMenu->GetMenuItemID(i)==ID_MENU_PROJECT_SETTINGS)
            {
               CMenu mnSubmenu;
               if (!XTPResourceManager()->LoadMenu(&mnSubmenu, ID_SUBMENU_PROJECT_SETTINGS))
                  return FALSE;
               CMenu* pSubMenu = mnSubmenu.GetSubMenu(0);
               int nCount = pSubMenu->GetMenuItemCount();
               if (pSubMenu)
               {
                  for (int j = 0; j < nCount; j++)
                  {
                     CString strCategory;
                     pControl = DYNAMIC_DOWNCAST(CXTPControlPopup, pCategoryControls->AddMenuItem(pSubMenu, j));
                  }
               }
            }*/
            if (pControl && bListSubItems)
            {
               CXTPControls* pControls = pControl->GetCommandBar()->GetControls();
               for (int j = 0; j < pControls->GetCount(); j++)
               {
                  if(pControls->GetAt(j)->IsVisible()==TRUE)
                  {
                     pCategoryControls->AddClone(pControls->GetAt(j));
                  }
               }
            }
         }
         else if((IsAssistantMenuID(pMenu->GetMenuItemID(i)) == true ) && (nCurrentMode == CStudioApp::MODE_ASSISTANT))
         {
            pControl = DYNAMIC_DOWNCAST(CXTPControlPopup, pCategoryControls->AddMenuItem(pMenu, i));
            if(pMenu->GetMenuItemID(i)==ID_MENU_OPEN)
            {
               CMenu mnSubmenu;
               if (!XTPResourceManager()->LoadMenu(&mnSubmenu, ID_SUBMENU_STUDIO_OPEN))
                  return FALSE;
               CMenu* pSubMenu = mnSubmenu.GetSubMenu(0);
               int nCount = pSubMenu->GetMenuItemCount();
               if (pSubMenu)
               {
                  for (int j = 0; j < nCount; j++)
                  {
                     CString strCategory;
                     pControl = DYNAMIC_DOWNCAST(CXTPControlPopup, pCategoryControls->AddMenuItem(pSubMenu, j));
                  }
               }

            }
            else  if(pMenu->GetMenuItemID(i)==ID_MENU_IMPORT)
            {
               CMenu mnSubmenu;
               if (!XTPResourceManager()->LoadMenu(&mnSubmenu, ID_SUBMENU_IMPORT))
                  return FALSE;
               CMenu* pSubMenu = mnSubmenu.GetSubMenu(0);
               int nCount = pSubMenu->GetMenuItemCount();
               if (pSubMenu)
               {
                  for (int j = 0; j < nCount; j++)
                  {
                     CString strCategory;
                     pControl = DYNAMIC_DOWNCAST(CXTPControlPopup, pCategoryControls->AddMenuItem(pSubMenu, j));
                  }
               }
            }
            else  if(pMenu->GetMenuItemID(i)==ID_MENU_PROJECT_SETTINGS)
            {
               CMenu mnSubmenu;
               if (!XTPResourceManager()->LoadMenu(&mnSubmenu, ID_SUBMENU_PROJECT_SETTINGS))
                  return FALSE;
               CMenu* pSubMenu = mnSubmenu.GetSubMenu(0);
               int nCount = pSubMenu->GetMenuItemCount();
               if (pSubMenu)
               {
                  for (int j = 0; j < nCount; j++)
                  {
                     CString strCategory;
                     pControl = DYNAMIC_DOWNCAST(CXTPControlPopup, pCategoryControls->AddMenuItem(pSubMenu, j));
                  }
               }
            }
            if (pControl && bListSubItems)
            {
               CXTPControls* pControls = pControl->GetCommandBar()->GetControls();
               for (int j = 0; j < pControls->GetCount(); j++)
               {
                  if(pControls->GetAt(j)->IsVisible()==TRUE)
                  {
                     pCategoryControls->AddClone(pControls->GetAt(j));
                  }
               }
            }
         }
      }
   }

   return TRUE;
}

BOOL CCustomizeSheet::AddCategories(UINT nIDResource, BOOL bListSubItems)
{
	CMenu menu;
   if (!XTPResourceManager()->LoadMenu(&menu, nIDResource))
      return FALSE;

   int nCount = menu.GetMenuItemCount();

   for (int i = 0; i < nCount; i++)
   {
      CString strCategory;
      if((i<2)||(i>4))
      {
         if (menu.GetMenuString(i, strCategory, MF_BYPOSITION) > 0)
         {
            CMenu* pMenu = menu.GetSubMenu(i);
            if (pMenu)
            {
               CXTPPaintManager::StripMnemonics(strCategory);

               if (!AddCategory(strCategory, pMenu, bListSubItems))
                  return FALSE;
            }
         }
      }
   }

   return TRUE;
}
BOOL CCustomizeSheet::AddTab(int iTabIndex)
{
   CXTPRibbonTab *pTab;
   CXTPRibbonGroup * pGroup;
   int iNrGroups=0;
   CWnd *pMainFrame = AfxGetMainWnd();
   if (pMainFrame && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
   {
      CXTPRibbonBar* pRibbonBar =((CMainFrame *)pMainFrame)->GetRibbonBars()->GetRibbonbar();
      if(pRibbonBar==NULL)
      {
         return FALSE;
      }
      else
      {
         pRibbonBar->GetControls();
         pTab = GetRibbonBar()->GetTab(iTabIndex);
         CString strCategory = pTab->GetCaption();
         CXTPControls* pCategoryControls = InsertCategory(strCategory);
         iNrGroups = pTab->GetGroups()->GetCount();
         for (int i = 0; i < iNrGroups; i++)
         {

            pGroup = pTab->GetGroups()->GetAt(i);
            CString strCategory = pGroup->GetCaption();
            if(pGroup!=NULL)
            {
               CXTPPaintManager::StripMnemonics(strCategory);
               if (!AddControlsFromGroup(pCategoryControls, pGroup, TRUE))
                  return FALSE;
            }
         }
      }
   }
   return TRUE;
}
BOOL CCustomizeSheet::AddControlsFromGroup(CXTPControls * pCategoryControls, CXTPRibbonGroup *pGroup, BOOL bListSubItems)
{
   CXTPControls* pControls =NULL;
   CXTPControl* pControl = NULL;
   int nCount = pGroup->GetCount();
   for (int i = 0; i < nCount; i++)
   {
      if (pGroup->GetAt(i)->GetID() > 0)
      {
         CXTPControl *pControlMother = pGroup->GetAt(i);

         pControlMother->SetIconSize(CSize(16,16));
         if((pControlMother->GetID()!= ID_FONTFAMILY)&&
            (pControlMother->GetID()!= ID_FONTSIZE)&&
            (pControlMother->GetID()!= IDC_FONT_FAMILY)&&
            (pControlMother->GetID()!= IDC_FONT_SIZE)&&
            (pControlMother->GetID()!= IDS_GRID_SPACING))
         {
            
            pControl =  pCategoryControls->AddClone(pControlMother);

            if((pControl->GetStyle()==xtpButtonIconAndCaptionBelow)||
               (pControl->GetStyle()==xtpButtonIconAndCaption))
            {
               pControl->SetStyle(xtpButtonIcon);
               pControl->SetIconSize(CSize(16,16));
            }
            if(pControlMother->GetID()==ID_GRID_SPACING)
            {
               CString csCaption;
               csCaption.LoadStringW(IDS_GRID_SPACING);
               csCaption.Replace(_T(":"),_T(""));
               pControl->SetCaption(csCaption);
            }
         }
         if(pControlMother->GetID()==ID_MEDIA_LRD_BUTTON)
         {
            CMenu mnSubmenu;
            if (!XTPResourceManager()->LoadMenu(&mnSubmenu, ID_MEDIA_LRD_MENU))
               return FALSE;
            CMenu* pSubMenu = mnSubmenu.GetSubMenu(0);
            int nCount = pSubMenu->GetMenuItemCount();
            if (pSubMenu)
            {
               for (int j = 0; j < nCount; j++)
               {
                  CString strCategory;
                  pControl = DYNAMIC_DOWNCAST(CXTPControlPopup, pCategoryControls->AddMenuItem(pSubMenu, j));
               }
            }
         }
         if(pControlMother->GetID()==ID_MEDIA_CLIP_BUTTON)
         {
            CMenu mnSubmenu;
            if (!XTPResourceManager()->LoadMenu(&mnSubmenu, ID_MEDIA_CLIP_MENU))
               return FALSE;
            CMenu* pSubMenu = mnSubmenu.GetSubMenu(0);
            int nCount = pSubMenu->GetMenuItemCount();
            if (pSubMenu)
            {
               for (int j = 0; j < nCount; j++)
               {
                  CString strCategory;
                  pControl = DYNAMIC_DOWNCAST(CXTPControlPopup, pCategoryControls->AddMenuItem(pSubMenu, j));
               }
            }
         }
      }
   }
   return TRUE;
}

BOOL CCustomizeSheet::AddCategories(CXTPControls* pControls)
{
	for (int i = 0; i < pControls->GetCount(); i++)
	{
		CXTPControl* pControl = pControls->GetAt(i);
		CString strCategory = pControl->GetCategory();

		if (!strCategory.IsEmpty())
		{
			CXTPControls* pCategoryControls = InsertCategory(strCategory);
			pCategoryControls->AddClone(pControl);
		}
	}
	return TRUE;
}


CXTPControls* CCustomizeSheet::InsertCategory(LPCTSTR strCategory, int nIndex)
{
	XTP_COMMANDBARS_CATEGORYINFO* pInfo = FindCategory(strCategory);
	if (!pInfo)
	{
		//pInfo = new XTP_COMMANDBARS_CATEGORYINFO(strCategory, m_pSheet->GetCommandBars());
      pInfo = new XTP_COMMANDBARS_CATEGORYINFO(strCategory, m_pCommandBars);
		m_arrCategories.InsertAt(nIndex == -1 ? m_arrCategories.GetSize() : nIndex, pInfo);
	}
	return pInfo->pControls;
}

CXTPControls* CCustomizeSheet::GetControls(LPCTSTR strCategory)
{
	XTP_COMMANDBARS_CATEGORYINFO* pInfo = FindCategory(strCategory);
	return pInfo ? pInfo->pControls : NULL;
}

void CCustomizeSheet::OnDblclkListQuickAccess()
{
	OnButtonRemove();

}

void CCustomizeSheet::OnDblclkListCommands()
{
	OnButtonAdd();

}

void CCustomizeSheet::OnButtonAdd()
{
   if (!GetDlgItem(IDC_CUST_BUTTON_ADD)->IsWindowEnabled())
		return;

	if (m_lstCommands.GetCurSel() == LB_ERR)
		return;

	CXTPControl* pControl = (CXTPControl*)m_lstCommands.GetItemDataPtr(m_lstCommands.GetCurSel());
	if (!pControl)
		return;

   CXTPControl * pControlClone = GetRibbonBar()->GetQuickAccessControls()->AddClone(pControl);
   pControlClone->SetHideFlag(xtpHideRibbonTab,FALSE);
   pControlClone->SetHideFlag(xtpHideGeneric,FALSE);

   GetRibbonBar()->OnRecalcLayout();
   
   RefreshQuickAccessList();
   GetRibbonBar()->Redraw();
   //ReOnRecalcLayout();
	//m_lstQuickAccess.SetCurSel(m_lstQuickAccess.GetCount() - 1);
 //  OnBnClickedButtonCqaUp();
 //  m_lstQuickAccess.SetCurSel(m_lstQuickAccess.GetCount() - 1);
 //  OnBnClickedButtonCqaUp();
   m_lstQuickAccess.SetCurSel(m_lstQuickAccess.GetCount() - 1);
   m_bSomethingChanged = true;
}

void CCustomizeSheet::OnButtonRemove()
{
	if (m_lstQuickAccess.GetCurSel() == LB_ERR)
		return;

	CXTPControl* pControl = (CXTPControl*)m_lstQuickAccess.GetItemDataPtr(m_lstQuickAccess.GetCurSel());
	if (!pControl)
		return;

	GetRibbonBar()->GetQuickAccessControls()->Remove(pControl);
	GetRibbonBar()->OnRecalcLayout();

	RefreshQuickAccessList();
   m_bSomethingChanged = true;
}

void CCustomizeSheet::OnButtonReset()
{
	CString strPrompt;
	//CXTPResourceManager::AssertValid(XTPResourceManager()->LoadString(&strPrompt, IDS_CUST_CONFIRM_RESET));
   strPrompt.LoadString(IDS_CUST_CONFIRM_RESET);

	if (/*m_pSheet->*/AfxMessageBox(strPrompt, MB_ICONWARNING | MB_YESNO) != IDYES)
		return;

	GetRibbonBar()->GetQuickAccessControls()->Reset();
	RefreshQuickAccessList();
}

void CCustomizeSheet::OnCommandsSelectionChanged()
{
	BOOL bEnabled = m_lstCommands.GetCurSel() != LB_ERR;

	if (bEnabled)
	{
		CXTPControl* pControl = (CXTPControl*)m_lstCommands.GetItemDataPtr(m_lstCommands.GetCurSel());
      CXTPControls* pControls = GetRibbonBar()->GetQuickAccessControls();
      for (int i = 0; i < pControls->GetCount(); i++)
      {
         CXTPControl* pQuickAccessControl = pControls->GetAt(i);
         if (pQuickAccessControl->GetID() == pControl->GetID() /*&& pQuickAccessControl->IsVisible(xtpHideWrap)*/ &&
            pQuickAccessControl->GetType() == pControl->GetType() && pQuickAccessControl->GetRuntimeClass() == pControl->GetRuntimeClass())
         {
            bEnabled = FALSE; 
           // return pQuickAccessControl;
         }
      }

		//bEnabled = GetRibbonBar()->IsAllowQuickAccessControl(pControl);
	}

   GetDlgItem(IDC_CUST_BUTTON_ADD)->EnableWindow(bEnabled);
}

void CCustomizeSheet::OnQuickAccessSelectionChanged()
{
   GetDlgItem(IDC_CUST_BUTTON_REMOVE)->EnableWindow(m_lstQuickAccess.GetCurSel() != LB_ERR);

}

void CCustomizeSheet::OnOK()
{
   // TODO: Add your specialized code here and/or call the base class
   //GetRibbonBar()->GetQuickAccessControls()->SetOriginalControls((CXTPOriginalControls *)GetRibbonBar()->GetQuickAccessControls());
   GetRibbonBar()->GetQuickAccessControls()->CreateOriginalControls();
   CWnd *pMainFrame = AfxGetMainWnd();
   if (pMainFrame && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
   {
      CRibbonBars* pRibbonBars =((CMainFrame *)pMainFrame)->GetRibbonBars();
      if(pRibbonBars!=NULL)
      {
         UINT nCurrentMode = ((CStudioApp *)AfxGetApp())->GetCurrentMode();
         if(nCurrentMode == CStudioApp::MODE_ASSISTANT)
         {
            pRibbonBars->SaveQuickAccessControlsAssistant();
            CXTPRibbonTab*pRibbonTab = pRibbonBars->GetRibbonbar()->GetSelectedTab();
            pRibbonBars->GetTabStartAssistant()->Select();
            pRibbonBars->GetTabEditAssistant()->Select();
            pRibbonBars->GetTabViewAssistant()->Select();
            pRibbonTab->Select();
         } 
         else if(nCurrentMode == CStudioApp::MODE_EDITOR)
         {
            pRibbonBars->SaveQuickAccessControlsEditor();
            CXTPRibbonTab*pRibbonTab = pRibbonBars->GetRibbonbar()->GetSelectedTab();
            pRibbonBars->GetTabViewEditor()->Select();
            pRibbonTab->Select();
         }
      }
   }
   CXTPPropertyPage::OnOK();
}

void CCustomizeSheet::OnCancel()
{
   if(m_bSomethingChanged == true)
      GetRibbonBar()->GetQuickAccessControls()->Reset();
   CXTPPropertyPage::OnCancel();
}

void CCustomizeSheet::OnBnClickedButtonCqaUp()
{
   // TODO: Add your control notification handler code here
   if (m_lstQuickAccess.GetCurSel() == LB_ERR)
		return;

   int iCurrSel = m_lstQuickAccess.GetCurSel();
   if(iCurrSel > 0)
   {
     //  CXTPControls* pCtrls = GetRibbonBar()->GetQuickAccessControls();//;->RemoveAll();
      CXTPControl* pControl = (CXTPControl*)m_lstQuickAccess.GetItemDataPtr(m_lstQuickAccess.GetCurSel());
      if (!pControl)
         return;
     //

     // CArray<CXTPControl*, CXTPControl*>aControls;
     // for(int i = 0; i < m_lstQuickAccess.GetCount(); i++)
     // {
     //    if(i == iCurrSel - 1)
     //    {
     //       aControls.Add(pControl);
     //    }
     //    else if(i == iCurrSel)
     //    {
     //       CXTPControl* pCtrl = (CXTPControl*)m_lstQuickAccess.GetItemDataPtr(i - 1);
     //       aControls.Add(pCtrl);
     //    }
     //    else
     //    {
     //       CXTPControl* pCtrl = (CXTPControl*)m_lstQuickAccess.GetItemDataPtr(i);
     //       aControls.Add(pCtrl);
     //    }
     // }
     //
     ///* aControls.RemoveAt(iCurrSel);
     // aControls.InsertAt(iCurrSel - 1, pControl);*/

     //
     // int n = pCtrls->GetCount();
     // for(int i = 0; i < aControls.GetCount(); i++)
     //    GetRibbonBar()->GetQuickAccessControls()->AddClone(aControls.GetAt(i));//, aControls.GetAt(i)->GetID());
     // 
     // for(int i = 0; i < pCtrls->GetCount(); i++)
     // {
     //    CXTPControl* pC = pCtrls->GetAt(i);
     //    if(pCtrls->GetAt(i)->IsVisible())
     //       GetRibbonBar()->GetQuickAccessControls()->Remove(pCtrls->GetAt(i));
     // }



     /* int index = pControl->GetIndex();
      CXTPControl* c1 = GetRibbonBar()->GetQuickAccessControls()->GetAt(index);
      CXTPControl* c2 = GetRibbonBar()->GetQuickAccessControls()->GetAt(index -1);*/
      //int i = GetRibbonBar()->GetQuickAccessControls()->IndexOf(pControl);
      GetRibbonBar()->GetQuickAccessControls()->AddClone(pControl, iCurrSel -1);
      GetRibbonBar()->GetQuickAccessControls()->Remove(pControl);
      //GetRibbonBar()->GetQuickAccessControls()->GE MoveBefore(pMy, i-1);
      ////GetRibbonBar()->GetQuickAccessControls()->GetAt(pControl->GetIndex() - , iCurrSel -1);
      GetRibbonBar()->OnRecalcLayout();

      RefreshQuickAccessList();
      m_bSomethingChanged = true;
      m_lstQuickAccess.SetCurSel(iCurrSel - 1 );
   }
}

void CCustomizeSheet::OnBnClickedButtonCqaDown()
{
   // TODO: Add your control notification handler code here
   if (m_lstQuickAccess.GetCurSel() == LB_ERR)
		return;

   int iCurrSel = m_lstQuickAccess.GetCurSel();
   int iCount = m_lstQuickAccess.GetCount();
   if(iCurrSel != m_lstQuickAccess.GetCount() - 1)
   {
      CXTPControl* pControl = (CXTPControl*)m_lstQuickAccess.GetItemDataPtr(m_lstQuickAccess.GetCurSel());
      if (!pControl)
         return;

      //int index = pControl->GetIndex();
      if(iCurrSel <= iCount - 2)
         GetRibbonBar()->GetQuickAccessControls()->AddClone(pControl, iCurrSel + 2);
      else
         GetRibbonBar()->GetQuickAccessControls()->AddClone(pControl);
      GetRibbonBar()->GetQuickAccessControls()->Remove(pControl);
      GetRibbonBar()->OnRecalcLayout();

      RefreshQuickAccessList();
      m_bSomethingChanged = true;
      m_lstQuickAccess.SetCurSel(iCurrSel + 1);
   }
}

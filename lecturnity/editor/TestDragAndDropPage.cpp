// TestDragAndDropPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "TestDragAndDropPage.h"
#include "MainFrm.h"
#include "MfcUtils.h"
#include "WhiteboardView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CTestDragAndDropPage 

#include "lutils.h"
#include "misc.h"
#include "MoveableObjects.h"

IMPLEMENT_DYNCREATE(CTestDragAndDropPage, CPropertyPage)


CString CTestDragAndDropPage::m_csDefaultDirectory = _T("");

CTestDragAndDropPage::CTestDragAndDropPage() : CTestAnswerPage(CTestDragAndDropPage::IDD)
{
   //{{AFX_DATA_INIT(CTestDragAndDropPage)
   //}}AFX_DATA_INIT
   
   m_csCaption.LoadString(CTestDragAndDropPage::IDD);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;

   if (m_csDefaultDirectory.IsEmpty())
   {
      _TCHAR tszDefaultDirectory[MAX_PATH];
      unsigned long lLength = MAX_PATH;
      bool bRet = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
                                                     _T("Software\\imc AG\\LECTURNITY\\Editor\\Dirs"), _T("DD"), 
                                                      tszDefaultDirectory, &lLength);
      if (bRet)
         m_csDefaultDirectory = tszDefaultDirectory;
      else
         Misc::GetImageHome(m_csDefaultDirectory);
   }

   m_bItemWasMoved = false;
}

CTestDragAndDropPage::~CTestDragAndDropPage()
{
}

void CTestDragAndDropPage::DoDataExchange(CDataExchange* pDX)
{
   CTestAnswerPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTestDragAndDropPage)
   DDX_Control(pDX, IDC_LIST_ANSWER_DD, m_answerListDD);
   DDX_Control(pDX, IDC_LIST_TARGET_DD, m_targetList);
   DDX_Control(pDX, IDC_TARGET_SELECTION, m_AnswerComboBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestDragAndDropPage, CTestAnswerPage)
//{{AFX_MSG_MAP(CTestDragAndDropPage)
   ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
   ON_BN_CLICKED(IDC_TARGET_LIST_ADD, OnTargetListAdd)
   ON_BN_CLICKED(IDC_TARGET_LIST_RENAME, OnTargetListRename)
   ON_BN_CLICKED(IDC_TARGET_LIST_DELETE, OnTargetListDelete)
   ON_BN_CLICKED(IDC_TARGET_LIST_UP, OnTargetListUp)
   ON_BN_CLICKED(IDC_TARGET_LIST_DOWN, OnTargetListDown)
   ON_BN_CLICKED(IDC_ANSWER_LIST_ADD, OnAnswerListAdd)
   ON_BN_CLICKED(IDC_ANSWER_LIST_RENAME, OnAnswerListRename)
   ON_BN_CLICKED(IDC_ANSWER_LIST_DELETE, OnAnswerListDelete)
   ON_BN_CLICKED(IDC_ANSWER_LIST_UP, OnAnswerListUp)
   ON_BN_CLICKED(IDC_ANSWER_LIST_DOWN, OnAnswerListDown)
   ON_UPDATE_COMMAND_UI(IDC_TARGET_LIST_DELETE, OnUpdateTargetListDelete)
   ON_UPDATE_COMMAND_UI(IDC_TARGET_LIST_UP, OnUpdateTargetListUp)
   ON_UPDATE_COMMAND_UI(IDC_TARGET_LIST_DOWN, OnUpdateTargetListDown)
   ON_UPDATE_COMMAND_UI(IDC_TARGET_LIST_RENAME, OnUpdateTargetListRename)
   ON_UPDATE_COMMAND_UI(IDC_ANSWER_LIST_DELETE, OnUpdateAnswerListDelete)
   ON_UPDATE_COMMAND_UI(IDC_ANSWER_LIST_UP, OnUpdateAnswerListUp)
   ON_UPDATE_COMMAND_UI(IDC_ANSWER_LIST_DOWN, OnUpdateAnswerListDown)
   ON_UPDATE_COMMAND_UI(IDC_ANSWER_LIST_RENAME, OnUpdateAnswerListRename)
   ON_UPDATE_COMMAND_UI(IDC_TARGET_SELECTION, OnUpdateAnswerComboBox)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST_ANSWER_DD, OnDblclkListAnswerDd)
   ON_CBN_SELCHANGE(IDC_TARGET_SELECTION, OnSelchangeAnswerCombobox)
   ON_CBN_SELCHANGE(IDC_ELEMENT_ALIGNMENT, OnSelchangeAlignmentCombobox)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ANSWER_DD, OnItemchangedListAnswerDd)
   ON_WM_VSCROLL()		 
   ON_MESSAGE_VOID(CListCtrlEx::WM_EDIT_FINISHED, OnFinishAnswerRename)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTestDragAndDropPage 

BOOL CTestDragAndDropPage::OnInitDialog() 
{
	CTestAnswerPage::OnInitDialog();
   
   DWORD dwIds[] = {IDC_GROUP_SOURCE,
                    IDC_ANSWER_LIST_ADD,
                    IDC_ANSWER_LIST_RENAME,
                    IDC_ANSWER_LIST_DELETE,
                    IDC_ANSWER_LIST_UP,
                    IDC_ANSWER_LIST_DOWN,
                    IDC_GROUP_TARGET,
                    IDC_TARGET_LIST_ADD,
                    IDC_TARGET_LIST_RENAME,
                    IDC_TARGET_LIST_DELETE,
                    IDC_TARGET_LIST_UP,
                    IDC_TARGET_LIST_DOWN,
                    IDC_GROUP_ALIGNMENT,
                    -1};
   MfcUtils::Localize(this, dwIds);

	m_AnswerListSelectedItemIndex = -1;

	m_ThumbnailRect.SetRect(0, 0, 64, 64);
	m_answerListDD.ModifyStyle(LVS_AUTOARRANGE, LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER);

	m_answerListDD.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
   m_answerListDD.SetIconSpacing(0, 50);
	m_imageList.Create(m_ThumbnailRect.Width(),m_ThumbnailRect.Height(),ILC_COLOR32,0,1024);
	m_answerListDD.SetImageList(&m_imageList, LVSIL_STATE);

   // set column width
	CRect rcAnswerList;
	m_answerListDD.GetClientRect(&rcAnswerList);
	LV_COLUMN lvcAnswer;
	lvcAnswer.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVIF_PARAM;
	lvcAnswer.iSubItem = 0;
	lvcAnswer.pszText = _T("");
	lvcAnswer.cx = rcAnswerList.Width();
	lvcAnswer.fmt = LVCFMT_LEFT;
	m_answerListDD.InsertColumn(0,&lvcAnswer);
	
   for (int i = 0; i < m_aImageData.GetSize(); ++i)
   {
	   LV_ITEM lvi;
	   lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	   lvi.iItem = i;
	   lvi.iSubItem = 0;
	   lvi.iImage = i;
	   lvi.stateMask = 0;
	   lvi.state = 0;
      lvi.lParam = (long)m_aImageData[i];
	   m_answerListDD.InsertItem(&lvi);
      m_answerListDD.AddImage(m_aImageData[i]->m_csImageName);
   }

   // this item is needed becaus clicking on the first empty line
   // adds a new item
	LV_ITEM lviAnswerEmpty;
	lviAnswerEmpty.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lviAnswerEmpty.iItem = i;
	lviAnswerEmpty.iSubItem = 0;
	lviAnswerEmpty.iImage = -1;
	lviAnswerEmpty.stateMask = 0;
	lviAnswerEmpty.state = 0;
   lviAnswerEmpty.lParam = NULL;
	m_answerListDD.InsertItem(&lviAnswerEmpty);

	// m_targetList
	m_targetList.ModifyStyle(0, LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER);
	m_targetList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

   // set column width
   CRect rcTargetList;
	m_targetList.GetClientRect(&rcTargetList);
	LV_COLUMN lvcTarget;
	lvcTarget.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVIF_PARAM;
	lvcTarget.iSubItem = 0;
	lvcTarget.pszText = _T("");
	lvcTarget.cx = rcTargetList.Width();
	lvcTarget.fmt = LVCFMT_LEFT;
	m_targetList.InsertColumn(0,&lvcTarget);
   
   for (i = 0; i < m_aTargetData.GetSize(); ++i)
   {
      LV_ITEM lvi;
      lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
      lvi.iItem = i;
      lvi.iSubItem = 0;
      lvi.pszText = _T("");
      lvi.iImage = 0;
      lvi.stateMask = 0;
      lvi.state = 0;
      lvi.lParam = (long)m_aTargetData[i];
      m_targetList.InsertItem(&lvi);

      m_targetList.SetItemText(i, 0, m_aTargetData[i]->m_csTargetName);
   }

   // this item is needed becaus clicking on the first empty line
   // adds a new item
	LV_ITEM lviTargetEmpty;
	lviTargetEmpty.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lviTargetEmpty.iItem = i;
	lviTargetEmpty.iSubItem = 0;
	lviTargetEmpty.pszText = _T("");
	lviTargetEmpty.iImage = 0;
	lviTargetEmpty.stateMask = 0;
	lviTargetEmpty.state = 0;
   lviTargetEmpty.lParam = 0;
	m_targetList.InsertItem(&lviTargetEmpty);

   CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_ELEMENT_ALIGNMENT);
   CString csEntry;
   csEntry.LoadString(IDS_LABEL_HORIZONTAL);
   pCombo->AddString(csEntry);
   csEntry.LoadString(IDS_LABEL_VERTICAL);
   pCombo->AddString(csEntry);
   pCombo->SetCurSel(m_iAlignment);

   m_aImageData.RemoveAll();
   m_aTargetData.RemoveAll();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTestDragAndDropPage::OnAnswerListAdd() 
{
   AnswerListAddItem();
}

void CTestDragAndDropPage::OnAnswerListRename() 
{
   if (m_answerListDD.GetSelectedItem() != m_answerListDD.GetItemCount()-1)	
      AnswerListEditItem();
}

void CTestDragAndDropPage::OnAnswerListDelete() 
{
   
   if(m_answerListDD.GetSelectedItem() != -1 &&
      m_answerListDD.GetSelectedItem() < m_answerListDD.GetItemCount()-1)
   {
      int iSelectedItem = m_answerListDD.GetSelectedItem();
      
      LV_ITEM lvi;
      lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
      lvi.iItem = iSelectedItem;
      lvi.iSubItem = 0;
      m_answerListDD.GetItem(&lvi);
      CSourceContainer *pData = (CSourceContainer *)lvi.lParam;
      if (pData != NULL)
         delete pData;

      m_answerListDD.DeleteItem(iSelectedItem);
   }
}

void CTestDragAndDropPage::OnAnswerListUp() 
{
   m_answerListDD.MoveSelectedUp();
   m_bItemWasMoved = true;
}

void CTestDragAndDropPage::OnAnswerListDown() 
{
   m_answerListDD.MoveSelectedDown();
   m_bItemWasMoved = true;
}

void CTestDragAndDropPage::OnTargetListAdd() 
{
   UINT nItemId = m_targetList.GetItemCount()-1;

   CTargetContainer *pDataContainer = new CTargetContainer;
   m_targetList.InsertNewItem(nItemId, pDataContainer);
}


void CTestDragAndDropPage::OnTargetListRename() 
{
   int iSelectedItem = m_targetList.GetSelectedItem();

   m_aRenameSources.RemoveAll();
   if (iSelectedItem != -1)
   {
      _TCHAR szBuff[MAX_PATH];
      LV_ITEM lviTarget;
      lviTarget.mask = LVIF_TEXT | LVIF_PARAM;
      lviTarget.iItem = iSelectedItem;
      lviTarget.iSubItem = 0;
      lviTarget.pszText = szBuff;
      lviTarget.cchTextMax = sizeof(szBuff);
      m_targetList.GetItem(&lviTarget);

      CString csTarget = lviTarget.pszText;
      CTargetContainer *pTargetData = (CTargetContainer *)lviTarget.lParam;

      if (pTargetData != NULL)
      {
         pTargetData->m_csTargetName = csTarget;

         // search for source (image) with the same target area
         for (int i = 0; i < m_answerListDD.GetItemCount() - 1; ++i)
         {
            LV_ITEM lviSource;
            lviSource.mask = LVIF_IMAGE | LVIF_PARAM;
            lviSource.iItem = i;
            lviSource.iSubItem = 0;
            m_answerListDD.GetItem(&lviSource);

            CSourceContainer *pImageData = (CSourceContainer *)lviSource.lParam;
            if (pImageData != NULL)
            {
               if (pImageData->m_pTargetArea != NULL)
               {
                  if (pImageData->m_pTargetArea == pTargetData->m_pTargetArea)
                  {
                     m_aRenameSources.Add(i);
                     break;
                  }
               }
               else
               {
                  if (pImageData->m_csTargetName == pTargetData->m_csTargetName)
                  {
                     m_aRenameSources.Add(i);
                     break;
                  }
               }
            }
         }
      }

   }


   m_targetList.EditItem(iSelectedItem);
}

void CTestDragAndDropPage::OnTargetListDelete() 
{
   m_targetList.EndEditItem();
   int iSelectedItem = m_targetList.GetSelectedItem();

   if (iSelectedItem != -1)
   {
      _TCHAR szBuff[MAX_PATH];
      LV_ITEM lviTarget;
      lviTarget.mask = LVIF_TEXT | LVIF_PARAM;
      lviTarget.iItem = iSelectedItem;
      lviTarget.iSubItem = 0;
      lviTarget.pszText = szBuff;
      lviTarget.cchTextMax = sizeof(szBuff);
      m_targetList.GetItem(&lviTarget);

      CString csTarget = lviTarget.pszText;
      CTargetContainer *pTargetData = (CTargetContainer *)lviTarget.lParam;

      // search for images which are linked with this target area
      for (int i = 0; i < m_answerListDD.GetItemCount() - 1; ++i)
      {
         LV_ITEM lviSource;
         lviSource.mask = LVIF_IMAGE | LVIF_PARAM;
         lviSource.iItem = i;
         lviSource.iSubItem = 0;
         m_answerListDD.GetItem(&lviSource);

         CSourceContainer *pImageData = (CSourceContainer *)lviSource.lParam;
         if (pImageData != NULL)
         {
            if (pImageData->m_pTargetArea != NULL)
            {
               if (pImageData->m_pTargetArea == pTargetData->m_pTargetArea)
               {
                  pImageData->m_csTargetName.Empty();
                  pImageData->m_pTargetArea = NULL;
                  break;
               }
            }
            else
            {
               if (pImageData->m_csTargetName == pTargetData->m_csTargetName)
               {
                  pImageData->m_csTargetName.Empty();
                  pImageData->m_pTargetArea = NULL;
                  break;
               }
            }
         }
      }
      if (pTargetData != NULL)
         delete pTargetData;
      pTargetData = NULL;

      m_targetList.DeleteItem(iSelectedItem);
   }


   PopulateAnswerComboBox();
}

void CTestDragAndDropPage::OnTargetListUp() 
{
   m_targetList.MoveSelectedUp();

   PopulateAnswerComboBox();

   m_bItemWasMoved = true;
   
}

void CTestDragAndDropPage::OnTargetListDown() 
{
   m_targetList.MoveSelectedDown();

   PopulateAnswerComboBox();

   m_bItemWasMoved = true;
}

void CTestDragAndDropPage::OnUpdateTargetListRename(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if(m_targetList.IsItemSelected())
   {
      int iSelectedItem = m_targetList.GetSelectedItem();
      if (iSelectedItem >= 0 && 
          iSelectedItem < m_targetList.GetItemCount() - 1)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

void CTestDragAndDropPage::OnUpdateTargetListDelete(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if(m_targetList.IsItemSelected())
   {
      int iSelectedItem = m_targetList.GetSelectedItem();
      if (iSelectedItem >= 0 && 
          iSelectedItem < m_targetList.GetItemCount() - 1)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

void CTestDragAndDropPage::OnUpdateTargetListUp(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (m_targetList.IsItemSelected())
   {
      int iSelectedItem = m_targetList.GetSelectedItem();
      if (iSelectedItem > 0 && 
          iSelectedItem < m_targetList.GetItemCount() - 1)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
   
}

void CTestDragAndDropPage::OnUpdateTargetListDown(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (m_targetList.IsItemSelected())
   {
      int iSelectedItem = m_targetList.GetSelectedItem();
      // item on (GetItemCount() - 1) is empty
      if (iSelectedItem >= 0 && 
          iSelectedItem < m_targetList.GetItemCount() - 2)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
   
}

void CTestDragAndDropPage::OnUpdateAnswerListRename(CCmdUI *pCmdUI)
{
   if((m_answerListDD.GetSelectedItem()==m_answerListDD.GetItemCount()-1)||(m_answerListDD.GetSelectedItem()==-1)) 
      pCmdUI->Enable(false);
   else
      pCmdUI->Enable(true);
   
}

void CTestDragAndDropPage::OnUpdateAnswerListDelete(CCmdUI *pCmdUI)
{
   if((m_answerListDD.GetSelectedItem() == m_answerListDD.GetItemCount()-1) ||
      (m_answerListDD.GetSelectedItem() == -1)) 
      pCmdUI->Enable(false);
   else
      pCmdUI->Enable(true);
   
}

void CTestDragAndDropPage::OnUpdateAnswerListUp(CCmdUI *pCmdUI)
{
   if((m_answerListDD.GetSelectedItem()==m_answerListDD.GetItemCount()-1)||
      (m_answerListDD.GetSelectedItem()==-1)||
      (m_answerListDD.GetSelectedItem()==0)) 
      pCmdUI->Enable(false);
   else
      pCmdUI->Enable(true);
   
}

void CTestDragAndDropPage::OnUpdateAnswerListDown(CCmdUI *pCmdUI)
{
   
   if((m_answerListDD.GetSelectedItem()>=m_answerListDD.GetItemCount()-2)||(m_answerListDD.GetSelectedItem()==-1)) 
      pCmdUI->Enable(false);
   else
      pCmdUI->Enable(true);
   
   
}

//---------------------------------------------------------------------------------------------------------------------
void CTestDragAndDropPage::OnUpdateAnswerComboBox(CCmdUI *pCmdUI)
{
   if(m_answerListDD.GetItemCount() == 0 ||
      m_answerListDD.GetSelectedItem() == -1 ||
      m_answerListDD.GetSelectedItem() >= m_answerListDD.GetItemCount() - 1) 
   {
      m_AnswerComboBox.SetCurSel(-1);
      m_AnswerListSelectedItemIndex = m_answerListDD.GetSelectedItem();
      pCmdUI->Enable(false);		
   }
   else
   {
      if(m_AnswerListSelectedItemIndex != m_answerListDD.GetSelectedItem())
      {
         LV_ITEM lvi;
         lvi.mask = LVIF_IMAGE | LVIF_PARAM;
         lvi.iItem = m_answerListDD.GetSelectedItem();
         lvi.iSubItem = 0;
         m_answerListDD.GetItem(&lvi);
         
         CSourceContainer *pData = (CSourceContainer *)lvi.lParam;
         int iFoundIndex  = m_AnswerComboBox.FindStringExact(-1, pData->m_csTargetName);
         if(CB_ERR!=iFoundIndex)
            m_AnswerComboBox.SetCurSel(iFoundIndex);
         else
            m_AnswerComboBox.SetCurSel(-1); 

         m_AnswerListSelectedItemIndex = m_answerListDD.GetSelectedItem();
      }
      
      pCmdUI->Enable(true);
   }
}
//---------------------------------------------------------------------------------------------------------------------
void CTestDragAndDropPage::OnKickIdle()
{
   UpdateDialogControls(this, FALSE);
}

void CTestDragAndDropPage::OnFinishAnswerRename()
{
   int nTargetItem = m_targetList.GetSelectedItem();
   
   if (nTargetItem != -1 && nTargetItem < m_targetList.GetItemCount() - 1)
   {
      
      _TCHAR szBuff[MAX_PATH];
      LV_ITEM lviTarget;
      lviTarget.mask = LVIF_TEXT | LVIF_PARAM;
      lviTarget.iItem = nTargetItem;
      lviTarget.iSubItem = 0;
      lviTarget.pszText = szBuff;
      lviTarget.cchTextMax = sizeof(szBuff);
      m_targetList.GetItem(&lviTarget);
      
      CString csTarget = lviTarget.pszText;
      
      CTargetContainer *pTargetData = (CTargetContainer *)lviTarget.lParam;

      // insert item with double click
      if (pTargetData == NULL)
      {
         pTargetData = new CTargetContainer();
         lviTarget.lParam = (long)pTargetData;
         m_targetList.SetItem(&lviTarget);
      }

      if (pTargetData != NULL)
      {
         pTargetData->m_csTargetName = csTarget;
         
         for (int i = 0; i < m_aRenameSources.GetSize(); ++i)
         {
            
            LV_ITEM lviSource;
            lviSource.mask = LVIF_PARAM;
            lviSource.iItem = m_aRenameSources[i];
            lviSource.iSubItem = 0;
            m_answerListDD.GetItem(&lviSource);
            
            CSourceContainer *pImageData = (CSourceContainer *)lviSource.lParam;
            if (pImageData)
               pImageData->m_csTargetName = csTarget;
            
         }
      }
   }

   PopulateAnswerComboBox();
}
//---------------------------------------------------------------------------------------------------------------------
//LRESULT CTestDragAndDropPage::OnIdleUpdateUIMessage(WPARAM, LPARAM)
//{
//   UpdateDialogControls( this, TRUE );
//   return 0;
//}

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

void CTestDragAndDropPage::AnswerListAddItem()
{
   CString csFilter;
   csFilter.LoadString(IDS_IMAGE_FILTER_E);

	CFileDialog fileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, csFilter, this);

   fileDialog.m_ofn.lpstrInitialDir = m_csDefaultDirectory;;
	if (IDOK == fileDialog.DoModal())
	{
      CString csFilename = fileDialog.GetPathName();
      if (!csFilename.IsEmpty())
      {
         m_csDefaultDirectory = csFilename;
         
         StringManipulation::GetPath(m_csDefaultDirectory);
         LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
                                             _T("Software\\imc AG\\LECTURNITY\\Editor\\Dirs"), _T("DD"), 
                                             m_csDefaultDirectory);

         int iImageIndex = m_answerListDD.AddImage(csFilename);
         if (iImageIndex != -1)
         {
				LV_ITEM lvi1;
				lvi1.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
				lvi1.iItem = m_answerListDD.GetItemCount()-1;
				lvi1.iSubItem = 0;
				lvi1.iImage = iImageIndex;
				lvi1.stateMask = LVIS_SELECTED;
				lvi1.state = LVIS_SELECTED;		
            CSourceContainer *pData = new CSourceContainer();
            pData->m_csImageName = csFilename;

            lvi1.lParam = (long)pData;

				m_answerListDD.InsertItem(&lvi1);
            m_answerListDD.EnsureVisible(lvi1.iItem, FALSE);
			}

		}
	}
		
}


void CTestDragAndDropPage::OnDblclkListAnswerDd(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_answerListDD.GetSelectedItem() == m_answerListDD.GetItemCount() - 1)
   {
      // add new item
      AnswerListAddItem();
   }
   else
   {
      // edit selected item
      AnswerListEditItem();
   }
   *pResult = 0;
}

void CTestDragAndDropPage::AnswerListEditItem()
{
   // TODO: edit crt selected item
   CString csFilter;
   csFilter.LoadString(IDS_IMAGE_FILTER_E);

   CFileDialog *pFileDialog = new CFileDialog(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, csFilter, this);
   if (IDOK == pFileDialog->DoModal())
   {
      CString csFilename = pFileDialog->GetPathName();
      if (!csFilename.IsEmpty())
      {
         int iImageIndex = m_answerListDD.AddImage(csFilename);
   
         LV_ITEM lvi;
         lvi.mask = LVIF_IMAGE | LVIF_PARAM;
         lvi.iItem = m_answerListDD.GetSelectedItem();
         lvi.iSubItem = 0;
         m_answerListDD.GetItem(&lvi);

         CSourceContainer *pData = (CSourceContainer *)lvi.lParam;
         if (pData != NULL)
            pData->m_csImageName = csFilename;

         lvi.iImage = iImageIndex;

         m_answerListDD.SetItem(&lvi);
         m_answerListDD.RedrawItems(m_answerListDD.GetSelectedItem(),m_answerListDD.GetSelectedItem());
       }
   }
   
   if (pFileDialog)
      delete pFileDialog;
}

void CTestDragAndDropPage::PopulateAnswerComboBox()
{
   m_targetList.EndEditItem();

   m_AnswerComboBox.ResetContent();

   static _TCHAR szBuff[MAX_PATH];
   LV_ITEM lvi;
   for (int i = 0; i < m_targetList.GetItemCount()-1; ++i)
   {
      lvi.mask = LVIF_TEXT | LVIF_PARAM;
      lvi.iItem = i;
      lvi.iSubItem = 0;
      lvi.pszText = szBuff;
      lvi.cchTextMax = sizeof(szBuff);
      m_targetList.GetItem(&lvi);

      CTargetContainer *pTargetData = (CTargetContainer *)lvi.lParam;

      int iIndex = m_AnswerComboBox.AddString(szBuff);
      m_AnswerComboBox.SetItemDataPtr(iIndex, pTargetData);
   }
}

void CTestDragAndDropPage::OnSelchangeAnswerCombobox() 
{
   if((m_answerListDD.GetSelectedItem() != m_answerListDD.GetItemCount()-1) &&
      (m_answerListDD.GetSelectedItem() != -1)) 
   {
      int iFoundIndex  = m_AnswerComboBox.GetCurSel();
      if(CB_ERR != iFoundIndex)
      {
         CString csSelItem;
         m_AnswerComboBox.GetLBText(iFoundIndex, csSelItem);
         
         LV_ITEM lvi;
         lvi.mask = LVIF_IMAGE | LVIF_PARAM;
         lvi.iItem = m_answerListDD.GetSelectedItem();
         lvi.iSubItem = 0;
         m_answerListDD.GetItem(&lvi);

         CSourceContainer *pSourceData = (CSourceContainer *)lvi.lParam;
         pSourceData->m_csTargetName = csSelItem;
         CTargetContainer *pTargetData = (CTargetContainer *)m_AnswerComboBox.GetItemDataPtr(iFoundIndex);
         pSourceData->m_pTargetArea = pTargetData->m_pTargetArea;
      }
   }
}

void CTestDragAndDropPage::OnSelchangeAlignmentCombobox() 
{
   m_bAlignmentChanged = true;
}

void CTestDragAndDropPage::OnItemchangedListAnswerDd(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   PopulateAnswerComboBox();
   *pResult = 0;
}

void CTestDragAndDropPage::OnOK() 
{
   m_targetList.EndEditItem();

   m_aImageData.RemoveAll();
   for (int i = 0; i < m_answerListDD.GetItemCount() - 1; ++i)
   {
      LV_ITEM lvi;
      lvi.mask = LVIF_IMAGE | LVIF_PARAM;
      lvi.iItem = i;
      lvi.iSubItem = 0;
      lvi.lParam = 0;

      m_answerListDD.GetItem(&lvi);

      int iImageIndex = lvi.iImage;
   
      CSourceContainer *pData = (CSourceContainer *)(lvi.lParam);
      if (pData != NULL)
      {
         if (iImageIndex >= 0)
         {
            CString csImageFilename;
            m_answerListDD.GetImageName(iImageIndex, csImageFilename);
            pData->m_csImageName = csImageFilename;
         }

         m_aImageData.Add(pData);
      }
   }  
   
   m_aTargetData.RemoveAll();
   UINT nBelongsTo = 1;
   bool bAllIndexSet = true;
   for (i = 0; i < m_targetList.GetItemCount() - 1; ++i)
   {
      _TCHAR szBuff[MAX_PATH];
      LV_ITEM lvi;
      lvi.mask = LVIF_TEXT | LVIF_PARAM;
      lvi.iItem = i;
      lvi.iSubItem = 0;
      lvi.pszText = szBuff;
      lvi.cchTextMax = sizeof(szBuff);
      lvi.lParam = 0;

      m_targetList.GetItem(&lvi);

      CTargetContainer *pData = (CTargetContainer *)lvi.lParam;

      bool bTargetFound = false;

      if (pData != NULL)
      {
         for (int j = 0; j < m_aImageData.GetSize(); ++j)
         {
            if (pData->m_pTargetArea != NULL &&
                pData->m_pTargetArea == m_aImageData[j]->m_pTargetArea)
            {
               m_aImageData[j]->m_iBelongsTo = nBelongsTo;
               pData->m_iBelongsTo = nBelongsTo;
               ++nBelongsTo;
               bTargetFound = true;
               break;
            }
         }
         
         if (!bTargetFound)
         {
            CString csTarget = lvi.pszText;
            for (int j = 0; j < m_aImageData.GetSize(); ++j)
            {
               if (pData->m_csTargetName == m_aImageData[j]->m_csTargetName)
               {
                  m_aImageData[j]->m_iBelongsTo = nBelongsTo;
                  pData->m_iBelongsTo = nBelongsTo;
                  ++nBelongsTo;
                  bTargetFound = true;
                  break;
               }
            }
         }

         m_aTargetData.Add(pData);
      }

      if (!bTargetFound)
         bAllIndexSet = false;

   }  

   CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_ELEMENT_ALIGNMENT);
   m_iAlignment = pCombo->GetCurSel();

	CTestAnswerPage::OnOK();
}

void CTestDragAndDropPage::OnCancel() 
{
   m_targetList.EndEditItem();

   m_aImageData.RemoveAll();
   for (int i = 0; i < m_answerListDD.GetItemCount() - 1; ++i)
   {
      LV_ITEM lvi;
      lvi.mask = LVIF_IMAGE | LVIF_PARAM;
      lvi.iItem = i;
      lvi.iSubItem = 0;
      lvi.lParam = 0;

      m_answerListDD.GetItem(&lvi);

      int iImageIndex = lvi.iImage;
   
      CSourceContainer *pData = (CSourceContainer *)(lvi.lParam);
      if (pData != NULL)
         delete pData;
   }  
   
   m_aTargetData.RemoveAll();
   for (i = 0; i < m_targetList.GetItemCount() - 1; ++i)
   {
      _TCHAR szBuff[MAX_PATH];
      LV_ITEM lvi;
      lvi.mask = LVIF_TEXT | LVIF_PARAM;
      lvi.iItem = i;
      lvi.iSubItem = 0;
      lvi.pszText = szBuff;
      lvi.cchTextMax = sizeof(szBuff);
      lvi.lParam = 0;

      m_targetList.GetItem(&lvi);

      CTargetContainer *pData = (CTargetContainer *)lvi.lParam;

      if (pData != NULL)
         delete pData;
   }  

	CTestAnswerPage::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// public functions

void CTestDragAndDropPage::Init(CQuestionEx *pQuestion, CEditorProject *pProject)
{
   CTestAnswerPage::Init(pQuestion, pProject);

   m_iAlignment = ALIGNMENT_HORIZONTAL;
   m_bAlignmentChanged = false;

   if (pQuestion)
   {
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
      pQuestion->GetObjectPointers(&aObjects);
      
      // search for images
      CSourceContainer *pSourceContainer = NULL;
      for (int i = 0; i < aObjects.GetSize(); ++i)
      {
         CInteractionAreaEx *pInteraction = aObjects[i];

         if (pInteraction != NULL)
         {
            if (pInteraction->GetClassId() == INTERACTION_CLASS_MOVE)
            {
               CMoveableObjects *pMoveableObject = (CMoveableObjects *)pInteraction;
               CArray<DrawSdk::DrawObject *,DrawSdk::DrawObject *> *paObjects = pMoveableObject->GetObjects();
               int iBelongsKey = pMoveableObject->GetBelongsKey();
               for (int j = 0; j < paObjects->GetSize(); ++j)
               {
                  DrawSdk::DrawObject *pObject = paObjects->GetAt(j);

                  if (pObject != NULL && pObject->GetType() == DrawSdk::IMAGE)
                  {
                     DrawSdk::Image *pImage = (DrawSdk::Image *)pObject;
                     if (pImage)
                     {  
                        pSourceContainer = new CSourceContainer();
                        pSourceContainer->m_csImageName = pImage->GetImageName();
                        pSourceContainer->m_pImage = pImage;
                        pSourceContainer->m_bUseHashKey = true;
                        pSourceContainer->m_nHashKey = pMoveableObject->GetHashKey();
                        pSourceContainer->m_iBelongsTo = iBelongsKey;

                        m_aImageData.Add(pSourceContainer);
                        pSourceContainer = NULL;
                     }
                  }
               }
            }
         }
      }

      CTargetContainer *pTargetContainer = NULL;
      // search for target areas and text
      for (i = 0; i < aObjects.GetSize(); ++i)
      {
         CInteractionAreaEx *pInteraction = aObjects[i];

         if (pInteraction != NULL)
         {
            if (pInteraction->GetClassId() == INTERACTION_CLASS_MOVE)
            {
               CMoveableObjects *pMoveableObject = (CMoveableObjects *)pInteraction;
               CArray<DrawSdk::DrawObject *,DrawSdk::DrawObject *> *paObjects = pMoveableObject->GetObjects();
               int iBelongsKey = pMoveableObject->GetBelongsKey();
               for (int j = 0; j < paObjects->GetSize(); ++j)
               {
                  DrawSdk::DrawObject *pObject = paObjects->GetAt(j);

                  if (pObject != NULL && pObject->GetType() == DrawSdk::TARGET)
                  {
                     DrawSdk::TargetArea *pTarget = (DrawSdk::TargetArea *)pObject;
                     if (pTarget)
                     {  
                        pTargetContainer = new CTargetContainer();
                        pTargetContainer->m_pTargetArea = pTarget;
                        pTargetContainer->m_csTargetName = pTarget->GetName();
                        pTargetContainer->m_bUseTargetHashKey = true;
                        pTargetContainer->m_nTargetHashKey = pMoveableObject->GetHashKey();
                        pTargetContainer->m_iBelongsTo = iBelongsKey;
                        m_aTargetData.Add(pTargetContainer);

                     }
                  }
                  if (pObject != NULL && pObject->GetType() == DrawSdk::TEXT)
                  {
                     DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
                     if (pTargetContainer != NULL && pText != NULL)
                     {  
                        pTargetContainer->m_pText = pText;
                        pTargetContainer->m_bUseTextHashKey = true;
                        pTargetContainer->m_nTextHashKey = pMoveableObject->GetHashKey();
                        pTargetContainer = NULL;
                     }
                  }
               }
            }
         }
      }

      for (i = 0; i < m_aImageData.GetSize(); ++i)
      {
         for (int j = 0; j < m_aTargetData.GetSize(); ++j)
         { 
            if (m_aImageData[i]->m_iBelongsTo == m_aTargetData[j]->m_iBelongsTo)
            {
               m_aImageData[i]->m_csTargetName = m_aTargetData[j]->m_csTargetName;
               m_aImageData[i]->m_pTargetArea = m_aTargetData[j]->m_pTargetArea;
               break;
            }
         }

      }

      DndAlignmentId dndAlignment = pQuestion->GetDndAlignment();
      m_iAlignment = 
         (dndAlignment == QUESTION_DND_ALIGNMENT_VERTICAL) ? ALIGNMENT_VERTICAL : ALIGNMENT_HORIZONTAL;
   }
}

void CTestDragAndDropPage::FillWithObjects(CQuestionEx *pQuestion, CTimePeriod &visibiltyPeriod, 
                                           CTimePeriod &activityPeriod, double dUpperBound)
{
   int iOffset = 5;

   double dScaleX, dScaleY;
   CQuestionEx::GetQuestionScale(m_pEditorProject, dScaleX, dScaleY);

   double dScaledYOffset = (double)OBJECT_BORDER_Y_TOP_OFFSET * dScaleY;

   double dObjectBorderYTop = 
      dUpperBound > dScaledYOffset ? dUpperBound : dScaledYOffset;

   double dObjectBorderYBottom = (double)OBJECT_BORDER_Y_BOTTOM_OFFSET * dScaleY;

   double dBorderXOffset = (double)OBJECT_BORDER_X_OFFSET * dScaleX;

   double dSpacingX = (double)SPACING * dScaleX;
   double dSpacingY = (double)SPACING * dScaleY;

   CRect rcView;
   CWhiteboardView *pWbView = CMainFrameE::GetCurrentInstance()->GetWhiteboardView();
   if (pWbView != NULL)
       pWbView->GetClientRect(&rcView);
   
   double dMaxWidth = 0;
   double dMaxHeight = 0;
   
   CSize siPage;
   m_pEditorProject->GetPageDimension(siPage);

   if (m_aImageData.GetSize() > 0)
   {
      int nImageWidth = (siPage.cx - 2*dBorderXOffset) / m_aImageData.GetSize();
      int nImageHeight = (siPage.cy - (int)dObjectBorderYTop - dObjectBorderYBottom - dSpacingY) / 2;

      if (m_iAlignment != ALIGNMENT_HORIZONTAL)
      {
         nImageWidth = (siPage.cx - 2*dBorderXOffset - dSpacingX) / 2;
         nImageHeight = (siPage.cy - (int)dObjectBorderYTop - dObjectBorderYBottom) / 
                         m_aImageData.GetSize();
      }

      nImageWidth -= 2*(iOffset+2);
      nImageHeight -= 2*(iOffset+2);

      for (int i = 0; i < m_aImageData.GetSize(); ++i)
      {
         CString csImageFilename = m_aImageData[i]->m_csImageName;
         if (!csImageFilename.IsEmpty())
         {
            int iStringLength = csImageFilename.GetLength();
            WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR)); // +1 for null termination
#ifdef _UNICODE
            wcscpy(wcString, csImageFilename);
#else
            MultiByteToWideChar(CP_ACP, 0, csImageFilename, iStringLength+1, 
               wcString, iStringLength+1);
#endif
            
            Gdiplus::Image *pImage = Gdiplus::Image::FromFile(wcString, FALSE);
            
            if (wcString)
               free(wcString);
            
            double dWidth = pImage->GetWidth();
            double dHeight = pImage->GetHeight();
            
            
            if (dWidth > nImageWidth) 
            {
               double dScale = nImageWidth / dWidth;
               dWidth = dWidth * dScale;
               dHeight = dHeight * dScale;
            }
            
            if (dHeight > nImageHeight) 
            {
               double dScale = nImageHeight / dHeight;
               dWidth = dWidth * dScale;
               dHeight = dHeight * dScale;
            }
            
            if (dWidth > dMaxWidth)
               dMaxWidth = dWidth;
            
            if (dHeight > dMaxHeight)
               dMaxHeight = dHeight;

            delete pImage;
         }
      }  
   }

   if (m_aTargetData.GetSize() > 0)
   {
      int nRectangleWidth = (siPage.cx - 2*dBorderXOffset) / m_aTargetData.GetSize();
      int nRectangleHeight = (siPage.cy - (int)(dObjectBorderYTop + dObjectBorderYBottom + dSpacingY)) / 2;
      
      if (m_iAlignment != ALIGNMENT_HORIZONTAL)
      {
         nRectangleWidth = (siPage.cx - 2*dBorderXOffset - dSpacingX) / 2;
         nRectangleHeight = (siPage.cy - (int)dObjectBorderYTop - dObjectBorderYBottom) / 
                             m_aTargetData.GetSize();
      }

      int nRectangleXPos = 0;
      int nRectangleYPos = 0;
      if (m_iAlignment == ALIGNMENT_HORIZONTAL)
      {
         nRectangleXPos = dBorderXOffset;
         nRectangleYPos = (int)(siPage.cy - dObjectBorderYBottom - dMaxHeight);
         if (nRectangleYPos < dObjectBorderYTop + dMaxHeight)
            nRectangleYPos = (int)(dObjectBorderYTop + dMaxHeight);
      }
      else
      {
         nRectangleXPos = (int)(siPage.cx - dObjectBorderYBottom - dMaxWidth);
         if (nRectangleXPos < dBorderXOffset + dMaxWidth)
            nRectangleXPos = (int)(dBorderXOffset + dMaxWidth);
         nRectangleYPos = (int)dObjectBorderYTop;
      }
      
      LOGFONT lf;
      LFont::FillWithDefaultValues(&lf);
      for (int i = 0; i < m_aTargetData.GetSize(); ++i)
      {
         double dX = (double)nRectangleXPos;
         double dY = (double)nRectangleYPos;
         double dWidth = 0;
         double dHeight = 0;
         
         bool bDoCalcPosition = true;
         if (!m_bItemWasMoved && !m_bAlignmentChanged && m_aTargetData[i]->m_pTargetArea != NULL)
         {
            dX = m_aTargetData[i]->m_pTargetArea->GetX();
            dY = m_aTargetData[i]->m_pTargetArea->GetY();
            dWidth = m_aTargetData[i]->m_pTargetArea->GetWidth();
            dHeight = m_aTargetData[i]->m_pTargetArea->GetHeight();
            bDoCalcPosition = false;
         }
         
         if (bDoCalcPosition)
         {
            // center in x or y direction
            if (m_iAlignment == ALIGNMENT_HORIZONTAL)
            {
               double dXOffset = (nRectangleWidth - dMaxWidth) / 2;
               if (dXOffset > 0)
                  dX = nRectangleXPos + (i * nRectangleWidth) + dXOffset;
               else
                  dX = nRectangleXPos + (i * nRectangleWidth);
            }
            else
            {
               double dYOffset = (nRectangleHeight - dMaxHeight) / 2;
               if (dYOffset > 0)
                  dY = nRectangleYPos + (i * nRectangleHeight) + dYOffset;
               else
                  dY = nRectangleYPos + (i * nRectangleHeight);
            }
            dX -= iOffset;
            dY -= iOffset;
            dWidth = dMaxWidth+2*iOffset;
            dHeight = dMaxHeight+2*iOffset;
         }
         
         // create target area
         CString csTargetText = m_aTargetData[i]->m_csTargetName;
         DrawSdk::TargetArea *pTarget = new DrawSdk::TargetArea(dX, dY, dWidth, dHeight, csTargetText, m_aTargetData[i]->m_iBelongsTo);
         
         CRect rcTarget;
         pTarget->GetLogicalBoundingBox(&rcTarget);
         
         CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>aObjects;
         aObjects.Add(pTarget);
         
         CMoveableObjects *pMoveableTarget = new CMoveableObjects();
         pMoveableTarget->Init(&rcTarget, &visibiltyPeriod, m_pEditorProject, &aObjects, MOVEABLE_TYPE_TARGET, m_aTargetData[i]->m_iBelongsTo);
         if (m_aTargetData[i]->m_bUseTargetHashKey)
            pMoveableTarget->SetHashKey(m_aTargetData[i]->m_nTargetHashKey);

         pQuestion->AddObject(pMoveableTarget);
         for (int j = 0; j < aObjects.GetSize(); ++j)
         {
            if (aObjects[j])
               delete aObjects[j];
         }
         aObjects.RemoveAll();
         
         // create text
         double dTextWidth = DrawSdk::Text::GetTextWidth(csTargetText, csTargetText.GetLength(), &lf);
         double dTextHeight = DrawSdk::Text::GetTextHeight(csTargetText, csTargetText.GetLength(), &lf);
         double dTextDescent = DrawSdk::Text::GetTextDescent(&lf);
         double dXTextPos = dX;
         double dYTextPos = dY - dTextDescent - 5;
         
         bDoCalcPosition = true;

         COLORREF clrText = RGB(0, 0, 0);
         LFont::FillWithDefaultValues(&lf);
         if (m_aTargetData[i]->m_pText != NULL)
         {
            clrText = ((DrawSdk::Text *)(m_aTargetData[i]->m_pText))->GetOutlineColorRef();
            ((DrawSdk::Text *)(m_aTargetData[i]->m_pText))->GetLogFont(&lf);

            dTextWidth = DrawSdk::Text::GetTextWidth(csTargetText, csTargetText.GetLength(), &lf);
            dTextHeight = DrawSdk::Text::GetTextHeight(csTargetText, csTargetText.GetLength(), &lf);
            dTextDescent = DrawSdk::Text::GetTextDescent(&lf);
            dYTextPos = dY - dTextDescent - 5;
            if (!m_bItemWasMoved && !m_bAlignmentChanged)
            {
               dXTextPos = m_aTargetData[i]->m_pText->GetX();
               dYTextPos = m_aTargetData[i]->m_pText->GetY();
               bDoCalcPosition = false;
            }
         }

         
         if (bDoCalcPosition)
         {
            if (m_iAlignment == ALIGNMENT_VERTICAL)
            {
               dXTextPos = dX - dTextWidth - 10;
               double dTextAscent = DrawSdk::Text::GetTextAscent(&lf);
               dYTextPos = dY + dTextAscent;
            }
         }
         
         DrawSdk::Text *pText = new DrawSdk::Text(dXTextPos, dYTextPos, dTextWidth, dTextHeight, clrText, 
            csTargetText, csTargetText.GetLength(), &lf, 
            DrawSdk::TEXT_TYPE_TARGET_SUPPORT);
         aObjects.Add(pText);
         
         CMoveableObjects *pMoveableText = new CMoveableObjects();
         pMoveableText->Init(NULL, &visibiltyPeriod, m_pEditorProject, &aObjects, MOVEABLE_TYPE_OTHER);
         if (m_aTargetData[i]->m_bUseTextHashKey)
            pMoveableText->SetHashKey(m_aTargetData[i]->m_nTextHashKey);
         pQuestion->AddObject(pMoveableText);

         for (j = 0; j < aObjects.GetSize(); ++j)
         {
            if (aObjects[j])
               delete aObjects[j];
         }
         aObjects.RemoveAll();
      }  
   }
   
   if (m_aImageData.GetSize() > 0)
   {
      int nImageWidth = (siPage.cx - 2*dBorderXOffset) / m_aImageData.GetSize();
      int nImageHeight = (siPage.cy - dObjectBorderYTop - dObjectBorderYBottom - dSpacingY) / 2;

      if (m_iAlignment != ALIGNMENT_HORIZONTAL)
      {
         nImageWidth = (siPage.cx - 2*dBorderXOffset - dSpacingX) / 2;
         nImageHeight = (siPage.cy - dObjectBorderYTop - dObjectBorderYBottom) / 
                         m_aImageData.GetSize();
      }

      
      int nImageXPos = dBorderXOffset;
      int nImageYPos = (int)dObjectBorderYTop;
      
      for (int i = 0; i < m_aImageData.GetSize(); ++i)
      {
         CString csImageFilename = m_aImageData[i]->m_csImageName;
         
         DrawSdk::Image *pImage = new DrawSdk::Image(0.0, 0.0, 0, 0.0, csImageFilename, true);
         double dX = nImageXPos;
         double dY = nImageYPos;
         double dWidth = pImage->GetImageWidth();
         double dHeight = pImage->GetImageHeight();
         if (dWidth > nImageWidth-2*(iOffset+2)) 
         {
            double dScale = (nImageWidth-2*(iOffset+2)) / dWidth;
            dWidth = dWidth * dScale;
            dHeight = dHeight * dScale;
            pImage->SetWidth(dWidth);
            pImage->SetHeight(dHeight);
         }
         
         if (dHeight > nImageHeight-2*(iOffset+2)) 
         {
            double dScale = (nImageHeight-2*(iOffset+2)) / dHeight;
            dWidth = dWidth * dScale;
            dHeight = dHeight * dScale;
            pImage->SetWidth(dWidth);
            pImage->SetHeight(dHeight);
         }
         
         bool bDoCalcPosition = true;
         
         if (!m_bItemWasMoved && !m_bAlignmentChanged && m_aImageData[i]->m_pImage != NULL)
         {
            dX = m_aImageData[i]->m_pImage->GetX();
            dY = m_aImageData[i]->m_pImage->GetY();
            dWidth = m_aImageData[i]->m_pImage->GetWidth();
            dHeight = m_aImageData[i]->m_pImage->GetHeight();
            bDoCalcPosition = false;
         }
         
         if (bDoCalcPosition)
         {
            // center in x or y direction
            if (m_iAlignment == ALIGNMENT_HORIZONTAL)
            {
               double dXOffset = (nImageWidth - dWidth) / 2;
               if (dXOffset > 0)
                  dX = nImageXPos + dXOffset;
               else
                  dX = nImageXPos;
            }
            else
            {
               double dYOffset = (nImageHeight - dHeight) / 2;
               if (dYOffset > 0)
                  dY = nImageYPos + dYOffset;
               else
                  dY = nImageYPos;
            }
         }
         
         pImage->SetX(dX);
         pImage->SetY(dY);
         pImage->SetWidth(dWidth);
         pImage->SetHeight(dHeight);
         
         CRect rcImage;
         pImage->GetLogicalBoundingBox(&rcImage);
         
         CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>aObjects;
         aObjects.Add(pImage);
         
         CMoveableObjects *pMoveableObject = new CMoveableObjects();
         
         pMoveableObject->Init(&rcImage, &visibiltyPeriod, m_pEditorProject, &aObjects, MOVEABLE_TYPE_SOURCE, m_aImageData[i]->m_iBelongsTo);
         if (m_aImageData[i]->m_bUseHashKey)
            pMoveableObject->SetHashKey(m_aImageData[i]->m_nHashKey);
         pQuestion->AddObject(pMoveableObject);
         
         for (int j = 0; j < aObjects.GetSize(); ++j)
         {
            if (aObjects[j])
               delete aObjects[j];
         }
         aObjects.RemoveAll();

         if (m_iAlignment == ALIGNMENT_HORIZONTAL)
            nImageXPos += nImageWidth;
         else
            nImageYPos += nImageHeight;
      }  
   }


   for (int i = 0; i < m_aImageData.GetSize(); ++i)
      delete m_aImageData[i];
   m_aImageData.RemoveAll();

   for (i = 0; i < m_aTargetData.GetSize(); ++i)
      delete m_aTargetData[i];
   m_aTargetData.RemoveAll();
}

DndAlignmentId CTestDragAndDropPage::GetAlignment()
{
   DndAlignmentId dndAlignment = 
      (m_iAlignment == ALIGNMENT_VERTICAL) ? QUESTION_DND_ALIGNMENT_VERTICAL : QUESTION_DND_ALIGNMENT_HORIZONTAL;

   return dndAlignment;
}

void CTestDragAndDropPage::OnDestroy() 
{
	CTestAnswerPage::OnDestroy();	
}

/////////////////////////////////////////////////////

CSourceContainer::CSourceContainer()
{
   m_csImageName.Empty();
   m_csTargetName.Empty();

   m_pImage = NULL;
   m_pTargetArea = NULL;

   m_bUseHashKey = false;
   m_nHashKey = 0;

   m_iBelongsTo = -1;
}

CTargetContainer::CTargetContainer()
{
   m_csTargetName.Empty();

   m_pTargetArea = NULL;
   m_pText = NULL;

   m_bUseTargetHashKey = false;
   m_nTargetHashKey = 0;
   m_bUseTextHashKey = false;
   m_nTextHashKey = 0;

   m_iBelongsTo = -1;
}



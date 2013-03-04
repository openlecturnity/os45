// TestMultipleChoicePage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "TestMultipleChoicePage.h"
#include "lutils.h"
#include "DynamicObject.h"
#include "MainFrm.h"
#include "MoveableObjects.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CTestMultipleChoicePage 

IMPLEMENT_DYNCREATE(CTestMultipleChoicePage, CPropertyPage)

CTestMultipleChoicePage::CTestMultipleChoicePage() : CTestAnswerPage(CTestMultipleChoicePage::IDD)
{
   //{{AFX_DATA_INIT(CTestMultipleChoicePage)
	//}}AFX_DATA_INIT

   m_csCaption.LoadString(CTestMultipleChoicePage::IDD);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;

   m_bItemWasMoved = false;
}

CTestMultipleChoicePage::~CTestMultipleChoicePage()
{
}

void CTestMultipleChoicePage::DoDataExchange(CDataExchange* pDX)
{
   CTestAnswerPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTestMultipleChoicePage)
   DDX_Control(pDX, IDC_QUESTION_LIST, m_listAnswer);
   DDX_Control(pDX, IDC_STYLE_SELECTION, m_comboChoice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestMultipleChoicePage, CTestAnswerPage)
//{{AFX_MSG_MAP(CTestMultipleChoicePage)
   ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
   ON_WM_CREATE()
   ON_BN_CLICKED(IDC_ANSWER_LIST_ADD,OnAdd)
   ON_BN_CLICKED(IDC_ANSWER_LIST_DELETE,OnDelete)
   ON_BN_CLICKED(IDC_ANSWER_LIST_RENAME,OnRename)
   ON_BN_CLICKED(IDC_ANSWER_LIST_UP,OnMoveup)
   ON_BN_CLICKED(IDC_ANSWER_LIST_DOWN,OnMovedown)
   ON_CBN_SELCHANGE(IDC_STYLE_SELECTION, OnStyleSelection)
   ON_UPDATE_COMMAND_UI(IDC_ANSWER_LIST_DELETE, OnUpdateDelete)
   ON_UPDATE_COMMAND_UI(IDC_ANSWER_LIST_RENAME, OnUpdateRename)
   ON_UPDATE_COMMAND_UI(IDC_ANSWER_LIST_UP, OnUpdateMoveup)
   ON_UPDATE_COMMAND_UI(IDC_ANSWER_LIST_DOWN, OnUpdateMovedown)
   ON_WM_LBUTTONDOWN()
   ON_WM_VSCROLL()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTestMultipleChoicePage 

BOOL CTestMultipleChoicePage::OnInitDialog() 
{
   CTestAnswerPage::OnInitDialog();
   
   DWORD dwIds[] = {IDC_GROUP_ANSWERS,
                    IDC_ANSWER_LIST_ADD,
                    IDC_ANSWER_LIST_RENAME,
                    IDC_ANSWER_LIST_DELETE,
                    IDC_ANSWER_LIST_UP,
                    IDC_ANSWER_LIST_DOWN,
                    IDC_LABEL_STYLE,
                    IDC_LABEL_CORRECT,
                    -1};
   MfcUtils::Localize(this, dwIds);

   m_ilCheckButtons.Create(16, 16, ILC_MASK | ILC_COLOR8, 0, 2);
   m_bmpCheckButtons.LoadBitmap(IDB_CHECKBUTTON);
   m_ilCheckButtons.Add(&m_bmpCheckButtons, RGB(255, 255, 255)); 

   m_ilRadioButtons.Create(16, 16, ILC_MASK | ILC_COLOR8, 0, 2);
   m_bmpRadioButtons.LoadBitmap(IDB_RADIOBUTTON);
   m_ilRadioButtons.Add(&m_bmpRadioButtons, RGB(255, 255, 255));

   // initialize list with 2 columns
   m_listAnswer.Init(2);

   CString csEntry;
   csEntry.LoadString(IDS_SINGLE_CHOICE);
   m_comboChoice.AddString(csEntry);
   csEntry.LoadString(IDS_MULTIPLE_CHOICE);
   m_comboChoice.AddString(csEntry);
   if (m_bSingleSelection)
   {
      m_comboChoice.SetCurSel(CListCtrlEx::SINGLE_SELECTION);
      m_listAnswer.SetImageList(&m_ilCheckButtons, LVSIL_STATE);
      m_listAnswer.ChangeSelectionType(CListCtrlEx::SINGLE_SELECTION);
   }
   else
   {
      m_comboChoice.SetCurSel(CListCtrlEx::MULTIPLE_SELECTION);
      m_listAnswer.SetImageList(&m_ilRadioButtons, LVSIL_STATE);
      m_listAnswer.ChangeSelectionType(CListCtrlEx::MULTIPLE_SELECTION);
   }

   m_listAnswer.ModifyStyle(0, LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS);
   m_listAnswer.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES | LVS_EX_FULLROWSELECT);
   m_listAnswer.SetItemState(1, 1, LVIS_OVERLAYMASK);
   
   
   // set column width
   CRect listRect;
   m_listAnswer.GetClientRect(&listRect);
  
   LV_COLUMN lvc;
   lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
   
   lvc.iSubItem = 0;
   lvc.pszText = _T("");
   lvc.cx = listRect.Width() - 40;
   lvc.fmt = LVCFMT_LEFT;
   m_listAnswer.InsertColumn(0, &lvc);
   
   lvc.iSubItem = 1;
   lvc.pszText = _T("");
   lvc.cx = 40;
   lvc.fmt = LVCFMT_IMAGE;
   m_listAnswer.InsertColumn(1, &lvc);
   CUIntArray arCheckedItems;
   for (int i = 0; i < m_aAnswers.GetSize(); ++i)
   {
      CAnswerContainer *pAnswer = m_aAnswers[i];

      if (pAnswer)
      {

         LV_ITEM lvi;
         lvi.mask = LVIF_TEXT| LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
         lvi.iItem = i;
         lvi.iSubItem = 0;
         lvi.pszText = _T("");
         lvi.iImage = 0;
         lvi.stateMask = 0;
         lvi.state = 0;
         lvi.lParam = (long)pAnswer;

         m_listAnswer.InsertItem(&lvi);
         m_listAnswer.SetItemText(i, 0, pAnswer->m_csAnswer);
         
         LV_ITEM lviRight;
         lviRight.mask = LVIF_IMAGE | LVIF_STATE;
         lviRight.iItem = i;
         lviRight.iSubItem = 1;
         lviRight.iImage = 0;
         lviRight.iIndent = 0;
         lviRight.stateMask = LVIS_STATEIMAGEMASK;
         lviRight.state = INDEXTOSTATEIMAGEMASK(1);
         if (pAnswer->m_bIsCorrect)
            arCheckedItems.Add(i);
         
         m_listAnswer.SetItem(&lviRight);
      }
   }

   // insert empty item
   LV_ITEM lvi;
   lvi.mask = LVIF_TEXT| LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
   lvi.iItem = i;
   lvi.iSubItem = 0;
   lvi.pszText = _T("");
   lvi.iImage = 0;
   lvi.stateMask = 0;
   lvi.state = 0;
   lvi.lParam = 0;
   m_listAnswer.InsertItem(&lvi);
			
   for (i = 0; i < arCheckedItems.GetSize(); ++i)
      m_listAnswer.CheckItem(arCheckedItems[i]);

   m_listAnswer.SelectItem(-1);
   
   return TRUE;  // return TRUE unless you set the focus to a control
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CTestMultipleChoicePage::PreCreateWindow(CREATESTRUCT& cs) 
{
   // TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
   
   return CPropertyPage::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CTestMultipleChoicePage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
      return -1;	
   return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnKickIdle()
{
   UpdateDialogControls(this, FALSE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnAdd() 
{
   int iNewItem = m_listAnswer.GetItemCount()-1;

   CAnswerContainer *pAnswer = new CAnswerContainer();
   m_listAnswer.InsertNewItem(iNewItem, pAnswer);
   
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnStyleSelection() 
{
   UpdateData();

   if (m_comboChoice.GetCurSel() == CListCtrlEx::SINGLE_SELECTION)
   {	
      m_listAnswer.SetImageList(&m_ilCheckButtons, LVSIL_STATE);
      m_listAnswer.ChangeSelectionType(CListCtrlEx::SINGLE_SELECTION);
      m_bSingleSelection = true;
   }
   else if (m_comboChoice.GetCurSel() == CListCtrlEx::MULTIPLE_SELECTION)
   {	
      m_listAnswer.SetImageList(&m_ilRadioButtons, LVSIL_STATE);
      m_listAnswer.ChangeSelectionType(CListCtrlEx::MULTIPLE_SELECTION);
      m_bSingleSelection = false;
   }
   GetDlgItem(IDC_STYLE_SELECTION)->SetFocus();
}

void CTestMultipleChoicePage::OnUpdateDelete(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (m_listAnswer.IsItemSelected())
   {
      int iSelectedItem = m_listAnswer.GetSelectedItem();
      if (iSelectedItem >= 0 && 
          iSelectedItem < m_listAnswer.GetItemCount() - 1)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnUpdateRename(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (m_listAnswer.IsItemSelected())
   {
      int iSelectedItem = m_listAnswer.GetSelectedItem();
      if (iSelectedItem >= 0 && 
          iSelectedItem < m_listAnswer.GetItemCount() - 1)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
   
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnUpdateMoveup(CCmdUI *pCmdUI)
{
   
   bool bEnable = false;

   if (m_listAnswer.IsItemSelected())
   {
      int iSelectedItem = m_listAnswer.GetSelectedItem();
      if (iSelectedItem > 0 && 
          iSelectedItem < m_listAnswer.GetItemCount() - 1)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnUpdateMovedown(CCmdUI *pCmdUI)
{
   
   bool bEnable = false;

   if (m_listAnswer.IsItemSelected())
   {
      // item on (GetItemCount() - 1) is empty
      int iSelectedItem = m_listAnswer.GetSelectedItem();
      if (iSelectedItem >= 0 && 
          iSelectedItem < m_listAnswer.GetItemCount() - 2)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnDelete()
{
   m_listAnswer.DeleteItem(m_listAnswer.GetSelectedItem());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnRename()
{
   m_listAnswer.EditItem(m_listAnswer.GetSelectedItem());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnMoveup()
{
   m_listAnswer.MoveSelectedUp();
   m_bItemWasMoved = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnMovedown()
{
   m_listAnswer.MoveSelectedDown();
   m_bItemWasMoved = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestMultipleChoicePage::OnLButtonDown(UINT nFlags, CPoint point) 
{
   m_listAnswer.EndEditItem();
}

void CTestMultipleChoicePage::OnOK() 
{
   m_aAnswers.RemoveAll();

   m_listAnswer.EndEditItem();
   for (int i = 0; i < m_listAnswer.GetItemCount()-1; ++i)
   {
      LV_ITEM lvi;
      lvi.mask = LVIF_STATE | LVIF_PARAM;
      lvi.iItem = i;
      lvi.iSubItem = 1;
      lvi.stateMask = 0xFFFF;  
      m_listAnswer.GetItem(&lvi);

      CAnswerContainer *pAnswer = (CAnswerContainer *)lvi.lParam;

      if (pAnswer == NULL)
      {
         pAnswer = new CAnswerContainer();
      }

      if (pAnswer)
      {
         bool bIsCorrect = false;
         UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
         if (nStateImageMask)
         {
            int nImage = (nStateImageMask>>12) - 1;
            bIsCorrect = nImage == 0 ? false: true;
         }
         pAnswer->m_bIsCorrect = bIsCorrect;
         
         CString csAnswer = m_listAnswer.GetItemText(i, 0);
         pAnswer->m_csAnswer = csAnswer;

         m_aAnswers.Add(pAnswer);
      }

   }

	CTestAnswerPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// public functions

void CTestMultipleChoicePage::Init(CQuestionEx *pQuestion, CEditorProject *pProject)
{
   CTestAnswerPage::Init(pQuestion, pProject);
   
   if (pQuestion != NULL)
   {
      if (pQuestion->IsMultipleChoice())
         m_bSingleSelection = false;
      else
         m_bSingleSelection = true;

      CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
      pQuestion->GetObjectPointers(&aObjects);
      CAnswerContainer *pAnswer = NULL;
      for (int i = 0; i < aObjects.GetSize(); ++i)
      {
         CInteractionAreaEx *pInteraction = aObjects[i];
         if (pInteraction != NULL)
         {
            if (pInteraction->GetClassId() == INTERACTION_CLASS_DYNAMIC)
            {
               CDynamicObject *pObject = (CDynamicObject *)pInteraction;
               if (pInteraction != NULL)
               {
                  pAnswer = new CAnswerContainer();
                  pAnswer->m_pButton = (CDynamicObject *)pInteraction;
                  pAnswer->m_bIsCorrect = pObject->GetHasToBeChecked();
                  m_aAnswers.Add(pAnswer);
               }
            }
            else if (pInteraction->GetClassId() == INTERACTION_CLASS_MOVE)
            {
               CMoveableObjects *pMoveableObject = (CMoveableObjects *)pInteraction;
               CArray<DrawSdk::DrawObject *,DrawSdk::DrawObject *> *paObjects = pMoveableObject->GetObjects();
               for (int j = 0; j < paObjects->GetSize(); ++j)
               {
                  DrawSdk::DrawObject *pObject = paObjects->GetAt(j);
                  if (pObject != NULL && pObject->GetType() == DrawSdk::TEXT)
                  {
                     DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
                     if (pText->GetTextType() == DrawSdk::TEXT_TYPE_RADIO_SUPPORT)
                     {
                        if (pAnswer)
                        {
                           pAnswer->m_pText = pText;
                           pAnswer->m_csAnswer = pText->GetString();
                           pAnswer->m_bUseHashKey = true;
                           pAnswer->m_nHashKey = pMoveableObject->GetHashKey();
                           pAnswer = NULL;
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

void CTestMultipleChoicePage::FillWithObjects(CQuestionEx *pQuestion, CTimePeriod &visibiltyPeriod,
                                              CTimePeriod &activityPeriod, double dUpperBound)
{
   double dScaleX, dScaleY;
   CQuestionEx::GetQuestionScale(m_pEditorProject, dScaleX, dScaleY);

   double dScaledYOffset = (double)Y_OFFSET * dScaleY;
   double dScaledXOffset = (double)X_OFFSET * dScaleX;

   double dObjectBorderYTop = 
      dUpperBound > dScaledYOffset ? dUpperBound : dScaledYOffset;

   CRect rcView;
   CWhiteboardView *pWbView = CMainFrameE::GetCurrentInstance()->GetWhiteboardView();
   if (pWbView != NULL)
       pWbView->GetClientRect(&rcView);

   CRect rcPage;
   double dZoomFactor = 1.0;
   m_pEditorProject->CalculateWhiteboardArea(&rcView, &rcPage, &dZoomFactor);

   double dMaxWidth = 0;
   double dXPos = dScaledXOffset;
   double dYPos = dObjectBorderYTop;
   LOGFONT lf;
   LFont::FillWithDefaultValues(&lf);
   if (m_aAnswers.GetSize() > 0)
   {
      CString csText = m_aAnswers[0]->m_csAnswer;
      double dTextHeight = DrawSdk::Text::GetTextHeight(csText, csText.GetLength(), &lf);
      double dTextAscent = DrawSdk::Text::GetTextAscent(&lf);
      dYPos += dTextHeight;

      for (int i = 0; i < m_aAnswers.GetSize(); ++i)
      {
         CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects;

         // create radio button
         CDynamicObject *pDynamicObject = new CDynamicObject();

         double dYDiff = dYPos - dTextAscent + ((dTextHeight-16) / 2);
         CRect rcButton((int)dXPos, (int)dYDiff, (int)dXPos + 16, (int)dYDiff + 16);
         if (!m_bItemWasMoved && m_aAnswers[i]->m_pButton != NULL)
         {
            rcButton = m_aAnswers[i]->m_pButton->GetArea();
         }

         DynamicTypeId typeId = DYNAMIC_RADIOBUTTON;
         if (!m_bSingleSelection)
            typeId = DYNAMIC_CHECKBOX;

         pDynamicObject->Init(&rcButton, &visibiltyPeriod, &activityPeriod, 
            m_pEditorProject, typeId, m_aAnswers[i]->m_bIsCorrect, NULL);
         if (m_aAnswers[i]->m_pButton != NULL)
            pDynamicObject->SetHashKey(m_aAnswers[i]->m_pButton->GetHashKey());

         pQuestion->AddObject(pDynamicObject);

         // create text
         LFont::FillWithDefaultValues(&lf);
         csText = m_aAnswers[i]->m_csAnswer;
         double dTextWidth = DrawSdk::Text::GetTextWidth(csText, csText.GetLength(), &lf);

         double dTextX = dXPos + TEXT_BUTTON_DIST;
         double dTextY = dYPos;
         COLORREF clrText = RGB(0, 0, 0);
         if (m_aAnswers[i]->m_pText != NULL)
         {
            clrText = ((DrawSdk::Text *)(m_aAnswers[i]->m_pText))->GetOutlineColorRef();
            ((DrawSdk::Text *)(m_aAnswers[i]->m_pText))->GetLogFont(&lf);

            dTextWidth = DrawSdk::Text::GetTextWidth(csText, csText.GetLength(), &lf);
            dTextHeight = DrawSdk::Text::GetTextHeight(csText, csText.GetLength(), &lf);

            if (!m_bItemWasMoved)
            {
               dTextX = m_aAnswers[i]->m_pText->GetX();
               dTextY = m_aAnswers[i]->m_pText->GetY();
            }
         }
            
         DrawSdk::Text *pText = new DrawSdk::Text(dTextX, dTextY, dTextWidth, dTextHeight, clrText, 
                                                  csText, csText.GetLength(), &lf, DrawSdk::TEXT_TYPE_RADIO_SUPPORT);
         aObjects.Add(pText);

         CMoveableObjects *pMoveableObject = new CMoveableObjects();
         pMoveableObject->Init(NULL, &visibiltyPeriod, m_pEditorProject, &aObjects, MOVEABLE_TYPE_OTHER);
         if (m_aAnswers[i]->m_bUseHashKey)
            pMoveableObject->SetHashKey(m_aAnswers[i]->m_nHashKey);
         pQuestion->AddObject(pMoveableObject);

         for (int j = 0; j < aObjects.GetSize(); ++j)
         {
            if (aObjects[j])
               delete aObjects[j];
         }
         aObjects.RemoveAll();

         dYPos += (int)dTextHeight + LINE_DIST;
      }  
      
      for (i = 0; i < m_aAnswers.GetSize(); ++i)
      {
         if (m_aAnswers[i])
            delete m_aAnswers[i];
      }
      m_aAnswers.RemoveAll();
   }
   
}

CAnswerContainer::CAnswerContainer()
{
   m_pText = NULL;
   m_pButton = NULL;
   m_bIsCorrect = false;
   m_bUseHashKey = false;
   m_nHashKey = 0;
}

// TestFillInBlankPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "TestFillInBlankPage.h"
#include "MainFrm.h"
#include "lutils.h"
#include "DynamicObject.h"
#include "BlankGroup.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CTestFillInBlankPage 

IMPLEMENT_DYNCREATE(CTestFillInBlankPage, CPropertyPage)

CTestFillInBlankPage::CTestFillInBlankPage() : CTestAnswerPage(CTestFillInBlankPage::IDD)
{
	//{{AFX_DATA_INIT(CTestFillInBlankPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_csCaption.LoadString(CTestFillInBlankPage::IDD);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;

   m_pCurrentBlank = NULL;
   LFont::FillWithDefaultValues(&m_lf);
   m_clrText = RGB(0, 0, 0);
   
   m_bPositionSet = false;
}

CTestFillInBlankPage::~CTestFillInBlankPage()
{
   for (int i = 0; i < m_aCorrectAnswer.GetSize(); ++i)
      delete m_aCorrectAnswer[i];
}

void CTestFillInBlankPage::DoDataExchange(CDataExchange* pDX)
{
	CTestAnswerPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestFillInBlankPage)
	DDX_Control(pDX, IDC_DELETE_SPACE, m_btnDeleteSpace);
	DDX_Control(pDX, IDC_ANSWER_LIST, m_lstAnswers);
	DDX_Control(pDX, IDC_EDIT_TEXT, m_answerFB);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestFillInBlankPage, CTestAnswerPage)
	//{{AFX_MSG_MAP(CTestFillInBlankPage)
	ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
	ON_BN_CLICKED(IDC_INSERT_SPACE, OnInsertSpace)
	ON_BN_CLICKED(IDC_DELETE_SPACE, OnDeleteSpace)
	ON_BN_CLICKED(IDC_ADD_ANSWER, OnAddAnswer)
	ON_BN_CLICKED(IDC_DELETE_ANSWER, OnDeleteAnswer)
	ON_UPDATE_COMMAND_UI(IDC_INSERT_SPACE, OnUpdateInsertSpace)
	ON_UPDATE_COMMAND_UI(IDC_DELETE_SPACE, OnUpdateDeleteSpace)
	ON_UPDATE_COMMAND_UI(IDC_ADD_ANSWER, OnUpdateAddAnswer)
	ON_UPDATE_COMMAND_UI(IDC_DELETE_ANSWER, OnUpdateDeleteAnswer)
	ON_UPDATE_COMMAND_UI(IDC_ANSWER_LIST, OnUpdateListAnswer)
	ON_WM_DRAWITEM()
	ON_NOTIFY(NM_CLICK, IDC_ANSWER_LIST, OnClickAnswerList)
   ON_WM_VSCROLL()
	ON_MESSAGE_VOID(CAnswerEditFB::WM_SELECTION_CHANGED, OnSelectionChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTestFillInBlankPage 

void CTestFillInBlankPage::OnInsertSpace() 
{
	// TODO: Add your control notification handler code here
	m_answerFB.SetFocus();
	m_answerFB.AddBlank();
//	&m_btnDeleteSpace = (CButton*)SetFocus();

	
}

void CTestFillInBlankPage::OnDeleteSpace() 
{
	// TODO: Add your control notification handler code here
	m_answerFB.DeleteSelectedBlank();
	m_answerFB.SetFocus();
}

void CTestFillInBlankPage::OnAddAnswer() 
{
   UINT nItemId = m_lstAnswers.GetItemCount()-1;

   m_lstAnswers.InsertNewItem(nItemId);
}

void CTestFillInBlankPage::OnDeleteAnswer() 
{
   int iSelectedItem = m_lstAnswers.GetSelectedItem();
   if (iSelectedItem >= 0 && 
       iSelectedItem < m_lstAnswers.GetItemCount() - 1)
      m_lstAnswers.DeleteItem(iSelectedItem);
  
}

void CTestFillInBlankPage::OnKickIdle()
{
	UpdateDialogControls(this, FALSE);
}


void CTestFillInBlankPage::OnSelectionChanged()
{
   UpdateBlankAnswers();

   m_lstAnswers.DeleteAllItems();

   m_pCurrentBlank = m_answerFB.GetSelectedBlank();
   if (m_pCurrentBlank)
   {
      CStringArray aAnswers;
      m_pCurrentBlank->GetAnswers(aAnswers);
      for (int i = 0; i < aAnswers.GetSize(); ++i)
      {
         m_lstAnswers.InsertItem(i, aAnswers[i], 0);
      }
   }

   int iNextRow = m_lstAnswers.GetItemCount();
   // set column width
   CRect rcAnswerList;
	m_lstAnswers.GetClientRect(&rcAnswerList);
	LV_COLUMN lvcAnswer;
	lvcAnswer.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvcAnswer.iSubItem = 0;
	lvcAnswer.pszText = _T("");
	lvcAnswer.cx = rcAnswerList.Width();
	lvcAnswer.fmt = LVCFMT_LEFT;
	m_lstAnswers.InsertColumn(0,&lvcAnswer);

   // this item is needed becaus clicking on the first empty line
   // adds a new item
	LV_ITEM lviAnswerEmpty;
	lviAnswerEmpty.mask = LVIF_TEXT | LVIF_STATE;
	lviAnswerEmpty.iItem = iNextRow;
	lviAnswerEmpty.iSubItem = 0;
	lviAnswerEmpty.pszText = _T("");
	lviAnswerEmpty.iImage = 0;
	lviAnswerEmpty.stateMask = 0;
	lviAnswerEmpty.state = 0;
	m_lstAnswers.InsertItem(&lviAnswerEmpty);
}

void CTestFillInBlankPage::OnUpdateInsertSpace(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_answerFB.GetBlankSelState());
}

void CTestFillInBlankPage::OnUpdateDeleteSpace(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_answerFB.GetBlankSelState());
}

void CTestFillInBlankPage::OnUpdateAddAnswer(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_answerFB.GetBlankSelState());
}

void CTestFillInBlankPage::OnUpdateDeleteAnswer(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (m_lstAnswers.IsItemSelected())
   {
      int iSelectedItem = m_lstAnswers.GetSelectedItem();
      if (iSelectedItem >= 0 && 
          iSelectedItem < m_lstAnswers.GetItemCount() - 1)
         bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

void CTestFillInBlankPage::OnUpdateListAnswer(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (m_answerFB.GetBlankSelState())
   {
      bEnable = true;
   }

   pCmdUI->Enable(bEnable);
}

BOOL CTestFillInBlankPage::OnInitDialog() 
{
   CTestAnswerPage::OnInitDialog();
   
   DWORD dwIds[] = {IDC_GROUP_FIBTEXT,
                    IDC_INSERT_SPACE,
                    IDC_DELETE_SPACE,
                    IDC_ADD_ANSWER,
                    IDC_DELETE_ANSWER,
                    -1};
   MfcUtils::Localize(this, dwIds);

	m_lstAnswers.ModifyStyle(0, LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER);
	m_lstAnswers.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

   m_answerFB.SetTextAndBlanks(m_aTextAndBlanks, m_aCorrectAnswer);
   m_answerFB.SetSel(-1, 0);

   // set column width
   CRect rcAnswerList;
	m_lstAnswers.GetClientRect(&rcAnswerList);
	LV_COLUMN lvcAnswer;
	lvcAnswer.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvcAnswer.iSubItem = 0;
	lvcAnswer.pszText = _T("");
	lvcAnswer.cx = rcAnswerList.Width();
	lvcAnswer.fmt = LVCFMT_LEFT;
	m_lstAnswers.InsertColumn(0,&lvcAnswer);

   // this item is needed becaus clicking on the first empty line
   // adds a new item
	LV_ITEM lviAnswerEmpty;
	lviAnswerEmpty.mask = LVIF_TEXT | LVIF_STATE;
	lviAnswerEmpty.iItem = 0;
	lviAnswerEmpty.iSubItem = 0;
	lviAnswerEmpty.pszText = _T("");
	lviAnswerEmpty.iImage = 0;
	lviAnswerEmpty.stateMask = 0;
	lviAnswerEmpty.state = 0;
	m_lstAnswers.InsertItem(&lviAnswerEmpty);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTestFillInBlankPage::OnClickAnswerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}


void CTestFillInBlankPage::OnOK() 
{
   UpdateBlankAnswers();

   m_aTextAndBlanks.RemoveAll();
   m_answerFB.GetTextAndBlanks(m_aTextAndBlanks);
	
	CTestAnswerPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// public functions

void CTestFillInBlankPage::Init(CQuestionEx *pQuestion, CEditorProject *pProject)
{
   CTestAnswerPage::Init(pQuestion, pProject);
   
   if (pQuestion != NULL)
   {
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
      pQuestion->GetObjectPointers(&aObjects);
      
      for (int i = 0; i < aObjects.GetSize(); ++i)
      {
         CInteractionAreaEx *pInteraction = aObjects[i];

         if (pInteraction != NULL)
         {
            if (pInteraction->GetClassId() == INTERACTION_CLASS_BLANKS)
            {
               if (!m_bPositionSet) 
               {
                  m_rcPosition = pInteraction->GetArea();
                  m_bPositionSet = true;
               }

               CBlankGroup *pBlanks = (CBlankGroup *)pInteraction;
            
               CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paTexts = pBlanks->GetDrawObjects();
               CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paBlanks = pBlanks->GetBlankObjects();

               CDynamicObject *pBlank = NULL;
               int iBlankIndex = 0;
               if (paBlanks->GetSize() > 0)
                  pBlank = (CDynamicObject *)paBlanks->GetAt(0);
               int iBlankX = -1;
               int iBlankBottom = -1;
               int iBlankTop = -1;
               if (pBlank)
               {
                  CRect rcBlank = pBlank->GetArea();
                  iBlankX = rcBlank.left;
                  iBlankTop = rcBlank.top;
                  iBlankBottom = rcBlank.bottom;
               }
               
               DrawSdk::DrawObject *pText = NULL;
               int iTextIndex = 0;
               int iTextX = -1;
               int iTextY = -1;
               if (paTexts->GetSize() > 0)
                  pText = paTexts->GetAt(0);
               
               if (pText)
               {
                  ((DrawSdk::Text *)pText)->GetLogFont(&m_lf);
                  iTextX = (int)pText->GetX();
                  iTextY = (int)pText->GetY();
                  m_clrText = ((DrawSdk::Text *)pText)->GetOutlineColorRef();
               }
               
               double dTextAscent = DrawSdk::Text::GetTextAscent(&m_lf); 
               double dLineHeight = 2 * abs(m_lf.lfHeight);
               
               while (pText)
               {
                  if (pBlank && 
                     ((iTextY < iBlankBottom && iTextY > iBlankTop && iBlankX < iTextX) ||   // same line
                     (iTextY > iBlankBottom)))                                               // text is in next line
                  {
                     m_aTextAndBlanks.Add(_T(""));
                     CStringArray *paStrings = new CStringArray;
                     CArray<CString, CString> *paCorrectTexts = pBlank->GetCorrectTexts();
                     if (paCorrectTexts != NULL)
                     {
                        for (int s=0; s<paCorrectTexts->GetSize(); ++s)
                           paStrings->Add(paCorrectTexts->GetAt(s));
                     }
                     m_aCorrectAnswer.Add(paStrings);
                     ++iBlankIndex;
                     if (iBlankIndex < paBlanks->GetSize())
                     {
                        pBlank = (CDynamicObject *)paBlanks->GetAt(iBlankIndex);
                        CRect rcBlank = pBlank->GetArea();
                        iBlankX = rcBlank.left;
                        iBlankTop = rcBlank.top;
                        iBlankBottom = rcBlank.bottom;
                     }
                     else
                        pBlank = NULL;
                  }
                  else
                  {
                     CString csText;
                     if (((DrawSdk::Text *)pText)->GetTextLength() > 0)
                     {
                        csText = ((DrawSdk::Text *)pText)->GetString();
                     }
                     
                     if (((DrawSdk::Text *)pText)->EndsWithReturn())
                     {
                        csText += _T('\r');
                        csText += _T('\n');
                     }
                     if (!(csText.IsEmpty() || csText == _T("")))
                        m_aTextAndBlanks.Add(csText);
   
                     ++iTextIndex;
                     if (iTextIndex < paTexts->GetSize())
                     {
                        pText = paTexts->GetAt(iTextIndex);
                        iTextX = (int)pText->GetX();
                        iTextY = (int)pText->GetY();
                     }
                     else
                        pText = NULL;
                  }
               }

               if (pBlank != NULL)
               {
                  for (int j = iBlankIndex; j < paBlanks->GetSize(); ++j)
                  {
                     pBlank = (CDynamicObject *)paBlanks->GetAt(j);

                     m_aTextAndBlanks.Add(_T(""));
                     CStringArray *paStrings = new CStringArray;
                     CArray<CString, CString> *paCorrectTexts = pBlank->GetCorrectTexts();
                     if (paCorrectTexts != NULL)
                     {
                        for (int s = 0; s < paCorrectTexts->GetSize(); ++s)
                           paStrings->Add(paCorrectTexts->GetAt(s));
                     }
                     m_aCorrectAnswer.Add(paStrings);
                  }
               }

            }
            /*/
            if (pInteraction->GetClassId() == INTERACTION_CLASS_MOVE)
            {
               CMoveableObjects *pObject = (CMoveableObjects *)pInteraction;
               CArray<DrawSdk::DrawObject *,DrawSdk::DrawObject *> *paObjects = pObject->GetObjects();
               for (int j = 0; j < paObjects->GetSize(); ++j)
               {
                  DrawSdk::DrawObject *pObject = paObjects->GetAt(j);
                  if (pObject != NULL && pObject->GetType() == DrawSdk::TEXT)
                  {
                     DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
                     if (pText->GetTextType() == DrawSdk::TEXT_TYPE_BLANK_SUPPORT)
                     {
                        m_aTextAndBlanks.Add(pText->GetString());
                     }
                  }
               }
            }
            else if (pInteraction->GetClassId() == INTERACTION_CLASS_DYNAMIC)
            {
               CDynamicObject *pObject = (CDynamicObject *)pInteraction;
               if (pObject != NULL)
               {
                  m_aTextAndBlanks.Add("");
                  CStringArray *paStrings = new CStringArray;
                  CArray<CString, CString> *paCorrectTexts = pObject->GetCorrectTexts();
                  if (paCorrectTexts != NULL)
                  {
                     for (int s=0; s<paCorrectTexts->GetSize(); ++s)
                        paStrings->Add(paCorrectTexts->GetAt(s));
                  }
                  m_aCorrectAnswer.Add(paStrings);
               }
            }
            /*/
         }
      }
   }

}

void CTestFillInBlankPage::FillWithObjects(CQuestionEx *pQuestion, CTimePeriod &visibiltyPeriod,
                                           CTimePeriod &activityPeriod, double dUpperBound)
{
   double dScaleX, dScaleY;
   CQuestionEx::GetQuestionScale(m_pEditorProject, dScaleX, dScaleY);

   double dScaledYOffset = Y_OFFSET * dScaleY;
   double dScaledXOffset = X_OFFSET * dScaleX;

   double dObjectBorderYTop = 
      dUpperBound > dScaledYOffset ? dUpperBound : dScaledYOffset;

   CRect rcView;
   CWhiteboardView *pWbView = CMainFrameE::GetCurrentInstance()->GetWhiteboardView();
   if (pWbView != NULL)
       pWbView->GetClientRect(&rcView);
   
   CSize siPage;
   m_pEditorProject->GetPageDimension(siPage);
   
   double dMaxWidth = 0;
   double dXPos = dScaledXOffset;
   double dXWidth = siPage.cx - dScaledXOffset;
   double dYPos = dObjectBorderYTop;
   
   UINT nCurrentPos = 0;
   double dYBorder = abs(m_lf.lfHeight) / 4;
   double dBlankWidth = DrawSdk::Text::GetTextWidth(_T(" "), 1, &m_lf);
   double dTextAscent = DrawSdk::Text::GetTextAscent(&m_lf); 
   double dTextHeight = abs(m_lf.lfHeight); 
   double dLineHeight = 2 * abs(m_lf.lfHeight);

   dYPos += dTextHeight;
   
   double dXOrigin = dScaledXOffset;
   if (m_bPositionSet)
   {
      dXPos = m_rcPosition.left;
      dYPos = m_rcPosition.top;
      dXWidth = m_rcPosition.right;
      dXOrigin = dXPos;
   }
   
   if (m_aTextAndBlanks.GetSize() > 0)
   {
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aTexts;
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> aBlanks;
      for (int i = 0; i < m_aTextAndBlanks.GetSize(); ++i)
      {
         if (m_aTextAndBlanks[i].IsEmpty())
         {
            DynamicTypeId typeId = DYNAMIC_TEXTFIELD;
            
            CStringArray aCorrectAnswers;
            m_answerFB.GetBlankAnswers(nCurrentPos, aCorrectAnswers);
            if (aCorrectAnswers.GetSize() == 0)
               aCorrectAnswers.Add(_T("  "));
            if (aCorrectAnswers.GetSize() > 0)
            {
               int iXWidth = 0;
               for (int i = 0; i < aCorrectAnswers.GetSize(); ++i)
               {
                  Gdiplus::RectF bbox;
                  DrawSdk::Text::GetBoundingBox(aCorrectAnswers[i], aCorrectAnswers[i].GetLength(), &m_lf, bbox);
                  if (bbox.Width > iXWidth)
                     iXWidth = bbox.Width;
               }
               
               if (dXPos + iXWidth > dXWidth)
               {
                  dXPos = dXOrigin;
                  dYPos += dLineHeight;
               }
               
               CRect rcBlank((int)dXPos, (int)dYPos, 
                  (int)dXPos + iXWidth, (int)(dYPos + dTextHeight + dYBorder));
               
               CArray<CString, CString> aCorrectTexts;
               for (int s=0; s<aCorrectAnswers.GetSize(); ++s)
                  aCorrectTexts.Add(aCorrectAnswers[s]);
               
               CDynamicObject *pDynamicObject = new CDynamicObject();
               pDynamicObject->Init(&rcBlank, &visibiltyPeriod, &activityPeriod, 
                  m_pEditorProject, typeId, false, &aCorrectTexts);
               
               aBlanks.Add(pDynamicObject);
               //pQuestion->AddObject(pDynamicObject);
               dXPos += iXWidth + dBlankWidth;
               
            }
            nCurrentPos += CAnswerEditFB::BLANK_WIDTH;
         }
         else
         {
            CString csWord;
            //CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects;
            
            CRect rcText;
            rcText.left = (int)dXPos;
            rcText.top = (int)(dYPos - DrawSdk::Text::GetTextAscent(&m_lf));
            //int dMaxTextX = dXPos;
            for (int j = 0; j < m_aTextAndBlanks[i].GetLength(); ++j)
            {
               if (m_aTextAndBlanks[i][j] == ' ')
               {
                  csWord += m_aTextAndBlanks[i][j];
                  
                  double dTextWidth = DrawSdk::Text::GetTextWidth(csWord, csWord.GetLength(), &m_lf);
                  double dTextHeight = DrawSdk::Text::GetTextHeight(csWord, csWord.GetLength(), &m_lf);
                  
                  if (dXPos + dTextWidth > (dXWidth+dBlankWidth))
                  {
                     dXPos = dXOrigin;
                     dYPos += dLineHeight;
                  }
                  
                  DrawSdk::Text *pText = new DrawSdk::Text(dXPos, dYPos+dTextAscent, dTextWidth, dTextHeight, m_clrText, 
                     csWord, csWord.GetLength(), &m_lf, DrawSdk::TEXT_TYPE_BLANK_SUPPORT);
                  aTexts.Add(pText);
                  //aObjects.Add(pText);
                  
                  dXPos += dTextWidth + dBlankWidth;
                  //if (dXPos > dMaxTextX)
                  //   dMaxTextX = dXPos;
                  
                  csWord.Empty();
               }
               else if (m_aTextAndBlanks[i][j] == 13)
               {
                  ++j;
                  
                  double dTextWidth = DrawSdk::Text::GetTextWidth(csWord, csWord.GetLength(), &m_lf);
                  double dTextHeight = DrawSdk::Text::GetTextHeight(csWord, csWord.GetLength(), &m_lf);
                  
                  if (dXPos + dTextWidth > (dXWidth+dBlankWidth))
                  {
                     dXPos = dXOrigin;
                     dYPos += dLineHeight;
                  }
                  
                  DrawSdk::Text *pText = new DrawSdk::Text(dXPos, dYPos+dTextAscent, dTextWidth, dTextHeight, m_clrText, 
                     csWord, csWord.GetLength(), &m_lf, DrawSdk::TEXT_TYPE_BLANK_SUPPORT);
                  pText->EndsWithReturn(true);
                  aTexts.Add(pText);
                  
                  dXPos = dXOrigin;
                  dYPos += dLineHeight;
                  
                  csWord.Empty();
               }
               else
               {
                  csWord += m_aTextAndBlanks[i][j];
               }
            }
            
            if (!csWord.IsEmpty())
            {
               double dTextWidth = DrawSdk::Text::GetTextWidth(csWord, csWord.GetLength(), &m_lf);
               double dTextHeight = DrawSdk::Text::GetTextHeight(csWord, csWord.GetLength(), &m_lf);
               
               if (dXPos + dTextWidth > (dXWidth+dBlankWidth))
               {
                  dXPos = X_OFFSET;
                  dYPos += dLineHeight;
               }
               
               DrawSdk::Text *pText = new DrawSdk::Text(dXPos, dYPos+dTextAscent, dTextWidth, dTextHeight, m_clrText, 
                  csWord, csWord.GetLength(), &m_lf, DrawSdk::TEXT_TYPE_BLANK_SUPPORT);
               aTexts.Add(pText);
               //aObjects.Add(pText);
               
               dXPos += dTextWidth + dBlankWidth;
               //if (dXPos > dMaxTextX)
               //   dMaxTextX = dXPos;
               
               
               csWord.Empty();
            }
            
            nCurrentPos += m_aTextAndBlanks[i].GetLength();
            
            //rcText.right = (int)dMaxTextX;
            //rcText.bottom = (int)(dYPos + DrawSdk::Text::GetTextDescent(&m_lf));
            
            //CMoveableObjects *pMoveableObject = new CMoveableObjects();
            //pMoveableObject->Init(&rcText, &visibiltyPeriod, m_pEditorDoc, &aObjects, MOVEABLE_TYPE_OTHER);
            //pQuestion->AddObject(pMoveableObject);
            
            //for (int i = 0; i < aObjects.GetSize(); ++i)
            //   delete aObjects[i];
         }
         
         if (dXPos > dXWidth)
         {
            dXPos = dXOrigin;
            dYPos += dLineHeight;
         }
      }  
      
      CBlankGroup *pBlankGroup = new CBlankGroup();
      pBlankGroup->Init(&visibiltyPeriod, &activityPeriod, &aTexts, &aBlanks, m_pEditorProject);
      
      pQuestion->AddObject(pBlankGroup);
      
      for (i = 0; i < aTexts.GetSize(); ++i)
         if (aTexts[i])
            delete aTexts[i];
         aTexts.RemoveAll();
         
         for (i = 0; i < aBlanks.GetSize(); ++i)
            if (aBlanks[i])
               delete aBlanks[i];
            aBlanks.RemoveAll();
   }
}

/////////////////////////////////////////////////////////////////////////////
// private functions

void CTestFillInBlankPage::UpdateBlankAnswers()
{
   if (m_pCurrentBlank)
   {
      m_lstAnswers.EndEditItem();
      
      CStringArray aAnswers;
      for (int i = 0; i < m_lstAnswers.GetItemCount() -1; ++i)
      {
         CString csAnswer = m_lstAnswers.GetItemText(i, 0);
         aAnswers.Add(csAnswer);
      }
      m_pCurrentBlank->UpdateAnswers(aAnswers);
   }
}

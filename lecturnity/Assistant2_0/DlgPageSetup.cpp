// DlgPageSetup.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#else
#include "..\Studio\Resource.h"
#endif
#include "DlgPageSetup.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPageSetup dialog


CDlgPageSetup::CDlgPageSetup(CWnd* pParent,UINT nPageWidth, UINT nPageHeight, COLORREF clrBackground):CDialog(CDlgPageSetup::IDD, pParent)
{
   
   if(pParent!=NULL)
   {
      pParent->EnableWindow(false);
      pParent->EnableWindow(true);
   }
   
   m_iSlidesPortrait = 0;
   m_nPageWidth = nPageWidth;
   m_nPageHeight = nPageHeight;
   m_csWidth.Format(_T("%d"), nPageWidth);
   m_csHeight.Format(_T("%d"), nPageHeight);
   m_clrBackground = clrBackground;
}


void CDlgPageSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPageSetup)	
	//DDX_Control(pDX, IDC_SLIDE_PREVIEW, m_slidePreview);
	DDX_Control(pDX, IDC_SLIDES_PORTRAIT, m_radioSlidesPortrait);
   DDX_Control(pDX, IDC_SLIDES_LANDSCAPE, m_radioSlidesLandscape);
	DDX_Control(pDX, IDC_COMBO_SLIDE, m_comboSlide);
   DDX_Control(pDX, IDC_SPIN_WIDTH, m_spinWidth);
   DDX_Control(pDX, IDC_EDIT_WIDTH, m_edtWidth);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_spinHeight);
   DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edtHeight);
   DDX_Control(pDX, IDC_COLOR_PICKER, m_cpBackColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPageSetup, CDialog)
	//{{AFX_MSG_MAP(CDlgPageSetup)
	ON_BN_CLICKED(IDC_SLIDES_PORTRAIT, OnRadioSlidesPortrait)
	ON_BN_CLICKED(IDC_SLIDES_LANDSCAPE, OnRadioSlidesLandscape)
	ON_CBN_CLOSEUP(IDC_COMBO_SLIDE, OnCloseupComboSlide)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnChangeWidth)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnChangeHeight)
   //ON_CPN_XT_SELENDOK(IDC_COLOR_PICKER, OnSelEndOkBackClr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPageSetup message handlers

BOOL CDlgPageSetup::OnInitDialog() 
{
   CDialog::OnInitDialog();

   CString csTitle;
   csTitle.LoadString(IDD_PAGE_SETUP);
   SetWindowText(csTitle);
   DWORD dwIds[] = {IDC_STATIC_PAGE_SIZE,
                    IDC_STATIC_PAGE_WIDTH, 
                    IDC_STATIC_PAGE_PIXEL1,
                    IDC_STATIC_PAGE_PIXEL2,
                    IDC_STATIC_PAGE_HEIGHT,
                    IDC_STATIC_PAGE_ORIENTATION,
                    IDC_STATIC_PAGE_BACKGROUND_COLOR,
                    IDOK,
                    IDCANCEL,
                   -1};
   MfcUtils::Localize(this, dwIds);
   m_comboSlide.ResetContent();
   CString csPageSize;
   csPageSize.LoadString(IDS_PAGE_SIZE_CUSTOM);
   m_comboSlide.AddString(csPageSize);
   csPageSize.LoadString(IDS_PAGE_SIZE_A4);
   m_comboSlide.AddString(csPageSize);
   csPageSize.LoadString(IDS_PAGE_SIZE_LETTER);
   m_comboSlide.AddString(csPageSize);
   m_comboSlide.SetCurSel(0);

   m_spinWidth.SetBuddy(&m_edtWidth);
   m_spinWidth.SetRange(1,10000);
   m_spinWidth.SetPos(m_nPageWidth);
   
   m_spinHeight.SetBuddy(&m_edtHeight);
   m_spinHeight.SetRange(1, 10000);
   m_spinHeight.SetPos(m_nPageHeight);
   
   //GetDlgItem(IDC_ICON_SLIDES_PORTRAIT)->ShowWindow(SW_HIDE);
     
   
   if((m_nPageWidth==594 && m_nPageHeight==842) || (m_nPageHeight==594 && m_nPageWidth==842))
      m_comboSlide.SetCurSel(1);
   else
      if((m_nPageWidth==612 && m_nPageHeight==792) || (m_nPageHeight==612 && m_nPageWidth==792))  
         m_comboSlide.SetCurSel(2);
      else
         m_comboSlide.SetCurSel(0);
   
   if(m_nPageWidth>=m_nPageHeight)
      m_radioSlidesLandscape.SetCheck(1);
   else
      m_radioSlidesPortrait.SetCheck(1);
 
   m_cpBackColor.SetColor(m_clrBackground);
   m_cpBackColor.RedrawWindow();
  
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
   
}


void CDlgPageSetup::OnRadioSlidesPortrait() 
{
   // TODO: Add your control notification handler code here
   GetDlgItem(IDC_ICON_SLIDES_LANDSCAPE)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_ICON_SLIDES_PORTRAIT)->ShowWindow(SW_SHOW);
   if(m_iSlidesLandscape == 0)
   {
      CString csSlideStyle;
      UpdateData();
      GetDlgItem(IDC_COMBO_SLIDE)->GetWindowText(csSlideStyle);
      UpdateData();

      int fPosWidth = m_spinWidth.GetPos();
      m_spinWidth.SetPos(m_spinHeight.GetPos());
      m_spinHeight.SetPos(fPosWidth);
      m_iSlidesLandscape = 1;   
   }
}

void CDlgPageSetup::OnRadioSlidesLandscape() 
{
   // TODO: Add your control notification handler code here
   GetDlgItem(IDC_ICON_SLIDES_LANDSCAPE)->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_ICON_SLIDES_PORTRAIT)->ShowWindow(SW_HIDE);	
   if(m_iSlidesLandscape == 1)
   {
      CString csSlideStyle;
      UpdateData();
      GetDlgItem(IDC_COMBO_SLIDE)->GetWindowText(csSlideStyle);
      UpdateData();
    
      int fPosWidth=m_spinWidth.GetPos();
      m_spinWidth.SetPos(m_spinHeight.GetPos());
      m_spinHeight.SetPos(fPosWidth); 
      m_iSlidesLandscape = 0;   
   }
}


void CDlgPageSetup::OnCloseupComboSlide() 
{
   // TODO: Add your control notification handler code here
   CString csSlideStyle;
   UpdateData();
   GetDlgItem(IDC_COMBO_SLIDE)->GetWindowText(csSlideStyle);
   
   if(csSlideStyle == _T("Custom")) 
   {
      m_spinWidth.SetPos(540);
      m_spinHeight.SetPos(720);    
   }
   else
      if(csSlideStyle == _T("A4 Paper (210x297 mm)")) 
      {
         m_spinWidth.SetPos(594);
         m_spinHeight.SetPos(842);
        
      }
      else
         if(csSlideStyle == _T("Letter (8.5x11 in)")) 
         {
            m_spinWidth.SetPos(612);
            m_spinHeight.SetPos(792);
         }
                
}

void CDlgPageSetup::OnChangeWidth() 
{  
   // TODO: Add your control notification handler code here
   CString csWidth,csHeight;
   UINT iWidth = 0, iHeight = 0;

   m_csWidth = _T("");
   m_csHeight = _T("");
   
   UpdateData();
   GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(csWidth);
   GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(csHeight);
   
   for (int i = 0; i < csWidth.GetLength(); i++)
   {
      if (csWidth[i] != _T(',')) 
         m_csWidth += csWidth[i];
      else 
         iWidth = 1;
   }

   for (int j = 0; j < csHeight.GetLength(); j++)
   {
      if (csHeight[j] != _T(',')) 
         m_csHeight += csHeight[j];
      else 
         iHeight = 1;
   }
        
   if(iWidth) GetDlgItem(IDC_EDIT_WIDTH)->SetWindowText(m_csWidth); 
   if(iHeight) GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(m_csHeight);
   
   iWidth = _ttoi(m_csWidth);
   iHeight = _ttoi(m_csHeight);
      

   if((iWidth==594 && iHeight==842) || (iHeight==594 && iWidth==842))
      m_comboSlide.SetCurSel(1);
   else
      if((iWidth==612 && iHeight==792) || (iHeight==612 && iWidth==792))  
         m_comboSlide.SetCurSel(2);
      else
         m_comboSlide.SetCurSel(0);

                              
   if(iWidth>=iHeight)
   {
      m_radioSlidesPortrait.SetCheck(0);
      m_radioSlidesLandscape.SetCheck(1);
      m_iSlidesLandscape = 0;
      GetDlgItem(IDC_ICON_SLIDES_LANDSCAPE)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_ICON_SLIDES_PORTRAIT)->ShowWindow(SW_HIDE);
   }
   else
   {
      m_radioSlidesPortrait.SetCheck(1);
      m_radioSlidesLandscape.SetCheck(0);
      m_iSlidesLandscape = 1;
      GetDlgItem(IDC_ICON_SLIDES_LANDSCAPE)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_ICON_SLIDES_PORTRAIT)->ShowWindow(SW_SHOW);
   }
}


void CDlgPageSetup::OnChangeHeight() 
{	
   // TODO: Add your control notification handler code here
   CString csWidth,csHeight;
   UINT iWidth = 0, iHeight = 0;

   m_csWidth = _T("");
   m_csHeight = _T("");
   

   UpdateData();
   GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(csWidth);
   GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(csHeight);
   
   for (int i = 0; i < csWidth.GetLength(); i++)
   {
      if(csWidth[i] != _T(',')) 
         m_csWidth += csWidth[i];
      else 
         iWidth = 1;
   }

   for (int j = 0; j < csHeight.GetLength(); j++)
   {
      if(csHeight[j] != _T(',')) 
         m_csHeight += csHeight[j];
      else 
         iHeight = 1;
   }
        
   if(iWidth) GetDlgItem(IDC_EDIT_WIDTH)->SetWindowText(m_csWidth); 
   if(iHeight) GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(m_csHeight);
        
   iWidth = _ttoi(m_csWidth);
   iHeight = _ttoi(m_csHeight);
      
                              
   if((iWidth==594 && iHeight==842) || (iHeight==594 && iWidth==842))
      m_comboSlide.SetCurSel(1);
   else
      if((iWidth==612 && iHeight==792) || (iHeight==612 && iWidth==792))  
         m_comboSlide.SetCurSel(2);
      else
         m_comboSlide.SetCurSel(0);

     
   if(iWidth>=iHeight)
   {
      m_radioSlidesPortrait.SetCheck(0);
      m_radioSlidesLandscape.SetCheck(1);
      m_iSlidesLandscape = 0;
      GetDlgItem(IDC_ICON_SLIDES_LANDSCAPE)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_ICON_SLIDES_PORTRAIT)->ShowWindow(SW_HIDE);
   }
   else
   {
      m_radioSlidesPortrait.SetCheck(1);
      m_radioSlidesLandscape.SetCheck(0);
      m_iSlidesLandscape = 1;
      GetDlgItem(IDC_ICON_SLIDES_LANDSCAPE)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_ICON_SLIDES_PORTRAIT)->ShowWindow(SW_SHOW);
   }

   
}


void CDlgPageSetup::OnOK() 
{
   // TODO: Add extra validation here
    
   EndDialog(IDOK);
   CDialog::OnOK();
}


UINT CDlgPageSetup::GetHeight()
{  
   return (UINT)_ttoi(m_csHeight);
}

UINT CDlgPageSetup::GetWidth()
{  
   return (UINT)_ttoi(m_csWidth);
}

COLORREF CDlgPageSetup::GetBackgroundColor()
{
    
	return m_cpBackColor.GetColor();
}



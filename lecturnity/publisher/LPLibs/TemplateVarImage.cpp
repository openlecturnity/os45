// TemplateVarImage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TemplateVarImage.h"
#include "TemplateConfigurePage.h"

#include "MfcUtils.h" // Localize

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarImage 


CTemplateVarImage::CTemplateVarImage(CTemplateKeyword *pKeyword)
	: CDialog(CTemplateVarImage::IDD, NULL /* parent */)
{
	//{{AFX_DATA_INIT(CTemplateVarImage)
	//}}AFX_DATA_INIT

   m_pKeyword = (CTemplateImageKeyword *) pKeyword;
}


void CTemplateVarImage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateVarImage)
	DDX_Control(pDX, IDC_TMP_VAR_IMAGE_ADJUST, m_btnCheckBox);
	DDX_Control(pDX, IDC_TMP_VAR_VARNAME, m_edVarName);
	DDX_Control(pDX, IDC_TEMPLATE_VAR_IMAGE, m_imgStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateVarImage, CDialog)
	//{{AFX_MSG_MAP(CTemplateVarImage)
	ON_BN_CLICKED(IDC_TEMPLATE_VAR_IMAGE_BROWSE, OnSelectImage)
	ON_BN_CLICKED(IDC_TEMPLATE_VAR_IMAGE_REMOVE, OnRemoveImage)
	ON_BN_CLICKED(IDC_TMP_VAR_IMAGE_ADJUST, OnAdaptImageSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTemplateVarImage 

BOOL CTemplateVarImage::OnInitDialog() 
{
	CDialog::OnInitDialog();

   DWORD adwIds[] =
   {
      IDC_TMP_VAR_VARIABLE_LABEL,
      IDC_TMP_VAR_TYPE_LABEL,
      IDC_TMP_VAR_IMAGE_TYPE,
      IDC_TMP_VAR_CONTENT_LABEL,
      IDC_TMP_VAR_IMAGE_ADJUST,
      IDC_TEMPLATE_VAR_IMAGE_BROWSE,
      IDC_TEMPLATE_VAR_IMAGE_REMOVE,
      -1
   };
   MfcUtils::Localize(this, adwIds);
	
   m_edVarName.SetWindowText(m_pKeyword->m_csName);
   CString csImage;
   RETRIEVE_CSTRING(m_pKeyword->GetFileName, csImage);
   if (csImage != _T(""))
      m_imgStatic.ReadImage(csImage);

   DWORD dwImageSizeHandling = m_pKeyword->GetImageSizeHandling();
   switch (dwImageSizeHandling)
   {
      case TE_IMAGE_FITS_ALREADY:
         m_btnCheckBox.EnableWindow(FALSE);
         m_btnCheckBox.SetCheck(0);
         break;
      case TE_IMAGE_ADAPT_SIZE:
         m_btnCheckBox.EnableWindow(TRUE);
         m_btnCheckBox.SetCheck(1);
         break;
      case TE_IMAGE_DO_NOT_ADAPT:
         m_btnCheckBox.EnableWindow(TRUE);
         m_btnCheckBox.SetCheck(0);
         break;
      default:
         m_btnCheckBox.EnableWindow(FALSE);
         m_btnCheckBox.SetCheck(0);
         break;
   }

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTemplateVarImage::OnSelectImage() 
{
   CString csFileFilter;
   csFileFilter.LoadString(IDS_IMAGE_FILEFILTER);
   CFileDialog fd(TRUE,
      NULL,
      NULL,
      OFN_EXPLORER | OFN_FILEMUSTEXIST,
      csFileFilter,
      this);
   if (IDOK == fd.DoModal())
   {
      m_imgStatic.ReadImage(fd.GetPathName());
      m_pKeyword->SetFileName(fd.GetPathName());
      ((CTemplateConfigurePage*)GetParent())->UpdateSubWindows();
      g_templateEngine.SetTemplateChanged();
      ((CPropertyPage *) GetParent())->SetModified();

      // Check the size of the image. Is it too large? Too small?
      int nWidth = m_imgStatic.GetImageWidth();
      int nHeight = m_imgStatic.GetImageHeight();

      if (nWidth < m_pKeyword->m_nMinWidth ||
          nWidth > m_pKeyword->m_nMaxWidth ||
          nHeight < m_pKeyword->m_nMinHeight ||
          nHeight > m_pKeyword->m_nMaxHeight)
      {
         CString csMsg;
         csMsg.Format(IDS_TMP_IMAGE_MISMATCH,
            nWidth, nHeight,
            m_pKeyword->m_nMinWidth, m_pKeyword->m_nMinHeight,
            m_pKeyword->m_nMaxWidth, m_pKeyword->m_nMaxHeight);
         int res = MessageBox(csMsg, _T("Publisher"), MB_YESNO | MB_ICONQUESTION);

         if (res == IDYES)
         {
            m_pKeyword->SetImageSizeHandling(TE_IMAGE_ADAPT_SIZE);
            m_btnCheckBox.EnableWindow(TRUE);
            m_btnCheckBox.SetCheck(1);
         }
         else
         {
            m_pKeyword->SetImageSizeHandling(TE_IMAGE_DO_NOT_ADAPT);
            m_btnCheckBox.EnableWindow(TRUE);
            m_btnCheckBox.SetCheck(0);
         }
      }
      else
      {
         m_pKeyword->SetImageSizeHandling(TE_IMAGE_FITS_ALREADY);
         m_btnCheckBox.EnableWindow(FALSE);
         m_btnCheckBox.SetCheck(0);
      }

      g_templateEngine.SetTemplateChanged();
      ((CPropertyPage *) GetParent())->SetModified();
   }
}

void CTemplateVarImage::OnRemoveImage() 
{
   m_imgStatic.ReadImage(NULL);
   m_pKeyword->SetFileName(_T(""));
   m_pKeyword->SetImageSizeHandling(TE_IMAGE_FITS_ALREADY);
   m_btnCheckBox.EnableWindow(FALSE);
   m_btnCheckBox.SetCheck(0);

   g_templateEngine.SetTemplateChanged();
   ((CPropertyPage *) GetParent())->SetModified();
}

void CTemplateVarImage::OnAdaptImageSize() 
{
   int nCheckState = m_btnCheckBox.GetCheck();
   switch (nCheckState)
   {
      case 0: // not checked
         m_pKeyword->SetImageSizeHandling(TE_IMAGE_DO_NOT_ADAPT);
         break;
      case 1: // checked
         m_pKeyword->SetImageSizeHandling(TE_IMAGE_ADAPT_SIZE);
         break;
      case 2: // indeterminate
         m_pKeyword->SetImageSizeHandling(TE_IMAGE_FITS_ALREADY);
         break;
   }

   g_templateEngine.SetTemplateChanged();
   ((CPropertyPage *) GetParent())->SetModified();
}

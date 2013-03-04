// RecordingsToolBar.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "..\Studio\Studio.h"
#include "RecordingsToolBar.h"
#include "RecordingProfileGallery.h"
#include "MainFrm.h"
#include "MiscFunctions.h"

#include "ProfileUtils.h"   // lsutl32


// CRecordingsToolBar

IMPLEMENT_DYNAMIC(CRecordingsToolBar, CXTPToolBar)

BEGIN_MESSAGE_MAP(CRecordingsToolBar, CXTPToolBar)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DRAWITEM()

    ON_UPDATE_COMMAND_UI(IDC_REPLAY, OnUpdateReplayRecord)
    ON_UPDATE_COMMAND_UI(IDC_EDIT, OnUpdateEditRecord)
    ON_UPDATE_COMMAND_UI(IDC_PUBLISH, OnUpdatePublishRecord)

    ON_COMMAND(IDC_REPLAY, OnReplayRecord)
    ON_COMMAND(IDC_EDIT, OnEditRecord)
    ON_COMMAND(IDC_PUBLISH, OnPublishRecord)

    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)

    ON_WM_TIMER()

    ON_XTP_CREATECONTROL()
    ON_XTP_EXECUTE(ID_PUBLISHING_PROFILES, OnXTPGalleryProfiles)
    ON_UPDATE_COMMAND_UI(ID_PUBLISHING_PROFILES, OnEnableGalleryButton)
END_MESSAGE_MAP()


CRecordingsToolBar::CRecordingsToolBar() {
    m_pCommandBars = NULL;
    m_pImageManager = NULL;
    m_bButtonsNeedUpdate = true;
    m_rcButtonPlay.SetRect(0, 0, 55, 50);
    m_rcButtonEdit.SetRect(0, 0, 55, 50);
    m_rcButtonPublish.SetRect(0, 0, 80, 50);
    m_ctrlToolTipsContext = NULL;
    m_bWaitForTimer = false;
    
	m_pImageManager = new CXTPImageManager;
}

CRecordingsToolBar::~CRecordingsToolBar() {
	if (m_pImageManager)
		m_pImageManager->InternalRelease();

    if (m_pCommandBars)
        m_pCommandBars->InternalRelease();

    CMDTARGET_RELEASE(m_ctrlToolTipsContext);
}

// static public functions
void CRecordingsToolBar::FillProfileGallery(CXTPControlGallery* pControlGallery, 
                                            CXTPControlGalleryItems *pGalleryItems) {
    if (pControlGallery == NULL || pGalleryItems == NULL)
        return;

    // ClearList
    pGalleryItems->RemoveAll();

    // Add each item to the gallery list
    CString csLastActivated;
    __int64 lCustomProfileID = 0;
    ProfileUtils::GetLastActivatedProfile(csLastActivated, lCustomProfileID);
    CArray<RegistryProfileInfo *, RegistryProfileInfo *> arRecentProfiles;
    ProfileUtils::GetRecentProfiles(arRecentProfiles);

    int iToolbarWidth = 0;
    if (arRecentProfiles.GetSize() > 0) {
        for (int i = 0; i < arRecentProfiles.GetSize(); ++i) {
            // Only profiles which are not activated and used the last 50 times
            // should be appended.
            if ((csLastActivated != arRecentProfiles[i]->GetName() ||
                arRecentProfiles[i]->GetCustomProfileID() != lCustomProfileID) &&
                arRecentProfiles[i]->GetNotUsedCount() < 50) {

                    CString csLocalizedName;
                    arRecentProfiles[i]->GetLocalizedName(csLocalizedName);

                    // Add Item
                  
                    CRecordingProfileItem *pItem = new CRecordingProfileItem();
                    pGalleryItems->AddItem(pItem, 
                                           arRecentProfiles[i]->GetDefaultProfileID(), 
                                           arRecentProfiles[i]->GetImageID());

                    // Set tool tip
                    if (pItem) {
                        pItem->SetCaption(_T(""));
                       
                        pItem->SetCustomProfileID(arRecentProfiles[i]->GetCustomProfileID());

                        CString csProfileDescription;
                        arRecentProfiles[i]->GetDescription(csProfileDescription);
                        CString csLastUsedDate;
                        arRecentProfiles[i]->GetDate(csLastUsedDate);
                        CString csRecentlyUsed;
                        csRecentlyUsed.LoadString(IDS_RECENTLY_USED);
                        CString csDocument;
                        csDocument.LoadString(IDS_DOCUMENT);
                        CString csLastPublishedDoc = arRecentProfiles[i]->GetLastPublishedDoc();

                        if (arRecentProfiles[i]->GetName() == _T("Custom")) {
                            __int64 lCustomProfileID = arRecentProfiles[i]->GetCustomProfileID();

                            // Fill java profile info class
                            ProfileInfo *pInfo = ProfileUtils::GetProfileWithID(lCustomProfileID);

                            CString csShortTitle = _T("");
                            if (pInfo != NULL) {
                                csLocalizedName = pInfo->GetTitle();
                                int iLength = csLocalizedName.GetLength() < 3 ? csLocalizedName.GetLength() : 3;
                                csShortTitle = csLocalizedName.Mid(0, iLength);
                                delete pInfo;
                            }

                            pItem->SetCaption(csShortTitle);
                            pItem->SetDrawTextOverImage(TRUE);

                        }

                        CString strHTML;
                        strHTML.Format(
                            _T("<StackPanel Margin='1' MaxWidth='400'>")
                            _T("<TextBlock Padding='1, 9, 1, 9' FontWeight='Bold'>%s</TextBlock>")
                            _T("<Grid><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/>")
                            _T("<ColumnDefinition Width='*'/></Grid.ColumnDefinitions>")
                            _T("<Image Margin='1' HorizontalAlignment='Left' VerticalAlignment='Top' Source='%d'/>") 
                            _T("<StackPanel Margin='1' MaxWidth='360' VerticalAlignment='Top' Grid.Column='1'>")
                            _T("<TextBlock Padding='4, 0, 10, 7' TextWrapping='Wrap'>%s</TextBlock>")
                            _T("<TextBlock Padding='4, 6, 10, 3' TextWrapping='Wrap' FontWeight='Bold'>%s:</TextBlock>")
                            _T("<TextBlock Padding='4, 1, 10, 1' TextWrapping='Wrap'>%s</TextBlock>")
                            _T("<TextBlock Padding='4, 6, 10, 3' TextWrapping='Wrap' FontWeight='Bold'>%s:</TextBlock>")
                            _T("<TextBlock Padding='4, 1, 10, 1' TextWrapping='Wrap'>%s</TextBlock>")
                            _T("</StackPanel>")
                            _T("</Grid>")
                            _T("</StackPanel>"),
                            csLocalizedName, arRecentProfiles[i]->GetImageID(),
                            csProfileDescription, csRecentlyUsed, csLastUsedDate,
                            csDocument, csLastPublishedDoc);


                        pItem->SetToolTip(strHTML);
                }
                iToolbarWidth += 38;
            }
            delete arRecentProfiles[i];
        }
    }
    arRecentProfiles.RemoveAll();

    CString csOtherProfiles;
    csOtherProfiles.LoadString(IDS_OTHER_PROFILES);
    CXTPControlGalleryItem *pItem = new CXTPControlGalleryItem();
    pItem->SetDrawOnlyText(TRUE);
    pItem->SetSize(CSize(100, 38));
    pGalleryItems->AddItem(pItem, ID_PROFILE_OTHER);
    pItem->SetCaption(csOtherProfiles);
    iToolbarWidth += 100;

    pControlGallery->SetControlSize(CSize(iToolbarWidth, 38));

}

// public functions

void CRecordingsToolBar::FillProfileGallery() {

    if (m_pCommandBars) {
        CXTPPopupBar *pMenu = 
            (CXTPPopupBar *)m_pCommandBars->GetContextMenus()->FindCommandBar(ID_MENU_PROFILES);
        if (pMenu == NULL)
            return;
        CXTPControlGallery *pControlGallery = (CXTPControlGallery *)pMenu->GetControl(0);
        if (pControlGallery == NULL)
            return;
        CXTPControlGalleryItems *pItems = pControlGallery->GetItems();
        if (pItems == NULL)
            return;
        FillProfileGallery(pControlGallery, pItems);
    }
   
}

void CRecordingsToolBar::ChangeProfileImage(int iImageID) {
 
    CSize szIcon(32, 32);
    if (iImageID >= 0) {
        CXTPImageManagerIcon *pIcon = m_pImageManager->GetImage(iImageID, szIcon.cx);
        if (pIcon)
            m_btnPublish.SetIcon(szIcon, pIcon);
    }
    SetIconText();
}

void CRecordingsToolBar::ShowProfileGallery() {

    if (m_pCommandBars && !m_bWaitForTimer) {
        SetTimer(SHOW_PROFILES_TIMER, 500, 0);
        m_bWaitForTimer = true;
    }
}

void CRecordingsToolBar::HideProfileGallery(bool bCalledFromGallery) {
    static bool bHidingInProgress = false;

    if (m_pCommandBars) {
        KillTimer(SHOW_PROFILES_TIMER);
        m_bWaitForTimer = false;
        CXTPPopupBar* pPopup = (CXTPPopupBar*)m_pCommandBars->GetContextMenus()->FindCommandBar(ID_MENU_PROFILES);
        if (pPopup->m_hWnd != NULL && pPopup->IsVisible() && !bHidingInProgress) {
            CPoint ptCursor;
            GetCursorPos(&ptCursor);
            CRect rcPopup;
            pPopup->GetWindowRect(&rcPopup);
            CRect rcPublish;
            m_btnPublish.GetWindowRect(&rcPublish);
            rcPublish.bottom += 2;
            if ((bCalledFromGallery || !rcPopup.PtInRect(ptCursor)) && !rcPublish.PtInRect(ptCursor)) {
                bHidingInProgress = true;
                pPopup->SetTrackingMode(FALSE);
                bHidingInProgress = false;
            }
        }
    }
}

// CRecordingsToolBar-Meldungshandler

int CRecordingsToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CXTPToolBar::OnCreate(lpCreateStruct) == -1)
        return -1;

    InitCommandBars();

    CMDTARGET_RELEASE(m_ctrlToolTipsContext);

    CMenu mnuProfiles;
    mnuProfiles.LoadMenu(ID_MENU_PROFILES);
    if (m_pCommandBars) {
        CXTPCommandBar *pCommandBar = 
            m_pCommandBars->GetContextMenus()->Add(ID_MENU_PROFILES, _T("Menu Profiles"), mnuProfiles.GetSubMenu(0));
    }

    CreateToolTips();
    CreateButtons();

    m_nCurrentColorScheme = ((CStudioApp *)AfxGetApp())->GetCurrentColorScheme();

    return 0;
}

void CRecordingsToolBar::OnPaint() {
    
    CRect rcClient;
    GetClientRect(&rcClient);

    CPaintDC dc(this); // device context for painting

    dc.SetBkMode(TRANSPARENT);

    CRect rc(rcClient);

    UINT nCurrentColorScheme = ((CStudioApp *)AfxGetApp())->GetCurrentColorScheme();
    if (m_nCurrentColorScheme != nCurrentColorScheme) {
        m_nCurrentColorScheme = nCurrentColorScheme;
        m_bButtonsNeedUpdate = true;
    }

    CXTPPaintManager* pPaintManager = GetPaintManager();
    if (!pPaintManager) 
        return;

    CXTPRibbonPaintManager *pRibbonPaintManager = pPaintManager->GetRibbonPaintManager();
    if (!pRibbonPaintManager)
        return;

	CXTPOffice2007Image* pImage = pRibbonPaintManager->LoadImage(_T("RIBBONGROUPS"));

	ASSERT(pImage);
	if (!pImage)
		return;

	CRect rcSrc(4, 1, pImage->GetWidth()-4, pImage->GetHeight()-2);
	pImage->DrawImage(&dc, rcClient, rcSrc, CRect(0, 0, 0, 0));

    CRect rcLine(4, 0, rcClient.Width()-4, 1);
    CRect rcImageLine(0, 0, pImage->GetWidth(), 1);
	pImage->DrawImage(&dc, rcLine, rcImageLine, CRect(0, 0, 0, 0));

    if (m_bButtonsNeedUpdate) {
        CDC *pButtonDc = new CDC();
        pButtonDc->CreateCompatibleDC(&dc);
        HBITMAP hButtonImage = ::CreateCompatibleBitmap(dc.GetSafeHdc(), rcClient.Width(), rcClient.Height());
        pButtonDc->SelectObject(hButtonImage);
        pImage->DrawImage(pButtonDc, rcClient, rcSrc, CRect(0, 0, 0, 0));

        m_btnPlay.UpdateBackground(pButtonDc, m_rcButtonPlay.left, m_rcButtonPlay.top);
        m_btnEdit.UpdateBackground(pButtonDc, m_rcButtonEdit.left, m_rcButtonEdit.top);
        m_btnPublish.UpdateBackground(pButtonDc, m_rcButtonPublish.left, m_rcButtonPublish.top);

        ::DeleteObject(hButtonImage);
        pButtonDc->DeleteDC();
        delete pButtonDc;

        m_bButtonsNeedUpdate = false;
    }
}

void CRecordingsToolBar::OnSize(UINT nType, int cx, int cy) {
    CXTPToolBar::OnSize(nType, cx, cy);

    int iButtonOffset = 5;
    int iButtonsWidth = (m_rcButtonPlay.Width() + m_rcButtonEdit.Width() + 
                         m_rcButtonPublish.Width() + 2*iButtonOffset);
    int iLeftOffset = (double)(cx - iButtonsWidth) / 2.0;

    CRect rcButton;
    rcButton.left = 0;
    rcButton.right = rcButton.left + m_rcButtonPlay.Width();
    rcButton.top = 4;
    rcButton.bottom = 4 + m_rcButtonPlay.Height();
    m_rcButtonPlay.SetRect(rcButton.TopLeft(), rcButton.BottomRight());

    rcButton.left = m_rcButtonPlay.right + iButtonOffset;
    rcButton.right = rcButton.left + m_rcButtonEdit.Width();
    rcButton.top = 4;
    rcButton.bottom = 4 + m_rcButtonEdit.Height();
    m_rcButtonEdit.SetRect(rcButton.TopLeft(), rcButton.BottomRight());

    rcButton.left = m_rcButtonEdit.right + iButtonOffset;
    rcButton.right = rcButton.left + m_rcButtonPublish.Width();
    rcButton.top = 4;
    rcButton.bottom = 4 + m_rcButtonPublish.Height();
    m_rcButtonPublish.SetRect(rcButton.TopLeft(), rcButton.BottomRight());

    if (iLeftOffset > 0) {
        m_rcButtonPlay.OffsetRect(iLeftOffset, 0);
        m_rcButtonEdit.OffsetRect(iLeftOffset, 0);
        m_rcButtonPublish.OffsetRect(iLeftOffset, 0);
    }

    m_btnPlay.SetWindowPos(0, m_rcButtonPlay.left, m_rcButtonPlay.top,
        m_rcButtonPlay.Width(), m_rcButtonPlay.Height(), SWP_NOZORDER);
    m_btnEdit.SetWindowPos(0, m_rcButtonEdit.left, m_rcButtonEdit.top,
        m_rcButtonEdit.Width(), m_rcButtonEdit.Height(), SWP_NOZORDER);
    m_btnPublish.SetWindowPos(0, m_rcButtonPublish.left, m_rcButtonPublish.top,
        m_rcButtonPublish.Width(), m_rcButtonPublish.Height(), SWP_NOZORDER);

    m_bButtonsNeedUpdate = true;
}

void CRecordingsToolBar::OnUpdateReplayRecord(CCmdUI* pCmdUI) {   
    CWnd *pRecordingsView = GetParent();
    if (pRecordingsView && 
        pRecordingsView->GetRuntimeClass() == RUNTIME_CLASS(CRecordingsView)) {

        if (((CRecordingsView *)pRecordingsView)->IsRecordSelected())
            pCmdUI->Enable(TRUE);
        else
            pCmdUI->Enable(FALSE);
    }
}

void CRecordingsToolBar::OnUpdateEditRecord(CCmdUI* pCmdUI) {
    CWnd *pRecordingsView = GetParent();
    if (pRecordingsView && 
        pRecordingsView->GetRuntimeClass() == RUNTIME_CLASS(CRecordingsView)) {

        if (((CRecordingsView *)pRecordingsView)->IsRecordSelected())
            pCmdUI->Enable(TRUE);
        else
            pCmdUI->Enable(FALSE);
    }
}

void CRecordingsToolBar::OnUpdatePublishRecord(CCmdUI* pCmdUI) {
    BOOL bEnable = TRUE;

    // Publish should not be possible in PowerTrainer mode
    if (CStudioApp::IsPowerTrainerMode())
        bEnable = FALSE;

    // Publish only possible if there is a selected entry
    CWnd *pRecordingsView = GetParent();
    if (pRecordingsView && 
        pRecordingsView->GetRuntimeClass() == RUNTIME_CLASS(CRecordingsView)) {

        if (pRecordingsView == NULL)
            bEnable = FALSE;
        else if(!((CRecordingsView *)pRecordingsView)->IsRecordSelected())
            bEnable = FALSE;
    }

    pCmdUI->Enable(bEnable);
}

void CRecordingsToolBar::OnReplayRecord() {   
    CWnd *pRecordingsView = GetParent();
    if (pRecordingsView && 
        pRecordingsView->GetRuntimeClass() == RUNTIME_CLASS(CRecordingsView)) {

        if(((CRecordingsView *)pRecordingsView)->IsRecordSelected())
            ((CRecordingsView *)pRecordingsView)->GetRecordList()->ReplayRecord();
    }
}

void CRecordingsToolBar::OnEditRecord() {   
    CWnd *pRecordingsView = GetParent();
    if (pRecordingsView && 
        pRecordingsView->GetRuntimeClass() == RUNTIME_CLASS(CRecordingsView)) {

        if(((CRecordingsView *)pRecordingsView)->IsRecordSelected())
            ((CRecordingsView *)pRecordingsView)->GetRecordList()->EditRecord();
    }
}

void CRecordingsToolBar::OnPublishRecord() {   
    CWnd *pRecordingsView = GetParent();
    
    if (pRecordingsView && 
        pRecordingsView->GetRuntimeClass() == RUNTIME_CLASS(CRecordingsView)) {

        if(((CRecordingsView *)pRecordingsView)->IsRecordSelected())
            ((CRecordingsView *)pRecordingsView)->GetRecordList()->PublishRecord();
    }
}

BOOL CRecordingsToolBar::PreTranslateMessage(MSG* pMsg) 
{
	m_ctrlToolTipsContext->FilterToolTipMessage(this, pMsg);
	
	return CXTPToolBar::PreTranslateMessage(pMsg);
}

INT_PTR CRecordingsToolBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const {
   HWND hWndChild = NULL;

   // find child window which hits the point
   // (don't use WindowFromPoint, because it ignores disabled windows)
   // see _AfxChildWindowFromPoint(m_hWnd, point);
   ::ClientToScreen(m_hWnd, &point);
   HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);
   for (; hChild != NULL; hChild = ::GetWindow(hChild, GW_HWNDNEXT)) {
      if ((UINT)(WORD)::GetDlgCtrlID(hChild) != (WORD)-1 &&
          (::GetWindowLong(hChild, GWL_STYLE) & WS_VISIBLE)) {
         // see if point hits the child window
         CRect rect;
         ::GetWindowRect(hChild, rect);
         if (rect.PtInRect(point)) {
            hWndChild = hChild;
            break;
         }
      }
   }

   if (hWndChild != NULL) {
      // return positive hit if control ID isn't -1
      INT_PTR nHit = (UINT)(WORD)::GetDlgCtrlID(hWndChild);
      CRect rcItem;
      ::GetWindowRect(hWndChild, rcItem);

      // hits against child windows always center the tip
      if (pTI != NULL && pTI->cbSize >= sizeof(XTP_TOOLTIP_TOOLINFO)) {
         // setup the TOOLINFO structure
         pTI->hwnd = m_hWnd;
         pTI->uId = (UINT_PTR)hWndChild;
         pTI->uFlags |= TTF_IDISHWND;
         pTI->lpszText = LPSTR_TEXTCALLBACK;

         // set TTF_NOTBUTTON and TTF_CENTERTIP if it isn't a button
         if (!(::SendMessage(hWndChild, WM_GETDLGCODE, 0, 0) &
            DLGC_BUTTON))
            pTI->uFlags |= TTF_NOTBUTTON|TTF_CENTERTIP;
      }
      CString csDescription;
      CString csTitle = GetToolTipByID(nHit);
      CString strHTML;
      if (nHit == IDC_PUBLISH) {
          // Get default title and description
          GetTooltipText(IDS_IDC_PUBLISH, csTitle, csDescription);

          // Get the information of the activated profile
          RegistryProfileInfo *pActivatedProfile = ProfileUtils::GetInfoFromActiveProfile();

          if (pActivatedProfile) {

              CString csLocalizedName;
              pActivatedProfile->GetLocalizedName(csLocalizedName);
              CString csProfileDescription;
              pActivatedProfile->GetDescription(csProfileDescription);
              CString csMarkup;
              GetDescriptionInMarkup(csDescription, csMarkup);
              CString csLastUsedDate;
              pActivatedProfile->GetDate(csLastUsedDate);
              CString csRecentlyUsed;
              csRecentlyUsed.LoadString(IDS_RECENTLY_USED);
              CString csDocument;
              csDocument.LoadString(IDS_DOCUMENT);
              CString csLastPublishedDoc = pActivatedProfile->GetLastPublishedDoc();

              if (pActivatedProfile->GetName() == _T("Custom")) {
                  __int64 lCustomProfileID = pActivatedProfile->GetCustomProfileID();

                  // Fill java profile info class
                  ProfileInfo *pInfo = ProfileUtils::GetProfileWithID(lCustomProfileID);

                  if (pInfo != NULL) {
                      csLocalizedName = pInfo->GetTitle();
                      delete pInfo;
                  }
              }

              CString csLastPubDoc = csLastPublishedDoc;
              //csLPubDoc.Replace(_T("&"), _T("&amp;"));
              StringManipulation::ReplaceHtmlSpecialCharacters(csLastPubDoc);

              strHTML.Format(
                  _T("<StackPanel Margin='1' MaxWidth='400'>")
                  _T("<TextBlock Padding='1, 3, 1, 9' FontWeight='Bold'>%s</TextBlock>")
                  _T("<Grid><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/>")
                  _T("<ColumnDefinition Width='*'/></Grid.ColumnDefinitions>")
                  _T("<Image Margin='1' HorizontalAlignment='Left' VerticalAlignment='Top' Source='%d'/>") 
                  _T("<StackPanel Margin='1' MaxWidth='360' VerticalAlignment='Top' Grid.Column='1'>")
                  _T("%s")
                  _T("</StackPanel>")
                  _T("</Grid>")
                  _T("<Border Height='1' Background='#9ebbdd' />")
                  _T("<Border Height='1' Background='White' />")
                  _T("<TextBlock Padding='1, 9, 1, 9' FontWeight='Bold'>%s</TextBlock>")
                  _T("<Grid><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/>")
                  _T("<ColumnDefinition Width='*'/></Grid.ColumnDefinitions>")
                  _T("<Image Margin='1' HorizontalAlignment='Left' VerticalAlignment='Top' Source='%d'/>") 
                  _T("<StackPanel Margin='1' MaxWidth='360' VerticalAlignment='Top' Grid.Column='1'>")
                  _T("<TextBlock Padding='4, 0, 10, 7' TextWrapping='Wrap'>%s</TextBlock>")
                  _T("<TextBlock Padding='4, 6, 10, 3' TextWrapping='Wrap' FontWeight='Bold'>%s:</TextBlock>")
                  _T("<TextBlock Padding='4, 1, 10, 1' TextWrapping='Wrap'>%s</TextBlock>")
                  _T("<TextBlock Padding='4, 6, 10, 3' TextWrapping='Wrap' FontWeight='Bold'>%s:</TextBlock>")
                  _T("<TextBlock Padding='4, 1, 10, 1' TextWrapping='Wrap'>%s</TextBlock>")
                  _T("</StackPanel>")
                  _T("</Grid>")
                  _T("</StackPanel>"),
                  csTitle, IDC_PUBLISH,
                  csMarkup, 
                  csLocalizedName, pActivatedProfile->GetImageID(),
                  csProfileDescription, csRecentlyUsed, csLastUsedDate,
                  csDocument, csLastPubDoc/*csLastPublishedDoc*/);

              delete pActivatedProfile;
          } else {
              GetTooltipText(IDS_IDC_PUBLISH, csTitle, csDescription);
              CString csMarkup;
              GetDescriptionInMarkup(csDescription, csMarkup);
              strHTML.Format(
                  _T("<StackPanel Margin='1' MaxWidth='300'>")
                  _T("<TextBlock Padding='1, 3, 1, 9' FontWeight='Bold'>%s</TextBlock>")
                  _T("<Grid><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/>")
                  _T("<ColumnDefinition Width='*'/></Grid.ColumnDefinitions>")
                  _T("<Image Margin='1' HorizontalAlignment='Left' VerticalAlignment='Top' Source='%d'/>") 
                  _T("<StackPanel Margin='1' MaxWidth='360' VerticalAlignment='Top' Grid.Column='1'>")
                  _T("%s")
                  _T("</StackPanel>")
                  _T("</Grid>")
                  _T("</StackPanel>"),
                  csTitle, IDC_PUBLISH, csMarkup);
          }
      } else {
          if (nHit == IDC_REPLAY)
            GetTooltipText(IDS_IDC_REPLAY, csTitle, csDescription);
          else if (nHit == IDC_EDIT)
            GetTooltipText(IDS_IDC_EDIT, csTitle, csDescription);
          else
            GetTooltipText(nHit, csTitle, csDescription);
          CString csMarkup;
          GetDescriptionInMarkup(csDescription, csMarkup);
          strHTML.Format(
              _T("<StackPanel Margin='1' MaxWidth='300'>")
              _T("<TextBlock Padding='1, 3, 1, 9' FontWeight='Bold'>%s</TextBlock>")
              _T("<Grid><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/>")
              _T("<ColumnDefinition Width='*'/></Grid.ColumnDefinitions>")
              _T("<Image Margin='1' HorizontalAlignment='Left' VerticalAlignment='Top' Source='%d'/>") 
              _T("<StackPanel Margin='1' MaxWidth='360' VerticalAlignment='Top' Grid.Column='1'>")
              _T("%s")
              _T("</StackPanel>")
              _T("</Grid>")
              _T("</StackPanel>"),
              csTitle, nHit, csMarkup);
      }

      m_ctrlToolTipsContext->SetImageManager(pTI, m_pImageManager);
      m_ctrlToolTipsContext->FillInToolInfo(pTI, m_hWnd, rcItem, nHit, strHTML);

      return nHit;
   }

   return -1;  // not found
}

BOOL CRecordingsToolBar::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult) {
    ASSERT(pNMHDR->code == TTN_NEEDTEXT);

    TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

    if (!(pTTT->uFlags & TTF_IDISHWND))
        return FALSE;

    UINT_PTR hWnd = pNMHDR->idFrom;
    // idFrom is actually the HWND of the tool
    UINT nID = (UINT)(WORD)::GetDlgCtrlID((HWND)hWnd);
    INT_PTR nHit = (UINT)(WORD)::GetDlgCtrlID((HWND)hWnd);

    pTTT->hinst = AfxGetInstanceHandle();

    *pResult = 0;
    // bring the tooltip window above other popup windows
    ::SetWindowPos(pNMHDR->hwndFrom,  HWND_TOP, 0, 0, 0, 0,
                   SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

    return TRUE;    // message was handled
}

void CRecordingsToolBar::OnTimer(UINT nIDEvent) 
{
    if (nIDEvent == SHOW_PROFILES_TIMER) {
        CPoint ptCursor;
        GetCursorPos(&ptCursor);
        CRect rcPublishButton;
        m_btnPublish.GetWindowRect(&rcPublishButton);	 
		int iGalleryWidth = 0; 
		int iGalleryHeight = 0;
        if (rcPublishButton.PtInRect(ptCursor) && !m_btnPublish.IsMouseButtonPressed()) {

            if (m_pCommandBars) {
                CXTPPopupBar *pMenu = 
                    (CXTPPopupBar *)m_pCommandBars->GetContextMenus()->FindCommandBar(ID_MENU_PROFILES);
                if (pMenu == NULL)
                    return;
                CXTPControlGallery *pControlGallery = (CXTPControlGallery *)pMenu->GetControl(0);
                if (pControlGallery == NULL)
                    return;
                iGalleryWidth = pControlGallery->GetWidth();
                iGalleryHeight = pControlGallery->GetHeight();
            }
            CPoint ptGallery(0, 0);
            ptGallery.x = (double)(m_rcButtonPublish.right+m_rcButtonPublish.left)/2.0 - (double)iGalleryWidth/2.0;
            ptGallery.y = m_rcButtonPublish.bottom;
            ClientToScreen(&ptGallery);
            // Test if there is enought space below the Publish button
            // If not display the popup above.
            RECT rcWork;
            SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcWork, 0);
            if (ptGallery.y + iGalleryHeight > (rcWork.bottom-10)) {
                ptGallery.x = (double)(m_rcButtonPublish.right+m_rcButtonPublish.left)/2.0 - (double)iGalleryWidth/2.0;
                ptGallery.y = m_rcButtonPublish.top - iGalleryHeight;
                ClientToScreen(&ptGallery);
            }

            CXTPPopupBar* pPopup = (CXTPPopupBar*)m_pCommandBars->GetContextMenus()->FindCommandBar(ID_MENU_PROFILES);
            if (pPopup->m_hWnd == NULL || !pPopup->IsVisible()) {
                // If the static variable dMaxWidthDivisor (default 0.33) is not changed, the popup size is 
                // restricted to dMaxWidthDivisor * (working area width)
                double dMaxWidthDivisor = CXTPPopupBar::m_dMaxWidthDivisor;
                CXTPPopupBar::m_dMaxWidthDivisor = 1.0;

                pPopup->EnableAnimation(TRUE);
                m_pCommandBars->TrackPopupMenu(ID_MENU_PROFILES, TPM_LEFTBUTTON , ptGallery.x , ptGallery.y, NULL, false);

                CXTPPopupBar::m_dMaxWidthDivisor = dMaxWidthDivisor;

            }
        }
        KillTimer(SHOW_PROFILES_TIMER);
        m_bWaitForTimer = false;
    }

    CXTPToolBar::OnTimer(nIDEvent);
}

void CRecordingsToolBar::CreateButtons() {
    
    CSize szIcon(32, 32);
    
    if (m_pImageManager) {
        // Images for "Replay", "Edit", "Publish"
        UINT uiButtons[] = {IDC_REPLAY, IDC_EDIT, IDC_PUBLISH}; 
        m_pImageManager->SetIcons(IDB_REPLAY_EDIT_PUBLISH, uiButtons, _countof(uiButtons), CSize(32, 32));
        // Images for different profiles
        m_pImageManager->SetIcons(IDB_PUBLISHING_PROFILES, 0, 0, CSize(32, 32));
    }

    CString csButton;

    CXTPImageManagerIcon *pIcon = NULL;
    csButton.LoadString(IDC_REPLAY);
    m_btnPlay.Create(csButton, WS_CHILD| WS_VISIBLE, m_rcButtonPlay, this, IDC_REPLAY);
    pIcon = m_pImageManager->GetImage(IDC_REPLAY, szIcon.cx);
    if (pIcon)
        m_btnPlay.SetIcon(szIcon, pIcon);
    
    csButton.LoadString(IDC_EDIT);
    m_btnEdit.Create(csButton, WS_CHILD | WS_VISIBLE, m_rcButtonEdit, this, IDC_EDIT);
    pIcon = m_pImageManager->GetImage(IDC_EDIT, szIcon.cx);
    if (pIcon)
        m_btnEdit.SetIcon(szIcon, pIcon);
    
    csButton.LoadString(IDC_PUBLISH);
    m_btnPublish.Create(csButton, WS_CHILD | WS_VISIBLE, m_rcButtonPublish, this, IDC_PUBLISH);
    pIcon = m_pImageManager->GetImage(IDC_PUBLISH, szIcon.cx);
    if (pIcon)
        m_btnPublish.SetIcon(szIcon, pIcon);

    CString csLastActivated;
    __int64 lCustomProfileID = 0;
    HRESULT hr = ProfileUtils::GetLastActivatedProfile(csLastActivated, lCustomProfileID);
    // Default: Flash
    if (FAILED(hr) || csLastActivated.IsEmpty()) {
        csLastActivated = _T("Flash");
        ProfileUtils::SetLastActivatedProfile(csLastActivated, lCustomProfileID);
    }
    int iImageID = ProfileUtils::GetImageIDFromName(csLastActivated);
    if (iImageID >= 0) {
        pIcon = m_pImageManager->GetImage(iImageID, szIcon.cx);
        if (pIcon)
            m_btnPublish.SetIcon(szIcon, pIcon);
    }
    SetIconText();

    //m_btnPublish.SetPushButtonStyle(xtpButtonSplitDropDown);
}

void CRecordingsToolBar::CreateToolTips() {
    // Disable command bar default tooltips.
    EnableToolTips(FALSE); 

   
    // Button tooltips
    m_ctrlToolTipsContext = new CXTPToolTipContext();
    m_ctrlToolTipsContext->SetStyle(xtpToolTipMarkup);
    m_ctrlToolTipsContext->SetMaxTipWidth(400);
    m_ctrlToolTipsContext->SetMargin(CRect(2, 2, 2, 2));  
    m_ctrlToolTipsContext->SetDelayTime(TTDT_INITIAL, 900);
    m_ctrlToolTipsContext->ShowTitleAndDescription();
    m_ctrlToolTipsContext->ShowToolTipAlways();
    m_ctrlToolTipsContext->ModifyToolTipStyle(0, XTP_TTS_OFFICE2007FRAME);
    if (m_pCommandBars)
        m_ctrlToolTipsContext->SetFont(m_pCommandBars->GetPaintManager()->GetIconFont());

    // Tooltips of gallery
    if (m_pCommandBars == NULL)
        return;

    CXTPToolTipContext* pToolTipContext = m_pCommandBars->GetToolTipContext();
    pToolTipContext->SetStyle(xtpToolTipMarkup);
    pToolTipContext->SetMargin(CRect(2, 2, 2, 2));
    pToolTipContext->ShowImage(TRUE, 0);
    pToolTipContext->SetMaxTipWidth(200);
    pToolTipContext->ModifyToolTipStyle(0, XTP_TTS_OFFICE2007FRAME);
    pToolTipContext->SetFont(m_pCommandBars->GetPaintManager()->GetIconFont());
    pToolTipContext->SetDelayTime(TTDT_INITIAL, 900);
    
    if (XTPImageManager()) {
        // Images for "Replay", "Edit", "Publish"
        UINT uiButtons[] = {IDC_REPLAY, IDC_EDIT, IDC_PUBLISH}; 
        XTPImageManager()->SetIcons(IDB_REPLAY_EDIT_PUBLISH, uiButtons, _countof(uiButtons), CSize(32, 32));
        // Images for different profiles
        XTPImageManager()->SetIcons(IDB_PUBLISHING_PROFILES, 0, 0, CSize(32, 32));
    }
}

CString CRecordingsToolBar::GetToolTipByID(UINT uID) const {
    CString strTipText;
    // don't handle the message if no string resource found
    if (XTPResourceManager()->LoadString(&strTipText, uID))
        return strTipText;

    return _T("Title");
}

void CRecordingsToolBar::GetTooltipText(UINT nTextID, CString &csTitle, CString &csDescription)  const { 
    CString csFromResources; 
    csFromResources.LoadString(nTextID); 
    int iPos = csFromResources.ReverseFind(_T('\n')); 
    csTitle = csFromResources.Right(csFromResources.GetLength() - iPos - 1); 
    csDescription = csFromResources.Left(iPos); 
    if (csDescription.IsEmpty() == true)
        csDescription = csTitle;
}

void CRecordingsToolBar::GetDescriptionInMarkup(CString csDescription, CString &csMarkup) const {

    csMarkup.Empty();
    int iLastUsedPos = 0;
    bool bFinished = false;
    bool bFirst = true;
    while (!bFinished) {
        int iPos = csDescription.Find(_T("\n"), iLastUsedPos);
        if (iPos > 0) {
            CString csPart = csDescription.Mid(iLastUsedPos, (iPos+1)-iLastUsedPos);
            CString csLine;
            csLine.Format(_T("<TextBlock Padding='4, 0, 10, 0' TextWrapping='Wrap'>%s</TextBlock>"), csPart);
            csMarkup += csLine;
            iLastUsedPos = iPos+1;
            bFirst = false;
        } else
            bFinished = true;
    }
}

BOOL CRecordingsToolBar::InitCommandBars(CRuntimeClass* pCommandBarsClass) {
    ASSERT(pCommandBarsClass->IsDerivedFrom(RUNTIME_CLASS(CXTPCommandBars)));
    m_pCommandBars =  (CXTPCommandBars*) pCommandBarsClass->CreateObject();
    ASSERT(m_pCommandBars);
    if (!m_pCommandBars)
        return FALSE;
    m_pCommandBars->SetSite(this);
    m_pCommandBars->EnableDocking();
    return TRUE;
}

int CRecordingsToolBar::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl) {  
    if (lpCreateControl->nID == ID_PUBLISHING_PROFILES) {
        CXTPControlGalleryItems *pItemsProfiles = CXTPControlGalleryItems::CreateItems(m_pCommandBars, ID_PUBLISHING_PROFILES);

        pItemsProfiles->GetImageManager()->SetIcons(IDB_PUBLISHING_PROFILES, 0, 0, CSize(32, 32)); // Get images from same id.
        pItemsProfiles->GetImageManager()->SetMaskColor(COLORREF_NULL); // Default TopLeft Pixel.
        pItemsProfiles->ClipItems(TRUE);
        pItemsProfiles->SetItemSize(CSize(38, 38));

        CRecordingProfileGallery* pControlGallery = new CRecordingProfileGallery(this);
        pControlGallery->SetStyle(xtpButtonIconAndCaption);
        pControlGallery->SetItems(pItemsProfiles);
        pControlGallery->ShowScrollBar(FALSE);

        CRecordingsToolBar::FillProfileGallery(pControlGallery, pItemsProfiles);

        lpCreateControl->pControl = pControlGallery;

        return TRUE;
    }

    return FALSE;
}

void CRecordingsToolBar::SetIconText() {
    CString csLastActivated = _T("");
    __int64 lCustomProfileID = 0;
    HRESULT hr = ProfileUtils::GetLastActivatedProfile(csLastActivated, lCustomProfileID);

    if (csLastActivated == _T("Custom")) {
        RegistryProfileInfo *pActivatedProfile = ProfileUtils::GetInfoFromActiveProfile();
        if (pActivatedProfile) {
            __int64 lCustomProfileID = pActivatedProfile->GetCustomProfileID();

            // Fill java profile info class
            ProfileInfo *pInfo = ProfileUtils::GetProfileWithID(lCustomProfileID);

            CString csLocalizedName = _T("");
            CString csShortTitle = _T("");
            if (pInfo != NULL) {
                csLocalizedName = pInfo->GetTitle();
                int iLength = csLocalizedName.GetLength() < 3 ? csLocalizedName.GetLength() : 3;
                csShortTitle = csLocalizedName.Mid(0, iLength);
                delete pInfo;
            }
            m_btnPublish.SetIconText(csShortTitle);

            delete pActivatedProfile;
        }
    } else 
        m_btnPublish.ResetIconText();
}

void CRecordingsToolBar::OnXTPGalleryProfiles(NMHDR* pNMHDR, LRESULT* pResult) {
    NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
    CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
    if (pGallery) {
        CRecordingProfileItem* pItem = (CRecordingProfileItem *)pGallery->GetItem(pGallery->GetSelectedItem());
        if (pItem) {
            CMainFrameA *pMainFrameA = CMainFrameA::GetCurrentInstance();
            CRecordingsView *pRecordingsView = NULL;
            if (pMainFrameA)
                pRecordingsView = pMainFrameA->GetRecordingView();
            int iProfileID = pItem->GetID();
            int iImageID = pItem->GetImageID();
            __int64 lCustomProfileID = pItem->GetCustomProfileID();
            if (pRecordingsView != NULL) {
                if (iProfileID == ID_PROFILE_OTHER) {
                    // format selection only
                    pRecordingsView->ShowProfileSelectionDialog();
                } else {
                    pRecordingsView->ChangePublishButtonImage(iProfileID, iImageID, lCustomProfileID);
                    CXTPControlGalleryItems *pItems = pGallery->GetItems();
                    pRecordingsView->FillProfileGallery(pGallery, pItems);
                }
            }
        }
        *pResult = TRUE; // Handled
    }
}

void CRecordingsToolBar::OnEnableGalleryButton(CCmdUI* pCmdUI) {
    pCmdUI->Enable();
}

// RecordingsList.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "RecordingsList.h"
#include "backend/la_presentationinfo.h"
#include "ConfirmDelete.h"
#include <gdiplus.h>
#include "ScreengrabbingExtendedExternal.h" // lsutl32
#include "ProfileUtils.h"                   // lsutl32
#include "CommandBars\XTPPaintManager.h"
#include "backend\mlb_misc.h"
#include "DlgRecordRename.h"

#include "..\Studio\Studio.h"
#ifdef _STUDIO
#include "..\Studio\MainFrm.h"
#include "..\Studio\RibbonBars.h"
#include "..\Studio\Message.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecordingsList

CRecordingsList::CRecordingsList() {
    m_nTextPadding = 40;
    m_ctrlToolTipsContext = NULL;
}

CRecordingsList::~CRecordingsList() {
    CMDTARGET_RELEASE(m_ctrlToolTipsContext);
}

BEGIN_MESSAGE_MAP(CRecordingsList, CXTListBox)
   //{{AFX_MSG_MAP(CRecordingsList)
   ON_WM_CREATE()
   ON_WM_CONTEXTMENU()
   ON_WM_CHAR()
   ON_COMMAND(ID_EXPORT_RECORDING, ExportRecording)
   ON_COMMAND(ID_PUBLISH, PublishRecord)
   ON_COMMAND(ID_PUBLISH_WITHOUT_PROFILE, PublishRecordAdvanced)
   ON_COMMAND(ID_PUBLISH_FORMAT, PublishRecordFormatSelection)
   ON_COMMAND(ID_EDIT, EditRecord)
   ON_COMMAND(IDC_REPLAY, ReplayRecord)
   ON_COMMAND(ID_REMOVE_ENTRY, RemoveEntry)
   ON_COMMAND(ID_DELETE_RECORDINGS, DeleteRecord)
   ON_COMMAND(ID_PROPERTIES, RecordProperties)
   ON_COMMAND(ID_RECORDINGS_EXPLORE, ExploreRecordingPath)
   ON_COMMAND(ID_RENAME_RECORD, RenameRecord)
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnUpdateList)
   ON_COMMAND(ID_STRUCTURE_INSERT, ClipStructureInsert)
   ON_COMMAND(ID_STRUCTURE_COMPLETE, ClipStructureInsert)
   ON_COMMAND(ID_STRUCTURE_REMOVE, ClipStructureRemove)
   ON_COMMAND(ID_STRUCTURE_EDIT, ClipStructureEdit)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
   ON_WM_PAINT()
   //}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecordingsList message handlers

void CRecordingsList::OnPaint()
{
   if ( GetCount() > 0 )
   {
      return CXTListBox::OnPaint();
   }

	CPaintDC dc(this);

	// Get the client rect.
	CRect r;
	GetClientRect(&r);

   if ( GetCount() <= 0 )
   {
      COLORREF clrWindowText = IsWindowEnabled() ? GetXtremeColor(COLOR_WINDOWTEXT) : GetXtremeColor(COLOR_GRAYTEXT);
      COLORREF clrWindow = GetBackColor();

      dc.SetBkColor(clrWindow);
      dc.SetTextColor(clrWindowText);
      if (clrWindow != COLORREF_NULL) 
         dc.FillSolidRect(&r, clrWindow);

      CString m_strNoItems;
      CXTPResourceManager::AssertValid(XTPResourceManager()->LoadString(&m_strNoItems, XTP_IDS_REPORT_NOITEMS));
      if (!m_strNoItems.IsEmpty())
      {
         CRect rcText(r);
         rcText.DeflateRect(5, 5, 5, 5);
         UINT uFlags = DT_CENTER | DT_TOP | DT_NOPREFIX | DT_WORDBREAK |
			   DT_WORD_ELLIPSIS | DT_END_ELLIPSIS | DT_EDITCONTROL;

         CFont *pFont = GetFont();
         if ( pFont != NULL )
         {
            LOGFONT lf;
            pFont->GetLogFont(&lf);
            HFONT hFontNew, hFontOld;
            hFontNew = CreateFontIndirect(&lf);
            if ( hFontNew != NULL )
            {
               hFontOld = (HFONT) SelectObject(dc.m_hDC, hFontNew);
               if ( hFontOld != NULL )
               {
                  dc.DrawText(m_strNoItems, rcText, uFlags);
                  SelectObject(dc.m_hDC, hFontOld);
                  DeleteObject(hFontNew);
               }
               else
                  DeleteObject(hFontNew);
            }
         }

         return;
      }
   }
}
int CRecordingsList::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    CreateToolTips();
    return CXTListBox::OnCreate(lpCreateStruct);
}

void CRecordingsList::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    if (GetCount() <= 0) {
        return;
    }
    // find current row
    CPoint ptPoint = point;
    ScreenToClient(&ptPoint);

    CMenu menu;
    CMenu *submenu;
    menu.LoadMenu(IDR_RECORDINGS);

    int iSelCount = GetSelCount();
    BOOL bOutside = FALSE;
    UINT nIndex = ItemFromPoint(ptPoint, bOutside);

    if (iSelCount <= 1) {

        // select current row
        if (nIndex >= 0 && !bOutside) {
            SetSel(GetCurSel(), FALSE);
            SetSel(nIndex, TRUE);
        }

        if(GetCurSel() >= 0)
        {
            ASSISTANT::PresentationInfo *pPresentation = 
                (ASSISTANT::PresentationInfo *)GetItemDataPtr(GetCurSel());

            /*CMenu menu;
            CMenu *submenu;
            menu.LoadMenu(IDR_RECORDINGS);*/  

            bool bIsInPowerTrainerMode = CStudioApp::IsPowerTrainerMode();

            if (bIsInPowerTrainerMode) {
                menu.EnableMenuItem(ID_PUBLISH, MF_GRAYED);
                menu.EnableMenuItem(ID_PUBLISH_FORMAT, MF_GRAYED);
                menu.EnableMenuItem(ID_PUBLISH_WITHOUT_PROFILE, MF_GRAYED);
                menu.EnableMenuItem(ID_EXPORT_RECORDING, MF_GRAYED);
            }

            // A filesystem check on the existance of an .lep file
            if (pPresentation != NULL && !pPresentation->IsModified())
                menu.EnableMenuItem(ID_EXPORT_RECORDING, MF_GRAYED);

            if (pPresentation != NULL)
            {         
                if(pPresentation->IsModified())
                {  // currently only unmodified / finalized documents are supported
                    menu.EnableMenuItem(ID_STRUCTURE_INSERT, MF_GRAYED);
                    menu.EnableMenuItem(ID_STRUCTURE_COMPLETE, MF_GRAYED);
                    menu.EnableMenuItem(ID_STRUCTURE_REMOVE, MF_GRAYED);
                    menu.EnableMenuItem(ID_STRUCTURE_EDIT, MF_GRAYED);
                }
                else if(pPresentation->IsDenverDocument())
                {
                    // pure screengrabbing document
                    menu.EnableMenuItem(ID_STRUCTURE_COMPLETE, MF_GRAYED);
                    menu.EnableMenuItem(ID_STRUCTURE_REMOVE, MF_GRAYED);
                    menu.EnableMenuItem(ID_STRUCTURE_EDIT, MF_GRAYED);
                }
                else if(pPresentation->IsClipSearchable())
                {
                    // fully structured
                    menu.EnableMenuItem(ID_STRUCTURE_INSERT, MF_GRAYED);
                    menu.EnableMenuItem(ID_STRUCTURE_COMPLETE, MF_GRAYED);
                }
                else if(pPresentation->IsClipStructured())
                {
                    // structure incomplete (not searchable)
                    menu.EnableMenuItem(ID_STRUCTURE_INSERT, MF_GRAYED);
                }
                else
                {  // other document type: no structuring features
                    menu.EnableMenuItem(ID_STRUCTURE_INSERT, MF_GRAYED);
                    menu.EnableMenuItem(ID_STRUCTURE_COMPLETE, MF_GRAYED);
                    menu.EnableMenuItem(ID_STRUCTURE_REMOVE, MF_GRAYED);
                    menu.EnableMenuItem(ID_STRUCTURE_EDIT, MF_GRAYED);
                }
            }

            submenu = menu.GetSubMenu(0);
            submenu->SetDefaultItem(IDC_REPLAY);
            if(submenu)
                submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
        }	
    } else {
        SetSel(nIndex, TRUE);
        menu.EnableMenuItem(IDC_REPLAY, MF_GRAYED);
        menu.EnableMenuItem(ID_EDIT, MF_GRAYED);
        menu.EnableMenuItem(ID_PUBLISH, MF_GRAYED);
        menu.EnableMenuItem(ID_PUBLISH_FORMAT, MF_GRAYED);
        menu.EnableMenuItem(ID_PUBLISH_WITHOUT_PROFILE, MF_GRAYED);
        menu.EnableMenuItem(ID_EXPORT_RECORDING, MF_GRAYED);
        menu.EnableMenuItem(ID_STRUCTURE_INSERT, MF_GRAYED);
        menu.EnableMenuItem(ID_STRUCTURE_COMPLETE, MF_GRAYED);
        menu.EnableMenuItem(ID_STRUCTURE_REMOVE, MF_GRAYED);
        menu.EnableMenuItem(ID_STRUCTURE_EDIT, MF_GRAYED);
        menu.EnableMenuItem(ID_RENAME_RECORD, MF_GRAYED);
        menu.EnableMenuItem(ID_PROPERTIES, MF_GRAYED);
        menu.EnableMenuItem(ID_RECORDINGS_EXPLORE, MF_GRAYED);

        submenu = menu.GetSubMenu(0);
        //submenu->SetDefaultItem(IDC_REPLAY);
        if(submenu)
            submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
    }
}

void CRecordingsList::SelectItem(int nItemID)
{
   SetCurSel(nItemID);
}

void CRecordingsList::ReplayRecord() 
{
   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );

   if (pPresentation != NULL)
      pPresentation->StartReplay();
}

void CRecordingsList::ExportRecording() 
{
   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );

   if (pPresentation != NULL)
      pPresentation->StartExport();
}

void CRecordingsList::PublishRecord() 
{
   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );

   if (pPresentation != NULL) {

       CString csActiveProfile;
       HRESULT hr = ProfileUtils::GetLastActivatedProfile(csActiveProfile);
       if (SUCCEEDED(hr)) {
           if (csActiveProfile == _T("YouTube")) {
               HKEY hKeyUser; 
               LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, 
                                _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"),
                                NULL, KEY_READ, &hKeyUser);
               // Key does not exist -> Call settings dialog.
               if (lResult != ERROR_SUCCESS) {
                   int iReturnID = ProfileUtils::ShowYouTubeSettings();
                   if (iReturnID == IDCANCEL)
                       return;
               }
           } else if (csActiveProfile == _T("Slidestar")) {
               HKEY hKeyUser; 
               LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, 
                                _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Slidestar_Default"),
                                NULL, KEY_READ, &hKeyUser);
               // Key does not exist -> Call settings dialog.
               if (lResult != ERROR_SUCCESS) {
                   int iReturnID = ProfileUtils::ShowSlidestarSettings();
                   if (iReturnID == IDCANCEL)
                       return;
               }
           }
       }

      pPresentation->StartConvert(true);    // direct mode
      CWnd *pParent = GetParent();
      if (pParent && pParent->GetRuntimeClass() == RUNTIME_CLASS(CRecordingsView))
          ((CRecordingsView *)pParent)->FillProfileGallery(NULL, NULL);
   }
}


void CRecordingsList::PublishRecordAdvanced() 
{
   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );

   if (pPresentation != NULL)
      pPresentation->StartConvert(false);	// do not direct mode (old style full-featured publisher)
}

void CRecordingsList::PublishRecordFormatSelection() 
{
   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );

   if (pPresentation != NULL)
      pPresentation->StartConvert(true, true);	// direct mode with format selection

   // update profile gallery
   CWnd *pParent = GetParent();
   if (pParent && pParent->GetRuntimeClass() == RUNTIME_CLASS(CRecordingsView)) {
       CString csLastActivated;
       __int64 lCustomProfileID = 0;
       ProfileUtils::GetLastActivatedProfile(csLastActivated, lCustomProfileID);
       int iImageID = ProfileUtils::GetImageIDFromName(csLastActivated);
       int iProfileID = ProfileUtils::GetDefaultProfileIDFromName(csLastActivated);
       ((CRecordingsView *)pParent)->ChangePublishButtonImage(iProfileID, iImageID, lCustomProfileID);
       ((CRecordingsView *)pParent)->FillProfileGallery(NULL, NULL);
   }
}

void CRecordingsList::ClipStructureInsert() 
{
   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );

   if (pPresentation != NULL && !pPresentation->IsModified()) {
      TRACE(_T("ClipStructureInsert\n"));
      CString csRecordPath = pPresentation->GetFilename();
      HRESULT hr = CScreengrabbingExtendedExternal::RunPostProcessing(csRecordPath);
      // update presentation info
      // TODO: this is not reliable - re-read information instead
      if(hr == S_OK) 
         pPresentation->SetClipSearchable( true );
      else
         // ???
         ;
   }
}

void CRecordingsList::ClipStructureRemove() 
{
   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );

   if (pPresentation != NULL && !pPresentation->IsModified() && pPresentation->IsClipStructured()) {
      TRACE(_T("ClipStructureRemove\n"));
      CString csRecordPath = pPresentation->GetFilename();
      HRESULT hr = CScreengrabbingExtendedExternal::RemoveStructure(csRecordPath);
      // update presentation info
      // TODO: this is not reliable - re-read information instead
      if(hr == S_OK) 
         pPresentation->SetDenverDocument( true );
      else
         Message::ShowError(ID_STRUCTURE_REMOVE_ERROR);
   }
}

void CRecordingsList::ClipStructureEdit() 
{
   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );

   if (pPresentation != NULL && !pPresentation->IsModified() && pPresentation->IsClipStructured()) {
      TRACE(_T("ClipStructureEdit\n"));
      CString csRecordPath = pPresentation->GetFilename();
      HRESULT hr = CScreengrabbingExtendedExternal::EditStructure(csRecordPath);
      // update presentation info
      // TODO: this is not reliable - re-read information instead
      if(hr == S_OK) 
         pPresentation->SetClipSearchable( true );
   }
}

void CRecordingsList::EditRecord() 
{
   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );

   if (pPresentation != NULL)
      pPresentation->StartEdit();
}

void CRecordingsList::RemoveEntry() 
{
  // int nSelectedItem = GetCurSel()-1;

   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );
   if (pPresentation)
   {
      CWnd *pParent = GetParent();
      if (pParent)
         pParent->SendMessage(WM_REMOVE_ENTRY, 0, (LPARAM)pPresentation);
   }

   /*if(nSelectedItem >= 0)
      SelectItem(nSelectedItem);*/
}

void CRecordingsList::DeleteRecord() 
{
  // int nSelectedItem = GetCurSel()-1;

   if ( GetCurSel() < 0 )
      ASSERT(FALSE);
   CConfirmDelete dlgConfirmDelete(this); 
   UINT nMessage = dlgConfirmDelete.DoModal();
   if(nMessage == IDYES)
   {
      ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );
      if (pPresentation)
      {
         CWnd *pParent = GetParent();
         if (pParent)
            pParent->SendMessage(WM_DELETE_ENTRY, 0, (LPARAM)pPresentation);
      }
   }
   /*if(nSelectedItem >= 0)
      SelectItem(nSelectedItem);*/
}

void CRecordingsList::RecordProperties() 
{
   if ( GetCurSel() < 0 )
      ASSERT(FALSE);

   int nSelectedItem = GetCurSel();

   ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );
   if (pPresentation == NULL)
   {
      ASSERT(FALSE);
      return;
   }

   CString csTitle = pPresentation->GetTitle();
   CString csAuthor = pPresentation->GetAuthor();
   CString csProducer = pPresentation->GetCreator();
   CString csKeywords = pPresentation->GetKeywords();

   CDlgMetadata dlgMetadata(this); 

   CString csRecordDate = pPresentation->GetRecorddate();;
   CString csRecordTime = pPresentation->GetRecordtime();
   CString csRecordLength = pPresentation->GetRecordlength();
   CString csRecordPath = pPresentation->GetFilename();

   dlgMetadata.Init(csTitle, csAuthor, csProducer, csKeywords);
   dlgMetadata.SetRecordInfo(csRecordDate, csRecordTime, csRecordLength, csRecordPath);

   int rc = dlgMetadata.DoModal();
   if(rc == IDOK)
   {
      pPresentation->SetTitle(dlgMetadata.GetTitle());
      pPresentation->SetAuthor(dlgMetadata.GetAuthor());
      pPresentation->SetCreator(dlgMetadata.GetProducer());
      pPresentation->SetKeywords(dlgMetadata.GetKeywords());
      pPresentation->ReWriteLmdFile();
      pPresentation->ReWriteLrdFile();
   }
   if (pPresentation)
   {
      CWnd *pParent = GetParent();
      if (pParent)
         pParent->SendMessage(WM_PROPERTIES, 0, (LPARAM)pPresentation);
   }

   if(nSelectedItem >= 0)
      SelectItem(nSelectedItem);
}

void CRecordingsList::ExploreRecordingPath() {
    
    ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( GetCurSel() );
    if (pPresentation == NULL) {
        ASSERT(FALSE);
        return;
    }

    CString csRecFileName = pPresentation->GetFilename();

    int iPos = csRecFileName.ReverseFind(_T('\\'));
    CString csPath = _T("");
    if (iPos != -1) {
        csPath = csRecFileName.Left(iPos);
    }

    HANDLE hDir = CreateFile(csPath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
         NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hDir != INVALID_HANDLE_VALUE) {
        ShellExecute(NULL, _T("explore"), csPath, NULL, NULL, SW_SHOWNORMAL);
    }
    CloseHandle(hDir);
}

void CRecordingsList::RenameRecord(){
    ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr(GetCurSel());
    if (pPresentation == NULL){
        ASSERT(FALSE);
        return;
    }
    CDlgRecordRename dlgRecordRename(this, pPresentation->GetFilename());
    if(dlgRecordRename.DoModal() == IDOK){
        CString newName = dlgRecordRename.GetPath() + dlgRecordRename.GetName();
        newName.MakeLower();
        CString csExtension = newName.Right(newName.GetLength() - newName.ReverseFind('.'));
        if (csExtension != _T(".lrd")){
            newName = dlgRecordRename.GetPath() + dlgRecordRename.GetName() + _T(".lrd");
        }
        pPresentation->SetFilename(newName);

        CWnd *pParent = GetParent();
        if (pParent)
            pParent->SendMessage(WM_PROPERTIES, 0, (LPARAM)pPresentation);
    }
}
void CRecordingsList::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);
   CRect rcItem = lpDIS->rcItem;

   if ((lpDIS->itemID != (UINT)-1) && (lpDIS->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
   {
      COLORREF clrWindow = GetBackColor();
      COLORREF clrWindowText = IsWindowEnabled() ? GetXtremeColor(COLOR_WINDOWTEXT) : GetXtremeColor(COLOR_GRAYTEXT);
      BOOL bSelected = ((lpDIS->itemState & ODS_SELECTED) != 0);

      CRect rcText(rcItem);
      rcText.DeflateRect(m_nTextPadding, 2, 0, 0);

      CRect rcDetails(rcItem);
      rcDetails.DeflateRect(m_nTextPadding + 12, 17, 0, 5);

      if (bSelected)
      {
         clrWindow = IsWindowEnabled() ? GetXtremeColor(COLOR_HIGHLIGHT) : GetXtremeColor(COLOR_GRAYTEXT);
         clrWindowText = GetXtremeColor(COLOR_HIGHLIGHTTEXT);
      }

      if (m_nStyle == xtListBoxOfficeXP && bSelected && IsWindowEnabled())
      {
         clrWindowText = GetXtremeColor(XPCOLOR_HIGHLIGHT_TEXT);
         clrWindow = GetXtremeColor(XPCOLOR_HIGHLIGHT);
         pDC->Draw3dRect(rcItem, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER), GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
         rcItem.DeflateRect(1, 1);
      }

      if (m_nStyle == xtListBoxOffice2007)
      {
         BOOL bHasFocus = ::GetFocus() == m_hWnd;
         BOOL bHighlighted = (int)lpDIS->itemID == m_nHotItem;

         if ((bSelected || bHighlighted) && IsWindowEnabled())
         {
            CXTPOffice2007Image* pImage = XTPOffice2007Images()->LoadFile(_T("LISTBOX"));
            if (pImage)
            {
               pImage->DrawImage(pDC, rcItem, pImage->GetSource(bSelected && bHighlighted ? 2 :
                  bHasFocus && bSelected ? 1 : !bHasFocus && bSelected ? 3 : 0, 4), CRect(4, 4, 4, 4), COLORREF_NULL);
            clrWindow = COLORREF_NULL;
            }
         }

         pDC->SetBkMode(TRANSPARENT);
         clrWindowText = XTPOffice2007Images()->GetImageColor(_T("LISTBOX"), _T("NormalText"));
      }

      // set the text and text background colors, then repaint the item.
      pDC->SetBkColor(clrWindow);
      pDC->SetTextColor(clrWindowText);
      if (clrWindow != COLORREF_NULL) pDC->FillSolidRect(&rcItem, clrWindow);


      // If it is the first item draw gray rectangle.
      //if (lpDIS->itemID == 0 && !(lpDIS->itemState & ODS_SELECTED) && !((int)lpDIS->itemID == m_nHotItem) )
      //{
      //   CXTPOffice2007Image* pImage = XTPOffice2007Images()->LoadFile(_T("PUSHBUTTON"));
      //   if (pImage)
      //      pImage->DrawImage(pDC, rcItem, pImage->GetSource(3, 5), CRect(4, 4, 4, 4), COLORREF_NULL);
      //   pDC->SetBkMode(TRANSPARENT);
      //}

      CString strText;
      GetText(lpDIS->itemID, strText);

      // Draw image on item
      Gdiplus::Image* pImage = NULL;
      HINSTANCE hInstance = AfxGetResourceHandle();
      if ( hInstance != NULL )
      {
         HRSRC hRsrc = ::FindResource ( hInstance, MAKEINTRESOURCE(IDR_PNG_RECORDING_EDIT), _T("PNG")); 
         ASSERT(hRsrc != NULL);
         if ( hRsrc != NULL )
         {
            DWORD dwSize = ::SizeofResource( hInstance, hRsrc);
            LPBYTE lpRsrc = (LPBYTE)::LoadResource( hInstance, hRsrc);
            ASSERT(lpRsrc != NULL);

            HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
            if ( hMem != NULL )
            {
               LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
               memcpy( pMem, lpRsrc, dwSize);
               IStream * pStream = NULL;
               ::CreateStreamOnHGlobal( hMem, FALSE, &pStream);
               if ( pStream != NULL )
               {
                  pImage = Gdiplus::Image::FromStream(pStream);
                  ::GlobalUnlock(hMem);
                  ::GlobalFree(hMem);
                  pStream->Release();
                  ::FreeResource(lpRsrc);

                  if ( pImage != NULL )
                  {
                     Gdiplus::Graphics graphics(pDC->m_hDC);
                     graphics.DrawImage(pImage, rcItem.left + 4, rcItem.top, pImage->GetWidth(), pImage->GetWidth());
                  }
               }
               else
               {
                  ::GlobalUnlock(hMem);
                  ::GlobalFree(hMem);
                  ::FreeResource(lpRsrc);
               }
            }
         }
      }
      // delete pImage allocated by Gdiplus::Image::FromStream(pStream)
      SAFE_DELETE(pImage);

      CString strIndex;
      strIndex.Format(_T("%d  %s"), lpDIS->itemID + 1, strText);
      ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *)GetItemDataPtr(lpDIS->itemID);
      if ( pPresentation != NULL )
      {
         CFont *pWindowFont = XTPPaintManager()->GetRegularFont();
         if ( pWindowFont != NULL )
         {
            LOGFONT lf;
            pWindowFont->GetLogFont(&lf);
            HFONT hFontNew, hFontOld;
            hFontNew = CreateFontIndirect(&lf);
            if ( hFontNew != NULL )
            {
               hFontOld = (HFONT) SelectObject(pDC->m_hDC, hFontNew);
               if ( hFontOld != NULL )
               {
                  pDC->DrawText(strIndex, &rcText, DT_LEFT | DT_SINGLELINE | DT_EXPANDTABS);
                  SelectObject(pDC->m_hDC, hFontOld);
                  DeleteObject(hFontNew);

                  CString strDetails;
                  strDetails.Format(_T("%s\t%s %s"), pPresentation->GetRecordlength(), pPresentation->GetRecorddate(), pPresentation->GetRecordtime());
                  pDC->DrawText(strDetails, &rcDetails, DT_LEFT | DT_SINGLELINE | DT_EXPANDTABS);
               }
               else
                  DeleteObject(hFontNew);
            }
         }
      }
   }
   if ((lpDIS->itemAction & ODA_FOCUS) && (m_nStyle != xtListBoxOffice2007))
      pDC->DrawFocusRect(&lpDIS->rcItem);
}

void CRecordingsList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
   lpMeasureItemStruct->itemHeight = 35;
}

int CRecordingsList::GetSelectedItem() {
    int iRet = 1;
    if (GetSelCount() != 1) {
        iRet = -1;
    }
    return iRet;//GetCurSel();
}
LRESULT CRecordingsList::OnUpdateList(WPARAM wParam,LPARAM lParam)
{
   //if ( GetFocus() != this )
   //   SetCurSel(-1);
   return 0L;
}
void CRecordingsList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   if (nChar == VK_RETURN)
   {
      ReplayRecord();
   }
   CXTListBox::OnChar(nChar, nRepCnt, nFlags);
}

void CRecordingsList::CreateToolTips() {
    //EnableToolTips(TRUE);
    if (m_ctrlToolTipsContext !=NULL) {
        ASSERT(m_ctrlToolTipsContext);
        return;
    }
    m_ctrlToolTipsContext = new CXTPToolTipContext();
    //m_ctrlToolTipsContext->SetModuleToolTipContext();
    m_ctrlToolTipsContext->SetStyle(xtpToolTipOffice2007);
    m_ctrlToolTipsContext->SetMaxTipWidth(200);
    m_ctrlToolTipsContext->ShowTitleAndDescription();       
    m_ctrlToolTipsContext->ShowImage(TRUE,0);
    m_ctrlToolTipsContext->SetMargin(CRect(2, 2, 2, 2));  
    m_ctrlToolTipsContext->SetDelayTime(TTDT_INITIAL, 1300);
    m_ctrlToolTipsContext->ShowToolTipAlways();

    CMainFrameA* pMainFrame = CMainFrameA::GetCurrentInstance();
    if (pMainFrame)
        m_ctrlToolTipsContext->SetFont(pMainFrame->GetCommandBars()->GetPaintManager()->GetIconFont());

}

INT_PTR CRecordingsList::OnToolHitTest(CPoint point, TOOLINFO* pTI) const {
    // TODO: Add your specialized code here and/or call the base class
    if (GetCount() > 0) {
        CPoint ptPoint = point;
        //ScreenToClient(&ptPoint);

        INT_PTR nHit = (UINT)(WORD)GetDlgCtrlID();
        BOOL bOutside;
        UINT nIndex = ItemFromPoint(ptPoint, bOutside);
        CRect rcItem;
        GetItemRect(nIndex, &rcItem);

        CString csText;
        ASSISTANT::PresentationInfo *pPresentation = (ASSISTANT::PresentationInfo *) GetItemDataPtr( nIndex );
        csText = pPresentation->GetFilename();

        m_ctrlToolTipsContext->FillInToolInfo(pTI, GetSafeHwnd(), rcItem, nHit, csText);

        return nHit;
    }
    return -1;
}

BOOL CRecordingsList::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult) {
    ASSERT(pNMHDR->code == TTN_NEEDTEXT);

    TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

    if (!(pTTT->uFlags & TTF_IDISHWND))
        return FALSE;

    UINT_PTR hWnd = pNMHDR->idFrom;
    // idFrom is actually the HWND of the tool
    UINT nID = (UINT)(WORD)::GetDlgCtrlID((HWND)hWnd);
    INT_PTR nHit = (UINT)(WORD)::GetDlgCtrlID((HWND)hWnd);
    pTTT->lpszText = MAKEINTRESOURCE(nID);
    pTTT->hinst = AfxGetInstanceHandle();

    *pResult = 0;
    // bring the tooltip window above other popup windows
    ::SetWindowPos(pNMHDR->hwndFrom,  HWND_TOP, 0, 0, 0, 0,
        SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

    return TRUE;    // message was handled
}

BOOL CRecordingsList::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if(m_ctrlToolTipsContext!=NULL) {
        m_ctrlToolTipsContext->FilterToolTipMessage(this, pMsg);
        m_ctrlToolTipsContext->RelayToolTipMessage(this,pMsg);
    } 

    return CXTListBox::PreTranslateMessage(pMsg);
}

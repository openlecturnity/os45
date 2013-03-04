#include "stdafx.h"

#include "ControlLineWidthSelector.h"
#include "ControlLineStyleSelector.h"
#include "ControlArrowStyleSelector.h"
#include "GalleryItems.h"
#include "LanguageSupport.h"

#include "Studio.h"
#include "MainFrm.h"
#include "RibbonBars.h"
#include "..\Studio\Resource.h"	
static int aiFontSize [22] = { 8, 9, 10, 12, 14, 16, 18, 20, 24, 28, 32,
36, 40, 44, 48, 54, 60, 66, 72, 80, 88, 96};

CRibbonBars::CRibbonBars(void)
{
   m_pCommandBars =NULL;
   m_pRibbonBar = NULL;
   m_pTabStartAssistant = NULL;
   m_pTabStartEditor =NULL;

   m_pTabViewAssistant = NULL;
   m_pTabViewEditor =NULL;

   m_pTabModify = NULL;
   m_pTabInsert =NULL;

   m_bRibbonBarAVisibility = false;
   m_bRibbonBarEVisibility = false;
}

CRibbonBars::~CRibbonBars(void)
{
   // CXTPCommandBars will delete m_pRibbonBar as this pointer is containd in array CXTPCommandBars::m_arrBars
   // All private CXTPRibbonTab tabs will be deleted by CXTPRibbonBar.
}

//Create ribbon
BOOL CRibbonBars::CreateRibbonBars(CXTPCommandBars* pCommandBarsParam,HWND m_hWnd,  CMainFrame * pMainFrame)
{
   CXTPCommandBars* pCommandBars = pCommandBarsParam;
   pCommandBars->GetCommandBarsOptions()->bShowKeyboardTips = TRUE;
   pCommandBars->GetShortcutManager()->SetAccelerators(IDR_MAINFRAME);
   pCommandBars->GetShortcutManager()->UseSystemKeyNameText(TRUE);
   pCommandBars->GetCommandBarsOptions()->bShowContextMenuAccelerators =TRUE;
   pCommandBars->GetCommandBarsOptions()->bToolBarAccelTips = FALSE;
   pCommandBars->GetCommandBarsOptions()->bAutoHideUnusedPopups =TRUE;
   pCommandBars->GetToolTipContext()->SetMaxTipWidth(400);
   
   SetCommandBars(pCommandBarsParam);
   CMenu menu;
   menu.Attach(::GetMenu(m_hWnd));
   pMainFrame->SetMenu(NULL);


   m_pRibbonBar = (CXTPRibbonBar*)pCommandBars->Add(_T("The Ribbon"), xtpBarTop, RUNTIME_CLASS(CXTPRibbonBar));
   if (!m_pRibbonBar)
   {
      return FALSE; 
   }

   m_pRibbonBar->EnableDocking(0);

   //pRibbonBar->GetTabPaintManager()->m_bSelectOnButtonDown = FALSE;

   // Hide ribbon bar in LIVECONTEXT preview mode 
   if (CStudioApp::IsLiveContextMode() && CStudioApp::IsLiveContextPreview()) { 
       m_pRibbonBar->SetVisible(FALSE);
   }

   //Create System menu
   CXTPControlPopup* pControlFile = (CXTPControlPopup*)m_pRibbonBar->AddSystemButton(IDS_SYSMENU);
   pControlFile->SetCaption(_T("File"));
   pControlFile->SetCommandBar(menu.GetSubMenu(0));
   pControlFile->GetCommandBar()->SetWidth(250);
   pControlFile->SetIconId(IDB_GEAR_ASSISTANT);
   pControlFile->GetCommandBar()->SetIconSize(CSize(32, 32));
   pCommandBars->GetImageManager()->SetIcons(IDS_SYSMENU);

   CreateTabStartAssistant(m_pRibbonBar);
   CreateTabTools(m_pRibbonBar);
   CreateTabViewAssistant(m_pRibbonBar);

   CreateTabStartEditor(m_pRibbonBar);
   CreateTabInsert(m_pRibbonBar);
   CreateTabViewEditor(m_pRibbonBar);
   LoadRibbonIconsAssistant();
   LoadRibbonIconsEditor();

   UINT uQuickAccessBar[] = {ID_FILE_NEW,ID_LSD_OPEN,ID_LSD_SAVE,ID_LSD_NEW,ID_PAGE_NEW,-1,-1,-1,-1,ID_EDIT_UNDO,-1,ID_EDIT_REDO/*,-1,-1,ID_EDIT_UNDO,-1,ID_EDIT_REDO*/};
   m_pRibbonBar->GetQuickAccessControls()->GetCommandBars()->GetImageManager()->SetIcons(IDR_PNG_QUICKACCES, uQuickAccessBar, _countof(uQuickAccessBar), CSize(16, 16));
   m_pRibbonBar->AllowQuickAccessDuplicates(FALSE);

   //Create "About" button
   UINT x = menu.GetMenuItemCount();
   CXTPControlPopup* pControlAbout = (CXTPControlPopup*)m_pRibbonBar->GetControls()->Add(xtpControlButtonPopup, ID_APP_ABOUT);
   pControlAbout->SetFlags(xtpFlagRightAlign);
   pControlAbout->SetCommandBar(menu.GetSubMenu(menu.GetMenuItemCount() - 1));

   //Hide menu entry "Help" if no manual exists
   //Hide menu entry "Buy Now" if full version
   bool bHasManual = CLanguageSupport::ManualExists();
   CString csOwnVersion;
   csOwnVersion.LoadString(IDS_VERSION);
   CSecurityUtils secure;
   int iVersionType = secure.GetVersionType(csOwnVersion);
   bool bIsEvaluation = (iVersionType & EVALUATION_VERSION) != 0;

   bool bHideMenuEntries = !bHasManual || !bIsEvaluation;
   if(bHideMenuEntries)
   {
      CXTPControls* pMenuControls = pControlAbout->GetCommandBar()->GetControls();
      for(int i = 0; i < pMenuControls->GetCount(); i++)
      {
         CXTPControl* pControl = pMenuControls->GetAt(i);
         if(pControl)
         {
             int nId = pControl->GetID();
             bool bNeedsToBeHidden = ( nId == ID_HELP && !bHasManual) || (nId == IDS_SHOW_IMC_WEBPAGE_BUYNOW && !bIsEvaluation);
             if(bNeedsToBeHidden)
             {
                 pControl->SetHideFlags(xtpHideGeneric);
                 pControl->SetVisible(FALSE);
             }
         }
      }
   }

   //Frame Theme
   m_pRibbonBar->SetCloseable(FALSE);
   m_pRibbonBar->EnableFrameTheme();

   return TRUE;
}

//Create Start tab
void CRibbonBars::CreateTabStartAssistant(CXTPRibbonBar* pRibbon)
{
   m_pTabStartAssistant = pRibbon->AddTab(ID_TAB_START);

   // Group Recorder
   CXTPRibbonGroup* pGroupRecorder = m_pTabStartAssistant->AddGroup(ID_GROUP_RECORDER);
   // Add Start/Stop Record button
   CXTPControl* pRecordAction = pGroupRecorder->Add(xtpControlButton, /*ID_RECORDER_START_STOP */ID_START_RECORDING);
   pRecordAction->SetStyle(xtpButtonIconAndCaptionBelow);
   pRecordAction->SetCaption(ID_START_RECORDING);
   pRecordAction->SetKeyboardTip(_T("R"));
   // Add Stop Record button
   CXTPControl* pRecordStopAction = pGroupRecorder->Add(xtpControlButton,  ID_STOP_RECORDING);
   pRecordStopAction->SetStyle(xtpButtonIconAndCaptionBelow);
   pRecordStopAction->SetCaption(ID_STOP_RECORDING);
   AddTooltip(pRecordStopAction, IDS_ID_STOP_RECORDING);
   // Add Pause recording button
   CXTPControl* pPauseRecording = pGroupRecorder->Add(xtpControlButton, ID_PAUSE_RECORDING);
   pPauseRecording->SetStyle(xtpButtonIconAndCaptionBelow);
   pPauseRecording->SetCaption(ID_PAUSE_RECORDING);
   AddTooltip(pPauseRecording, IDS_ID_PAUSE_RECORDING);

   // Add Screen Grabbing button
   CXTPControl* pStartScreenGrabbing = pGroupRecorder->Add(xtpControlButton, ID_TOOL_SCREENGRABBING);
   pStartScreenGrabbing->SetStyle(xtpButtonIconAndCaptionBelow);
   //CString csStartSgTitle;
   //csStartSgTitle.LoadString(ID_GROUP_SCREENGRABBING);
   pStartScreenGrabbing->SetCaption(ID_TOOL_SCREENGRABBING);
   // Group Recorder end

   // Group ImportPowerPoint
   CXTPRibbonGroup* pGroupPowerPoint = m_pTabStartAssistant->AddGroup(ID_GROUP_IMPORT_POWERPOINT);
   // Add Import PowerPoint button
   CXTPControl* pImportPowerPointAction = pGroupPowerPoint->Add(xtpControlButton, ID_PPT_IMPORT);
   pGroupPowerPoint->SetControlsCentering();
   pImportPowerPointAction->SetStyle(xtpButtonIconAndCaptionBelow);
   pImportPowerPointAction->SetCaption(ID_PPT_IMPORT);
   AddTooltip(pImportPowerPointAction, IDS_ID_PPT_IMPORT);

   // Group Start Presentation
   CXTPRibbonGroup* pGroupStartPresentation = m_pTabStartAssistant->AddGroup(ID_GROUP_START_PRESENTATION);
   pGroupStartPresentation->SetControlsCentering();
   // Add Start From Current Slide button
   CXTPControl* pControlStartFromBeggining = pGroupStartPresentation->Add(xtpControlButton, ID_FULL_SCREEN);
   pControlStartFromBeggining->SetStyle(xtpButtonIconAndCaptionBelow);
   pControlStartFromBeggining->SetCaption(ID_FULL_SCREEN);
   AddTooltip(pControlStartFromBeggining, IDS_ID_FULL_SCREEN);
   // Add Start From Beggining button
   CXTPControl* pControlStartFromCurrentSlide = pGroupStartPresentation->Add(xtpControlButton, ID_FULL_SCREEN_CS);
   pControlStartFromCurrentSlide->SetStyle(xtpButtonIconAndCaptionBelow);
   pControlStartFromCurrentSlide->SetCaption(ID_FULL_SCREEN_CS);
   AddTooltip(pControlStartFromCurrentSlide, IDS_ID_FULL_SCREEN_CS);
   // End Group Start Presentation

   // Group Presentation-Tools
   CXTPRibbonGroup* pGroupPresentation = m_pTabStartAssistant->AddGroup(ID_GROUP_PRESENTATION_TOOLS);
   pGroupPresentation->SetControlsCentering();

   // Create Markers popup button with gallery
   // Create Menu for Markers button. Markers button will have a menu attached.
   CMenu menuMarkers;
   menuMarkers.LoadMenu(ID_MENU_MARKERS);

   CXTPControlPopup* pControlMarkers = (CXTPControlPopup*)pGroupPresentation->Add(xtpControlSplitButtonPopup, ID_BUTTON_MARKERS);
   pControlMarkers->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlMarkers, IDS_TOOLTIP_MARKERS);

   CXTPPopupBar *pPopupBarMarkers = CXTPPopupBar::CreatePopupBar(GetCommandBars());
   pPopupBarMarkers->LoadMenu(menuMarkers.GetSubMenu(0));

   pControlMarkers->SetCommandBar(pPopupBarMarkers);
   pPopupBarMarkers->InternalRelease();
   //End Create Markers popup button with gallery

   // Create Pens popup button with gallery
   // Create Menu for pens button. Pens button will have a menu attached.
   CMenu menuPens;
   menuPens.LoadMenu(ID_MENU_PENS);

   CXTPControlPopup* pControlPens = (CXTPControlPopup*)pGroupPresentation->Add(xtpControlSplitButtonPopup, ID_BUTTON_PENS);
   pControlPens->SetIconSize(CSize(32, 32));
   pControlPens->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlPens, IDS_TOOLTIP_PENS);

   CXTPPopupBar* pPopupBarPens = CXTPPopupBar::CreatePopupBar(GetCommandBars());
   pPopupBarPens->LoadMenu(menuPens.GetSubMenu(0));

   pControlPens->SetCommandBar(pPopupBarPens);
   pPopupBarPens->InternalRelease();
   //End Create Pens popup button with gallery


   // Add Pointer button
   CXTPControl *pPointer = pGroupPresentation->Add(xtpControlButton, ID_TOOL_TELEPOINTER);
   pPointer->SetStyle(xtpButtonIconAndCaptionBelow);
   pPointer->SetCaption(ID_TOOL_TELEPOINTER);
   AddTooltip(pPointer, IDS_ID_TOOL_TELEPOINTER);
   // Add SimpleNavigation button
   CXTPControl *pSimplifiedNavigation = pGroupPresentation->Add(xtpControlButton, ID_TOOL_SIMPLENAVIGATION);
   pSimplifiedNavigation->SetStyle(xtpButtonIconAndCaptionBelow);
   pSimplifiedNavigation->SetCaption(ID_TOOL_SIMPLENAVIGATION);
   AddTooltip(pSimplifiedNavigation, IDS_ID_TOOL_SIMPLENAVIGATION);
   //group Presentation-Tools end

   // Group Recorder
   //CXTPRibbonGroup* pGroupScreenGrabbing = m_pTabStartAssistant->AddGroup(ID_GROUP_SCREENGRABBING);
   //pGroupScreenGrabbing->ShowOptionButton();
   //pGroupScreenGrabbing->GetControlGroupOption()->SetID(ID_CHANGE_SG_OPTIONS);
   //pGroupScreenGrabbing->SetControlsCentering();

   //// Add Screen Grabbing button
   //CXTPControl* pStartScreenGrabbing = pGroupScreenGrabbing->Add(xtpControlButton, ID_TOOL_SCREENGRABBING);
   //pStartScreenGrabbing->SetStyle(xtpButtonIconAndCaptionBelow);

   // Group Insert
   CXTPRibbonGroup* pGroupInsert = m_pTabStartAssistant->AddGroup(ID_GROUP_INSERT);
   pGroupInsert->SetControlsCentering();
   CXTPControl* pControlGrInsert;
   pControlGrInsert = pGroupInsert->Add(xtpControlButton, ID_LSD_NEW);
   pControlGrInsert->SetCaption(ID_LSD_NEW);
   AddTooltip(pControlGrInsert, IDS_ID_LSD_NEW);
   CXTPControl* pBnp = pGroupInsert->Add(xtpControlButton, ID_PAGE_NEW);
   pBnp->SetCaption(ID_PAGE_NEW);
   AddTooltip(pBnp, IDS_ID_PAGE_NEW);
   pControlGrInsert = pGroupInsert->Add(xtpControlButton, ID_CHAPTER_NEW);
   pControlGrInsert->SetCaption(ID_CHAPTER_NEW);
   AddTooltip(pControlGrInsert, IDS_ID_CHAPTER_NEW);

}

//Create View tab
void CRibbonBars::CreateTabViewAssistant(CXTPRibbonBar* pRibbon)
{
   m_pTabViewAssistant = pRibbon->AddTab(ID_TAB_VIEW);

   // Group View
   CXTPRibbonGroup* pGroupView = m_pTabViewAssistant->AddGroup(ID_GROUP_VIEW);

   // Add Standard Layout button
   CXTPControl* pControlStandardLayout = pGroupView->Add(xtpControlButton, ID_STANDARD_LAYOUT);
   pControlStandardLayout->SetStyle(xtpButtonIconAndCaptionBelow);
   pControlStandardLayout->SetCaption(ID_STANDARD_LAYOUT);
   AddTooltip(pControlStandardLayout, IDS_STANDARD_LAYOUT);

   // Add Page Focused Layout button
   CXTPControl* pControlPageFocusedLayout = pGroupView->Add(xtpControlButton, ID_PAGE_FOCUSED_LAYOUT);
   pControlPageFocusedLayout->SetStyle(xtpButtonIconAndCaptionBelow);
   pControlPageFocusedLayout->SetCaption(ID_PAGE_FOCUSED_LAYOUT);
   AddTooltip(pControlPageFocusedLayout, IDS_PAGE_FOCUSED_LAYOUT);

   // Add Presentation Popup Button
   CXTPControlPopup *pControlPresentation = CXTPControlPopup::CreateControlPopup(xtpControlSplitButtonPopup/*xtpControlButtonPopup*/);
   CXTPPopupBar* pPresentationBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   CMenu menu;
   menu.LoadMenu(IDR_SUBMENU_VIEW_PRESENTATION);

   pPresentationBar->SetCommandBars(GetCommandBars());
   pPresentationBar->LoadMenu(menu.GetSubMenu(0));
   pPresentationBar->SetShowGripper(FALSE);
   pPresentationBar->SetDefaultButtonStyle(xtpButtonIconAndCaption);

   pControlPresentation->SetCommandBar(pPresentationBar);

   pPresentationBar->EnableCustomization(FALSE);
   pPresentationBar->InternalRelease(); 
   pPresentationBar->SetIconSize(CSize(16,16));

   pGroupView->Add(pControlPresentation, ID_VIEW_PRESENTATION)->SetStyle(xtpButtonIconAndCaptionBelow);
   pControlPresentation->SetCaption(ID_VIEW_PRESENTATION);
   AddTooltip(pControlPresentation, IDS_VIEW_PRESENTATION);
   // End Group View

   // Group Show Hide
   CXTPRibbonGroup* pGroupShowHide = m_pTabViewAssistant->AddGroup(ID_GROUP_SHOW_HIDE);

   // Add Show Document Structure Check Box
   CXTPControl* pControlShowDocumentStructure = pGroupShowHide->Add(xtpControlCheckBox, ID_SHOW_STRUCTURE);
   pControlShowDocumentStructure->SetFlags(xtpFlagRightAlign);
   pControlShowDocumentStructure->SetCaption(ID_SHOW_STRUCTURE);
   AddTooltip(pControlShowDocumentStructure, IDS_SHOW_STRUCTURE);
   //pControlDocumentStructure->SetStyle();

   // Add Show Recordings Check Box
   CXTPControl* pControlShowRecordings = pGroupShowHide->Add(xtpControlCheckBox, ID_SHOW_RECORDINGS);
   pControlShowRecordings->SetFlags(xtpFlagRightAlign);
   pControlShowRecordings->SetCaption(ID_SHOW_RECORDINGS);
   AddTooltip(pControlShowRecordings, IDS_SHOW_RECORDINGS);

   // Add Show Status Bar Check Box
   CXTPControl* pControlShowStatusBar = pGroupShowHide->Add(xtpControlCheckBox, ID_VIEW_STATUS_BAR);
   pControlShowStatusBar->SetFlags(xtpFlagRightAlign);
   pControlShowStatusBar->SetCaption(IDS_VIEW_STATUS_BAR);
   AddTooltip(pControlShowStatusBar, IDS_VIEW_STATUS_BAR);

   // Add Show Monitor Widget Check Box
   CXTPControl* pControlShowMonitorWidget = pGroupShowHide->Add(xtpControlCheckBox, ID_VIEW_MONITOR_WIDGET);
   pControlShowMonitorWidget->SetFlags(xtpFlagRightAlign);
   pControlShowMonitorWidget->SetCaption(ID_VIEW_MONITOR_WIDGET);
   AddTooltip(pControlShowMonitorWidget, IDS_VIEW_MONITOR_WIDGET);

   //Add Show Startup Page checkbox
   CXTPControl* pControlShowStartupPage = pGroupShowHide->Add(xtpControlCheckBox, ID_VIEW_SHOW_STARTUP_PAGE);
   //pControlShowStartupPage->SetFlags(xtpFlagRightAlign);
   pControlShowStartupPage->SetCaption(ID_VIEW_SHOW_STARTUP_PAGE);
   AddTooltip(pControlShowStartupPage, IDS_SHOW_STARTUPPAGE);
   // End Group Show Hide

   // Group Presentation
   CXTPRibbonGroup* pGroupPresentation = m_pTabViewAssistant->AddGroup(ID_GROUP_PRESENTATION);
   //pGroupPresentation->ShowOptionButton();
   //pGroupPresentation->GetControlGroupOption()->SetID(ID_MONITOR_OPTION);

   // Add Display Presentation Bar Check Box
   CXTPControl* pControlDisplayPresentationBar = pGroupPresentation->Add(xtpControlCheckBox, ID_PRESENTATION_BAR_SHOW);
   pControlDisplayPresentationBar->SetFlags(xtpFlagRightAlign);
   pControlDisplayPresentationBar->SetCaption(ID_PRESENTATION_BAR_SHOW);
   AddTooltip(pControlDisplayPresentationBar, IDS_ID_PRESENTATION_BAR_SHOW);
   //pControlDocumentStructure->SetStyle();

   // Add Auto Hide Presentation Bar Check Box
   CXTPControl* pControlAutoHidePresentationBar = pGroupPresentation->Add(xtpControlCheckBox, ID_PRESENTATION_BAR_AUTOHIDE);
   pControlAutoHidePresentationBar->SetFlags(xtpFlagRightAlign);
   pControlAutoHidePresentationBar->SetCaption(ID_PRESENTATION_BAR_AUTOHIDE);
   AddTooltip(pControlAutoHidePresentationBar, IDS_ID_PRESENTATION_BAR_AUTOHIDE);

   // Add Hide Mouse Cursor Within Page View Check Box
   CXTPControl* pControlHideMouseCursor = pGroupPresentation->Add(xtpControlCheckBox, ID_HIDE_MOUSE_POINTER);
   pControlHideMouseCursor->SetFlags(xtpFlagRightAlign);

   pControlHideMouseCursor->SetCaption(ID_HIDE_MOUSE_POINTER);
   AddTooltip(pControlHideMouseCursor, IDS_ID_HIDE_MOUSE_POINTER);
   pControlHideMouseCursor->SetChecked(true);
   // End Group Presentation


   // Group Grid
   CXTPRibbonGroup* pGroupGrid = m_pTabViewAssistant->AddGroup(ID_GROUP_GRID);

   // Add Display Grid Check Box
   CXTPControl* pControlDisplayGrid = pGroupGrid->Add(xtpControlCheckBox, ID_GRID_DISPLAY);
   pControlDisplayGrid->SetFlags(xtpFlagNoMovable);
   pControlDisplayGrid->SetCaption(ID_GRID_DISPLAY);
   AddTooltip(pControlDisplayGrid, IDS_ID_GRID_DISPLAY);
   pControlDisplayGrid->SetHeight(40);

   // Add Grid Spacping label
   CXTPControl* pControlGridSpacingLabel = pGroupGrid->Add(xtpControlLabel, IDS_GRID_SPACING);
   pControlGridSpacingLabel->SetFlags(xtpFlagRightAlign | xtpFlagNoMovable);
   AddTooltip(pControlGridSpacingLabel, IDS_IDS_GRID_SPACING);

   // Add Snap to Grid Check Box
   CXTPControl* pControlSnapToGridCheckBox = pGroupGrid->Add(xtpControlCheckBox, ID_GRID_SNAP);
   pControlSnapToGridCheckBox->SetFlags(xtpFlagNoMovable);
   pControlSnapToGridCheckBox->SetCaption(ID_GRID_SNAP);
   AddTooltip(pControlSnapToGridCheckBox, IDS_ID_GRID_SNAP);
   pControlSnapToGridCheckBox->SetHeight(40);

   // Add Grid spacing edit control
   CXTPControlEdit* pControlEditSpacing = (CXTPControlEdit*)pGroupGrid->Add(xtpControlEdit, ID_GRID_SPACING);
   AddTooltip(pControlEditSpacing, IDS_ID_GRID_SPACING);
   pControlEditSpacing->SetWidth(60);
   pControlEditSpacing->ShowSpinButtons();

   // End Group Grid


   // Group Lock Elements
   CXTPRibbonGroup* pGroupLockElements = m_pTabViewAssistant->AddGroup(ID_GROUP_LOCK_ELEMENTS);

   // Add Lock Images Check Box
   CXTPControl* pControlLockImages = pGroupLockElements->Add(xtpControlCheckBox, ID_FREEZE_IMAGES);
   pControlLockImages->SetFlags(xtpFlagRightAlign);
   pControlLockImages->SetCaption(ID_FREEZE_IMAGES);
   AddTooltip(pControlLockImages, IDS_FREEZE_IMAGES);
   //pControlDocumentStructure->SetStyle();

   // Add Lock Slide Master Elements Check Box
   CXTPControl* pControlLockSlideMasterElements = pGroupLockElements->Add(xtpControlCheckBox, ID_FREEZE_MASTER);
   pControlLockSlideMasterElements->SetFlags(xtpFlagRightAlign);
   pControlLockSlideMasterElements->SetCaption(ID_FREEZE_MASTER);
   AddTooltip(pControlLockSlideMasterElements, IDS_FREEZE_MASTER);

   // Add Lock All Imported Objects Check Box
   CXTPControl* pControlLockImportedObjects = pGroupLockElements->Add(xtpControlCheckBox, IDC_FREEZE_DOCUMENT);
   pControlLockImportedObjects->SetFlags(xtpFlagRightAlign);
   pControlLockImportedObjects->SetCaption(IDC_FREEZE_DOCUMENT);
   AddTooltip(pControlLockImportedObjects, IDS_FREEZE_DOCUMENT);
   // End Group Lock Elements
}

//Create Tools tab
void CRibbonBars::CreateTabTools(CXTPRibbonBar* pRibbon)
{
   CXTPCommandBars* pCommandBars = GetCommandBars();
   m_pTabTools = pRibbon->AddTab(ID_TAB_TOOLS);

   // Group Clipboard
   CXTPRibbonGroup* pGroupClipboard = m_pTabTools->AddGroup(ID_GROUP_CLIPBOARD);
   pGroupClipboard->SetControlsCentering();

   CXTPControl* pControlPaste = (CXTPControl*)pGroupClipboard->Add(xtpControlButton, ID_EDIT_PASTE);
   pControlPaste->SetCaption(ID_EDIT_PASTE);
   AddTooltip(pControlPaste, IDS_ID_EDIT_PASTE);
   pControlPaste->SetKeyboardTip(_T("V"));

   CXTPControl* pControlCut = pGroupClipboard->Add(xtpControlButton, ID_EDIT_CUT);
   pControlCut->SetKeyboardTip(_T("X"));
   pControlCut->SetStyle(xtpButtonIconAndCaption);
   pControlCut->SetIconSize(CSize(16, 16));
   pControlCut->SetCaption(ID_EDIT_CUT);
   AddTooltip(pControlCut, IDS_ID_EDIT_CUT);

   CXTPControl* pControlCopy = pGroupClipboard->Add(xtpControlButton, ID_EDIT_COPY);
   pControlCopy->SetKeyboardTip(_T("C"));
   pControlCopy->SetStyle(xtpButtonIconAndCaption);
   pControlCopy->SetIconSize(CSize(16, 16));
   pControlCopy->SetCaption(ID_EDIT_COPY);
   AddTooltip(pControlCopy, IDS_ID_EDIT_COPY);

   CXTPControl* pControlCopy1 = pGroupClipboard->Add(xtpControlButton, ID_TOOL_COPY);
   pControlCopy->SetKeyboardTip(_T("T"));
   pControlCopy1->SetStyle(xtpButtonIconAndCaption);
   pControlCopy1->SetIconSize(CSize(16, 16));
   pControlCopy1->SetCaption(ID_TOOL_COPY);
   AddTooltip(pControlCopy1, IDS_ID_TOOL_COPY);

   // Group Font
   // Font family
   CXTPRibbonGroup* pGroupFont = m_pTabTools->AddGroup(ID_GROUP_FONT);
   pGroupFont->SetControlsGrouping();

   CXTPControlComboBox* pFontCombo = new CXTPControlComboBox();
   pFontCombo->SetDropDownListStyle();
   pFontCombo->EnableAutoComplete();
   pFontCombo->SetWidth(163);
   pGroupFont->Add(pFontCombo, ID_FONTFAMILY);
   AddTooltip(pFontCombo, IDS_ID_FONTFAMILY);

   CXTPPopupBar* pPopupBar = CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(pCommandBars);
   pFontCombo->SetCommandBar(pPopupBar);

   CXTPControlGallery *pControlGallery = new CXTPControlGallery();
   pControlGallery->SetControlSize(CSize(290, 508));
   pControlGallery->SetResizable(FALSE, TRUE);
   pControlGallery->ShowLabels(TRUE);


   CXTPControlGalleryItems* pItemsFontFace = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_FONT_FAMILY_LIST);
   pItemsFontFace->SetItemSize(CSize(0, 26));
   CGalleryItemFontFace::AddFontItems(pItemsFontFace);


   pControlGallery->SetItems(pItemsFontFace);
   pPopupBar->GetControls()->Add(pControlGallery, ID_FONT_FAMILY_LIST);
   pPopupBar->InternalRelease();

   // Font style
   CXTPControl* pFontStyleWeight = pGroupFont->Add(xtpControlButton, ID_FONTWEIGHT);
   AddTooltip(pFontStyleWeight, IDS_ID_FONTWEIGHT);
   pFontStyleWeight->SetBeginGroup(true);
   CXTPControl* pFontStyleSlant = pGroupFont->Add(xtpControlButton, ID_FONTSLANT);
   AddTooltip(pFontStyleSlant, IDS_ID_FONTSLANT);
   CXTPControl* pFontStyleIncrease = pGroupFont->Add(xtpControlButton, ID_FONT_SIZE_INCREASE);
   AddTooltip(pFontStyleIncrease, IDS_ID_FONT_SIZE_INCREASE);
   CXTPControl* pFontStyleDecrease = pGroupFont->Add(xtpControlButton, ID_FONT_SIZE_DECREASE);
   AddTooltip(pFontStyleDecrease, IDS_ID_FONT_SIZE_DECREASE);

   // Text color
   CXTPControlPopupColor* pPopupColorTxt = new CXTPControlPopupColor();
   pPopupColorTxt->SetBeginGroup(true);
   CXTPPopupBar* pColorBarTxt = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   pColorBarTxt->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_TEXT_COLOR);
   CXTPControlColorSelector *pControlColorSelectorTxt = new CXTPControlColorSelector();
   pControlColorSelectorTxt->SetBeginGroup(TRUE);
   pColorBarTxt->GetControls()->Add(pControlColorSelectorTxt, ID_SELECTOR_TEXT_COLOR);
   CXTPControlButtonColor *pControlButtonColorTxt = new CXTPControlButtonColor();
   pControlButtonColorTxt->SetBeginGroup(TRUE);
   pColorBarTxt->GetControls()->Add(pControlButtonColorTxt, IDS_MORE_TEXT_COLORS);

   pPopupColorTxt->SetCommandBar(pColorBarTxt);

   pColorBarTxt->SetTearOffPopup(_T("Text Color"), IDR_TEXTCOLOR_POPUP, 0);
   pColorBarTxt->EnableCustomization(FALSE);
   pColorBarTxt->InternalRelease();

   pGroupFont->Add(pPopupColorTxt, ID_TEXT_COLOR);
   AddTooltip(pPopupColorTxt, IDS_ID_TEXT_COLOR);


   // Text size
   CXTPControlComboBox* pComboSize = new CXTPControlComboBox();
   pComboSize->SetBeginGroup(true);
   pComboSize->SetDropDownListStyle();
   pComboSize->SetWidth(35);
   pComboSize->SetEditText(_T("10"));
   pGroupFont->Add(pComboSize, ID_FONTSIZE);
   AddTooltip(pComboSize, IDS_ID_FONTSIZE);

   pPopupBar = CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(pCommandBars);
   pComboSize->SetCommandBar(pPopupBar);

   pControlGallery = new CXTPControlGallery();
   pControlGallery->SetControlSize(CSize(42, 16 * 17));
   pControlGallery->SetResizable(FALSE, TRUE);

   CXTPControlGalleryItems* pItemsFontSize = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_FONT_SIZE_LIST);
   pItemsFontSize->SetItemSize(CSize(0, 17));
   for(int i = 0; i < 22; i++)
   {
      CString s;
      s.Format(_T("%d"), aiFontSize[i]);
      pItemsFontSize->AddItem(s);
   }

   pControlGallery->SetItems(pItemsFontSize);

   pPopupBar->GetControls()->Add(pControlGallery, ID_FONT_SIZE_LIST);
   pPopupBar->InternalRelease();	

   // Group Styles
   // Fill color
   CXTPRibbonGroup* pGroupStyles = m_pTabTools->AddGroup(ID_GROUP_STYLES);
   pGroupStyles->SetControlsCentering();

   CXTPControlPopupColor* pPopupColor = new CXTPControlPopupColor();
   CXTPPopupBar* pColorBar= (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   pColorBar->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_FILL_COLOR);
   CXTPControlColorSelector *pControlColorSelector = new CXTPControlColorSelector();
   pControlColorSelector->SetBeginGroup(TRUE);
   pColorBar->GetControls()->Add(pControlColorSelector, ID_SELECTOR_FILL_COLOR);
   CXTPControlButtonColor *pControlButtonColor = new CXTPControlButtonColor();
   pControlButtonColor->SetBeginGroup(TRUE);
   pColorBar->GetControls()->Add(pControlButtonColor, IDS_MORE_FILL_COLORS);

   pPopupColor->SetCommandBar(pColorBar);

   pColorBar->SetTearOffPopup(_T("Fill Color"), IDR_FILLCOLOR_POPUP, 0);
   pColorBar->EnableCustomization(FALSE);
   pColorBar->InternalRelease();

   pGroupStyles->Add(pPopupColor, ID_FILL_COLOR);
   AddTooltip(pPopupColor, IDS_ID_FILL_COLOR);

   // Line color
   CXTPControlPopupColor* pPopupColorLine = new CXTPControlPopupColor();
   CXTPPopupBar* pColorBarLine = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   //pColorBarLine->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_LINE_COLOR);
   CXTPControlColorSelector *pControlColorSelectorLine = new CXTPControlColorSelector();
   pControlColorSelectorLine->SetBeginGroup(TRUE);
   pColorBarLine->GetControls()->Add(pControlColorSelectorLine, ID_SELECTOR_LINE_COLOR);
   CXTPControlButtonColor *pControlButtonColorLine = new CXTPControlButtonColor();
   pControlButtonColorLine->SetBeginGroup(TRUE);
   pColorBarLine->GetControls()->Add(pControlButtonColorLine, IDS_MORE_LINE_COLORS);

   pPopupColorLine->SetCommandBar(pColorBarLine);

   pColorBarLine->SetTearOffPopup(_T("Line Color"), IDR_LINECOLOR_POPUP, 0);
   pColorBarLine->EnableCustomization(FALSE);
   pColorBarLine->InternalRelease();

   pGroupStyles->Add(pPopupColorLine, ID_LINE_COLOR);
   AddTooltip(pPopupColorLine, IDS_ID_LINE_COLOR);

   // Line width
   CXTPControlPopup *pControlPopupLineWidth = CXTPControlPopup::CreateControlPopup(xtpControlPopup);
   CXTPPopupBar* pLineWidthBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   CControlLineWidthSelector *pControlLineWidthSelector = new CControlLineWidthSelector();
   pControlLineWidthSelector->SetBeginGroup(TRUE);
   pLineWidthBar->GetControls()->Add(pControlLineWidthSelector, ID_SELECTOR_LINE_WIDTH);
   pControlPopupLineWidth->SetCommandBar(pLineWidthBar);

   pLineWidthBar->EnableCustomization(FALSE);
   pLineWidthBar->InternalRelease(); 
   pGroupStyles->Add(pControlPopupLineWidth, ID_LINE_WIDTH)->SetStyle(xtpButtonIcon);
   AddTooltip(pControlPopupLineWidth, IDS_ID_LINE_WIDTH);

   // Line style
   CXTPControlPopup *pControlPopupLineStyle = CXTPControlPopup::CreateControlPopup(xtpControlPopup);
   CXTPPopupBar* pLineStyleBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   CControlLineStyleSelector *pControlLineStyleSelector = new CControlLineStyleSelector();
   pLineStyleBar->GetControls()->Add(pControlLineStyleSelector, ID_SELECTOR_LINE_STYLE);
   pControlPopupLineStyle->SetCommandBar(pLineStyleBar);

   pLineStyleBar->EnableCustomization(FALSE);
   pLineStyleBar->InternalRelease();
   pGroupStyles->Add(pControlPopupLineStyle, ID_LINE_STYLE)->SetStyle(xtpButtonIcon);
   AddTooltip(pControlPopupLineStyle, IDS_ID_LINE_STYLE);

   // Arrow style
   CXTPControlPopup *pControlPopupArrowStyle = CXTPControlPopup::CreateControlPopup(xtpControlPopup);
   CXTPPopupBar* pArrowStyleBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   CControlArrowStyleSelector *pControlArrowStyleSelector = new CControlArrowStyleSelector();
   pArrowStyleBar->GetControls()->Add(pControlArrowStyleSelector, ID_SELECTOR_ARROW_STYLE);
   pControlPopupArrowStyle->SetCommandBar(pArrowStyleBar);

   pArrowStyleBar->EnableCustomization(FALSE);
   pArrowStyleBar->InternalRelease();
   pGroupStyles->Add(pControlPopupArrowStyle, ID_ARROW_STYLE)->SetStyle(xtpButtonIcon);
   AddTooltip(pControlPopupArrowStyle, IDS_ID_ARROW_STYLE);
   // Group Lines
   CXTPRibbonGroup* pGroupLines = m_pTabTools->AddGroup(ID_GROUP_LINES);
   pGroupLines->SetControlsCentering();

   CXTPControl* pControl;
   pControl = pGroupLines->Add(xtpControlButton, ID_TOOL_LINE);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetCaption(ID_TOOL_LINE);
   AddTooltip(pControl, IDS_ID_TOOL_LINE);
   pControl->SetIconSize(CSize(16, 16));
   pControl = pGroupLines->Add(xtpControlButton, ID_TOOL_POLYLINE);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetCaption(ID_TOOL_POLYLINE);
   AddTooltip(pControl, IDS_ID_TOOL_POLYLINE);
   pControl->SetIconSize(CSize(16, 16));
   pControl = pGroupLines->Add(xtpControlButton, ID_TOOL_FREEHAND);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetCaption(ID_TOOL_FREEHAND);
   pControl->SetIconSize(CSize(16, 16));
   AddTooltip(pControl, IDS_ID_TOOL_FREEHAND);

   // Group Shapes
   CXTPRibbonGroup* pGroupShapes = m_pTabTools->AddGroup(ID_GROUP_SHAPES);
   pGroupShapes->SetControlsCentering();
   pControl = pGroupShapes->Add(xtpControlButton, ID_TOOL_OVAL);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetCaption(ID_TOOL_OVAL);
   AddTooltip(pControl, IDS_ID_TOOL_OVAL);
   pControl->SetIconSize(CSize(16, 16));
   pControl = pGroupShapes->Add(xtpControlButton, ID_TOOL_RECTANGLE);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetCaption(ID_TOOL_RECTANGLE);
   AddTooltip(pControl, IDS_ID_TOOL_RECTANGLE);
   pControl->SetIconSize(CSize(16, 16));
   pControl = pGroupShapes->Add(xtpControlButton, ID_TOOL_POLYGON);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetCaption(ID_TOOL_POLYGON);
   AddTooltip(pControl, IDS_ID_TOOL_POLYGON);
   pControl->SetIconSize(CSize(16, 16));

   // Group Insert
   CXTPRibbonGroup* pGroupInsert = m_pTabTools->AddGroup(ID_GROUP_INSERT);
   pGroupInsert->SetControlsCentering();
   CXTPControl* pBnp = pGroupInsert->Add(xtpControlButton, ID_PAGE_NEW);
   pBnp->SetCaption(ID_PAGE_NEW);
   AddTooltip(pBnp, IDS_ID_PAGE_NEW);
   pBnp = pGroupInsert->Add(xtpControlButton, ID_CHAPTER_NEW);
   pBnp->SetCaption(ID_CHAPTER_NEW);
   AddTooltip(pBnp, IDS_ID_CHAPTER_NEW);
   pBnp = pGroupInsert->Add(xtpControlButton, ID_INSERT_IMAGE);
   pBnp->SetCaption(ID_INSERT_IMAGE);
   AddTooltip(pBnp, IDS_ID_INSERT_IMAGE);
   pBnp = pGroupInsert->Add(xtpControlButton, ID_TOOL_TEXT);
   pBnp->SetCaption(ID_TOOL_TEXT);
   AddTooltip(pBnp, IDS_ID_TOOL_TEXT);

   // Group Explore
   CXTPRibbonGroup* pGroupExplore = m_pTabTools->AddGroup(ID_GROUP_EXPLORE);
   pGroupExplore->SetControlsCentering();
   // Add explore button
   CXTPControl *pExploreHome = pGroupExplore->Add(xtpControlButton, ID_EXPLORE_HOME);
   pExploreHome->SetCaption(ID_EXPLORE_HOME);
   AddTooltip(pExploreHome, IDS_ID_EXPLORE_HOME);
}


void CRibbonBars::CreateTabStartEditor(CXTPRibbonBar* pRibbon)
{
   CXTPCommandBars* pCommandBars = GetCommandBars();
   m_pTabStartEditor = pRibbon->AddTab(ID_TAB_START_E);

   // Group Overview
   CXTPRibbonGroup* pGroupOverview = m_pTabStartEditor->AddGroup(ID_GROUP_OVERVIEW);
   CXTPControl* pControlBackTo;

   //Back to button
   pControlBackTo = (CXTPControl*)pGroupOverview->Add(xtpControlButton, IDB_BACK_TO_BUTTON);
   AddTooltip(pControlBackTo, IDS_IDB_BACK_TO_BUTTON);
   pControlBackTo->SetStyle(xtpButtonIconAndCaptionBelow);
   // Group Clipboard
   CXTPRibbonGroup* pGroupClipboard = m_pTabStartEditor->AddGroup(ID_GROUP_CLIPBOARD_E);
   // pGroupClipboard->SetControlsCentering();

   //paste button
   CXTPControl* pControlPaste = (CXTPControl*)pGroupClipboard->Add(xtpControlButton, IDC_EDIT_PASTE);
   pControlPaste->SetStyle(xtpButtonIconAndCaptionBelow);
   pControlPaste->SetCaption(IDC_EDIT_PASTE);
   AddTooltip(pControlPaste, IDS_IDC_EDIT_PASTE);

   //copy button
   CXTPControl* pControlCopy = pGroupClipboard->Add(xtpControlButton, IDC_EDIT_COPY);
   pControlCopy->SetStyle(xtpButtonIconAndCaptionBelow);
   pControlCopy->SetCaption(IDC_EDIT_COPY);
   AddTooltip(pControlCopy, IDS_IDC_EDIT_COPY);

   //cut button
   CXTPControl* pControlCut = pGroupClipboard->Add(xtpControlButton, IDC_EDIT_CUT);
   pControlCut->SetStyle(xtpButtonIconAndCaptionBelow);
   pControlCut->SetCaption(IDC_EDIT_CUT);
   AddTooltip(pControlCut, IDS_IDC_EDIT_CUT);


   // Group Font
   CXTPRibbonGroup* pGroupFont = m_pTabStartEditor->AddGroup(ID_GROUP_FONT_E);
   pGroupFont->SetControlsGrouping();

   // Font family
   CXTPControlComboBox* pFontCombo = new CXTPControlComboBox();
   pFontCombo->SetDropDownListStyle();
   pFontCombo->EnableAutoComplete();
   pFontCombo->SetWidth(163);
   pGroupFont->Add(pFontCombo, IDC_FONT_FAMILY);
   AddTooltip(pFontCombo, IDS_IDC_FONT_FAMILY);

   CXTPPopupBar* pPopupBar = CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(pCommandBars);
   pFontCombo->SetCommandBar(pPopupBar);

   CXTPControlGallery *pControlGallery = new CXTPControlGallery();
   pControlGallery->SetControlSize(CSize(290, 508));
   pControlGallery->SetResizable(FALSE, TRUE);
   pControlGallery->ShowLabels(TRUE);


   CXTPControlGalleryItems* pItemsFontFace = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_FONT_FAMILY_LIST_E);
   pItemsFontFace->SetItemSize(CSize(0, 26));
   CGalleryItemFontFace::AddFontItems(pItemsFontFace);


   pControlGallery->SetItems(pItemsFontFace);
   pPopupBar->GetControls()->Add(pControlGallery, ID_FONT_FAMILY_LIST_E);
   pPopupBar->InternalRelease();

   // Font style
   CXTPControl* pFontStyleWeight = pGroupFont->Add(xtpControlButton, IDC_FONT_WEIGHT);
   AddTooltip(pFontStyleWeight, IDS_IDC_FONT_WEIGHT);
   pFontStyleWeight->SetBeginGroup(true);
   CXTPControl* pFontStyleSlant = pGroupFont->Add(xtpControlButton, IDC_FONT_SLANT);
   AddTooltip(pFontStyleSlant, IDS_IDC_FONT_SLANT);
   CXTPControl* pFontStyleIncrease =pGroupFont->Add(xtpControlButton, ID_FONT_SIZE_INCREASE_E);
   AddTooltip(pFontStyleIncrease, IDS_ID_FONT_SIZE_INCREASE_E);
   CXTPControl* pFontStyleDecrease =pGroupFont->Add(xtpControlButton, ID_FONT_SIZE_DECREASE_E);
   AddTooltip(pFontStyleDecrease, IDS_ID_FONT_SIZE_DECREASE_E);

   // Text color
   CXTPControlPopupColor* pPopupColorTxt = new CXTPControlPopupColor();
   pPopupColorTxt->SetBeginGroup(true);
   CXTPPopupBar* pColorBarTxt = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   // pColorBarTxt->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_TEXT_COLOR);
   CXTPControlColorSelector *pControlColorSelectorTxt = new CXTPControlColorSelector();
   pControlColorSelectorTxt->SetBeginGroup(TRUE);
   pColorBarTxt->GetControls()->Add(pControlColorSelectorTxt, ID_SELECTOR_TEXT_COLOR_E);
   CXTPControlButtonColor *pControlButtonColorTxt = new CXTPControlButtonColor();
   pControlButtonColorTxt->SetBeginGroup(TRUE);
   pColorBarTxt->GetControls()->Add(pControlButtonColorTxt, IDS_MORE_TEXT_COLORS_E);

   pPopupColorTxt->SetCommandBar(pColorBarTxt);

   pColorBarTxt->SetTearOffPopup(_T("Text Color"), IDR_TEXTCOLOR_POPUP, 0);
   pColorBarTxt->EnableCustomization(FALSE);
   pColorBarTxt->InternalRelease();

   pGroupFont->Add(pPopupColorTxt, IDC_TEXT_COLOR);
   AddTooltip(pPopupColorTxt, IDS_IDC_TEXT_COLOR);

   //// Text size
   static int asiFontSize [22] = { 8, 9, 10, 12, 14, 16, 18, 20, 24, 28, 32,
      36, 40, 44, 48, 54, 60, 66, 72, 80, 88, 96};
   CXTPControlComboBox* pComboSize = new CXTPControlComboBox();
   pComboSize->SetBeginGroup(true);
   pComboSize->SetDropDownListStyle();
   pComboSize->SetWidth(35);
   pComboSize->SetEditText(_T("10"));
   pGroupFont->Add(pComboSize, IDC_FONT_SIZE);
   AddTooltip(pComboSize, IDS_IDC_FONT_SIZE);

   pPopupBar = CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(pCommandBars);
   pComboSize->SetCommandBar(pPopupBar);

   pControlGallery = new CXTPControlGallery();
   pControlGallery->SetControlSize(CSize(42, 16 * 17));
   pControlGallery->SetResizable(FALSE, TRUE);

   CXTPControlGalleryItems* pItemsFontSize = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_FONT_SIZE_LIST_E);
   pItemsFontSize->SetItemSize(CSize(0, 17));

   for(int i = 0; i < 22; i++)
   {
      CString s;
      s.Format(_T("%d"), asiFontSize[i]);
      pItemsFontSize->AddItem(s);
   }

   pControlGallery->SetItems(pItemsFontSize);

   pPopupBar->GetControls()->Add(pControlGallery, ID_FONT_SIZE_LIST_E);
   pPopupBar->InternalRelease();

   CXTPControl* pFontAlignLeft = pGroupFont->Add(xtpControlButton, IDC_FONT_ALIGN_LEFT_E);
   AddTooltip(pFontAlignLeft, IDS_IDC_FONT_ALIGN_LEFT_E);
   pFontAlignLeft->SetBeginGroup(TRUE);
   CXTPControl* pFontAlignCenter =pGroupFont->Add(xtpControlButton, IDC_FONT_ALIGN_CENTER_E);
   AddTooltip(pFontAlignCenter, IDS_IDC_FONT_ALIGN_CENTER_E);
   CXTPControl* pFontAlignRight =pGroupFont->Add(xtpControlButton, IDC_FONT_ALIGN_RIGHT_E);
   AddTooltip(pFontAlignRight, IDS_IDC_FONT_ALIGN_RIGHT_E);

   //Group Styles
   CXTPRibbonGroup* pGroupStyles = m_pTabStartEditor->AddGroup(ID_GROUP_STYLES_E);
   pGroupStyles->SetControlsCentering();

   // Fill color
   CXTPControlPopupColor* pPopupColor = new CXTPControlPopupColor();
   CXTPPopupBar* pColorBar= (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   //pColorBar->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_FILL_COLOR);
   CXTPControlColorSelector *pControlColorSelector = new CXTPControlColorSelector();
   pControlColorSelector->SetBeginGroup(TRUE);
   pColorBar->GetControls()->Add(pControlColorSelector, ID_SELECTOR_FILL_COLOR_E);
   CXTPControlButtonColor *pControlButtonColor = new CXTPControlButtonColor();
   pControlButtonColor->SetBeginGroup(TRUE);
   pColorBar->GetControls()->Add(pControlButtonColor, IDS_MORE_FILL_COLORS_E);

   pPopupColor->SetCommandBar(pColorBar);

   pColorBar->SetTearOffPopup(_T("Fill Color"), IDR_FILLCOLOR_POPUP, 0);
   pColorBar->EnableCustomization(FALSE);
   pColorBar->InternalRelease();

   pGroupStyles->Add(pPopupColor, IDC_FILL_COLOR);
   AddTooltip(pPopupColor, IDS_IDC_FILL_COLOR);

   // Line color
   CXTPControlPopupColor* pPopupColorLine = new CXTPControlPopupColor();
   CXTPPopupBar* pColorBarLine = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   //pColorBarLine->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_LINE_COLOR);
   CXTPControlColorSelector *pControlColorSelectorLine = new CXTPControlColorSelector();
   pControlColorSelectorLine->SetBeginGroup(TRUE);
   pColorBarLine->GetControls()->Add(pControlColorSelectorLine, ID_SELECTOR_LINE_COLOR_E);
   CXTPControlButtonColor *pControlButtonColorLine = new CXTPControlButtonColor();
   pControlButtonColorLine->SetBeginGroup(TRUE);
   pColorBarLine->GetControls()->Add(pControlButtonColorLine, IDS_MORE_LINE_COLORS_E);

   pPopupColorLine->SetCommandBar(pColorBarLine);

   pColorBarLine->SetTearOffPopup(_T("Line Color"), IDR_LINECOLOR_POPUP, 0);
   pColorBarLine->EnableCustomization(FALSE);
   pColorBarLine->InternalRelease();

   pGroupStyles->Add(pPopupColorLine, IDC_LINE_COLOR);
   AddTooltip(pPopupColorLine, IDS_IDC_LINE_COLOR);

   // Line width
   CXTPControlPopup *pControlPopupLineWidth = CXTPControlPopup::CreateControlPopup(xtpControlPopup);
   CXTPPopupBar* pLineWidthBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   CControlLineWidthSelector *pControlLineWidthSelector = new CControlLineWidthSelector();
   pControlLineWidthSelector->SetBeginGroup(TRUE);
   pLineWidthBar->GetControls()->Add(pControlLineWidthSelector, ID_SELECTOR_LINE_WIDTH_E);
   pControlPopupLineWidth->SetCommandBar(pLineWidthBar);

   pLineWidthBar->EnableCustomization(FALSE);
   pLineWidthBar->InternalRelease(); 
   pGroupStyles->Add(pControlPopupLineWidth, IDC_LINE_WIDTH)->SetStyle(xtpButtonIcon);
   AddTooltip(pControlPopupLineWidth, IDS_IDC_LINE_WIDTH);

   // Line style
   CXTPControlPopup *pControlPopupLineStyle = CXTPControlPopup::CreateControlPopup(xtpControlPopup);
   CXTPPopupBar* pLineStyleBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   CControlLineStyleSelector *pControlLineStyleSelector = new CControlLineStyleSelector();
   pLineStyleBar->GetControls()->Add(pControlLineStyleSelector, ID_SELECTOR_LINE_STYLE_E);
   pControlPopupLineStyle->SetCommandBar(pLineStyleBar);

   pLineStyleBar->EnableCustomization(FALSE);
   pLineStyleBar->InternalRelease();
   pGroupStyles->Add(pControlPopupLineStyle, IDC_LINE_STYLE)->SetStyle(xtpButtonIcon);
   AddTooltip(pControlPopupLineStyle, IDS_IDC_LINE_STYLE);

   // Arrow style
   CXTPControlPopup *pControlPopupArrowStyle = CXTPControlPopup::CreateControlPopup(xtpControlPopup);
   CXTPPopupBar* pArrowStyleBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();

   CControlArrowStyleSelector *pControlArrowStyleSelector = new CControlArrowStyleSelector();
   pArrowStyleBar->GetControls()->Add(pControlArrowStyleSelector, ID_SELECTOR_ARROW_STYLE_E);
   pControlPopupArrowStyle->SetCommandBar(pArrowStyleBar);
   UINT uiArrowStyle[] = {-1, -1, -1, -1,-1, -1 ,ID_SELECTOR_ARROW_STYLE_E}; 
   pCommandBars->GetImageManager()->SetIcons(IDR_TOOLBAR_DRAW, uiArrowStyle, _countof(uiArrowStyle), CSize(16, 16));

   pArrowStyleBar->EnableCustomization(FALSE);
   pArrowStyleBar->InternalRelease();
   pGroupStyles->Add(pControlPopupArrowStyle, IDC_ARROW_STYLE)->SetStyle(xtpButtonIcon);
   AddTooltip(pControlPopupArrowStyle, IDS_IDC_ARROW_STYLE);

   // Arrange(stack order)
   CXTPControlPopup *pControlPopupArrange = (CXTPControlPopup*)pGroupStyles->Add(xtpControlButtonPopup, IDC_STACK_ORDER);
   AddTooltip(pControlPopupArrange, IDS_IDC_STACK_ORDER);
   pControlPopupArrange->SetStyle(xtpButtonIconAndCaptionBelow);

   CMenu menuStackOrder;
   menuStackOrder.LoadMenu(IDR_SUBMENU_STACK_ORDER);

   pControlPopupArrange->SetCommandBar(menuStackOrder.GetSubMenu(0));

   //Group Playback Controls
   CXTPRibbonGroup* pGroupPlaybackControls = m_pTabStartEditor->AddGroup(ID_GROUP_PLAYBACK_CONTROLS);

   //Start/Stop preview button
   CXTPControl *pControlStartStop = pGroupPlaybackControls->Add(xtpControlButton, IDC_STARTSTOPPREVIEW);
   pControlStartStop->SetStyle(xtpButtonIconAndCaptionBelow);

   // jump back button
   CXTPControl *pControlJumpBack = pGroupPlaybackControls->Add(xtpControlButton, IDC_JUMP_BACK);
   pControlJumpBack->SetStyle(xtpButtonIcon);
   pControlJumpBack->SetHeight(24);
   pControlJumpBack->SetWidth(24);
   pControlJumpBack->SetIconSize(CSize(16, 16));
   AddTooltip(pControlJumpBack, IDS_IDC_JUMP_BACK);

   // jump to beginning button
   CXTPControl *pControlJumpBegin = pGroupPlaybackControls->Add(xtpControlButton, IDC_JUMP_POS1);
   pControlJumpBegin->SetStyle(xtpButtonIcon);
   pControlJumpBegin->SetHeight(24);
   pControlJumpBegin->SetWidth(24);
   pControlJumpBegin->SetIconSize(CSize(16, 16));
   AddTooltip(pControlJumpBegin, IDS_IDC_JUMP_POS1);

   // jump forward button
   CXTPControl *pControlJumpForward = pGroupPlaybackControls->Add(xtpControlButton, IDC_JUMP_FORWARD);
   pControlJumpForward->SetStyle(xtpButtonIcon);
   pControlJumpForward->SetHeight(24);
   pControlJumpForward->SetWidth(24);
   pControlJumpForward->SetIconSize(CSize(16, 16));
   AddTooltip(pControlJumpForward, IDS_IDC_JUMP_FORWARD);

   // jump end button
   CXTPControl *pControlJumpEnd = pGroupPlaybackControls->Add(xtpControlButton, IDC_JUMP_END);
   pControlJumpEnd->SetStyle(xtpButtonIcon);
   pControlJumpEnd->SetHeight(24);
   pControlJumpEnd->SetWidth(24);
   pControlJumpEnd->SetIconSize(CSize(16, 16));
   AddTooltip(pControlJumpEnd, IDS_IDC_JUMP_END);


   //Select Group
   CXTPRibbonGroup* pGroupSelect = m_pTabStartEditor->AddGroup(ID_GROUP_SELECT);

   //Select current page button
   CXTPControl *pControlMarkPage = pGroupSelect->Add(xtpControlButton, ID_MARK_PAGE);
   pControlMarkPage->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlMarkPage, IDS_ID_MARK_PAGE);

   //Select current clip button
   CXTPControl *pControlMarkClip = pGroupSelect->Add(xtpControlButton, ID_MARK_CLIP);
   pControlMarkClip->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlMarkClip, IDS_ID_MARK_CLIP);
   

   //Remove/Delete Group
   CXTPRibbonGroup* pGroupRemoveDelete = m_pTabStartEditor->AddGroup(ID_GROUP_REMOVE_DELETE);


   //button remove video
   CXTPControl *pControlRemoveVideo = pGroupRemoveDelete->Add(xtpControlButton, ID_EDIT_REMOVE_VIDEO);
   pControlRemoveVideo->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlRemoveVideo, IDS_ID_EDIT_REMOVE_VIDEO);

   //button remove still image
   CXTPControl *pControlDeleteImage = pGroupRemoveDelete->Add(xtpControlButton, ID_EDIT_DELETESTILLIMAGE);
   pControlDeleteImage->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlDeleteImage, IDS_ID_EDIT_DELETESTILLIMAGE);

   //button remove still image
   CXTPControl *pControlDeleteQuestion = pGroupRemoveDelete->Add(xtpControlButton, IDC_DELETE_QUESTION);
   pControlDeleteQuestion->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlDeleteQuestion, IDS_IDC_DELETE_QUESTION);

   //button remove Current Clip
   CXTPControl *pControlClipDelete = pGroupRemoveDelete->Add(xtpControlButton, ID_CLIP_DELETE);
   pControlClipDelete->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlClipDelete, IDS_ID_CLIP_DELETE);

   //Test Group
   CXTPRibbonGroup* pGroupTest = m_pTabStartEditor->AddGroup(ID_GROUP_TEST);

   //test button
   CXTPControl *pControlSettings = pGroupTest->Add(xtpControlButton, IDC_SHOW_QUESTIONAIRE_SETTINGS);
   pControlSettings->SetStyle(xtpButtonIconAndCaptionBelow);
   CString csCaption1, csCaption2;
   csCaption1.LoadString(ID_GROUP_TEST);
   csCaption2.LoadString(IDC_SHOW_QUESTIONAIRE_SETTINGS);
   pControlSettings->SetCaption(csCaption1 + _T(" ") + csCaption2);
   AddTooltip(pControlSettings, IDS_IDC_SHOW_QUESTIONAIRE_SETTINGS);

   //Demo Document Group
   CXTPRibbonGroup* pGroupDemoDoc = m_pTabStartEditor->AddGroup(ID_GROUP_DEMO_DOCUMENT);
   pGroupDemoDoc->SetControlsCentering();

   //Create Demo Document button
   CXTPControl *pControlCreateDemo = pGroupDemoDoc->Add(xtpControlButton, ID_CREATE_DEMO_DOCUMENT);
   pControlCreateDemo->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlCreateDemo, IDS_ID_CREATE_DEMO_DOCUMENT);

   //Create Manual button
   /*Bugfix for bug 5955
   * The button "Create Manual" should not be used in pathc 4.0.p9
   * The code that adds the button to ribbon has been commented out
   * The cone will be later used for LECTURNITY 5.0
   */
   /*CXTPControl *pControlCreateManual = pGroupDemoDoc->Add(xtpControlButton, ID_CREATE_MANUAL);
   pControlCreateManual->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlCreateManual, IDS_ID_CREATE_MANUAL);*/

   // Mouse Pointer Group
   CXTPRibbonGroup* pGroupEmbed = m_pTabStartEditor->AddGroup(ID_GROUP_EMBED);
   pGroupEmbed->SetControlsCentering();

   //Create Mouse Pointer button
   CXTPControl *pControlMousePointer = pGroupEmbed->Add(xtpControlButton, ID_EMBED_MOUSEPOINTER);
   pControlMousePointer->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlMousePointer, IDS_ID_EMBED_MOUSEPOINTER);

   //Create Show Click Pages button
   CXTPControl *pControlClickPages = pGroupEmbed->Add(xtpControlButton, IDC_EMBED_CLICK_PAGES);
   pControlClickPages->SetStyle(xtpButtonIconAndCaptionBelow);
   pControlClickPages->SetIconSize(CSize(32, 32));
   CString csTitle;
   csTitle.LoadString(IDS_CLICKPAGES_TITLE);
   pControlClickPages->SetTooltip(csTitle);
   CString csDescription;
   csDescription.LoadString(IDS_CLICKPAGE_TOOLTIP);
   pControlClickPages->SetDescription(csDescription);
}

void CRibbonBars::CreateTabInsert(CXTPRibbonBar* pRibbon)
{
   m_pTabInsert = pRibbon->AddTab(ID_TAB_INSERT);
   // Group Overview
   CXTPRibbonGroup* pGroupOverview = m_pTabInsert->AddGroup(ID_GROUP_OVERVIEW);
   CXTPControl* pControlBackTo;

   //Back to button
   pControlBackTo = (CXTPControl*)pGroupOverview->Add(xtpControlButton, IDB_BACK_TO_BUTTON);
   pControlBackTo->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlBackTo, IDS_IDB_BACK_TO_BUTTON);

   //Add group Media
   CXTPRibbonGroup* pGroupMedia = m_pTabInsert->AddGroup(ID_GROUP_MEDIA);
   CXTPControl* pControl;
   CXTPControlPopup* pControlPopup;

   //button LRD
   pControlPopup = (CXTPControlPopup*)pGroupMedia->Add(xtpControlButtonPopup, ID_MEDIA_LRD_BUTTON);
   pControlPopup->SetStyle(xtpButtonIconAndCaptionBelow);

   CMenu menuLrd;
   menuLrd.LoadMenu(ID_MEDIA_LRD_MENU);

   pControlPopup->SetCommandBar(menuLrd.GetSubMenu(0));

   //button image
   pControl = pGroupMedia->Add(xtpControlButton, ID_STILLIMAGE);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);

   //button clip
   pControlPopup = (CXTPControlPopup*)pGroupMedia->Add(xtpControlButtonPopup, ID_MEDIA_CLIP_BUTTON);
   pControlPopup->SetStyle(xtpButtonIconAndCaptionBelow);

   CMenu menuClip;
   menuClip.LoadMenu(ID_MEDIA_CLIP_MENU);

   pControlPopup->SetCommandBar(menuClip.GetSubMenu(0));

   //button video
   pControl = pGroupMedia->Add(xtpControlButton, ID_EDIT_IMPORT_VIDEO);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);
   pControl->SetIconId(ID_EDIT_IMPORT_VIDEO_RIBBON);
   //button audio
   pControl = pGroupMedia->Add(xtpControlButton, ID_EDIT_IMPORTAUDIO);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);
   pControl->SetIconId(ID_EDIT_IMPORTAUDIO_RIBBON);

   //Add Group Interaction elements
   CXTPRibbonGroup* pGroupInteractive = m_pTabInsert->AddGroup(ID_GROUP_INTERACTIVE_ELEMENTS);
   pGroupInteractive->ShowOptionButton();
   pGroupInteractive->GetControlGroupOption()->SetID(ID_GODEF_OPTIONS);

   AddTooltip(pGroupInteractive->GetControlGroupOption(), IDS_ID_GROUP_GRAPHICAL_OBJECTS);

   //Button interactive callout
   //CXTPRibbonGroup* pGroupDemoObjects = m_pTabInsert->AddGroup(ID_GROUP_DEMO_OBJECTS);
   CMenu menuDemo;
   menuDemo.LoadMenu(IDR_POPUP_DEMOOBJECTS);
   //pGroupDemoObjects->SetControlsCentering(TRUE);

   CXTPControlPopup* pControlDemo = (CXTPControlPopup*)pGroupInteractive->Add(xtpControlSplitButtonPopup, ID_DEMOOBJECT);
   pControlDemo->SetIconSize(CSize(32, 32));
   pControlDemo->SetStyle(xtpButtonIconAndCaptionBelow);

   CXTPPopupBar* pPopupBarDemo = CXTPPopupBar::CreatePopupBar(GetCommandBars());
   pPopupBarDemo->LoadMenu(menuDemo.GetSubMenu(0));

   pControlDemo->SetCommandBar(pPopupBarDemo);
   pPopupBarDemo->InternalRelease();

   //button interactive area
   pControl = pGroupInteractive->Add(xtpControlButton, ID_INTERACTION_AREA);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);

   //button interactive button
   pControl = pGroupInteractive->Add(xtpControlButton, ID_INTERACTION_BUTTON);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);

   //UINT uiTabInsertSmall[] = {ID_ADD_QUESTION_MC, ID_ADD_QUESTION_DD, ID_ADD_QUESTION_FB}; 
   //button single/multiple choice
   pControl = pGroupInteractive->Add(xtpControlButton, ID_ADD_QUESTION_MC);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetIconSize(CSize(15, 15));

   //button drag and drop
   pControl = pGroupInteractive->Add(xtpControlButton, ID_ADD_QUESTION_DD);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetIconSize(CSize(15, 15));
   pControl->SetCaption(IDS_ID_ADD_QUESTION_DD);

   //button fill in the blank
   pControl = pGroupInteractive->Add(xtpControlButton, ID_ADD_QUESTION_FB);
   pControl->SetStyle(xtpButtonIconAndCaption);
   pControl->SetIconSize(CSize(15, 15));

   //Add group markers
   CXTPRibbonGroup* pGroupMarkers = m_pTabInsert->AddGroup(ID_GROUP_MARKERS);

   //button Stopmark
   pControl = pGroupMarkers->Add(xtpControlButton, IDC_INSERT_STOPMARK);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);

   //button Stopmark
   pControl = pGroupMarkers->Add(xtpControlButton, IDC_INSERT_TARGETMARK);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);

   //Add group Objects
   CXTPRibbonGroup* pGroupObjects = m_pTabInsert->AddGroup(ID_GROUP_GRAPHICAL_OBJECTS);

   pGroupObjects->ShowOptionButton();
   pGroupObjects->GetControlGroupOption()->SetID(ID_GODEF_OPTIONS);
   
   AddTooltip(pGroupObjects->GetControlGroupOption(), IDS_ID_GROUP_GRAPHICAL_OBJECTS);
   
   CMenu menuCallout;
   menuCallout.LoadMenu(IDR_POPUP_CALLOUTS);

   CXTPControlPopup* pControlCallout = (CXTPControlPopup*)pGroupObjects->Add(xtpControlSplitButtonPopup, ID_GRAPHICALOBJECT_CALLOUT);
   pControlCallout->SetIconSize(CSize(32, 32));
   pControlCallout->SetStyle(xtpButtonIconAndCaptionBelow);

   CXTPPopupBar* pPopupBarCallout = CXTPPopupBar::CreatePopupBar(GetCommandBars());
   pPopupBarCallout->LoadMenu(menuCallout.GetSubMenu(0));

   pControlCallout->SetCommandBar(pPopupBarCallout);
   pPopupBarCallout->InternalRelease();

   pControl = pGroupObjects->Add(xtpControlButton, ID_GRAPHICALOBJECT_TEXT);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);
   pControl->SetIconId(ID_TOOL_TEXT);

   //Add group Demo Objects
   /*CXTPRibbonGroup* pGroupDemoObjects = m_pTabInsert->AddGroup(ID_GROUP_DEMO_OBJECTS);
   CMenu menuDemo;
   menuDemo.LoadMenu(IDR_POPUP_DEMOOBJECTS);
   pGroupDemoObjects->SetControlsCentering(TRUE);

   CXTPControlPopup* pControlDemo = (CXTPControlPopup*)pGroupDemoObjects->Add(xtpControlSplitButtonPopup, ID_DEMOOBJECT);
   pControlDemo->SetIconSize(CSize(32, 32));
   pControlDemo->SetStyle(xtpButtonIconAndCaptionBelow);

   CXTPPopupBar* pPopupBarDemo = CXTPPopupBar::CreatePopupBar(GetCommandBars());
   pPopupBarDemo->LoadMenu(menuDemo.GetSubMenu(0));

   pControlDemo->SetCommandBar(pPopupBarDemo);
   pPopupBarDemo->InternalRelease();*/


}

void CRibbonBars::CreateTabViewEditor(CXTPRibbonBar* pRibbon) {
   m_pTabViewEditor = pRibbon->AddTab(ID_TAB_VIEW_E);

   // Group Overview
   CXTPRibbonGroup* pGroupOverview = m_pTabViewEditor->AddGroup(ID_GROUP_OVERVIEW);
   CXTPControl* pControlBackTo;

   //Back to button
   pControlBackTo = (CXTPControl*)pGroupOverview->Add(xtpControlButton, IDB_BACK_TO_BUTTON);
   pControlBackTo->SetStyle(xtpButtonIconAndCaptionBelow);
   AddTooltip(pControlBackTo, IDS_IDB_BACK_TO_BUTTON);

   //add group Show/Hide
   CXTPRibbonGroup* pGroupShowHide = m_pTabViewEditor->AddGroup(ID_GROUP_SHOW_HIDE_E);
   CXTPControl* pControl = NULL;

   pControl = pGroupShowHide->Add(xtpControlButton, IDC_SHOW_STRUCTURE);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);
   pControl->SetIconSize(CSize(32, 32));

   pControl = pGroupShowHide->Add(xtpControlButton, IDC_SHOW_STREAMS);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);
   pControl->SetIconSize(CSize(32, 32));

   pControl = pGroupShowHide->Add(xtpControlButton, IDC_SHOW_PAGEOBJECTS);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);
   pControl->SetIconSize(CSize(32, 32));

   pControl = pGroupShowHide->Add(xtpControlButton, IDC_SHOW_VIDEO);
   pControl->SetStyle(xtpButtonIconAndCaptionBelow);
   pControl->SetIconSize(CSize(32, 32));
   
   CXTPRibbonGroup* pGroupShowInTimeline = m_pTabViewEditor->AddGroup(ID_GROUP_SHOW_IN_TIMELINE);
   pControl = pGroupShowInTimeline->Add(xtpControlCheckBox, ID_MARKS);
   pControl = pGroupShowInTimeline->Add(xtpControlCheckBox, ID_VIDEO);
   pControl = pGroupShowInTimeline->Add(xtpControlCheckBox, ID_CLIPS);
   pControl = pGroupShowInTimeline->Add(xtpControlCheckBox, ID_FOLIEN);
   pControl = pGroupShowInTimeline->Add(xtpControlCheckBox, ID_AUDIO);
}

CXTPCommandBars* CRibbonBars::GetCommandBars()
{
   return m_pCommandBars;
}
void CRibbonBars::SetCommandBars(CXTPCommandBars* m_pCommandBarsParam)
{
   m_pCommandBars = m_pCommandBarsParam;
}
CXTPControl *CRibbonBars::GetControlOnRibbonBar(UINT nGroupId, UINT nControlId)
{
   if ( m_pRibbonBar != NULL )
   {
      CXTPRibbonGroup *pGroup = (CXTPRibbonGroup*)m_pRibbonBar->FindGroup(nGroupId);
      if ( pGroup != NULL )
      {
         return (CXTPControl*)pGroup->FindControl(nControlId);
      }
   }
   return NULL;
}

//Load ribbon icons
void CRibbonBars::LoadRibbonIconsAssistant()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();

   UINT uGearInactiv = {IDB_GEAR_ASSISTANT};
   pCommandBars->GetImageManager()->SetIcons(IDB_GEAR_ASSISTANT, &uGearInactiv, 1, CSize(41, 40), xtpImageNormal);
   CXTPControlPopup* pSysButton = m_pRibbonBar->GetSystemButton();
   if(pSysButton)
   {
      pSysButton->GetImageManager()->SetIcons(IDB_GEAR_ASSISTANT, &uGearInactiv, 1, CSize(41, 40), xtpImageNormal);
      pSysButton->GetImageManager()->SetIcons(IDB_GEAR_ASSISTANT_MOUSEOVER, &uGearInactiv, 1, CSize(41, 40),xtpImageHot );
      pSysButton->GetImageManager()->SetIcons(IDB_GEAR_ASSISTANT_ACTIV, &uGearInactiv, 1, CSize(41, 40), xtpImagePressed);
   }

   UINT uiSystemMenu1[] = {ID_MENU_NEW, ID_MENU_OPEN, ID_FILE_PACK, ID_MENU_SAVE, ID_MENU_SAVE_AS}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu1, _countof(uiSystemMenu1), CSize(32, 32));

   UINT uiSystemMenu1_1[] = {-1, -1, -1, -1, -1, -1, -1,-1, -1, -1, -1, -1, -1, -1, -1,ID_LRD_OPEN};  
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uiSystemMenu1_1, _countof(uiSystemMenu1_1), CSize(32, 32));

   UINT uiSystemMenu3[] = {-1, ID_LSD_OPEN, -1, ID_LSD_SAVE, ID_LSD_SAVE_AS, ID_LSD_CLOSE}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu3, _countof(uiSystemMenu3), CSize(32, 32));

   UINT uiSystemMenu4[] = {-1, -1, -1, ID_FILE_SAVE, ID_FILE_SAVE_AS, -1, ID_MENU_PROJECT_SETTINGS, ID_SHOW_PROPERTIES, ID_SETUP_PAGE}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu4, _countof(uiSystemMenu4), CSize(32, 32));

   UINT uiSystemMenu5[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, ID_MENU_IMPORT, ID_PPT_IMPORT, IDC_IMPORT_POWERPOINT_AS,ID_MENU_REVERT,-1,ID_LSD_NEW}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu5, _countof(uiSystemMenu5), CSize(32, 32));

   UINT uiSystemMenu6[] = {-1, -1, -1, -1, -1, ID_CLOSE,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1/*ID_LSD_OPEN*/}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU, uiSystemMenu6, _countof(uiSystemMenu6), CSize(32, 32));  

   UINT uiTabTools[] = {ID_FULL_SCREEN, ID_FULL_SCREEN_CS,-1,-1,-1,-1,ID_BUTTON_MARKERS,-1,-1,ID_BUTTON_PENS,-1,-1,-1,-1,-1,-1,ID_TOOL_TELEPOINTER,ID_TOOL_SIMPLENAVIGATION, -1/*ID_START_RECORDING*//*ID_RECORDER_START_STOP*/,ID_PAUSE_RECORDING, -1, ID_TOOL_SCREENGRABBING,ID_STOP_RECORDING};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiTabTools, _countof(uiTabTools), CSize(32, 32));

   UINT uiStartRecordingNormal[] = {-1, -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, ID_START_RECORDING,-1, -1, -1,-1,-1 };
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiStartRecordingNormal, _countof(uiStartRecordingNormal), CSize(32, 32),xtpImageNormal);
   UINT uiStartRecordingMouseOver[] = {-1, -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1,-1, -1, -1,-1,-1,ID_START_RECORDING };
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiStartRecordingMouseOver, _countof(uiStartRecordingMouseOver), CSize(32, 32),xtpImageHot);
   //UINT uiStartRecordingDisabled[] = {-1, -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1,-1, -1, -1,-1,ID_START_RECORDING};
   //pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiStartRecordingDisabled, _countof(uiStartRecordingDisabled), CSize(32, 32),xtpImageDisabled);



   UINT uiTabView[] = {ID_STANDARD_LAYOUT, ID_PAGE_FOCUSED_LAYOUT,ID_VIEW_PRESENTATION};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_VIEW, uiTabView, _countof(uiTabView), CSize(32, 32));

   UINT uSmallIcons[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,ID_APP_ABOUT,ID_APP_EXIT,ID_MENU_ASSISTANT_SETTINGS};
   pCommandBars->GetImageManager()->SetIcons(IDB_SMALL_ICONS, uSmallIcons, _countof(uSmallIcons), CSize(16, 16));

   UINT uTabStartLarge[] = {ID_EDIT_PASTE,ID_FILL_COLOR,ID_LINE_COLOR,ID_PAGE_NEW,ID_CHAPTER_NEW,ID_INSERT_IMAGE,ID_TOOL_TEXT,IDC_REPLAY,IDC_EDIT,IDC_PUBLISH,ID_EXPLORE_HOME};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uTabStartLarge, _countof(uTabStartLarge), CSize(32, 32));

   UINT uTabStartSmall[] = {ID_FONTWEIGHT,ID_FONTSLANT,ID_FONT_SIZE_INCREASE,ID_FONT_SIZE_DECREASE,ID_TEXT_COLOR};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_SMALL, uTabStartSmall, _countof(uTabStartSmall), CSize(16, 16));  

   UINT uTabStartDraw[] = {-1, -1,-1, -1,-1,ID_TOOL_FREEHAND,ID_TOOL_LINE,ID_TOOL_RECTANGLE,ID_TOOL_OVAL,ID_TOOL_POLYLINE, ID_TOOL_POLYGON};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_SMALL, uTabStartDraw, _countof(uTabStartDraw), CSize(16, 16));

   UINT uTabStartLines[] = {-1, -1,-1, -1,-1,-1,-1,-1,-1,-1, -1, -1,-1, -1, ID_LINE_WIDTH,ID_LINE_STYLE, ID_ARROW_STYLE};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_SMALL, uTabStartLines, _countof(uTabStartLines), CSize(16, 16));

   UINT uTabStartEditElem[] = {-1, -1,-1, -1,-1,-1,-1,-1,-1,-1, -1, -1,-1, -1, -1,-1, -1,ID_EDIT_CUT,ID_EDIT_COPY,-1,ID_TOOL_COPY};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_SMALL, uTabStartEditElem, _countof(uTabStartEditElem), CSize(16, 16));


   // Add icons for Tab Start groups so when ribbon size is reduced the coresponding images will be visible
   UINT uGroupClipboard[] = {ID_GROUP_CLIPBOARD, -1, ID_GROUP_STYLES, ID_GROUP_INSERT};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uGroupClipboard, _countof(uGroupClipboard), CSize(32, 32));

   UINT uGroupFont[] = {-1, -1, -1, -1,-1, -1, -1, -1,-1, -1, -1, ID_GROUP_FONT};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uGroupFont, _countof(uGroupFont), CSize(32, 32));

   UINT uGroupStyles[] = {-1, -1, -1 , -1, -1, -1,-1, -1, -1,-1, -1, -1,-1,ID_GROUP_LINES, ID_GROUP_SHAPES};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uGroupStyles, _countof(uGroupStyles), CSize(32, 32));

   UINT uGroupRecords[] = {-1, -1, -1, -1, -1, -1, -1,-1, -1, -1, -1, -1, -1, -1, -1,ID_GROUP_RECORDING};  
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uGroupRecords, _countof(uGroupRecords), CSize(32, 32));

   UINT uGroupExplore[] = {-1, -1, -1,-1, -1, -1,-1, -1, -1,-1,-1,-1,-1,-1, ID_GROUP_EXPLORE};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE, uGroupExplore, _countof(uGroupExplore), CSize(32, 32));

   // Add icons for Tab Tools groups so when ribbon size is reduced the coresponding images will be visible
   UINT uGroupsTools[] = {ID_GROUP_START_PRESENTATION, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,ID_GROUP_PRESENTATION_TOOLS,-1,-1,-1,-1,ID_TOOL_TELEPOINTER,-1, ID_GROUP_RECORDER,-1, -1, ID_GROUP_SCREENGRABBING};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uGroupsTools, _countof(uGroupsTools), CSize(32, 32));

   // Add icons for Tab Tools groups so when ribbon size is reduced the coresponding images will be visible
   UINT uGroupsPresentation[] = {ID_GROUP_VIEW,-1, -1, ID_GROUP_SHOW_HIDE,ID_GROUP_PRESENTATION,ID_GROUP_GRID,ID_GROUP_LOCK_ELEMENTS};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_VIEW, uGroupsPresentation, _countof(uGroupsPresentation), CSize(32, 32));

   UINT uPresentationBar[] = {ID_ADDORREMOVEBUTTONS_EXITFULLSCREEN, 
      -1,-1,-1,-1,ID_ADDORREMOVEBUTTONS_MARKERS, -1,-1,ID_ADDORREMOVEBUTTONS_PENCILS,
      -1,-1,-1,-1,-1,-1,ID_ADDORREMOVEBUTTONS_POINTER,ID_ADDORREMOVEBUTTONS_SIMPLIFIEDNAVIGATION,
      ID_ADDORREMOVEBUTTONS_PAGEFOCUSED,ID_ADDORREMOVEBUTTONS_DOCUMENTSTRUCT,ID_ADDORREMOVEBUTTONS_RECORDINGS,
      ID_ADDORREMOVEBUTTONS_CUT,ID_ADDORREMOVEBUTTONS_COPY,ID_ADDORREMOVEBUTTONS_PASTE,ID_ADDORREMOVEBUTTONS_UNDO,ID_ADDORREMOVEBUTTONS_ONECLICKCOPY,
      ID_ADDORREMOVEBUTTONS_TEXT,ID_ADDORREMOVEBUTTONS_LINE,ID_ADDORREMOVEBUTTONS_POLYLINE,ID_ADDORREMOVEBUTTONS_FREEHAND,
      ID_ADDORREMOVEBUTTONS_ELLIPSE,ID_ADDORREMOVEBUTTONS_RECTANGLE,ID_ADDORREMOVEBUTTONS_POLYGON,ID_ADDORREMOVEBUTTONS_INSERTPAGE,
      ID_ADDORREMOVEBUTTONS_START,-1,ID_ADDORREMOVEBUTTONS_STOP,ID_ADDORREMOVEBUTTONS_PAUSERECORDING,ID_ADDORREMOVEBUTTONS_SCREENGRABBING,-1,
      ID_ADDORREMOVEBUTTONS_PREVIOUSPAGE, ID_ADDORREMOVEBUTTONS_NEXTPAGE};
   pCommandBars->GetImageManager()->SetIcons(IDB_PRESENTATIONBAR_SMALLICONS_HOT, uPresentationBar, _countof(uPresentationBar), CSize(16, 16)); 

}

void CRibbonBars::LoadRibbonIconsEditor()
{
   CXTPCommandBars* pCommandBars = GetCommandBars();

   UINT uiSystemMenu1[] = {-1, ID_FILE_OPEN, -1, ID_FILE_SAVE, ID_FILE_SAVE_AS}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU_E, uiSystemMenu1, _countof(uiSystemMenu1), CSize(32, 32));

   UINT uiSystemMenu2[] = {-1, -1/*ID_LRD_OPEN*/, -1, -1,-1 ,ID_FILE_IMPORT ,ID_FILE_EXPORT,ID_FILE_EXPORTSELECTION, -1, -1, -1, -1, -1, -1,-1, ID_FILE_EXPORT_SETTINGS,ID_MENU_EXPORT}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU_E, uiSystemMenu2, _countof(uiSystemMenu2), CSize(32, 32));

   UINT uiSystemMenu3[] = {-1, -1, -1, -1,-1, -1, -1, -1, ID_MENU_AUDIO, ID_EDIT_EXPORTAUDIO, ID_EDIT_IMPORTAUDIO}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU_E, uiSystemMenu3, _countof(uiSystemMenu3), CSize(32, 32));

   UINT uiSystemMenu4[] = {-1, -1, -1, -1,-1, -1, -1, -1,-1, -1,-1, ID_MENU_VIDEO,ID_EDIT_IMPORT_VIDEO}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEM_MENU_E, uiSystemMenu4, _countof(uiSystemMenu4), CSize(32, 32));

   UINT uiTabStartLarge[] = {IDC_EDIT_PASTE, IDC_EDIT_COPY, IDC_EDIT_CUT, IDC_FILL_COLOR, IDC_LINE_COLOR,
      IDC_STARTSTOPPREVIEW, ID_MARK_PAGE, ID_MARK_CLIP, IDC_SHOW_QUESTIONAIRE_SETTINGS,
      ID_EDIT_REMOVE_VIDEO, ID_EDIT_DELETESTILLIMAGE, IDC_DELETE_QUESTION,-1,-1,-1,ID_CLIP_DELETE, -1, -1, -1, -1, -1, ID_CREATE_DEMO_DOCUMENT, ID_EMBED_MOUSEPOINTER, ID_CREATE_MANUAL}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE_E, uiTabStartLarge, _countof(uiTabStartLarge), CSize(32, 32));


   UINT uiBackToNormal[] = {-1, -1, -1, -1, -1,-1, -1, -1, -1, -1, -1, -1,-1,-1,-1,-1,-1,-1,-1,-1,
      IDB_BACK_TO_BUTTON
   }; 
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE_E, uiBackToNormal, _countof(uiBackToNormal), CSize(32, 32),xtpImageNormal);

   UINT uiBackToMouseOver[] = {-1, -1, -1, -1, -1,-1, -1, -1, -1, -1, -1, -1,-1,-1,-1,-1,-1,-1,-1,
      IDB_BACK_TO_BUTTON
   }; 
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE_E, uiBackToMouseOver, _countof(uiBackToMouseOver), CSize(32, 32),xtpImageHot);

   UINT uiBackToDisabled[] = {-1, -1, -1, -1, -1,-1, -1, -1, -1, -1, -1, -1,-1,-1,-1,-1,-1,-1,
      IDB_BACK_TO_BUTTON
   }; 
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE_E, uiBackToDisabled, _countof(uiBackToDisabled), CSize(32, 32),xtpImageDisabled);

   UINT uiTabStartSmall[] = {IDC_FONT_WEIGHT, IDC_FONT_SLANT, ID_FONT_SIZE_INCREASE_E, ID_FONT_SIZE_DECREASE_E, IDC_TEXT_COLOR,
      IDC_JUMP_BACK, IDC_JUMP_POS1, IDC_JUMP_FORWARD, IDC_JUMP_END}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_SMALL_E, uiTabStartSmall, _countof(uiTabStartSmall), CSize(16, 16));

   UINT uiStackOrder[] = {IDC_STACK_ORDER}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_STACK_ORDER, uiStackOrder, _countof(uiStackOrder), CSize(32, 32));
   UINT uiStackOrderButtons[] = {ID_STACKORDER_BRINGTOFRONT, ID_STACKORDER_SENDTOBACK, ID_STACKORDER_BRINGFORWARD, ID_STACKORDER_SENDBACKWARD}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_STACK_ORDER_BUTTONS, uiStackOrderButtons, _countof(uiStackOrderButtons), CSize(16, 16));

   UINT uiTabInsertLarge[] = {ID_MEDIA_LRD_BUTTON, ID_STILLIMAGE, ID_MEDIA_CLIP_BUTTON, ID_EDIT_IMPORT_VIDEO_RIBBON, ID_EDIT_IMPORTAUDIO_RIBBON,
      ID_INTERACTION_AREA, ID_INTERACTION_BUTTON, IDC_INSERT_STOPMARK, IDC_INSERT_TARGETMARK}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_INSERT_LARGE_E, uiTabInsertLarge, _countof(uiTabInsertLarge), CSize(32, 32));

   UINT uiTabInsertSmall[] = {ID_ADD_QUESTION_MC, ID_ADD_QUESTION_DD, ID_ADD_QUESTION_FB}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_INSERT_SMALL_E, uiTabInsertSmall, _countof(uiTabInsertSmall), CSize(15, 15));

   UINT uiTabView[] = {IDC_SHOW_STRUCTURE, IDC_SHOW_STREAMS, IDC_SHOW_PAGEOBJECTS, IDC_SHOW_VIDEO, -1, -1, IDC_EMBED_CLICK_PAGES}; 
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_VIEW_E, uiTabView, _countof(uiTabView), CSize(32, 32));

   UINT uiSmallIcons[] = {-1, -1, -1, -1, IDC_REDO,IDC_UNDO,IDC_LINE_WIDTH, IDC_LINE_STYLE, IDC_ARROW_STYLE,-1, ID_APP_ABOUT, ID_APP_EXIT};
   pCommandBars->GetImageManager()->SetIcons(IDB_SMALL_ICONS_E, uiSmallIcons, _countof(uiSmallIcons), CSize(16, 16));

   UINT uiInsertIcons[] = { ID_INSERT_PRESENTATION, ID_INSERT_PRESENTATION_BEGINNING,ID_INSERT_PRESENTATION_END,ID_CLIP_INSERT,ID_CLIP_INSERT_LRD};
   pCommandBars->GetImageManager()->SetIcons(IDB_INSERT_PRESENTATION_ICONS, uiInsertIcons, _countof(uiInsertIcons), CSize(16, 16));


   // Add icons for Tab Start groups so when ribbon size is reduced the coresponding images will be visible
   UINT uGroupsTabStart[] = {ID_GROUP_CLIPBOARD_E, -1,-1, -1,ID_GROUP_STYLES_E,  ID_GROUP_PLAYBACK_CONTROLS, ID_GROUP_SELECT,-1, -1,-1, -1, -1, -1  ,ID_GROUP_FONT_E, ID_GROUP_REMOVE_DELETE,
       -1, -1, -1, -1, -1, -1, ID_GROUP_DEMO_DOCUMENT, ID_GROUP_EMBED};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_START_LARGE_E, uGroupsTabStart, 24, CSize(32, 32));

   // Add icons for Tab Insert groups so when ribbon size is reduced the coresponding images will be visible
   UINT uGroupsTabInsert[] = {-1, -1, -1, -1,-1, ID_GROUP_INTERACTIVE_ELEMENTS, -1, -1, ID_GROUP_MARKERS, ID_GROUP_MEDIA};
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_INSERT_LARGE_E, uGroupsTabInsert, _countof(uGroupsTabInsert), CSize(32, 32));

   // Add icons for Tab View groups so when ribbon size is reduced the coresponding images will be visible
   UINT uGroupsTabView[] = {-1,-1,-1,-1,ID_GROUP_SHOW_HIDE_E ,ID_GROUP_SHOW_IN_TIMELINE  };
   pCommandBars->GetImageManager()->SetIcons(IDB_TAB_VIEW_E, uGroupsTabView, _countof(uGroupsTabView), CSize(32, 32));

   UINT uStatusBarIconsZoomAll[] = { ID_INDICATOR_ZOOM_ALL };
   pCommandBars->GetImageManager()->SetIcons(IDR_STATUSBAR_ZOOM_ALL, uStatusBarIconsZoomAll, _countof(uStatusBarIconsZoomAll), CSize(16, 16));

   UINT uStatusBarIconsZoomSelection[] = { ID_INDICATOR_ZOOM_SELECTED };
   pCommandBars->GetImageManager()->SetIcons(IDR_STATUSBAR_ZOOM_SELECTED, uStatusBarIconsZoomSelection, _countof(uStatusBarIconsZoomSelection), CSize(16, 16));

   UINT uiCallout[] = {ID_GRAPHICALOBJECT_CALLOUT,-1,-1,-1,-1,-1,-1,-1};
   pCommandBars->GetImageManager()->SetIcons(IDB_CALLOUTS, uiCallout, _countof(uiCallout), CSize(32, 32));

   UINT uiGraphicalObj[] = {ID_GROUP_GRAPHICAL_OBJECTS,-1,-1,-1,-1,-1,-1,-1};
   pCommandBars->GetImageManager()->SetIcons(IDB_CALLOUTS, uiGraphicalObj, _countof(uiGraphicalObj), CSize(32, 32));

   UINT uiDemoObj[] = {ID_DEMOOBJECT,-1,-1,-1,-1,-1,-1,-1};
   pCommandBars->GetImageManager()->SetIcons(IDB_DEMOOBJECTS, uiDemoObj, _countof(uiDemoObj), CSize(32, 32));

   UINT uiDemoGroup[] = {ID_GROUP_DEMO_OBJECTS,-1,-1,-1,-1,-1,-1,-1};
   pCommandBars->GetImageManager()->SetIcons(IDB_DEMOOBJECTS, uiDemoGroup, _countof(uiDemoGroup), CSize(32, 32));

   UINT uiAlignLeft[] = {IDC_FONT_ALIGN_LEFT_E};
   pCommandBars->GetImageManager()->SetIcons(IDB_ALIGN_LEFT, uiAlignLeft, _countof(uiAlignLeft), CSize(16, 16));
   UINT uiAlignCenter[] = {IDC_FONT_ALIGN_CENTER_E};
   pCommandBars->GetImageManager()->SetIcons(IDB_ALIGN_CENTER, uiAlignCenter, _countof(uiAlignCenter), CSize(16, 16));
   UINT uiAlignRight[] = {IDC_FONT_ALIGN_RIGHT_E};
   pCommandBars->GetImageManager()->SetIcons(IDB_ALIGN_RIGHT, uiAlignRight, _countof(uiAlignRight), CSize(16, 16));

   UINT uiGODefaultOption[] = {ID_GODEF_OPTIONS};
   pCommandBars->GetImageManager()->SetIcons(IDB_GODEF_OPTIONS, uiGODefaultOption, _countof(uiGODefaultOption), CSize(123, 105));

}
void CRibbonBars::SetRibbonBarAVisibility(bool bRibbonBarAVisibility)
{
   ShowAssistantRibbonTabs(bRibbonBarAVisibility);
   m_bRibbonBarAVisibility = bRibbonBarAVisibility;
}
void CRibbonBars::SetRibbonBarEVisibility(bool bRibbonBarEVisibility)
{
   ShowEditorRibbonTabs(bRibbonBarEVisibility);
   m_bRibbonBarEVisibility = bRibbonBarEVisibility;
}

bool CRibbonBars::IsRibbonBarAVisibile()
{
   return m_bRibbonBarAVisibility ;
}
bool CRibbonBars::IsRibbonBarEVisibile()
{
   return m_bRibbonBarEVisibility ;
}


HRESULT CRibbonBars::ShowAssistantRibbonTabs(bool bShow)
{
   HRESULT hr = S_OK;

   if (SUCCEEDED(hr)) {
      if(m_pTabStartAssistant)
         m_pTabStartAssistant->SetVisible(bShow);
      else
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr)) {
      if(m_pTabModify)
         m_pTabModify->SetVisible(bShow);
      else
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr)) {
      if(m_pTabViewAssistant)
         m_pTabViewAssistant->SetVisible(bShow);
      else
         hr = E_FAIL;
   }

   return hr;
}

HRESULT CRibbonBars::ShowEditorRibbonTabs(bool bShow)
{
   HRESULT hr = S_OK;

   if (SUCCEEDED(hr)) {
      if(m_pTabStartEditor)
         m_pTabStartEditor->SetVisible(bShow);
      else
         hr = E_FAIL;
   }
   if (SUCCEEDED(hr)) {
      if(m_pTabInsert)
         m_pTabInsert->SetVisible(bShow);
      else
         hr = E_FAIL;
   }
   if (SUCCEEDED(hr)) {
      if(m_pTabViewEditor)
         m_pTabViewEditor->SetVisible(bShow);
      else
         hr = E_FAIL;
   }
   return hr;
}
CXTPRibbonBar* CRibbonBars::GetRibbonbar()
{
   return m_pRibbonBar;
}
void CRibbonBars::ReInitQuickAccessControlsAssistant()
{
   CXTPOriginalControls *pOriginalControls =  m_pRibbonBar->GetQuickAccessControls()->GetOriginalControls();
   if(pOriginalControls!=NULL)
   {
      pOriginalControls->RemoveAll();
   }
   GetCommandBars()->GetShortcutManager()->SetAccelerators(IDR_MAINFRAME);
   m_pRibbonBar->GetQuickAccessControls()->RemoveAll();
   LoadQuickAccessControlsAssistant();
   
}
void CRibbonBars::ReInitQuickAccessControlsEditor()
{
   GetCommandBars()->GetShortcutManager()->SetAccelerators(IDR_MAINFRAME_E);
   if(IsLoaded()==false)
   {
      RemoveAssistantQuickAccessControls();
      CXTPOriginalControls *pOriginalControls =  m_pRibbonBar->GetQuickAccessControls()->GetOriginalControls();
      if(pOriginalControls!=NULL)
      {
         pOriginalControls->RemoveAll();
      }
      m_pRibbonBar->GetQuickAccessControls()->RemoveAll();
      LoadQuickAccessControlsEditor();
      SetLoaded(true);
   }
}
void CRibbonBars::SaveQuickAccessControlsAssistant()
{
   if(IsSaved() == false)
   {
      CXTPControls* pControls = m_pRibbonBar->GetQuickAccessControls();
      pControls->CreateOriginalControls();
      CXTPOriginalControls* pOriginalControls = m_pRibbonBar->GetQuickAccessControls()->GetOriginalControls();
      UINT nCount = pOriginalControls->GetCount();
      UINT nID = 0;
      UINT nIndexControl = 0;
      CString csControlName;
      for (int i = 0; i < nCount; i++)
      {
         CXTPControl* pControl = pOriginalControls->GetAt(i);
         nID = pControl->GetID();
         csControlName.Format(_T("Control[%d]"), i);
         AfxGetApp()->WriteProfileInt(_T("QuickAccessAssistant"), csControlName, nID);
         nIndexControl++;
      }
      AfxGetApp()->WriteProfileInt(_T("QuickAccessAssistant"), _T("Count"), nIndexControl);
      SetSaved(true);
   }
}

void CRibbonBars::SaveQuickAccessControlsEditor()
{
   CXTPControls* pControls = m_pRibbonBar->GetQuickAccessControls();
   pControls->CreateOriginalControls();
   CXTPOriginalControls* pOriginalControls = m_pRibbonBar->GetQuickAccessControls()->GetOriginalControls();
   UINT nCount = pOriginalControls->GetCount();
   UINT nID = 0;

   UINT nIndexControl=0;
   CString csControlName;   

   for (int i = 0; i < nCount; i++)
   {  
      CXTPControl* pControl = pOriginalControls->GetAt(i);
      nID = pControl->GetID();
      csControlName.Format(_T("Control[%d]"), nIndexControl);
      AfxGetApp()->WriteProfileInt(_T("QuickAccessEditor"), csControlName, nID);
      nIndexControl ++;
   }
   AfxGetApp()->WriteProfileInt(_T("QuickAccessEditor"), _T("Count"), nIndexControl);
}
void CRibbonBars::LoadQuickAccessControlsAssistant()
{
   UINT nCount = 0;
   UINT nID = 0;
   CString csControlName;  
   CXTPControl *pControl=NULL;
   XTPControlType  iControlType = xtpControlButton;
   m_pRibbonBar->GetQuickAccessControls()->RemoveAll();
   DWORD dwCount = AfxGetApp()->GetProfileInt(_T("QuickAccessAssistant"), _T("Count"), nCount);
   CXTPControls* pControls = m_pRibbonBar->GetQuickAccessControls();
   if(dwCount>0)
   {
      for (int i = 0; i < dwCount; i++)
      {
         csControlName.Format(_T("Control[%d]"), i);
         DWORD dwID = AfxGetApp()->GetProfileInt(_T("QuickAccessAssistant"), csControlName, nID);
         CXTPControl *pControlRet = m_pRibbonBar->GetQuickAccessControls()->FindControl(dwID);
         if(pControlRet==NULL)
         {  
            if(m_pRibbonBar!=NULL)
            {
               CXTPControls * pControls = m_pRibbonBar->GetControls();
               CXTPControl * pControlRibbon;
            
               if(pControls!=NULL)
               {
                  pControlRibbon= pControls->FindControl(dwID);
                  if(pControlRibbon!=NULL)
                  {
                     iControlType = pControlRibbon->GetType();
                     pControlRibbon->SetIconSize(CSize(16,16));
                     pControlRibbon->SetVisible(TRUE);
                     pControlRibbon->RedrawParent();
                     pControlRibbon->SetEnabled(TRUE);
                     pControl = m_pRibbonBar->GetQuickAccessControls()->AddClone(pControlRibbon,i,TRUE/*iControlType, dwID*/);
                     if((pControl->GetStyle()==xtpButtonIconAndCaptionBelow)||
                        (pControl->GetStyle()==xtpButtonIconAndCaption))
                     {
                        pControl->SetStyle(xtpButtonIcon);
                        pControl->SetIconSize(CSize(16,16));
                     }
                     if(dwID == ID_GRID_SPACING)
                     {
                        CString csCaption;
                        csCaption.LoadStringW(IDS_GRID_SPACING);
                        csCaption.Replace(_T(":"),_T(""));
                        pControl->SetCaption(csCaption);
                     }
                     //pControl->SetHideFlags(~xtpHideRibbonTab);
                     pControl->SetHideFlag(xtpHideRibbonTab,FALSE);
                     pControl->SetHideFlag(xtpHideGeneric,FALSE);
                     pControl->SetEnabled(TRUE);
                     pControl->SetVisible(TRUE);
                     pControl->RedrawParent();
                     CXTPRibbonTab*pRibbonTab = m_pRibbonBar->GetSelectedTab();
                     m_pTabStartAssistant->Select();
                     m_pTabTools->Select();
                     m_pTabViewAssistant->Select();
                     pRibbonTab->Select();
                  }
                  else
                  {
                     iControlType = xtpControlButton;
                     pControl = m_pRibbonBar->GetQuickAccessControls()->Add(iControlType, dwID);
                     pControl->SetStyle(xtpButtonAutomatic);
                     if(dwID ==IDS_SHOW_IMC_WEBPAGE_TUTORIALS)
                     {
                        pControl->SetCaption(IDS_SHOW_IMC_CAPTION_TUTORIALS);
                     }
                     if(dwID ==IDS_SHOW_IMC_WEBPAGE_GUIDED_TOUR)
                     {
                        pControl->SetCaption(IDS_SHOW_IMC_CAPTION_GUIDED_TOUR);
                     }
                     if(dwID ==IDS_SHOW_IMC_WEBPAGE_COMUNITY)
                     {
                        pControl->SetCaption(IDS_SHOW_IMC_CAPTION_COMUNITY);
                     }
                     if(dwID ==IDS_SHOW_IMC_WEBPAGE_BUYNOW)
                     {
                        pControl->SetCaption(IDS_SHOW_IMC_CAPTION_BUYNOW);
                     }
                  }
               }
            }
         }
      }
   }
   else if(dwCount==0)
   {
      m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_LSD_OPEN);
      m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_LSD_NEW);
      m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_PAGE_NEW);
     AddAssistantQuickAccessControls(FALSE);
   }
   //AddVisibleAssistantQuickAccessControls(TRUE);

   //RemoveEditorQuickAccessControls();
   UINT uQuickAccessBar[] = {ID_FILE_NEW,ID_LSD_OPEN,ID_LSD_SAVE,ID_LSD_NEW,ID_PAGE_NEW,-1,-1,-1,-1,ID_EDIT_UNDO,-1,ID_EDIT_REDO , 
                             -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                             -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                             -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                             -1,-1,-1,-1, ID_START_RECORDING};
   m_pRibbonBar->GetQuickAccessControls()->GetCommandBars()->GetImageManager()->SetIcons(IDR_PNG_QUICKACCES, uQuickAccessBar, _countof(uQuickAccessBar), CSize(16, 16));
   m_pRibbonBar->GetQuickAccessControls()->CreateOriginalControls();
   m_pRibbonBar->GetQuickAccessControls()->GetCommandBars()->GetImageManager()->RefreshAll();
   m_pRibbonBar->GetQuickAccessControls()->GetCommandBars()->UpdateCommandBars();
   m_pRibbonBar->GetQuickAccessControls()->GetCommandBars()->RedrawCommandBars();
   m_pRibbonBar->GetPaintManager()->RefreshMetrics();
   
   m_pRibbonBar->OnRecalcLayout();
   
   m_pRibbonBar->RedrawWindow();
   m_pRibbonBar->UpdateData();
   SaveQuickAccessControlsAssistant();
}
void CRibbonBars::LoadQuickAccessControlsEditor()
{
   UINT nCount = 0;
   UINT nID = 0;
   CXTPControl *pControl=NULL;
   XTPControlType  iControlType = xtpControlButton;
   CString csControlName;
   RemoveAssistantQuickAccessControls();
   m_pRibbonBar->GetQuickAccessControls()->RemoveAll();
   DWORD dwCount = AfxGetApp()->GetProfileInt(_T("QuickAccessEditor"), _T("Count"), nCount);
   CXTPControls* pControls = m_pRibbonBar->GetQuickAccessControls();

   if(dwCount>0)
   {
      for (int i = 0; i < dwCount; i++)
      {
         csControlName.Format(_T("Control[%d]"), i);
         DWORD dwID = AfxGetApp()->GetProfileInt(_T("QuickAccessEditor"), csControlName, nID);
         if (dwID == IDC_UNDO || dwID == IDC_REDO)
             continue;
         CXTPControl *pControlRet = m_pRibbonBar->GetQuickAccessControls()->FindControl(dwID);
         if(pControlRet==NULL)
         {
            if(m_pRibbonBar!=NULL)
            {
               CXTPControls * pControls = m_pRibbonBar->GetControls();
               CXTPControl * pControlRibbon;
               if(pControls!=NULL)
               {
                  pControlRibbon= pControls->FindControl(dwID);
                  if(pControlRibbon!=NULL)
                  {
                     iControlType = pControlRibbon->GetType();
                     pControlRibbon->SetIconSize(CSize(16,16));
                     pControlRibbon->SetVisible(TRUE);
                     pControlRibbon->RedrawParent();
                     pControlRibbon->SetEnabled(TRUE);
                     pControl = m_pRibbonBar->GetQuickAccessControls()->AddClone(pControlRibbon/*,i,TRUE*//*iControlType, dwID*/);
                     if((pControl->GetStyle()==xtpButtonIconAndCaptionBelow)||
                        (pControl->GetStyle()==xtpButtonIconAndCaption))
                     {
                        pControl->SetStyle(xtpButtonIcon);
                        pControl->SetIconSize(CSize(16,16));

                     }
                     CXTPRibbonTab*pRibbonTab = m_pRibbonBar->GetSelectedTab();
                     m_pTabStartEditor->Select();
                     m_pTabInsert->Select();
                     m_pTabViewEditor->Select();
                     pRibbonTab->Select();
                     pControl->SetEnabled(TRUE);
                     pControl->SetVisible(TRUE);
                     pControl->RedrawParent();
                  }
                  else
                  {
                     iControlType = xtpControlButton;
                     pControl = m_pRibbonBar->GetQuickAccessControls()->Add(iControlType, dwID);
                     pControl->SetStyle(xtpButtonAutomatic);
                  }
               }
            }
         }
      }
   }
   else if(dwCount==0)
   {
      if(pControls->FindControl(ID_FILE_SAVE)==NULL)
      {
         m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_FILE_SAVE);
      }
      /*if(pControls->FindControl(IDC_UNDO)==NULL)
      {
         m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, IDC_UNDO);
      }
      if(pControls->FindControl(IDC_REDO)==NULL)
      {
         m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, IDC_REDO);
      }*/
   }
   m_pRibbonBar->GetQuickAccessControls()->CreateOriginalControls();
   m_pRibbonBar->GetQuickAccessControls()->GetCommandBars()->GetImageManager()->RefreshAll();
   m_pRibbonBar->GetQuickAccessControls()->GetCommandBars()->UpdateCommandBars();
   m_pRibbonBar->GetQuickAccessControls()->GetCommandBars()->RedrawCommandBars();
   m_pRibbonBar->GetPaintManager()->RefreshMetrics();
   
   m_pRibbonBar->OnRecalcLayout();
   
   m_pRibbonBar->RedrawWindow();
   m_pRibbonBar->UpdateData();
   SaveQuickAccessControlsEditor();

}
void CRibbonBars::GetTooltipComponents(UINT nID, CString &csTitle, CString &csDescription) {
    CString csFromResources;
    csFromResources.LoadString(nID);

    int iPos = csFromResources.ReverseFind(_T('\n'));

    csTitle = csFromResources.Right(csFromResources.GetLength() - iPos - 1);
    csDescription = csFromResources.Left(iPos);
}

void CRibbonBars::AddTooltip(CXTPControl* pControl, UINT nID)
{
   CString csTitle, csDescription;
   GetTooltipComponents(nID, csTitle, csDescription);

   pControl->SetTooltip(csTitle);
   pControl->SetDescription(csDescription);
}
bool CRibbonBars::FindQuickAccessControl(UINT nID,CXTPControl *pControl)
{
   CXTPControls* pControls =m_pRibbonBar->GetQuickAccessControls();
   for(int i = 0; i < pControls->GetCount(); i++)
   {
      pControl = pControls->GetAt(i);
      if(pControl && pControl->GetID() == nID)
      {
         return true;
         break;
      }
   }
   return false;
}

void CRibbonBars::SetVisibleQuickAccessControl(UINT nID,BOOL bVisible)
{
   bool bFound=false;
   CXTPControls* pControls = m_pRibbonBar->GetQuickAccessControls();
   CXTPControl* pControl = pControls->FindControl(xtpControlButton,nID,!bVisible,FALSE);
   if(pControl!=NULL)
   {
      pControl->SetVisible(bVisible);
   }
}
void CRibbonBars::RemoveEditorQuickAccessControls()
{
   CXTPControls* pControls = m_pRibbonBar->GetQuickAccessControls();
   if(pControls!=NULL)
   {
      CXTPControl* pControl = pControls->FindControl(ID_FILE_SAVE);
      if(pControl!=NULL)
      {
         pControls->Remove(pControl);
      }

      pControl = pControls->FindControl(IDC_UNDO);
      if(pControl!=NULL)
      {
         pControls->Remove(pControl);
      }
      pControl = pControls->FindControl(IDC_REDO);
      if(pControl!=NULL)
      {
         pControls->Remove(pControl);
      }
      UINT nCount=0;
      UINT nID=0;
      CString csControlName;
      DWORD dwCount = AfxGetApp()->GetProfileInt(_T("QuickAccessEditor"), _T("Count"), nCount);
      if(dwCount>0)
      {
         for (int i = 0; i < dwCount; i++)
         {
            csControlName.Format(_T("Control[%d]"), i);

            DWORD dwID = AfxGetApp()->GetProfileInt(_T("QuickAccessEditor"), csControlName, nID);
            pControl = pControls->FindControl(dwID);
            if(pControl != NULL)
            {
               pControls->Remove(pControl);
            }
         }
      }
   }
}
void CRibbonBars::RemoveAssistantQuickAccessControls()
{
   CXTPControls* pControls = m_pRibbonBar->GetQuickAccessControls();
   if(pControls!=NULL)
   {
      CXTPControl* pControl = pControls->FindControl(ID_LSD_SAVE);
      if(pControl!=NULL)
      {
         pControls->Remove(pControl);
      }

      pControl = pControls->FindControl(ID_EDIT_UNDO);
      if(pControl!=NULL)
      {
         pControls->Remove(pControl);
      }
#ifdef _REDO
      pControl = pControls->FindControl(ID_EDIT_REDO);
      if(pControl!=NULL)
      {
         pControls->Remove(pControl);
      }
#endif
      pControl = pControls->FindControl(ID_LSD_OPEN);
      if(pControl!=NULL)
      {
         pControls->Remove(pControl);
      }
      pControl = pControls->FindControl(ID_LSD_NEW);
      if(pControl!=NULL)
      {
         pControls->Remove(pControl);
      }
      pControl = pControls->FindControl(ID_PAGE_NEW);
      if(pControl!=NULL)
      {
         pControls->Remove(pControl);
      }
   }
}
void CRibbonBars::AddAssistantQuickAccessControls(BOOL bVisible)
{
   CXTPControls* pControls = m_pRibbonBar->GetQuickAccessControls();
   if(pControls!=NULL)
   {
      CXTPControl* pControl = pControls->FindControl(xtpControlButton,ID_LSD_SAVE,!bVisible,FALSE);
      if(pControl==NULL)
      {
         CXTPControl* pLsdSaveControl = m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_LSD_SAVE);
         pLsdSaveControl->SetVisible(bVisible);
      }

      pControl = pControls->FindControl(xtpControlButton,ID_EDIT_UNDO,!bVisible,FALSE);
      if(pControl==NULL)
      {
         CXTPControl* pEditUndoControl = m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_EDIT_UNDO);
         pEditUndoControl->SetVisible(bVisible);
      }
#ifdef _REDO
      pControl = pControls->FindControl(xtpControlButton,ID_EDIT_REDO,!bVisible,FALSE);
      if(pControl==NULL)
      {
         CXTPControl* pEditRedoControl = m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_EDIT_REDO);
         pEditRedoControl->SetVisible(bVisible);
      }
#endif
   }
}
void CRibbonBars::AddVisibleAssistantQuickAccessControls(BOOL bVisible)
{
   CXTPControls* pControls = m_pRibbonBar->GetQuickAccessControls();
   if(pControls!=NULL)
   {
      CXTPControl* pControl = pControls->FindControl(xtpControlButton,ID_LSD_OPEN,bVisible,FALSE);
      if(pControl==NULL)
      {
         CXTPControl* pLsdOpenControl = m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_LSD_OPEN);
         pLsdOpenControl->SetVisible(bVisible);
      }

      pControl = pControls->FindControl(xtpControlButton,ID_LSD_NEW,bVisible,FALSE);
      if(pControl==NULL)
      {
         CXTPControl* pLsdNewControl = m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_LSD_NEW);
         pLsdNewControl->SetVisible(bVisible);
      }

      pControl = pControls->FindControl(xtpControlButton,ID_PAGE_NEW,bVisible,FALSE);
      if(pControl==NULL)
      {
         CXTPControl* pPageNewControl = m_pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_PAGE_NEW);
         pPageNewControl->SetVisible(bVisible);
      }
   }
}
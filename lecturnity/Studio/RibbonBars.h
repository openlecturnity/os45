#ifndef _RIBBONBARS_H
#define _RIBBONBARS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef _STUDIO
#include "..\Assistant2_0\MainFrm.h"
#include "..\editor\MainFrm.h"
#include "MainFrm.h"
#else
#ifdef _ASSISTANT
#include "..\Assistant2_0\MainFrm.h"
#else
#ifdef _EDITOR
#include "..\editor\MainFrm.h"
#endif
#endif
#endif

class CRibbonBars
{
public:
   CRibbonBars();
#ifdef _STUDIO
   BOOL CreateRibbonBars(CXTPCommandBars* pCommandBarsParam, HWND m_hWnd, CMainFrame * pMainFrame);
#else
#ifdef _ASSISTANT
   BOOL CreateRibbonBars(CXTPCommandBars* pCommandBarsParam, HWND m_hWnd, CMainFrameA * pMainFrame);
#else
#ifdef _EDITOR
   BOOL CreateRibbonBars(CXTPCommandBars* pCommandBarsParam, HWND m_hWnd, CMainFrameE * pMainFrame);
#endif
#endif
#endif
   CXTPControl *GetControlOnRibbonBar(UINT nGroupId, UINT nControlId);
   CXTPCommandBars* GetCommandBars(); 
   CXTPRibbonBar* GetRibbonbar();
   void SetCommandBars(CXTPCommandBars* m_pCommandBarsParam);
   void SetRibbonBarAVisibility(bool bRibbonBarAVisibility);
   void SetRibbonBarEVisibility(bool bRibbonBarEVisibility);
   bool IsRibbonBarAVisibile();
   bool IsRibbonBarEVisibile();
   void ReInitQuickAccessControlsAssistant();
   void ReInitQuickAccessControlsEditor();
   void SaveQuickAccessControlsAssistant();
   void SaveQuickAccessControlsEditor();
   void LoadQuickAccessControlsAssistant();
   void SetVisibleQuickAccessControl(UINT nID,BOOL bVisible);
public:
   //inline  members
   void SetSaved(bool bSaved) {m_bSaved = bSaved;}
   bool IsSaved() { return m_bSaved;}

   void SetLoaded(bool bLoaded) {m_bLoaded = bLoaded;}
   bool IsLoaded() { return m_bLoaded;}
   
   CXTPRibbonTab * GetTabStartAssistant() {return m_pTabStartAssistant;}
   CXTPRibbonTab * GetTabEditAssistant() {return m_pTabTools;}
   CXTPRibbonTab * GetTabViewAssistant() {return m_pTabViewAssistant;}

   CXTPRibbonTab * GetTabStartEditor() {return m_pTabStartEditor;}
   CXTPRibbonTab * GetTabInsertEditor() {return m_pTabInsert;}
   CXTPRibbonTab * GetTabViewEditor() {return m_pTabViewEditor;}
public:
   ~CRibbonBars(void);
private:
   void CreateTabTools(CXTPRibbonBar* pRibbon);
   void CreateTabViewAssistant(CXTPRibbonBar* pRibbon);
   void CreateTabStartAssistant(CXTPRibbonBar* pRibbon);
   void CreateTabStartEditor(CXTPRibbonBar* pRibbon);
   void CreateTabInsert(CXTPRibbonBar* pRibbon);
   void CreateTabViewEditor(CXTPRibbonBar* pRibbon);
   void AddVisibleAssistantQuickAccessControls(BOOL bVisible); 
   void AddAssistantQuickAccessControls(BOOL bVisible);
   void LoadQuickAccessControlsEditor();
   bool FindQuickAccessControl(UINT nID,CXTPControl* pControlParam);
   void RemoveAssistantQuickAccessControls();
   void RemoveEditorQuickAccessControls(); 
   void LoadRibbonIconsAssistant();
   void LoadRibbonIconsEditor();
   HRESULT ShowAssistantRibbonTabs(bool bShow);
   HRESULT ShowEditorRibbonTabs(bool bShow);
   CXTPCommandBars* m_pCommandBars;
   CXTPRibbonBar* m_pRibbonBar;

   CXTPRibbonTab* m_pTabStartAssistant;
   CXTPRibbonTab* m_pTabStartEditor;

   CXTPRibbonTab* m_pTabViewAssistant;
   CXTPRibbonTab* m_pTabViewEditor;

   CXTPRibbonTab* m_pTabModify;
   CXTPRibbonTab* m_pTabInsert;

   CXTPRibbonTab* m_pTabTools;

   bool m_bRibbonBarAVisibility;
   bool m_bRibbonBarEVisibility;
   bool m_bSaved;
   bool m_bLoaded;
public:
   void AddTooltip(CXTPControl* pControl, UINT nID);
   static void GetTooltipComponents(UINT nID, CString &csTitle, CString &csDescription);
};
#endif

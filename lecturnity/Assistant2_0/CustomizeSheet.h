#pragma once


// CCustomizeSheet dialog

class CCustomizeSheet : public CXTPPropertyPage
{
	DECLARE_DYNAMIC(CCustomizeSheet)

public:
	CCustomizeSheet();
   CCustomizeSheet(CXTPCommandBars * pCommandBars);
	virtual ~CCustomizeSheet();

   CXTPRibbonBar* GetRibbonBar();
   BOOL AddCategories(UINT nIDResource,  BOOL bListSubItems = FALSE);
	BOOL AddCategories(CXTPControls* pControls);
	BOOL AddCategory(LPCTSTR strCategory, CMenu* pMenu, BOOL bListSubItems = FALSE);
	CXTPControls* InsertCategory(LPCTSTR strCategory, int nIndex = -1);
	CXTPControls* GetControls(LPCTSTR strCategory);
   BOOL AddTab(int iTabIndex);
   BOOL AddControlsFromGroup(CXTPControls * pCategoryControls, CXTPRibbonGroup *pGroup, BOOL bListSubItems);
   
   bool IsAssistantMenuID(UINT nID);
   bool IsEditorMenuID(UINT nID);
  
// Dialog Data
	enum { IDD = IDD_DIALOG_CUSTOMIZE_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   afx_msg void OnKickIdle();
   afx_msg void OnCheckShowQuickAccessBelow();
	afx_msg void OnCategoriesSelectionChanged();
	afx_msg void OnDblclkListQuickAccess();
	afx_msg void OnDblclkListCommands();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonReset();
	afx_msg void OnCommandsSelectionChanged();
	afx_msg void OnQuickAccessSelectionChanged();
	DECLARE_MESSAGE_MAP()
protected:
   void RefreshQuickAccessList();

public:
   virtual BOOL OnInitDialog();
public:
   virtual void OnOK();
   virtual void OnCancel();

private:
	XTP_COMMANDBARS_CATEGORYINFO* FindCategory(LPCTSTR strCategory) const;
	XTP_COMMANDBARS_CATEGORYINFO* GetCategoryInfo(int nIndex);
    CFont m_font;

public:
	CComboBox   m_lstCategories;            // Categories list
	CXTPCustomizeCommandsListBox    m_lstQuickAccess;   // Quick access list
	CXTPCustomizeCommandsListBox    m_lstCommands;      // Commands list
	BOOL    m_bShowQuickAccessBelow;                    // TRUE to show quick access below ribbon

protected:
	//CXTPCustomizeSheet* m_pSheet;           // Parent Sheet window
	CXTPCommandBars* m_pCommandBars;        // Parent CommandBars object
	CXTPCommandBarsCategoryArray m_arrCategories;   // Array of categories.
   CXTPPropertyPageStaticCaption m_caption;
   bool m_bSomethingChanged;
public:
   afx_msg void OnBnClickedButtonCqaUp();
public:
   afx_msg void OnBnClickedButtonCqaDown();
};

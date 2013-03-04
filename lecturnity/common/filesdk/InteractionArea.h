#if !defined(AFX_INTERACTIONAREA_H__ACF4F831_13D0_480D_9EEE_E150F0FAB2ED__INCLUDED_)
#define AFX_INTERACTIONAREA_H__ACF4F831_13D0_480D_9EEE_E150F0FAB2ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SgmlParser.h"
#include "..\drawsdk\objects.h"

#define E_IA_UNINITIALIZED _HRESULT_TYPEDEF_(0x80d10001L)
#define E_IA_INVALID_DATA  _HRESULT_TYPEDEF_(0x80d10002L)

class FILESDK_EXPORT CLcElementInfo {
public:
    CLcElementInfo();
    virtual ~CLcElementInfo();

    CLcElementInfo* Copy();
    CRect GetArea(){return m_rcArea;};
    CString GetVisibility(){return m_csVisibility;};
    CString GetName(){return m_csName;};
    CString GetProcessName(){return m_csProcessName;};
    //CString GetTimestamp(){return m_csTimeStamp;};
    CPoint GetPtLocation(){return m_ptLocation;};
    CPoint GetPtSize(){return m_ptSize;};
    CString GetAccType(){return m_csAccType;};
    CString GetSapType(){return m_csSapType;};
    CString GetIdAcc(){return m_csIdAcc;};
    CString GetIdUiAuto(){return m_csIdUiAuto;};
    CString GetIdUiAutoTree(){return m_csIdUiAutoTree;};
    CString GetIdSap(){return m_csIdSap;};

    void SetArea(CRect rcArea){m_rcArea = rcArea;};
    void SetVisibility(CString csVisibility){m_csVisibility = csVisibility;};
    void SetName(CString csName){m_csName = csName;};
    void SetProcessName(CString csProcessName){m_csProcessName = csProcessName;};
    //void SetTimestamp(CString csTimeStamp){m_csTimeStamp = csTimeStamp;};
    void SetPtLocation(CPoint ptLocation){m_ptLocation = ptLocation;};
    void SetPtSize(CPoint ptSize){m_ptSize = ptSize;};
    void SetAccType(CString csAccType){m_csAccType = csAccType;};
    void SetSapType(CString csSapType){m_csSapType = csSapType;};
    void SetIdAcc(CString csIdAcc){m_csIdAcc = csIdAcc;};
    void SetIdUiAuto(CString csIdUiAuto){m_csIdUiAuto = csIdUiAuto;};
    void SetIdUiAutoTree(CString csIdUiAutoTree){m_csIdUiAutoTree = csIdUiAutoTree;};
    void SetIdSap(CString csIdSap){m_csIdSap = csIdSap;};
    void SetElementInfo(CLcElementInfo *lcElementInfo);
    bool operator == (CLcElementInfo& ) const;
    bool operator != (CLcElementInfo& ) const;
private:
    CRect m_rcArea;
    CString m_csVisibility;
    CString m_csName;
    CString m_csProcessName;
    CPoint  m_ptLocation;
    CPoint  m_ptSize;
    CString m_csAccType;
    CString m_csSapType;

    // internal ids only necessary for LC; passed back
    CString m_csIdAcc;
    CString m_csIdUiAuto;
    CString m_csIdUiAutoTree;
    CString m_csIdSap;

};
class FILESDK_EXPORT CInteractionArea  
{
public:
	CInteractionArea();
	virtual ~CInteractionArea();

   HRESULT ReadFromSgml(SgmlElement *pSgml, CString& csRecordPath);
   HRESULT Init(DrawSdk::DrawObject *pSupportObject);

   bool IsVisibleAt(UINT nTimestampMs);
   UINT GetVisibilityStart() { return m_nVisibilityStartMs; }
   UINT GetVisibilityEnd() { return m_nVisibilityEndMs; }

   void Draw(HDC hdc, double dOffX = 0.0, double dOffY = 0.0, DrawSdk::ZoomManager *pZoom = NULL);

   bool IsInitialized() { return m_bIsInitialized; }
   CRect& GetActivityArea() { return m_rcDimensions; }
   bool IsHandCursor() { return m_bHandCursor; }
   CString& GetVisibility() { return m_csVisibility; }
   CString& GetActivity() { return m_csActivity; }
   
   CString& GetMouseClickAction() { return m_csClickAction; }
   CString& GetMouseDownAction() { return m_csDownAction; }
   CString& GetMouseUpAction() { return m_csUpAction; }
   CString& GetMouseEnterAction() { return m_csEnterAction; }
   CString& GetMouseLeaveAction() { return m_csLeaveAction; }

   bool IsButton() { return m_bIsButton; }
   bool IsRadioButton() { return m_bIsRadio; }
   bool IsCheckBox() { return m_bIsCheck; }
   bool IsTextField() { return m_bIsField; }
   bool IsSupport() { return m_bIsSupport; }
   bool IsCallout() { return m_bIsCallout; }
   bool IsClickObject() { return m_bIsClickObject; }
   bool HasClickObject() { return m_bHasClickObject; }
   bool IsUserEdited() {return m_bIsUserEdited;}
   CString GetRtf(){return m_csRtf;}
   /**
    * If true, this object has been automatically inserted when creating demo document.
    * If true, when this object is deleted will require that all demo document objets be deleted.
    */
   bool IsDemoDocumentObject(){ return m_bDemoDocumentObject;};

   CString& GetCorrectText() { return m_csCorrectText; }
   HRESULT GetCorrectTexts(CArray<CString, CString> *paCorrectTexts);
   bool HasToBeChecked() { return m_bHasToBeChecked; }
   DrawSdk::DrawObject *GetSupportObject() { return m_pSupportObject; }

   CString& GetButtonType() { return m_csButtonType; }

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>* GetNormalObjects(); 
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>* GetOverObjects(); 
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>* GetPressedObjects(); 
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>* GetInactiveObjects(); 
   // Visibility/Activity times are stored in an Array of CPoints as pairs of beginning/end timestamps:
   HRESULT GetVisibilityTimes(CArray<CPoint, CPoint>* paVisibilityTimePeriods);
   HRESULT GetActivityTimes(CArray<CPoint, CPoint>* paActivitiyTimePeriods);

   CLcElementInfo* GetLcElementInfo(){return m_pLcElementInfo;};

private:
   CRect m_rcDimensions;
   bool m_bHandCursor;
   CString m_csClickAction;
   CString m_csDownAction;
   CString m_csUpAction;
   CString m_csEnterAction;
   CString m_csLeaveAction;
   CString m_csVisibility;
   CString m_csActivity;
   bool m_bIsButton;
   bool m_bIsRadio;
   bool m_bIsCheck;
   bool m_bIsField;
   bool m_bIsSupport;
   bool m_bIsCallout;
   bool m_bIsUserEdited;
   CString m_csRtf;

   bool m_bIsClickObject;
   bool m_bHasClickObject;
   /**
    * If true, this object has been automatically inserted when creating demo document.
    * If true, when this object is deleted will require that all demo document objets be deleted.
    * A new value demoModeObject="true" will be inserted in BUTTON tag when object will be saved.
    */
   bool m_bDemoDocumentObject;
   UINT m_nVisibilityStartMs;
   UINT m_nVisibilityEndMs;

   CString m_csCorrectText;
   bool m_bHasToBeChecked;
   DrawSdk::DrawObject *m_pSupportObject;

   CString m_csButtonType;

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aNormalObjects;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aOverObjects;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aPressedObjects;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aInactiveObjects;
   
   CLcElementInfo* m_pLcElementInfo; 
   HRESULT SetLcElementInfo(SgmlElement *pSgml);

   bool m_bIsInitialized;
   bool m_bObjectsExtracted;
   // Note: m_bObjectsExtracted: this is some kind of memory management or pointer counter:
   // if the objects (passed as pointers) are used somewhere else they
   // must not be deleted here.
};

#endif // !defined(AFX_INTERACTIONAREA_H__ACF4F831_13D0_480D_9EEE_E150F0FAB2ED__INCLUDED_)

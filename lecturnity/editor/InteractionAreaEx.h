#if !defined(AFX_INTERACTIONAREAEX_H__C22F3678_3D81_40AA_B1D9_C3168A47C06A__INCLUDED_)
#define AFX_INTERACTIONAREAEX_H__C22F3678_3D81_40AA_B1D9_C3168A47C06A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArrayOutput.h" // FileSdk
#include "InteractionArea.h" // FileSdk
#include "MarkStream.h"
#include "MediaSegment.h"
#include "MouseAction.h"

class CQuestionnaireEx; // forward declaration; otherwise you'll get a circular include


#define E_IAX_UNINITIALIZED _HRESULT_TYPEDEF_(0x80d20001L)
#define E_IAX_INVALID_DATA  _HRESULT_TYPEDEF_(0x80d20002L)
#define E_IAX_WRITE_FAILED  _HRESULT_TYPEDEF_(0x80d20003L)
#define E_IAX_INVALID_STATE _HRESULT_TYPEDEF_(0x80d20004L)


enum AreaPeriodId
{
   INTERACTION_PERIOD_NEVER,
      INTERACTION_PERIOD_PAGE,
      INTERACTION_PERIOD_END_PAGE,
      INTERACTION_PERIOD_TIME,
      INTERACTION_PERIOD_ALL_PAGES
};

enum InteractionClassId
{
   INTERACTION_CLASS_NONE,
      INTERACTION_CLASS_AREA,
      INTERACTION_CLASS_BUTTON,
      INTERACTION_CLASS_MOVE,
      INTERACTION_CLASS_DYNAMIC,
      INTERACTION_CLASS_CHANGE,
      INTERACTION_CLASS_FEEDBACK,
      INTERACTION_CLASS_BLANKS,
      INTERACTION_CLASS_GRAPHICAL_OBJECT,
      INTERACTION_CLASS_CLICK_OBJECT
};

enum ButtonTypeId
{
   BUTTON_TYPE_NONE,
      BUTTON_TYPE_SUBMIT,
      BUTTON_TYPE_DELETE,
      BUTTON_TYPE_OK,
      BUTTON_TYPE_OTHER
};

enum ButtonDimension
{
   OK_WIDTH = 80, 
   OK_HEIGHT = 20,
   BUTTON_X_OFFSET = 5,
   BUTTON_Y_OFFSET = 5
};

/*Used for grouping items in Interaction/Page Objects view*/
enum InteractionRecordType
{
   INTERACTION_RECORD_TEXT = 0,
   INTERACTION_RECORD_TITLE,
   INTERACTION_RECORD_ANSWERS,
   INTERACTION_RECORD_BUTTONS,
   INTERACTION_RECORD_FEEDBACK,
   INTERACTION_RECORD_STATUS,
   INTERACTION_RECORD_ELEMENTS,
   INTERACTION_RECORD_MISC,
   INTERACTION_RECORD_GRAPHIC_OBJECTS,
   INTERACTION_RECORD_CLICK_OBJECTS,
   INTERACTION_RECORD_UNKNOWN
};
/**
 * Stores one activity or visibility range of an interaction area.
 *
 * You have to adapt the numeric information upon copy and insertion
 * with "DecrementBegin()" and "IncrementBegin()".
 */
class CTimePeriod
{
public:
   CTimePeriod();
   virtual ~CTimePeriod();

   HRESULT Init(AreaPeriodId idPeriod, UINT nPageMarkTime, UINT nPageEndTime, bool bAllowLengthOne = false);
   HRESULT Parse(CString& csTimesDefine);

   HRESULT ResolveTimes(CEditorProject *pProject);

   HRESULT CloneTo(CTimePeriod *pTarget);
   //HRESULT ClonePartTo(CTimePeriod *pTarget, UINT nFromMs, UINT nLengthMs);
   HRESULT EnlargeWith(UINT nFromMs, UINT nLengthMs);
   HRESULT IncreaseLength(UINT nLengthMs);
   HRESULT TrimTo(UINT nFromMs, UINT nLengthMs);
   HRESULT Subtract(UINT nFromMs, UINT nLengthMs);

   AreaPeriodId GetPeriodId() { return m_idPeriod; }
   UINT GetBegin(bool bUsePageTimes = false);
   UINT GetEnd();
   UINT GetLength(bool bUsePageTimes = false);
   HRESULT SetDocumentLength(UINT nLengthMs);

   bool Contains(UINT nPositionMs, bool bUsePageTimes = false);
   bool IntersectsRange(UINT nFromMs, UINT nLengthMs, bool bUsePageTimes = false);
   bool IntersectsRange(CTimePeriod *pOther);
   bool MeetsRange(UINT nFromMs, UINT nLengthMs, bool bUsePageTimes = false);
   bool EntirelyWithin(UINT nFromMs, UINT nLengthMs);
   bool EntirelyWithin(CTimePeriod *pOther);
   bool IsEmpty();

   HRESULT DecrementBegin(UINT nMoveMs);
   HRESULT IncrementBegin(UINT nMoveMs);

   HRESULT AppendTimes(_TCHAR *tszTarget);
   HRESULT AppendTimes(CArrayOutput *pTarget);

   HRESULT RegisterWithMark(CEditorProject *pProject);
   HRESULT UnregisterWithMark(CEditorProject *pProject);

private:
   AreaPeriodId m_idPeriod;
   UINT m_nPageStartMs;
   CMediaSegment m_RawTimes;
};


class CInteractionAreaEx : public CObject
{
   DECLARE_DYNCREATE(CInteractionAreaEx)

public:
	CInteractionAreaEx();
	virtual ~CInteractionAreaEx();
   
   HRESULT Init(CInteractionArea *pInteraction, CEditorProject *pProject, UINT nInsertPositionMs);
   HRESULT ResolveTimes(CEditorProject *pProject, UINT nInsertPositionMs);
   void UnregisterMarks();

   HRESULT Init(CRect *prcRect, bool bHandCursor, CMouseAction *pClickAction,
      CMouseAction *pDownAction, CMouseAction *pUpAction, CMouseAction *pEnterAction, 
      CMouseAction *pLeaveAction, CTimePeriod *pVisibility, CTimePeriod *pActivity, CEditorProject *pProject);

   /**
    * If you want to have a button you can "convert" a normal interaction area to 
    * one with this method. Before of course you have to call the normal Init().
    */
   HRESULT InitButton(ButtonTypeId idButtonType,
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paNormalObjects,
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paOverObjects,
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paPressedObjects,
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paInactiveObjects, bool bIsImageButton);
   // TODO like for CDynamicObject or CChangeableObject (added later) 
   // there should also be an extension class for a button.

   bool IsQuestionObject();

   virtual CInteractionAreaEx* MakeNew();
   CInteractionAreaEx* Copy(bool bCopyHashKey = false);

   ButtonTypeId GetButtonType() { return m_idButtonType; }
   //void SetButtonType(ButtonTypeId idButtonType) { m_idButtonType = idButtonType; }

   virtual HRESULT ChangeAppearance(CQuestionnaireEx *pQuestionnaire);

   virtual void Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing);
   virtual void DrawBorder(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager);

   /**
    * Creates appropriate time information in pTarget for visibility and
    * activity. The hash key of pTarget will be the same so it can be matched
    * upon reinsertion.
    * If bDoDelete is true the present time information will be shortened.
    *
    * @returns S_FALSE if the whole area was consumed by the delete and the area
    *          has not visibility and activity anymore.
    */
   //HRESULT CutOutPartTo(CInteractionAreaEx *pTarget, UINT nFromMs, UINT nLengthMs, bool bDoDelete);

   /**
    * Joins visibility and activity of this area and pArea. Both must have the
    * same hash key and thus the same AreaPeriodId for visibility and activity.
    * With bDryRun set to true you can check if one area will join with another one.
    * If S_FALSE (or an error) is returned the two do not belong to each other.
    */
   HRESULT AddArea(CInteractionAreaEx *pArea, UINT nToMs, bool bIgnoreHash = false);

   bool MeetsWith(CInteractionAreaEx *pArea, UINT nToMs, bool bIgnoreHash = false);
   HRESULT Enlarge(UINT nFromMs, UINT nLengthMs);
   virtual HRESULT IncrementTimes(UINT nMoveMs);
   virtual HRESULT DecrementTimes(UINT nMoveMs);

   HRESULT CorrectActivityUponPaste(UINT nActivityStartMs, UINT nActivityEndMs);
   HRESULT CorrectVisibilityUponPaste(UINT nVisibilityStartMs, UINT nVisibilityEndMs);
   HRESULT SetActivityPeriod(CTimePeriod *pOther);
   virtual HRESULT SetVisibilityPeriod(CTimePeriod *pOther);

   bool SetSelected(bool bSelected);
	bool IsSelected() {return m_bSelected;}

   HRESULT Execute(MouseActionId idMouse, CEditorDoc *pDoc);

   UINT GetHashKey();
   void SetHashKey(UINT nHashKey) { m_nHashKey = nHashKey; }

   bool ContainsPoint(CPoint *pptMousePos);
   bool LiesInsideRect(CRect *prcOtherRect);

   bool Intersects(UINT nRangeStartMs, UINT nRangeLengthMs, bool bUsePageTimes = false);
   bool Intersects(CInteractionAreaEx *pOther);
   bool ActiveIntersects(UINT nRangeStartMs, UINT nRangeLengthMs);
   bool EntirelyWithin(UINT nFromMs, UINT nLengthMs);
   bool NextTo(UINT nFromMs, UINT nLengthMs, bool bUsePageTimes = false);
   bool IsSplitPosition(UINT nPositionMs);

   HRESULT TrimTo(UINT nFromMs, UINT nLengthMs);
   HRESULT RemovePart(UINT nFromMs, UINT nLengthMs);
   CInteractionAreaEx *Split(UINT nSplitMs);

   /**
    * Note: normal areas have no visibility and thus always return false.
    */
   bool VisibleIntersects(UINT nRangeStartMs, UINT nRangeLengthMs);

   virtual CRect& GetArea() { return m_rcDimensions; }
   bool IsHandCursor() { return m_bHandCursor; }
   CMouseAction* GetClickAction() { return m_pClickAction; }
   CMouseAction* GetUpAction() { return m_pUpAction; }
   CMouseAction* GetDownAction() { return m_pDownAction; }
   CMouseAction* GetEnterAction() { return m_pEnterAction; }
   CMouseAction* GetLeaveAction() { return m_pLeaveAction; }
   AreaPeriodId GetVisibilityPeriod();
   UINT GetVisibilityStart();
   UINT GetVisibilityLength();
   AreaPeriodId GetActivityPeriod();
   UINT GetActivityStart();
   UINT GetActivityLength();

   // TODO make private (and CInteractionStream a friend)
   virtual HRESULT SetArea(CRect *prcArea);
   virtual HRESULT Move(float fDiffX, float fDiffY);
   virtual HRESULT AutoMove(float fDiffX, float fDiffY);

   int GetStackingOrder() { return m_iStackingLevel; }
   HRESULT ToFront();
   HRESULT ToBack();
   // TODO make private (and CInteractionStream a friend)
   HRESULT SetStackingOrder(int iStacking) { m_iStackingLevel = iStacking; return S_OK; }
   
   bool IsHiddenDuringEdit() { return m_bHiddenDuringEdit; }
   bool IsTimelineDisplay() { return m_bTimelineDisplay; }
   CRect GetTimelineRect(){return m_rcTimelineRect;}
   bool IsHiddenDuringPreview() { return m_bHiddenDuringPreview; }
   // see m_bDemoDocumentObject description
   bool IsDemoDocumentObject(){return m_bDemoDocumentObject;};
   bool SetHiddenDuringEdit(bool bHidden);
   bool SetTimelineDisplay(bool bVisible);
   void SetTimelineRect(CRect rc){m_rcTimelineRect.CopyRect(rc);};
   bool SetHiddenDuringPreview(bool bHidden);
   // see m_bDemoDocumentObject description
   void SetDemoDocumentObject(bool bDemoDocument){m_bDemoDocumentObject = bDemoDocument;};

   HRESULT FindQuestionId(CEditorProject *pProject);

   bool IsButton() { return m_bIsButton; }
   bool IsImageButton() { return m_bIsImageButton; }
   virtual HRESULT AddDuringParsing(CInteractionArea *pInteraction, CEditorProject *pProject, UINT nInsertPositionMs) {return S_OK;};
 
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *GetNormalObjects() { return &m_aNormalObjects; }
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *GetOverObjects() { return &m_aOverObjects; }
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *GetPressedObjects() { return &m_aPressedObjects; }
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *GetInactiveObjects() { return &m_aInactiveObjects; }

   /**
    * Returns true if the state changed and it needs repainting.
    */
   virtual bool InformMouseStatus(bool bMouseOver, bool bMousePressed);

   virtual bool Activate(UINT nPositionMs, bool bPreviewing, bool bAlwaysActive = false);
   bool DisableForPreview();
   bool IsDisabledForPreview() { return m_bDisabledForPreview; }
   virtual bool Reset(bool bAfterPreview);

   // TODO remove these two (used in CPageObjectsList)
   void GetInteractionType(CString& csType) { csType = GetInteractionType(); }
   void GetInteractionName(CString& csName) { csName = GetInteractionName(); }

   // Be aware to use different display type strings as they are used to identify the type of items.
   CString& GetInteractionType() { return m_csDisplayType; }
   CString& GetInteractionName() { return m_csDisplayName; }
   /**
    * If bChangeTexts == true and IsInteractionNameText() returns true
    * you have to take care of undo.
    */
   virtual HRESULT SetInteractionName(CString &csName, bool bChangeTexts);
   /** 
    * @returns whether or not the "bChangeTexts" of SetInteractionName()
    * has any consequences for the objects.
    */
   virtual bool IsInteractionNameText() { return m_bIsButton; }

   /**
    * Writes the definition of this area to a file.
    * 
    * If it is a button with images:
    * There is a difference between LEP and OBJ saving:
    * - LEP uses the full image path and doesn't copy the image.
    * - OBJ uses just a (new) file name and copies the image to the target directory.
    * Thus when saving OBJ tszExportPrefix must be non-null and bUseFullImagePath
    * must be false and vice versa.
    */
   virtual HRESULT Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, LPCTSTR tszExportPrefix, CEditorProject *pProject);

   virtual InteractionClassId GetClassId();
   virtual _TCHAR* GetSubClassIdent() { return NULL; }
   virtual HRESULT AppendSubClassParams(_TCHAR *tszOutput) { return S_FALSE; }
   
   static HRESULT CreateButtonObjects(CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects,
                                      CRect &rcArea, 
                                      COLORREF clrFill, COLORREF clrLine, 
                                      int iLineWidth, int iLineStyle,
                                      CString &csButtonText, COLORREF clrText, LOGFONT *pLf);

   static HRESULT CreateButtonObjects(CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects,
                                      CRect &rcArea, CSize &siImage, CString &csFilename,
                                      CString &csButtonText, COLORREF clrText, LOGFONT *pLf);

   static HRESULT CalculateButtonSize(CSize &siButton, CString csButtonText, LOGFONT *pLf);
   static HRESULT CalculateButtonSize(CSize &siButton, CString csFileName);

   bool IsInStringArray(CString cStr, CArray<CString, CString>&acsTarget);
   InteractionRecordType GetTypeId();

   bool IsRandomTest();
   HRESULT SetInteractionNameOnly(CString &csName);

protected:
   virtual HRESULT CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey = false);
   HRESULT MergePeriods(CTimePeriod *pPeriodThis, UINT nOtherStart, UINT nOtherLength);

   CTimePeriod m_Visibility;
   CTimePeriod m_Activity;

   CString m_csDisplayType;
   CString m_csDisplayName;

   bool m_bMouseOver;
   bool m_bMousePressed;
   bool m_bActive;
   bool m_bDisabledForPreview;
   bool m_bHiddenDuringEdit;
   bool m_bTimelineDisplay;
   CRect m_rcTimelineRect;
   bool m_bHiddenDuringPreview;
   /**
    * If true, this object has been automatically inserted when creating demo document.
    * If true, when this object is deleted will require that all demo document objets be deleted.
    * Represents the .lep: <editor><streams><BUTTON demoModeObject="true"> tag value. demoModeObject value exists just when property is true.
    * Simillar value also in filesdk::CInteractionArea
    */
   bool m_bDemoDocumentObject;
   
   CRect m_rcDimensions;

private:
   void Clear();
   HRESULT WriteObjectArray(CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects,
                            CFileOutput *pOutput, LPCTSTR tszIdentifier, bool bUseFullImagePath,
                            CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
                            LPCTSTR tszSourcePath, LPCTSTR tszExportPrefix);
   
   HRESULT RemoveButtonObjects();

   void CreateNameAndType();

   static int s_iUpperStacking;
   static int s_iLowerStacking;

   CEditorProject *m_pEditorProject;

   UINT m_nHashKey;
   int m_iStackingLevel;

   bool m_bSelected;

   // in order to avoid small round-downs (CRect are ints) adding up:
   float m_fMoveOverhangX;
   float m_fMoveOverhangY;
   bool m_bHandCursor;
   CMouseAction *m_pClickAction;
   CMouseAction *m_pDownAction;
   CMouseAction *m_pUpAction;
   CMouseAction *m_pEnterAction;
   CMouseAction *m_pLeaveAction;

   bool m_bIsButton;
   bool m_bIsImageButton;
   ButtonTypeId m_idButtonType;

   UINT m_nQuestionJoinId;

   bool m_bTimesResolved;

   bool m_bIsInitialized;

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aNormalObjects;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aOverObjects;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aPressedObjects;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aInactiveObjects;

   bool m_bAutoMove;
};

#endif // !defined(AFX_INTERACTIONAREAEX_H__C22F3678_3D81_40AA_B1D9_C3168A47C06A__INCLUDED_)

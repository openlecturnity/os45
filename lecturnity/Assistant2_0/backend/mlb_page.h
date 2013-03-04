/*********************************************************************

 program : mlb_page.h
 authors : Gabriela Maass
 date    : 18.02.2000
 desc    : 

 **********************************************************************/

#ifndef __ASSISTANT_PAGE__
#define __ASSISTANT_PAGE__

#ifdef WIN32
#  pragma warning (disable: 4786)
#endif

#include "sgml.h"
#include "mlb_generic.h"
#include "mlb_objects.h"
#include "mlb_telepointer.h"
#include "mlb_undolist.h"

namespace ASSISTANT {

  class Page : public GenericContainer 
    {
    protected:
 
      CArray<DrawObject *, DrawObject *> // All objects ever seen on this page 
	      objects; 
      CArray<DrawObject *, DrawObject *> 
	      activeObjects; 
      UndoList
	      *undoList;
      DrawObject
	      *active;
       Telepointer // my own pointer; pointers of other participants are in the objectlist
	      *onlinePointer;
       Gdiplus::ARGB // Color of the page background 
	      m_argbBackground;
      int
	      pageNumber, 
	      firstTime,
	      width,
	      height,
	      sessionType;
      ASSISTANT::Rectangle
         *backRect;
      CString 
         imagePath_,
         pageID_; 
      float 
         zoomFactor_;
      int
         insertionCounter;
	  bool
		  bFullScreenOn;
      
    public:

      enum type {
	      A4_WIDTH  = 594,
	      A4_HEIGHT = 842
      };

      Page(uint32 _id, const _TCHAR *_title, int _width, int _height, COLORREF clrBackground);
      Page(uint32 _id,  _TCHAR *szPageNumber, GenericContainer *session, SGMLElement *root);
      ~Page();
      
      virtual void AddToStructureTree(CDocumentStructureView *pDocStructTree, UINT nParentId);

      /*** General Functions ***/
      virtual int GetType() {return ASSISTANT::GenericObject::PAGE;}

      /*** General Functions ***/
      virtual void SetName(LPCTSTR _name);
      virtual HRESULT GetKeywords(CString &csKeywords);
      virtual void SetKeywords(LPCTSTR keywords);
	  
      /*** Draw ***/
      virtual void Zoom(); 

      virtual void Activate();
      virtual void DeActivate();

      /*** Save Functions ***/
      virtual void SaveContainer(FILE *fp, LPCTSTR path, int level,bool all);
      virtual void SaveMetadata(CFileOutput *pLmdFile);

      /*** Find Functions ***/
      virtual GenericObject *GetObject(uint32 id);
      virtual GenericObject *GetGroup(uint32 id);

      virtual float GetObjectOrder(uint32 id);
      virtual float GetPushBackObjectOrder();
      virtual float GetEndObjectOrder();
      virtual float GetFirstObjectOrder();
      virtual float GetNextObjectOrder(uint32 id);
    //  virtual GenericObject* GetObjectWithOrder(uint32 order);

      DrawObject *GetActiveObject();
      void SetActiveObject(DrawObject *obj) {active = obj;}

      /*** AOF-Recording Functions ***/
      virtual void ResetRecording(); 
      void WriteObjectFile();   

      /*** General Functions ***/
      void Init();
      Page *Copy();
      void InitTelepointer();
      void LoadObjects(SGMLElement *root, LPCTSTR path, LPCTSTR szPageNumber);
      void SelectAllObjects();

      /*** Attributes ***/
      void SetPageDimension(int _width, int _height);
      void SetColor(Gdiplus::ARGB argbBackground);
      Gdiplus::ARGB GetColor() {return m_argbBackground;}

      //     void SetDimension(int _width, int _height);

      /*** Return private variables ***/
      float GetZoomFactor() {return zoomFactor_;}
      float CalculateZoomFactor(CRect &rcWhiteboard);
      void CalculateOffset(CRect &rcWhiteboard, float fZoomFactor, UINT &nXOffset, UINT &nYOffset);
      int  GetWidth() {return width;}
      int  GetHeight() {return height;}
      Gdiplus::ARGB GetBackground() {return m_argbBackground;}

      /*** Functions concerned objects ***/
      void Undo();
      int GetLastUndoTime();
      void CutObjects();
      void DeleteObjects(bool bDoUndo = true);
      void CopyObjects();
      void PasteObjects();  
		void AddSelectedObjectsToPasteBuffer(CArray<DrawObject *, DrawObject *> &arSelectedObjects);

      /*** Draw ***/
      virtual void DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC);
      virtual void DrawThumbWhiteboard(CRect rcWhiteboard, CDC *pDC);
	  void SetFullScreenOn(bool bState);

      /*** Insert ***/
      void AppendObject(DrawObject *obj, bool isActive=true, bool bAppendToUndo=false);
      void InActivateObject(DrawObject *obj);
      //     void AppendGroup(Group *grp);

      /*** Select ***/
      HRESULT SelectObjects(CRect &rcObjects);
      bool HasSelectedObjects();
      HRESULT SelectSingleObject(CRect &rcObject);
      void SelectObjects(int x, int y, int status); 
      void UnSelectObjects();
      void UnSelectImages();
      void UnSelectMasterObjects();
      void UnSelectDocumentObjects();

      /*** Change ***/
      void Change(DrawObject *pObj, bool bAlreadyInserted);
      void ChangeFontFamily(LPCTSTR szFontFamily);
      void ChangeFontSize(int iFontSize);
      void ChangeFontWeight(bool bBold);
      void ChangeFontSlant(bool bItalic);
      void ChangeLineColor(COLORREF clrLine, int iTransparency=255);
      void ChangeFillColor(COLORREF clrFill, int iTransparency=255);
      void ChangeTextColor(COLORREF clrText, int iTransparency=255);
      void ChangeFillColor();
      void ChangeLineWidth(int iLineWidth);
      void ChangeLineStyle(int iLineStyle);
      void ChangeArrowStyle(int iArrowConfig, LPCTSTR szArrowStyle);
      void InsertTextLine();
      void DeleteEmptyTextObjects();

      virtual void InsertImage(_TCHAR *filename, _TCHAR *path, int x, int y, bool single=true);

      void ActivateOnlinePointer(Gdiplus::PointF &ptObject);
      void MoveOnlinePointer(Gdiplus::PointF &ptObject);
      void DrawOnlinePointer(CRect &rcWhiteboard, CDC *pDC);
      void DeActivateOnlinePointer();

      void RaiseObjects();
      void LowerObjects();
      bool ShowNextObject();
      bool HidePreviousObject();
      bool HasNextObject();
      bool HasPreviousObject();
      void ShowAnimatedObject(int actualPos, bool backwards=true);
      void HideAnimatedObject(int actualPos, bool backwards=true);
      void ShowInteractiveObject(int actualPos, int iObjectId, bool backwards=true);
      void HideInteractiveObject(int actualPos, int iObjectId, bool backwards=true);
      bool ShowNextInteractiveObject(LPCTSTR objectIds, int iPPTObjectId);
      bool ShowObjectIfEnteredBefore(int actualPos);

      /*** Delete ***/
      void DeleteObjectFromContainer(GenericObject* obj);
      virtual void DeleteAll();
      void Clear();

      void ChangeSubcomponentsActiveness(bool active);
      void ReplaceIDs();

      CString &GetImagePath() {return imagePath_;}

      /*** Misc ***/
      void TranslateFromScreenToObjectCoordinates(CRect &rcScreen, CRect &rcObjects);

      /*** AOF-Recording Functions ***/
      void SaveAsAOF(int _time=-1, bool doSave=false);
      void SavePointAsAOF(PolyObject *obj);
      int RecordableObjects();
      int GetPageNumber() {return pageNumber;}

      virtual bool HasChanged();
      virtual void SetChanged(bool changed);
      
    virtual int GetMaxWidth(int &actMax);
    virtual int GetMaxHeight(int &actMin);

      virtual void ResetFirstInserted();

      virtual void CollectPages(CArray<Page *, Page *> &pageList);
      const _TCHAR *GetSlideId() {return pageID_;}

      void CallLink(DrawObject *object_, Gdiplus::PointF &ptObject);
      void ResetInteractiveObjects();


      // new
      virtual bool RenameContainer(UINT nContainerId, LPCTSTR szContainerName);

      ASSISTANT::DrawObject *GetObject(Gdiplus::PointF &ptMouse);
      bool PointIsInObject(Gdiplus::PointF &ptMouse);
      
      bool IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditType, int &iPointIndex);
      bool IsHyperlinkPoint(Gdiplus::PointF &ptObject);
      void ActivateHyperlink(Gdiplus::PointF &ptObject);
      void SetResizePoint(int iPointIndex) {m_iResizePoint = iPointIndex;}
      void ResizeObjects(float fDiffX,float fDiffY);

      void GetSelectedObjects(CArray<DrawObject *, DrawObject *> &arSelectedObjects);

      void AppendToUndoList(int action, CArray<DrawObject *, DrawObject *> &objects);

      bool IsNextObjectAnimated();
      bool IsPreviousObjectAnimated();
         
      protected:
         int m_iResizePoint;
    };
}

#endif

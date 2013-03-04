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

#include "SgmlParser.h"
#include "DrawObjects.h"

namespace ASSISTANT {

  class Page
    {
    protected:
 
      CArray<DrawObject *, DrawObject *> // All objects ever seen on this page 
	      objects; 
      CArray<DrawObject *, DrawObject *> 
	      activeObjects; 
      DrawObject
	      *active;
       COLORREF // Color of the page background 
	      m_clrBackground;
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
         pageID_,
         m_csName; 
      float 
         zoomFactor_;
      
    public:

      enum type {
	      A4_WIDTH  = 594,
	      A4_HEIGHT = 842
      };

      Page(SGMLElement *root, CString &csDocumentPath, CStringArray &arLsdTmpFileNames);
      ~Page();
      
      /*** General Functions ***/
      void LoadObjects(SGMLElement *root, LPCTSTR path, CStringArray &arLsdTmpFileNames);

      /*** General Functions ***/
      virtual void SetName(LPCTSTR _name) {m_csName = _name;}
      /*** Return private variables ***/
      int  GetWidth() {return width;}
      int  GetHeight() {return height;}
      COLORREF GetBackground() {return m_clrBackground;}

      CString &GetName() {return m_csName;}

      /*** Draw ***/
      virtual void DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC);

      /*** Insert ***/
      void AppendObject(DrawObject *obj, bool isActive=true, bool bAppendToUndo=false);
      //     void AppendGroup(Group *grp);

      void ActivateOnlinePointer(Gdiplus::PointF &ptObject);
      void MoveOnlinePointer(Gdiplus::PointF &ptObject);
      void DrawOnlinePointer(CRect &rcWhiteboard, CDC *pDC);
      void DeActivateOnlinePointer();

      
      void InActivateObject(DrawObject *obj);
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

      void ChangeSubcomponentsActiveness(bool active);
      void ReplaceIDs();

      CString &GetImagePath() {return imagePath_;}

    virtual int GetMaxWidth(int &actMax);
    virtual int GetMaxHeight(int &actMin);

      virtual void CollectPages(CArray<Page *, Page *> &pageList);
      LPCTSTR GetSlideId() {return pageID_;}

      void ResetInteractiveObjects();


      // new

      void GetObjects(CArray<DrawObject *, DrawObject *> &arObjects);
      
    };
}

#endif

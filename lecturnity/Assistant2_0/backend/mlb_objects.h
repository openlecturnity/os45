/*********************************************************************
program : mlb_objects.h
authors : Gabriela Maass
date    : 3.12.1999
desc    : 

**********************************************************************/

#ifndef __ASSISTANT_OBJECT__
#define __ASSISTANT_OBJECT__

#ifdef WIN32
#  pragma warning (disable: 4786)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <olectl.h>

#ifndef WIN32
#  include <unistd.h>
#else
#include <windows.h>
#endif

#include "mlb_types.h"
#include "mlb_generic.h"
#include "sgml.h"
#include "la_imagelist.h"
#include "FileOutput.h"

class CTextEdit;

namespace ASSISTANT
{ 
   class Page;
   
   class DrawObject : public GenericObject {
   protected:
      Page
         *page;
      Gdiplus::PointF
         selectPoints[8];
      int
         selectCount;
      double 
         m_dX, m_dY,						/* x- and y-Coordinate (Pixel) */
         m_dWidth, m_dHeight;			/* width and height (Pixel) */
      int 
         recID;    
      bool
         visible,            /* true=visible, false=invisible */
         selected,           /* true=selected, false=not selected */
         recorded;           /* true=recorded, false=not recorded */
      DrawObject 
         *group;
      CString 
         hyperlink_,
         currentDirectory_;
      CString
         m_ssLinkedObjects;
      int
         m_iPPTId;
      bool
         isInternLink,
         linkWasActivated;
      bool
         m_bIsMasterObject;
      bool
         m_bIsDocumentObject;

   public:
      enum type {
         CUTOBJECTS = 2,
            TOLERANZ = 3,
            NOTHING = 0,
            MOVE = 1,
            SIZE_NWSE = 2,
            SIZE_NESW = 3,
            SIZE_WE = 4,
            SIZE_NS = 5
      };
      
      
      /* creator */
      DrawObject(double _x, double _y, double _width, double _height, int _zPosition, uint32 id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR linkedObjects);
      virtual ~DrawObject();
      
      
      virtual int GetType() = 0;
      void SetPage(Page *_page) {page = _page;}
      Page *GetPage() {return page;}
      
      virtual void SetVisible() {visible = true;}
      virtual void SetInvisible() {visible = false;}
      virtual bool IsVisible() {return visible;}

      virtual void SetMasterObject(bool bIsMasterObject) {m_bIsMasterObject = bIsMasterObject;}
      virtual bool IsMasterObject() {return m_bIsMasterObject;}
      virtual void SetDocumentObject(bool bIsDocumentObject) {m_bIsDocumentObject = bIsDocumentObject;}
      virtual bool IsDocumentObject() {return m_bIsDocumentObject;}
      
      /*** own functions ***/
      /* Draw object on the canvas */
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0) {};
      /* Delete and Draw */
      /* Draw markers to show that the object was selected */
      virtual void DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY);
      /* Delete markers */
      
      virtual void Zoom(float _zoomFactor);
      
      virtual bool IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditTyp, int &iPointIndex, double dZoomFactor);
      virtual void ComputeCoords(double _x, double _y, double& _newX, double& _newY, double& _newWidth, double& _newHeight);
      /* hidden the object (delete from canvas) */
      virtual void Hide();
      /* Change the coordinates of the object */
      virtual void ModifyCoords(double _x, double _y, double _width, double _height, float _zoomFactor);
      /* Change the size of the object */
      virtual void Resize(int _width, int _height);
      /* Move the object */
      virtual void Move(float diffX, float diffY); 
      virtual void Resize(float fDiffX, float fDiffY, int iPointIndex);

      virtual int Select(int _canvasX, int _canvasY, float _zoomFactor);
      /* If the point x,y is an editable point return 1 */
      /* returns a copy of the object */              
      virtual DrawObject* Copy(bool keepZPosition = false) {return NULL;}
      /* write object to the Document */
      virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL) {}
      /* save object as postscript */
      virtual void SaveAsAOF(CFileOutput *fp) = 0;
      virtual int RecordableObjects() = 0;
      virtual int GetSelectCount() {return selectCount;}
      virtual double GetX() {return m_dX;}
      virtual double GetY() {return m_dY;}
      virtual void SetX(double _x) {m_dX = _x;}
      virtual void SetY(double _y) {m_dY = _y;}
      virtual double GetWidth() {return m_dWidth;}
      virtual double GetHeight() {return m_dHeight;}
      virtual void SetWidth(int _width) {m_dWidth = _width;}
      virtual void SetHeight(int _height) {m_dHeight = _height;}
      virtual void GetBBox(int *minX, int *minY, int *maxX, int *maxY);
      virtual void GetBoundingBox(CRect &rcObject);
      virtual void SetSelected(bool bIsSelected);
      virtual bool IsSelected();
      virtual void SetGroup(DrawObject *_group) {group = _group;}
      virtual DrawObject *GetGroup();
      virtual DrawObject *GetActive() {return this;}

	  
      
      virtual int GetRecordID() {return recID;}
      virtual void SetUnrecorded() {recorded = false;}
      virtual void SetRecorded() {recorded = true;}
      
      virtual bool IsInRegion(int _canvasX1, int _canvasY1, int _canvasX2, int _canvasY2);
      virtual bool Overlap(int _canvasX, int _canvasY, float _zoomFactor);
      
      virtual void AdjustGui() {};
      
      virtual bool HasHyperlink(Gdiplus::PointF &ptMouse);
      virtual LPCTSTR GetHyperlink(Gdiplus::PointF &ptMouse) {return hyperlink_;}
      virtual LPCTSTR GetLinkDirectory(Gdiplus::PointF &ptMouse) {return currentDirectory_;}
      virtual void ActivateHyperlink(Gdiplus::PointF &ptMouse) {linkWasActivated = true;}
      virtual bool IsInternLink() {return isInternLink;}
      virtual void LinkIsIntern(bool isIntern) {isInternLink = isIntern;}
      virtual bool HasObjectLinks(Gdiplus::PointF &ptMouse);
      virtual LPCTSTR GetInteractiveObjects(Gdiplus::PointF &ptMouse) {return m_ssLinkedObjects;}
      virtual void MakeHyperlinkString(CString &hyperlinkString);
      
      
      virtual void SetPPTObjectId(int iId) {m_iPPTId = iId;}
      virtual int GetPPTObjectId() {return m_iPPTId;}
      // Some actions needs a list of objects sorted in visibility order
      virtual void InsertInVisibilityOrder(CArray<DrawObject *, DrawObject *> &sortedList);
      virtual void IncreaseOrder(int increaseValue) {order += increaseValue;}
      
      virtual bool IsIdentic(DrawObject *obj);
      
      virtual bool IsInRect(CRect &rcSelection);
      virtual bool ContainsRect(CRect &rcSelection);
      virtual bool ContainsPoint(Gdiplus::PointF &ptMouse);

      void GetNormalizedCoordinates(double &dX, double &dY, double &dWidth, double &dHeight);

   };
   
   /***************************************************************************************************/
   
   class ColoredObject : public DrawObject {
   protected:
      Gdiplus::ARGB
         m_argbLineColor;
      Gdiplus::ARGB
         m_argbFillColor;
      Gdiplus::DashStyle
         m_gdipLineStyle;

      CString
         arrowStyle;
      int       
         arrowConfig;   
      int m_iArrowStyle_;
      byte iBeginArrowStyle;
      byte iBeginArrowDimension;
      byte iEndArrowStyle;
      byte iEndArrowDimension;
      int
         lineWidth;
	 
      
      
   public:
      ColoredObject(double _x, double _y, double _width, double _height, int _zPosition, 
                    Gdiplus::ARGB argbLineColor, Gdiplus::ARGB argbFillColor, 
                    int _lineWidth, Gdiplus::DashStyle gdipLineStyle, uint32 _id, 
                    LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);

      virtual ~ColoredObject();
      
      Gdiplus::ARGB GetLineColor() {return m_argbLineColor;}
      Gdiplus::ARGB GetFillColor() {return m_argbFillColor;}

      int   GetLineWidth() {return lineWidth;}
      Gdiplus::DashStyle GetLineStyle() {return m_gdipLineStyle;}
      
      bool IsFilled();
      
      virtual void ChangeLineColor(Gdiplus::ARGB argbLineColor);
      virtual void ChangeFillColor(Gdiplus::ARGB argbFillColor);
      virtual void ChangeLineWidth(int  _lineWidth);
      virtual void ChangeLineStyle(Gdiplus::DashStyle gdipLineStyle);
      
      Gdiplus::CustomLineCap *GetLineCap(int iArrowDimension, int iArrowStyle);
      CString &GetArrowStyle() {return arrowStyle;}
      int GetArrowConfig() {return arrowConfig;}
      
      void ConvertLinestyleToDash();
      void ComputeArrowConfig(int arrowConfig);
      void SetArrowCaps(Gdiplus::Pen &pen);
      
      virtual bool IsIdentic(DrawObject *obj);

	  
      
   };
   
   /***************************************************************************************************/
   
   class Rectangle : public ColoredObject {
   public:
      bool isBackground;
	  
      
      Rectangle(double _x, double _y, double _width, double _height, int _zPosition, 
                Gdiplus::ARGB argbLineColor, Gdiplus::ARGB argbFillColor, 
                int _lineWidth, Gdiplus::DashStyle gdipLineStyle, uint32 _id, 
                LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      Rectangle(double _x, double _y, double _width, double _height, int _zPosition, 
                Gdiplus::ARGB argbLineColor, uint32 _id, 
                LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      virtual ~Rectangle();
      
      virtual int GetType() {return GenericObject::RECTANGLE;};
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL);
      virtual void SaveAsAOF(CFileOutput *fp);
      virtual int RecordableObjects();
      virtual bool Overlap(int _canvasX, int _canvasY, float _zoomFactor);
      
      virtual void AdjustGui();
      virtual bool ContainsPoint(Gdiplus::PointF &ptMouse);

	  
      
      static DrawObject *Load(SGMLElement *hilf);
      
   };
   
   /***************************************************************************************************/
   
   class Oval : public ColoredObject {
   public:
      Oval(double _x, double _y, double _width, double _height, int _zPosition, 
           Gdiplus::ARGB argbLineColor, Gdiplus::ARGB argbFillColor, 
           int _lineWidth, Gdiplus::DashStyle gdipLineStyle, uint32 _id, 
           LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      
      virtual ~Oval();
      
      virtual int GetType() {return GenericObject::OVAL;}
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL);
      virtual void SaveAsAOF(CFileOutput *fp);
      virtual int RecordableObjects();
      virtual bool Overlap(int _canvasX, int _canvasY, float _zoomFactor);
      
      virtual void AdjustGui();
      virtual bool ContainsPoint(Gdiplus::PointF &ptMouse);
      static DrawObject *Load(SGMLElement *hilf);
      
   };
   
   /***************************************************************************************************/
   
   class Line : public ColoredObject {
   protected:
      int 
         type;              /* normal line or freehand */
      
   public:
      
      Line(double _x, double _y, double _width, double _height, int _zPosition, 
           Gdiplus::ARGB argbLineColor, int _lineWidth, Gdiplus::DashStyle gdipLineStyle, 
           LPCTSTR _arrowStyle, int _arrowConfig, uint32 _id, 
           LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      virtual ~Line();
      
      
      virtual int GetType() {return GenericObject::LINE;};
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      
      virtual void DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY);
      
      virtual bool  IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditTyp, int &iPointIndex, double dZoomFactor);
      virtual void MovePoint(int point, int _canvasX, int _canvasY, float _zoomFactor);
      virtual void Resize(float fDiffX, float fDiffY, int iPointIndex);
      
      virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL);
      virtual void SaveAsAOF(CFileOutput *fp);
      virtual int RecordableObjects();
      
      virtual void ChangeArrowStyle(LPCTSTR _style);
      virtual void ChangeArrowConfig(int _config);
      virtual bool Overlap(int _canvasX, int _canvasY, float _zoomFactor);
      
      
      virtual void AdjustGui();
      virtual bool ContainsPoint(Gdiplus::PointF &ptMouse);
      
      static DrawObject *Load(SGMLElement *hilf);
      
      
      virtual bool IsIdentic(DrawObject *obj);

      static void CalculateLineParameters(double &dX, double &dY, 
                                          double &dWidth, double &dHeight, 
                                          int &ori, int &dir); 
   };
   
   /***************************************************************************************************/
   
   class PolyObject : public ColoredObject {
   protected:
      int selectedPoint;
      bool isFreehand;
      
   public:
      CArray<Point *, Point*>  points;
      
      PolyObject(double _x, double _y, double _width, double _height, int _zPosition, 
                 Gdiplus::ARGB argbLineColor, Gdiplus::ARGB argbFillColor, 
                 int _lineWidth, Gdiplus::DashStyle gdipLineStyle, uint32 _id, 
                 LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      
      virtual ~PolyObject() {}
      
      virtual void DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY);
      
      virtual void Move(float diffX, float diffY); 
      virtual void MovePoint(int point,int x, int y, float _zoomFactor);
      
      virtual void SetX(double _x);
      virtual void SetY(double _y);
      
      virtual bool IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditTyp, int &iPointIndex, double dZoomFactor);
      
      void AppendNewPoint(double _canvasX, double _canvasY);
      void DrawNewSegment(CView *pView, UINT nOffX, UINT nOffY, float _zoomFactor, 
                          CPoint &ptNew, bool bInitialize=false);
      void Redraw(CView *pView, UINT nOffX, UINT nOffY, float _zoomFactor);
      void DeleteSegments();
      virtual bool Overlap(int _canvasX, int _canvasY, float _zoomFactor);
      
      virtual bool IsIdentic(DrawObject *obj);
      
   protected:
      bool AllPointsEqual();
   };
   
   /***************************************************************************************************/
   
   class PolyLine : public PolyObject {
   protected:
      bool
         isPencil;
      
   public:
      PolyLine(double _x, double _y, double _width, double _height, int _zPosition, 
               Gdiplus::ARGB argbLineColor, int _lineWidth, Gdiplus::DashStyle gdipLineStyle, 
               LPCTSTR _arrowStyle, int _arrowConfig, uint32 _id, 
               LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects, 
               bool _isFreehand=false, bool _isPencil=false);
      virtual ~PolyLine();
      
      virtual int GetType();
      
      CString &GetArrowStyle() {return arrowStyle;}
      int GetArrowConfig() {return arrowConfig;}
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      virtual void Hide();
      
      void GetLastPointsRect(CRect &rcLastPoints);
      void GetUpdateRect(CRect &rcObject);
      
      virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL);
      virtual void SaveAsAOF(CFileOutput *fp);
      virtual int RecordableObjects();
      virtual void ChangeLineColor(Gdiplus::ARGB argbLineColor);
      virtual void ChangeLineWidth(int  _lineWidth);
      virtual void ChangeLineStyle(Gdiplus::DashStyle gdipLineStyle);
      virtual void ChangeArrowStyle(LPCTSTR _style);
      virtual void ChangeArrowConfig(int _config);
      
      virtual void AdjustGui();
      virtual bool ContainsRect(CRect &rcSelection);
      virtual bool ContainsPoint(Gdiplus::PointF &ptMouse);
      
      static DrawObject *Load(SGMLElement *hilf);
      
      virtual bool IsIdentic(DrawObject *obj);
      
   };
   
   /***************************************************************************************************/
   
   class Polygon : public PolyObject {
   public:
      Polygon(double _x, double _y, double _width, double _height, int _zPosition, 
              Gdiplus::ARGB argbLineColor, Gdiplus::ARGB argbFillColor, 
              int _lineWidth, Gdiplus::DashStyle gdipLineStyle, uint32 _id, 
              LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects);

      virtual ~Polygon();
      
      virtual int GetType() {return GenericObject::POLYGON;};
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      
      virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL);
      virtual void SaveAsAOF(CFileOutput *fp);
      virtual int RecordableObjects();
      
      virtual void AdjustGui();
      virtual bool ContainsPoint(Gdiplus::PointF &ptMouse);
      virtual bool ContainsRect(CRect &rcSelection);
      
      static DrawObject *Load(SGMLElement *hilf);
   };
   
   
   class Marker : public PolyObject {
   public:
      Marker(double _x, double _y, int _zPosition, 
             Gdiplus::ARGB argbLineColor, int lineWidth, uint32 _id, 
             LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      virtual ~Marker();
      
      virtual int GetType() {return GenericObject::MARKER;};
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      virtual void Hide();
      
      virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL);
      virtual void SaveAsAOF(CFileOutput *fp);
      virtual int RecordableObjects();
      
      virtual void ChangeLineColor(Gdiplus::ARGB argbLineColor);
      virtual void ChangeLineWidth(int  _lineWidth);
      
      virtual bool ContainsPoint(Gdiplus::PointF &ptMouse);
      
      static DrawObject *Load(SGMLElement *hilf);
   };
   
   
   /***************************************************************************************************/
   
   
   class Text : public ColoredObject {
   private:
      friend class Group;
      
      Gdiplus::ARGB
         m_argbActivatedLinkColor;
      
      CString
         text;
      int
         actPos;
      CArray<int, int>
         lineHeight;
      CString 
         fontFamily,
         fontSize,
         fontWeight,
         fontSlant,
         textPosition;
      int
         textStyle;
      int
         drawSize;
      float 
         fAscent, fDescent;
      int
         offY;
      LOGFONT 
         m_logFont;
      BOOL m_bIsUnderlined;
      
      bool
         m_bHasSupportedFont;
      bool
         m_bTextWidthIsStatic;
      bool
         m_bHasReturnAtEnd;
      
   public:
      enum type {
         INSERT,
            BACKSPACE,
            DELETECHAR,
            RETURN,
            RIGHT,
            LEFT,
            UP,
            DOWN,
            HOME,
            END
      };
      
      bool firstPos;
      
      Text(double _x, double _y, double _width, double _height, int _zPosition, 
           Gdiplus::ARGB argbLineColor, LPCTSTR _textString, LPCTSTR _family, LPCTSTR _size, 
           LPCTSTR _weight, LPCTSTR _slant, int _textStyle, uint32 _id, 
           LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects, Gdiplus::ARGB argbLinkColor);
      
      virtual ~Text();
      
      bool HasReturnAtEnd() {return m_bHasReturnAtEnd;}
      void SetReturnAtEnd(bool bHasReturn) {m_bHasReturnAtEnd = bHasReturn;}
      virtual bool IsVisible();
      
      virtual int GetType() {return GenericObject::ASCIITEXT;};
      virtual double GetHeight();
      virtual double GetWidth();
      virtual float GetAscent();
      virtual float GetDescent();
      
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      virtual void Move(float diffX, float diffY); 
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      
      virtual bool IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditTyp, int &iPointIndex, double dZoomFactor);
      
      virtual void Hide();
      virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL);
      virtual void SaveAsAOF(CFileOutput *fp);
      virtual int RecordableObjects();
      virtual int  Select(int _canvasX, int _canvasY, float _zoomFactor); 
      virtual bool Overlap(int _canvasX, int _canvasY, float _zoomFactor);
      
      void AppendNewChars(_TCHAR* _string);
      void InsertNewChars(LPCTSTR _string);
      bool DeleteChar();       
      bool DeleteNextChar();       
      
      bool Empty() {return text.IsEmpty() == 0 ? false : true;}
      
      bool GoRight(int actY);
      bool GoLeft(int actY);
      bool GoEnd();
      bool GoHome();
      int  GetPos();
      void Update();
      void GetBBox(int *minX, int *minY, int *maxX, int *maxY);
      virtual void GetBoundingBox(CRect &rcObject);
      virtual void GetBoundingBox(Gdiplus::RectF &gdipRcObject);
      void UpdateItemList();
      void UpdateLineHeights();
      void ChangeFont(LPCTSTR family, LPCTSTR size, LPCTSTR weight, LPCTSTR slant);
      
      void ChangeFontFamily(LPCTSTR szFontFamily);
      void ChangeFontSize(int iFontSize);
      
      virtual void ChangeLineColor(Gdiplus::ARGB argbLineColor);
      void NewLine();
      void SplitCurrentItem(int pos);   
      void ChangeString(LPCTSTR _textString);
      int  EndPos();
      
      
      LPCTSTR GetString();
      Gdiplus::ARGB GetColor();
      LPCTSTR GetFamily();
      LPCTSTR GetSize();
      int GetFontSize() {return drawSize;}
      LPCTSTR GetWeight();
      LPCTSTR GetSlant();
      CFont *GetFont() {return NULL;}
      
      void GetLogFont(LOGFONT *pLogFont) {memcpy(pLogFont, &m_logFont, sizeof(LOGFONT));}
      int GetTextStyle() {return textStyle;}
      
      void AdjustGui();
      int  GetStringSize() {return text.GetLength();}
      
      void SetFirstActive();
      void SetLastActive(); 
      void SetLeftActive();
      void SetRightActive();
      void SetActive(int idx);
      
      int GetLength() {return text.GetLength();}
      
      static DrawObject *Load(SGMLElement *hilf);
      
      Gdiplus::ARGB &GetLinkColor() {return m_argbActivatedLinkColor;}
      void SetFontUnsupported();
      
      
      virtual bool IsIdentic(DrawObject *obj);
      
      private:
         int GetValidFontStyle(Gdiplus::FontFamily &fontFamily);
  };
  
  /***************************************************************************************************/
  
  class Group : public DrawObject {
  public:
     CArray<DrawObject *, DrawObject *>
        groupObjects;
  private:
     int
        actItem;
     bool 
        textGroup;
     CTextEdit *m_pEditText;
     bool m_bResetEditText;
     LOGFONT m_logFont;
     COLORREF m_clrText;
     
  public:
     Group(uint32 _id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
     virtual ~Group();
     
     virtual int GetType() {return GenericObject::GROUP;};
     
     //     DrawObject* Copy(Page* page, bool keepZPosition = false);
     DrawObject* Copy(bool keepZPosition = false);
     virtual void SetVisible();
     
     virtual DrawObject *GetGroup();
     virtual DrawObject *GetActive();
     
     DrawObject* GetObject(int idx);
     DrawObject* GetObjectFromID(uint32 _id);
     DrawObject* GetFirst() {if (groupObjects.GetSize() == 0) return NULL; return groupObjects[0];}
     //std::vector<uint32>* sDs();
     
     virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
     virtual void GetBBox(int *minX, int *minY, int *maxX, int *maxY);
     virtual void GetBoundingBox(CRect &rcObject);
     virtual void DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY);
     void UpdateWindowRect();
     
     virtual bool  IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditTyp, int &iPointIndex, double dZoomFactor);
     virtual void SetSelected(bool bIsSelected);

     
     virtual void Resize(float fDiffX, float fDiffY, int iPointIndex);
     virtual void Move(float diffX, float diffY); 
     
     virtual void SetX(double _x);
     virtual void SetY(double _y);
     
     virtual void Zoom(float _zoomFactor);
     virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL);
     virtual void SaveAsAOF(CFileOutput *fp);
     virtual int RecordableObjects();
     virtual void SetUnrecorded();
     virtual bool Overlap(int _canvasX, int _canvasY, float _zoomFactor);
     virtual bool IsInRegion(int _canvasX1, int _canvasY1, int _canvasX2, int _canvasY2);
     virtual void Hide();
     virtual void SetInvisible();
     virtual void AdjustGui();
     
     //    virtual void GetBBox(int *minX, int *minY, int *maxX, int *maxY);
     
     void AddObject(DrawObject *obj);
     GenericObject* DeleteObject(GenericObject* obj);
     DrawObject* DeleteObject(uint32 id);
     //DrawObject* SubstractObject(DrawObject *obj);
     void DeleteAll();
     
     bool AllEmpty();
     bool Empty() {return groupObjects.GetSize() == 0;}
     int GetSize() {return groupObjects.GetSize();}
     
     bool IsTextGroup();
     bool IsEmptyText();
     void LoadObjects(Page *page, LPCTSTR szPageNumber, SGMLElement *root, LPCTSTR path, bool append=true);
     void UpdateDimension();
     void Update();
     
     void SetFirstActive();
     void SetLastActive();
     void SetLeftActive();
     void SetRightActive();
     void SetActiveItem(DrawObject* obj);
     
     /* Functions for Text-Object in Group */
     void ChangeFontFamily(LPCTSTR szFontFamily);
     void ChangeFontSize(int iFontSize);
     void ChangeFontWeight(LPCTSTR fontweight);
     void ChangeFontSlant(LPCTSTR fontslant);
     
     virtual void ChangeLineColor(Gdiplus::ARGB argbLineColor);
     virtual void ChangeLineWidth(int  _lineWidth);
     virtual void ChangeFillColor(Gdiplus::ARGB argbFillColor);
     virtual void ChangeLineStyle(Gdiplus::DashStyle gdipLineStyle);
     
     void CreateTextString(CString &csText);
     void ChangeObjects(CString &csWindowText, bool bSave);
     void SetLogFont(LOGFONT logFont) {memcpy(&m_logFont, &logFont, sizeof(LOGFONT));}
     void SetTextColor(COLORREF clrText) {m_clrText = clrText;}
     void GetTextFontFamily(CString &csFontFamily);
     int GetTextFontSize();
     void GetTextFontSlant(CString &csFontSlant);
     void GetTextFontWeight(CString &csFontWeight);
     
     static DrawObject *Load(SGMLElement *hilf);
     void TakeParameter(DrawObject *_obj);
     void TakeIds(DrawObject *_obj);
     
     
     virtual bool HasHyperlink(Gdiplus::PointF &ptObject);
     virtual LPCTSTR GetInteractiveObjects(Gdiplus::PointF &ptMouse);
     virtual bool HasObjectLinks(Gdiplus::PointF &ptMouse);
     virtual LPCTSTR GetHyperlink(Gdiplus::PointF &ptObject);
     virtual LPCTSTR GetLinkDirectory(Gdiplus::PointF &ptMouse);
     virtual bool IsInternLink();
     virtual void LinkIsIntern(bool isIntern);
     virtual void ActivateHyperlink(Gdiplus::PointF &ptObject);
     
     // Some actions needs a list of objects sorted in visibility order
     // To do this with a group you need recursion
     virtual void InsertInVisibilityOrder(CArray<DrawObject *, DrawObject *> &sortedList);
     // This function increases the object order of all group objects with the given value
     virtual void IncreaseOrder(int increaseValue);
     // This function appends all group object to page
     virtual void AppendObjectsToPage(Page *page);
     
     virtual bool IsIdentic(DrawObject *obj);
     
     virtual bool ContainsPoint(Gdiplus::PointF &ptMouse);
     
     virtual void SetDocumentObject(bool bIsDocumentObject);
  };
  
  
  /***************************************************************************************************/
  
  class WindowsImage : public DrawObject {
  private:
     CString 
        filename_,
        suffix_,
        srcPath_,
        destName_,
        imagePath_;
     ImageIdentifier
        *image_;
     bool
        externImage_;
     
  public:
     WindowsImage(double _x, double _y, double _width, double _height, int _zPosition, 
                  LPCTSTR filename, uint32 _id, 
                  LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
     virtual ~WindowsImage();
     
     virtual void GetBBox(int *minX, int *minY, int *maxX, int *maxY);
     virtual void GetBoundingBox(CRect &rcObject);
     //   static char* BuildBlob(string filename, uint32& blength, string& type);
     
     ImageIdentifier *GetImage() {return image_;}
     void SetImage(ImageIdentifier *_image);
     void SetExtern(bool isExtern) {externImage_ = isExtern;}
     
     void SetImageExtern(bool externImage) {externImage_ = externImage;}
     
     int BuildImage(ImageIdentifier *image, bool verifyName=true);
     void BuildImage(ASSISTANT::Page *page, LPCTSTR path, bool verifyName=true);
     
     void ModifyCoords(double _x, double _y, double _width, double _height, float _zoomFactor);
     void Resize(int _width, int _height);
     void Update(float _zoomFactor);
     
     virtual int GetType() {return GenericObject::IMAGE;}
     
     virtual DrawObject *Copy(bool keepZPosition = false);
     virtual bool IsVisible();
     
     virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
     
     virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL);
     
     virtual void SaveAsAOF(CFileOutput *fp);
     virtual int RecordableObjects();
     
     virtual void AdjustGui() {};
     
     
     /* Function for AOF-Recording */
     void WriteImage();
     void CopyImageTo(LPCTSTR _destPath);
     
     static DrawObject *Load(SGMLElement *hilf, CString &imagePath);
     
     virtual bool IsIdentic(DrawObject *obj);

      virtual bool IsInRect(CRect &rcSelection);
      virtual bool ContainsRect(CRect &rcSelection);
      virtual bool ContainsPoint(Gdiplus::PointF &ptMouse);
     
     virtual void SetUnrecorded();
  };
  
  
  class AnimatedObject : public DrawObject {
  public:
     enum
     {
        EXIT = 0,
           ENTER = 1
     } AnimationType;
     enum
     {
        ACTIVE = 1,
           PASSIV = 0
     } AnimationStatus;
     
     enum
     {
        CLICK_ON_PAGE = 0,
           CLICK_ON_OBJECT = 1
     } TriggerType;
     
     CArray<DrawObject *, DrawObject *> 
        animatedObjects;
     
  private:
     int
        actItem;
     int 
        m_iAnimationId;
     int 
        m_iAnimationType;
     int
        m_iStatus;
     int 
        m_iClickId;
     int
        m_iTrigger;
     int 
        m_iCallerId;
     
  public:
     AnimatedObject(uint32 _id, int iAnimationId, int iAnimationType, 
                    int iClickId, int iTrigger, int iCallerId, 
                    LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
     
     virtual int GetType() {return GenericObject::ANIMATED;};
     
     //DrawObject* Copy(Page* page, bool keepZPosition = false);
     DrawObject* Copy(bool keepZPosition = false);
     
     virtual void SetVisible();
     
     DrawObject* GetObject(int idx);
     DrawObject* GetObjectFromID(uint32 _id);
     DrawObject* GetFirst() {if (animatedObjects.GetSize() == 0) return NULL; return animatedObjects[0];}
     //std::vector<uint32>* GetObjectIDs() ;
     
     virtual DrawObject *GetGroup();
     virtual void GetBBox(int *minX, int *minY, int *maxX, int *maxY);
     virtual void GetBoundingBox(CRect &rcObject);
     virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
     virtual void DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY);
     virtual void Move(float diffX, float diffY); 
     
     virtual void SetX(double _x);
     virtual void SetY(double _y);
     
     virtual void Zoom(float _zoomFactor);
     
     virtual bool IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditTyp, int &iPointIndex, double dZoomFactor);
     virtual bool IsInRegion(int _canvasX1, int _canvasY1, int _canvasX2, int _canvasY2);
     
     virtual void SaveObject(FILE *fp, int level=0, LPCTSTR path=NULL);
     virtual void SaveAsAOF(CFileOutput *fp);
     virtual int RecordableObjects();
     virtual void SetUnrecorded();
     virtual bool Overlap(int _canvasX, int _canvasY, float _zoomFactor);
     virtual void Hide();
     // Hide all common objects in this and animObject
     void Hide(AnimatedObject *animObject);
     // Show all objects appears in this and not in animObject
     void Show(AnimatedObject *animObject);
     
     virtual void SetInvisible();
     virtual void AdjustGui();
     
     void AddObject(DrawObject *obj);
     GenericObject* DeleteObject(GenericObject* obj);
     DrawObject* DeleteObject(uint32 id);
     void DeleteAll();
     
     bool Empty() {return animatedObjects.GetSize() == 0;}
     int GetSize() {return animatedObjects.GetSize();}
     
     void LoadObjects(Page *page, LPCTSTR szPageNumber, SGMLElement *root, LPCTSTR path, bool append=true);
     void UpdateDimension();
     
     void SetFirstActive();
     void SetLastActive();
     void SetLeftActive();
     void SetRightActive();
     void SetActiveItem(DrawObject* obj);
     
     virtual void ChangeLineColor(Gdiplus::ARGB argbLineColor);
     virtual void ChangeLineWidth(int  _lineWidth);
     virtual void ChangeFillColor(Gdiplus::ARGB argbFillColor);
     virtual void ChangeLineStyle(Gdiplus::DashStyle gdipLineStyle);
     
     static DrawObject *Load(SGMLElement *hilf);
     void TakeParameter(DrawObject *_obj);
     void TakeIds(DrawObject *_obj);
     
     virtual bool HasHyperlink(Gdiplus::PointF &ptObject);
     virtual LPCTSTR GetInteractiveObjects(Gdiplus::PointF &ptMouse);
     virtual bool HasObjectLinks(Gdiplus::PointF &ptMouse);
     virtual LPCTSTR GetHyperlink(Gdiplus::PointF &ptObject);
     virtual LPCTSTR GetLinkDirectory(Gdiplus::PointF &ptMouse);
     virtual bool IsInternLink();
     virtual void LinkIsIntern(bool isIntern);
     virtual void ActivateHyperlink(Gdiplus::PointF &ptObject);
     
     // Some actions needs a list of objects sorted in visibility order
     // To do this with a group you need recursion
     virtual void InsertInVisibilityOrder(CArray<DrawObject *, DrawObject *> &sortedList);
     // This function increases the object order of all group objects with the given value
     virtual void IncreaseOrder(int increaseValue);
     // This function appends all group object to page
     virtual void AppendObjectsToPage(Page *page);
     
     // returns the animation type (0 = exit, 1 = enter)
     int GetAnimationType() {return m_iAnimationType;}
     int GetAnimationId() {return m_iAnimationId;}
     // returns true if object is activated (shown or hidden, depends on type)
     int GetStatus() {return m_iStatus;}
     void SetStatus(int iStatus) {m_iStatus = iStatus;}
     
     // returns 
     int GetClickId() {return m_iClickId;}
     int GetTrigger() {return m_iTrigger;}
     int GetCallerId() {return m_iCallerId;}
     
     virtual bool IsIdentic(DrawObject *obj);
     bool IsAllInvisible();
     
     virtual bool ContainsPoint(Gdiplus::PointF &ptMouse);

     virtual void SetDocumentObject(bool bIsDocumentObject);
  };
  
}

#endif

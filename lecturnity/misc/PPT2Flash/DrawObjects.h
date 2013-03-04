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

#include "SgmlParser.h"

namespace ASSISTANT
{ 
   class Page;
   
    enum type {
      RECTANGLE, 
      OVAL,
      LINE,
      TEXT,
      POLYGON,
      MARKER,
      FREEHAND,
      POLYLINE,
      IMAGE,
      PENCIL,
      GROUP,
      ANIMATED
    };
   
   typedef CString Color;
   typedef CString Style;
   
   class DrawObject
   {
   protected:
      Page
         *page;
      double 
         m_dX, m_dY,						/* x- and y-Coordinate (Pixel) */
         m_dWidth, m_dHeight;			/* width and height (Pixel) */
      bool
         visible;           /* true=recorded, false=not recorded */
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
      float
         m_fOrder;
      
      
   public:
      DrawObject(double _x, double _y, double _width, double _height, int _zPosition, 
                 UINT id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR linkedObjects);
      virtual ~DrawObject();
      
      
      virtual int GetType() = 0;
      void SetPage(Page *_page) {page = _page;}
      Page *GetPage() {return page;}
      
    
      virtual float GetOrder() {return m_fOrder;} 
      virtual void SetOrder(float _order) {m_fOrder = _order;}

      virtual void SetVisible() {visible = true;}
      virtual void SetInvisible() {visible = false;}
      virtual bool IsVisible() {return visible;}

      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0) {};
      
      virtual void Zoom(float _zoomFactor);
      
      /* hidden the object (delete from canvas) */
      virtual void Hide();
      /* Change the size of the object */
     
      virtual DrawObject* Copy(bool keepZPosition = false) {return NULL;}
      /* write object to the Document */
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
      virtual void SetGroup(DrawObject *_group) {group = _group;}
      virtual DrawObject *GetGroup();
      virtual DrawObject *GetActive() {return this;}
      
      virtual bool HasHyperlink();
      virtual LPCTSTR GetHyperlink() {return hyperlink_;}
      virtual LPCTSTR GetLinkDirectory() {return currentDirectory_;}
      virtual void ActivateHyperlink() {linkWasActivated = true;}
      virtual bool IsInternLink() {return isInternLink;}
      virtual void LinkIsIntern(bool isIntern) {isInternLink = isIntern;}
      virtual bool HasObjectLinks();
      virtual LPCTSTR GetInteractiveObjects() {return m_ssLinkedObjects;}
      virtual void MakeHyperlinkString(CString &hyperlinkString);
      
      
      virtual void SetPPTObjectId(int iId) {m_iPPTId = iId;}
      virtual int GetPPTObjectId() {return m_iPPTId;}
      // Some actions needs a list of objects sorted in visibility order
      virtual void InsertInVisibilityOrder(CArray<DrawObject *, DrawObject *> &sortedList);
      
      virtual bool IsIdentic(DrawObject *obj);

   public:
      static Gdiplus::ARGB MakeARGB(LPCTSTR color);
      static int CalculateAofLinestyle(LPCTSTR lineStyle);
      static COLORREF CalculateColor(Color &color);
   };
   
   /***************************************************************************************************/
   
   class ColoredObject : public DrawObject {
   protected:
      Color
         color,
         fillColor;
      Gdiplus::ARGB
         m_argbLineColor;
      Gdiplus::ARGB
         m_argbFillColor;
      Style
         lineStyle;
      Style
         arrowStyle;
      int       
         arrowConfig;   
      int m_iArrowStyle_;
      byte iBeginArrowStyle;
      byte iBeginArrowDimension;
      byte iEndArrowStyle;
      byte iEndArrowDimension;
      int
         m_gdipLineStyle;
      Gdiplus::DashStyle m_gdipDashStyle;
      int
         lineWidth;
      
      
   public:
      ColoredObject(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR color, 
         LPCTSTR _fillColor, int _lineWidth, LPCTSTR _lineStyle, UINT _id, 
         LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      virtual ~ColoredObject();
      
      Color GetLineColor() {return color;}
      Gdiplus::ARGB GetARGBLineColor() {return m_argbLineColor;}
      Color GetFillColor() {return fillColor;}
      Gdiplus::ARGB GetARGBFillColor() {return m_argbFillColor;}
      int   GetLineWidth() {return lineWidth;}
      Style GetLineStyle() {return lineStyle;}
      int GetGdiplusLineStyle() {return m_gdipLineStyle;}
      
      bool IsFilled();
      Gdiplus::CustomLineCap *GetLineCap(int iArrowDimension, int iArrowStyle);
      Style GetArrowStyle() {return arrowStyle;}
      int GetArrowConfig() {return arrowConfig;}
      int GetNewArrowStyle() {return m_iArrowStyle_;}
      
      void ConvertLinestyleToDash();
      void ComputeArrowConfig(int arrowConfig);
      void SetArrowCaps(Gdiplus::Pen &pen);
      
      virtual bool IsIdentic(DrawObject *obj);
   };
   
   /***************************************************************************************************/
   
   class Rectangle : public ColoredObject {
   public:
      bool isBackground;
      
      Rectangle(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _color, 
         LPCTSTR _fillColor, int _lineWidth, LPCTSTR _lineStyle, UINT _id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      Rectangle(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _color, UINT _id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      virtual ~Rectangle();
      
      virtual int GetType() {return RECTANGLE;};
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      
      static DrawObject *Load(SGMLElement *hilf);
   };
   
   /***************************************************************************************************/
   
   class Oval : public ColoredObject {
   public:
      Oval(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _color, 
         LPCTSTR _fillColor, int _lineWidth, LPCTSTR _lineStyle, UINT _id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      
      virtual ~Oval();
      
      virtual int GetType() {return OVAL;}
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      static DrawObject *Load(SGMLElement *hilf);
   };
   
   /***************************************************************************************************/
   
   class Line : public ColoredObject {
   protected:
      int 
         type;              /* normal line or freehand */
      Style
         capStyle,
         joinStyle;
      
   public:
      
      Line(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _color, 
         int _lineWidth, LPCTSTR _lineStyle, LPCTSTR _capStyle, LPCTSTR _joinStyle, 
         LPCTSTR _arrowStyle, int _arrowConfig, UINT _id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      virtual ~Line();
      
      
      virtual int GetType() {return LINE;};
      
      Style GetCapStyle() {return capStyle;}
      Style GetJoinStyle() {return joinStyle;}
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      
      virtual void Resize(float fDiffX, float fDiffY, int iPointIndex);
      
      static DrawObject *Load(SGMLElement *hilf);
      
      virtual bool IsIdentic(DrawObject *obj);
   };
   
   /***************************************************************************************************/
   
   class PolyObject : public ColoredObject {
   protected:
      int selectedPoint;
      bool isFreehand;
      
   public:
      CArray<Gdiplus::PointF *, Gdiplus::PointF*>  points;
      
      PolyObject(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _color, 
         LPCTSTR _fillColor, int _lineWidth, LPCTSTR _lineStyle, UINT _id, LPCTSTR _hyperlink,
         LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      
      virtual ~PolyObject() {}
      
      virtual void SetX(double _x);
      virtual void SetY(double _y);
      
      void AppendNewPoint(int _canvasX, int _canvasY);
      void DeleteSegments();
      CArray<Gdiplus::PointF *, Gdiplus::PointF *> *GetPoints();
      
      virtual bool IsIdentic(DrawObject *obj);
   protected:
      bool AllPointsEqual();
   };
   
   /***************************************************************************************************/
   
   class PolyLine : public PolyObject {
   protected:
      bool
         smooth,
         isPencil;
      
      Style
         capStyle,
         joinStyle;
      
   public:
      PolyLine(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _color, 
         int _lineWidth, LPCTSTR _lineStyle, LPCTSTR _capStyle, LPCTSTR _joinStyle, 
         LPCTSTR _arrowStyle, int _arrowConfig, bool _smooth, UINT _id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects, bool _isFreehand=false, bool _isPencil=false);
      virtual ~PolyLine();
      
      virtual int GetType();
      
      Style GetCapStyle() {return capStyle;}
      Style GetJoinStyle() {return joinStyle;}
      Style GetArrowStyle() {return arrowStyle;}
      int GetArrowConfig() {return arrowConfig;}
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      virtual void Hide();
      
      void GetLastPointsRect(CRect &rcLastPoints);
      
      static DrawObject *Load(SGMLElement *hilf);
      
      virtual bool IsIdentic(DrawObject *obj);
   };
   
   /***************************************************************************************************/
   
   class Polygon : public PolyObject {
   public:
      Polygon(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _color, 
         LPCTSTR _fillColor, int _lineWidth, LPCTSTR _lineStyle, UINT _id, LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects);
      virtual ~Polygon();
      
      virtual int GetType() {return POLYGON;};
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      
      static DrawObject *Load(SGMLElement *hilf);
   };
   
   
   class Marker : public PolyObject {
   public:
      Marker(double _x, double _y, int _zPosition, LPCTSTR color, int lineWidth, UINT _id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
      virtual ~Marker();
      
      virtual int GetType() {return MARKER;};
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      virtual void Hide();
      
      long GetColor();
      
      static DrawObject *Load(SGMLElement *hilf);
   };
   
   
   /***************************************************************************************************/
   
   
   class Text : public ColoredObject {
   private:
      friend class Group;
      
      Color
         activatedLinkColor;
      
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
      double 
         fAscent, fDescent;
      int
         offY;
      LOGFONT 
         m_logFont;
      
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
         LPCTSTR _color, LPCTSTR _textString, LPCTSTR _family, LPCTSTR _size, 
         LPCTSTR _weight, LPCTSTR _slant, LPCTSTR _position, int _textStyle, UINT _id, 
         LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects, LPCTSTR linkColor);
      
      virtual ~Text();
      
      bool HasReturnAtEnd() {return m_bHasReturnAtEnd;}
      void SetReturnAtEnd(bool bHasReturn) {m_bHasReturnAtEnd = bHasReturn;}
      virtual bool IsVisible();
      
      virtual int GetType() {return TEXT;};
      virtual double GetHeight();
      virtual double GetWidth();
      
      
      virtual DrawObject *Copy(bool keepZPosition = false);
      
      virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
      
      virtual void Hide();
      char *CalculateFontname(int &size); 
      
      bool Empty() {return text.IsEmpty() == 0 ? false : true;}
      
      char *GetTextRef();
      bool GoRight(int actY);
      bool GoLeft(int actY);
      bool GoEnd();
      bool GoHome();
      int  GetPos();
      void Update();
      void GetBBox(int *minX, int *minY, int *maxX, int *maxY);
      virtual void GetBoundingBox(CRect &rcObject);
      void UpdateItemList();
      void UpdateLineHeights();

      void NewLine();
      void SplitCurrentItem(int pos);   
      void ChangeString(LPCTSTR _textString);
      int  EndPos();
      
      
      LPCTSTR GetString();
      LPCTSTR GetColor();
      LPCTSTR GetFamily();
      LPCTSTR GetSize();
      LPCTSTR GetWeight();
      LPCTSTR GetSlant();
      LPCTSTR GetPosition();
      CFont *GetFont() {return NULL;}
      
      void GetLogFont(LOGFONT *pLogFont) {memcpy(pLogFont, &m_logFont, sizeof(LOGFONT));}
      int GetTextStyle() {return textStyle;}
      
      void AdjustGui();
      int  GetStringSize() {return text.GetLength();}
      char GetChar(int idx);
      
      void SetFirstActive();
      void SetLastActive(); 
      void SetLeftActive();
      void SetRightActive();
      void SetActive(int idx);
      
      Text* Split();
      
      int GetLength() {return text.GetLength();}
      
      static DrawObject *Load(SGMLElement *hilf);
      
      CString &GetLinkColor() {return activatedLinkColor;}
      void SetFontUnsupported();
      
      
      LPTSTR GetTtfName();
      static void FillLogFont(LOGFONT &logFont, LPCTSTR fontFamily, int fontSize, 
                              LPCTSTR fontWeight, LPCTSTR fontSlant);
      
      virtual bool IsIdentic(DrawObject *obj);

      private:
         int GetValidFontStyle(Gdiplus::FontFamily &fontFamily);
         double GetTextAscent(LOGFONT *logFont);
         double GetTextDescent(LOGFONT *logFont);
         double GetTextWidth(const _TCHAR *textString, int textLength, LOGFONT *logFont);
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
     bool m_bResetEditText;
     LOGFONT m_logFont;
     COLORREF m_clrText;
     
  public:
     Group(UINT _id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
     virtual ~Group();
     
     virtual int GetType() {return GROUP;};
     
     //     DrawObject* Copy(Page* page, bool keepZPosition = false);
     DrawObject* Copy(bool keepZPosition = false);
     
     virtual void SetVisible();
     virtual DrawObject *GetGroup();
     virtual DrawObject *GetActive();
     
     void AddObject(DrawObject *obj);
     
     DrawObject* GetObject(int idx);
     DrawObject* GetObjectFromID(UINT _id);
     DrawObject* GetFirst() {if (groupObjects.GetSize() == 0) return NULL; return groupObjects[0];}
     //std::vector<UINT>* sDs();
     
     virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
     
     virtual void SetX(double _x);
     virtual void SetY(double _y);
     
     virtual void Zoom(float _zoomFactor);
     virtual void Hide();
     virtual void SetInvisible();
     
     //    virtual void GetBBox(int *minX, int *minY, int *maxX, int *maxY);
     
     bool AllEmpty();
     bool Empty() {return groupObjects.GetSize() == 0;}
     int GetSize() {return groupObjects.GetSize();}
     
     bool IsTextGroup();
     void LoadObjects(Page *page, SGMLElement *root, LPCTSTR path, CStringArray &arLsdTmpFileNames, bool append=true);
     void UpdateDimension();
     void Update();
     
     void SetFirstActive();
     void SetLastActive();
     void SetLeftActive();
     void SetRightActive();
     void SetActiveItem(DrawObject* obj);

     void SetLogFont(LOGFONT logFont) {memcpy(&m_logFont, &logFont, sizeof(LOGFONT));}
     void SetTextColor(COLORREF clrText) {m_clrText = clrText;}
     
     static DrawObject *Load(SGMLElement *hilf);
     void TakeParameter(DrawObject *_obj);
     void TakeIds(DrawObject *_obj);
     
     
     virtual bool HasHyperlink();
     virtual LPCTSTR GetInteractiveObjects();
     virtual bool HasObjectLinks();
     virtual LPCTSTR GetHyperlink();
     virtual LPCTSTR GetLinkDirectory();
     virtual bool IsInternLink();
     virtual void LinkIsIntern(bool isIntern);
     virtual void ActivateHyperlink();
     void ActivateAssociatedHyperlink(DrawObject *hyperlink);
     
     // Some actions needs a list of objects sorted in visibility order
     // To do this with a group you need recursion
     virtual void InsertInVisibilityOrder(CArray<DrawObject *, DrawObject *> &sortedList);
     // This function appends all group object to page
     virtual void AppendObjectsToPage(Page *page);
     
     
      virtual bool IsIdentic(DrawObject *obj);
  };
  
  
  /***************************************************************************************************/
  
  class Image : public DrawObject {
  private:
     CString 
        filename_,
        suffix_,
        srcPath_,
        destName_,
        imagePath_;
      Gdiplus::Image 
         *gdipImage_;
     
  public:
     Image(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR filename, UINT _id, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
     virtual ~Image();
     
     virtual void GetBBox(int *minX, int *minY, int *maxX, int *maxY);
     virtual void GetBoundingBox(CRect &rcObject);
     //   static char* BuildBlob(string filename, UINT& blength, string& type);
     
     void Resize(int _width, int _height);
     void Update(float _zoomFactor);
     
     virtual int GetType() {return IMAGE;}
     
     virtual DrawObject *Copy(bool keepZPosition = false);
     virtual bool IsVisible();
     
     virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
     
     CString &GetImageName() {return filename_;}
     CString &GetImagePath() {return imagePath_;}
     
     virtual void AdjustGui() {};
     
     
     static DrawObject *Load(SGMLElement *hilf, CString &imagePath, CStringArray &arLsdTmpFileNames);
     
      virtual bool IsIdentic(DrawObject *obj);

      int WriteScaledImageFile(WCHAR* scaledImageName, float scaleX, float scaleY, const WCHAR* format);
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
     AnimatedObject(UINT _id, int iAnimationId, int iAnimationType, int iClickId, int iTrigger, int iCallerId, LPCTSTR _hyperlink, LPCTSTR currentDirectory, LPCTSTR _linkedObjects);
     
     virtual int GetType() {return ANIMATED;};
     
     //DrawObject* Copy(Page* page, bool keepZPosition = false);
     DrawObject* Copy(bool keepZPosition = false);
     
     virtual void SetVisible();
     
     DrawObject* GetObject(int idx);
     DrawObject* GetObjectFromID(UINT _id);
     DrawObject* GetFirst() {if (animatedObjects.GetSize() == 0) return NULL; return animatedObjects[0];}
     //std::vector<UINT>* GetObjectIDs() ;
     
     virtual DrawObject *GetGroup();
     virtual void Draw(CDC *pDC, float zoomFactor, double dOffX=0.0, double dOffY=0.0);
     
     virtual void SetX(double _x);
     virtual void SetY(double _y);
     
     virtual void Zoom(float _zoomFactor);
     
     virtual bool IsInRegion(int _canvasX1, int _canvasY1, int _canvasX2, int _canvasY2);
     
     virtual void Hide();
     // Hide all common objects in this and animObject
     void Hide(AnimatedObject *animObject);
     // Show all objects appears in this and not in animObject
     void Show(AnimatedObject *animObject);
     
     virtual void SetInvisible();
     virtual void AdjustGui();
     
     void AddObject(DrawObject *obj);
     
     bool Empty() {return animatedObjects.GetSize() == 0;}
     int GetSize() {return animatedObjects.GetSize();}
     
     void LoadObjects(Page *page, SGMLElement *root, LPCTSTR path, CStringArray &arLsdTmpFileNames, bool append=true);
     void UpdateDimension();
     
     void SetFirstActive();
     void SetLastActive();
     void SetLeftActive();
     void SetRightActive();
     void SetActiveItem(DrawObject* obj);
     
     
     static DrawObject *Load(SGMLElement *hilf);
     void TakeParameter(DrawObject *_obj);
     void TakeIds(DrawObject *_obj);
     
     virtual bool HasHyperlink();
     virtual LPCTSTR GetInteractiveObjects();
     virtual bool HasObjectLinks();
     virtual LPCTSTR GetHyperlink();
     virtual LPCTSTR GetLinkDirectory();
     virtual bool IsInternLink();
     virtual void LinkIsIntern(bool isIntern);
     virtual void ActivateHyperlink();
     
     // Some actions needs a list of objects sorted in visibility order
     // To do this with a group you need recursion
     virtual void InsertInVisibilityOrder(CArray<DrawObject *, DrawObject *> &sortedList);
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
     
     bool IsAllInvisible();
     
      virtual bool IsIdentic(DrawObject *obj);
  };
  
}

#endif

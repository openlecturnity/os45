#ifndef __DRAWSDK_OBJECTS__
#define __DRAWSDK_OBJECTS__

#include "tools.h"

#include <crtdbg.h>
#include <gdiplus.h>

namespace DrawSdk
{
   class DRAWSDK_EXPORT CharArray
   {
   public:
      CharArray(int iInitialSize = 1024, int iIncreaseSize = -1);
      ~CharArray();

      /** Returns the current valid characters in the buffer. */
      int GetLength() { return m_iBufferContent; }

      /** The returned string is most likely not terminated. */
      _TCHAR *GetBuffer() { return m_tszContent; };

      void AppendData(_TCHAR *tszData, int iLength = -1);

      void Reset() { m_iBufferContent = 0; }

   private:
      int m_iBufferSize;
      int m_iIncreaseSize;
      int m_iBufferContent;
      _TCHAR *m_tszContent;
   };

   class DRAWSDK_EXPORT DPoint
   {
   public:
      DPoint();
      DPoint(double xPos, double yPos);

      double x;
      double y;
   };

   class DRAWSDK_EXPORT ZoomManager
   {
   public:
      ZoomManager(int sx, int sy);
      ZoomManager(double zoomFactor);
      ~ZoomManager();

      int GetScreenX(double x);
      int GetScreenY(double y);
      int GetRelativeX(int x);
      int GetRelativeY(int y);
      double GetZoomFactor();

   private:
      int dimX_;
      int dimY_;
      double zoomFactor_;
      bool useZoomFactor_;
   };

   class DRAWSDK_EXPORT DrawObject
   {
   public:
      DrawObject();
      DrawObject(double x, double y, double width, double height);
      DrawObject(Gdiplus::RectF *rect);

      // only implemented in Rectangle, Text and Image, TargetPoint:
      virtual DrawObject* Copy() { _ASSERT(false); return NULL; }

      virtual ~DrawObject();

      virtual void Draw(HDC hdc, double offX = 0.0, double offY = 0.0, ZoomManager *pZm = NULL) = 0;
      virtual int GetType() = 0;  

      virtual double GetX() {return x_;}
      virtual double GetY() {return y_;}
      virtual double GetWidth() {return width_;}
      virtual double GetHeight() {return height_;}

      void SetX(double x) {x_ = x;}
      void SetY(double y) {y_ = y;}
      virtual void SetWidth(double width) {width_ = width;}
      virtual void SetHeight(double height) {height_ = height;}

      virtual void Move(double deltaX, double deltaY);
      bool Contains(double dPosX, double dPosY);

      bool IsDndMoveable() { return m_bDndMoveable; } // only Image uses/changes this at the moment
      void SetBelongsKey(int iBelongsKey) { m_iBelongsKey = iBelongsKey; } // only for Image and TargetArea at the moment
      int  GetBelongsKey() { return m_iBelongsKey; }
      bool SetVisibility(LPCTSTR szVisible);
      _TCHAR *GetVisibility(_TCHAR *szTarget = NULL);
      bool HasVisibility() { return m_tszVisibility != NULL; }
      
      virtual void GetLogicalBoundingBox(RECT *pRect);
      virtual void GetScreenBoundingBox(RECT *pRect, double xOff, double yOff, ZoomManager *pZoom);

      void ResetObjectNumber() {objectNumber_ = -1;}
      virtual int GetObjectNumber() {return objectNumber_;}
      void SetObjectNumber(int objectNumber) {objectNumber_ = objectNumber;}

      Gdiplus::ARGB ReverseColor(Gdiplus::ARGB color);

      // only implemented in Rectangle, Text and Image, TargetPoint:
      //virtual void Write(_TCHAR tszBuffer[], bool bUseFullImagePath = false) { _ASSERT(false); } 
      //virtual void Write(HANDLE objFile) = 0;

      virtual void Write(CharArray *pArray, bool bUseFullImagePath = false) = 0;
      
   protected:
      virtual void Init();
      void CopyInteractiveFields(DrawObject *pObject);

   protected:
      double x_;
      double y_;
      double width_;                 // Object width
      double height_;                // Object height

      int objectNumber_;

      bool m_bDndMoveable;
      int m_iBelongsKey;
      _TCHAR *m_tszVisibility;
   };
   
   class DRAWSDK_EXPORT ColorObject : public DrawObject
   {
   public:
      ColorObject();
      ColorObject(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle=0);
      ColorObject(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle=0);
      
      ColorObject(Gdiplus::RectF *rect, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle=0);
      ColorObject(Gdiplus::RectF *rect, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle=0);
      virtual ~ColorObject();
      
      virtual void Draw(HDC hdc, double offX = 0.0, double offY = 0.0, ZoomManager *pZm = NULL) = 0;
      virtual void GetLogicalBoundingBox(RECT *pRect);
      
      virtual Gdiplus::ARGB GetOutlineColor() {return m_argbLineColor;}
      virtual Gdiplus::ARGB GetFillColor() {return m_argbFillColor;}
      virtual COLORREF GetOutlineColorRef();
      virtual COLORREF GetFillColorRef();
      virtual double GetLineWidth() {return lineWidth_;}
      virtual int GetLineStyle() {return lineStyle_;}
      virtual bool GetXorMode() {return useXor_;}
      virtual bool GetIsFilled() {return isFilled_;}
      virtual bool GetIsTransparent() {return transparent_;}
      
      void SetLineColor(Gdiplus::ARGB outlineColor);
      void SetFillColor(Gdiplus::ARGB fillColor);
      void SetFillColor();

      void SetLineWidth(double lineWidth) {lineWidth_ = lineWidth;}
      void SetLineStyle(int lineStyle);

      virtual void SetArrowStyle(int arrowStyle);
      virtual void SetArrowConfig(int arrowConfig);
      virtual int GetArrowStyle() {return arrowStyle_;}
      int GetArrowConfig();

      void ConvertLinestyleToDash();
      
      void SetXorMode(bool useXor) {useXor_ = useXor;}

      Gdiplus::CustomLineCap *GetLineCap(int iArrowDimension, int iArrowStyle);
      void ComputeArrowConfig(int arrowConfig);
      void SwapArrows();
      void SetArrowCaps(Gdiplus::Pen &pen);

   protected:
      virtual void Init();

   protected:
      Gdiplus::ARGB m_argbLineColor;   // Line color
      Gdiplus::ARGB m_argbFillColor;   // Fill color
      Gdiplus::DashStyle m_gdipDashStyle;
      int arrowStyle_;
      byte iBeginArrowStyle;
      byte iBeginArrowDimension;
      byte iEndArrowStyle;
      byte iEndArrowDimension;
      double lineWidth_;
      int lineStyle_;                // AOF line style
      int penStyle_;                 // Windows line style
      int brushStyle_;               // Windows fill style
      long brushHatch_;              // Windows Fill pattern
      bool transparent_;             // Transparent (true) or opaque (false)
      bool useXor_;
      bool isFilled_;
   };

   class DRAWSDK_EXPORT Rectangle : public ColorObject
   {
   public:
      Rectangle();
      Rectangle(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle);
      Rectangle(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle);
      virtual ~Rectangle();

      virtual DrawObject* Copy();
      
      virtual void Draw(HDC hdc, double offX = 0.0, double offY = 0.0, ZoomManager *pZm = NULL);    

      virtual int GetType() {return RECTANGLE;}

      //virtual void Write(_TCHAR tszBuffer[], bool bUseFullImagePath = false);
      //virtual void Write(HANDLE objFile);
      virtual void Write(CharArray *pArray, bool bUseFullImagePath = false);
      
      bool HasSameValues(Rectangle *pOther);
   };

   class DRAWSDK_EXPORT Oval : public ColorObject
   {
   public:
      Oval();
      Oval(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle);
      Oval(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle);
      
      Oval(Gdiplus::RectF *rect, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle);
      Oval(Gdiplus::RectF *rect, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle);

      virtual ~Oval();
      
      virtual void Draw(HDC hdc, double offX = 0.0, double offY = 0.0, ZoomManager *pZm = NULL);    
      virtual int GetType() {return OVAL;}
      
      //virtual void Write(HANDLE objFile) ;
      virtual void Write(CharArray *pArray, bool bUseFullImagePath = false);
   };

   class DRAWSDK_EXPORT Line : public ColorObject
   {
   public:
      Line();
      Line(double x, double y, double width, double height, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle=0, int arrowStyle=0, int arrowConfig=0, int ori=0, int dir=0);
      virtual ~Line();

      virtual DrawObject* Copy();

      virtual void Draw(HDC hdc, double offX = 0.0, double offY = 0.0, ZoomManager *pZm = NULL);    
      
      virtual int GetType() {return LINE;}

      virtual int GetOrientation() {return ori_;}
      virtual int GetDirection() {return dir_;}

      //virtual void Write(_TCHAR tszBuffer[], bool bUseFullImagePath = false);
      //virtual void Write(HANDLE objFile);

      virtual void Write(CharArray *pArray, bool bUseFullImagePath = false);

   private:
      int ori_;
      int dir_;
      DPoint firstArrow_[6];
      DPoint lastArrow_[6];
   };

   class DRAWSDK_EXPORT Polygon : public ColorObject
   {
   public:
      Polygon();
      Polygon(DPoint *pts, int ptCount, Gdiplus::ARGB outlineColor, Gdiplus::ARGB fillColor, double lineWidth, int lineStyle);
      Polygon(DPoint *pts, int ptCount, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle=0, int arrowStyle=0, int arrowConfig = 0, bool autoClose=false);
  
      virtual ~Polygon();
      
      virtual void Draw(HDC hdc, double offX = 0.0, double offY = 0.0, ZoomManager *pZm = NULL);    

      virtual DrawObject* Copy();

      virtual int GetType() { return isFreehand_ ? FREEHAND : POLYGON; }

      //virtual void Write(HANDLE objFile);
      virtual void Write(CharArray *pArray, bool bUseFullImagePath = false);
	  virtual void ToFile(FILE *fp);
      
      virtual double GetX();
      virtual double GetY();
      virtual double GetWidth();
      virtual double GetHeight();
      virtual void CalculateDimension();
      virtual void Move(double deltaX, double deltaY);
      virtual void SetPoints(DPoint *pts, int ptCount);
	  /** Get Points returns the number of points, if pts == NULL */
      virtual int GetPoints(DPoint *pts, int ptCount);
      virtual void AddPoint(DPoint &pts);
      virtual int GetCount();
      virtual void SetAutoClose(bool b=true) { autoClose_ = b; }
      virtual bool GetIsAutoClose() {return autoClose_;}

      void SetIsFreehand(bool isFreehand) {isFreehand_ = isFreehand;}
      virtual bool GetIsFreehand() {return isFreehand_;}


   protected:
      virtual void Init();
      virtual void DeletePoints();
      bool AllPointsEqual();

   protected:
      void *pts_; // is a vector<DPoint *>
      int ptCount_;
      bool isFreehand_;
      bool autoClose_;
      bool bboxCalced_;
   };
   
   
   class DRAWSDK_EXPORT Marker : public Polygon
   {
   public:
      Marker();
      Marker(DPoint *pts, int ptCount, Gdiplus::ARGB outlineColor, double lineWidth, int lineStyle=0);
      virtual ~Marker();

      virtual void Draw(HDC hdc, double offX = 0.0, double offY = 0.0, ZoomManager *pZm = NULL);    
      virtual int GetType() {return MARKER;}

      //virtual void Write(HANDLE objFile);
      virtual void Write(CharArray *pArray, bool bUseFullImagePath = false);
	  virtual void ToFile(FILE *fp);

   protected:
      virtual void Init();
   };
   
   class DRAWSDK_EXPORT Text : public ColorObject
   {
   public:
      Text();
      Text(double x, double y, Gdiplus::ARGB outlineColor, const _TCHAR *textString, int textLength, LOGFONT *font);
      Text(double x, double y, Gdiplus::ARGB outlineColor, const _TCHAR *textString, int textLength, LOGFONT *font, const _TCHAR *ttf);
      
      Text(double x, double y, double width, double height, 
         Gdiplus::ARGB outlineColor, const _TCHAR *textString, int textLength, 
         LOGFONT *font, TextTypeId idTextType = TEXT_TYPE_NOTHING);
      Text(double x, double y, double width, double height, 
         Gdiplus::ARGB outlineColor, const _TCHAR *textString, int textLength, LOGFONT *font,
         const _TCHAR *ttf, TextTypeId idTextType = TEXT_TYPE_NOTHING);
      virtual ~Text();

      virtual DrawObject* Copy();
      
      virtual void Draw(HDC hdc, double offX = 0.0, double offY = 0.0, ZoomManager *pZm = NULL);    
      virtual void GetLogicalBoundingBox(RECT *pRect);

      virtual int GetType() {return TEXT;}
      
      //virtual void Write(_TCHAR tszBuffer[], bool bUseFullImagePath = false);
      //virtual void Write(HANDLE objFile);
      virtual void Write(CharArray *pArray, bool bUseFullImagePath = false);
      void Write(CharArray *pArray, bool bUseFullImagePath, 
                 bool bUseStrictXml, int nTimeBeginMs);

      void CalculateDimension();

      void DrawCursor(HDC hdc, int position, ZoomManager *pZm=NULL);

      void SetDrawCursor(bool b=true) { drawCursor_ = b; }
      void SetCursorPos(int nPos=-1) { cursorPos_ = nPos; }
      void SetFont(LOGFONT *font);
      void ChangeFontFamily(const _TCHAR *tszFontFamily);
      void ChangeFontSize(int iFontSize);
      void SetStringNumber(int iNumber);
      void SetString(const _TCHAR *textString, int textLength);
      virtual const _TCHAR *GetString();
      virtual int GetTextLength() {return textLength_;}
      virtual LOGFONT GetLogFont() {return logFont_;}
      virtual void GetLogFont(LOGFONT *pLf) {memcpy(pLf, &logFont_, sizeof(LOGFONT));}
      virtual const _TCHAR *GetTtfName();
      virtual int GetTmAscent();

      bool HasTextType() { return m_idTextType != TEXT_TYPE_NOTHING; }
      bool HasChangeableType() { return m_idTextType > TEXT_TYPE_NOTHING && m_idTextType < TEXT_TYPE_CHANGE_LAST; }
      TextTypeId GetTextType() { return m_idTextType; }

      static double GetTextWidth(const _TCHAR *textString, int textLength, LOGFONT *logFont);
      static double GetTextHeight(const _TCHAR *textString, int textLength, LOGFONT *logFont);
      static void GetTextDimension(const _TCHAR *textString, int textLength, LOGFONT *logFont, UINT &nWidth, UINT &nHeight);
      static void GetBoundingBox(const _TCHAR *textString, int textLength, LOGFONT *logFont, Gdiplus::RectF &bbox);
      static void CalculateCenterPos(const _TCHAR *textString, int textLength, LOGFONT *pLogFont, 
                                        Gdiplus::RectF &gdipRcArea, Gdiplus::PointF &gdipPtTextPos);
      static double GetTextAscent(LOGFONT *logFont);
      static double GetTextDescent(LOGFONT *logFont);
      static double GetCharWidth(_TCHAR ascii, LOGFONT *logFont);
      static double GetXPos(double xStart, double textWidth, const _TCHAR *textString, LOGFONT *logFont, int index);
      static int GetTextIndex(double xStart, double textWidth, const _TCHAR *textString, LOGFONT *logFont, int xPos);   
      static int GetValidFontStyle(Gdiplus::FontFamily &fontFamily, LOGFONT *pLogFont = NULL);

      static TextTypeId GetTextTypeForString(LPCTSTR tszType);

      static LPTSTR GetTtfName(LOGFONT &lf);

      bool EndsWithReturn() {return m_bEndsWithReturn;}
      void EndsWithReturn(bool bEndsWithReturn) {m_bEndsWithReturn = bEndsWithReturn;}
      void DrawSpecial(bool bDrawSpecial) {m_bDrawSpecial = bDrawSpecial;};
      bool IsDrawSpecial(){return m_bDrawSpecial;};

   protected:
      virtual void Init();
   private:
      void GetTextTypeString(_TCHAR *tszTarget);

      LOGFONT logFont_;
      //TCHAR textString_[512];
      _TCHAR *textString_;
      _TCHAR *ttfName_;

      int textLength_;
      int cursorPos_;

      bool drawCursor_;
      Gdiplus::PointF *m_pCharacterPosition;
      double m_lastZoomFactor;

      TextTypeId m_idTextType;
      bool m_bEndsWithReturn;
      bool m_bDrawSpecial;
   };

   class DRAWSDK_EXPORT Image : public DrawObject
   {
   public:
      Image();
      Image(double x, double y, double width, double height, 
         const _TCHAR *imageName, bool bDndMoveable = false, int iBelongsKey = 0);
      Image(double x, double y, double width, double height, 
         const _TCHAR *imageName, Gdiplus::Image *gdipImage, bool bDndMoveable = false, int iBelongsKey = 0);
      virtual ~Image();

      virtual DrawObject* Copy();
            
      virtual void Draw(HDC hdc, double offX = 0.0, double offY = 0.0, ZoomManager *pZm = NULL);    

      virtual int GetType() {return IMAGE;}
      virtual LPCTSTR GetImageName() {return imageName_;}
      void SetImagePrefix(LPCTSTR newImageName);
      int WriteScaledImageFile(WCHAR* scaledImageName, float scaleX, float scaleY, const WCHAR* format);

      //virtual void Write(_TCHAR tszBuffer[], bool bUseFullImagePath = false);
      //virtual void Write(HANDLE objFile);

      virtual void Write(CharArray *pArray, bool bUseFullImagePath = false);

   protected:
      virtual void Init();

   public:
      int GetImageWidth();
      int GetImageHeight();

   private:
      void DoubleSlashes(_TCHAR *tszSource, _TCHAR *tszTarget);

      _TCHAR *imageName_;
      _TCHAR *imagePrefix_;
      bool m_bImageStoredOutside;

      Gdiplus::Image 
         *gdipImage_;
   };

   class DRAWSDK_EXPORT Pointer : public DrawObject
   {
   public:
      Pointer();
      Pointer(double x, double y, bool bMousePointer=false);
      virtual ~Pointer();

      virtual void Draw(HDC hdc, double offX = 0.0, double offY = 0.0, ZoomManager *pZm = NULL); 

      virtual int GetType() {return TELEPOINTER;}
      virtual void GetLogicalBoundingBox(RECT *pRect);

      //virtual void Write(HANDLE objFile);
      virtual void Write(CharArray *pArray, bool bUseFullImagePath = false);

   public:
      bool IsMousePointer();

   protected:
      virtual void Init();

   private:
      void DrawTelePointer(HDC hdc, int zX, int zY, int zWidth, int zHeight);
      void DrawMousePointer(HDC hdc, int zX, int zY, int zWidth, int zHeight);

   private:
      bool m_bMousePointer;
   };

   class DRAWSDK_EXPORT TargetArea : public DrawObject
   {
   public:
      TargetArea(double dX, double dY, double dWidth, double dHeight, LPCTSTR tszName, int iBelongsKey);
      virtual ~TargetArea();
      virtual int GetType() { return TARGET; }

      double GetCenterX();
      double GetCenterY();
      LPCTSTR GetName() { return m_tszName; }

      virtual DrawObject* Copy();
            
      virtual void Draw(HDC hdc, double dOffX = 0.0, double dOffY = 0.0, ZoomManager *pZm = NULL);    

      //virtual void Write(_TCHAR tszBuffer[], bool bUseFullImagePath = false);
      //virtual void Write(HANDLE hObjFile);
      virtual void Write(CharArray *pArray, bool bUseFullImagePath = false);
   private:
      _TCHAR *m_tszName;
   };
}

#endif
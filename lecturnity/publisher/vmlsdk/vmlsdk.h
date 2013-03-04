//////////////////////////////////////////////////////////////////////
//
// vmlsdk.h: Interface for the class CVmlSdk.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VMLSDK_H__50FD999E_43A9_45EF_9436_51E9887DE108__INCLUDED_)
#define AFX_VMLSDK_H__50FD999E_43A9_45EF_9436_51E9887DE108__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "vmlfont.h"

#include "WhiteboardStream.h" 
#include "objects.h"

//////////////////////////////////////////////////////////////////////

class CVmlSdk  
{
public:
   // Constructor/Destructor
   CVmlSdk();
	virtual ~CVmlSdk();

   // methods
   void Init();
   void Clear();

   int StartParsing();

   // Get/Set methods
   CString GetEventFileName();
   CString GetObjectFileName();
   CString GetOutputFileName();
   void SetEventFilename( CString eventFilename );
   void SetObjectFilename( CString objectFilename );
   void SetOutputFilename( CString outputFilename );
   void SetSlideDimension( int slideWidth, 
                           int slideHeight );
   void SetEventTimestamps( long* eventTimestamps, 
                            int etLength );

private:
   // member variables
   CWhiteboardStream*            m_pWhiteboardStream;
   CArray<CPage*, 
          CPage*>                m_arrPages;
   CArray<DrawSdk::DrawObject*, 
          DrawSdk::DrawObject*>  m_arrObjects;
   CArray<CVmlFont*, 
          CVmlFont*>             m_arrVmlFonts;
   CArray<int, int>              m_arrUsedObjects;
   FILE*                         m_fp;
   CString                       m_strEventFilename;
   CString                       m_strObjectFilename;
   CString                       m_strOutputFilename;
   CString                       m_strText;
//   CString                       m_strVisibility;
   long*                         m_lEventTimestamps;
   int                           m_nEventsUsed; 
   int                           m_nSlideWidth;
   int                           m_nSlideHeight;
   int                           m_nObjectIndex;
   int                           m_nObjectNumber;
   int                           m_nDefaultFontSize;
   int                           m_nPointerWidth;
   int                           m_nPointerHeight;


   // methods
   int CreateVmlPage();
   int WriteTemplateFileIntoStream( CString tmplFile );
   int WriteTextLineIntoStream( CString strText );
   int WriteTextLineIntoStream( CString strText, FILE* fp );
   int WriteVmlPageContentIntoStream();
   int WriteVmlPageObjectsIntoStream();
   int WriteVmlTelepointerIntoStream();
   int GetTelepointerIndexFromEvent( CWhiteboardEvent* pEvent );

   int CreateRectangleFromObject( DrawSdk::Rectangle* pObject );
   int CreateOvalFromObject( DrawSdk::Oval* pObject );
   int CreateLineFromObject( DrawSdk::Line* pObject );
   int CreateTextFromObject( DrawSdk::Text* pObject );
   int CreatePolygonFromObject( DrawSdk::Polygon* pObject );
   int CreatePolygonFromObject( DrawSdk::Polygon* pObject, bool bIsMarker );
   int CreateMarkerFromObject( DrawSdk::Marker* pObject );
   int CreateFreehandFromObject( DrawSdk::Polygon* pObject );
   int CreateTelepointerFromObject( DrawSdk::Pointer* pObject );
   int CreateImageFromObject( DrawSdk::Image* pObject );

   int WriteVmlGroup();

   /* <POINTER> */
   int WriteVmlPointer( int nX, 
                        int nY, 
                        int nWidth, 
                        int nHeight );

   /* <IMAGE> */
   int WriteVmlImage( int nX, 
                      int nY, 
                      int nWidth, 
                      int nHeight, 
                      CString strImageName );
   /* <LINE> */
   int WriteVmlLine( int nX, 
                     int nY, 
                     int nWidth, 
                     int nHeight, 
                     COLORREF clrOutlineColor, 
                     int nLineWidth, 
                     int nLineStyle, 
                     int nArrowStyle, 
                     int nOri, 
                     int nDir );
   /* <FILLEDPOLY> */
   int WriteVmlPolygon( int nX, 
                        int nY, 
                        int nWidth, 
                        int nHeight, 
                        DrawSdk::DPoint* pPts, 
                        int nPtCount, 
                        COLORREF clrOutlineColor, 
                        COLORREF clrFillColor, 
                        int nLineWidth, 
                        int nLineStyle );
   /* <FREEHAND>,<POLYLINE> */
   int WriteVmlPolygon( int nX, 
                        int nY, 
                        int nWidth, 
                        int nHeight, 
                        DrawSdk::DPoint* pPts, 
                        int nPtCount, 
                        COLORREF clrOutlineColor, 
                        int nLineWidth, 
                        int nLineStyle, 
                        bool bIsMarker = false );
   /* <FILLEDCIRC> */
   int WriteVmlOval( int nX, 
                     int nY, 
                     int nWidth, 
                     int nHeight, 
                     COLORREF clrOutlineColor, 
                     COLORREF clrFillColor, 
                     int nLineWidth, 
                     int nLineStyle );
   /* <OUTLINECIRC> */
   int WriteVmlOval( int nX, 
                     int nY, 
                     int nWidth, 
                     int nHeight, 
                     COLORREF clrOutlineColor, 
                     int nLineWidth, 
                     int nLineStyle );
   /* <FILLEDRECT> */
   int WriteVmlRectangle( int nX, 
                          int nY, 
                          int nWidth, 
                          int nHeight, 
                          COLORREF clrOutlineColor, 
                          COLORREF clrFillColor, 
                          int nLineWidth, 
                          int nLineStyle );
   /* <OUTLINERECT> */
   int WriteVmlRectangle( int nX, 
                          int nY, 
                          int nWidth, 
                          int nHeight, 
                          COLORREF clrOutlineColor, 
                          int nLineWidth, 
                          int nLineStyle );
   /* <TEXT>,<TEXTITEM> */
   int WriteVmlText( int nX, 
                     int nY, 
                     int nWidth, 
                     int nHeight, 
                     int nAscent, 
                     COLORREF clrOutlineColor, 
                     CString strText, 
                     int nTextLength, 
                     LOGFONT lfFont, 
                     CString strTtfName );
   /* <TEXT>,<TEXTITEM> */
   int WriteVmlText( int nX, 
                     int nY, 
                     int nWidth, 
                     int nHeight, 
                     int nAscent, 
                     COLORREF clrOutlineColor, 
                     CString strText, 
                     int nTextLength, 
                     LOGFONT lfFont );

   /* RECT,CIRC Object */
   int WriteVmlRectCircObject( CString strType, 
                               int nX, 
                               int nY, 
                               int nWidth, 
                               int nHeight, 
                               CString strOutlineColor, 
                               CString strFillColor, 
                               int nLineWidth, 
                               CString strLineStyle );

   /* POLYLINE,FREEHAND,FILLEDPOLY */
   int WriteVmlPolygon( int nX, 
                        int nY, 
                        int nWidth, 
                        int nHeight, 
                        CString strPoints, 
                        CString strOutlineColor, 
                        CString strFillColor, 
                        int nLineWidth, 
                        CString strLineStyle, 
                        bool bAutoClose, 
                        bool bIsTransparent );

   // other methods
   CString GetLineStyleStringFromIndex( int lineStyle );
   CString GetColorStringFromColorRef( COLORREF colorRef );
   CString GetFontNameStringFromLogFont( LOGFONT logFont );
   CString GetPointsStringFromPointArray( DrawSdk::DPoint* pPts, 
                                          int nPtCount, 
                                          bool bAutoClose );
   CString GetStartArrowStringFromIndex( int arrowStyle, 
                                         int dir );
   CString GetEndArrowStringFromIndex( int arrowStyle, 
                                       int dir );

   /** images displayed with vml must be copied into the target directory */
   int CopyImageFileIntoTargetDirectory( CString strFileName );

   /** Add the font-name and the TTF filename to the fontlists (if they not already exist) */
   int AddFontToFontList( CString strFontName, 
                          CString strTtfName, 
                          CString strFontWeight, 
                          CString strFontStyle );

   /** Write the font-embedding css file */ 
   int WriteFontEmbeddingCssFile();

   /** Write the event queue into the 'control_vml.js' file */
   int WriteEventsIntoControlFile();

   /** Get a reduced event list without 'Telepointer' events; 
    *  function returns the number of all events */
   int GetReducedEventArrayList(CArray<CWhiteboardEvent*, CWhiteboardEvent*> &eventList);

   /** Upgrade array of used object depending on an event */
   int UpgradeArrayOfUsedObjects( CWhiteboardEvent* pEvent );

//   // obsolete!
//   int CreateVmlPages();
//   int WriteVmlPageContentIntoStream( int pageIndex );
//   int WriteVmlPageObjectsIntoStream( int pageIndex );
//   int WriteVmlEventObjectsIntoStream( CWhiteboardEvent* pEvent, 
//                                       CWhiteboardEvent* pEventBefore, 
//                                       bool bIsPageBegin );
};

#endif // !defined(AFX_VMLSDK_H__50FD999E_43A9_45EF_9436_51E9887DE108__INCLUDED_)

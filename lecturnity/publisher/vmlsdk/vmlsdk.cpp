//////////////////////////////////////////////////////////////////////
//
// vmlsdk.cpp: Implementation of the class CVmlSdk.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "vmlsdk.h"
#include "imc_lecturnity_converter_StreamingMediaConverter.h"
#include "resource.h"


//////////////////////////////////////////////////////////////////////
// Forward Declarations.
//////////////////////////////////////////////////////////////////////

int parseAndWriteVml( CString strInputFilename, 
                      CString strObjectFilename,
                      CString strOutputFilename, 
                      long* eventTimestamps, 
                      int etLength, 
                      int slideWidth, 
                      int slideHeight );

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// class CVmlSdk: Constructor/Destructor
//////////////////////////////////////////////////////////////////////

CVmlSdk::CVmlSdk()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   Init();
}

//////////////////////////////////////////////////////////////////////

CVmlSdk::~CVmlSdk()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   Clear();
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Methods (private)
//////////////////////////////////////////////////////////////////////

void CVmlSdk::Init()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   m_pWhiteboardStream = new CWhiteboardStream();
   m_pWhiteboardStream->Init();

   m_arrPages.RemoveAll();
   m_arrObjects.RemoveAll();

   m_arrVmlFonts.RemoveAll();

   m_arrUsedObjects.RemoveAll();

   m_fp = NULL;

   m_strEventFilename.Empty();
   m_strObjectFilename.Empty();
   m_strOutputFilename.Empty();
   m_strText.Empty();
//   m_strVisibility.Format( "visible" );

   m_lEventTimestamps      = NULL;

   m_nSlideWidth           = -1;
   m_nSlideHeight          = -1;
   m_nObjectIndex          = 0;
   m_nObjectNumber         = 0;
   m_nDefaultFontSize      = 12;
   m_nPointerWidth         = 30;
   m_nPointerHeight        = 30;
}

//////////////////////////////////////////////////////////////////////

void CVmlSdk::Clear()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   if ( m_pWhiteboardStream != NULL )
   {
      m_pWhiteboardStream->Clear();
      delete m_pWhiteboardStream;
   }
   m_pWhiteboardStream = NULL;

   m_arrPages.RemoveAll();
   m_arrObjects.RemoveAll();

   m_arrVmlFonts.RemoveAll();

   m_arrUsedObjects.RemoveAll();

   if ( m_fp != NULL )
   {
      fclose( m_fp );
   }
   m_fp = NULL;

   m_strEventFilename.Empty();
   m_strObjectFilename.Empty();
   m_strOutputFilename.Empty();
   m_strText.Empty();
//   m_strVisibility.Empty();

   delete[] m_lEventTimestamps;
   m_lEventTimestamps = NULL;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreateVmlPage()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   CString vmlFilename;
   CString vmlTemplate;

   vmlFilename.Empty();
   vmlTemplate.Empty();

//   // Reset Object index
//   m_nObjectIndex = 0;
   
   // Create and open HTML file
   // vmlFilename is something like "Demo_vml.html"
   vmlFilename.Format( "%s_vml.html", m_strOutputFilename );
   if ( (m_fp = fopen( vmlFilename, "wt" )) == NULL )
   {
      printf( "VML error: unable to create file %s ! \n", vmlFilename );
      return ERROR_OPEN_FAILED;
   }
      

   // Write VML Header
   vmlTemplate.Format( "Streaming Templates\\vml_1.tmpl" );
   hr = WriteTemplateFileIntoStream( vmlTemplate );
   if ( hr != S_OK )
   {
      printf( "VML error: unable to read template file %s ! \n", vmlTemplate );
      return hr;
   }


   // Write a line withe the <title> tag
   CString strSlide;
//   strSlide.Format( IDS_SLIDE );
//   vmlTemplate.Format( "    <title>VML: %s %d</title>\n", strSlide, 0 );
   vmlTemplate.Format( "    <title>VML objects</title>\n" );
   WriteTextLineIntoStream( vmlTemplate );
   strSlide.Empty();


   // Write VML middle part
   vmlTemplate.Format( "Streaming Templates\\vml_2.tmpl" );
   hr = WriteTemplateFileIntoStream( vmlTemplate );
   if ( hr != S_OK )
   {
      printf( "VML error: unable to read template file %s ! \n", vmlTemplate );
      return hr;
   }

      
   // Write VML content
//   hr = WriteVmlPageContentIntoStream( pageIndex );
   hr = WriteVmlPageContentIntoStream();
   if ( hr != S_OK )
   {
      printf( "VML error: unable to handle VML content ! \n" );
      return hr;
   }


   // Write VML footer
   vmlTemplate.Format( "Streaming Templates\\vml_3.tmpl" );
   hr = WriteTemplateFileIntoStream( vmlTemplate );
   if ( hr != S_OK )
   {
      printf( "VML error: unable to read template file %s ! \n", vmlTemplate );
      return hr;
   }


   // Close HTML file
   if ( fclose( m_fp ) )
   {
      printf( "VML error: unable to close file %s ! \n", vmlFilename );
      return ERROR_FILE_CORRUPT;
   }
   m_fp = NULL;

   StringManipulation::GetFilename( vmlFilename );
   printf( "VML: %s written. \n", vmlFilename );

   vmlTemplate.Empty();
   vmlFilename.Empty();

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::WriteTemplateFileIntoStream( CString tmplFile )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   FILE *fpTemplate;

   if ( (fpTemplate = fopen( tmplFile, "rt" )) == NULL )
   {
      printf( "VML error: unable to open file %s ! \n", tmplFile );
      return ERROR_OPEN_FAILED;
   }

   // Copy template file into the new file
   int count, total = 0;
   char buffer[512];
   while ( !feof(fpTemplate) )
   {
      // Read from template stream
      count = fread( buffer, sizeof( char ), 512, fpTemplate );
      if( ferror( fpTemplate ) )
      {
         printf( "VML error: corrupted template file %s ! \n", tmplFile );
         return ERROR_FILE_CORRUPT;
      }

      // Write into the VML stream
      fwrite( buffer, sizeof( char ), count, m_fp );

      /* Total up actual bytes read */
      total += count;
   }

   if ( fclose( fpTemplate ) )
   {
      printf( "VML error: unable to close file %s ! \n", tmplFile );
      return ERROR_FILE_CORRUPT;
   }
   fpTemplate = NULL;

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::WriteTextLineIntoStream( CString strText )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   return WriteTextLineIntoStream( strText, m_fp );
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::WriteTextLineIntoStream( CString strText, FILE* fp )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Append "\n"
   strText.Format( "%s\n", strText );

   int nLen = strText.GetLength();
   fwrite( strText, sizeof( char ), nLen, fp );

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::WriteVmlPageContentIntoStream()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   m_strText.Empty();

   // Write the wrapping <group> tag
   m_strText.Format( "    <!-- Grouping all VML objects -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "    <v:group id=\"content01\" " );
   WriteTextLineIntoStream( m_strText );
   // Consider 3 extra pixels in the slide dimension for the shadow 
   m_strText.Format( "      style=\"position:absolute;top:0px;left:0px;width:%dpx;height:%dpx;", 
                   m_nSlideWidth+3, m_nSlideHeight+3 );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        visibility:visible;background-color:none;" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        font-size:%dpx\"", 
                     m_nDefaultFontSize);
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      coordsize=\"%d,%d\" coordorigin=\"0,0\">", 
                   m_nSlideWidth+3, m_nSlideHeight+3 );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      <v:stroke on=\"true\" weight=\"1px\" />" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );


   // Write the shadow object
   m_strText.Format( "      <!-- Shadow -->" );
   WriteTextLineIntoStream( m_strText );

   // Set Object Index: -1 for the shadow
   // that is no part of the object queue
   m_nObjectIndex = -1;
   
   COLORREF outlineCol = RGB( 104, 104, 104 ); //( 0x00686868 );
   COLORREF fillCol = RGB( 104, 104, 104 ); //( 0x00686868 );
   WriteVmlRectangle( 3, 3,
                      m_nSlideWidth, m_nSlideHeight, 
                      outlineCol, 
                      fillCol,
                      1, 0 );


   // Write the page objects
   hr = WriteVmlPageObjectsIntoStream();
   if ( hr != S_OK )
   {
      printf( "VML error: unable to write slide content!" );
      return hr;
   }


   // Write a border line around the VML page
   m_strText.Format( "      <!-- Border Line -->" );
   WriteTextLineIntoStream( m_strText );

   outlineCol = RGB( 0, 0, 0 ); //( 0x00000000 );
   WriteVmlRectangle( 0, 0,
                      m_nSlideWidth-1, m_nSlideHeight-1, 
                      outlineCol, 
                      1, 0 );

   // Write the Telepointer object
   hr = WriteVmlTelepointerIntoStream();
   if ( hr != S_OK )
   {
      printf( "VML error: unable to write telepointer object!" );
      return hr;
   }

   
   // Write the wrapping </group> tag
   m_strText.Empty();
   m_strText.Format( "    <!-- End of Grouping all VML objects -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "    </v:group>" );
   WriteTextLineIntoStream( m_strText );

   m_strText.Empty();


   // Decrease object Index: 
   // - the border line does not belong to the VML content
   // - the 3 objects building the Teleointer do not belong to the VML content
   m_nObjectIndex -= 4;


   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::WriteVmlPageObjectsIntoStream()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Reset Object index
   m_nObjectIndex = 0;
   
   int nLength = m_arrObjects.GetSize();

   DrawSdk::DrawObject* pObject = NULL;

   for ( int i = 0; i < nLength; ++i)
   {
      // Detect the object
      pObject = m_arrObjects.ElementAt(i);

      // Detect the object type
      int type = pObject->GetType();

      switch ( type )
      {
         case DrawSdk::RECTANGLE:
            hr = CreateRectangleFromObject( (DrawSdk::Rectangle*)pObject );
            break;

         case DrawSdk::OVAL:
            hr = CreateOvalFromObject( (DrawSdk::Oval*)pObject );
            break;

         case DrawSdk::LINE:
            hr = CreateLineFromObject( (DrawSdk::Line*)pObject );
            break;

         case DrawSdk::TEXT:
            hr = CreateTextFromObject( (DrawSdk::Text*)pObject );
            break;

         case DrawSdk::POLYGON:
            hr = CreatePolygonFromObject( (DrawSdk::Polygon*)pObject );
            break;

         case DrawSdk::MARKER:
            hr = CreateMarkerFromObject( (DrawSdk::Marker*)pObject );
            break;

         case DrawSdk::FREEHAND:
            hr = CreateFreehandFromObject( (DrawSdk::Polygon*)pObject );
            break;

         case DrawSdk::TELEPOINTER:
            hr = CreateTelepointerFromObject( (DrawSdk::Pointer*)pObject );
            break;

         case DrawSdk::IMAGE:
            hr = CreateImageFromObject( (DrawSdk::Image*)pObject );
            break;

         default:
            // do nothing
            break;
      }

      if ( hr != S_OK )
      {
         printf( "VML: Error while writing page objects into stream! \n" );
         return hr;
      }
   }

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::WriteVmlTelepointerIntoStream()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int x = 0;
   int y = 0;
   int width = m_nPointerWidth;
   int height = m_nPointerHeight;

   COLORREF outlineColor;
   COLORREF fillColor;
   int lineWidth = 1;
   int lineStyle = 0;

   // Calculate 4 points for the Telepointer shape
   DrawSdk::DPoint* pPts = (DrawSdk::DPoint*)malloc( 4 * sizeof(DrawSdk::DPoint) );

   pPts[0].x = x + width - 1;
   pPts[0].y = y + 0;
   pPts[1].x = x + 0;
   pPts[1].y = y + (int)( 0.50 * height ) - 1;
   pPts[2].x = x + (int)( 0.35 * width );
   pPts[2].y = y + (int)( 0.65 * height );
   pPts[3].x = x + (int)( 0.50 * width );
   pPts[3].y = y + height - 1;

   // 3 Points for the red triangles
   DrawSdk::DPoint* pPtsT = (DrawSdk::DPoint*)malloc( 3 * sizeof(DrawSdk::DPoint) );


   m_strText.Empty();

   // Write the wrapping <group> tag
   m_strText.Format( "      <!-- TELEPOINTER -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      <v:group id=\"vmlpointer\" " );
   WriteTextLineIntoStream( m_strText );
   // Consider 3 extra pixels in the slide dimension for the shadow 
   m_strText.Format( "        style=\"position:absolute;top:0px;left:0px;width:%dpx;height:%dpx;", 
                   m_nSlideWidth+3, m_nSlideHeight+3 );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        visibility:hidden;background-color:none;\" " );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        coordsize=\"%d,%d\" coordorigin=\"0,0\">", 
                   m_nSlideWidth+3, m_nSlideHeight+3 );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );

   // Write the red triangle
   outlineColor = RGB( 255, 0, 0 ); //( 0x00FF0000 );
   fillColor = RGB( 255, 0, 0 ); //( 0x00FF0000 );
   pPtsT[0].x = pPts[0].x;
   pPtsT[0].y = pPts[0].y;
   pPtsT[1].x = pPts[1].x;
   pPtsT[1].y = pPts[1].y;
   pPtsT[2].x = pPts[2].x;
   pPtsT[2].y = pPts[2].y;

   hr = WriteVmlPolygon( x, y, width, height, 
                         pPtsT, 3, 
                         outlineColor, fillColor, lineWidth, lineStyle );

   // Write the dark red triangle
   outlineColor = RGB( 128, 0, 0 ); //( 0x00800000 );
   fillColor = RGB( 128, 0, 0 ); //( 0x00800000 );
   pPtsT[0].x = pPts[0].x;
   pPtsT[0].y = pPts[0].y;
   pPtsT[1].x = pPts[2].x;
   pPtsT[1].y = pPts[2].y;
   pPtsT[2].x = pPts[3].x;
   pPtsT[2].y = pPts[3].y;

   hr = WriteVmlPolygon( x, y, width, height, 
                         pPtsT, 3, 
                         outlineColor, fillColor, lineWidth, lineStyle );

   // Write the border line
   outlineColor = RGB( 0, 0, 0 ); //( 0x00000000 );
   fillColor = RGB( 0, 0, 0 ); //( 0x00000000 );

   hr = WriteVmlPolygon( x, y, width, height, 
                         pPts, 4, 
                         outlineColor, lineWidth, lineStyle, false );

   // Write the wrapping </group> tag
   m_strText.Format( "      <!-- TELEPOINTER -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      </v:group>" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );


   // Clear 
   delete pPts;
   delete pPtsT;
   pPts = NULL;
   pPtsT = NULL;

   m_strText.Empty();

   return hr;
}

//////////////////////////////////////////////////////////////////////

/**
 * Returns -1, if no Telepointer is found 
 * - otherwise returns the index of the telepointer in the object array
 */
int CVmlSdk::GetTelepointerIndexFromEvent( CWhiteboardEvent* pEvent )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   int tpIndex = -1;

   if ( pEvent == NULL )
      return tpIndex;


   int nNumOfObjects = pEvent->GetObjectsPointer()->GetSize();
   DrawSdk::DrawObject* pDrawObject = NULL;
   
   for ( int i = 0; i < nNumOfObjects; ++i )
   {
      pDrawObject = (DrawSdk::DrawObject*)pEvent->GetObjectsPointer()->ElementAt(i);
      if ( pDrawObject->GetType() == DrawSdk::TELEPOINTER )
      {
         tpIndex = i;
         break;
      }
   }

   return tpIndex;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreateRectangleFromObject( DrawSdk::Rectangle* pObject )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int x       = (int)( pObject->GetX() );
   int y       = (int)( pObject->GetY() );
   int width   = (int)( pObject->GetWidth() );
   int height  = (int)( pObject->GetHeight() );

   COLORREF outlineColor = pObject->GetOutlineColor();
   COLORREF fillColor = pObject->GetFillColor();
   int lineWidth = pObject->GetLineWidth();
   int lineStyle = pObject->GetLineStyle();
   bool isFilled = pObject->GetIsFilled();

   // Decrease width/height about 1px
   width--;
   height--;

   if ( isFilled )
   {
      hr = WriteVmlRectangle( x, y, width, height, 
                              outlineColor, fillColor, 
                              lineWidth, lineStyle );
   }
   else
   {
      hr = WriteVmlRectangle( x, y, width, height, 
                              outlineColor, 
                              lineWidth, lineStyle );
   }

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreateOvalFromObject( DrawSdk::Oval* pObject )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int x       = (int)( pObject->GetX() );
   int y       = (int)( pObject->GetY() );
   int width   = (int)( pObject->GetWidth() );
   int height  = (int)( pObject->GetHeight() );

   COLORREF outlineColor = pObject->GetOutlineColor();
   COLORREF fillColor = pObject->GetFillColor();
   int lineWidth = pObject->GetLineWidth();
   int lineStyle = pObject->GetLineStyle();
   bool isFilled = pObject->GetIsFilled();

   // Decrease width/height about 1px
   width--;
   height--;

   if ( isFilled )
   {
      hr = WriteVmlOval( x, y, width, height, 
                         outlineColor, fillColor, 
                         lineWidth, lineStyle );
   }
   else
   {
      hr = WriteVmlOval( x, y, width, height, 
                         outlineColor, 
                         lineWidth, lineStyle );
   }

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreateLineFromObject( DrawSdk::Line* pObject )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int x       = (int)( pObject->GetX() );
   int y       = (int)( pObject->GetY() );
   int width   = (int)( pObject->GetWidth() );
   int height  = (int)( pObject->GetHeight() );

   COLORREF outlineColor = pObject->GetOutlineColor();
   int lineWidth = pObject->GetLineWidth();
   int lineStyle = pObject->GetLineStyle();

   int arrowStyle = pObject->GetArrowStyle();
   int ori = pObject->GetOrientation();
   int dir = pObject->GetDirection();

   hr = WriteVmlLine( x, y, width, height, 
                      outlineColor, lineWidth, lineStyle, 
                      arrowStyle, ori, dir );

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreateTextFromObject( DrawSdk::Text* pObject )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int x       = (int)( pObject->GetX() );
   int y       = (int)( pObject->GetY() );
   int width   = (int)( pObject->GetWidth() );
   int height  = (int)( pObject->GetHeight() );

   int ascent = pObject->GetTmAscent();

   COLORREF outlineColor = pObject->GetOutlineColor();

   CString strText( pObject->GetString() );
   int textLength = pObject->GetTextLength();
   LOGFONT lfFont = pObject->GetLogFont();
   CString strTtfName = pObject->GetTtfName();

   hr = WriteVmlText( x, y, width, height, ascent, 
                      outlineColor, strText, textLength, 
                      lfFont, strTtfName );
   
   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreatePolygonFromObject( DrawSdk::Polygon* pObject )
{
   return CreatePolygonFromObject( pObject, false );
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreatePolygonFromObject( DrawSdk::Polygon* pObject, bool bIsMarker )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int x       = (int)( pObject->GetX() );
   int y       = (int)( pObject->GetY() );
   int width   = (int)( pObject->GetWidth() );
   int height  = (int)( pObject->GetHeight() );

   // Get the array with the points
   int ptCount = pObject->GetPoints( NULL, 0 );
   DrawSdk::DPoint* pPts = (DrawSdk::DPoint*)malloc( ptCount * sizeof(DrawSdk::DPoint) );
   hr = pObject->GetPoints( pPts, ptCount );

   COLORREF outlineColor = pObject->GetOutlineColor();
   COLORREF fillColor = pObject->GetFillColor();
   int lineWidth = pObject->GetLineWidth();
   int lineStyle = pObject->GetLineStyle();
   bool bAutoClose = pObject->GetIsAutoClose();
   bool isFreehand = pObject->GetIsFreehand();

   if ( !bAutoClose || isFreehand )
   {
      // Polyline, Freehand, Marker
      hr = WriteVmlPolygon( x, y, width, height, 
                            pPts, ptCount, 
                            outlineColor, lineWidth, lineStyle, bIsMarker );
   }
   else
   {
      // Closed Polygon
      hr = WriteVmlPolygon( x, y, width, height, 
                            pPts, ptCount, 
                            outlineColor, fillColor, lineWidth, lineStyle );
   }

   delete pPts;
   pPts = NULL;

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreateMarkerFromObject( DrawSdk::Marker* pObject )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Can be handled like a "Polygon"
   hr = CreatePolygonFromObject( pObject, true );

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreateFreehandFromObject( DrawSdk::Polygon* pObject )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Can be handled like a "Polygon"
   hr = CreatePolygonFromObject( pObject );

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreateTelepointerFromObject( DrawSdk::Pointer* pObject )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int x       = (int)( pObject->GetX() );
   int y       = (int)( pObject->GetY() );
   int width   = (int)( ((DrawSdk::DrawObject *) pObject)->GetWidth() );
   int height  = (int)( ((DrawSdk::DrawObject *) pObject)->GetHeight() );

   m_nPointerWidth   = width;
   m_nPointerHeight  = height;
 
   hr = WriteVmlPointer( x, y, width, height );

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::CreateImageFromObject( DrawSdk::Image* pObject )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int x       = (int)( pObject->GetX() );
   int y       = (int)( pObject->GetY() );
   int width   = (int)( ((DrawSdk::DrawObject *) pObject)->GetWidth() );
   int height  = (int)( ((DrawSdk::DrawObject *) pObject)->GetHeight() );
   CString strImageName( pObject->GetImageName() );
   // Remove Path from the file name
   StringManipulation::GetFilename( strImageName );

   // Copy the image to the destination directory
   hr = CopyImageFileIntoTargetDirectory( strImageName );
   if ( hr != S_OK )
   {
      printf( "VML error: Error while copying image file!\n" );
      return hr;
   }

   hr = WriteVmlImage( x, y, width, height, 
                       strImageName );

   return hr;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/* <IMAGE> */
int CVmlSdk::WriteVmlPointer( int nX, 
                              int nY, 
                              int nWidth, 
                              int nHeight )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   m_strText.Empty();

/*
   // This can be:
   m_strText.Format( "      <!-- <POINTER> -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      <v:shape id=\"vmlobj%d\" stroke=\"false\" style=\"position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx; ",
                     m_nObjectIndex, nX, nY, nWidth, nHeight );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        visibility:%s;\">", 
                     m_strVisibility );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      </v:shape>" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );
*/

   // or (shorter):

/*
   m_strText.Format( "      <!-- <POINTER> -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      <v:shape id=\"vmlobj%d\" stroke=\"false\" fill=\"false\"></v:shape> ",
                     m_nObjectIndex );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );

   m_strText.Empty();
*/

   // ---------------------------
   // Telepointer is an extra object created at the end of the object list.
   // --> Don't write anything here!
   // ---------------------------
   
   // Increase object index number
   m_nObjectIndex++;

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* <IMAGE> */
int CVmlSdk::WriteVmlImage( int nX,
                            int nY, 
                            int nWidth, 
                            int nHeight, 
                            CString strImageName )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Debug Info
   //printf( "*WriteVmlImage(%d, %d, %d, %d, %s)\n", nX, nY, nWidth, nHeight, strImageName );

   m_strText.Empty();

   m_strText.Format( "      <!-- <IMAGE> -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      <v:image id=\"vmlobj%d\" src=\"%s\" style=\"position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;\">",
                     m_nObjectIndex, strImageName, nX, nY, nWidth, nHeight );
   WriteTextLineIntoStream( m_strText );
//   m_strText.Format( "        visibility:%s;\">", 
//                     m_strVisibility );
//   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      </v:image>" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );

   m_strText.Empty();

   // Increase object index number
   m_nObjectIndex++;

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* <LINE> */
int CVmlSdk::WriteVmlLine( int nX,
                           int nY, 
                           int nWidth, 
                           int nHeight, 
                           COLORREF clrOutlineColor, 
                           int nLineWidth, 
                           int nLineStyle,
                           int nArrowStyle, 
                           int nOri, 
                           int nDir )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Debug Info
   //printf( "*WriteVmlLine(%d, %d, %d, %d, 0x%08x, %d, %d, %d, %d, %d)\n", nX, nY, nWidth, nHeight, clrOutlineColor, nLineWidth, nLineStyle, nArrowStyle, nOri, nDir );

   m_strText.Empty();

   // Note: Orientation and direction are considered in 'DrawSdk::Line' objects
   // nHeight can be a negative number!

   int x1 = nX;
   int y1 = nY;
   int x2 = nX + nWidth;
   int y2 = nY + nHeight;

   CString strOutlineColor( GetColorStringFromColorRef(clrOutlineColor) );
   CString strLineStyle( GetLineStyleStringFromIndex(nLineStyle) );
   CString strStartArrow( GetStartArrowStringFromIndex(nArrowStyle, nDir) ); 
   CString strEndArrow( GetEndArrowStringFromIndex(nArrowStyle, nDir) ); 

   m_strText.Format( "      <!-- <LINE> -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      <v:line id=\"vmlobj%d\" from=\"%d,%d\" to=\"%d,%d\" strokeweight=\"%dpx\" ",
                     m_nObjectIndex, x1, y1, x2, y2, nLineWidth );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        style=\"position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;\">",
                     0, 0, nWidth, nHeight );
   WriteTextLineIntoStream( m_strText );
//   m_strText.Format( "        visibility:%s;\">", 
//                     m_strVisibility );
//   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        <v:stroke on=\"true\" weight=\"%dpx\" color=\"%s\" dashstyle=\"%s\" ",
                     nLineWidth, strOutlineColor, strLineStyle );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "          startarrow=\"%s\" endarrow=\"%s\"", 
                     strStartArrow, strEndArrow );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "          endcap=\"square\" />" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      </v:line>" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );

   strOutlineColor.Empty();
   strLineStyle.Empty();
   strStartArrow.Empty();
   strEndArrow.Empty();

   m_strText.Empty();

   // Increase object index number
   m_nObjectIndex++;

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* <FILLEDPOLY> */
int CVmlSdk::WriteVmlPolygon( int nX, 
                              int nY, 
                              int nWidth, 
                              int nHeight, 
                              DrawSdk::DPoint* pPts, 
                              int nPtCount,
                              COLORREF clrOutlineColor, 
                              COLORREF clrFillColor, 
                              int nLineWidth, 
                              int nLineStyle )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   m_strText.Empty();

   bool bAutoClose = true;
   bool bIsTransparent = false;

   CString strPoints( GetPointsStringFromPointArray(pPts, nPtCount, bAutoClose) );
   CString strOutlineColor( GetColorStringFromColorRef(clrOutlineColor) );
   CString strFillColor( GetColorStringFromColorRef(clrFillColor) );
   CString strLineStyle( GetLineStyleStringFromIndex(nLineStyle) );

   m_strText.Format( "      <!-- <FILLEDPOLY> -->" );
   WriteTextLineIntoStream( m_strText );
   
   WriteVmlPolygon( nX,
                    nY, 
                    nWidth, 
                    nHeight, 
                    strPoints, 
                    strOutlineColor, 
                    strFillColor, 
                    nLineWidth, 
                    strLineStyle, 
                    bAutoClose, 
                    bIsTransparent );

   strPoints.Empty();
   strOutlineColor.Empty();
   strFillColor.Empty();
   strLineStyle.Empty();

   m_strText.Empty();

   // Increase object index number
   m_nObjectIndex++;

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* <FREEHAND>,<POLYLINE> */
int CVmlSdk::WriteVmlPolygon( int nX, 
                              int nY, 
                              int nWidth, 
                              int nHeight, 
                              DrawSdk::DPoint* pPts, 
                              int nPtCount,
                              COLORREF clrOutlineColor, 
                              int nLineWidth, 
                              int nLineStyle, 
                              bool bIsMarker )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   m_strText.Empty();

   bool bAutoClose = false;
   bool bIsTransparent = bIsMarker;

   CString strPoints( GetPointsStringFromPointArray(pPts, nPtCount, bAutoClose) );
   CString strOutlineColor( GetColorStringFromColorRef(clrOutlineColor) );
   CString strFillColor( "none" );
   CString strLineStyle( GetLineStyleStringFromIndex(nLineStyle) );

   m_strText.Format( "      <!-- <POLYLINE> -->" );
   WriteTextLineIntoStream( m_strText );
   
   WriteVmlPolygon( nX,
                    nY, 
                    nWidth, 
                    nHeight, 
                    strPoints, 
                    strOutlineColor, 
                    strFillColor, 
                    nLineWidth, 
                    strLineStyle, 
                    bAutoClose, 
                    bIsTransparent );

   strPoints.Empty();
   strOutlineColor.Empty();
   strFillColor.Empty();
   strLineStyle.Empty();

   m_strText.Empty();

   // Increase object index number
   m_nObjectIndex++;

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* <FILLEDCIRC> */
int CVmlSdk::WriteVmlOval( int nX,
                           int nY, 
                           int nWidth, 
                           int nHeight, 
                           COLORREF clrOutlineColor, 
                           COLORREF clrFillColor, 
                           int nLineWidth, 
                           int nLineStyle )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Debug Info
   //printf( "*WriteVmlOval(%d, %d, %d, %d, 0x%08x, 0x%08x, %d, %d)\n", nX, nY, nWidth, nHeight, clrOutlineColor, clrFillColor, nLineWidth, nLineStyle );

   m_strText.Empty();

   CString strType( "oval" );
   CString strOutlineColor( GetColorStringFromColorRef(clrOutlineColor) );
   CString strFillColor( GetColorStringFromColorRef(clrFillColor) );
   CString strLineStyle( GetLineStyleStringFromIndex(nLineStyle) );

   m_strText.Format( "      <!-- <FILLEDCIRC> -->" );
   WriteTextLineIntoStream( m_strText );
   
   WriteVmlRectCircObject( strType, 
                           nX, 
                           nY, 
                           nWidth, 
                           nHeight, 
                           strOutlineColor, 
                           strFillColor, 
                           nLineWidth, 
                           strLineStyle );

      

   strType.Empty();
   strOutlineColor.Empty();
   strFillColor.Empty();
   strLineStyle.Empty();

   m_strText.Empty();

   // Increase object index number
   m_nObjectIndex++;

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* <OUTLINECIRC> */
int CVmlSdk::WriteVmlOval( int nX,
                           int nY, 
                           int nWidth, 
                           int nHeight, 
                           COLORREF clrOutlineColor, 
                           int nLineWidth, 
                           int nLineStyle )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Debug Info
   //printf( "*WriteVmlOval(%d, %d, %d, %d, 0x%08x, %d, %d)\n", nX, nY, nWidth, nHeight, clrOutlineColor, nLineWidth, nLineStyle );

   m_strText.Empty();

   CString strType( "oval" );
   CString strOutlineColor( GetColorStringFromColorRef(clrOutlineColor) );
   CString strFillColor( "none" );
   CString strLineStyle( GetLineStyleStringFromIndex(nLineStyle) );

   m_strText.Format( "      <!-- <OUTLINECIRC> -->" );
   WriteTextLineIntoStream( m_strText );
   
   WriteVmlRectCircObject( strType, 
                           nX, 
                           nY, 
                           nWidth, 
                           nHeight, 
                           strOutlineColor, 
                           strFillColor, 
                           nLineWidth, 
                           strLineStyle );

      

   strType.Empty();
   strOutlineColor.Empty();
   strFillColor.Empty();
   strLineStyle.Empty();

   m_strText.Empty();

   // Increase object index number
   m_nObjectIndex++;

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* <FILLEDRECT> */
int CVmlSdk::WriteVmlRectangle( int nX,
                                int nY, 
                                int nWidth, 
                                int nHeight, 
                                COLORREF clrOutlineColor, 
                                COLORREF clrFillColor, 
                                int nLineWidth, 
                                int nLineStyle )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Debug Info
   //printf( "*WriteVmlRectangle(%d, %d, %d, %d, 0x%08x, 0x%08x, %d, %d)\n", nX, nY, nWidth, nHeight, clrOutlineColor, clrFillColor, nLineWidth, nLineStyle );

   m_strText.Empty();

   CString strType( "rect" );
   CString strOutlineColor( GetColorStringFromColorRef(clrOutlineColor) );
   CString strFillColor( GetColorStringFromColorRef(clrFillColor) );
   CString strLineStyle( GetLineStyleStringFromIndex(nLineStyle) );

   m_strText.Format( "      <!-- <FILLEDRECT> -->" );
   WriteTextLineIntoStream( m_strText );
   
   WriteVmlRectCircObject( strType, 
                           nX, 
                           nY, 
                           nWidth, 
                           nHeight, 
                           strOutlineColor, 
                           strFillColor, 
                           nLineWidth, 
                           strLineStyle );

      

   strType.Empty();
   strOutlineColor.Empty();
   strFillColor.Empty();
   strLineStyle.Empty();

   m_strText.Empty();

   // Increase object index number
   m_nObjectIndex++;

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* <OUTLINERECT> */
int CVmlSdk::WriteVmlRectangle( int nX,
                                int nY, 
                                int nWidth, 
                                int nHeight, 
                                COLORREF clrOutlineColor, 
                                int nLineWidth, 
                                int nLineStyle )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Debug Info
   //printf( "*WriteVmlRectangle(%d, %d, %d, %d, 0x%08x, %d, %d)\n", nX, nY, nWidth, nHeight, clrOutlineColor, nLineWidth, nLineStyle );

   m_strText.Empty();

   CString strType( "rect" );
   CString strOutlineColor( GetColorStringFromColorRef(clrOutlineColor) );
   CString strFillColor( "none" );
   CString strLineStyle( GetLineStyleStringFromIndex(nLineStyle) );

   m_strText.Format( "      <!-- <FILLEDRECT> -->" );
   WriteTextLineIntoStream( m_strText );
   
   WriteVmlRectCircObject( strType, 
                           nX, 
                           nY, 
                           nWidth, 
                           nHeight, 
                           strOutlineColor, 
                           strFillColor, 
                           nLineWidth, 
                           strLineStyle );

      

   strType.Empty();
   strOutlineColor.Empty();
   strFillColor.Empty();
   strLineStyle.Empty();

   m_strText.Empty();

   // Increase object index number
   m_nObjectIndex++;

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* <TEXT>,<TEXTITEM> */
int CVmlSdk::WriteVmlText( int nX,
                           int nY, 
                           int nWidth, 
                           int nHeight, 
                           int nAscent, 
                           COLORREF clrOutlineColor, 
                           CString strText,
                           int nTextLength, 
                           LOGFONT lfFont, 
                           CString strTtfName )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Debug Info
   //printf( "*WriteVmlText(%d, %d, %d, %d, %d, 0x%08x, %s, %d, %d, %d, %d, %s, %s)\n", nX, nY, nWidth, nHeight, nAscent, clrOutlineColor, strText, nTextLength, lfFont.lfHeight, lfFont.lfWeight, lfFont.lfItalic, lfFont.lfFaceName, strTtfName );
   
   int nFontSize = (-1) * lfFont.lfHeight; 
   int percentageFontSize = (int)( 0.5 + (100.0 * (double)nFontSize / (double)m_nDefaultFontSize) );
   CString strFontName( lfFont.lfFaceName );
   CString strFontWeight;
   CString strFontStyle;
   CString strColor( GetColorStringFromColorRef(clrOutlineColor) );

   if ( lfFont.lfWeight == FW_BOLD )
      strFontWeight.Format( "bold" );
   else
      strFontWeight.Format( "normal" );

   if ( lfFont.lfItalic == TRUE )
      strFontStyle.Format( "italic" );
   else
      strFontStyle.Format( "normal" );


   m_strText.Empty();

   // Note: vertical position of the text must be corrected by font base line (ascent)
   nY -= nAscent;

/*   
   // 1. choice: Text in <v:rect>: 
   // - advantage: text can be marked and copied
   // - disadvantage: text size can not be fitted to the bounding rectangle

   // Font size must be extended a little bit (empirically)
   percentageFontSize = (int)( 1.01 * percentageFontSize );
   // Bounding Box must be extended a little bit to hold all text (empirically)
   nWidth  = (int)( 1.05 * nWidth ); 
   nHeight = (int)( 1.05 * nHeight ); 

   m_strText.Format( "      <!-- <TEXTITEM> -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      <v:rect id=\"vmlobj%d\" style=\"position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;\">",
                     m_nObjectIndex, nX, nY, nWidth, nHeight );
   WriteTextLineIntoStream( m_strText );
//   m_strText.Format( "        visibility:%s;\">", 
//                     m_strVisibility );
//   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        <v:stroke on=\"false\" />" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        <v:fill on=\"false\" />" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        <v:textpath style=\"font-family:\'%s\';font-size:%d%%;font-weight:\'%s\';font-style:\'%s\';color:\'%s\';\">",
                     strFontName, percentageFontSize, strFontWeight, strFontStyle, strColor );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "          %s", 
                     strText );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        </v:textpath>" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      </v:rect>" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );
*/

///*
   // 2. choice: "Render mode"
   // The use of <v:shape> allows fitting via 'fitpath' (--> 'font-size' has no effect any more)
   // - advantage: text size can be fitted to the bounding rectangle
   // - disadvantage: text can not be marked and copied

   // Note: vertical position of the text must be corrected by font size
   nY += nAscent; // undo the shift done above
   int deltaY = nFontSize - nHeight; 

   m_strText.Format( "      <!-- <TEXTITEM> -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      <v:shape id=\"vmlobj%d\" coordsize=\"%d,%d\" style=\"position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;antialias:true;\">",
                     m_nObjectIndex, nWidth, nHeight, nX, nY, nWidth, nHeight );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        <v:path textpathok=\"true\" v=\"m %d,%d l %d,%d\" />", 
                     0, deltaY, nWidth, deltaY );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        <v:stroke on=\"false\" />" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        <v:fill on=\"true\" color=\"%s\" />", 
                     strColor );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        <v:textpath on=\"true\" fitpath=\"true\" style=\"font-family:\'%s\';font-size:%d%%;font-weight:\'%s\';font-style:\'%s\';\" ",
                     strFontName, percentageFontSize, strFontWeight, strFontStyle );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "          string=\"%s\" />", 
                     strText );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      </v:shape>" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );
//*/

/*
   // 3. choice: "Text outside of VML"
   // - advantage: text can be marked and copied
   // - disadvantage: text size can not be fitted to the bounding rectangle

   // Position and size in relative coordinates (in %; like MS Producer)
   float percentageX      = (float)( 100.0f * (float)nX / (float)m_nSlideWidth );
   float percentageY      = (float)( 100.0f * (float)nY / (float)m_nSlideHeight );
   float percentageWidth  = (float)( 110.0f * (float)nWidth / (float)m_nSlideWidth );
   float percentageHeight = (float)( 110.0f * (float)nHeight / (float)m_nSlideHeight );
   // Font size must be extended a little bit (empirically)
   percentageFontSize = (int)( 1.01 * percentageFontSize );

   m_strText.Format( "      <!-- <TEXTITEM> -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      <span id=\"vmlobj%d\" style=\"position:absolute;left:%g%%;top:%g%%;width:%g%%;height:%g%%;antialias:true;font-family:\'%s\';font-size:%d%%;font-weight:\'%s\';font-style:\'%s\';color:\'%s\';\">",
                     m_nObjectIndex, percentageX, percentageY, percentageWidth, percentageHeight, strFontName, percentageFontSize, strFontWeight, strFontStyle, strColor );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "          %s", 
                     strText );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      </span>" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );

*/


   // Add the font-name and the TTF filename to the fontlists (if they not already exist)
   AddFontToFontList( strFontName, 
                      strTtfName, 
                      strFontWeight, 
                      strFontStyle );

   strFontName.Empty();
   strFontWeight.Empty();
   strFontStyle.Empty();
   strColor.Empty();
   
   m_strText.Empty();

   // Increase object index number
   m_nObjectIndex++;

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* <TEXT>,<TEXTITEM> */
int CVmlSdk::WriteVmlText( int nX,
                           int nY, 
                           int nWidth, 
                           int nHeight, 
                           int nAscent, 
                           COLORREF clrOutlineColor, 
                           CString strText,
                           int nTextLength, 
                           LOGFONT lfFont )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   CString strTtfName( "none" );

   hr = WriteVmlText( nX,
                      nY, 
                      nWidth, 
                      nHeight, 
                      nAscent, 
                      clrOutlineColor, 
                      strText, 
                      nTextLength, 
                      lfFont, 
                      strTtfName );

   strTtfName.Empty();

   return hr;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/* OVAL,RECT Object */
int CVmlSdk::WriteVmlRectCircObject( CString strType, 
                                     int nX, 
                                     int nY, 
                                     int nWidth, 
                                     int nHeight, 
                                     CString strOutlineColor, 
                                     CString strFillColor, 
                                     int nLineWidth, 
                                     CString strLineStyle )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   m_strText.Empty();

   m_strText.Format( "      <v:%s id=\"vmlobj%d\" style=\"position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;\">",
                     strType, m_nObjectIndex, nX, nY, nWidth, nHeight );
   WriteTextLineIntoStream( m_strText );
//   m_strText.Format( "        visibility:%s;\">", 
//                     m_strVisibility );
//   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        <v:stroke on=\"true\" weight=\"%dpx\" color=\"%s\" dashstyle=\"%s\" joinstyle=\"round\" endcap=\"square\" />",
                     nLineWidth, strOutlineColor, strLineStyle );
   WriteTextLineIntoStream( m_strText );
   if ( strFillColor == "none" )
      m_strText.Format( "        <v:fill on=\"false\" />" );
   else
      m_strText.Format( "        <v:fill on=\"true\" color=\"%s\" />", 
                        strFillColor );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      </v:%s>", 
                     strType );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );

   m_strText.Empty();

   return hr;
}

//////////////////////////////////////////////////////////////////////

/* POLYLINE,FREEHAND,FILLEDPOLY */
int CVmlSdk::WriteVmlPolygon( int nX, 
                              int nY, 
                              int nWidth, 
                              int nHeight, 
                              CString strPoints, 
                              CString strOutlineColor, 
                              CString strFillColor, 
                              int nLineWidth, 
                              CString strLineStyle, 
                              bool bAutoClose, 
                              bool bIsTransparent )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Special case: 'Marker': transparent --> 50% opacity (default: 100%)
   float opacity = 1.0;
   if ( bIsTransparent )
      opacity = 0.5;

   // Debug Info
   // Note: 'bAutoClose' is used for this output only
   //printf( "*WriteVmlPolygon(%s, 0x00%s, 0x00%s, %d, %s, %d, %d)\n", strPoints, strOutlineColor, strFillColor, nLineWidth, strLineStyle, bAutoClose, bIsTransparent );

   m_strText.Empty();

   m_strText.Format( "      <v:polyline id=\"vmlobj%d\" points=\"%s\" ",
                     m_nObjectIndex, strPoints );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        style=\"position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;\">",
                     0, 0, nWidth, nHeight );
   WriteTextLineIntoStream( m_strText );
//   m_strText.Format( "        visibility:%s;\">", 
//                     m_strVisibility );
//   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        <v:stroke on=\"true\" weight=\"%dpx\" color=\"%s\" dashstyle=\"%s\" joinstyle=\"miter\" endcap=\"square\" opacity=\"%g\" />",
                     nLineWidth, strOutlineColor, strLineStyle, opacity );
   WriteTextLineIntoStream( m_strText );
   if ( strFillColor == "none" )
      m_strText.Format( "        <v:fill on=\"false\" />" );
   else
      m_strText.Format( "        <v:fill on=\"true\" color=\"%s\" />", 
                        strFillColor );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      </v:polyline>" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );
   
   m_strText.Empty();

   return hr;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CString CVmlSdk::GetLineStyleStringFromIndex( int lineStyle )
{
   CString strText;
   strText.Empty();

   switch ( lineStyle )
   {
      case 0: 
               strText.Format( "solid" );
               break;
      case 1: 
               strText.Format( "shortdot" );
               break;
      case 2: 
               strText.Format( "shortdash" );
               break;
      default:
               strText.Format( "solid" );
               break;
   }

   return strText;
}

//////////////////////////////////////////////////////////////////////

CString CVmlSdk::GetColorStringFromColorRef( COLORREF colorRef )
{
   CString strText;
   strText.Empty();

   strText.Format( "#%02x%02x%02x", 
                   GetRValue( colorRef ), 
                   GetGValue( colorRef ), 
                   GetBValue( colorRef ) );

   return strText;
}

//////////////////////////////////////////////////////////////////////

CString CVmlSdk::GetFontNameStringFromLogFont( LOGFONT logFont )
{
   CString strText;
   strText.Empty();

   return strText;
}

//////////////////////////////////////////////////////////////////////

CString CVmlSdk::GetPointsStringFromPointArray( DrawSdk::DPoint* pPts, 
                                                int nPtCount, 
                                                bool bAutoClose )
{
   CString strText;
   CString strXY;
   strText.Empty();
   strXY.Empty();

   int nLen = 0;

   for ( int i = 0; i < nPtCount; ++i )
   {
      if ( i == 0 )
         strXY.Format("%g %g", pPts[i].x, pPts[i].y );
      else
         strXY.Format(" %g %g", pPts[i].x, pPts[i].y );

      nLen = strText.GetLength();
      strText.Insert( nLen, strXY );
   }

   // Closed Polygon?
   if ( bAutoClose )
   {
      strXY.Format(" %g %g", pPts[0].x, pPts[0].y );
      nLen = strText.GetLength();
      strText.Insert( nLen, strXY );
   }

   strXY.Empty();

   return strText;
}

//////////////////////////////////////////////////////////////////////

CString CVmlSdk::GetStartArrowStringFromIndex( int arrowStyle, 
                                               int dir )
{
   CString strText;
   strText.Empty();

   // Note: In 'DrawSdk::Line' arrowStyle '1' and '2' are exchanged!

   switch ( arrowStyle )
   {
      case 0: 
               strText.Format( "none" );
               break;
      case 1: 
               if ( dir == 1 )
                  strText.Format( "none" );
               else // default
                  strText.Format( "block" );
               break;
      case 2: 
               if ( dir == 1 )
                  strText.Format( "block" );
               else // default
                  strText.Format( "none" );
               break;
      case 3: 
               strText.Format( "block" );
               break;
      default:
               strText.Format( "none" );
               break;
   }

   return strText;
}

//////////////////////////////////////////////////////////////////////

CString CVmlSdk::GetEndArrowStringFromIndex( int arrowStyle, 
                                             int dir )
{
   CString strText;
   strText.Empty();

   // Note: In 'DrawSdk::Line' arrowStyle '1' and '2' are exchanged!

   switch ( arrowStyle )
   {
      case 0: 
               strText.Format( "none" );
               break;
      case 1: 
               if ( dir == 1 )
                  strText.Format( "block" );
               else // default
                  strText.Format( "none" );
               break;
      case 2: 
               if ( dir == 1 )
                  strText.Format( "none" );
               else // default
                  strText.Format( "block" );
               break;
      case 3: 
               strText.Format( "block" );
               break;
      default:
               strText.Format( "none" );
               break;
   }

   return strText;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int CVmlSdk::CopyImageFileIntoTargetDirectory( CString strFileName )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   CString strSource;
   CString strDestination;

   strSource.Empty();
   strDestination.Empty();

   // Insert '\' at the beginning of the file name
   strFileName.Insert( 0, '\\' );

   // Extract the source path from event or object file name (without '\')
   strSource.Format( "%s", m_strEventFilename );
   StringManipulation::GetPath( strSource );
   // Append '\' and image file name
   strSource.Insert( strSource.GetLength(), strFileName );

   // Extract the destination path from the output file name (without '\')
   strDestination.Format( "%s", m_strOutputFilename );
   StringManipulation::GetPath( strDestination );
   // Append '\' and image file name
   strDestination.Insert( strDestination.GetLength(), strFileName );


   // Copy the image file (overwrite existing file)
   hr = CopyFile( strSource, strDestination, FALSE );
   if ( hr == 0 )
   {
      printf( "VML error: Unable to copy image file %s!\n", strDestination );
      hr = ERROR_CANNOT_COPY;
   }
   else
   {
      hr = S_OK;
      printf( "VML: image file %s copied. \n", strFileName );
   }

   strSource.Empty();
   strDestination.Empty();

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::AddFontToFontList( CString strFontName, 
                                CString strTtfName, 
                                CString strFontWeight, 
                                CString strFontStyle )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   // Debug Info
   //printf( "** AddFontToFontList(%s, %s, %s, %s)\n", strFontName, strTtfName, strFontWeight, strFontStyle );


   // Is TTF file name a real file name?
   // If not, do nothing here
   if ( strTtfName == "none" )
   {
      return hr;
   }


   CVmlFont* pVmlFont = new CVmlFont( strFontName,
                                      strTtfName, 
                                      strFontWeight, 
                                      strFontStyle );

   // Look, if font already exists in the font list
   // If so, do nothing here
   for ( int i = 0; i < m_arrVmlFonts.GetSize(); ++i )
   {
      if ( pVmlFont->Equals( m_arrVmlFonts[i] ) )
      {
         delete pVmlFont;
         pVmlFont = NULL;

         return hr;
      }
   }

   // Add the font to the font list
   m_arrVmlFonts.Add( pVmlFont );

//   // Clear // Don't do this here!
//   delete pVmlFont;
//   pVmlFont = NULL;

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::WriteFontEmbeddingCssFile()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   CString strDestination;
   CString strText;
   strDestination.Empty();
   strText.Empty();

   // Extract the destination path from the output file name (without '\')
   strDestination.Format( "%s", m_strOutputFilename );
   StringManipulation::GetPath( strDestination );
   // Append '\' and CSS file name
   strDestination.Insert( strDestination.GetLength(), "\\vmlfonts.css" );


   //Write the css file
   FILE* fp;

   if ( (fp = fopen( strDestination, "wt" )) == NULL )
   {
      printf( "VML error: unable to create file %s ! \n", strDestination );
      return ERROR_OPEN_FAILED;
   }

   for ( int i = 0; i < m_arrVmlFonts.GetSize(); ++i )
   {
      strText.Format( "@font-face {" );
      WriteTextLineIntoStream( strText, fp );
      strText.Format( "  font-family: %s;", m_arrVmlFonts[i]->GetFontName() );
      WriteTextLineIntoStream( strText, fp );
      strText.Format( "  font-style: %s;", m_arrVmlFonts[i]->GetFontStyle() );
      WriteTextLineIntoStream( strText, fp );
      strText.Format( "  font-weight: %s;", m_arrVmlFonts[i]->GetFontWeight() );
      WriteTextLineIntoStream( strText, fp );
      strText.Format( "  src: url(%s);", m_arrVmlFonts[i]->GetTtfName() );
      WriteTextLineIntoStream( strText, fp );
      strText.Format( "}" );
      WriteTextLineIntoStream( strText, fp );
   }

   if ( fclose( fp ) )
   {
      printf( "VML error: unable to close file %s ! \n", strDestination );
      return ERROR_FILE_CORRUPT;
   }
   fp = NULL;

   StringManipulation::GetFilename( strDestination );
   printf( "VML: %s written (%d fonts). \n", strDestination, m_arrVmlFonts.GetSize() );


   strDestination.Empty();
   strText.Empty();

   return hr;
}

//////////////////////////////////////////////////////////////////////

int CVmlSdk::WriteEventsIntoControlFile()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   CString strSource;
   CString strText;
   CString strItem;
   strSource.Empty();
   strText.Empty();
   strItem.Empty();

   // Extract the destination path from the output file name (without '\')
   strSource.Format( "%s", m_strOutputFilename );
   StringManipulation::GetPath( strSource );
   // Append '\' and JavaScript file name
   strSource.Insert( strSource.GetLength(), "\\control_vml.js" );


   // Create and get an array with the event list
   CArray<CWhiteboardEvent*, CWhiteboardEvent*> eventList;
   GetReducedEventArrayList( eventList );
   // Debug Info
   printf( "VML: %d events used for JavaScript.\n", eventList.GetSize() );


   // Open the JavaScript file to append new lines
   FILE* fp;

   if ( (fp = fopen( strSource, "a+t" )) == NULL )
   {
      printf( "VML error: unable to open file %s ! \n", strSource );
      return ERROR_OPEN_FAILED;
   }


   strText.Format( "" );
   WriteTextLineIntoStream( strText, fp );
   strText.Format( "var numOfVmlObjects = %d;", m_arrObjects.GetSize() );
   WriteTextLineIntoStream( strText, fp );
   strText.Format( "" );
   WriteTextLineIntoStream( strText, fp );
   strText.Format( "/* ------------------------------------------------------------------------" );
   WriteTextLineIntoStream( strText, fp );
   strText.Format( " * Create instances of the \"vmlEntry\" object" );
   WriteTextLineIntoStream( strText, fp );
   strText.Format( " * ------------------------------------------------------------------------ */" );
   WriteTextLineIntoStream( strText, fp );
   strText.Format( "function initVmlEvents()" );
   WriteTextLineIntoStream( strText, fp );
   strText.Format( "{" );
   WriteTextLineIntoStream( strText, fp );

   // Write event list
   for ( int i = 0; i < eventList.GetSize(); ++i )
   {
      int timestamp = eventList.ElementAt( i )->GetTimestamp();
      int objectCount = eventList.ElementAt( i )->GetObjectsPointer()->GetSize();
      int textLength = 0;

      strText.Format( "  vmlEntries[%d] = new vmlEntry(%d, %d", i, timestamp, objectCount );

      DrawSdk::DrawObject* pDrawObject = NULL;

      // Get objects from the actual event
      for ( int k = 0; k < objectCount; ++k )
      {
         pDrawObject = (DrawSdk::DrawObject*)eventList.ElementAt( i )->GetObjectsPointer()->ElementAt( k );
         int objectNumber = pDrawObject->GetObjectNumber();
         // Write object number, ignore Telepointer objects
         if ( pDrawObject->GetType() != DrawSdk::TELEPOINTER )
         {
            strItem.Format( ", %d", objectNumber );
            textLength = strText.GetLength();
            strText.Insert( textLength, strItem );
         }
      }

      textLength = strText.GetLength();
      strText.Insert( textLength, ");" );

      WriteTextLineIntoStream( strText, fp );
      //printf( "- VML Event %d written.\n", i );
   }

   strText.Format( "}" );
   WriteTextLineIntoStream( strText, fp );
   strText.Format( "" );
   WriteTextLineIntoStream( strText, fp );
   

   if ( fclose( fp ) )
   {
      printf( "VML error: unable to close file %s ! \n", strSource );
      return ERROR_FILE_CORRUPT;
   }
   fp = NULL;

   strSource.Empty();
   strText.Empty();
   strItem.Empty();

   return hr;
}

//////////////////////////////////////////////////////////////////////

/**
 * Method returns length of the reduced array list
 */
int CVmlSdk::GetReducedEventArrayList(CArray<CWhiteboardEvent*, CWhiteboardEvent*> &eventList)
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   eventList.RemoveAll();

   // Create a pointer to an CWhiteboardEvent object
   CWhiteboardEvent* pEvent = NULL;
   CWhiteboardEvent* pEventBefore = NULL;


   if ( (m_lEventTimestamps != NULL) && (m_nEventsUsed > 0) )
   {
      // A reduced event list has already been created (by the StreamingMediaConverter)

      for ( int i = 0; i < m_nEventsUsed; ++i )
      {
         pEvent = m_pWhiteboardStream->GetEvent( m_lEventTimestamps[i] );

         // Add event to the event list
         eventList.Add( pEvent );

         // Upgrade list of used objects
         UpgradeArrayOfUsedObjects( pEvent );
      }
   }
   else
   {
      // No timestamps of a reduced event list --> time consuming search

      int nDuration = m_pWhiteboardStream->GetLength();

      for (int timestamp = 0; timestamp <= nDuration; timestamp+=10 )
      {
         pEvent = m_pWhiteboardStream->GetEvent( timestamp );

         // Discard double events
         if ( pEvent == NULL || pEvent == pEventBefore )
            continue;

         // Special case: page begin timestamp
         bool bIsPageBegin = false;
         if ( pEventBefore == NULL )
         {
            bIsPageBegin = true;
         }
         else 
         {
            if ( pEvent->GetPageNumber() != pEventBefore->GetPageNumber() )
               bIsPageBegin = true;
         }


         // Count the number of objects displayed in an event
         int nNumOfObjects = pEvent->GetObjectsPointer()->GetSize();
         int nNumOfObjectsBefore = ( pEventBefore != NULL ) ? 
                                   pEventBefore->GetObjectsPointer()->GetSize() : 0;

         // Look for 'TELEPOINTER' object
         int nTelepointerIndex = GetTelepointerIndexFromEvent( pEvent );
         int nTelepointerIndexBefore = GetTelepointerIndexFromEvent( pEventBefore );
         bool bEventHasTelepointer = ( nTelepointerIndex > -1 ) ? 
                                     true : false;
         bool bEventBeforeHasTelepointer = ( nTelepointerIndexBefore > -1 ) ? 
                                           true : false;


         // Ignore events with Telepointer --> do nothing here and continue
         // Exceptions: - page begin
         //             - number of objects (without Telepointer) has changed
         bool bObjectNumberHasChanged = false;
         int reducedNumOfObjects = ( bEventHasTelepointer ) ? 
                                   nNumOfObjects-1 : nNumOfObjects; 
         int reducedNumOfObjectsBefore = ( bEventBeforeHasTelepointer ) ? 
                                         nNumOfObjectsBefore-1 : nNumOfObjectsBefore; 

         if ( reducedNumOfObjects != reducedNumOfObjectsBefore )
            bObjectNumberHasChanged = true;
         else
            bObjectNumberHasChanged = false;

         if ( bEventHasTelepointer && !bIsPageBegin && !bObjectNumberHasChanged )
         {
            // ignore event --> do nothing here
         }
         else
         {
            // Add event to the event list
            eventList.Add( pEvent );
            // Debug Info
            printf( "VML: GetReducedEventArrayList() - Event %d added (at %d ms from %d ms).\n", eventList.GetSize(), timestamp, nDuration );

            // Upgrade list of used objects
            UpgradeArrayOfUsedObjects( pEvent );
         }
         // Remember last event
         pEventBefore = pEvent;
      }
   }

   // Clear
   pEvent = NULL;
   pEventBefore = NULL;

   return eventList.GetSize();
}

//////////////////////////////////////////////////////////////////////

/**
 * Method returns length of the list of used objects
 */
 int CVmlSdk::UpgradeArrayOfUsedObjects( CWhiteboardEvent* pEvent )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   DrawSdk::DrawObject* pDrawObject = NULL;
   int objectCount = pEvent->GetObjectsPointer()->GetSize();

   for ( int i = 0; i < objectCount; ++i )
   {
      pDrawObject = (DrawSdk::DrawObject*)pEvent->GetObjectsPointer()->ElementAt( i );
      int objectNumber = pDrawObject->GetObjectNumber();

      bool isAlreadyIncluded = false;

      for ( int j = 0; j < m_arrUsedObjects.GetSize(); ++j )
      {
         if ( objectNumber == m_arrUsedObjects.ElementAt( j ) )
         {
            isAlreadyIncluded = true;
            break;
         }
      }

      if ( !isAlreadyIncluded )
      {
         m_arrUsedObjects.Add( objectNumber );
      }
   }

   pDrawObject = NULL;

   return m_arrUsedObjects.GetSize();
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Methods (public)
//////////////////////////////////////////////////////////////////////

int CVmlSdk::StartParsing()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   m_pWhiteboardStream->SetObjFilename( m_strObjectFilename );
   m_pWhiteboardStream->SetEvqFilename( m_strEventFilename );
   //m_pWhiteboardStream->SetPageDimension( [LPCSTR] );  // is this used anywhere?

   // Open and parse event & object queue
   m_pWhiteboardStream->Open();


   // Get 'CPage' objects
   int nLength = m_pWhiteboardStream->GetLength();
   int nFirst = 0;
   int nLast  = 0;

   m_pWhiteboardStream->GetPages( m_arrPages, 0, nLength, 0, nFirst, nLast );
   printf( "VML: %d Pages found.\n", m_arrPages.GetSize() );

   // Get draw objects
   m_pWhiteboardStream->GetObjectArray( m_arrObjects );
   printf( "VML: %d Objects found.\n", m_arrObjects.GetSize() );

//   // Debug info
//   for ( int i = 0; i < m_arrObjects.GetSize(); ++i )
//   {
//      printf( " VML: Object %d:  type=%d x=%g y=%g w=%g h=%g\n", 
//              m_arrObjects.ElementAt(i)->GetObjectNumber(), 
//              m_arrObjects.ElementAt(i)->GetType(), 
//              m_arrObjects.ElementAt(i)->GetX(), 
//              m_arrObjects.ElementAt(i)->GetY(), 
//              m_arrObjects.ElementAt(i)->GetWidth(), 
//              m_arrObjects.ElementAt(i)->GetHeight() );
//   }


   // Write the event queue into the 'control_vml.js' file
   hr = WriteEventsIntoControlFile();
   if ( hr != S_OK)
   {
      printf( "VML error: Unable to write events into JavaScript file!\n" );
      return hr;
   }
   
   // Create VML page
   hr = CreateVmlPage();
   if ( hr != S_OK)
   {
      printf( "VML error: Unable to create VML page(s)!\n" );
      return hr;
   }

   // Create font embedding css file
   hr = WriteFontEmbeddingCssFile();
   if ( hr != S_OK)
   {
      printf( "VML error: Unable to create font embedding css file!\n" );
      return hr;
   }
   
   return hr;
}

//////////////////////////////////////////////////////////////////////
// Get/Set methods (public)
//////////////////////////////////////////////////////////////////////

CString CVmlSdk::GetEventFileName()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   return m_strEventFilename;
}

//////////////////////////////////////////////////////////////////////

CString CVmlSdk::GetObjectFileName()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   return m_strObjectFilename;
}

//////////////////////////////////////////////////////////////////////

CString CVmlSdk::GetOutputFileName()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   return m_strOutputFilename;
}

//////////////////////////////////////////////////////////////////////

void CVmlSdk::SetEventFilename( CString eventFilename )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   m_strEventFilename.Empty();
   m_strEventFilename.Insert( 0, eventFilename );

   //printf( "VML Event Queue Filename: %s\n", m_strEventFilename );

   return;
}

//////////////////////////////////////////////////////////////////////

void CVmlSdk::SetObjectFilename( CString objectFilename )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   m_strObjectFilename.Empty();
   m_strObjectFilename.Insert( 0, objectFilename );

   //printf( "VML Object Queue Filename: %s\n", m_strObjectFilename );

   return;
}

//////////////////////////////////////////////////////////////////////

void CVmlSdk::SetOutputFilename( CString outputFilename )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   m_strOutputFilename.Empty();
   m_strOutputFilename.Insert( 0, outputFilename );

   //printf( "VML Output Filename: %s\n", m_strOutputFilename );

   return;
}

//////////////////////////////////////////////////////////////////////

void CVmlSdk::SetSlideDimension( int slideWidth, int slideHeight )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   m_nSlideWidth = slideWidth;
   m_nSlideHeight = slideHeight;

   //printf( "VML Slide Dimension: %d x %d \n", m_nSlideWidth, m_nSlideHeight );

   return;
}

//////////////////////////////////////////////////////////////////////

void CVmlSdk::SetEventTimestamps( long* eventTimestamps, int etLength )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   m_nEventsUsed = etLength;

   printf( "VML Events: %d events used \n", etLength );

   if ( m_nEventsUsed > 0 )
      m_lEventTimestamps = new long[etLength];
   else
      return;

   for ( int i = 0; i < m_nEventsUsed; ++i )
   {
      m_lEventTimestamps[i] = eventTimestamps[i];

      //printf( "VML Event[%d] at %d ms \n", i, m_lEventTimestamps[i] );
   }

   return;
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Global methods
//////////////////////////////////////////////////////////////////////

int parseAndWriteVml( CString strEventFilename, 
                      CString strObjectFilename,
                      CString strOutputFilename, 
                      long* eventTimestamps, 
                      int etLength, 
                      int slideWidth, 
                      int slideHeight )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   CVmlSdk* vmlSdk = new CVmlSdk();

   vmlSdk->SetEventFilename( strEventFilename );
   vmlSdk->SetObjectFilename( strObjectFilename );
   vmlSdk->SetOutputFilename( strOutputFilename );
   vmlSdk->SetEventTimestamps( eventTimestamps, 
                               etLength );
   vmlSdk->SetSlideDimension( slideWidth, slideHeight );

   hr = vmlSdk->StartParsing();

   return hr;
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// main
//////////////////////////////////////////////////////////////////////

void main( int argc, char** argv )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   if ( argc < 6 )
   {
      printf( "\n" );
      printf( "usage: vmlsdk <eventFilename> <objectFilename> <outputFilename> <slideWidth> <slideHeight>\n" );
      printf( "(Filenames without suffixes!)\n" );
      printf( "\n" );
      return;
   }

   CString eventFilename( argv[1] );
   CString objectFilename( argv[2] );
   CString outputFilename( argv[3] );
   int slideWidth = atoi( argv[4] );
   int slideHeight = atoi( argv[5] );
   long* eventTimestamps = NULL;
   int etLength = 0;

   HRESULT hr = parseAndWriteVml( eventFilename, 
                                  objectFilename, 
                                  outputFilename, 
                                  eventTimestamps, 
                                  etLength, 
                                  slideWidth, 
                                  slideHeight );

   if (hr != S_OK)
   {
      printf( "vmlsdk failed with error code 0x%08x !\n", hr );
   }

   eventFilename.Empty();
   objectFilename.Empty();
   outputFilename.Empty();

   return;
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// JNI interface
//////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_imc_lecturnity_converter_StreamingMediaConverter_writeVmlFiles( JNIEnv*    env, 
                                                                                            jobject    obj_this, 
                                                                                            jstring    eventFilename, 
                                                                                            jstring    objectFilename, 
                                                                                            jstring    outputFilename, 
                                                                                            jint       slideWidth, 
                                                                                            jint       slideHeight, 
                                                                                            jlongArray eventTimestamps ) 
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int n;
   
   jboolean isCopy;
   const jchar* c_strWide;
   char* c_strACP = NULL;
   

   // Get the event queue file name
   c_strWide = env->GetStringChars( eventFilename, &isCopy );
   
   n = WideCharToMultiByte( CP_ACP, 0, c_strWide, -1, c_strACP, 0, NULL, NULL );
   c_strACP = (char*)malloc( n * 2 + 1 );
   WideCharToMultiByte( CP_ACP, 0, c_strWide, -1, c_strACP, n, NULL, NULL );
   
   CString strEventFilename( c_strACP );

   if ( isCopy == JNI_TRUE )
   {
      env->ReleaseStringChars( eventFilename, c_strWide );
   }
   env->DeleteLocalRef( eventFilename );
   free( c_strACP );


   // Get the object queue file name
   c_strWide = env->GetStringChars( objectFilename, &isCopy );
   
   n = WideCharToMultiByte( CP_ACP, 0, c_strWide, -1, c_strACP, 0, NULL, NULL );
   c_strACP = (char*)malloc( n * 2 + 1 );
   WideCharToMultiByte( CP_ACP, 0, c_strWide, -1, c_strACP, n, NULL, NULL );
   
   CString strObjectFilename( c_strACP );

   if ( isCopy == JNI_TRUE )
   {
      env->ReleaseStringChars( objectFilename, c_strWide );
   }
   env->DeleteLocalRef( objectFilename );
   free( c_strACP );


   // Get the output file name
   c_strWide = env->GetStringChars( outputFilename, &isCopy );
   
   n = WideCharToMultiByte( CP_ACP, 0, c_strWide, -1, c_strACP, 0, NULL, NULL );
   c_strACP = (char*)malloc( n * 2 + 1 );
   WideCharToMultiByte( CP_ACP, 0, c_strWide, -1, c_strACP, n, NULL, NULL );
   
   CString strOutputFilename( c_strACP );

   if ( isCopy == JNI_TRUE )
   {
      env->ReleaseStringChars( outputFilename, c_strWide );
   }
   env->DeleteLocalRef( outputFilename );
   free( c_strACP );


   // Get the slide dimensions
   int nSlideWidth  = (int)( slideWidth );
   int nSlideHeight = (int)( slideHeight );


   // Get the event timestamps
   int sz = env->GetArrayLength( eventTimestamps );
   long* lEventTimestamps = new long[sz];
   jlong* longArrayElements = env->GetLongArrayElements( eventTimestamps, &isCopy );
   
   for ( int i = 0; i < sz; ++i )
   {
      lEventTimestamps[i] = (long)longArrayElements[i];
   }
   
   if (isCopy == JNI_TRUE)
      env->ReleaseLongArrayElements( eventTimestamps, longArrayElements, 0);


   // Start the parsing & writing process
   hr = parseAndWriteVml( strEventFilename, 
                          strObjectFilename, 
                          strOutputFilename, 
                          lEventTimestamps, 
                          sz, 
                          nSlideWidth, 
                          nSlideHeight );

   strEventFilename.Empty();
   strObjectFilename.Empty();
   strOutputFilename.Empty();
   delete[] lEventTimestamps;
   lEventTimestamps = NULL;

   if (hr != S_OK)
   {
      return( (jint)imc_lecturnity_converter_StreamingMediaConverter_VML_CREATION_FAILED );
   }

   return( (jint)imc_lecturnity_converter_StreamingMediaConverter_SUCCESS );
}

//////////////////////////////////////////////////////////////////////

///      CString tmp;
///      tmp.Format(IDS_TEST, "Dummy");
///      MessageBox(GetForegroundWindow(), tmp, NULL, MB_OK);

//////////////////////////////////////////////////////////////////////
/*
int CVmlSdk::CreateVmlPages()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int pageIndex;
   int nNumOfPages = m_arrPages.GetSize();

   CString vmlFilename;
   CString vmlTemplate;

   vmlFilename.Empty();
   vmlTemplate.Empty();

   for ( pageIndex = 0; pageIndex < nNumOfPages; ++pageIndex )
   {
      // Reset Object index
      m_nObjectIndex = 0;

      // Create and open HTML file
      // vmlFilename is something like "Demo_vml_0.html"
      vmlFilename.Format( "%s_vml_%d.html", m_strOutputFilename, pageIndex );
      if ( (m_fp = fopen( vmlFilename, "wt" )) == NULL )
      {
         printf( "VML error: unable to create file %s ! \n", vmlFilename );
         return ERROR_OPEN_FAILED;
      }
      

      // Write VML Header
      vmlTemplate.Format( "Streaming Templates\\vml_1.tmpl" );
      hr = WriteTemplateFileIntoStream( vmlTemplate );
      if ( hr != S_OK )
      {
         printf( "VML error: unable to read template file %s ! \n", vmlTemplate );
         return hr;
      }


      // Write a line withe the <title> tag
      CString strSlide;
      strSlide.Format( IDS_SLIDE );
      //strSlide.Format( m_arrPages.ElementAt(pageIndex)->GetTitle() );
      //strSlide.Format( "(%d, %d)", m_arrPages.ElementAt(pageIndex)->GetBegin(), m_arrPages.ElementAt(pageIndex)->GetEnd() );
      vmlTemplate.Format( "    <title>VML: %s %d</title>\n", strSlide, pageIndex );
      WriteTextLineIntoStream( vmlTemplate );
      strSlide.Empty();


      // Write VML middle part
      vmlTemplate.Format( "Streaming Templates\\vml_2.tmpl" );
      hr = WriteTemplateFileIntoStream( vmlTemplate );
      if ( hr != S_OK )
      {
         printf( "VML error: unable to read template file %s ! \n", vmlTemplate );
         return hr;
      }

      
      // Write VML content
      hr = WriteVmlPageContentIntoStream( pageIndex );
      if ( hr != S_OK )
      {
         printf( "VML error: unable to handle VML content ! \n" );
         return hr;
      }


      // Write VML footer
      vmlTemplate.Format( "Streaming Templates\\vml_3.tmpl" );
      hr = WriteTemplateFileIntoStream( vmlTemplate );
      if ( hr != S_OK )
      {
         printf( "VML error: unable to read template file %s ! \n", vmlTemplate );
         return hr;
      }


      // Close HTML file
      if ( fclose( m_fp ) )
      {
         printf( "VML error: unable to close file %s ! \n", vmlFilename );
         return ERROR_FILE_CORRUPT;
      }
      m_fp = NULL;


      printf( "VML: %s written. \n", vmlFilename );
   }

   vmlTemplate.Empty();
   vmlFilename.Empty();

   return hr;
}
*/
//////////////////////////////////////////////////////////////////////
/*
int CVmlSdk::WriteVmlPageContentIntoStream( int pageIndex )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   m_strText.Empty();

   // Write the wrapping <group> tag
   m_strText.Format( "    <!-- Grouping all the VML content -->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "    <v:group id=\"content01\" " );
   WriteTextLineIntoStream( m_strText );
   // Consider 2 extra pixels in the slide dimension for the shadow 
   m_strText.Format( "      style=\'position:absolute;top:0px;left:0px;width:%dpx;height:%dpx;", 
                   m_nSlideWidth+2, m_nSlideHeight+2 );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        visibility:visible;background-color:none;" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "        font-size:%dpx\'", 
                     m_nDefaultFontSize);
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      coordsize=\"%d,%d\" coordorigin=\"0,0\">", 
                   m_nSlideWidth+2, m_nSlideHeight+2 );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "      <v:stroke on=\"true\" weight=\"1px\" />" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "" );
   WriteTextLineIntoStream( m_strText );


   // Write the shadow object
   m_strText.Format( "      <!-- Shadow -->" );
   WriteTextLineIntoStream( m_strText );

   COLORREF outlineCol = RGB( 104, 104, 104 ); //( 0x00686868 );
   COLORREF fillCol = RGB( 104, 104, 104 ); //( 0x00686868 );
   WriteVmlRectangle( 2, 2,
                      m_nSlideWidth-1, m_nSlideHeight-1, 
                      outlineCol, 
                      fillCol,
                      1, 0 );


   // Write the page objects
   hr = WriteVmlPageObjectsIntoStream( pageIndex );
   if ( hr != S_OK )
   {
      printf( "VML error: unable to write slide content!" );
      return hr;
   }


   // Write the wrapping </group> tag
   m_strText.Empty();
   m_strText.Format( "    <!-- End of grouping all VML content-->" );
   WriteTextLineIntoStream( m_strText );
   m_strText.Format( "    </v:group>" );
   WriteTextLineIntoStream( m_strText );

   m_strText.Empty();

   return hr;
}
*/
//////////////////////////////////////////////////////////////////////
/*
int CVmlSdk::WriteVmlPageObjectsIntoStream( int pageIndex )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   HRESULT hr = S_OK;

   int timestamp = 0;
   int nPageBegin = m_arrPages.ElementAt(pageIndex)->GetBegin();
   int nPageEnd   = m_arrPages.ElementAt(pageIndex)->GetEnd(); 
   printf( "VML: Slide%d : from %d msec to %d msec. \n", pageIndex, nPageBegin, nPageEnd );

   // Create a pointer to an CWhiteboardEvent object
   CWhiteboardEvent* pEvent = NULL;
   CWhiteboardEvent* pEventBefore = NULL;

   for ( timestamp = nPageBegin; timestamp < nPageEnd; ++timestamp)
   {
      pEvent = m_pWhiteboardStream->GetEvent( timestamp );

      if ( pEvent == NULL || pEvent == pEventBefore )
         continue;

//      // Debug Info
//      printf( " VML event at %d msec (%d Objects: %d %d %d...) \n", 
//              timestamp, pEvent->GetObjectCount(), 
//              ((DrawSdk::DrawObject*)pEvent->GetObjectsPointer()->ElementAt(0))->GetObjectNumber(), 
//              ((DrawSdk::DrawObject*)pEvent->GetObjectsPointer()->ElementAt(1))->GetObjectNumber(), 
//              ((DrawSdk::DrawObject*)pEvent->GetObjectsPointer()->ElementAt(2))->GetObjectNumber() );

      // Special case: page begin timestamp
      bool bIsPageBegin = ( timestamp == nPageBegin ) ? true : false;

      hr = WriteVmlEventObjectsIntoStream( pEvent, pEventBefore, bIsPageBegin );


      pEventBefore = pEvent;
   }

   return hr;
}
*/
//////////////////////////////////////////////////////////////////////
/*
int CVmlSdk::WriteVmlEventObjectsIntoStream( CWhiteboardEvent* pEvent, 
                                             CWhiteboardEvent* pEventBefore, 
                                             bool bIsPageBegin )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   DrawSdk::DrawObject* pDrawObject = NULL;

   HRESULT hr = S_OK;

   int nNumOfObjects = pEvent->GetObjectsPointer()->GetSize();
   int nNumOfObjectsBefore = ( pEventBefore != NULL ) ? 
                             pEventBefore->GetObjectsPointer()->GetSize() : 0;

   // Look for 'TELEPOINTER' object
   int nTelepointerIndex = GetTelepointerIndexFromEvent( pEvent );
   int nTelepointerIndexBefore = GetTelepointerIndexFromEvent( pEventBefore );
   bool bEventHasTelepointer = ( nTelepointerIndex > -1 ) ? 
                               true : false;
   bool bEventBeforeHasTelepointer = ( nTelepointerIndexBefore > -1 ) ? 
                                     true : false;


   // Ignore events with Telepointer --> do nothing here and return
   // Exceptions: - page begin
   //             - number of objects (without Telepointer) has changed
   bool bObjectNumberHasChanged = false;
   int reducedNumOfObjects = ( bEventHasTelepointer ) ? 
                             nNumOfObjects-1 : nNumOfObjects; 
   int reducedNumOfObjectsBefore = ( bEventBeforeHasTelepointer ) ? 
                                   nNumOfObjectsBefore-1 : nNumOfObjectsBefore; 
   if ( reducedNumOfObjects != reducedNumOfObjectsBefore )
      bObjectNumberHasChanged = true;
   else
      bObjectNumberHasChanged = false;

   if ( bEventHasTelepointer && !bIsPageBegin && !bObjectNumberHasChanged )
      return hr;


   // Debug Info
   printf( " VML event at %d msec (%d Objects: %d %d %d...) \n", 
           pEvent->GetTimestamp(), 
           pEvent->GetObjectCount(), 
           ((DrawSdk::DrawObject*)pEvent->GetObjectsPointer()->ElementAt(0))->GetObjectNumber(), 
           ((DrawSdk::DrawObject*)pEvent->GetObjectsPointer()->ElementAt(1))->GetObjectNumber(), 
           ((DrawSdk::DrawObject*)pEvent->GetObjectsPointer()->ElementAt(2))->GetObjectNumber() );


   // Get the new objects of this event, which do not appear in the event before
   
   return hr;
}
*/
//////////////////////////////////////////////////////////////////////


/*********************************************************************

  program : mlb_image.cpp
  authors : Gabriela Maass
  date    : 04.04.2000
  desc    : Functions to read, draw and manipulate images
  
**********************************************************************/
#include "StdAfx.h"

#include "DrawObjects.h"

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/


ASSISTANT::Image::Image(double _x, double _y, double _width, double _height, int _zPosition, LPCTSTR _filename, UINT _id,
                        LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
: DrawObject(_x, _y, _width, _height, _zPosition, _id, _hyperlink, _currentDirectory, _linkedObjects)
{ 
   /* cut path, change suffix */
   srcPath_ = _filename;
   
   filename_ = _filename;
   int iSlashPos = filename_.ReverseFind(_T('\\'));
   
   if (iSlashPos != -1) 
   {
      ++iSlashPos;
      imagePath_ = filename_.Left(iSlashPos);
      filename_.Delete(0, iSlashPos);
   }
   
   // convert char-String in WCHAR-String  
   int iStringLength = _tcslen(_filename) + 1;
   WCHAR *wszFilename = new WCHAR[iStringLength];
#ifdef _UNICODE
   wcscpy(wszFilename, _filename);
#else
   MultiByteToWideChar( CP_ACP, 0, _filename, iStringLength, 
                        wszFilename, iStringLength);
#endif

   // Read image
   gdipImage_ = new Gdiplus::Image(wszFilename, FALSE);

   if (wszFilename)
      delete[] wszFilename;

   
}

ASSISTANT::Image::~Image()
{
   filename_.Empty();
   srcPath_.Empty();

   if (gdipImage_)
      delete gdipImage_;
}

bool ASSISTANT::Image::IsVisible()
{
   if (visible) 
      return true; 
   else
      return false;
}

void ASSISTANT::Image::Update(float _zoomFactor)
{
}

ASSISTANT::DrawObject *ASSISTANT::Image::Copy(bool keepZPosition)
{
   Image *ret = new Image(m_dX, m_dY, m_dWidth, m_dHeight, -1, filename_, 0, hyperlink_, currentDirectory_, m_ssLinkedObjects);
   
   if (isInternLink)
      ret->LinkIsIntern(true);
   else
      ret->LinkIsIntern(false);
   
   return (DrawObject *)ret;
}

void ASSISTANT::Image::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{ 
   if (!visible)
      return;
   
}

void ASSISTANT::Image::GetBBox(int *minX, int *minY, int *maxX, int *maxY)
{  
   if ((m_dX+m_dWidth) < m_dX) {
      *minX = (int)(m_dX + m_dWidth - 3);
      *maxX = (int)(m_dX + 3);
   }
   else {
      *minX = (int)(m_dX - 3);
      *maxX = (int)(m_dX + m_dWidth + 3);
   }
   if ((m_dY+m_dHeight) < m_dY) {
      *minY = (int)(m_dY + m_dHeight - 3);
      *maxY = (int)(m_dY + 3);
   }
   else {
      *minY = (int)(m_dY - 3);
      *maxY = (int)(m_dY + m_dHeight + 3);
   }
   
}

void ASSISTANT::Image::GetBoundingBox(CRect &rcObject)
{
   rcObject.left = (int)m_dX;
   rcObject.right = (int)(m_dX + m_dWidth);
   rcObject.top = (int)m_dY;
   rcObject.bottom = (int)(m_dY + m_dHeight);
   
   rcObject.NormalizeRect();
}


bool ASSISTANT::Image::IsIdentic(DrawObject *obj)
{
   // the same object is identic
   if (obj == this)
      return true;

   if (!DrawObject::IsIdentic(obj))
      return false;

   // PowerPoint creates an images for every animated object
   //if (image_ != ((WindowsImage *)obj)->GetImage())
   //   return false;
   // all parameters are identic
   return true;
}

/**** Static Functions ***/

ASSISTANT::DrawObject *ASSISTANT::Image::Load(SGMLElement *hilf, CString &imagePath, CStringArray &arLsdTmpFileNames) 
{
   Image
      *obj;
   TCHAR
      *tmp;
   int
      x, y,
      width, height,
		zPosition,
      iPPTId;
   TCHAR
      *filename,
      *hyperlink,
      *internLink,
      *currentDirectory,
      *linkedObjects;
   
   CString
      filePath;
   
   if (!hilf)
      return NULL;
   
   tmp = hilf->GetAttribute("x");
   if (tmp) x = _ttoi(tmp);
   else     x = 0;
   
   tmp = hilf->GetAttribute("y");
   if (tmp) y = _ttoi(tmp);
   else     y = 0;
   
   tmp = hilf->GetAttribute("width");
   if (tmp) width = _ttoi(tmp);
   else     width = 0;
   
   tmp = hilf->GetAttribute("height");
   if (tmp) height = _ttoi(tmp);
   else     height = 0;
   
   tmp = hilf->GetAttribute("filename");
   if (tmp) filename = _tcsdup(tmp);
   else     filename = _tcsdup(_T(""));
   
   tmp = hilf->GetAttribute("address");
   if (tmp) hyperlink = _tcsdup(tmp);
   else     hyperlink = NULL;
   
   tmp = hilf->GetAttribute("intern");
   if (tmp) internLink = _tcsdup(tmp);
   else     internLink = NULL;
   
   tmp = hilf->GetAttribute("path");
   if (tmp) currentDirectory = _tcsdup(tmp);
   else     currentDirectory = NULL;
   
   tmp = hilf->GetAttribute("linkedObjects");
   if (tmp) linkedObjects = _tcsdup(tmp);
   else     linkedObjects = NULL;

   tmp = hilf->GetAttribute("id");
   if (tmp) iPPTId = _ttoi(tmp);
   else     iPPTId = -1;
   
   
   tmp = hilf->GetAttribute("ZPosition");
   if (tmp) zPosition = _ttoi(tmp);
   else     zPosition = -1;

   filePath = imagePath;
   filePath += _T("\\");
   filePath += filename;
   
   arLsdTmpFileNames.Add(filePath);

   if (hyperlink != NULL)
   {
      obj = new ASSISTANT::Image(x, y, width, height, zPosition, filePath,
         0, hyperlink, currentDirectory, linkedObjects);
   
      obj->LinkIsIntern(false);
   }
   else
   {
     obj = new ASSISTANT::Image(x, y, width, height, zPosition, filePath,
        0, internLink, currentDirectory, linkedObjects);
   
      obj->LinkIsIntern(true);
   }
   
   if (obj && iPPTId > 0)
      obj->SetPPTObjectId(iPPTId);

   filePath.Empty();
   
   delete filename;
   
   if (hyperlink)
      delete hyperlink;

   if (internLink)
      delete internLink;

   if (currentDirectory)
      delete currentDirectory;
   
   if (linkedObjects)
      delete linkedObjects;
   
   return obj;
}

int ASSISTANT::Image::WriteScaledImageFile(WCHAR* scaledImageName, float scaleX, float scaleY, const WCHAR* format)
{
   int hr = S_OK;
 
//   // Starting the Gdiplus machine should be executed outside of this method
//   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
//   ULONG_PTR gdiplusToken;
//   Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

   Gdiplus::ImageCodecInfo *pImageCodecInfo = NULL;

   CLSID *pClsid = NULL;

   HDC hdc = CreateCompatibleDC(NULL);
   

   // Get the encoder CLSID depending on the image format (image/jpeg, image/gif, image/png)
   UINT num = 0;
   UINT size = 0;

   Gdiplus::GetImageEncodersSize(&num, &size);
   if (size == 0)
      hr = -1;  // Failure

   if (SUCCEEDED(hr))
   {
      pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
      if (pImageCodecInfo == NULL)
         hr = -1;  // Failure
   }

   if (SUCCEEDED(hr))
   {
      Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

      for (UINT j = 0; j < num; ++j)
      {
         if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
         {
            pClsid = &pImageCodecInfo[j].Clsid;
            break;  // Success
         }    
      }

      if (pClsid == NULL)
         hr = -1;  // Failure
   }


   if (SUCCEEDED(hr) && gdipImage_ != NULL)
   {
      // Resize the image by scaling factors
      Gdiplus::REAL destWidth  = (Gdiplus::REAL)(scaleX * this->GetWidth());
      Gdiplus::REAL destHeight = (Gdiplus::REAL)(scaleY * this->GetHeight());

      // If no correct size is defined in the object queue, use the real image size instead
      if ( (this->GetWidth() <= 0.0f) && (this->GetHeight() <= 0.0f) )
      {
         destWidth  = scaleX * gdipImage_->GetWidth();
         destHeight = scaleY * gdipImage_->GetHeight();
      }

      // At least we have to write one pixel
      if (destWidth < 1.0f)
         destWidth = 1.0f;
      if (destHeight < 1.0f)
         destHeight = 1.0f;

      Gdiplus::Bitmap scaledImage(destWidth, destHeight);

      scaledImage.SetResolution(gdipImage_->GetHorizontalResolution(), 
                                gdipImage_->GetVerticalResolution());

      Gdiplus::Graphics grPhoto(&scaledImage);

      grPhoto.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
      grPhoto.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
      grPhoto.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

      Gdiplus::RectF destRect(0.0f, 0.0f, destWidth, destHeight);

      hr = grPhoto.DrawImage(gdipImage_, destRect, 
                             0, 0, gdipImage_->GetWidth(),gdipImage_->GetHeight(), 
                             Gdiplus::UnitPixel);


      // Write the scaled image to a file
      if (SUCCEEDED(hr))
         hr = scaledImage.Save(scaledImageName, pClsid, NULL);
   }


   // Cleanup
   if (hdc)
	{
		::DeleteDC(hdc);
		hdc = NULL;
	}
   if (pImageCodecInfo)
   {
      free(pImageCodecInfo);
      pImageCodecInfo = NULL;
   }

   
//   // Stop the Gdiplus machine --> this should be executed outside of this method (see above)
//   Gdiplus::GdiplusShutdown(gdiplusToken);

   return hr;
}


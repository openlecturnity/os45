#include "StdAfx.h"

#include "objects.h"

DrawSdk::Image::Image() : DrawObject()
{
   Init();
}

DrawSdk::Image::Image(double x, double y, double width, double height, 
                      const _TCHAR *imageName, bool bDndMoveable, int iBelongsKey) : DrawObject(x, y, width, height)
{
   Init();

   imageName_ = (_TCHAR *)malloc((sizeof _TCHAR) * (_tcslen(imageName) + 1));
   _tcsncpy(imageName_, imageName, _tcslen(imageName));
   imageName_[_tcslen(imageName)] = '\0';
   
   imagePrefix_ = (_TCHAR *)malloc((sizeof _TCHAR) * (_tcslen(imageName_) + 1));
   _TCHAR *pos = _tcsrchr(imageName_, _T('\\'));
   if (pos)
   {
      ++pos;
      _tcscpy(imagePrefix_, pos);
   }
   else
      _tcscpy(imagePrefix_, imageName_);

   // convert char-String in WCHAR-String  
   int iStringLength = _tcslen(imageName) + 1;
   WCHAR *wcFilename = (WCHAR *)malloc(iStringLength*sizeof(WCHAR));
#ifdef _UNICODE
   wcscpy(wcFilename, imageName);
#else
   MultiByteToWideChar( CP_ACP, 0, imageName, iStringLength, 
                        wcFilename, iStringLength);
#endif

   // Read image
   gdipImage_ = new Gdiplus::Image(wcFilename, FALSE);

   m_bImageStoredOutside = false;

   m_bDndMoveable = bDndMoveable;
   m_iBelongsKey = iBelongsKey;

   if (wcFilename)
      free(wcFilename);
}

DrawSdk::Image::Image(double x, double y, double width, double height, 
                      const _TCHAR *imageName, Gdiplus::Image *gdipImage, bool bDndMoveable, int iBelongsKey) : DrawObject(x, y, width, height)
{
   Init();

   imageName_ = (_TCHAR *)malloc((sizeof _TCHAR) * (_tcslen(imageName) + 1));
   _tcsncpy(imageName_, imageName, _tcslen(imageName));
   imageName_[_tcslen(imageName)] = '\0';
   
   imagePrefix_ = (_TCHAR *)malloc((sizeof _TCHAR) * (_tcslen(imageName_) + 1));
   _TCHAR *pos = _tcsrchr(imageName_, _T('\\'));
   if (pos)
   {
      ++pos;
      _tcscpy(imagePrefix_, pos);
   }
   else
      _tcscpy(imagePrefix_, imageName_);

   // Set image
   gdipImage_ = gdipImage;
   m_bImageStoredOutside = true;

   m_bDndMoveable = bDndMoveable;
   m_iBelongsKey = iBelongsKey;
}

DrawSdk::Image::~Image()
{
   if (imageName_)
   {
      // it was allocated with malloc so let's be consistent
      free(imageName_);
      imageName_ = NULL;
   }

   if (imagePrefix_)
   {
      free(imagePrefix_);
      imagePrefix_ = NULL;
   }

   if (!m_bImageStoredOutside && gdipImage_)
   {
      delete gdipImage_;
      gdipImage_ = NULL;
   }
}

DrawSdk::DrawObject* DrawSdk::Image::Copy()
{
   Image *pNew = NULL;
   
   // TODO what about creating the image again and again (when m_bImageStoredOutside == false)?
   
   if (m_bImageStoredOutside)
      pNew = new Image(x_, y_, width_, height_, imageName_, gdipImage_, m_bDndMoveable, m_iBelongsKey);
   else
      pNew = new Image(x_, y_, width_, height_, imageName_, m_bDndMoveable, m_iBelongsKey);
   CopyInteractiveFields(pNew);
   return pNew;
}

void DrawSdk::Image::Init()
{
   imageName_ = NULL;
   imagePrefix_ = NULL;
   gdipImage_ = NULL;

   m_bImageStoredOutside = true;
}

void DrawSdk::Image::Draw(HDC hdc, double offX, double offY, ZoomManager *pZm)
{
   Gdiplus::Graphics graphics(hdc);
   graphics.TranslateTransform(offX, offY);
   if (pZm != NULL)
   {
      double zoom = pZm->GetZoomFactor();
      graphics.ScaleTransform(zoom, zoom);
   }
   
   if (gdipImage_)
   {
      Gdiplus::Status status;

      Gdiplus::RectF boundRect;
      Gdiplus::Unit boundUnit;
      gdipImage_->GetBounds(&boundRect, &boundUnit);

      Gdiplus::RectF destRect;
      destRect.X = x_;
      destRect.Y = y_;
      destRect.Width = width_;
      destRect.Height = height_;

     
      graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
      graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
      graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

      status = graphics.DrawImage(gdipImage_, destRect, 
         boundRect.X, boundRect.Y, GetImageWidth(), GetImageHeight(), Gdiplus::UnitPixel);
   }
}


void DrawSdk::Image::SetImagePrefix(LPCTSTR newImageName)
{
   if (imagePrefix_)
      free(imagePrefix_);
   imagePrefix_ = NULL;
   
   imagePrefix_ = (_TCHAR *)malloc((sizeof _TCHAR) * (_tcslen(newImageName) + 1));
   const _TCHAR *pos = _tcsrchr(newImageName, _T('\\'));
   if (pos)
   {
      ++pos;
      _tcscpy(imagePrefix_, pos);
   }
   else
      _tcscpy(imagePrefix_, newImageName);

}

void DrawSdk::Image::Write(CharArray *pArray, bool bUseFullImagePath)
{
   if (pArray == NULL)
      return;

   _TCHAR *tszMove = _T("");
   static _TCHAR tszBelongs[100];
   tszBelongs[0] = 0;
   if (m_bDndMoveable)
   {
      tszMove = _T(" move=\"true\"");
      _stprintf(tszBelongs, _T(" belongs=\"%d\""), m_iBelongsKey);
   }

   static _TCHAR tszVisibility[100];
   tszVisibility[0] = 0;
   if (HasVisibility())
      _stprintf(tszVisibility, _T(" visible=\"%s\""), GetVisibility());

   static _TCHAR tszEscapedPath[MAX_PATH * 2];
   ZeroMemory(tszEscapedPath, MAX_PATH * 2);
   if (bUseFullImagePath)
      DoubleSlashes(imageName_, tszEscapedPath); 
 
   _TCHAR *tszOutputName = bUseFullImagePath ? tszEscapedPath : imagePrefix_;

   static _TCHAR tszTotal[1500];
   tszTotal[0] = 0;
   _stprintf(tszTotal, 
      _T("<IMAGE x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xf=\"%.2f\" yf=\"%.2f\" widthf=\"%.2f\" heightf=\"%.2f\" fname=\"%s\"%s%s%s></IMAGE>\n"),
      (int)x_, (int)y_, (int)width_, (int)height_, 
      x_, y_, width_, height_, 
      tszOutputName, tszMove, tszBelongs, tszVisibility);

   pArray->AppendData(tszTotal);
}

int DrawSdk::Image::GetImageWidth()
{
   int 
      width;

   if (gdipImage_)
      width = gdipImage_->GetWidth();
   else
      width = 0;

   return width;
}

int DrawSdk::Image::GetImageHeight()
{
   int 
      height;

   if (gdipImage_)
      height = gdipImage_->GetHeight();
   else
      height = 0;

   return height;
}

int DrawSdk::Image::WriteScaledImageFile(WCHAR* scaledImageName, float scaleX, float scaleY, const WCHAR* format)
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

      for(int j = 0; j < num; ++j)
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

   // Bugfix 5301: 
   // If 'gdipImage_' is NULL then it was not kept in memory and therefore 
   // a temporary Gdiplus::Image object is created for the rescaling. 
   // This avoids having too much image data in the memory.
   Gdiplus::Image *tmpImage = NULL;
   if (SUCCEEDED(hr) && gdipImage_ == NULL) {
       // convert char-String in WCHAR-String  
       int iStringLength = _tcslen(imageName_) + 1;
       WCHAR *wcFilename = (WCHAR *)malloc(iStringLength*sizeof(WCHAR));
#ifdef _UNICODE
       wcscpy(wcFilename, imageName_);
#else
       MultiByteToWideChar( CP_ACP, 0, imageName, iStringLength, 
           wcFilename, iStringLength);
#endif
       tmpImage = new Gdiplus::Image(wcFilename, FALSE);

       if (wcFilename)
           free(wcFilename);
   } else {
       tmpImage = gdipImage_;
   }


   if (SUCCEEDED(hr) && tmpImage != NULL)
   {
      // Resize the image by scaling factors
      Gdiplus::REAL destWidth  = scaleX * this->GetWidth();
      Gdiplus::REAL destHeight = scaleY * this->GetHeight();

      // If no correct size is defined in the object queue, use the real image size instead
      if ( (this->GetWidth() <= 0.0f) && (this->GetHeight() <= 0.0f) )
      {
         destWidth  = scaleX * tmpImage->GetWidth();
         destHeight = scaleY * tmpImage->GetHeight();
      }

      // At least we have to write one pixel
      if (destWidth < 1.0f)
         destWidth = 1.0f;
      if (destHeight < 1.0f)
         destHeight = 1.0f;

      Gdiplus::Bitmap scaledImage((int)(destWidth + 0.5f), (int)(destHeight + 0.5f));

      scaledImage.SetResolution(tmpImage->GetHorizontalResolution(), 
                                tmpImage->GetVerticalResolution());

      Gdiplus::Graphics grPhoto(&scaledImage);

      grPhoto.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
      grPhoto.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
      grPhoto.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

      Gdiplus::Rect destRect(0, 0, (int)(destWidth + 0.5f), (int)(destHeight + 0.5f));

      hr = grPhoto.DrawImage(tmpImage, destRect, 
                             0, 0, tmpImage->GetWidth(),tmpImage->GetHeight(), 
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

   if (tmpImage != NULL && tmpImage != gdipImage_)
       delete tmpImage;

   
//   // Stop the Gdiplus machine --> this should be executed outside of this method (see above)
//   Gdiplus::GdiplusShutdown(gdiplusToken);

   return hr;
}

// private
void DrawSdk::Image::DoubleSlashes(_TCHAR *tszSource, _TCHAR *tszTarget)
{
   if (tszSource == NULL || tszTarget == NULL)
      return;

   int iLength = _tcslen(tszSource) + 1; // include 0

   int iTargetPos = 0;
   for (int i=0; i<iLength; ++i)
   {
      if (tszSource[i] == '\\')
         tszTarget[iTargetPos++] = '\\';

      tszTarget[iTargetPos++] = tszSource[i];
   }
}
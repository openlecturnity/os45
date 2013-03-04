#include "StdAfx.h"
//#include <windows.h>

#include "mlb_misc.h"
#include "la_imagelist.h"



ASSISTANT::ImageList::ImageList()
{
}

ASSISTANT::ImageList::~ImageList()
{
   for (int i = 0; i < imageElements.GetSize(); ++i)
      delete imageElements[i];
   imageElements.RemoveAll();
}

bool ASSISTANT::ImageList::IsEmpty()
{
   if (imageElements.GetSize() == 0)
      return true;
   else
      return false; 
}


void ASSISTANT::ImageList::GetFileprefix(CString &fileName)
{
   int 
      count;
   _TCHAR
      cCount[20];

   count = -1;
   for (int i = 0; i < imageElements.GetSize(); ++i)
   {
      ImageIdentifier *pImageIdentifier = imageElements[i];
      if (pImageIdentifier != NULL && pImageIdentifier->IsNameEqual(fileName))
         count++;
   }

   if (count >= 0)
   {
      _stprintf(cCount, _T("_%02d"), count);
      fileName += cCount;
   }
   
   return;
}

ASSISTANT::ImageIdentifier *ASSISTANT::ImageList::FindImage(AssistantPicture *image)
{
   for (int i = 0; i < imageElements.GetSize(); ++i)
   {
      ImageIdentifier *pImageIdentifier = imageElements[i];
      if (pImageIdentifier != NULL && pImageIdentifier->IsEqual(image))
         return pImageIdentifier;
   }

   return NULL;
}

ASSISTANT::ImageIdentifier *ASSISTANT::ImageList::AddImage(int iPageWidth, int iPageHeight, LPCTSTR filename)
{
   ImageIdentifier
      *newElement;
   AssistantPicture 
      *image;
      
   if ((newElement = ImageIsAlreadyInserted(filename)) != NULL)
      return newElement;

   newElement = new ImageIdentifier();

   image = new AssistantPicture(iPageWidth, iPageHeight, filename);

   if (image == NULL)
   {
      CString csMessage;
      csMessage.Format(IDS_IMAGE_NOT_FOUND, filename);
      MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      return NULL;
   }

   if (!image->HasImage())
   {
      delete image;
      image = NULL;
      return NULL;
   }

   newElement->SetFilename(filename);
   newElement->SetImage(image);

   CString suffix = filename;
   ASSISTANT::GetSuffix(suffix);
   newElement->SetSuffix(suffix);

   CString
      newPrefix;

   newPrefix = filename;

   ASSISTANT::GetName(newPrefix);
   ASSISTANT::GetPrefix(newPrefix);
   newElement->SetPrefix(newPrefix);
   GetFileprefix(newPrefix);
   newElement->SetName(newPrefix);

   imageElements.Add(newElement);

   return newElement;
}

void ASSISTANT::ImageList::ResetRecording()
{
   for (int i = 0; i < imageElements.GetSize(); ++i)
   {
      ImageIdentifier *pImageIdentifier = imageElements[i];
      if (pImageIdentifier != NULL)
      {
         pImageIdentifier->UnsetWritten();
         pImageIdentifier->SetUsed(false);
      }
   }

   return;
}

int ASSISTANT::ImageList::GetUsedImageCount()
{
   int 
      count;

   count = 0;
   for (int i = 0; i < imageElements.GetSize(); ++i)
   {
      ImageIdentifier *pImageIdentifier = imageElements[i];
      if (pImageIdentifier != NULL)
      {
         if (pImageIdentifier->IsUsed() && !pImageIdentifier->IsWritten()) 
            ++count;
      }
   }

   return count;
}

ASSISTANT::ImageIdentifier *ASSISTANT::ImageList::ImageIsAlreadyInserted(LPCTSTR filename)
{
   for (int i = 0; i < imageElements.GetSize(); ++i)
   {
      ImageIdentifier *pImageIdentifier = imageElements[i];
      if (pImageIdentifier != NULL && 
          pImageIdentifier->IsFilenameEqual(filename))
         return pImageIdentifier;

   }

   return NULL;
}

/***************************************************************/

ASSISTANT::ImageIdentifier::ImageIdentifier()
{
   image_ = NULL;
   dataLength_ = 0;

   isImageUsed_ = false;
   isWritten_ = false;
   isIntern_ = false;
}

ASSISTANT::ImageIdentifier::~ImageIdentifier()
{

   if (image_)
      delete image_;
}

bool ASSISTANT::ImageIdentifier::IsEqual(AssistantPicture *image)
{
   if (image_ == image)
      return true;
   else
      return false;
}

void ASSISTANT::ImageIdentifier::SetImage(AssistantPicture *image)
{
   image_ = image;
}


void ASSISTANT::ImageIdentifier::SetFilename(const _TCHAR *filename)
{
   fileName_ = filename;
}

int ASSISTANT::ImageIdentifier::GetWidth()
{
   int 
      width;

   if (image_ == NULL)
      return 0;

	width = image_->GetWidth();

   return width;
}

int ASSISTANT::ImageIdentifier::GetHeight()
{
   int
      height;

   if (image_ == NULL)
   {
      return 0;
   }

	height = image_->GetHeight();

   return height;
}


void ASSISTANT::ImageIdentifier::WriteImageTo(const _TCHAR *filename, HDC hdc, int width, int height)
{
   if (!isImageUsed_ || isWritten_) 
      return;

   //GUI::Progress::IncrementProgress();
   isWritten_ = true;

   if (image_)
      image_->Write(hdc, filename, width, height);
}


void ASSISTANT::ImageIdentifier::CopyImageTo(const _TCHAR *filename)
{
    // filename is a target path or target file name??
    // The variable name gives no hint about this!

    if (!isIntern_)
    {
        if (filename != NULL && _tcslen(filename) > 0)
        {
            if (fileName_ != filename)
            {
                if (CopyFile(fileName_, filename, false) == 0)
                {
                    CString csMessage;
                    csMessage.Format(IDS_ERROR_WRITE_IMAGE, filename);
                    MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
                }
            }
        }
        else
            int iDummy = 0;
    }
    else
    {
        image_->Write(NULL, filename);
    }
}

bool ASSISTANT::ImageIdentifier::IsNameEqual(CString &name)
{
   if (imagePrefix_ == name)
      return true;
   else
      return false;
}

bool ASSISTANT::ImageIdentifier::IsFilenameEqual(LPCTSTR filename)
{
   if (fileName_ == filename)
      return true;
   else
      return false;
}

ASSISTANT::AssistantPicture::AssistantPicture(int iPageWidth, int iPageHeight, LPCTSTR filename)
{
   fileName_ = filename;

   image_ = NULL;

   m_bShouldRewritten = true;

   // convert char-String in WCHAR-String  
   int iStringLength = _tcslen(filename) + 1;

   WCHAR *wcFilename = (WCHAR *)malloc(iStringLength*sizeof(WCHAR));
#ifdef _UNICODE
   wcscpy(wcFilename, filename);
#else
   MultiByteToWideChar( CP_ACP, 0, filename, iStringLength, 
                        wcFilename, iStringLength);
#endif

   Gdiplus::Image *tmpImage = Gdiplus::Image::FromFile(wcFilename, FALSE);

   GUID gdipGuid;
   tmpImage->GetRawFormat(&gdipGuid);
   if (gdipGuid == Gdiplus::ImageFormatPNG)
      m_bShouldRewritten = false;

   Gdiplus::REAL sourceWidth = (Gdiplus::REAL)tmpImage->GetWidth();
   Gdiplus::REAL sourceHeight = (Gdiplus::REAL)tmpImage->GetHeight();
   Gdiplus::REAL sourceX = 0;
   Gdiplus::REAL sourceY = 0;
   
   Gdiplus::REAL scaleX = (Gdiplus::REAL)(96.0 / tmpImage->GetHorizontalResolution());
   Gdiplus::REAL scaleY = (Gdiplus::REAL)(96.0 / tmpImage->GetVerticalResolution());

   if (scaleX < 1.0 && scaleY < 1.0)
   {
      Gdiplus::REAL pageScaleX = 1600 / iPageWidth;
      Gdiplus::REAL pageScaleY = 1200 / iPageHeight;
      Gdiplus::REAL pageScale = pageScaleX > pageScaleY ? pageScaleX : pageScaleY;
      if ((scaleX * pageScale) < 1 && (scaleY * pageScale) < 1)
      {
         scaleX = scaleX * pageScale;
         scaleY = scaleY * pageScale;
      }
      else
      {
         scaleX = 1.0;
         scaleY = 1.0;
      }
   }

   Gdiplus::REAL destWidth = sourceWidth * scaleX;
   Gdiplus::REAL destHeight = sourceHeight * scaleY;
   Gdiplus::REAL destX = 0;
   Gdiplus::REAL destY = 0; 
   
   image_ = new Gdiplus::Bitmap((int)destWidth, (int)destHeight);
   image_->SetResolution(96.0, 96.0); //emfImage->GetHorizontalResolution(), emfImage->GetVerticalResolution());

   Gdiplus::Graphics grPhoto(image_);
   
   Gdiplus::RectF boundRect;
   Gdiplus::Unit boundUnit;
   tmpImage->GetBounds(&boundRect, &boundUnit);
   
   grPhoto.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
   grPhoto.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
   grPhoto.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
   
   Gdiplus::RectF destRect(0.0, 0.0, destWidth, destHeight);
   
   grPhoto.DrawImage(tmpImage, destRect,
         boundRect.X, boundRect.Y, sourceWidth, sourceHeight,
         Gdiplus::UnitPixel);

   if (tmpImage)
      delete tmpImage;

   if (image_->GetLastStatus() != Gdiplus::Ok)
   {
      delete image_;
      image_ = NULL;
   }

   if (wcFilename)
      free(wcFilename);
}

ASSISTANT::AssistantPicture::~AssistantPicture()
{
   fileName_.Empty();

   if (image_)
      delete image_;
}


int ASSISTANT::AssistantPicture::GetWidth()
{
   int 
      width;

   if (image_)
      width = image_->GetWidth();
   else
      width = 0;

   return width;
}

int ASSISTANT::AssistantPicture::GetHeight()
{
   int 
      height;

   if (image_)
      height = image_->GetHeight();
   else
      height = 0;

   return height;
}

int ASSISTANT::AssistantPicture::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

Gdiplus::Bitmap *ASSISTANT::AssistantPicture::CreateScaledImageFixedSize(Gdiplus::Image *imgPhoto, int width, int height)
{
   Gdiplus::REAL sourceWidth = (Gdiplus::REAL)imgPhoto->GetWidth();
   Gdiplus::REAL sourceHeight = (Gdiplus::REAL)imgPhoto->GetHeight();
   Gdiplus::REAL sourceX = 0;
   Gdiplus::REAL sourceY = 0;
    int destX = 0;
    int destY = 0; 

    float nPercent = 0;
    float nPercentW = 0;
    float nPercentH = 0;

    nPercentW = ((float)width/(float)sourceWidth);
    nPercentH = ((float)height/(float)sourceHeight);
    if(nPercentH < nPercentW)
    {
        nPercent = nPercentH;
    }
    else
    {
        nPercent = nPercentW;
    }

    Gdiplus::REAL destWidth  = sourceWidth * nPercent;
    Gdiplus::REAL destHeight = sourceHeight * nPercent;

    Gdiplus::Bitmap *scaledImage = new Gdiplus::Bitmap(width, height);

    
    scaledImage->SetResolution(imgPhoto->GetHorizontalResolution(), 
                     imgPhoto->GetVerticalResolution());
    
    Gdiplus::Graphics grPhoto(scaledImage);
    
    Gdiplus::RectF boundRect;
    Gdiplus::Unit boundUnit;
    image_->GetBounds(&boundRect, &boundUnit);
    
    
    grPhoto.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
    grPhoto.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
    grPhoto.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    
    Gdiplus::RectF destRect(0.0, 0.0, destWidth, destHeight);
    grPhoto.DrawImage(imgPhoto, destRect,
                                boundRect.X, boundRect.Y, sourceWidth,sourceHeight,
                        Gdiplus::UnitPixel);

    return scaledImage;
}

void ASSISTANT::AssistantPicture::Write(HDC hDC, const _TCHAR *filename, int width, int height)
{
 
   // Length of string with '0'
   int iStringLength = _tcslen(filename) + 1;
   WCHAR *wcSaveFilename = (WCHAR *)malloc(iStringLength*sizeof(WCHAR));

#ifdef _UNICODE
   wcscpy(wcSaveFilename, filename);
#else
   MultiByteToWideChar( CP_ACP, 0, filename, iStringLength, 
                        wcSaveFilename, iStringLength);
#endif

   CLSID pngClsid;
   GetEncoderClsid(L"image/png", &pngClsid);

   if (m_bShouldRewritten) //image_->GetType() = Gdiplus::ImageTypeMetafile)
   {
      Gdiplus::Bitmap *scaledImage = CreateScaledImageFixedSize(image_, width, height);
      scaledImage->Save(wcSaveFilename,  &pngClsid, NULL);
      delete scaledImage;

   }
   else
   {
      CopyFile(fileName_, filename, false);
      //image_->Save(wcSaveFilename,  &pngClsid, NULL);
   }

   if (wcSaveFilename)
      delete wcSaveFilename;
}

void ASSISTANT::AssistantPicture::Draw(Gdiplus::Graphics &graphics, int x, int y, int width, int height)
{
   if (image_)
   {
      Gdiplus::Status status;

      Gdiplus::RectF boundRect;
      Gdiplus::Unit boundUnit;
      image_->GetBounds(&boundRect, &boundUnit);

      Gdiplus::RectF destRect;
      destRect.X = (Gdiplus::REAL)x;
      destRect.Y = (Gdiplus::REAL)y;
      destRect.Width = (Gdiplus::REAL)width;
      destRect.Height = (Gdiplus::REAL)height;


      graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
      graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
      graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
      status = graphics.DrawImage(image_, destRect, 
         boundRect.X, boundRect.Y, (Gdiplus::REAL)image_->GetWidth(), (Gdiplus::REAL)image_->GetHeight(), Gdiplus::UnitPixel);

      //Gdiplus::Pen pen(Gdiplus::Color(255,255,0,0), 1.0f); 
      //graphics.DrawRectangle(&pen, destRect);

      if (status != Gdiplus::Ok)
      {
         // TODO Fehler
      }
   }
}

bool ASSISTANT::AssistantPicture::HasImage()
{
   if (image_)
      return true;

   return false;
}

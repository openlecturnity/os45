/*********************************************************************

  program : mlb_image.cpp
  authors : Gabriela Maass
  date    : 04.04.2000
  desc    : Functions to read, draw and manipulate images
  
**********************************************************************/
#include "StdAfx.h"

#include "..\..\Studio\Resource.h"
#include "la_project.h" // In StdAfx.h
#include "mlb_objects.h"
#include "mlb_misc.h"

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/


ASSISTANT::WindowsImage::WindowsImage(double _x, double _y, double _width, double _height, int _zPosition, 
                                      LPCTSTR _filename, uint32 _id, 
                                      LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects)
                        : DrawObject(_x, _y, _width, _height, _zPosition, _id, 
                                     _hyperlink, _currentDirectory, _linkedObjects)
{ 
   image_   = NULL;
   
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
   
   externImage_ = true;
}

ASSISTANT::WindowsImage::~WindowsImage()
{
   filename_.Empty();
   srcPath_.Empty();
}

int ASSISTANT::WindowsImage::BuildImage(ImageIdentifier *_image, bool verifyImage)
{ 
   
   if (_image) 
   {
      image_ = _image;
      if (((int)m_dWidth == 0) && ((int)m_dHeight == 0)) 
      {
         m_dWidth = image_->GetWidth();
         m_dHeight = image_->GetHeight();
      }
   }
   else 
   {
      CString csImageNotFound;
      csImageNotFound.Format(IDS_IMAGE_NOT_FOUND, filename_);
      MessageBox(NULL, csImageNotFound, NULL, MB_OK | MB_TOPMOST);
   }
   
   
   return 1;
}

void ASSISTANT::WindowsImage::BuildImage(ASSISTANT::Page *page, LPCTSTR path, bool verifyImage)
{  
   ImageList
      *imageList;
   CString
      destPath;
   
   imageList = ASSISTANT::Project::active->GetImageList();
   
   image_ = imageList->AddImage(page->GetWidth(), page->GetHeight(), srcPath_);
   
   if (image_ == NULL) 
   {
      CString csMessage;
      csMessage.Format(IDS_IMAGE_NOT_FOUND, filename_);
      MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      visible = false;
      return;
   }
   
   
   if ((m_dWidth == 0) || (m_dHeight == 0))
   {
      m_dWidth = image_->GetWidth();
      m_dHeight = image_->GetHeight();
   }
}

void ASSISTANT::WindowsImage::SetImage(ImageIdentifier *_image)
{
   image_ = _image;
}

bool ASSISTANT::WindowsImage::IsVisible()
{
   if (visible) 
      return true; 
   else
      return false;
}

void ASSISTANT::WindowsImage::Update(float _zoomFactor)
{
   if (!visible) 
      return;
   
   recorded = false;
   
   ModifyCoords(m_dX, m_dY, m_dWidth, m_dHeight, _zoomFactor);
}

ASSISTANT::DrawObject *ASSISTANT::WindowsImage::Copy(bool keepZPosition)
{
   WindowsImage *ret = new WindowsImage(m_dX, m_dY, m_dWidth, m_dHeight, -1, filename_, Project::active->GetObjectID(), hyperlink_, currentDirectory_, m_ssLinkedObjects);
   
   ret->SetImage(image_);
   if (isInternLink)
      ret->LinkIsIntern(true);
   else
      ret->LinkIsIntern(false);
   
   if (keepZPosition)
      ret->SetOrder(order);
   
   ret->SetImageExtern(externImage_);
   
   return (DrawObject *)ret;
}

void ASSISTANT::WindowsImage::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{ 
   if (!visible)
      return;
   
   if (image_ == NULL)
      return;
   
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform(dOffX, dOffY);
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   AssistantPicture *pImage = image_->GetImage();

   if (pImage)
      pImage->Draw(graphics, (int)m_dX, (int)m_dY, (int)m_dWidth, (int)m_dHeight);
}

void ASSISTANT::WindowsImage::ModifyCoords(double _x, double _y , double _width, double _height, float _zoomFactor)
{
   if (!visible) return;
   
   m_dX = _x;
   m_dY = _y;
   
   Resize(_width, _height);
   /*/
   ReDraw(_zoomFactor);
   /*/
}


void ASSISTANT::WindowsImage::Resize(int _width, int _height)
{ 
   recorded = false;

   m_dWidth       = _width;
   m_dHeight      = _height;
}

void ASSISTANT::WindowsImage::GetBBox(int *minX, int *minY, int *maxX, int *maxY)
{  
   if ((m_dX+m_dWidth) < m_dX) {
      *minX = m_dX + m_dWidth - 3;
      *maxX = m_dX + 3;
   }
   else {
      *minX = m_dX - 3;
      *maxX = m_dX + m_dWidth + 3;
   }
   if ((m_dY+m_dHeight) < m_dY) {
      *minY = m_dY + m_dHeight - 3;
      *maxY = m_dY + 3;
   }
   else {
      *minY = m_dY - 3;
      *maxY = m_dY + m_dHeight + 3;
   }
   
}

void ASSISTANT::WindowsImage::GetBoundingBox(CRect &rcObject)
{
   rcObject.left = m_dX;
   rcObject.right = m_dX + m_dWidth;
   rcObject.top = m_dY;
   rcObject.bottom = m_dY + m_dHeight;
   
   rcObject.NormalizeRect();
}

void ASSISTANT::WindowsImage::SaveObject(FILE *fp, int level, LPCTSTR path)
{
   if (!visible || (image_ == NULL)) return;
   
   WCHAR *tab = (WCHAR *)malloc((level*2+1)*sizeof(WCHAR));
   *tab = L'\0';
   for (int i = 0; i < level; i++) 
      wcscat(tab, L"  ");
   
   CString imageName = image_->GetPrefix();
   imageName += image_->GetSuffix();
   WCHAR *wszImage = ASSISTANT::ConvertTCharToWChar(imageName);

   CString hyperlinkString;
   MakeHyperlinkString(hyperlinkString);
   WCHAR *wszHyperlink = ASSISTANT::ConvertTCharToWChar(hyperlinkString);

   // Note: The file name may contain single quotes which
   // usually have to be backslashed. In this case, we do
   // not have to do that, because the file name is in
   // double quotes.
   fwprintf(fp, L"%s<IMAGE x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" filename=\"%s\" ZPosition=\"%d\"%s>\n",
                tab, (int)m_dX, (int)m_dY, (int)m_dWidth, (int)m_dHeight, 
                wszImage, (int)order, wszHyperlink);
   
   if (wszImage)
      delete wszImage;
   
   if (wszHyperlink)
      delete wszHyperlink;

   fwprintf(fp, L"%s</IMAGE>\n", tab);
   
   if (tab)
      delete tab;
   
   CString
      destPath;
   destPath = path;
   destPath += _T("\\");
   destPath += imageName;
   
   image_->CopyImageTo(destPath);
}

int ASSISTANT::WindowsImage::RecordableObjects()
{
   if (!visible || (image_ == NULL))
      return 0;
   
   return 1;
}

void ASSISTANT::WindowsImage::SaveAsAOF(CFileOutput *fp)
{
   CString
      imageName;
   
   if (!visible || recorded || (image_ == NULL)) return;
   
   recorded = true;
   
   imageName = image_->GetPrefix(); 
   imageName += _T(".png");
   
   recID = ASSISTANT::Project::active->recCount;
   ASSISTANT::Project::active->recCount++;
   
   CString line;

   line.Format(_T("<IMAGE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f fname=\"%s\"></IMAGE>\n"),
               (int)m_dX ,(int)m_dY, (int)m_dWidth, (int)m_dHeight, 
               m_dX ,m_dY, m_dWidth, m_dHeight, 
               imageName);
   fp->Append(line);
   
   image_->SetUsed(true);

   ASSISTANT::Project::active->AppendImageToWriteQueue(this);
}

void ASSISTANT::WindowsImage::WriteImage()
{
   CString
      imageName;
   
   if (image_ == NULL)
      return;
   
   imageName = ASSISTANT::Project::active->GetDataPath();
   imageName += _T("\\");
   imageName += image_->GetPrefix();
   imageName += _T(".png");

   HDC hDC = CreateCompatibleDC(NULL);
   image_->WriteImageTo(imageName, hDC, (int)m_dWidth, (int)m_dHeight);
}


void ASSISTANT::WindowsImage::CopyImageTo(LPCTSTR _destPath)
{
   if (image_) 
   {
      CopyFile(srcPath_, _destPath, true);
   }
}


/**** Static Functions ***/

ASSISTANT::DrawObject *ASSISTANT::WindowsImage::Load(ASSISTANT::SGMLElement *hilf, CString &imagePath) 
{
   WindowsImage
      *obj;
   _TCHAR
      *tmp;
   int
      x, y,
      width, height,
		zPosition,
      iPPTId;
   _TCHAR
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
   
   if (hyperlink != NULL)
   {
      obj = new ASSISTANT::WindowsImage(x, y, width, height, zPosition, filePath,
         Project::active->GetObjectID(), hyperlink, currentDirectory, linkedObjects);
   
      obj->LinkIsIntern(false);
   }
   else
   {
     obj = new ASSISTANT::WindowsImage(x, y, width, height, zPosition, filePath,
        Project::active->GetObjectID(), internLink, currentDirectory, linkedObjects);
   
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

bool ASSISTANT::WindowsImage::IsIdentic(DrawObject *obj)
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


bool ASSISTANT::WindowsImage::IsInRect(CRect &rcSelection)
{
   if (!visible || ASSISTANT::Project::active->DoFreezeImages())
      return false; 

   return DrawObject::IsInRect(rcSelection);
}

bool ASSISTANT::WindowsImage::ContainsRect(CRect &rcSelection)
{
   if (!visible || ASSISTANT::Project::active->DoFreezeImages())
      return false; 

   CRect rcObject;
   GetBoundingBox(rcObject);

   if (rcSelection.Width() == 0 && rcSelection.Height() == 0)
   {
      CPoint ptSelection(rcSelection.left, rcSelection.top);
      if (rcObject.PtInRect(ptSelection))
         return true;
      else
         return false;
   }
   else
   {
      CRect rcIntersection;
      rcIntersection.IntersectRect(rcSelection, rcObject);
      
      
      if (rcIntersection == rcSelection)
         return true;
      else
         return false;
   }
}

bool ASSISTANT::WindowsImage::ContainsPoint(Gdiplus::PointF &ptMouse)
{
   if (!visible || ASSISTANT::Project::active->DoFreezeImages())
      return false; 

   CRect rcObject;
   GetBoundingBox(rcObject);
   
   CPoint pt((int)ptMouse.X, (int)ptMouse.Y);

   if (rcObject.PtInRect(pt))
      return true;
   else
      return false;
   
}

void ASSISTANT::WindowsImage::SetUnrecorded()
{
   recorded = false;
}

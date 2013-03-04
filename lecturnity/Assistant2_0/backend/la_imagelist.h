#ifndef __ASSISTANT_IMAGELIST__
#define __ASSISTANT_IMAGELIST__

#include <olectl.h>
#include <gdiplus.h>
#include <afxtempl.h>

namespace ASSISTANT {
   
   class AssistantPicture
   {
   public:
      Gdiplus::Bitmap 
         *image_;
      CString
         fileName_;
      bool 
         m_bShouldRewritten;
      
      AssistantPicture(int iPageWidth, int iPageHeight, LPCTSTR filename);
      ~AssistantPicture();
      
      int GetWidth();
      int GetHeight();
      void Draw(Gdiplus::Graphics &graphics, int x, int y, int width, int height);
      void Write(HDC hDC, LPCTSTR filename, int width=0, int height=0);
      bool HasImage();
      
   private:
      Gdiplus::Bitmap *CreateScaledImageFixedSize(Gdiplus::Image *imgPhoto, int width, int height);
      int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
   };
   
   class ImageIdentifier 
   {
   protected:
      AssistantPicture
         *image_;
      CString
         imageName_,
         imagePrefix_, 
         imageSuffix_,
         fileName_;
      int 
         dataLength_;
      bool
         isImageUsed_,
         isWritten_,
         isIntern_;
      
   public:
      ImageIdentifier();
      //ImageIdentifier(IPicture *image, char *fileData);
      ~ImageIdentifier();
      
      bool IsEqual(AssistantPicture *image);
      AssistantPicture *GetImage() {return image_;}
      void SetFilename(LPCTSTR filename);
      void SetImage(AssistantPicture *image);
      int GetWidth();
      int GetHeight();
      int GetDataLength() {return dataLength_;}
      void WriteImageTo(const _TCHAR *filename, HDC hdc, int width=0, int height=0);
      void CopyImageTo(const _TCHAR *filename);
      void SetUsed(int used) {isImageUsed_ = used == 0 ? false : true;}
      bool IsUsed() {return isImageUsed_;}
      void UnsetWritten() {isWritten_ = false;}
      bool IsWritten() {return isWritten_;}
      void IsIntern() {isIntern_ = true;}
      
      void SetName(LPCTSTR imageName) {imageName_ = imageName;}
      void SetPrefix(LPCTSTR imagePrefix) {imagePrefix_ = imagePrefix;}
      void SetSuffix(LPCTSTR imageSuffix) {imageSuffix_ = imageSuffix;}
      CString &GetPrefix() {return imageName_;}
      CString &GetSuffix() {return imageSuffix_;}
      bool IsNameEqual(CString &name);
      bool IsFilenameEqual(const _TCHAR *filename);
      
   };
   
   class ImageList 
   {
   protected:
      CArray<ImageIdentifier *, ImageIdentifier *>  
         imageElements;
      bool
         isIntern_;
   public:
      ImageList();
      ~ImageList();
      
      void GetFileprefix(CString &fileName);
      bool IsEmpty();
      
      ImageIdentifier *FindImage(AssistantPicture *image);
      ImageIdentifier *AddImage(int iPageWidth, int iPageHeight, LPCTSTR filename);
      void ResetRecording();
      int GetUsedImageCount();
      ASSISTANT::ImageIdentifier *ImageIsAlreadyInserted(LPCTSTR filename);
   };
}
   
#endif
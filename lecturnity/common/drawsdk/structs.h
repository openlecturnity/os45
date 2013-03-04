#ifndef __DRAWSDK_STRUCTS__
#define __DRAWSDK_STRUCTS__

#include "tools.h"
#include <gdiplus.h>

namespace DrawSdk
{
   class DRAWSDK_EXPORT CImageInformation
   {
      
   public:
      CImageInformation(const _TCHAR *tszFilename);
      ~CImageInformation();
      
   public:
      bool IsEqualFilename(const _TCHAR *tszFilename);
      bool OpenImage();
      Gdiplus::Image *GetImageData() {return m_gdipImage;}
      
   private:
      _TCHAR *m_tszFilename;
      Gdiplus::Image *m_gdipImage;
   };
   
   class DRAWSDK_EXPORT CImageArray
   {
   public:
      CImageArray();
      ~CImageArray();
      
   public:
      CImageInformation *GetImage(const _TCHAR *tcFilename);
      void Add(CImageInformation *pImage);
      
   private:
      // array to save image information
      // std::vector<CImageInformation *>
      void *m_arImageList;
   };
}
#endif //__DRAWSDK_STRUCTS__
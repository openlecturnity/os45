#include "StdAfx.h"
#include "structs.h"

#define IMAGE_LIST (*((std::vector<DrawSdk::CImageInformation *> *) m_arImageList))

DrawSdk::CImageArray::CImageArray()
{
   m_arImageList = (void *) (new std::vector<DrawSdk::CImageInformation *>);
}

DrawSdk::CImageArray::~CImageArray()
{
   for (int i = 0; i < IMAGE_LIST.size(); ++i)
   {
      CImageInformation *pImage = IMAGE_LIST[i];
      delete pImage;
   }
   IMAGE_LIST.clear();
   delete ((std::vector<DrawSdk::CImageInformation *> *) m_arImageList);
   m_arImageList = NULL;
}

DrawSdk::CImageInformation *DrawSdk::CImageArray::GetImage(const _TCHAR *tszFilename)
{
   CImageInformation *pImage = NULL;
   if (!IMAGE_LIST.empty())
   {
      for (int i = 0; i < IMAGE_LIST.size(); ++i)
      {
         CImageInformation *pTempImage = IMAGE_LIST[i];
         if (pTempImage->IsEqualFilename(tszFilename))
         {
            pImage = pTempImage;
            break;
         }
      }
   }

   if (pImage == NULL)
   {
      pImage = new CImageInformation(tszFilename);
      Add(pImage);
   }

   return pImage;
}

void DrawSdk::CImageArray::Add(CImageInformation *pImage)
{
   IMAGE_LIST.push_back(pImage);
}

/************************************************
 ************************************************
 ************************************************/

DrawSdk::CImageInformation::CImageInformation(const _TCHAR *tszFilename)
{
   m_tszFilename = new TCHAR[_tcslen(tszFilename) + 1];
   _tcsncpy(m_tszFilename, tszFilename, _tcslen(tszFilename));
   m_tszFilename[_tcslen(tszFilename)] = '\0';

   m_gdipImage = NULL;
   OpenImage();
}

DrawSdk::CImageInformation::~CImageInformation()
{
   if (m_tszFilename)
      delete[] m_tszFilename;
   if (m_gdipImage)
      delete m_gdipImage;
}

bool DrawSdk::CImageInformation::IsEqualFilename(const _TCHAR *tszFilename)
{
   if (_tcscmp(m_tszFilename, tszFilename) == 0)
      return true;

   return false;
}

bool DrawSdk::CImageInformation::OpenImage()
{
   // convert char-String in WCHAR-String  
   int iStringLength = _tcslen(m_tszFilename) + 1;
   WCHAR *wcFilename = (WCHAR *)malloc(iStringLength*sizeof(WCHAR));
#ifdef _UNICODE
   wcscpy(wcFilename, m_tszFilename);
#else
   MultiByteToWideChar( CP_ACP, 0, m_tszFilename, iStringLength, 
                        wcFilename, iStringLength);
#endif

   // Read image
   m_gdipImage = new Gdiplus::Image(wcFilename, FALSE);

   if (wcFilename)
      free(wcFilename);

   return true;
}
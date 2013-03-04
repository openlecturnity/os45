
class ImageWriter
{

   public:
      ImageWriter(const WCHAR *wszImagePath, const WCHAR *szImagePrefix, const WCHAR *szImageSuffix);
      ImageWriter(const WCHAR *wszImagePath);
      ~ImageWriter();

      HRESULT FindEmfImageData();
      HRESULT FindWmfImageData();
      HRESULT RewriteImage();
      HRESULT WriteMetafileAsPNG(bool bUseGDIFunctions);
      HRESULT ExtractDimensionFromMetafile(bool bDeleteImage);
      
      HRESULT CheckEnhancedMetafile();

      bool GetResult() {return m_bSuccess;}
      WCHAR *GetImageFilename() {return m_wszImageFilename;}
      void AddBitmap(BITMAPINFO *pBmi, char *pData, int iWidth, int iHeight);
      bool IsValidBitmap(int iDestX, int iDestY, int iWidth, int iHeight);
      HRESULT Write(Gdiplus::Bitmap *gdipBitmap, bool bConvertToJpeg=true);
      int GetBitmapCount() {return m_iBitmapCount;}
      void SetImageNotSupported() {m_bSupportedImage = false;}
      bool IsImageSupported() {return m_bSupportedImage;}

      Gdiplus::Image *CreateScaledImageFixedSize(Gdiplus::Metafile *emfImage);

      int GetX() {return m_iDestX;}
      int GetY() {return m_iDestY;}
      int GetWidth() {return m_iWidth;}
      int GetHeight() {return m_iHeight;}
      
      void SetHorizontalResolution(int iHorizontalResolution) {m_iHorizontalResolution = iHorizontalResolution;}
      void SetVerticalResolution(int iVerticalResolution) {m_iVerticalResolution = iVerticalResolution;}

   private:
      enum
      {
         PNG_MAXSIZE = 400000
      };
      int GetEncoderClsid(const WCHAR* wszFormat, CLSID* pClsid);
      WCHAR *ComposeFilename(WCHAR *wszFileSuffix);
      void ConvertEnhancedMetafile();
      void SaveEnhancedMetafile();
      void SaveEnhancedMetafileWithoutGrid();
      int GetFileSize();
      
   private:
      WCHAR *m_wszImagePath;
      WCHAR *m_wszImagePrefix;
      WCHAR *m_wszImageSuffix;
      WCHAR *m_wszImageFilename;

      HENHMETAFILE m_hEmf;
      Gdiplus::Bitmap *m_gdipBitmap[2];
      int m_iBitmapCount;

      bool  m_bSuccess;
      bool  m_bSupportedImage;
      
      bool m_bEmptyImage;

      int m_iWidth;
      int m_iHeight; 

      int m_iDestX;
      int m_iDestY;

      Gdiplus::REAL m_fScaleX;
      Gdiplus::REAL m_fScaleY;

      int m_iHorizontalResolution;
      int m_iVerticalResolution;

};

class FontExtractor
{
public:
   FontExtractor(LPCWSTR wszImagePath);
   ~FontExtractor() {};

   WCHAR *GetFontFamily();
   void SetFontFamily(WCHAR *wcFontFamily);
   HRESULT ExtractEmfFontInfo();
   HRESULT ExtractWmfFontInfo(LPCWSTR wszImageName);
   void IncreaseCharCount(int nChars) {m_nCharCount += nChars;}
   HRESULT GetCharCount() {return m_nCharCount;}
   void IncreaseActualCharCount(int nChars) {m_nActualChar += nChars;}
   HRESULT GetActualCharCount() {return m_nActualChar;}

public:
   bool m_bDoCopy;

private:
   WCHAR m_wcFontFamily[256];

   HENHMETAFILE m_hEmf;

   int m_nCharCount;
   int m_nActualChar;

};


struct UsedCallbackData
{
   Gdiplus::Metafile *gdipMetafile;
   bool bIsWMF;
   int iX;
   int iY;
   float fScaleX;
   float fScaleY;
   int iFillRectCount;
};


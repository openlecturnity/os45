
short __declspec(dllexport) CALLBACK  ExtractFontFamily(const WCHAR *wszImagePath, WCHAR *wszFontFamily, int *iStringLength);
short __declspec(dllexport) CALLBACK  FontSupportsString(const WCHAR *wszString, int iLength, const WCHAR *wszFontFamily, int isBold, int isItalic);
short __declspec(dllexport) CALLBACK  GetFontMetrics(const WCHAR *wszFontFamily, int iFontSize, int isBold, int isItalic, double *iAscent, double *iDescent);

short __declspec(dllexport) CALLBACK  RewritePng(const WCHAR *szImagePath, WCHAR *szImageName, const WCHAR *szImagePrefix, WCHAR *szImageSuffix, int *iStringLength);
short __declspec(dllexport) CALLBACK  ConvertMetafile(const WCHAR *szImagePath, WCHAR *szImageName, const WCHAR *szImagePrefix, WCHAR *szImageSuffix, int *iStringLength);
short __declspec(dllexport) CALLBACK  SaveMetafileAsPNG(const WCHAR *szImagePath, WCHAR *szImageName, const WCHAR *szImagePrefix, WCHAR *szImageSuffix, int *iStringLength, 
                                                        int iHorizontalResolution, int iVerticalResolution, int *iX, int *iY, int *iWidth, int *iHeight);
short __declspec(dllexport) CALLBACK  SaveMetafileAsPNGWithoutGrid(const WCHAR *szImagePath, WCHAR *szImageName, const WCHAR *szImagePrefix, WCHAR *szImageSuffix, int *iStringLength, 
                                                                   int iHorizontalResolution, int iVerticalResolution, int *iX, int *iY, int *iWidth, int *iHeight);

short __declspec(dllexport) CALLBACK  CheckMetafile(const WCHAR *szImagePath);

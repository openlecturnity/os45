#ifndef __TEMPLATEKEYWORDS_H__5DD73091_CBAC_4be6_BE7D_3D9E5B4D27E7
#define __TEMPLATEKEYWORDS_H__5DD73091_CBAC_4be6_BE7D_3D9E5B4D27E7

#include "imc_lecturnity_converter_LPLibs.h"

// Keywords for 'Standard Navigation'
#define NUMBER_OF_NAVIGATION_STATES imc_lecturnity_converter_LPLibs_NUMBER_OF_NAVIGATION_STATES 

const CString CS_NAVIGATION_KEYWORDS[] = {
   "NavigationControlBar", 
   "NavigationStandardButtons", 
   "NavigationTimeLine", 
   "NavigationTimeDisplay", 
   "NavigationDocumentStructure", 
   "NavigationPlayerSearchField", 
   "NavigationPlayerConfigButtons",
   "NavigationPluginContextMenu"
};


enum TE_TYPES
{
   TE_TYPE_IMAGE = 1,
   TE_TYPE_INTEGER,
   TE_TYPE_COLOR,
   TE_TYPE_GENERAL,
   TE_TYPE_TEXT,
   TE_TYPE_SELECT
};

struct TE_VAR
{
   DWORD  dwSize;
   DWORD  dwType;
   WCHAR  wszVarName[128];
};

class CTemplateKeyword
{
public:
   CTemplateKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName);
   virtual void Restore(LPVOID pData, DWORD dwSize);
   virtual bool Serialize(LPVOID pData, DWORD *pdwSize);
   virtual int GetType()=0;
   virtual CString GetReplaceString()=0;
   virtual CString GetStringRepresentation() = 0;
   virtual bool ParseFromString(CString& csRepresentation) = 0;
public:
   TE_VAR *m_pConfig;
   CString m_csName;
   CString m_csFriendlyName;
   DWORD m_dwMinStructSize;
};

enum TE_IMAGE_SIZE_HANDLING
{
   TE_IMAGE_FITS_ALREADY = 1,
   TE_IMAGE_ADAPT_SIZE,
   TE_IMAGE_DO_NOT_ADAPT
};

struct TE_VAR_IMAGE : TE_VAR
{
   WCHAR wszFileName[MAX_PATH];
   DWORD  dwImageSizeHandling;
};

class CTemplateImageKeyword : public CTemplateKeyword
{
public:
   CTemplateImageKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName,
      int nMinWidth, int nMinHeight, int nMaxWidth, int nMaxHeight);
   void SetFileName(const _TCHAR *tszFileName);
   UINT GetFileName(_TCHAR *tszFileName, DWORD *pdwSize);
   void SetImageSizeHandling(DWORD dwAdaptImageSize);
   DWORD GetImageSizeHandling();
   CString GetImageName();
   virtual int GetType() {return TE_TYPE_IMAGE;}
   virtual CString GetReplaceString();
   virtual CString GetStringRepresentation();
   virtual bool ParseFromString(CString& csRepresentation);
private:
   void GetImageSizeFromFile(CString csFileName, int &nOrigWidth, int &nOrigHeight);
   bool CalculateAdaptedImageSize(int &nOrigWidth, int &nOrigHeight, int &nNewWidth, int &nNewHeight);
public:
   int m_nMinWidth;
   int m_nMinHeight;
   int m_nMaxWidth;
   int m_nMaxHeight;
};

struct TE_VAR_INTEGER : TE_VAR
{
   int   nValue;
};

class CTemplateIntegerKeyword : public CTemplateKeyword
{
public:
   CTemplateIntegerKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName,
      int nMinValue, int nMaxValue);
   void SetValue(int nValue);
   int GetValue();
   virtual int GetType() {return TE_TYPE_INTEGER;}
   virtual CString GetReplaceString();
   virtual CString GetStringRepresentation();
   virtual bool ParseFromString(CString& csRepresentation);
public:
   int m_nMinValue;
   int m_nMaxValue;
};

struct TE_VAR_COLOR : TE_VAR
{
   COLORREF rgbColor;
};

class CTemplateColorKeyword : public CTemplateKeyword
{
public:
   CTemplateColorKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName);
   void SetColor(COLORREF color);
   COLORREF GetColor();
   virtual int GetType() {return TE_TYPE_COLOR;}
   virtual CString GetReplaceString();
   virtual CString GetStringRepresentation();
   virtual bool ParseFromString(CString& csRepresentation);
};

struct TE_VAR_GENERAL : TE_VAR
{
   DWORD dwOffset;
   WCHAR wszGeneral[1];
};
 
class CTemplateGeneralKeyword : public CTemplateKeyword
{
public:
   CTemplateGeneralKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName);
   void SetGeneral(const _TCHAR *tszGeneral);
   UINT GetGeneral(_TCHAR *tszGeneral, DWORD *pdwSize);
   virtual int GetType() {return TE_TYPE_GENERAL;}
   virtual CString GetReplaceString();
   virtual CString GetStringRepresentation();
   virtual bool ParseFromString(CString& csRepresentation);
};

struct TE_VAR_TEXT : TE_VAR
{
   DWORD dwOffset;
   WCHAR wszText[1];
};

class CTemplateTextKeyword : public CTemplateKeyword
{
public:
   CTemplateTextKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName, int nMaxLength);
   void SetText(const _TCHAR *tszText);
   UINT GetText(_TCHAR *tszText, DWORD *pdwText);
   virtual int GetType() {return TE_TYPE_TEXT;}
   virtual CString GetReplaceString();
   virtual CString GetStringRepresentation();
   virtual bool ParseFromString(CString& csRepresentation);
public:
   int m_nMaxLength;
};

struct TE_VAR_SELECT : TE_VAR_TEXT
{
};

class CTemplateSelectKeyword : public CTemplateTextKeyword
{
public:
   CTemplateSelectKeyword(const _TCHAR *tszName, const _TCHAR *tszFriendlyName, 
                          const _TCHAR *tszVisibleOptions, const _TCHAR *tszOptions);
   virtual int GetType() {return TE_TYPE_SELECT;}
   
   // Note: the selected value is the text of the CTemplateTextKeyword (super class)
public:
   CArray<CString, CString> m_caVisibleOptions;
   CArray<CString, CString> m_caOptions;
   int m_nOptionCount;
};

#endif // __TEMPLATEKEYWORDS_H__5DD73091_CBAC_4be6_BE7D_3D9E5B4D27E7
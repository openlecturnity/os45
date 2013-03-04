#ifndef __ASSISTANT_FONT__
#define __ASSISTANT_FONT__

#include "afxtempl.h"

namespace ASSISTANT
{
   
   typedef struct _tagTT_OFFSET_TABLE{
      USHORT	uMajorVersion;
      USHORT	uMinorVersion;
      USHORT	uNumOfTables;
      USHORT	uSearchRange;
      USHORT	uEntrySelector;
      USHORT	uRangeShift;
   }TT_OFFSET_TABLE;
   
   typedef struct _tagTT_TABLE_DIRECTORY{
      char	szTag[4];			//table name
      ULONG	uCheckSum;			//Check sum
      ULONG	uOffset;			//Offset from beginning of file
      ULONG	uLength;			//length of the table in bytes
   }TT_TABLE_DIRECTORY;
   
   typedef struct _tagTT_NAME_TABLE_HEADER{
      USHORT	uFSelector;			//format selector. Always 0
      USHORT	uNRCount;			//Name Records count
      USHORT	uStorageOffset;		//Offset for strings storage, from start of the table
   }TT_NAME_TABLE_HEADER;
   
   typedef struct _tagTT_NAME_RECORD{
      USHORT	uPlatformID;
      USHORT	uEncodingID;
      USHORT	uLanguageID;
      USHORT	uNameID;
      USHORT	uStringLength;
      USHORT	uStringOffset;	//from start of storage area
   }TT_NAME_RECORD;
      
   typedef struct _tagFONT_PROPERTIES{
      CString csName;
      CString csCopyright;
      CString csTrademark;
      CString csFamily;
      CString ttfName;
   }FONT_PROPERTIES, *LPFONT_PROPERTIES;

   typedef struct _PAGENUMBER{
      CString csPageNumber;
      int iPageNumber;
   }PAGE_NUMBER;

   class FontInfo
   {
   public:

      FontInfo();
      ~FontInfo();

      void SetFontFamily(const _TCHAR *fontFamily);
      CString &GetFontFamily() {return fontFamily_;}
      void SetFontFilename(const _TCHAR *fontFace, const _TCHAR *fontFilename);
      void AddFontFace(const _TCHAR *fontFace, int index);
      CString &GetFontFilename(bool isBold, bool isItalic);
      bool IsSystemFamilyEqual(const _TCHAR *fontFamily);
      bool IsFontFamilyEqual(const _TCHAR *fontFamily);
      bool HasTTFFilename();
      void AddErrorPage(LPCTSTR szPageNumber, int iPageNumber);
      bool HasNewErrors();    
      void DeleteErrorPages();
      void FillErrorPageList(CString &csErrorPageList);
      

   private:
      CString fontFamily_;
      CString fontFace_[4];
      // 0 == regular
      // 1 == bold
      // 2 == italic
      // 3 == bold + italic
      CString fontFilename_[4];
      bool hasTTF_;
      bool isAskedForTTF_;
      
      CArray<PAGE_NUMBER *, PAGE_NUMBER *> m_aErrorPages;
   };

   class FontList
   {
   public:

      FontList();
      ~FontList();
      
      void GetFontFamilies(CString &fontFamilies);
      void GetFontFilename(const _TCHAR *fontFamily, CString &fileName, bool isBold, bool isItalic);
      FontInfo *AddNewFamily(const _TCHAR *family, bool bIsTTFont);
      bool HasTTFFilename(const _TCHAR *fontFamily);
      void AddErrorPage(const _TCHAR *fontFamily, LPCTSTR szPageNumber, int iPageNumber);
      bool IncludeUnsupportedFonts();
      void DeleteErrorPages();
      void FillErrorList(CStringArray &arFontNames, CStringArray &arErrorPages, 
                         CStringArray &arErrorTypes);


   private:
      void InitVariables();
      void ReleaseVariables();
      void InitializeFontfamilies();
      void AddFilenames();
      //void AddTTFFilenames();
      bool GetFontProperties(const _TCHAR *fileName, const _TCHAR *fontPath, LPFONT_PROPERTIES lpFontProps);
      bool FindFontInfo(const _TCHAR *entryName, CArray<FontInfo *, FontInfo *> *pList);

      
   private:
      CArray<FontInfo *, FontInfo *> fontInfoList_;
      CArray<FontInfo *, FontInfo *> notSupportedFonts_;
      CArray<FontInfo *, FontInfo *> m_lstNoTrueTypeFonts_;


   };
}

#endif
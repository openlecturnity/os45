#ifndef __SGML__
#define __SGML__

#ifndef WIN32
#  pragma warning (disable: 4786)
#  include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <afxtempl.h>

#include "..\ProgressDialog.h"

namespace ASSISTANT
{
   class Attribute
   {
   public: 
      CString
         Name,
         Value;
      
      Attribute();
      ~Attribute();
   };
   
   class SGMLElement
   {
   public:
      CString
         Name,
         Parameter;
      int 
         parameterLength;
      CArray<Attribute *, Attribute *>
         attributes;
      bool
         closed;
      SGMLElement
         *parent, *son,
         *prev, *next;
      
      SGMLElement();
      ~SGMLElement();
      void ParseString(CString &strn);
      LPTSTR GetAttribute(LPCSTR att);
      void AddParameter(CString &str, int length);
      int verify(CString &strn);
   };
   
   class SGMLTree
   {
   public:
      enum FileType {
         FILE_TYPE_ASCII,
            FILE_TYPE_UNICODE,
            FILE_TYPE_UTF8
      };
      
   public:
      SGMLElement
         *root;
      _TCHAR 
         *filename;
      FileType 
         m_iFileType;
      
      
      SGMLTree(CString &sfilename);
      ~SGMLTree();
      int Buildtree(int &pages, CProgressDialogA *pProgress = NULL, bool bOnlyInformation = false);
      SGMLElement *EvaluateTag(SGMLElement *hilf, CString &str);
      
   private:
      void CopyToTchar(BYTE *buffer, UINT nBufLen, TCHAR *tszString, UINT nTcharLen);
      int ReadTcharFromFile(FILE *fp, TCHAR &tcChar);
      
   };
   
};

#endif

#include "StdAfx.h"

#include "resource.h"
#include "SgmlParser.h"

#define BLANK 32

Attribute::Attribute() 
{
   Name.Empty();
   Value.Empty();
}

Attribute::~Attribute() 
{
  if (!Name.IsEmpty()) Name.Empty();
  if (!Value.IsEmpty()) Value.Empty();
}

SGMLElement::SGMLElement()
{
   attributes.RemoveAll();
   Name.Empty();
   Parameter.Empty();
   parameterLength = 0;
   closed     = false; 
   parent     = NULL;
   son        = NULL;
   prev       = NULL;
   next       = NULL;
}

SGMLElement::~SGMLElement()
{
   Name.Empty();
   Parameter.Empty();

   if (attributes.GetSize() > 0) 
   {
      for (int i = 0; i < attributes.GetSize(); ++i)
      {
         delete attributes[i];
      }
      attributes.RemoveAll();
   }
}


void SGMLElement::ParseString(CString &strn)
{
   Attribute
      *att;
   int 
      i, j;

   int iPos1 = 0;
   int iPos2 = strn.Find(_T(' '), iPos1);
   if (iPos2 == -1) 
   {
      iPos2 = strn.GetLength();
   }
   Name = strn.Mid(iPos1,(iPos2-iPos1));

   att = NULL;
   for (i = 0; i < strn.GetLength(); ++i)
   {
      if (strn[i] == _T('\\'))
         ++i;

      if (strn[i] == _T(' '))
      {
         att = new Attribute();
         attributes.Add(att);

         ++i;
         for (j = i; j < strn.GetLength(); j++)
         {
            if (strn[j] == _T('='))
               break;
            if (strn[j] == _T('\\'))
               ++j;
            att->Name += strn[j];
         }
         i = j;
      }
      if (att && strn[i] == _T('\"'))
      {
         ++i;
         for (j = i; j < strn.GetLength(); j++)
         {
            if (strn[j] == _T('\"'))
               break;
            if (strn[j] == _T('\\'))
               ++j;
            att->Value += strn[j];
         }
         if (att->Name == _T("length")) 
         {
            parameterLength = _ttoi(att->Value);
         }
         i = j;
      }
   }

}

LPTSTR SGMLElement::GetAttribute(LPCSTR att) 
{
   for (int i = 0; i < attributes.GetSize(); ++i)
   {
      if (attributes[i]->Name == att)
      {
         if (attributes[i]->Value.GetLength() == 0)
            return NULL;
         else
            return (LPTSTR)(LPCTSTR)attributes[i]->Value;
      }
   }
   
   return NULL;
}

int SGMLElement::verify(CString &strn)
{
  int i = 0;
  if (strn[i] == _T('/'))
     ++i;

  CString tmp = strn;
  tmp = strn.Right(strn.GetLength() - i);

  if (Name != tmp) 
  {
    return 0;
  }

  return 1;
}

SGMLTree::SGMLTree(CString &sfilename)
{
  root = NULL;
  filename = _tcsdup(sfilename);
  m_iFileType = FILE_TYPE_ASCII;
}

SGMLTree::~SGMLTree()
{
   SGMLElement
      *tmp;

   free (filename);

   if (root)
   {
      tmp = root;

      while (tmp->next) 
         tmp = tmp->next;

      while (tmp) 
      {
         if (tmp->son) 
         {
            tmp = tmp->son; 
            while (tmp->next) 
	         tmp = tmp->next;
         }
         else 
         {
            if (tmp->prev) 
            {
	            tmp = tmp->prev;
	            delete tmp->next;
	            tmp->next = NULL;
            }
            else if (tmp->parent) 
            {
	            tmp = tmp->parent;
	            delete tmp->son;
	            tmp->son = NULL;
            }
            else 
            {
	            delete tmp;
	            tmp = NULL;
	            root = NULL;
            }
         }
      }
   }
}

SGMLElement *SGMLTree::EvaluateTag(SGMLElement *hilf, CString &str)
{
   CString
      zw;

   zw = str;

   if (hilf) 
   {
      if (zw[0] == _T('/')) 
      {
         if (hilf->closed) 
         {
	         while (hilf->prev) hilf = hilf->prev;
	         if (hilf->parent) 
            {
	            hilf = hilf->parent;
	         }
            else 
            {
               // ERROR
            }
         }
         int iResult = hilf->verify(zw);
         /*/
         if (iResult == 0)
            ERROR;
         /*/
         hilf->closed = true;
      }
      else 
      {
         if (hilf->closed) 
         {
	         hilf->next = new SGMLElement();
	         hilf->next->prev = hilf;
	         hilf->next->parent = hilf->parent;
	         hilf = hilf->next;
         }
         else 
         {
	         hilf->son = new SGMLElement();
	         hilf->son->parent = hilf;
	         hilf = hilf->son;
         }
         hilf->ParseString(zw);
      }
   }
   else 
   {
      hilf = new SGMLElement();
      root = hilf;
      hilf->ParseString(zw);
   }

   zw.Empty();

   return hilf;
}
	
void SGMLTree::CopyToTchar(BYTE *buffer, UINT nBufLen, TCHAR *tszString, UINT nTcharLen)
{
#ifdef _UNICODE
   if (m_iFileType == FILE_TYPE_ASCII)
   {
      //if (nTcharLen > nBufLen)
      //   MessageBox(NULL, _T("CopyToTchar"), _T("nTcharLen > nBufLen"), MB_OK);
      MultiByteToWideChar(CP_ACP, 0, (char *)buffer, -1, tszString, nTcharLen+1);
   }
   else
      _tcsncpy(tszString, (TCHAR *)buffer, nTcharLen);
#else
   if (m_iFileType == FILE_TYPE_UNICODE)
   {
      //if (nTcharLen > nBufLen)
      //   MessageBox(NULL, "CopyToTchar", "nTcharLen > nBufLen", MB_OK);
      WideCharToMultiByte(CP_ACP, 0, (WCHAR *)buffer, -1, tszString, nTcharLen+1, NULL, NULL);
   }
   else
      _tcsncpy(tszString, (TCHAR *)buffer, nTcharLen);
#endif
}

int SGMLTree::ReadTcharFromFile(FILE *fp, TCHAR &tcChar)
{
   size_t bytesRead = 0;
   if (m_iFileType == FILE_TYPE_ASCII)
   {
      char chr[2];
      bytesRead = fread(chr, sizeof(char), 1, fp);
      
      if (bytesRead > 0)
      {
#ifdef _UNICODE
         MultiByteToWideChar(CP_ACP, 0, chr, 1, &tcChar, 1);
#else
         tcChar = chr[0];
#endif
      }
   }
   else
   {
      WCHAR chr[2];
      bytesRead = fread(chr, sizeof(WCHAR), 1, fp);
      
      if (bytesRead > 0)
      {
#ifdef _UNICODE
         tcChar = chr[0];
#else
         WideCharToMultiByte(CP_ACP, 0, chr, 1, &tcChar, 1, NULL, NULL);
#endif
      }
   }
   
   return bytesRead;
}

int SGMLTree::Buildtree(int &pages)
{
   SGMLElement
      *hilf,
      *hilf2;
   FILE
      *datei;
   long
      abs_count, last_value, count;
   int
      i, j,
      ident,
      actualValue,
      lastValue;
   float
      progressStep;
   CString
      str;
   BYTE
      buffer[22];
   TCHAR 
      head[11];
   
   pages = 0;

   datei = _tfopen(filename, _T("rb"));
   if (datei == NULL) 
   { 
      //CString csMessage;
      //csMessage.Format(IDS_ERROR_FILE_OPEN, filename);
      //MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      return -1;
   }
   
   UINT nBufLen = 22;
   fread(buffer, 1, nBufLen, datei);

   if (buffer[0] == 0xFF && buffer[1] == 0xFE)
      m_iFileType = FILE_TYPE_UNICODE;

   int iOffset = 0;
   if (m_iFileType == FILE_TYPE_UNICODE)
      iOffset = 2;

   CopyToTchar(buffer+iOffset, nBufLen-iOffset, head, 10);

   if (_tcsncmp(head, _T("<MLB"), 4) != 0)  
   { 
	   if (_tcsncmp(head, _T("<ASSISTANT"), 10) != 0) 
      {
         //CString csMessage;
         //csMessage.Format(IDS_ERROR_LEGAL_DOCUMENT, filename);
         //MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
			return -1;
	   }
   }
   
   fseek(datei,0,SEEK_END);  
   abs_count = ftell(datei);
   fseek(datei,0,SEEK_SET);
   last_value = 0;
   progressStep = abs_count/50;
   lastValue = 0;
   
   root = NULL;
   hilf = NULL;
     
   count = 0;
   str.Empty();
   ident = 1;
   i = 0;
   TCHAR tcChar;
   while (ReadTcharFromFile(datei, tcChar)) 
   {
      actualValue = (int)((float)count / progressStep);
      if (actualValue > lastValue) 
      {
         lastValue = actualValue;
      }

      if ((tcChar == _T('\\')) && (ident==0)) 
      {
         str += tcChar;
         ReadTcharFromFile(datei, tcChar);
         str += tcChar;
         i++;
      }
      else if ((tcChar == _T('<')) && (ident==1)) 
      {
         ident = 0;
         str.Empty();
         i = 0;
      }
      else if ((tcChar == _T('>')) && (ident==0)) 
      {
         ident = 1;
         hilf2 = hilf;
         hilf = EvaluateTag(hilf2, str);
         if (hilf->closed && hilf->Name == _T("PAGE")) pages++;
      }
      else if ((tcChar == _T('"')) && (ident==1)) 
      {
         ident = 2;
         str.Empty();
         for (j=0; j<hilf->parameterLength; j++) 
         {
	         ReadTcharFromFile(datei, tcChar);
      	   hilf->Parameter += tcChar;
         }
         ReadTcharFromFile(datei, tcChar);
         if (tcChar != _T('"'))
         {
            //CString csParseError;
            //csParseError.Format(IDS_PARSE_ERROR, hilf->Parameter);
            //MessageBox(NULL, csParseError, NULL, MB_OK | MB_TOPMOST);
         }
         ident = 1;
         i = 0;
      }
      else if ((ident==0) || (ident==2))
      {
         str += tcChar;
         i++;
      }
      count++;
   }
   
   str.Empty();
   fclose(datei);

   return 1;
}            

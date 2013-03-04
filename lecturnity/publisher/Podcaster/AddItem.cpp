// AddItem.cpp: implementation of the CAddItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "AddItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAddItem::CAddItem()
{
   ::CoInitialize(NULL);
}

CAddItem::~CAddItem()
{
   ::CoUninitialize();
}

BOOL CAddItem::MoveOnFront(LPCTSTR tstrXmlFileOut) 
{
	// TODO: Add your control notification handler code here
   char *buffer = NULL;
   DWORD dwSize = 0, dwTotal = 0, dwChunk = 0;
   
   char *first_item = NULL;
   char *last_item = NULL;
   char *endof_last_item = NULL;

   const char *szBegTag = "<item>";
   const char *szEndTag = "</item>";
   const char *szSeparator = "\n  ";

   CString srcFileName = tstrXmlFileOut;
   CString dstFileName = tstrXmlFileOut;
   CFile src, dst;

   // read te entire file in memory
   if(src.Open(srcFileName,  CFile::modeRead, NULL))
   {
    
      dwSize = src.GetLength();
      buffer = new char[dwSize];
      src.Read((void *)buffer, dwSize);
      src.Close();
   }
   else
   {
      return FALSE; //error oppening
   }

	// parse buffer
   first_item = strstr(buffer, szBegTag);
   if(first_item == NULL)
   {
      delete buffer;
      return FALSE; // normal it is not possible ????
   }

   // find last item
   last_item = strstr(first_item + strlen(szBegTag), szBegTag);
   if(last_item == NULL)
   {
      delete buffer;
      return TRUE; // no others items, it ok
   }
   
   // serach if they are more items
   char *tmp = last_item;
   while(TRUE)
   {
      last_item = strstr(tmp + strlen(szBegTag), szBegTag);
      if(last_item == NULL)
      {
         last_item = tmp;
         break;
      }
      tmp = last_item;
   }

   // find end of item
   endof_last_item = strstr(last_item, szEndTag);
   if(last_item == NULL)
   {
      delete buffer;
      return FALSE; // also not possible no end of item found
   } 
   endof_last_item += strlen(szEndTag);

   // read te entire file in memory
   if(dst.Open(dstFileName,  CFile::modeCreate | CFile::modeWrite, NULL))
   { 
      // first chunk
      dwChunk = first_item - buffer ;
      dst.Write((void *)buffer, dwChunk);
      dwTotal += dwChunk;

      // last item
      dwChunk = endof_last_item - last_item;
      dst.Write((void *)last_item, dwChunk);
      dwTotal += dwChunk;

      dst.Write((LPCSTR)szSeparator, strlen(szSeparator));

      // body
      dwChunk = last_item - first_item;
      dst.Write((void *)first_item, dwChunk);
      dwTotal += dwChunk;

      // rest of file
      dwChunk = dwSize - dwTotal;
      char *from = endof_last_item;
      if(dwChunk > 0)
      {
         dst.Write((void *)from, dwChunk);
      }
 
      dst.Close();
      delete buffer;
   }
   else
   {
      delete buffer;
      return FALSE; // can not write to file
   }
   
   return TRUE;
}

BOOL CAddItem::AddAttribute(MSXML2::IXMLDOMNodePtr item, LPCTSTR tstrAttributeName, LPCTSTR tstrAttributeText)
{
   BOOL bResult = FALSE;
   MSXML2::IXMLDOMNodePtr attribute = docPtr->createAttribute(tstrAttributeName); 
   if(attribute != NULL)
   {
      _variant_t varText(tstrAttributeText);
      hr = attribute->put_nodeValue(varText);
      if(SUCCEEDED(hr))
      {
         hr = item->attributes->setNamedItem(attribute);
         bResult = SUCCEEDED(hr);
      }
   }
   return bResult;
}

BOOL CAddItem::AddElement(MSXML2::IXMLDOMNodePtr item, LPCTSTR tstrTagName, LPCTSTR tstrTagText)
{
   BOOL bResult = FALSE;
   MSXML2::IXMLDOMNodePtr element = docPtr->createElement(tstrTagName);    
   if(element != NULL)
   {
      MSXML2::IXMLDOMNodePtr elementText = docPtr->createTextNode(tstrTagText);
      if(elementText != NULL)
      {
         hr = element->appendChild(elementText);
         if(SUCCEEDED(hr))
         {  
            hr = item->appendChild(element);
            bResult = SUCCEEDED(hr);
         }
      }
   }
   return bResult;
}

BOOL CAddItem::AddElementAndAttributes(MSXML2::IXMLDOMNodePtr item, LPCTSTR tstrTagName, LPCTSTR tstrTagText)
{
   BOOL bResult = FALSE;
   MSXML2::IXMLDOMNodePtr element = docPtr->createElement(tstrTagName);    
   if(element != NULL)
   {
      MSXML2::IXMLDOMNodePtr elementText = docPtr->createTextNode(tstrTagText);
      if(elementText != NULL)
      {
         element->appendChild(elementText);
         
         bResult  = AddAttribute(element, _T("length"), csLength);
         bResult &= AddAttribute(element, _T("type"),   csType);
         bResult &= AddAttribute(element, _T("url"),    csUrl);
   
         if(bResult)
         {
            hr = item->appendChild(element);
            bResult = SUCCEEDED(hr);
         }
      }
   }
   return bResult;
}

BOOL CAddItem::AddSeparator(MSXML2::IXMLDOMNodePtr node, LPCTSTR strSeparator = _T("\n   "))
{
   BOOL bResult = FALSE;
   MSXML2::IXMLDOMNodePtr elementText = docPtr->createTextNode(strSeparator);
   hr = node->appendChild(elementText);
   bResult = SUCCEEDED(hr);
   return bResult;
}

BOOL CAddItem::Insert(LPCTSTR tstrXmlFileInp, LPCTSTR tstrXmlFileOut)
{
   BOOL bResult = FALSE;

   try
   {
      //init
      hr = docPtr.CreateInstance(_T("msxml2.domdocument"));
      if(!SUCCEEDED(hr))
      {
         return bResult;
      }
      docPtr->async = FALSE;
      docPtr->preserveWhiteSpace = FALSE;
      // load a document
      _variant_t varXml(tstrXmlFileInp);
      _variant_t varOut((bool)TRUE);
      if(docPtr->load(varXml) == FALSE)
      {
         ErrorMessage();
         return bResult;
      }

      root = docPtr->documentElement->selectSingleNode(_T("channel"));
      if(root == NULL)
      {
         return bResult;
      }
      AddSeparator(root, _T("\n  "));

      // build tree
      MSXML2::IXMLDOMNodePtr item = docPtr->createElement(_T("item"));
      if(item == NULL)
      {
         return FALSE;
      }      
      
      AddSeparator(item);
      // build title
      bResult = AddElement(item, _T("title"), csTitle);    
      AddSeparator(item);
      // build author
      bResult &= AddElement(item, _T("itunes:author"), csAuthor);
      AddSeparator(item);
      // build description
      bResult &= AddElement(item, _T("description"), _T(""));
      AddSeparator(item);
      // build enclosure
      bResult &= AddElementAndAttributes(item, _T("enclosure"), _T(""));
      AddSeparator(item);
      // build pubdate
      bResult &= AddElement(item, _T("pubDate"), csDate);
      AddSeparator(item, _T("\n  "));
      
      if(bResult)
      {
         hr = root->appendChild(item);
         if(!SUCCEEDED(hr))
         {
            return FALSE;
         }
         AddSeparator(root, _T("\n "));
   
         _variant_t varXmlFileOut(tstrXmlFileOut);
         docPtr->documentElement->normalize();
         hr = docPtr->save(varXmlFileOut);
         
         if(SUCCEEDED(hr))
         {
            bResult = MoveOnFront(tstrXmlFileOut);
         }
         else
         {
            bResult = FALSE;
         }
         
         // error message
         if(bResult == FALSE)
         {
            ErrorMessage();
         }
      }
   }
   catch(...)
   {
      ErrorMessage();
      bResult = FALSE;
   }

   return bResult;

}

void CAddItem::ErrorMessage()
{
   CString csFormat, csErrorMessage;
   MSXML2::IXMLDOMParseErrorPtr errPtr = docPtr->GetparseError();
   if(errPtr->line == 0)
   {
      csErrorMessage.LoadString(IDS_ERRPARSE);
   }
   else
   {
      _bstr_t bstrErr(errPtr->reason);         
      csFormat.LoadString(IDS_ERRPARSE2);
      csErrorMessage.Format(csFormat,
         errPtr->errorCode, errPtr->line, errPtr->linepos, (char*)bstrErr);
   }
   AfxMessageBox(csErrorMessage, MB_ICONSTOP);
}

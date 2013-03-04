// SetMetadata.cpp: implementation of the CSetMetadata class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "podcaster.h"
#include "SetMetadata.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSetMetadata::CSetMetadata()
{
   csTitle       = _T("??");
   csSubtitle    = _T("??");
	csAuthor      = _T("??");
   csDescription = _T("??");
	csSummary     = _T("??");
  	csLanguage    = _T("??");
	csCopyright   = _T("??");
  	csOwner       = _T("??");
	csEMail       = _T("??");
  	csLink        = _T("??");
	csWebMaster   = _T("??");
  	csExplicit    = _T("??");
	csKeywords    = _T("??");
}

CSetMetadata::~CSetMetadata()
{

}

void CSetMetadata::SetElement(LPTSTR strElement, LPCTSTR strValue)
{
   MSXML2::IXMLDOMNodeListPtr pIDOMNodeList;
   MSXML2::IXMLDOMNodePtr pIDOMNode;
   
   long value;

   pIDOMNodeList = docPtr->getElementsByTagName(strElement);
   hr = pIDOMNodeList->get_length(&value);
   if(SUCCEEDED(hr))
   {
      pIDOMNodeList->reset();
      for(int ii = 0; ii < value; ii++)
      {
         pIDOMNodeList->get_item(ii, &pIDOMNode);
         // only the first node should be modified
         pIDOMNodeList->get_item(ii, &pIDOMNode);
         if(pIDOMNode)
         {
            _variant_t v(strValue);
            pIDOMNode->put_text(v.bstrVal);
            return;
         }
      }
   }
}

BOOL CSetMetadata::Parse(LPCTSTR tstrXmlFile, LPCTSTR tstrXmlTmpFile)
{
   BOOL bResult = FALSE;
   
   try
   {
      //init
      hr = docPtr.CreateInstance(_T("msxml2.domdocument"));
      if(!SUCCEEDED(hr))
      {
         return FALSE;
      }
      
      docPtr->async = FALSE;
      // load a document
      _variant_t varXml(tstrXmlFile);
      _variant_t varOut((bool)TRUE);
      
      if(docPtr->load(varXml) == FALSE)
      {
         ErrorMessage();
         return bResult;
      }

      SetElement(_T("title"), csTitle);
      SetElement(_T("itunes:subtitle"), csSubtitle);
      SetElement(_T("itunes:author"), csAuthor);
      SetElement(_T("description"), csDescription);
      SetElement(_T("itunes:summary"), csSummary);
      SetElement(_T("language"), csLanguage);
      SetElement(_T("copyright"), csCopyright);
      SetElement(_T("itunes:name"), csOwner);
      SetElement(_T("itunes:email"), csEMail);
      SetElement(_T("link"), csLink);
      SetElement(_T("webMaster"), csWebMaster);
      SetElement(_T("itunes:explicit"), csExplicit);
      SetElement(_T("itunes:keywords"), csKeywords);

      _variant_t varXmlFileOut(tstrXmlTmpFile);
      docPtr->documentElement->normalize();
      hr = docPtr->save(varXmlFileOut);
      bResult = SUCCEEDED(hr);

   }
   catch(...)
   {
      ErrorMessage();
      bResult = FALSE;
   }

   return bResult;
}

void CSetMetadata::ErrorMessage()
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

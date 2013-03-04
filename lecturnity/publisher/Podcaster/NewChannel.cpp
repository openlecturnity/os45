// NewChannel.cpp: implementation of the CNewChannel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "NewChannel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNewChannel::CNewChannel()
{
}

CNewChannel::~CNewChannel()
{
}

BOOL CNewChannel::AddAttribute(MSXML2::IXMLDOMNodePtr item, LPCTSTR strAttributeName, LPCTSTR tstrAttributeText)
{
   BOOL bResult = FALSE;
   MSXML2::IXMLDOMNodePtr attribute = docPtr->createAttribute(strAttributeName); 
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

BOOL CNewChannel::AddElement(MSXML2::IXMLDOMNodePtr item, LPCTSTR strTagName, LPCTSTR tstrTagText)
{
   BOOL bResult = FALSE;
   element = docPtr->createElement(strTagName);    
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

BOOL CNewChannel::AddSeparator(MSXML2::IXMLDOMNodePtr node, LPCTSTR tstrSeparator = _T("\n "))
{
   BOOL bResult = FALSE;
   MSXML2::IXMLDOMNodePtr elementText = docPtr->createTextNode(tstrSeparator);
   hr = node->appendChild(elementText);
   bResult = SUCCEEDED(hr);
   return bResult;
}

BOOL CNewChannel::Create(LPCTSTR tstrXmlFile)
{
   BOOL bResult = FALSE;
   CTime date = CTime::GetCurrentTime();
   CString csDate = date.Format(_T("%a, %d %b %Y %H:%M:%S +100"));

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

      // process instruction
      pi = docPtr->createProcessingInstruction(_T("xml"), _T("version=\"1.0\" encoding=\"UTF-8\""));
      hr = docPtr->appendChild(pi);
      if(!SUCCEEDED(hr))
      {
         return FALSE;
      }

      //rss
      root = docPtr->createElement(_T("rss"));    
      if(root == NULL)
      {
         return FALSE;
      }
      hr = docPtr->appendChild(root);
      if(!SUCCEEDED(hr))
      {
         return FALSE;
      }

      // root attributes
      AddAttribute(root, _T("version"), _T("2.0"));
      AddAttribute(root, _T("xmlns:content"), _T("http://purl.org/rss/1.0/modules/content/"));
      AddAttribute(root, _T("xmlns:dc"), _T("http://purl.org/dc/elements/1.1/"));
      AddAttribute(root, _T("xmlns:itunes"), _T("http://www.itunes.com/DTDs/Podcast-1.0.dtd"));
      AddAttribute(root, _T("xmlns:wfw"), _T("http://wellformedweb.org/CommentAPI/"));
      AddSeparator(root);

      // channel
      MSXML2::IXMLDOMNodePtr channel = docPtr->createElement(_T("channel"));    
      if(channel == NULL)
      {
         return FALSE;
      }
      hr = root->appendChild(channel);
      if(!SUCCEEDED(hr))
      {
         return FALSE;
      }    
      AddSeparator(channel, _T("\n  "));     
      
      AddElement(channel, _T("title"), m_csTitle);
      AddSeparator(channel, _T("\n  "));     
      AddElement(channel, _T("link"), m_csLink);
      AddSeparator(channel, _T("\n  "));
      AddElement(channel, _T("description"), m_csDescription);
      AddSeparator(channel, _T("\n  "));
      AddElement(channel, _T("itunes:author"), m_csAutor);      
      AddSeparator(channel, _T("\n  "));
      AddElement(channel, _T("language"), m_csLanguage);      
      AddSeparator(channel, _T("\n  "));
      AddElement(channel, _T("copyright"), m_csCopyright);      
      AddSeparator(channel, _T("\n  "));
      AddElement(channel, _T("pubDate"), csDate);      
      AddSeparator(channel, _T("\n  "));
      AddElement(channel, _T("lastBuildDate"), csDate);      
      AddSeparator(channel, _T("\n  "));
      AddElement(channel, _T("itunes:subtitle"), m_csSubtitle);      
      AddSeparator(channel, _T("\n  "));
      AddElement(channel, _T("itunes:summary"), m_csSummary);      
      AddSeparator(channel, _T("\n  "));
      
      AddElement(channel, _T("itunes:link"), m_csLink);      
      AddAttribute(element, _T("href"),  _T(""));
      AddAttribute(element, _T("rel"),  _T(""));
      AddAttribute(element, _T("type"), _T(""));
      AddSeparator(channel, _T("\n  "));
      
      AddElement(channel, _T("webMaster"), m_csWebmaster);      
      AddSeparator(channel, _T("\n  "));

      // owner
      MSXML2::IXMLDOMNodePtr owner = docPtr->createElement(_T("owner"));    
      if(owner == NULL)
      {
         return FALSE;
      }
      hr = channel->appendChild(owner);
      if(!SUCCEEDED(hr))
      {
         return FALSE;
      }
      AddSeparator(owner, _T("\n   "));

      AddElement(owner, _T("itunes:name"), m_csOwner);      
      AddSeparator(owner, _T("\n   "));
      AddElement(owner, _T("itunes:email"), m_csEMail);      
      AddSeparator(owner, _T("\n  "));

      // image
      AddSeparator(channel, _T("\n  "));
      MSXML2::IXMLDOMNodePtr image = docPtr->createElement(_T("image"));    
      if(image == NULL)
      {
         return FALSE;
      }
      hr = channel->appendChild(image);
      if(!SUCCEEDED(hr))
      {
         return FALSE;
      }     
      AddSeparator(image, _T("\n   "));
      
      AddElement(image, _T("url"), _T(""));      
      AddSeparator(image, _T("\n   "));
      AddElement(image, _T("title"), _T(""));      
      AddSeparator(image, _T("\n   "));
      AddElement(image, _T("link"), _T(""));      
      AddSeparator(image, _T("\n  "));

      AddElement(channel, _T("itunes:explicit"), m_csExplicit);      
      AddSeparator(channel, _T("\n  "));
      AddElement(channel, _T("itunes:keywords"), m_csKeyword);      
      AddSeparator(channel, _T("\n "));
               
      AddSeparator(root, _T("\n"));

      _variant_t varXmlFile(tstrXmlFile);
      docPtr->documentElement->normalize();
      hr = docPtr->save(varXmlFile);
      bResult = SUCCEEDED(hr);

   }
   catch(...)
   {
      ErrorMessage();
      bResult = FALSE;
   }

   return bResult;

}

void CNewChannel::ErrorMessage()
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

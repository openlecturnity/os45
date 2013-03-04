#ifndef __CHANGEURLCMD_H__CB5B5D80_53DF_43e0_B605_AA71477CBB77
#define __CHANGEURLCMD_H__CB5B5D80_53DF_43e0_B605_AA71477CBB77

#include "changeurlengine.h"

class CChangeUrlCmd : public CChangeUrlEngine
{
private:
   CString m_csUrlHttp;
   CString m_csUrlStreaming;
   CString m_csDocumentName;
   CString m_csSmilFileName;

   UINT    m_nTargetType;

public:
   CChangeUrlCmd() : CChangeUrlEngine() { }
   virtual ~CChangeUrlCmd() { }

   virtual CString GetDocumentName() { return m_csDocumentName; }
   virtual void SetDocumentName(LPCTSTR tszDocName) { m_csDocumentName = tszDocName; }
   virtual CString GetUrlHttp() { return m_csUrlHttp; }
   virtual void SetUrlHttp(LPCTSTR tszUrlHttp) { m_csUrlHttp = tszUrlHttp; }
   virtual CString GetUrlStreaming() { return m_csUrlStreaming; }
   virtual void SetUrlStreaming(LPCTSTR tszUrlStreaming) { m_csUrlStreaming = tszUrlStreaming; }
   virtual void SetSmilFileName(LPCTSTR tszSmilFileName);
   virtual bool RequestSmilFileName(CString &csSmilFileName);

   virtual UINT GetTargetType() { return m_nTargetType; }
   virtual void SetTargetType(UINT nTargetType) { m_nTargetType = nTargetType; }

   virtual void LogMessage(CString csMsg, UINT nBeep = 0);
   virtual bool QuestionMessage(CString csMsg);
private:
   void ReplaceSpecialCharactersInString(CString &csMsg);
};


#endif // __CHANGEURLCMD_H__CB5B5D80_53DF_43e0_B605_AA71477CBB77
#ifndef __CHANGEURLENGINE_H__DAD36A23_14F4_4f12_A856_C3C834C362C5
#define __CHANGEURLENGINE_H__DAD36A23_14F4_4f12_A856_C3C834C362C5

#include "ZipProgressDlg.h"

#define DOCTYPE_WMT 0
#define DOCTYPE_REAL 1
#define DOCTYPE_FLASH 2

#define REPLAY_TYPE_LOCAL 0
#define REPLAY_TYPE_HTTP 1
#define REPLAY_TYPE_STREAMING 2

class CChangeUrlEngine
{
public:
   CChangeUrlEngine();
   virtual ~CChangeUrlEngine();

   virtual void DisableControls() {}
   virtual void EnableStep3(BOOL bEnable=TRUE) {}
   virtual void EnableStep2(BOOL bEnable=TRUE, UINT nReplayType=REPLAY_TYPE_LOCAL) {}

   virtual CString GetDocumentName()=0;
   virtual void SetDocumentName(LPCTSTR tszDocName)=0;
   virtual CString GetUrlHttp()=0;
   virtual void SetUrlHttp(LPCTSTR tszUrlHttp) {};
   virtual CString GetUrlStreaming()=0;
   virtual void SetUrlStreaming(LPCTSTR tszUrlStreaming) {};
   virtual void SetDocType(LPCTSTR tszDocType) {}
   virtual void SetDocVersion(LPCTSTR tszDocVersion) {}
   virtual void SetScormSupport(LPCTSTR tszScormSupport) {}
   virtual void SetSmilFileName(LPCTSTR tszSmilFileName) { m_csSmilFileName = tszSmilFileName; }
   virtual bool RequestSmilFileName(CString &csSmilFileName) { return false; }
   virtual void SetWaitCursor(bool bDisplay) {}
   virtual CWnd *GetCWnd() { return NULL; }

   virtual UINT GetTargetType()=0;

   bool ResolveHtmlFileNames();
   bool ResolveDocumentNames(const _TCHAR *tszPath, const _TCHAR *tszBaseName);
   bool RetrieveCurrentType();
   bool CheckScormPackage(CString &csPath, CString &csFileName);
   void CleanupPrevious();
   void RequestZipCancel();

   bool UpdateUrls();
   bool MarkDocumentAsUpdated();

   virtual void LogMessage(UINT nID, UINT nBeep = 0);
   virtual void LogMessage(UINT nID, LPCTSTR tszParam, UINT nBeep = 0);
   virtual void LogMessage(CString csMsg, UINT nBeep = 0)=0;
   virtual bool QuestionMessage(UINT nID);
   virtual bool QuestionMessage(CString csMsg)=0;

   // The path to the HTML/SMI/js files, including a backslash
   CString m_csPath;
   // The base name of the document, excluding any extensions or additions
   CString m_csBaseName;
   // The base name of the document containing the master player
   CString m_csMasterPlayerDocument;
   // The base name of the document containing the clip players (if applicable)
   CString m_csClipPlayersDocument;
   // The file name (complete) of the SMIL document (if applicable)
   CString m_csSmilFileName;
   // The temporary directory used for the SCORM package (if applicable)
   CString m_csScormTempDir;
   // True if a temporary directory was used.
   bool m_bTempDirUsed;

   bool m_bOldStyleDocument; // false: pre 1.6.1.p1 document
   UINT m_nDocType; // DOCTYPE_WMT/DOCTYPE_REAL
   UINT m_nOriginalReplayType; // REPLAY_TYPE_LOCAL/HTTP/STREAMING
   bool m_bHasClips; // isClipsSeparated
   UINT m_nNumberOfClips; // numOfClips
   bool m_bIsStandaloneMode; // g_isStandaloneMode
   CString m_csDocVersion; // Version of LECTURNITY document
   bool m_bVersionKnown;

private:
   void CheckFileForPlugins(const _TCHAR *tszFileName, const _TCHAR *tszPath);
   static UINT ExtractZipThread(LPVOID pParam);
   void ExtractZip();
   static UINT CreateZipThread(LPVOID pParam);
   void CreateZip();
   bool m_bThreadSuccess;
   bool m_bThreadCancelRequest;
   CString m_csThreadTmpPath;
   CString m_csThreadFileName;

   CZipProgressDlg *m_pProgressDlg;
   void WaitForProgressDlg();
};

#endif // __CHANGEURLENGINE_H__DAD36A23_14F4_4f12_A856_C3C834C362C5
#pragma once

#include "Uploader.h" 


struct UploadThreadParam
{
   TransferParam tp;
   CString csSrc;
   CString csDest;
   CArray<CString, CString> aFiles;
   bool bDelete;
   double *dProgress;
   int *piCurrFile;
   bool *pbIsCancel;
   bool* pbIsThreadActive;
};

struct PodcastThreadParam
{
   TransferParam MediaTP;
   TransferParam ChannelTP;
   CString csMediaSrc;
   CString csMediaDest;
   CArray<CString, CString> aFiles;
   CString csTitle;
   CString csAuthor;
   bool bDelete;
   double *dProgress;
   int *piCurrFile;
   bool *pbIsCancel;
   bool* pbIsThreadActive;
};

struct YtUploadthreadParam
{
    YtTransferParam yTP;
    double *pdProgress;
    int *piCurrFile;
    bool *pbIsCancel;
    bool *pbIsThreadActive;
    CString csAuthToken;
    CString csFile;
};

struct SavedData
{
   TransferParam tp;
   CString csFileOnServer;
};

class CTransferingContent
{
   CTransferingContent(void);

   ~CTransferingContent(void);
   BOOL AddItem(LPCTSTR srcFileName, CString csItemFileName, CString csItemDirectoryUrl, CString csItemSrc, CString csTitle, CString csAuthor, LPCTSTR dstFileName);
   BOOL IsMediaUrlOk(CString &csURL);
   CString GetFileName(CString csFilePath);
   DWORD GetFileLength(CString csFile);

public:
   CWinThread *m_Thread;
   UploadThreadParam m_utp;
   PodcastThreadParam m_ptp;
   YtUploadthreadParam m_ytp;

   static CTransferingContent& GetInstance();

   HRESULT Upload(TransferParam tp, CString csSrc, CString csDst, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive, bool bDelete = false);
   UINT Podcast(TransferParam MediaTP, TransferParam ChannelTP, CString csMediaSrc, CString csMediaDst, CString csTitle, CString csAuthor, double *dProgress, bool *pbIsCancel, bool *pbIsThreadActive);
   HRESULT YtUpload(YtTransferParam tp, CString csFile, CString csAuthToken, double *pdProgress, bool *pbIsCancel, bool *pbIsThreadActive);
   UINT MyStartUpload(TransferParam tp, CArray<CString, CString>&aUplFiles, double *dProg, int* piCurrFile, bool* pbIsCancel, bool* pbIsThreadActive, bool bDelete = false);
   UINT MyStartPodcast(TransferParam MediaTP, TransferParam ChannelTP, CArray<CString, CString>&aUplFiles, CString csTitle, CString csAuthor, double *dProg, int* piCurrFile, bool* pbIsCancel, bool* pbIsThreadActive);
   UINT MyStartYtUpload(YtTransferParam tp, CString csFile, CString csAuthToken, double *pdProgress, int* piCurrFile, bool *pbIsCancel, bool *pbIsThreadActive);
   HRESULT YtAuthentify(CString csUser, CString csPass, CString &csAuthToken, CString &csAuthUser);
   double GetUploadProgress();
   UINT Cleanup(bool bIsCancelled = true);
   CString GetErrStr(){return m_csErrStr;};
   int GetErrCode(){return m_iErrCode;};
   HRESULT CheckTarget(CString csServer, int iProtocol, int iPort, CString csUser, CString csPassword, CString csTargetDirOrFile, bool* pbIsDir, bool* pbIsEmptyDir);
   BOOL CreateSubDirectory(TransferParam tpTransfer, CString csSubDirName);
   BOOL RemoveSubDirectory(TransferParam tpTransfer, CString csSubDirName);
   void SetParentHwnd(HWND hParentHwnd){m_hParentHwnd = hParentHwnd;};
   HWND GetParentHwnd(){return m_hParentHwnd;};
private:
   double m_dProgress;
   CString m_csErrStr;
   HWND m_hParentHwnd;
   int m_iErrCode;
};

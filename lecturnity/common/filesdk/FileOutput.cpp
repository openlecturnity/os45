#include "stdafx.h"
#include "FileOutput.h"

CFileOutput::CFileOutput()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_hFile = INVALID_HANDLE_VALUE;
   m_bFileOpened = false;
   m_bDoUtf8Bom = false;
}

CFileOutput::~CFileOutput()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Flush();

   if (m_bFileOpened)
      ::CloseHandle(m_hFile);
}

HRESULT CFileOutput::OpenForWrite(CString csFilename, bool bDoUtf8) {

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    HANDLE hFile = CreateFile(csFilename, GENERIC_WRITE, 0, NULL, 
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    hr = Init(hFile, 0, bDoUtf8);
    if (FAILED(hr)) {
        if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
    } else
        m_bFileOpened = true;

    return hr;
}

HRESULT CFileOutput::Init(HANDLE hFile, UINT nIndentLength, bool bDoUtf8)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
      return E_POINTER;

   m_hFile = hFile;
   m_bDoUtf8Bom = bDoUtf8;

   hr = CArrayOutput::Init(4096, nIndentLength, bDoUtf8);

   return hr;
}

HRESULT CFileOutput::Append(BYTE *aBytes, UINT nByteCount)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (aBytes == NULL)
      return E_POINTER;

   if (m_hFile == INVALID_HANDLE_VALUE)
      return E_OUT_UNINITIALIZED;

   if (m_bDoUtf8Bom)
   {
      if (::GetFileSize(m_hFile, NULL) == 0)
      {
         BYTE aBom[3];
         aBom[0] = 0xef;
         aBom[1] = 0xbb;
         aBom[2] = 0xbf;
         CArrayOutput::Append(aBom, 3);
      }

      m_bDoUtf8Bom = false;
   }

   if (nByteCount > CArrayOutput::GetEmptyLength())
      hr = Flush();
   
   if (SUCCEEDED(hr))
   {
      if (nByteCount > CArrayOutput::GetBufferLength())
      {
         // write directly
         Flush();
         
         ULONG nWritten = 0;
         bool bSuccess = ::WriteFile(m_hFile, aBytes, nByteCount, &nWritten, false) != 0;
         if (!bSuccess)
            hr = E_OUT_WRITE_FAILED;
      }
      else
      {
         // copy to buffer
         
         hr = CArrayOutput::Append(aBytes, nByteCount);
      }
   }

   return hr;
}

HRESULT CFileOutput::Append(DrawSdk::DrawObject *pObject, bool bUseFullImagePath)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // make sure there is at least the whole buffer free
   Flush();

   return CArrayOutput::Append(pObject, bUseFullImagePath);
}

HRESULT CFileOutput::Flush()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (m_hFile == INVALID_HANDLE_VALUE)
      return E_OUT_UNINITIALIZED;

   if (CArrayOutput::GetDataLength() > 0)
   {
      ULONG nWritten = 0;
      bool bSuccess = ::WriteFile(m_hFile, CArrayOutput::GetBuffer(), CArrayOutput::GetDataLength(), &nWritten, false) != 0;
      if (!bSuccess)
         hr = E_OUT_WRITE_FAILED;

      CArrayOutput::Reset();
   }

   return hr;
}
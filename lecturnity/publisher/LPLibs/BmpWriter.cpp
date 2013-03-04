#include "stdafx.h"
#include "BmpWriter.h"

CBmpWriter::CBmpWriter(void)
{
}

CBmpWriter::~CBmpWriter(void)
{
}

void EnqueueValue(BYTE *pData, int i)
{
   pData[0] = i & 0xff;
   pData[1] = (i >> 8) & 0xff;
   pData[2] = (i >> 16) & 0xff;
   pData[3] = (i >> 24) & 0xff;
}

void EnqueueValue(BYTE *pData, short s)
{
   pData[0] = s & 0xff;
   pData[1] = (s >> 8) & 0xff;
}


// static
HRESULT CBmpWriter::WriteBmpImage(BYTE *pData, int iDataLen, int iImgWidth, int iImgHeight, const WCHAR *wszTargetFile)
{
   if (pData == NULL || wszTargetFile == NULL)
      return E_POINTER;

   if (iDataLen < iImgWidth*iImgHeight*3) // TODO properly not 24 bit RGB?
      return E_INVALIDARG;

   if (iImgWidth <= 0 || iImgHeight <= 0)
      return E_INVALIDARG;

   HANDLE hFile = ::CreateFileW(wszTargetFile, GENERIC_WRITE, 0, NULL, 
      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

   if (hFile == INVALID_HANDLE_VALUE)
      return E_FAIL;

   BYTE aHeader[54];
   ZeroMemory(aHeader, 54);
   BYTE *pHeaderP = aHeader;

   //
   // BMP header

   EnqueueValue(pHeaderP, (short)0x4d42); // "BM"
   pHeaderP += 2;

   EnqueueValue(pHeaderP, iDataLen + 54); // file size
   pHeaderP += 4;

   EnqueueValue(pHeaderP, 0); // 2x reserved
   pHeaderP += 4;

   EnqueueValue(pHeaderP, 54); // image data offset
   pHeaderP += 4;


   //
   // DIB header

   EnqueueValue(pHeaderP, 40); // dib header size
   pHeaderP += 4;

   EnqueueValue(pHeaderP, iImgWidth);
   pHeaderP += 4;

   EnqueueValue(pHeaderP, iImgHeight);
   pHeaderP += 4;

   EnqueueValue(pHeaderP, (short)1); // number of planes, must be 1
   pHeaderP += 2;

   EnqueueValue(pHeaderP, (short)24); // bit depth
   pHeaderP += 2;

   EnqueueValue(pHeaderP, 0); // compression (uncompressed)
   pHeaderP += 4;

   EnqueueValue(pHeaderP, iDataLen); // raw image size
   pHeaderP += 4;

   // remaining 16 bytes stay "0"; they are 2x resolution and 2x palette information

   DWORD dwBytes = 0;

   ::WriteFile(hFile, aHeader, 54, &dwBytes, NULL);
   ::WriteFile(hFile, pData, iDataLen, &dwBytes, NULL);

   ::CloseHandle(hFile);

   return S_OK;
}
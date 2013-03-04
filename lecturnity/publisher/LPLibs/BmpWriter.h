#pragma once

class CBmpWriter
{
public:
   static HRESULT WriteBmpImage(BYTE *pData, int iDataLen, int iImgWidth, int iImgHeight, const WCHAR *wszTargetFile);

private:
   CBmpWriter(void);
   ~CBmpWriter(void);
};

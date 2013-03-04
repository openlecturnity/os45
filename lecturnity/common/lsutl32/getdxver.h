#include "StdAfx.h"

class LSUTL32_EXPORT DirectXDiag
{
public:
   static HRESULT GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion );
};

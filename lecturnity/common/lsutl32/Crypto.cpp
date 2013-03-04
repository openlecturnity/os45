// Crypto.cpp: implementation of the CCrypto class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Crypto.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrypto::CCrypto()
{
   // build a key
   m_aKey[0] = 0x07;
   m_aKey[1] = 0xba;
   m_aKey[2] = 0xc3;
   m_aKey[3] = 0x2f;
   m_aKey[4] = 0x4a;
   m_aKey[5] = 0x12;
   m_aKey[6] = 0xd8;
   m_aKey[7] = 0x6e;
}

CCrypto::~CCrypto()
{

}

/**
 * Convert a string to his encrypted form: EXOR with key and split to hex characters.
 */
CString CCrypto::Encode(CString csString)
{
   char *szString;
   CString csPassword, csOneByte;
   csPassword.Empty();

#ifdef _UNICODE
   int nLen = ::WideCharToMultiByte(CP_ACP, 0, csString, -1, NULL, 0, NULL, NULL);
   szString = (char*)malloc((nLen + 1) * sizeof(char));
   ::WideCharToMultiByte(CP_ACP, 0, csString, -1, szString, nLen + 1, NULL, NULL);
   szString[nLen] = '\0';
#else
   szString = (char*)malloc((csString.GetLength() + 1) * sizeof(char));
   strcpy(szString, csString);
#endif //_UNICODE

   int iLength = (int)strlen(szString);

   for (int i = 0; i < iLength; i++)
   {
      BYTE e = szString[i] ^ m_aKey[i % 8];   //encrypt char
      csOneByte.Format(_T("%02X"), e);
      csPassword += csOneByte;
   }


   free(szString);
   
   return csPassword;
}

CString CCrypto::Decode(CString csString)
{
   BYTE btHiNibble, btLoNibble, btByte, btNewByte;
   char *szString;
   CString csPassword;
   csPassword.Empty();

#ifdef _UNICODE
   int nLen = ::WideCharToMultiByte(CP_ACP, 0, csString, -1, NULL, 0, NULL, NULL);
   szString = (char*)malloc((nLen + 1) * sizeof(char));
   ::WideCharToMultiByte(CP_ACP, 0, csString, -1, szString, nLen + 1, NULL, NULL);
   szString[nLen] = '\0';
#else
   szString = (char*)malloc((csString.GetLength() + 1) * sizeof(char));
   strcpy(szString, csString);
#endif //_UNICODE

   int iLength = (int)strlen(szString);

   for (int i = 0; i < iLength; i += 2)
   {
      btHiNibble = szString[i] - 0x30;
      if(btHiNibble > 0x09)
      {
         btHiNibble -= 7;
      }

      btLoNibble = szString[i + 1] - 0x30;
      if(btLoNibble > 0x09)
      {
         btLoNibble -= 7;
      }

      // combine to a single byte
      btByte = (btHiNibble << 4) | btLoNibble;

      btNewByte = (btByte ^ m_aKey[(i >> 1) % 8]);   //decrypt
      csPassword += btNewByte;
   }

   free(szString);
   
   return csPassword;
}

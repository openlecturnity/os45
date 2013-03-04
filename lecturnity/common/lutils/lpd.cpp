#include "StdAfx.h"
#include "lutils.h"

#include "zlib.h"
#include <mmsystem.h>


UINT ConvertLeValue(LBuffer *plBuffer, UINT nLength)
{
   BYTE *pHeaderBytes = plBuffer->GetByteAccess();

   UINT nReturn = 0;

   // NO breaks, read carefully:
   switch(nLength)
   {
   case 4:
      nReturn = ((pHeaderBytes[0] << 24) & 0xff000000) | ((pHeaderBytes[1] << 16) & 0xff0000);
      pHeaderBytes += 2; // advance pointer for next step
   case 2:
      nReturn |= ((pHeaderBytes[0] << 8) & 0xff00);
      pHeaderBytes += 1; // advance pointer for next step
   default:
      nReturn |= (pHeaderBytes[0] & 0xff);
   }

   return nReturn;
}

// #################################################################

JavaVariable::JavaVariable()
{
   m_nType = 0;
   m_lValue = 0;
   // m_lstrValue and m_lstrName don't need to be initialized or reset
}

JavaVariable::~JavaVariable()
{
   // nothing to be done; only stack objects used (directly)
}

USHORT JavaVariable::ReadTypeAndName(LIo::InputStream *pStream, JavaVariable *pPrevious)
{
   LBuffer lbHeaderBuffer(2048);
   BYTE *pHeaderBytes = lbHeaderBuffer.GetByteAccess();

   UINT nRead = 0;
   HRESULT hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 1, &nRead);
   m_nType = pHeaderBytes[0];
   // check for valid (expected) type; one of three

   hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // variable name length
   UINT nNameLength = ConvertLeValue(&lbHeaderBuffer, 2);
   hr = pStream->ReadBytes(&lbHeaderBuffer, 0, nNameLength, &nRead); 
   
   char *szVariableName = new char[nNameLength+1];
   ::ZeroMemory(szVariableName, nNameLength+1);
   memcpy(szVariableName, pHeaderBytes, nNameLength);
   m_lstrName.AllocateAndSet(szVariableName);
   delete[] szVariableName;
      
   if (JAVA_ID_NOTPRIMITIVE == m_nType)
   {
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 1, &nRead); // the "second" type
      USHORT nNotPrimitiveType = pHeaderBytes[0];
      
      if (JAVA_ID_SAME_TYPE != nNotPrimitiveType)
      {
         m_nType = nNotPrimitiveType;
         // check for ID_STRING

         // assume that it is a string and discard the type
         hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // variable name length
         UINT nNameLength = ConvertLeValue(&lbHeaderBuffer, 2);
         hr = pStream->ReadBytes(&lbHeaderBuffer, 0, nNameLength, &nRead); 
   
      }
      else
      {
         // this type equals the previous one in the stream
         m_nType = pPrevious->m_nType;
         hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 4, &nRead); // unknown elements
      }
   }

   //_RPT2(_CRT_WARN, "%02x %s\n", m_nType, (LPTSTR)m_lstrName);
   

   return m_nType;
}

HRESULT JavaVariable::WriteTypeAndName(LIo::DataOutputStream *pStream)
{
   HRESULT hr = S_OK;

   // if (NULL == pStream)
   //   return E_POINTER;

   if (JAVA_ID_STRING == m_nType) // non-primitve type
      hr = pStream->WriteByte(JAVA_ID_NOTPRIMITIVE);
   else
      hr = pStream->WriteByte(m_nType);

   if (SUCCEEDED(hr))
      hr = pStream->WriteUtf8WithLength(&m_lstrName);

   if (SUCCEEDED(hr) && JAVA_ID_STRING == m_nType)
   {
      //
      // , JavaVariable *pPrevious
      // different to the reading part above:
      // if the type of a non-primitive variable is the same of the one
      // before it will nevertheless completly specified here
      // 
   
      hr = pStream->WriteByte(m_nType);

      // write string type name
      LString lsStringType(_T("Ljava/lang/String;"));
      if (SUCCEEDED(hr))
         pStream->WriteUtf8WithLength(&lsStringType);

   }

   return hr;
}

/**
 * Currently only supports boolean, int and String!
 */
HRESULT JavaVariable::ReadValue(LIo::InputStream *pStream)
{
   HRESULT hr = S_OK;

   LBuffer lbHeaderBuffer(2048);
   BYTE *pHeaderBytes = lbHeaderBuffer.GetByteAccess();
   UINT nRead = 0;
   UINT nStringByteLength = 0;

   switch(m_nType)
   {
   case JAVA_ID_BOOLEAN:
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 1, &nRead);
      m_lValue = ConvertLeValue(&lbHeaderBuffer, 1);
      break;
   case JAVA_ID_INT:
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 4, &nRead);
      m_lValue = ConvertLeValue(&lbHeaderBuffer, 4);
      break;
   case JAVA_ID_STRING:
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 1, &nRead); // "t"

      if (JAVA_ID_NULL != pHeaderBytes[0])
      {
         hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // the length in bytes
         nStringByteLength = ConvertLeValue(&lbHeaderBuffer, 2);
         hr = pStream->ReadBytes(&lbHeaderBuffer, 0, nStringByteLength, &nRead); // the string content

         m_lstrValue.AllocateAndSetUtf8((char *)pHeaderBytes, nStringByteLength);
      }
      // else string value is null
      break;
   default:
      hr = E_NOTIMPL;
      break;
   }

   return hr;
}

HRESULT JavaVariable::WriteValue(LIo::DataOutputStream *pStream)
{
   HRESULT hr = S_OK;

   //if (NULL == pStream)
   //   return E_POINTER;

   switch(m_nType)
   {
   case JAVA_ID_BOOLEAN:
      hr = pStream->WriteByte(m_lValue);
      break;
   case JAVA_ID_INT:
      hr = pStream->WriteLeInt(m_lValue);
      break;
   case JAVA_ID_STRING:
      if (m_lstrValue.Length() > 0)
      {
         hr = pStream->WriteByte('t');
         if (SUCCEEDED(hr))
            pStream->WriteUtf8WithLength(&m_lstrValue);
      }
      else
         hr = pStream->WriteByte(JAVA_ID_NULL);
      
      break;
   default:
      hr = E_NOTIMPL;
      break;
   }

   return hr;
}



// #################################################################

JavaClassInstance::JavaClassInstance()
{
   m_nVariableCount = 0;
   m_paVariables = NULL;
}

JavaClassInstance::~JavaClassInstance()
{
   m_nVariableCount = 0;
   if (NULL != m_paVariables)
      delete[] m_paVariables;
   m_paVariables = NULL;
}

UINT JavaClassInstance::GetVariableCount()
{
   return m_nVariableCount;
}

HRESULT JavaClassInstance::InitAndReadValues(JavaVariable *paVariables, UINT nVariableCount, LIo::InputStream *pStream)
{
   HRESULT hr = S_OK;

   m_nVariableCount = nVariableCount;
   m_paVariables = new JavaVariable[nVariableCount];
   for (UINT i=0; i<m_nVariableCount; ++i)
      m_paVariables[i] = paVariables[i]; 
   // Why in hell does all this array stuff work?
   // Shouldn't it fail because of single pointers and stack objects?

   for (UINT j=0; j<m_nVariableCount && SUCCEEDED(hr); ++j)
      hr = m_paVariables[j].ReadValue(pStream);

   return hr;
}

HRESULT JavaClassInstance::WriteVariables(LIo::DataOutputStream *pStream)
{
   HRESULT hr = S_OK;

   if (NULL == pStream)
      return E_POINTER;
 
   for (UINT i=0; i<m_nVariableCount && SUCCEEDED(hr); ++i)
   {
      hr = m_paVariables[i].WriteTypeAndName(pStream);
   }

   return hr;
}
   
HRESULT JavaClassInstance::WriteValues(LIo::DataOutputStream *pStream)
{
   HRESULT hr = S_OK;

   if (NULL == pStream)
      return E_POINTER;

   for (UINT i=0; i<m_nVariableCount && SUCCEEDED(hr); ++i)
   {
      hr = m_paVariables[i].WriteValue(pStream);
   }
   
   return hr;
}



// #################################################################

LpdFileStruct::LpdFileStruct()
{
   // m_lsFileName; // stack object

   m_bIsCompressed = false;
   m_nOffset = 0;
   m_nRealSize = 0;
   m_nCompressedSize = 0;
}

LpdFileStruct::~LpdFileStruct()
{
   // only stack objects
}

HRESULT LpdFileStruct::InitAndReadValues(JavaVariable *paVariables, UINT nVariableCount, LIo::InputStream *pStream)
{
   HRESULT hr = S_OK;

   // extract variable information
   hr = JavaClassInstance::InitAndReadValues(paVariables, nVariableCount, pStream);

   if (SUCCEEDED(hr))
   {
      // distribute variable information
      for (UINT i=0; i<m_nVariableCount; ++i)
      {
         if (m_paVariables[i].m_lstrName == _T("name"))
         {
            m_lsFileName = m_paVariables[i].m_lstrValue; // makes a copy
         }
         else if (m_paVariables[i].m_lstrName == _T("size"))
         {
            m_nCompressedSize = (UINT)m_paVariables[i].m_lValue;
         }
         else if (m_paVariables[i].m_lstrName == _T("realSize"))
         {
            m_nRealSize = (UINT)m_paVariables[i].m_lValue;
         }
         else if (m_paVariables[i].m_lstrName == _T("offset"))
         {
            m_nOffset = (UINT)m_paVariables[i].m_lValue;
         }
         else if (m_paVariables[i].m_lstrName == _T("isCompressed"))
         {
            m_bIsCompressed = m_paVariables[i].m_lValue != 0;
         }
         else if (m_paVariables[i].m_lstrName == _T("realName"))
         {
            int iDummy = 0;
         }
      }
   }

   return hr;
}

HRESULT LpdFileStruct::WriteValues(LIo::DataOutputStream *pStream)
{
   HRESULT hr = S_OK;

   // check if the values of the variables of the super class
   // correspond with these values here

   for (UINT i=0; i<m_nVariableCount; ++i)
   {
      if (m_paVariables[i].m_lstrName == _T("name") && !(m_lsFileName == m_paVariables[i].m_lstrValue))
      {
         m_paVariables[i].m_lstrValue = m_lsFileName; // makes a copy
      }
      else if (m_paVariables[i].m_lstrName == _T("size") && m_nCompressedSize != (UINT)m_paVariables[i].m_lValue)
      {
         m_paVariables[i].m_lValue = m_nCompressedSize;;
      }
      else if (m_paVariables[i].m_lstrName == _T("realSize") && m_nRealSize != (UINT)m_paVariables[i].m_lValue)
      {
         m_paVariables[i].m_lValue = m_nRealSize;
      }
      else if (m_paVariables[i].m_lstrName == _T("offset") && m_nOffset != (UINT)m_paVariables[i].m_lValue)
      {
         m_paVariables[i].m_lValue = m_nOffset;
      }
      else if (m_paVariables[i].m_lstrName == _T("isCompressed") && m_bIsCompressed != (m_paVariables[i].m_lValue != 0))
      {
         m_paVariables[i].m_lValue = 1;
      }
   }


   hr = JavaClassInstance::WriteValues(pStream);

   return hr;
}



// #################################################################


#include <crtdbg.h>

LpdHandler::LpdHandler()
{
   m_pKeyBuffer = NULL;

   m_paFiles = NULL;
   m_nFileCount = 0;
   m_nTotalBytes = 0;
   m_nConfigByte = 0;
   m_nFilesStart = 0;

   m_bMethodActive = false;
   m_bCancelRequested = false;
   m_nCopiedBytes = 0;
}

LpdHandler::~LpdHandler()
{
   if (NULL != m_paFiles)
      delete[] m_paFiles;
   m_paFiles = NULL;

   if (m_pKeyBuffer)
      delete m_pKeyBuffer;
   m_pKeyBuffer = NULL;
}

HRESULT LpdHandler::Init(const _TCHAR *tszDocument, LProgressListener *pListener)
{
   HRESULT hr = S_OK;

   m_pProgressListener = pListener;

   m_lsFileName += (_TCHAR *)tszDocument; // cast: will get copied anyway

   // double code (below)
   LFile inputFile((LPTSTR)m_lsFileName);
   LIo::FileInputStream fileStream;
   if (SUCCEEDED(hr))
      hr = fileStream.Init(&inputFile);

   m_pKeyBuffer = new LBuffer(2048);

   UINT nRead = 0;
   if (SUCCEEDED(hr))
      hr = fileStream.ReadBytes(m_pKeyBuffer, 0, 4, &nRead); // EPF_ identifier
   if (SUCCEEDED(hr) && nRead < 4)
      hr = E_PARSING;

   // check for EPF
   if (SUCCEEDED(hr))
   {
      BYTE *pKeyBytes = m_pKeyBuffer->GetByteAccess();
      if(pKeyBytes[0] != 'E' || pKeyBytes[1] != 'P' || pKeyBytes[2] != 'F')
         hr = E_PARSING;
      
      m_nConfigByte = pKeyBytes[3];

   }
   
   if (SUCCEEDED(hr))
      hr = fileStream.ReadBytes(m_pKeyBuffer, 0, 2048, &nRead);
   if (SUCCEEDED(hr) && nRead < 2048)
      hr = E_PARSING;
  
   // make it faster by factor 20
   LIo::BufferedInputStream bufferStream;
   if (SUCCEEDED(hr))
      hr = bufferStream.Init(&fileStream);

   LIo::XorInputStream xorStream;
   if (SUCCEEDED(hr))
      hr = xorStream.Init(&bufferStream, m_pKeyBuffer);

   if (SUCCEEDED(hr))
      hr = ParseHeader(&xorStream, &m_nFilesStart);

   if (SUCCEEDED(hr) && m_nFilesStart == 0)
      hr = E_PARSING;
   
   if (SUCCEEDED(hr))
      m_nFilesStart += 2052; // identifier and xor buffer (above)

  
   if (SUCCEEDED(hr))
   {
      for (UINT c=0; c<m_nFileCount && SUCCEEDED(hr); ++c)
      {
         UINT nLength = m_paFiles[c].m_nCompressedSize;
         if (!m_paFiles[c].m_bIsCompressed)
            nLength = m_paFiles[c].m_nRealSize;
         
         m_nTotalBytes += nLength;
      }
   }

   
   return hr;
}

/*private*/ HRESULT LpdHandler::ParseHeader(LIo::InputStream *pStreamIn, UINT *pnHeaderLength)
{
   HRESULT hr = S_OK;

   LIo::CounterInputStream countStream;
   hr = countStream.Init(pStreamIn);

   // short hand for below aka old code
   LIo::InputStream *pStream = &countStream;


   LBuffer lbHeaderBuffer(4096);
   BYTE *pHeaderBytes = lbHeaderBuffer.GetByteAccess();
   
   // the now following Java ObjectStream parsing is especially
   // tailored (simplified) for LPDs
   UINT nRead = 0;
   hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 4, &nRead); // 0xaced0005
   
   // check for ObjectStream identifier
   UINT nFileIdentifier = ConvertLeValue(&lbHeaderBuffer, 4);
   if (SUCCEEDED(hr) && 0xaced0005 != nFileIdentifier)
      hr = E_PARSING;


   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // 0x7572
   // maybe check for array identifier
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // array class type string length
   UINT nArrayTypeLength = ConvertLeValue(&lbHeaderBuffer, 2);
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, nArrayTypeLength, &nRead);
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 8, &nRead); // serial version id
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 1, &nRead); // unknown
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // unknown length (=0)
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // 0x7870
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 4, &nRead); // array element count
   
   m_nFileCount = ConvertLeValue(&lbHeaderBuffer, 4);
   
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // 0x7372
   // maybe check for normal class identifier
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // (internal) class type string length
   UINT nClassTypeLength = ConvertLeValue(&lbHeaderBuffer, 2);
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, nClassTypeLength, &nRead);
   // maybe check class type
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 8, &nRead); // serial version id
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 1, &nRead); // unknown
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // class element count
   
   UINT nVariableCount = ConvertLeValue(&lbHeaderBuffer, 2);
   if (SUCCEEDED(hr) && 7 != nVariableCount)
      hr = E_PARSING;

   JavaVariable *paVariables = new JavaVariable[nVariableCount];
   for (UINT v=0; v<nVariableCount && SUCCEEDED(hr); ++v)
      hr = paVariables[v].ReadTypeAndName(pStream, v > 0 ? &paVariables[v-1] : NULL);
   
   if (SUCCEEDED(hr))
      hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // 0x7870
   
   // check for values identifier
   UINT nPossibleValuesId = ConvertLeValue(&lbHeaderBuffer, 2);
   if (SUCCEEDED(hr) && JAVA_STREAM_ID_VALUES != nPossibleValuesId)
      hr = E_PARSING;

   if (SUCCEEDED(hr))
      m_paFiles = new LpdFileStruct[m_nFileCount];
   for (UINT c=0; c<m_nFileCount && SUCCEEDED(hr); ++c)
   {
      hr = m_paFiles[c].InitAndReadValues(paVariables, nVariableCount, pStream);
      if (c+1 < m_nFileCount && SUCCEEDED(hr))
      {
         // jump over the values delimiter
         hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 2, &nRead); // 0x7371
         hr = pStream->ReadBytes(&lbHeaderBuffer, 0, 4, &nRead); // unknown
      }
   }

   if (SUCCEEDED(hr))
      *pnHeaderLength = countStream.GetReadBytes();
   
   delete[] paVariables;

   
   return hr;
}

HRESULT LpdHandler::CopyPackedFiles(const _TCHAR *tszPath)
{
   m_bMethodActive = true;

   HRESULT hr = S_OK;

   ::timeBeginPeriod(1);

   DWORD dwStartMs = ::timeGetTime();

      
   // double code (above)
   LFile inputFile((LPTSTR)m_lsFileName);
   LIo::FileInputStream fileStream;
   if (SUCCEEDED(hr))
      hr = fileStream.Init(&inputFile);

   if (SUCCEEDED(hr))
      hr = fileStream.Skip(m_nFilesStart);
  
   // make it faster by factor 20
   LIo::BufferedInputStream bufferStream;
   if (SUCCEEDED(hr))
      hr = bufferStream.Init(&fileStream);

   LIo::XorInputStream xorStream;
   if (SUCCEEDED(hr))
      hr = xorStream.Init(&bufferStream, m_pKeyBuffer);

   m_nCopiedBytes = 0;
   for (UINT c=0; c<m_nFileCount && SUCCEEDED(hr); ++c)
   {
      LString lsPath((_TCHAR *)tszPath);
      lsPath += _T("\\");
      lsPath += m_paFiles[c].m_lsFileName;
      LFile lFile(lsPath);

      UINT nLength = m_paFiles[c].m_nCompressedSize;
      if (!m_paFiles[c].m_bIsCompressed)
         nLength = m_paFiles[c].m_nRealSize;

      LIo::FileOutputStream fileOut;
      hr = fileOut.Init(&lFile);

      if (m_bCancelRequested)
         hr = E_ABORT;

      if (SUCCEEDED(hr))
         hr = Copy(&xorStream, &fileOut, nLength);
   }

   DWORD dwEndMs = ::timeGetTime();

#ifndef _DEBUG
   _TCHAR tszDummy[2048];
   _stprintf(tszDummy, _T("### Tooken time: %d ms"), dwEndMs-dwStartMs);
   ::MessageBox(NULL, tszDummy, _T("Zeitverbrauch"), MB_OK);
#endif
   _RPT1(_CRT_WARN, "### Tooken time: %d ms\n", dwEndMs-dwStartMs);

   ::timeEndPeriod(1);

   
   if (m_bCancelRequested)
      m_bCancelRequested = false;

   m_bMethodActive = false;

   return hr;
}

HRESULT LpdHandler::RecreateLpd(const _TCHAR *tszPathAndName, BYTE btNewType, bool *pbTypesDifferent)
{
   m_bMethodActive = true;

   HRESULT hr = S_OK;

   if (NULL == m_paFiles || 0 == m_nFileCount)
      return E_FAIL;

   LBuffer dummyBuffer(128);
   if (SUCCEEDED(hr))
      hr = RepackTypeInfo(&dummyBuffer, btNewType, pbTypesDifferent);

   if (SUCCEEDED(hr) && !*pbTypesDifferent)
      return S_FALSE; // nothing done



   LFile lFile(tszPathAndName);
   LIo::FileOutputStream fileOutput;
   if (SUCCEEDED(hr))
      hr = fileOutput.Init(&lFile);

   LIo::BufferedOutputStream bufferedOutput;
   if (SUCCEEDED(hr))
      hr = bufferedOutput.Init(&fileOutput);

   LIo::DataOutputStream dataOutput;
   if (SUCCEEDED(hr))
      hr = dataOutput.Init(&bufferedOutput);

   if (SUCCEEDED(hr))
   {
      hr = dataOutput.WriteByte('E');
      hr = dataOutput.WriteByte('P');
      hr = dataOutput.WriteByte('F');
      hr = dataOutput.WriteByte(m_nConfigByte);
   }

   
   LBuffer lbEmptyBuffer(2048);
   lbEmptyBuffer.FillRandomly();
   lbEmptyBuffer.SetAddedSize(2048);

   if (SUCCEEDED(hr))
   {
      // for now write an empty xor-key

      hr = bufferedOutput.WriteBytes(&lbEmptyBuffer, 0, 2048);
   }

   LIo::XorOutputStream xorOutput;
   if (SUCCEEDED(hr))
      hr = xorOutput.Init(&bufferedOutput, &lbEmptyBuffer);

   
   if (SUCCEEDED(hr))
      hr = RewriteBaseHeader(&xorOutput);

   if (SUCCEEDED(hr))
      hr = RewriteValuesAndFiles(&xorOutput, btNewType); // in the order defined by RewriteBaseHeader()

   if (m_bCancelRequested)
      m_bCancelRequested = false;

   m_bMethodActive = false;

   return hr;
}


void LpdHandler::RequestCancelAction()
{
   if (m_bMethodActive)
   {
      m_bCancelRequested = true;
   }
   // else the variable will not be reset
}



/*private*/ HRESULT LpdHandler::RewriteBaseHeader(LIo::OutputStream *pDataStream)
{
   HRESULT hr = S_OK;

   if (NULL == pDataStream)
      return E_POINTER;

   if (NULL == m_paFiles || 0 == m_nFileCount)
      return E_FAIL;

   LIo::DataOutputStream dataOut;
   if (SUCCEEDED(hr))
      hr = dataOut.Init(pDataStream);


   dataOut.WriteLeInt(JAVA_OBJECT_FILE_ID);
   dataOut.WriteLeShort(JAVA_STREAM_ID_ARRAY);
   LString lsArrayIdent(_T("[Limc.epresenter.filesdk.FileStruct;"));
   dataOut.WriteUtf8WithLength(&lsArrayIdent);
   dataOut.WriteLeInt(0xa4d94367); // serial version id (8 bytes)
   dataOut.WriteLeInt(0xb4f24da8);
   dataOut.WriteByte(0x02); // unknown
   dataOut.WriteLeShort(0); // unknown length
   dataOut.WriteLeShort(JAVA_STREAM_ID_VALUES);
   dataOut.WriteLeInt(m_nFileCount);
   dataOut.WriteLeShort(JAVA_STREAM_ID_CLASS);
   LString lsClassIdent(_T("imc.epresenter.filesdk.FileStruct"));
   dataOut.WriteUtf8WithLength(&lsClassIdent);
   dataOut.WriteLeInt(0xc3feec1b); // serial version id (8 bytes)
   dataOut.WriteLeInt(0x62f17651);
   dataOut.WriteByte(0x02); // unknown
   dataOut.WriteLeShort(m_paFiles[0].GetVariableCount());

   if (SUCCEEDED(hr))
      //hr = 
      m_paFiles[0].WriteVariables(&dataOut);


   return hr;
}

/*private*/ HRESULT LpdHandler::RewriteValuesAndFiles(LIo::OutputStream *pDataStream, BYTE btNewType)
{
   HRESULT hr = S_OK;

   if (NULL == pDataStream)
      return E_POINTER;

   LIo::DataOutputStream dataOut;
   if (SUCCEEDED(hr))
      hr = dataOut.Init(pDataStream);

   bool bTypesDifferent = false;
   LBuffer packedTypeInfo(128);
   if (SUCCEEDED(hr))
      hr = RepackTypeInfo(&packedTypeInfo, btNewType, &bTypesDifferent);


   if (bTypesDifferent && SUCCEEDED(hr))
   {
      // check for correct offset and size values
      
      INT iOffsetCorrection = 0;
      for (UINT f=0; f<m_nFileCount; ++f)
      {
         if (m_paFiles[f].m_lsFileName == _T("_internal_versiontypeinfo"))
         {
            if (m_paFiles[f].m_nCompressedSize != (unsigned)packedTypeInfo.GetAddedSize())
            {
               // this file has a different size
               m_paFiles[f].m_nCompressedSize = packedTypeInfo.GetAddedSize();

               // the following have a different offset
               iOffsetCorrection = packedTypeInfo.GetAddedSize() - m_paFiles[f].m_nCompressedSize;
            }
         }
         else if (iOffsetCorrection != 0)
         {
            m_paFiles[f].m_nOffset += iOffsetCorrection;
         }
      }
   }
   // else bTypesDifferent doesn't matter here, it was checked above/at an earlier time

   if (SUCCEEDED(hr))
      dataOut.WriteLeShort(JAVA_STREAM_ID_VALUES);

   UINT f;
   for (f=0; f<m_nFileCount && SUCCEEDED(hr); ++f)
   {
      // hr = 
      m_paFiles[f].WriteValues(&dataOut);
      if (f+1 < m_nFileCount && SUCCEEDED(hr))
      {
         hr = dataOut.WriteLeShort(JAVA_STREAM_ID_SEPARATOR);
         if (SUCCEEDED(hr))
            hr = dataOut.WriteLeInt(0x007e0002); // unknown
      }
   }


   // for now this is a hack; there seems to be no (easy) "instanceof" operator
   LIo::XorOutputStream *pXorStream = (LIo::XorOutputStream *)pDataStream;
   pXorStream->ResetKeyPointer();


   // double code (above)
   LFile inputFile((LPTSTR)m_lsFileName);
   LIo::FileInputStream fileStream;
   if (SUCCEEDED(hr))
      hr = fileStream.Init(&inputFile);
   
   if (SUCCEEDED(hr))
      hr = fileStream.Skip(m_nFilesStart);
   
   LIo::BufferedInputStream bufferStream;
   bufferStream.Init(&fileStream);
   
   LIo::XorInputStream xorStreamIn;
   if (SUCCEEDED(hr))
      hr = xorStreamIn.Init(&bufferStream, m_pKeyBuffer);

   m_nCopiedBytes = 0;
   for (f=0; f<m_nFileCount && SUCCEEDED(hr); ++f)
   {
      UINT nLength = m_paFiles[f].m_bIsCompressed ? m_paFiles[f].m_nCompressedSize : m_paFiles[f].m_nRealSize;
      if (!(m_paFiles[f].m_lsFileName == _T("_internal_versiontypeinfo")))
      {
         
         ::timeBeginPeriod(1);
         DWORD dwStartMs = ::timeGetTime();

 
         hr = Copy(&xorStreamIn, &dataOut, nLength);

         DWORD dwEndMs = ::timeGetTime();
         ::timeEndPeriod(1);

         _RPT2(_CRT_WARN, "Copied %d in %dms\n", nLength, (dwEndMs-dwStartMs));
      }
      else
      {
         hr = dataOut.WriteBytes(&packedTypeInfo, 0, packedTypeInfo.GetAddedSize());
         if (SUCCEEDED(hr))
            hr = xorStreamIn.Skip(nLength);
      }
   }


   return hr;
}

/*private*/ HRESULT LpdHandler::RepackTypeInfo(LBuffer *pResultBuffer, BYTE btNewType, bool *pbTypesDifferent)
{
   HRESULT hr = S_OK;

   if (NULL == pResultBuffer)
      return E_POINTER;

   *pbTypesDifferent = false;

   bool bFileFound = false;
   for (UINT f=0; f<m_nFileCount; ++f)
   {
      if (m_paFiles[f].m_lsFileName == _T("_internal_versiontypeinfo"))
      {
         bFileFound = true;

         if ((unsigned)pResultBuffer->GetSize() <= m_paFiles[f].m_nCompressedSize)
            hr = E_INVALIDARG; // we need some space to operate

         // double code (above)
         LFile inputFile((LPTSTR)m_lsFileName);
         LIo::FileInputStream fileStream;
         if (SUCCEEDED(hr))
            hr = fileStream.Init(&inputFile);
         
         if (SUCCEEDED(hr))
            hr = fileStream.Skip(m_nFilesStart);
         
         LIo::BufferedInputStream bufferStream;
         bufferStream.Init(&fileStream);
         
         LIo::XorInputStream xorStream;
         if (SUCCEEDED(hr))
            hr = xorStream.Init(&bufferStream, m_pKeyBuffer);

         if (SUCCEEDED(hr))
            hr = xorStream.Skip(m_paFiles[f].m_nOffset);

         UINT nRead = 0;
         if (SUCCEEDED(hr))
            hr = xorStream.ReadBytes(pResultBuffer, 0, m_paFiles[f].m_nCompressedSize, &nRead);

         LBuffer uncompressedBuffer(m_paFiles[f].m_nRealSize + 100);
         UINT nUncompressedLength = 0;
         if (SUCCEEDED(hr))
         {
            hr = DecompressData(pResultBuffer, &uncompressedBuffer, &nUncompressedLength);

            if (FAILED(hr))
               hr = E_PARSING;
         }
         
         //
         // this is THE place to check for an input format error
         // as this file is most likely located at the end of the (input) file
         // some error up to this position will make Zlib fail
         //
         if (SUCCEEDED(hr) && nUncompressedLength != m_paFiles[f].m_nRealSize)
            hr = E_PARSING;

         if (SUCCEEDED(hr) && btNewType != uncompressedBuffer.GetByteAccess()[0])
         {
            uncompressedBuffer.GetByteAccess()[0] = btNewType;

            *pbTypesDifferent = true;

            UINT nCompressedLength = 0;
            if (SUCCEEDED(hr))
               hr = CompressData(&uncompressedBuffer, pResultBuffer, &nCompressedLength);
         }

      }
   }

   if (SUCCEEDED(hr) && !bFileFound)
      hr = E_PARSING;

   return hr;
}

/*private*/ HRESULT LpdHandler::Copy(LIo::InputStream *pLpdStream, LIo::OutputStream *pOutput, UINT nLength)
{
   HRESULT hr = S_OK;

   LBuffer buffer(4096);
   BYTE *pBytes = buffer.GetByteAccess();

   //_RPT3(_CRT_WARN, "Creating %s %d:%d ... ", (LPTSTR)*plsName, nCompressedSize, nRealSize);


   UINT nWritten = 0;
   while (nWritten < nLength && SUCCEEDED(hr))
   {
      UINT nToDoTotal = nLength - nWritten;
      UINT nToDoThis = min(nToDoTotal, (unsigned)buffer.GetSize());
   
      if (m_bCancelRequested)
         hr = E_ABORT;

      UINT nRead = 0;
      if (SUCCEEDED(hr))
         hr = pLpdStream->ReadBytes(&buffer, 0, nToDoThis, &nRead);
      
      if (nRead > 0 && SUCCEEDED(hr))
      {
         pOutput->WriteBytes(&buffer, 0, nRead);
         nWritten += nRead;
      }
      else
      {
         // to do some kind of error
         break;
      }

      
      m_nCopiedBytes += nRead;

      if (m_pProgressListener)
         m_pProgressListener->ReportProgress(m_nCopiedBytes, m_nTotalBytes);
   
   }  

   //_RPT0(_CRT_WARN, "Done.\n");


   return hr;
}


/*private*/ HRESULT LpdHandler::DecompressData(LBuffer *pSourceBuffer, LBuffer *pTargetBuffer, UINT *pnUncompressedLength)
{
   // ZLib compressed stuff

   ULONG nTargetLength = pTargetBuffer->GetSize();
   int iReturn = uncompress(pTargetBuffer->GetByteAccess(), &nTargetLength,
      pSourceBuffer->GetByteAccess(), pSourceBuffer->GetAddedSize());

   if(Z_OK == iReturn)
   {
      *pnUncompressedLength = nTargetLength;
      pTargetBuffer->SetAddedSize(nTargetLength);
      return S_OK;
   }
   else
      return E_FAIL;
}

/*private*/ HRESULT LpdHandler::CompressData(LBuffer *pSourceBuffer, LBuffer *pTargetBuffer, UINT *pnCompressedLength)
{
   // ZLib compressed stuff

   ULONG nTargetLength = pTargetBuffer->GetSize();
   int iReturn = compress(pTargetBuffer->GetByteAccess(), &nTargetLength,
      pSourceBuffer->GetByteAccess(), pSourceBuffer->GetAddedSize());

   if(Z_OK == iReturn)
   {
      *pnCompressedLength = nTargetLength;
      pTargetBuffer->SetAddedSize(nTargetLength);
      return S_OK;
   }
   else
      return E_FAIL;
}


   /* code for pretty printing the next section of an input stream
   dwRead = pStream->ReadBytes(&lbHeaderBuffer, 0, 80);
   for (UINT i=0; i<dwRead; ++i)
   {
      _RPT1(_CRT_WARN, "%02x ", pHeaderBytes[i]);
      
      if (((i+1) % 16) == 0)
      {
         for (UINT j=i-15; j<i+1; ++j)
         {
            int ch = pHeaderBytes[j];
            if (ch >= (int)'!' && ch <= (int)'~')
               _RPT1(_CRT_WARN, "%c", ch);
            else
               _RPT0(_CRT_WARN, ".");
         }

         _RPT0(_CRT_WARN, "\n");
      }
   }
     */

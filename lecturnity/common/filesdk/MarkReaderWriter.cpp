// MarkStream.cpp: Implementierung der Klasse CMarkStream.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkReaderWriter.h"

CMarkReaderWriter::CMarkReaderWriter()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_aAllMarks.SetSize(0, 160);
}

CMarkReaderWriter::~CMarkReaderWriter()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i=0; i<m_aAllMarks.GetSize(); ++i)
      delete m_aAllMarks[i];
   m_aAllMarks.RemoveAll();

}

HRESULT CMarkReaderWriter::Parse(SgmlElement *pElement)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   const _TCHAR *tszType = pElement->GetValue(_T("type"));
   const _TCHAR *tszLabel = pElement->GetValue(_T("label"));
   const _TCHAR *tszTime = pElement->GetValue(_T("time"));
   const _TCHAR *tszDemoDocumentObject = pElement->GetValue(_T("demoDocumentObject"));

   if (tszType == NULL || _tcslen(tszType) < 1)
      return E_SJM_INVALID_DATA;

   if (tszTime == NULL || _tcslen(tszTime) < 1)
      return E_SJM_INVALID_DATA;


   int iPositionMs = pElement->GetIntValue(_T("time"));
   //UINT nId = (unsigned)pElement->GetIntValue("id");
   UINT nId = 0; // means undefined

   bool bIsJumpMark = _tcscmp(tszType, _T("jump")) == 0;
   bool bIsDemoDocumentObject = false;
   if (tszDemoDocumentObject) {
        bIsDemoDocumentObject = _tcscmp(tszDemoDocumentObject, _T("true")) == 0;
   }

   CStopJumpMark *pMark = new CStopJumpMark();
   hr = pMark->Init(bIsJumpMark, tszLabel, iPositionMs, nId, bIsDemoDocumentObject);

   // TODO: sort and unify
   if (SUCCEEDED(hr))
      m_aAllMarks.Add(pMark);
   else
      delete pMark;


   return hr;
}

HRESULT CMarkReaderWriter::Extract(CStopJumpMark *aMarks)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (m_aAllMarks.GetSize() == 0)
      return S_FALSE;

   for (int i=0; i<m_aAllMarks.GetSize() && SUCCEEDED(hr); ++i)
   {
      hr = m_aAllMarks[i]->CloneTo(&aMarks[i]);

      delete m_aAllMarks[i];
   }
   m_aAllMarks.RemoveAll();

   return hr;
}

int CMarkReaderWriter::GetCount()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_aAllMarks.GetSize();
}




//////////////////////////////////////////////////////////////////////
// CStopJumpMark Klasse
//////////////////////////////////////////////////////////////////////

CStopJumpMark::CStopJumpMark()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_bIsJumpMark = false;
   m_nPositionMs = 0;
   ZeroMemory(m_tszLabel, MAX_MARK_LABEL_LENGTH * sizeof _TCHAR);

   m_nId = 0;
   m_bIsInitialized = false;
   m_bIsDemoDocumentObject = false;

   m_nUseCounter = 0;
}

CStopJumpMark::~CStopJumpMark()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

}

/*
UINT CStopJumpMark::GetId(bool bCreate, bool bDummy)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_nId == 0 && bCreate)
   {
      ++s_nMarkCounter;

      m_nId = s_nMarkCounter;
   }

   return m_nId;
}
*/

   

HRESULT CStopJumpMark::Init(bool bIsJumpMark, LPCTSTR tszLabel, UINT nPositionMs, UINT nId, bool bIsDemoDocumentObject)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_bIsJumpMark = bIsJumpMark;
   m_nPositionMs = nPositionMs;

   ZeroMemory(m_tszLabel, MAX_MARK_LABEL_LENGTH * sizeof _TCHAR);

   if (tszLabel != NULL)
   {
      int nLength = _tcslen(tszLabel);
      _tcsncpy(m_tszLabel, tszLabel, min(MAX_MARK_LABEL_LENGTH, nLength));
   }

   // TODO is this unique?
   m_nId = CreateId(nId);
  
   m_bIsDemoDocumentObject = bIsDemoDocumentObject;
   m_bIsInitialized = true;

   return S_OK;
}

HRESULT CStopJumpMark::CloneTo(CStopJumpMark *pMark, bool bCreateNewId)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (pMark == NULL)
      return E_POINTER;

   if (!m_bIsInitialized)
      return E_SJM_UNINITIALIZED;

   pMark->m_bIsJumpMark = m_bIsJumpMark;
   pMark->m_nPositionMs = m_nPositionMs;
   pMark->SetDemoDocumentObject(m_bIsDemoDocumentObject);

   int nLength = _tcslen(m_tszLabel);
   _tcsncpy(pMark->m_tszLabel, m_tszLabel, min(MAX_MARK_LABEL_LENGTH, nLength));


   if (!bCreateNewId)
   {
      pMark->m_nId = m_nId;
      pMark->m_nUseCounter = m_nUseCounter;
   }
   else
   {
      pMark->CreateId(0); // create a new id; pMark->m_nId == 0 as it was not yet initialized
      pMark->m_nUseCounter = 0;
   }


   pMark->m_bIsInitialized = true;
   
   return S_OK;
}

HRESULT CStopJumpMark::IncrementPosition(UINT nDiffMs, UINT nMaxValue)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (!m_bIsInitialized)
      return E_SJM_UNINITIALIZED;
   
   if (m_nPositionMs + nDiffMs >= nMaxValue)
      nDiffMs = nMaxValue - m_nPositionMs - 1;

   m_nPositionMs += nDiffMs;

   return S_OK;
}

HRESULT CStopJumpMark::DecrementPosition(UINT nDiffMs)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (!m_bIsInitialized)
      return E_SJM_UNINITIALIZED;

   if (nDiffMs <= m_nPositionMs)
      m_nPositionMs -= nDiffMs;
   else
   {
      m_nPositionMs = 0;
      hr = S_FALSE; // TODO meaningful error (value)?
   }

   return hr;
}

HRESULT CStopJumpMark::SetLabel(LPCTSTR tszLabel)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (!m_bIsInitialized)
      return E_SJM_UNINITIALIZED;

	if (tszLabel != NULL)
   {
      int nLength = _tcslen(tszLabel);
      _tcsncpy(m_tszLabel, tszLabel, min(MAX_MARK_LABEL_LENGTH, nLength));
   }
	else
		ZeroMemory(m_tszLabel, MAX_MARK_LABEL_LENGTH * sizeof _TCHAR);

	return hr;
}


HRESULT CStopJumpMark::RegisterUser()
{
   ++m_nUseCounter;

   return S_OK;
}

HRESULT CStopJumpMark::UnregisterUser()
{
   if (m_nUseCounter > 0)
      --m_nUseCounter;
   else
      return E_FAIL;

   return S_OK;
}


// private
UINT CStopJumpMark::CreateId(UINT nIdSeed)
{
   static UINT s_nMarkCounter = 0;
   ++s_nMarkCounter; 
   // increase mark counter in any case, in order to have a unique id
   // for every mark added by the user (lateron)

   if (nIdSeed == 0)
      return s_nMarkCounter;
   else
      return nIdSeed;
}


bool CStopJumpMark::IsDemoDocumentObject() {
    return m_bIsDemoDocumentObject; 
}

void CStopJumpMark::SetDemoDocumentObject(bool bIsDemoDocumentObject){
    m_bIsDemoDocumentObject = bIsDemoDocumentObject;
}
// InteractionReaderWriter.cpp: Implementierung der Klasse CInteractionReaderWriter.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InteractionReaderWriter.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CInteractionReaderWriter::CInteractionReaderWriter()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_aInteractions.RemoveAll();
   m_aInteractions.SetSize(0, 50);
}

CInteractionReaderWriter::~CInteractionReaderWriter()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i=0; i<m_aInteractions.GetSize(); ++i)
      delete m_aInteractions[i];
   m_aInteractions.RemoveAll();
}

bool CInteractionReaderWriter::IsInteractionObject(LPCTSTR tszIdentifier)
{
   if (tszIdentifier != NULL)
   {
      if (_tcscmp(tszIdentifier, _T("AREA")) == 0)
         return true;
      else if (_tcscmp(tszIdentifier, _T("BUTTON")) == 0)
         return true;
      else if (_tcscmp(tszIdentifier, _T("RADIO")) == 0)
         return true;
      else if (_tcscmp(tszIdentifier, _T("CHECK")) == 0)
         return true;
      else if (_tcscmp(tszIdentifier, _T("FIELD")) == 0)
         return true;
      else if (_tcscmp(tszIdentifier, _T("CALLOUT")) == 0)
         return true;
      else if (_tcscmp(tszIdentifier, _T("CLICK")) == 0)
         return true;
   }

   return false;
}
   
HRESULT CInteractionReaderWriter::Parse(SgmlElement *pElement, CString& csRecordPath)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (pElement == NULL)
      return E_POINTER;

   CInteractionArea *pInteraction = new CInteractionArea();
   hr = pInteraction->ReadFromSgml(pElement, csRecordPath);

   if (SUCCEEDED(hr))
      m_aInteractions.Add(pInteraction);
   else
      delete pInteraction;

   return hr;
}



HRESULT CInteractionReaderWriter::AddSupportObject(DrawSdk::DrawObject *pObject)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   if (pObject == NULL)
      return E_POINTER;

   CInteractionArea *pInteraction = new CInteractionArea();
   hr = pInteraction->Init(pObject);

   if (SUCCEEDED(hr))
      m_aInteractions.Add(pInteraction);
   else
      delete pInteraction;

   return hr;
}


HRESULT CInteractionReaderWriter::Extract(CInteractionArea **paInteractions)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (paInteractions == NULL)
      return E_POINTER;

   if (m_aInteractions.GetSize() == 0)
      return S_FALSE;

   for (int i=0; i<m_aInteractions.GetSize(); ++i)
   {
      paInteractions[i] = m_aInteractions[i];
   }
   m_aInteractions.RemoveAll();

   return S_OK;
}

UINT CInteractionReaderWriter::GetCount()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_aInteractions.GetSize();
}

HRESULT CInteractionReaderWriter::Extract(CArray<CInteractionArea *, CInteractionArea *> *paInteractions)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (paInteractions == NULL)
      return E_POINTER;

   paInteractions->Append(m_aInteractions);
   m_aInteractions.RemoveAll();

   return S_OK;
}


CInteractionArea *CInteractionReaderWriter::GetObject(UINT nIdx)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (nIdx < (unsigned)m_aInteractions.GetSize())
      return m_aInteractions[nIdx];
   else
      return NULL;
}

HRESULT CInteractionReaderWriter::ExtractQuestionInteractives(CArray<CInteractionArea *, CInteractionArea *> *paInteractives)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (paInteractives == NULL)
      return E_POINTER;

   // we have to preserve the order of the objects:
   // so we cannot proceed through the array from the end for deleting at the same time

   bool *abExtracted = new bool[m_aInteractions.GetSize()];

   bool bOneFound = false;
   for (int i=0; i<m_aInteractions.GetSize(); ++i)
   {
      bool bIsQuestionButton = false;
      if (m_aInteractions[i]->IsButton())
      {
         CString csAction = m_aInteractions[i]->GetMouseClickAction();

         if (csAction.Compare(_T("check")) == 0 || csAction.Compare(_T("reset")) == 0)
            bIsQuestionButton = true;
      }

      if (bIsQuestionButton || m_aInteractions[i]->IsCheckBox() || m_aInteractions[i]->IsRadioButton() 
         || m_aInteractions[i]->IsTextField() || m_aInteractions[i]->IsSupport())
      {
         bOneFound = true;
         paInteractives->Add(m_aInteractions[i]);
         abExtracted[i] = true;
      }
      else
      {
         abExtracted[i] = false;
      }
   }
   
   if (bOneFound)
   {
      for (int i=m_aInteractions.GetSize()-1; i>=0; --i)
      {
         if (abExtracted[i])
            m_aInteractions.RemoveAt(i);
      }
   }

   delete[] abExtracted;

   if (bOneFound)
      return S_OK;
   else
      return S_FALSE;
}

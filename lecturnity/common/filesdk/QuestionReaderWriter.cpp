#include "stdafx.h"
#include "QuestionReaderWriter.h"

CQuestionReaderWriter::CQuestionReaderWriter()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_aAllQuestionnaires.SetSize(0, 5);
}

CQuestionReaderWriter::~CQuestionReaderWriter()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // remove not extracted objects

   for (int i=0; i<m_aAllQuestionnaires.GetSize(); ++i)
      delete m_aAllQuestionnaires[i];
}

HRESULT CQuestionReaderWriter::Parse(SgmlElement *pElement, CString &csRecordPath)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   CQuestionnaire *pQe = new CQuestionnaire();
   hr = pQe->Parse(pElement, csRecordPath);

   if (SUCCEEDED(hr))
      m_aAllQuestionnaires.Add(pQe);
   else
      delete pQe;

   return hr;
}

HRESULT CQuestionReaderWriter::Extract(CQuestionnaire **paQuestions)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (paQuestions == NULL)
      return E_POINTER;

   for (int i=0; i<m_aAllQuestionnaires.GetSize(); ++i)
      paQuestions[i] = m_aAllQuestionnaires[i];

   m_aAllQuestionnaires.RemoveAll();

   return S_OK;
}

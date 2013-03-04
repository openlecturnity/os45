#if !defined(AFX_QUESTIONREADERWRITER_H__523BCF34_A96B_4000_9462_50CD435216DD__INCLUDED_)
#define AFX_QUESTIONREADERWRITER_H__523BCF34_A96B_4000_9462_50CD435216DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SgmlParser.h"
#include "Questionnaire.h"

class FILESDK_EXPORT CQuestionReaderWriter  
{
public:
	CQuestionReaderWriter();
	virtual ~CQuestionReaderWriter();
	
   HRESULT Parse(SgmlElement *pElement, CString &csRecordPath);
   HRESULT Extract(CQuestionnaire **paQuestions);
   UINT GetCount() { return m_aAllQuestionnaires.GetSize(); }

private:
   CArray<CQuestionnaire *, CQuestionnaire *> m_aAllQuestionnaires;
};

#endif // !defined(AFX_QUESTIONREADERWRITER_H__523BCF34_A96B_4000_9462_50CD435216DD__INCLUDED_)

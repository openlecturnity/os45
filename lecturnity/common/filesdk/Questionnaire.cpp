#include "stdafx.h"
#include "Questionnaire.h"
#include "WhiteboardStream.h" // Note: inclusion is only possible in this cpp file not in the header.

#include "lutils.h"


CFeedbackDefinition::CFeedbackDefinition()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_bIsDisabled = false;
   m_bIsInherited = false;
   m_pOkButton = NULL;

   m_aObjects.SetSize(0, 8);
}

CFeedbackDefinition::~CFeedbackDefinition()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i=0; i<m_aObjects.GetSize(); ++i)
      delete m_aObjects[i];

   if (m_pOkButton != NULL)
      delete m_pOkButton;
}

HRESULT CFeedbackDefinition::Parse(SgmlElement *pElement, CString& csRecordPath) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    if (pElement == NULL)
        return E_POINTER;

    LPCTSTR szName = pElement->GetName();
    if (_tcscmp(szName, _T("feedback")) != 0)
        return E_INVALIDARG;

    LPCTSTR szType = pElement->GetValue(_T("type"));
    if (szType == NULL)
        return E_QUE_INVALID_DATA;

    LPCTSTR szValue = pElement->GetValue(_T("value"));
    if (szValue == NULL)
        return E_QUE_INVALID_DATA;


    m_csType = szType;

    // TODO in contrast to the other parameters this should be 
    // UTF-8 (or something tranlated) in later version.
    LPCTSTR szText = pElement->GetValue(_T("text"));

    if (szText != NULL)
        m_csText = szText;

    // these sizes can be specified but do not have to (at the moment)
    int x = pElement->GetIntValue(_T("x"));
    int y = pElement->GetIntValue(_T("y"));
    int w = pElement->GetIntValue(_T("width"));
    int h = pElement->GetIntValue(_T("height"));


    if (_tcscmp(szValue, _T("enable")) == 0) {
        // there are objects defining the appearance

        CArray<SgmlElement *, SgmlElement *> aElements;
        pElement->GetElements(aElements);
        for (int i = 0; i < aElements.GetCount() && SUCCEEDED(hr); ++i) {
            SgmlElement *pNext = aElements[i];
            if (pNext != NULL && SUCCEEDED(hr)) {
                bool bIsButton = _tcscmp(pNext->GetName(), _T("BUTTON")) == 0;
                if (!bIsButton) {
                    DrawSdk::DrawObject *pObject = CWhiteboardStream::CreateObject(pNext, csRecordPath);
                    if (pObject != NULL)
                        m_aObjects.Add(pObject);
                    // TODO else error handling?
                } else {
                    if (m_pOkButton != NULL)
                        hr = E_QUE_INVALID_DATA; // only one button allowed

                    if (SUCCEEDED(hr)) {
                        CInteractionArea *pButton = new CInteractionArea();
                        hr = pButton->ReadFromSgml(pNext, csRecordPath);

                        if (SUCCEEDED(hr))
                            m_pOkButton = pButton;
                        else
                            delete pButton;
                    }
                }
            }
        }
    } else if (_tcscmp(szValue, _T("change-text")) == 0) {
        // the text above must be set and is another text than the one in the questionnaire
        m_bIsInherited = true;

        // TODO check for text length > 0??
    } else if (_tcscmp(szValue, _T("inherit")) == 0) {
        m_bIsInherited = true;
    } else if (_tcscmp(szValue, _T("disable")) == 0) {
        m_bIsDisabled = true;
    }

    return hr;

}

HRESULT CFeedbackDefinition::ExtractObjects(CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paTarget)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (paTarget == NULL)
      return E_POINTER;

   paTarget->Append(m_aObjects);
   m_aObjects.RemoveAll();

   return paTarget->GetSize() > 0 ? S_OK : S_FALSE;
}

CInteractionArea* CFeedbackDefinition::ExtractOkButton()
{
   CInteractionArea *pArea = m_pOkButton;
   m_pOkButton = NULL;
   return pArea;
}


 
/////////////////////////////////////////////////////////////////////////////


CQuestion::CQuestion()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_nAchievablePoints = 0;
   m_nMaximumTime = 0;
   m_nMaximumAttempts = 0;
   m_aFeedback.SetSize(0, 10);
}

CQuestion::~CQuestion()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i=0; i<m_aFeedback.GetSize(); ++i)
      delete m_aFeedback[i];
}

HRESULT CQuestion::Parse(SgmlElement *pElement, CString& csRecordPath) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    if (pElement == NULL)
        return E_POINTER;

    LPCTSTR szName = pElement->GetName();
    if (_tcscmp(szName, _T("QUESTION")) != 0)
        return E_INVALIDARG;

    LPCTSTR tszType = pElement->GetValue(_T("type"));
    if (tszType != NULL)
        m_csType = tszType;

    // TODO these are TCHARs (or WCHARs) instead of the chars below
    LPCTSTR tszTitle = pElement->GetValue(_T("title"));
    if (tszTitle != NULL)
        m_csTitle = tszTitle;

    LPCTSTR tszQuestion = pElement->GetValue(_T("text"));
    if (tszQuestion != NULL) {
        m_csQuestion = tszQuestion;

        // BUGFIX #3122
        // The question text can have returns (\r\n from and for the text field).
        m_csQuestion.Replace(_T("\\r\\n"), _T("\r\n"));
    }

    LPCTSTR tszScormId = pElement->GetValue(_T("id"));
    if (tszScormId != NULL)
        m_csScormId = tszScormId;


    LPCTSTR szAttempts = pElement->GetValue(_T("attempts"));
    LPCTSTR szTime = pElement->GetValue(_T("time"));
    LPCTSTR szPoints = pElement->GetValue(_T("points"));
    LPCTSTR szActive = pElement->GetValue(_T("active"));
    LPCTSTR szVisible = pElement->GetValue(_T("visible"));
    LPCTSTR szPage = pElement->GetValue(_T("page"));
    LPCTSTR szDndAlign = pElement->GetValue(_T("dnd-align"));

    if (szPoints == NULL || szActive == NULL)
        return E_QUE_INVALID_DATA;

    LPCTSTR szSuccessAction = pElement->GetValue(_T("onSuccess"));
    if (szSuccessAction != NULL)
        m_csSuccessAction = szSuccessAction;

    LPCTSTR szFailureAction = pElement->GetValue(_T("onFailure"));
    if (szFailureAction != NULL)
        m_csFailureAction = szFailureAction;


    if (szAttempts != NULL)
        m_nMaximumAttempts = _ttoi(szAttempts);

    if (szTime != NULL)
        m_nMaximumTime = _ttoi(szTime);

    if (szPoints != NULL)
        m_nAchievablePoints = _ttoi(szPoints);

    if (szActive != NULL)
        m_csObjectActivity = szActive;

    if (szVisible != NULL)
        m_csObjectVisibility = szVisible;

    if (szPage != NULL)
        m_csPageVisibility = szPage;

    if (szDndAlign != NULL)
        m_csDndAlignment = szDndAlign;

    CArray<SgmlElement *, SgmlElement *> aElements;
    pElement->GetElements(aElements);
    for (int i = 0; i < aElements.GetCount() && SUCCEEDED(hr); ++i) {
        SgmlElement *pNext = aElements[i];
        if (pNext != NULL) {
            if (_tcscmp(pNext->GetName(), _T("feedback")) == 0) {
                CFeedbackDefinition *pFeedback = new CFeedbackDefinition();
                hr = pFeedback->Parse(pNext, csRecordPath);

                if (SUCCEEDED(hr))
                    m_aFeedback.Add(pFeedback);
                else
                    delete pFeedback;
            }
            // else ignore

        }
    }

    return S_OK;
}


HRESULT CQuestion::ExtractFeedback(CArray<CFeedbackDefinition *, CFeedbackDefinition *> *paFeedback)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (paFeedback == NULL)
      return E_POINTER;

   paFeedback->Append(m_aFeedback);
   m_aFeedback.RemoveAll();

   return paFeedback->GetSize() > 0 ? S_OK : S_FALSE;
}
 
HRESULT CQuestion::GetActivityTimes(CPoint* pActivityTimePeriod)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;
   // Activity times are stored in an array of CPoints as pairs of beginning/end timestamps.
   // The String 'm_csActivity' can also contain text.
   CArray<CPoint, CPoint> aActivityTimePeriods;
   hr = StringManipulation::ExtractAllTimePeriodsFromString(m_csObjectActivity, &aActivityTimePeriods);
   if (SUCCEEDED(hr))
   {
      // A Question contains only one (activity) time period
      pActivityTimePeriod->x = aActivityTimePeriods[0].x;
      pActivityTimePeriod->y = aActivityTimePeriods[0].y;
   }
   return hr;
}

/////////////////////////////////////////////////////////////////////////////


CQuestionnaire::CQuestionnaire()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_nMaximumPoints = 0;
   m_nPassPoints = 0;
   m_bTypeRelative = false;   
   m_bShowEvaluation = false;
   m_bShowProgress = false;
   m_bDoStopmarks = false;
   m_bHasImageButtons = false;
   m_bContainsButtonConfig = false;
   m_bIsRandomTest = false;
   m_iNumberOfRandomQuestions = 0;
   m_aQuestions.SetSize(0, 15);
   m_aFeedback.SetSize(0, 10);
}

CQuestionnaire::~CQuestionnaire()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int i = 0;
   for (i=0; i<m_aQuestions.GetSize(); ++i)
      delete m_aQuestions[i];
   
   for (i=0; i<m_aFeedback.GetSize(); ++i)
      delete m_aFeedback[i];
}

HRESULT CQuestionnaire::Parse(SgmlElement *pElement, CString& csRecordPath) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    if (pElement == NULL)
        return E_POINTER;

    LPCTSTR szName = pElement->GetName();
    if (_tcscmp(szName, _T("QUESTIONNAIRE")) != 0)
        return E_INVALIDARG;

    // TODO these are TCHARs (or WCHARs) instead of the chars below
    LPCTSTR tszTitle = pElement->GetValue(_T("title"));
    if (tszTitle != NULL)
        m_csTitle = tszTitle;

    m_bShowEvaluation = pElement->GetBoolValue(_T("eval"));
    m_bShowProgress = pElement->GetBoolValue(_T("progress"));
    m_bDoStopmarks = pElement->GetBoolValue(_T("stops"));
    m_bHasImageButtons = pElement->GetBoolValue(_T("image-buttons"));

    m_bIsRandomTest = pElement->GetBoolValue(_T("randomizedQuestions"));
    int iRandomQuestionsNumber = pElement->GetIntValue(_T("randomizedQuestionsNumber"));

    LPCTSTR szSuccessAction = pElement->GetValue(_T("onSuccess"));
    if (szSuccessAction != NULL)
        m_csSuccessAction = szSuccessAction;

    LPCTSTR szFailureAction = pElement->GetValue(_T("onFailure"));
    if (szFailureAction != NULL)
        m_csFailureAction = szFailureAction;

    LPCTSTR tszOk = pElement->GetValue(_T("ok-t"));
    LPCTSTR tszSubmit = pElement->GetValue(_T("submit-t"));
    LPCTSTR tszDelete = pElement->GetValue(_T("delete-t"));

    if (tszOk != NULL)
        m_csOkText = tszOk;
    if (tszSubmit != NULL)
        m_csSubmitText = tszSubmit;
    if (tszDelete != NULL)
        m_csDeleteText = tszDelete;


    bool bPassFound = false;
    bool bQuestionFound = false;

    CArray<SgmlElement *, SgmlElement *> aElements;
    pElement->GetElements(aElements);
    for (int i = 0; i < aElements.GetCount() && SUCCEEDED(hr); ++i) {
        SgmlElement *pNext = aElements[i];
        if (pNext != NULL) {
            LPCTSTR szName = pNext->GetName();
            if (_tcscmp(szName, _T("pass")) == 0) {
                bPassFound = true;

                LPCTSTR szType = pNext->GetValue(_T("type"));
                LPCTSTR szValue = pNext->GetValue(_T("value"));

                if (szType == NULL || szValue == NULL)
                    hr = E_QUE_INVALID_DATA;

                int iNeededPoints = pNext->GetIntValue(_T("value"));

                if (_tcscmp(szType, _T("relative")) == 0)
                    m_bTypeRelative = true;

                m_nPassPoints = iNeededPoints; // can be 0 and can be a relative value
            } else if (_tcscmp(szName, _T("default")) == 0) {
                hr = ParseDefaultButtonConfig(pNext);
            } else if (_tcscmp(szName, _T("QUESTION")) == 0) {
                bQuestionFound = true;

                CQuestion *pQn = new CQuestion();
                hr = pQn->Parse(pNext, csRecordPath);

                if (SUCCEEDED(hr)) {
                    m_aQuestions.Add(pQn);
                    m_nMaximumPoints += pQn->GetAchievablePoints();
                }
                else
                    delete pQn;
            } else if (_tcscmp(szName, _T("feedback")) == 0) {
                CFeedbackDefinition *pFeedback = new CFeedbackDefinition();
                hr = pFeedback->Parse(pNext, csRecordPath);

                if (SUCCEEDED(hr))
                    m_aFeedback.Add(pFeedback);
                else
                    delete pFeedback;
            } else
                hr = E_QUE_INVALID_DATA; 
            // TODO this is a format error: maybe store somewhere what was not recognized?
        }
    }

    // default value of random question number is total number of questions.
    if (iRandomQuestionsNumber <= 0)
        iRandomQuestionsNumber = GetQuestionCount();
    m_iNumberOfRandomQuestions = iRandomQuestionsNumber;

    if (SUCCEEDED(hr)) {
        if (!bPassFound)
            hr = E_QUE_INVALID_DATA;
    }

    return hr;
}

void CQuestionnaire::GetInactiveButtonValues(COLORREF &clrFill, COLORREF &clrLine, 
                                             COLORREF &clrText, LOGFONT &lf, CString &csFilename)
{
   clrFill = m_aclrButtonFill[0];
   clrLine = m_aclrButtonLine[0];
   clrText = m_aclrButtonText[0];
   lf = m_alfButton[0];
   csFilename = m_acsButtonFilename[0];
}

void CQuestionnaire::GetNormalButtonValues(COLORREF &clrFill, COLORREF &clrLine, 
                                           COLORREF &clrText, LOGFONT &lf, CString &csFilename)
{
   clrFill = m_aclrButtonFill[1];
   clrLine = m_aclrButtonLine[1];
   clrText = m_aclrButtonText[1];
   lf = m_alfButton[1];
   csFilename = m_acsButtonFilename[1];
}

void CQuestionnaire::GetMouseoverButtonValues(COLORREF &clrFill, COLORREF &clrLine, 
                                              COLORREF &clrText, LOGFONT &lf, CString &csFilename)
{
   clrFill = m_aclrButtonFill[2];
   clrLine = m_aclrButtonLine[2];
   clrText = m_aclrButtonText[2];
   lf = m_alfButton[2];
   csFilename = m_acsButtonFilename[2];
}

void CQuestionnaire::GetPressedButtonValues(COLORREF &clrFill, COLORREF &clrLine, 
                                            COLORREF &clrText, LOGFONT &lf, CString &csFilename)
{
   clrFill = m_aclrButtonFill[3];
   clrLine = m_aclrButtonLine[3];
   clrText = m_aclrButtonText[3];
   lf = m_alfButton[3];
   csFilename = m_acsButtonFilename[3];
}

HRESULT CQuestionnaire::ExtractQuestions(CArray<CQuestion *, CQuestion *> *paQuestions)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (paQuestions == NULL)
      return E_POINTER;

   paQuestions->Append(m_aQuestions);
   m_aQuestions.RemoveAll();

   return paQuestions->GetSize() > 0 ? S_OK : S_FALSE;
}

HRESULT CQuestionnaire::ExtractFeedback(CArray<CFeedbackDefinition *, CFeedbackDefinition *> *paFeedback)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (paFeedback == NULL)
      return E_POINTER;

   paFeedback->Append(m_aFeedback);
   m_aFeedback.RemoveAll();

   return paFeedback->GetSize() > 0 ? S_OK : S_FALSE;
}

HRESULT CQuestionnaire::GetActivityTimes(CPoint *pActivitiyTimePeriod)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // Init with unreal timestamps
   int nBeginMs = 2147483647;
   int nEndMs = -1;

   CPoint timePeriodMs;
   for (int i = 0; i < m_aQuestions.GetSize(); ++i)
   {
      m_aQuestions[i]->GetActivityTimes(&timePeriodMs);
      if (timePeriodMs.x < nBeginMs)
         nBeginMs = timePeriodMs.x;
      if (timePeriodMs.y > nEndMs)
         nEndMs = timePeriodMs.y;
   }

   pActivitiyTimePeriod->x = nBeginMs;
   pActivitiyTimePeriod->y = nEndMs;

   // Unreal timestamps can occur if e.g. no question is defined
   if ( (nBeginMs >= 2147483647) || (nEndMs <= -1 ) )
      return S_FALSE;
   else
      return S_OK;
}

// private
HRESULT CQuestionnaire::ParseDefaultButtonConfig(SgmlElement *pConfigElement) {
    if (pConfigElement == NULL)
        return E_POINTER;

    if (_tcscmp(pConfigElement->GetName(), _T("default")) != 0)
        return E_INVALIDARG;

    CArray<SgmlElement *, SgmlElement *> aConfigElements;
    pConfigElement->GetElements(aConfigElements);
    for (int i = 0; i < aConfigElements.GetCount(); ++i) {
        SgmlElement *pButtonConfig = aConfigElements[i];
        if (pButtonConfig != NULL) {
            if (_tcscmp(pButtonConfig->GetName(), _T("button-config")) != 0)
                return E_QUE_INVALID_DATA;

            LPCTSTR szType = pButtonConfig->GetValue(_T("type"));

            if (szType == NULL)
                return E_QUE_INVALID_DATA;

            int iArrayIndex = -1;
            if (_tcscmp(szType, _T("passive")) == 0)
                iArrayIndex = 0;
            else if (_tcscmp(szType, _T("normal")) == 0)
                iArrayIndex = 1;
            else if (_tcscmp(szType, _T("over")) == 0)
                iArrayIndex = 2;
            else if (_tcscmp(szType, _T("down")) == 0)
                iArrayIndex = 3;
            else
                return E_QUE_INVALID_DATA;

            CArray<SgmlElement *, SgmlElement *> aButtonElements;
            pButtonConfig->GetElements(aButtonElements);
            for (int j = 0; j < aButtonElements.GetCount(); ++j) {
                SgmlElement *pDetailConfig = aButtonElements[j];
                if (pDetailConfig != NULL) {
                    if (_tcscmp(pDetailConfig->GetName(), _T("colors")) == 0) {
                        m_aclrButtonFill[iArrayIndex] = pDetailConfig->GetIntValue(_T("bg"));
                        m_aclrButtonLine[iArrayIndex] = pDetailConfig->GetIntValue(_T("line"));
                        m_aclrButtonText[iArrayIndex] = pDetailConfig->GetIntValue(_T("text"));
                    } else if (_tcscmp(pDetailConfig->GetName(), _T("font")) == 0) {
                        LFont::FillWithDefaultValues(&m_alfButton[iArrayIndex]);
                        m_alfButton[iArrayIndex].lfHeight = pDetailConfig->GetIntValue(_T("size"));
                        m_alfButton[iArrayIndex].lfWeight = pDetailConfig->GetIntValue(_T("weight"));
                        m_alfButton[iArrayIndex].lfItalic = pDetailConfig->GetIntValue(_T("slant"));
                        _tcscpy(m_alfButton[iArrayIndex].lfFaceName, pDetailConfig->GetValue(_T("family")));
                    } else if (_tcscmp(pDetailConfig->GetName(), _T("file")) == 0) {
                        m_acsButtonFilename[iArrayIndex] = pDetailConfig->GetValue(_T("path"));
                    } else
                        return E_QUE_INVALID_DATA;
                }
            }
        }
    }

    m_bContainsButtonConfig = true;

    return S_OK;
}
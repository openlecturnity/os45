#include "StdAfx.h"
#include "ButtonAppearanceSettings.h"
#include "InteractionButtonDialog.h"
#include "editorDoc.h"
#include "EditInteractionDialog.h"
#include "lutils.h"
#include "MfcUtils.h"

#include "WhiteboardView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CButtonAppearanceSettings::CButtonAppearanceSettings(bool bDemo) {
    Init(bDemo);
}

CButtonAppearanceSettings::~CButtonAppearanceSettings() {
}

void CButtonAppearanceSettings::Init(bool bDemo) {
    for (int i = 0; i < ACTION_EVENT_COUNT; ++i) {
        m_csFileNames[i].Empty();
        m_siImageDimension[i].cx = 0;
        m_siImageDimension[i].cy = 0;
        LFont::FillWithDefaultValues(&m_lf[i], _T("Segoe UI"));
        m_nLineWidth[i] = 1;
        m_nLineStyle[i] = 0;
    }

    m_clrFill[ACTION_PASSIV] = RGB(244, 255, 244);//RGB(171, 199, 238);  
    m_clrLine[ACTION_PASSIV] = RGB(140, 178, 32);//RGB(163, 193, 245);
    m_clrText[ACTION_PASSIV] = RGB(159, 174, 204);

    m_clrFill[ACTION_NORMAL] = RGB(255, 255, 255);//RGB(171, 199, 238);  
    m_clrLine[ACTION_NORMAL] = RGB(140, 178, 32);//RGB(163, 193, 245);
    m_clrText[ACTION_NORMAL] = RGB(140, 178, 32);//RGB(0, 0, 0);

    m_clrFill[ACTION_OVER] = RGB(234, 234, 234);//RGB(255, 228, 176);  
    m_clrLine[ACTION_OVER] = RGB(140, 178, 32);//RGB(0, 0, 128);
    m_clrText[ACTION_OVER] = RGB(140, 178, 32);

    m_clrFill[ACTION_PRESSED] = RGB(178, 178, 178);  
    m_clrLine[ACTION_PRESSED] = RGB(140, 178, 32);//RGB(0, 0, 128);
    m_clrText[ACTION_PRESSED] = RGB(140, 178, 32);

    m_bIsImageButton = false;

    CString csImagePath = CButtonAppearanceSettings::GetButtonImagesPath();

    if (!csImagePath.IsEmpty()) {
		if(bDemo)
			csImagePath += _T("\\Metro\\Small\\");
		else
			csImagePath += _T("\\Metro\\Medium\\");
        m_csFileNames[ACTION_PASSIV] = csImagePath + _T("Passive.png");
        m_csFileNames[ACTION_NORMAL] = csImagePath + _T("Normal.png");
        m_csFileNames[ACTION_OVER] = csImagePath + _T("MouseOver.png");
        m_csFileNames[ACTION_PRESSED] = csImagePath + _T("MouseDown.png");
    }

}

CString CButtonAppearanceSettings::GetButtonImagesPath() {
    CString csButtonPath;

    unsigned long ulMaxLength = MAX_PATH;
    _TCHAR tszString[MAX_PATH];
    bool bSuccess = LRegistry::ReadSettingsString(_T(""), _T("InstallDir"), tszString, &ulMaxLength, NULL);
    if (bSuccess) {
        csButtonPath = tszString;
        csButtonPath += _T("Studio\\ButtonImages");

        if (_taccess(csButtonPath, 00) != 0) {
            ::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), 
                _T("Unexpected: Button directory does not exist."), _T("LECTURNITY"), MB_OK | MB_ICONERROR);
            csButtonPath = _T("");
        }
    }

    return csButtonPath;
}

void CButtonAppearanceSettings::CreateDemoButton(CEditorProject *pProject, CInteractionAreaEx *pInteraction, CRect &rcArea, int iTimestampPos){

    CString csImagePath = CButtonAppearanceSettings::GetButtonImagesPath();
    // Warning! This will overrite default values.
   /* if (!csImagePath.IsEmpty()) {
        csImagePath += _T("\\Windows\\Small\\Blue\\");
        m_csFileNames[ACTION_PASSIV] = csImagePath + _T("Passive.png");
        m_csFileNames[ACTION_NORMAL] = csImagePath + _T("Normal.png");
        m_csFileNames[ACTION_OVER] = csImagePath + _T("MouseOver.png");
        m_csFileNames[ACTION_PRESSED] = csImagePath + _T("MouseDown.png");
    }*/
    CString csButtonText;
    csButtonText.LoadString(IDS_NEXT);
    m_bIsImageButton = false;


    // make the button size same as "passive" image size
    CSize siButton(0, 0);
    //CInteractionAreaEx::CalculateButtonSize(siButton, m_csFileNames[0]);
    CInteractionAreaEx::CalculateButtonSize(siButton, csButtonText, &m_lf[0]);

    /*rcArea.right = rcArea.left + siButton.cx;
    rcArea.bottom = rcArea.top + siButton.cy;*/

    CMouseAction mouseAction;
    mouseAction.Init(pProject, INTERACTION_START_REPLAY, INTERACTION_START_REPLAY, _T(""), INTERACTION_NO_ACTION);

    AreaPeriodId nVisualityPeriodId = INTERACTION_PERIOD_TIME;
    UINT nVisualityStartMs = iTimestampPos;
    UINT nVisualityEndMs = iTimestampPos;

    AreaPeriodId nActivityPeriodId = INTERACTION_PERIOD_TIME;
    UINT nActivityStartMs = iTimestampPos;
    UINT nActivityEndMs = iTimestampPos;

    CTimePeriod visibility;
    visibility.Init(nVisualityPeriodId, nVisualityStartMs, nVisualityEndMs);
    CTimePeriod activity;
    activity.Init(nActivityPeriodId, nActivityStartMs, nActivityEndMs);

    pInteraction->Init(&rcArea, false, &mouseAction, NULL, NULL, NULL, NULL, 
        &visibility, &activity, pProject);
    pInteraction->SetDemoDocumentObject(true);

//    for (int i = 0; i < ACTION_EVENT_COUNT; ++i) {
//        if (!m_csFileNames[i].IsEmpty())
//        {
//            int iStringLength = m_csFileNames[i].GetLength();
//            WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR)); // +1 for null termination
//#ifdef _UNICODE
//            wcscpy(wcString, m_csFileNames[i]);
//#else
//            MultiByteToWideChar(CP_ACP, 0, m_csFileNames[i], iStringLength+1, 
//                wcString, iStringLength+1);
//#endif
//
//            Gdiplus::Image *m_pGdipImage = Gdiplus::Image::FromFile(wcString, FALSE);
//            if (m_pGdipImage) {
//                m_siImageDimension[i].cx = m_pGdipImage->GetWidth();
//                m_siImageDimension[i].cy = m_pGdipImage->GetHeight();
//            }
//
//            if (wcString)
//                free(wcString);
//        }
//    }

    CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects[ACTION_EVENT_COUNT];
    for (int i = 0; i < ACTION_EVENT_COUNT; ++i) {
        /*CSize siButton(0, 0);
        CInteractionAreaEx::CalculateButtonSize(siButton, m_csFileNames[i]);
        rcArea.right = rcArea.left + siButton.cx;
        rcArea.bottom = rcArea.top + siButton.cy;*/
        //LFont::FillWithDefaultValues(&m_lf[i]);
        //m_nLineWidth[i] = 1;
        //m_nLineStyle[i] = 0;

       /* CInteractionAreaEx::CreateButtonObjects(&aObjects[i], rcArea, m_siImageDimension[i], m_csFileNames[i],
            csButtonText, m_clrText[i], &m_lf[i]);*/
        CInteractionAreaEx::CreateButtonObjects(&aObjects[i], rcArea, m_clrFill[i], m_clrLine[i], m_nLineWidth[i], m_nLineStyle[i], csButtonText, m_clrText[i], &m_lf[i]);  
    }

    pInteraction->InitButton(BUTTON_TYPE_OTHER, &(aObjects[ACTION_NORMAL]), 
        &(aObjects[ACTION_OVER]), &(aObjects[ACTION_PRESSED]), &(aObjects[ACTION_PASSIV]), true);

    for (i = 0; i < ACTION_EVENT_COUNT; ++i) {
        for (int j = 0; j < aObjects[i].GetSize(); ++j) {
            DrawSdk::DrawObject *pObject = aObjects[i][j];
            if (pObject)
                delete pObject;
        }
    }
}

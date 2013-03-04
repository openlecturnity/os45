#pragma once

class CInteractionAreaEx;
class CEditorProject;
/**
 * 
*/
class CButtonAppearanceSettings {
public:
	CButtonAppearanceSettings(bool bDemo = false);
    ~CButtonAppearanceSettings();

	void Init(bool bDemo = false);

    /**
     * Set back the buttons default values. The \\Aqua2\\Medium\\Blue\\ images will be set.
     */
    void ReInitialize(){Init();};

    /**
     * Creates demo mode button.
     */
    void CreateDemoButton(CEditorProject *pProject, CInteractionAreaEx *pInteraction, CRect &rcArea, int iTimestampPos);

    static CString GetButtonImagesPath();

    enum ButtonSettings{
        ACTION_EVENT_COUNT = 4,
        ACTION_PASSIV = 0,
        ACTION_NORMAL = 1,
        ACTION_OVER = 2,
        ACTION_PRESSED = 3,
        TEXT_BUTTON = 0,
        IMAGE_BUTTON = 1
    };
	

public:
    bool m_bIsImageButton;

    COLORREF m_clrLine[CButtonAppearanceSettings::ACTION_EVENT_COUNT];
    COLORREF m_clrFill[CButtonAppearanceSettings::ACTION_EVENT_COUNT];
    COLORREF m_clrText[CButtonAppearanceSettings::ACTION_EVENT_COUNT];

    LOGFONT m_lf[CButtonAppearanceSettings::ACTION_EVENT_COUNT];

    CString m_csFileNames[CButtonAppearanceSettings::ACTION_EVENT_COUNT];

    CSize m_siImageDimension[CButtonAppearanceSettings::ACTION_EVENT_COUNT];

    UINT m_nLineWidth[CButtonAppearanceSettings::ACTION_EVENT_COUNT];
    UINT m_nLineStyle[CButtonAppearanceSettings::ACTION_EVENT_COUNT];
};

#pragma once

class LSUTL32_EXPORT CTransparentStatic : public CStatic {
    DECLARE_DYNAMIC(CTransparentStatic)

public:
    CTransparentStatic();
    virtual ~CTransparentStatic();

    void SetTextColor(COLORREF clrForeground);

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();

private:
    COLORREF m_clrForeground;
};

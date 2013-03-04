#ifndef __COLOR_STATIC_WITH_TEXT
#define __COLOR_STATIC_WITH_TEXT

#pragma once


// CColorStatic

class CColorStaticWithText : public CStatic
{
	DECLARE_DYNAMIC(CColorStaticWithText)

public:
	CColorStaticWithText();
	virtual ~CColorStaticWithText();

protected:
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnPaint();
};

#endif


// GraphicalObjectEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GraphicalObjectEditCtrl.h"
#include "MiscFunctions.h"


// CGraphicalObjectEditCtrl

IMPLEMENT_DYNAMIC(CGraphicalObjectEditCtrl, CRichEditCtrl)

CGraphicalObjectEditCtrl::CGraphicalObjectEditCtrl() {
    m_dHeight = 0;
    m_rcCalloutDimension = CRect(0,0,0,0);
    m_bOffsetsChanged = false;
    m_iDemoModeButtonHeight = 0;
    m_rcEdit = CRect(0,0,0,0);
    m_nDevicePixelsX = 96; //Default
    m_nDevicePixelsY = 96;

}

CGraphicalObjectEditCtrl::~CGraphicalObjectEditCtrl() {
}


BEGIN_MESSAGE_MAP(CGraphicalObjectEditCtrl, CRichEditCtrl)
    //{{AFX_MSG_MAP(CGraphicalObjectEditCtrl)
    ON_WM_ERASEBKGND()
    ON_WM_CHAR()
    //}}AFX_MSG_MAP
    ON_NOTIFY_REFLECT(EN_REQUESTRESIZE, &CGraphicalObjectEditCtrl::OnEnRequestresize)
    ON_WM_CREATE()
    ON_NOTIFY_REFLECT(EN_SELCHANGE, &CGraphicalObjectEditCtrl::OnEnSelchange)
END_MESSAGE_MAP()


// CGraphicalObjectEditCtrl message handlers

BOOL CGraphicalObjectEditCtrl::OnEraseBkgnd(CDC* pDC) {
    m_nDevicePixelsX = pDC->GetDeviceCaps(LOGPIXELSX);
    m_nDevicePixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
    return TRUE; 
}

void CGraphicalObjectEditCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)  {
    if (nChar == 9) {   // TAB
        return;
    }

    int nLineCount = GetLineCount();
    CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);
    CString csWindowText = _T("");
    GetWindowText(csWindowText);
    GetParent()->SendMessage(WM_USER + 136, 0, 0);
   // CalculatePosition(csWindowText);
}

void CGraphicalObjectEditCtrl::CalculatePosition(CString csWindowText) {
    CString csEditText = _T("");
    GetWindowText(csEditText);
    if(!csEditText.IsEmpty()) {
        //SetWindowText(csWindowText);
        SetSel(csEditText.GetLength(), csEditText.GetLength());
    }
    //UINT nLastCharPos = csWindowText.GetLength() - 1;
    //CPoint ptLastChar = GetCharPos(nLastCharPos);
    //int nFontSize = abs(m_logFont.lfHeight);
    //if ((m_dHeight != (ptLastChar.y + 1.2*nFontSize)) || m_bOffsetsChanged) {
    //    m_dHeight = ptLastChar.y + 1.2*nFontSize;
    //    double yPos = m_rcCalloutDimension.top + (m_rcCalloutDimension.Height() - m_dHeight - m_iDemoModeButtonHeight)/2; 
    //    m_rcEdit.left = m_rcCalloutDimension.left + TEXT_BORDER;
    //    m_rcEdit.top = yPos;
    //    m_rcEdit.right = m_rcEdit.left + m_rcCalloutDimension.Width() - 2*TEXT_BORDER;
    //    m_rcEdit.bottom = m_rcEdit.top + m_dHeight;
    //    MoveWindow(&m_rcEdit, TRUE);
    //    //MoveWindow(m_rcCalloutDimension.left + TEXT_BORDER, yPos, m_rcCalloutDimension.Width() - 2*TEXT_BORDER, m_dHeight, true);

    //    if(m_bOffsetsChanged) {
    //        m_bOffsetsChanged = false;
    //    }
    //} 
}

BOOL CGraphicalObjectEditCtrl::PreTranslateMessage(MSG* pMsg)  {
    if (IsWindowVisible()) {
        if (pMsg->hwnd == GetSafeHwnd()) {
            if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP || pMsg->message == WM_CHAR) {
                ::TranslateMessage(pMsg);
                ::DispatchMessage(pMsg);
                return TRUE;
            }
        }
    }

    return CRichEditCtrl::PreTranslateMessage(pMsg);
}

void CGraphicalObjectEditCtrl::SetOffsets(CPoint ptOffsets) {
    m_ptOffsets = ptOffsets;
    m_bOffsetsChanged = true;
}

int CGraphicalObjectEditCtrl::GetLineHeight(int iLine) {
    int iLineCount = GetLineCount();
    if (iLine > iLineCount)
        return -1;
    int iStartIndex = LineIndex(iLine);
    int iEndIndex = LineIndex(iLine + 1);

    CPoint ptTop = GetCharPos(iStartIndex);
    CPoint ptBottom = m_rcEdit.BottomRight();
    ptBottom.y -= m_rcEdit.top;
    if (iEndIndex != -1)
        ptBottom = GetCharPos(iEndIndex);
    return ptBottom.y - ptTop.y;
}

int CGraphicalObjectEditCtrl::GetWordYOffset(LOGFONT lf, int iLine, int iMaxDescent) {

    int iLineHeight = GetLineHeight(iLine);
    CDC* pDC = GetDC();
    int iSavedDc = pDC->SaveDC();
    CFont font;
    font.CreateFontIndirect(&lf);
    pDC->SelectObject(&font);
    TEXTMETRIC tm;
    GetTextMetrics(pDC->m_hDC, &tm);
    int descent = (int)tm.tmDescent;
    int textHeight = (int)tm.tmHeight;
    pDC->RestoreDC(iSavedDc);
    
	return (iMaxDescent - textHeight) * 72.0 / m_nDevicePixelsY + 0.5;
}

int CGraphicalObjectEditCtrl::GetWordDescent(LOGFONT lf, int iLine) {
    int iLineHeight = GetLineHeight(iLine);
    CDC* pDC = GetDC();
    int iSavedDc = pDC->SaveDC();
    CFont font;
    font.CreateFontIndirect(&lf);
    pDC->SelectObject(&font);
    TEXTMETRIC tm;
    GetTextMetrics(pDC->m_hDC, &tm);
    int descent = (int)tm.tmDescent;
    int textHeight = (int)tm.tmHeight;
    pDC->RestoreDC(iSavedDc);
	return textHeight;
}

void CGraphicalObjectEditCtrl::GetEditLines(CArray<SEditLine, SEditLine>&caEditLines) {
    int iLineCount = GetLineCount();
    CString csWindowText;
    GetWindowText(csWindowText);
    CString csLt;
    
    for (int i = 0 ; i < iLineCount; i++)
    {
        CArray<SEditLine, SEditLine>aTemp;
        csLt.Empty();
        SEditLine seLine;
        int iStartIndex = LineIndex(i);
        
        int iEndIndex =  LineLength(iStartIndex);

        if (iStartIndex != -1)
        {
            int iLineLength = LineLength(iStartIndex);
            int x = GetLine(i, csLt.GetBuffer(iLineLength + sizeof(TCHAR)), iLineLength + sizeof(TCHAR));
            csLt.ReleaseBuffer(iLineLength);//(iLineLength);
        }

        CArray<CString, CString> aLineStrs;
        SetSel(iStartIndex, iStartIndex + 1);
        //COLORREF cl = GetSelectionColor();
        CRichEditCharacterFormat cf = GetSelFormat();
        int st = 0;
        int ct = 0;
        int iFontSize = 0;
        int iMaxFont = 0;
        int iMaxIndex = -1;
        for (int j = 1; j < csLt.GetLength(); j++) {
            SetSel(iStartIndex + j, iStartIndex + j +1);
            //COLORREF clr = GetSelectionColor();
            CRichEditCharacterFormat cfNext = GetSelFormat();
            ct++;
            if (cf != cfNext)
            {
                CString csPart = csLt.Mid(st, ct);
                
                
                seLine.iOrigLineNumber = i;
                seLine.csText = csPart;//csLineText;
                seLine.iStartIndex = st;
                seLine.iEndIndex = st + ct;
                
                //seLine.clr = cl;
                seLine.clr = cf.m_clrTextColor;
                LOGFONT lf;
                CString csFontWeight, csFontslant;
                csFontWeight = cf.m_bIsBold ? _T("bold") : _T("");
                csFontslant = cf.m_bIsItalic ? _T("italic") : _T("");
                iFontSize = cf.m_iFontSize * m_nDevicePixelsY / 72;
                if(iMaxFont < iFontSize){
                    iMaxFont = iFontSize;
                    iMaxIndex ++;
                }
                FontManipulation::Create(cf.m_csFontName, iFontSize/*cf.m_iFontSize*/, csFontWeight, csFontslant, false, lf);
                memcpy(&seLine.lf, &lf, sizeof(LOGFONT));

                CPoint ptStart = PosFromChar(iStartIndex + st);
                int iHeight = GetLineHeight(i);//m_rcEdit.Height() / iLineCount;
                //ptStart.y += yOffset;
                CRect rcline;
                CPoint ptEnd;
                ptEnd.x = ptStart.x + 7 * csPart.GetLength();//csLt.GetLength(); // TODO soare: calculate correctly
                ptEnd.y = ptStart.y + iHeight;
                rcline.SetRect(ptStart, ptEnd);//CPoint(10, iHeight));
                rcline.OffsetRect(m_rcEdit.TopLeft());

				seLine.iOrigLineHeight = iHeight;
                seLine.rcLine = rcline;
                TRACE(_T("!!GOe x=%d, y=%d\n"), rcline.left, rcline.top);

                //caEditLines.Add(seLine);
                aTemp.Add(seLine);
                st += ct;
                ct = 0;
                //cl = clr;
                cf = cfNext;
            }
        }

        if (st < csLt.GetLength()) {
            ct++;
            CString csPart = csLt.Mid(st, ct);
            if(!csPart.IsEmpty()) { 
				seLine.iOrigLineNumber = i;
                seLine.csText = csPart;//csLineText;
                seLine.iStartIndex = st;
                seLine.iEndIndex = st + ct;
                
                //seLine.clr = cl;
                seLine.clr = cf.m_clrTextColor;
                LOGFONT lf;
                CString csFontWeight, csFontslant;
                csFontWeight = cf.m_bIsBold ? _T("bold") : _T("");
                csFontslant = cf.m_bIsItalic ? _T("italic") : _T("");
                iFontSize = cf.m_iFontSize * m_nDevicePixelsY / 72;
                if(iMaxFont < iFontSize){
                    iMaxFont = iFontSize;
                    iMaxIndex ++;
                }
                FontManipulation::Create(cf.m_csFontName, iFontSize/*cf.m_iFontSize*/, csFontWeight, csFontslant, false, lf);
                memcpy(&seLine.lf, &lf, sizeof(LOGFONT));

                CPoint ptStart = PosFromChar(iStartIndex + st);
                int iHeight = GetLineHeight(i);//m_rcEdit.Height() / iLineCount;
                //ptStart.y += yOffset;
                CRect rcline;
                CPoint ptEnd;
                ptEnd.x = ptStart.x + 7 * csPart.GetLength();//csLt.GetLength(); // TODO soare: calculate correctly
                ptEnd.y = ptStart.y + iHeight;
                rcline.SetRect(ptStart, ptEnd);//CPoint(10, iHeight));
                rcline.OffsetRect(m_rcEdit.TopLeft());

				seLine.iOrigLineHeight = iHeight;
                seLine.rcLine = rcline;
                TRACE(_T("!!GOe x=%d, y=%d\n"), rcline.left, rcline.top);

                //caEditLines.Add(seLine);
                aTemp.Add(seLine);
            }
        }
        int iMaxDescent = -1;
        int iaCount = aTemp.GetCount();
        if (iaCount > 0 && aTemp.GetAt(aTemp.GetCount() -1).csText == _T("\r"))
            iaCount --;
        for (int k = 0; k < iaCount/*aTemp.GetCount()*/; k++) {
            //caEditLines.Add(aTemp.GetAt(i));
            int dsc = GetWordDescent(aTemp.GetAt(k).lf, i);
            if (dsc > iMaxDescent)
                iMaxDescent = dsc;
        }

        for (int kk = 0; kk < iaCount; kk ++) {
            int yOff = GetWordYOffset(aTemp.GetAt(kk).lf, i, iMaxDescent);
            aTemp.GetAt(kk).rcLine.top += yOff;
            caEditLines.Add(aTemp.GetAt(kk));
        }
        aTemp.RemoveAll();
        //CPoint ptStart = PosFromChar(iStartIndex);
        //int iHeight = m_rcEdit.Height() / iLineCount;
        //seLine.csText = csLt;//csLineText;
        //seLine.iStartIndex = iStartIndex;
        //seLine.iEndIndex = iEndIndex;
        //CRect rcline;
        //CPoint ptEnd;
        //ptEnd.x = ptStart.x + 7 * csLt.GetLength(); // TODO soare: calculate correctly
        //ptEnd.y = ptStart.y + iHeight;
        //rcline.SetRect(ptStart, ptEnd);//CPoint(10, iHeight));
        //rcline.OffsetRect(m_rcEdit.TopLeft());

        //seLine.rcLine = rcline;
        //caEditLines.Add(seLine);
    }
}

DWORD CALLBACK CGraphicalObjectEditCtrl::RichEditStreamInCallback(FOP_DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	RichEditStreamInStruct* pstr = (RichEditStreamInStruct*) dwCookie;

	LPCSTR psz = pstr->m_psz;
	psz += pstr->m_lOffset;

	if (pstr->m_lLength > pstr->m_lOffset + cb)
	{
		strncpy((LPSTR) pbBuff, psz, cb);
		*pcb = cb;
	}
	else
	{
		strcpy((LPSTR) pbBuff, psz);
		*pcb = (long)strlen(psz);
	}

	pstr->m_lOffset += *pcb;

	return 0;
}

DWORD CALLBACK CGraphicalObjectEditCtrl::RichEditStreamOutCallback(FOP_DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	RichEditStreamOutStruct* pstr = (RichEditStreamOutStruct*) dwCookie;

	if (cb > 0)
	{
		CString* psResult = pstr->m_pString;
		LPTSTR pszBuffer = psResult->GetBuffer(cb+1+pstr->m_lOffset) + pstr->m_lOffset;

#ifndef _UNICODE
		strcpy(pszBuffer, (LPSTR) pbBuff);
#else
		MultiByteToWideChar(CP_ACP, 0, (LPSTR) pbBuff, cb,
			pszBuffer, cb+1);
#endif

		psResult->ReleaseBuffer();

		*pcb = cb;

		pstr->m_lOffset += cb;

		return 0;
	}

	return 1;
}

void AFXAPI CGraphicalObjectEditCtrl::SetRichText(CRichEditCtrl* pWnd, LPCSTR szRTFInput)
{
	RichEditStreamInStruct es = { szRTFInput, (long)strlen(szRTFInput), 0 };
	EDITSTREAM esin = {(FOP_DWORD) &es, 0, RichEditStreamInCallback};
	pWnd->StreamIn(SF_RTF, esin);
}

void AFXAPI CGraphicalObjectEditCtrl::GetRichText(CRichEditCtrl* pWnd, CString& csOutput)//szRTFInput)
{
	//_FOPEditStreamInStruct es = { szRTFInput, (long)strlen(szRTFInput), 0 };
    RichEditStreamOutStruct es = {&csOutput, 0};
    EDITSTREAM esin = {(FOP_DWORD) &es, 0, RichEditStreamOutCallback};
	pWnd->StreamOut(SF_RTF, esin);
}

void CGraphicalObjectEditCtrl::FixPos()
{
        /*ScaleAndOffsetRect(rcDimension, m_ptOffsets, m_fZoomFactor);*/
        CRect rcDim = m_rcCalloutDimension;
        rcDim.bottom -= m_iDemoModeButtonHeight;
        CPoint ptCenter = rcDim.CenterPoint();//m_rcCalloutDimension.CenterPoint();
        CRect rcPrev = m_rcEdit;
        m_rcEdit.left = m_rcCalloutDimension.left  + TEXT_BORDER;/*+ m_iPadding*/ ;//+ EDIT_CTRL_BORDER * m_fZoomFactor ;
        m_rcEdit.right = m_rcCalloutDimension.right - TEXT_BORDER; /*- m_iPadding*/;// - EDIT_CTRL_BORDER * m_fZoomFactor ;
        m_rcEdit.top = ptCenter.y - rcPrev.Height() / 2;//m_rcEdit.Height() / 2/* + 1*/;
        m_rcEdit.bottom = m_rcEdit.top + rcPrev.Height();//m_rcEdit.Height();

        MoveWindow(&m_rcEdit, TRUE);
}

void CGraphicalObjectEditCtrl::OnEnRequestresize(NMHDR *pNMHDR, LRESULT *pResult)
{
    REQRESIZE *pReqResize = reinterpret_cast<REQRESIZE *>(pNMHDR);
    // TODO:  The control will not send this notification unless you override the
    // CRichEditCtrl::OnInitDialog() function to send the EM_SETEVENTMASK message
    // to the control with the ENM_REQUESTRESIZE flag ORed into the lParam mask.

    // TODO:  Add your control notification handler code here

    CRect rcText;
    rcText.SetRect(pReqResize->rc.left, pReqResize->rc.top, pReqResize->rc.right, pReqResize->rc.bottom);
    if (m_rcEdit.Height() != rcText.Height()) 
	{
        //TRACE(_T("!!!Soare reqSize: l:%d, t:%d, r:%d, b:%d\n"), pReqResize->rc.left, pReqResize->rc.top, pReqResize->rc.right, pReqResize->rc.bottom); 
        
        /*ScaleAndOffsetRect(rcDimension, m_ptOffsets, m_fZoomFactor);*/
        CRect rcDim = m_rcCalloutDimension;
        rcDim.bottom -= m_iDemoModeButtonHeight;
        CPoint ptCenter = rcDim.CenterPoint();//m_rcCalloutDimension.CenterPoint();
        
        m_rcEdit.left = m_rcCalloutDimension.left  + TEXT_BORDER;/*+ m_iPadding*/ ;//+ EDIT_CTRL_BORDER * m_fZoomFactor ;
        m_rcEdit.right = m_rcCalloutDimension.right - TEXT_BORDER; /*- m_iPadding*/;// - EDIT_CTRL_BORDER * m_fZoomFactor ;
        m_rcEdit.top = ptCenter.y - rcText.Height() / 2/* + 1*/;
        m_rcEdit.bottom = m_rcEdit.top + rcText.Height();
       
        MoveWindow(&m_rcEdit, TRUE);
    }


    *pResult = 0;
}

int CGraphicalObjectEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  Add your specialized creation code here

    return 0;
}

void CGraphicalObjectEditCtrl::GetSelectionFormatParameters(bool &bIsBold, bool &bIsItalic, CString &csFontName, long & iFontSize, COLORREF &clrTextColor, int &iAlign) {
    bIsBold = SelectionIsBold();
    bIsItalic = SelectionIsItalic();
    csFontName = GetSelectionFontName();
    iFontSize = ceil((double)( GetSelectionFontSize() * m_nDevicePixelsY / 72));
    clrTextColor = GetSelectionColor();
    PARAFORMAT pf;
    pf.cbSize = sizeof(PARAFORMAT);
    pf.dwMask = PFM_ALIGNMENT;  
    GetParaFormat(pf);
    if((pf.wAlignment & 0xFF) == PFA_LEFT)
        iAlign = 0;
    else if((pf.wAlignment & 0xFF) == PFA_CENTER)
        iAlign = 1;
    else if((pf.wAlignment & 0xFF) == PFA_RIGHT)
        iAlign = 2;
    else
        iAlign = 1;
}

// Protected formatting methods
CRichEditCharacterFormat CGraphicalObjectEditCtrl::GetSelFormat() {
    
    CRichEditCharacterFormat recf;
    recf.m_bIsBold = SelectionIsBold();
    recf.m_bIsItalic = SelectionIsItalic();
    recf.m_iFontSize = GetSelectionFontSize();//GetFontSize();//
    recf.m_csFontName = GetSelectionFontName();
    recf.m_clrTextColor = GetSelectionColor();

    return recf;
}

void CGraphicalObjectEditCtrl::SetDefaultColor(COLORREF clrText) {
   
    long iStart, iEnd;
    GetSel(iStart, iEnd);
    SetSel(0,-1);
    SetSelectionColor(clrText);
    SetSel(iStart, iEnd);
}

CHARFORMAT CGraphicalObjectEditCtrl::GetSelCharFormat(DWORD dwMask) {

    CHARFORMAT cf;
    cf.cbSize = sizeof(CHARFORMAT);

    cf.dwMask = dwMask;

    GetSelectionCharFormat(cf);

    return cf;
}

void CGraphicalObjectEditCtrl::SetSelCharStyle(int MASK, int STYLE, int nStart, int nEnd) {

	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);
	//cf.dwMask = MASK;
	
	GetSelectionCharFormat(cf);
	
	if (cf.dwMask & MASK)	// selection is all the same
	{
		cf.dwEffects ^= STYLE; 
	}
	else
	{
		cf.dwEffects |= STYLE;
	}
	
	cf.dwMask = MASK;

	SetSelectionCharFormat(cf);

}
// Public formatting methods
bool CGraphicalObjectEditCtrl::SelectionIsBold() {

    CHARFORMAT cf = GetSelCharFormat();	

    if (cf.dwEffects & CFM_BOLD)
        return true;
    else
        return false;
}

bool CGraphicalObjectEditCtrl::SelectionIsItalic() {

    CHARFORMAT cf = GetSelCharFormat();	

    if (cf.dwEffects & CFM_ITALIC)
        return true;
    else
        return false;
}

void CGraphicalObjectEditCtrl::SetSelectionBold() {

	long start=0, end=0;
	GetSel(start, end);		// Get the current selection

	SetSelCharStyle(CFM_BOLD, CFE_BOLD, start, end);	// Make it bold
}

void CGraphicalObjectEditCtrl::SetSelectionItalic() {

	long start=0, end=0;
	GetSel(start, end);

	SetSelCharStyle(CFM_ITALIC, CFE_ITALIC, start, end);
}

long CGraphicalObjectEditCtrl::GetSelectionFontSize() {

	CHARFORMAT cf = GetSelCharFormat();

	long nSize = ceil(cf.yHeight/20.0);

	return nSize;
}

CString CGraphicalObjectEditCtrl::GetSelectionFontName() {

	CHARFORMAT cf = GetSelCharFormat();

	CString csName = cf.szFaceName;

	return csName;
}

void CGraphicalObjectEditCtrl::SetSelectionFontSize(int nPointSize) {

    int currSize = GetSelectionFontSize();
	CHARFORMAT cf = GetSelCharFormat();

	nPointSize *= 20;	// convert from to twips
    cf.yHeight = (long) (nPointSize * 72/m_nDevicePixelsY /*+ 0.5*/);
	
	cf.dwMask = CFM_SIZE;

	SetSelectionCharFormat(cf);
}

void CGraphicalObjectEditCtrl::SetSelectionFontName(CString csFontName) {

	CHARFORMAT cf = GetSelCharFormat();

	// Set the font name.
	for (int i = 0; i <= csFontName.GetLength()-1; i++)
		cf.szFaceName[i] = csFontName[i];

    cf.szFaceName[i] = _T('\0');

	cf.dwMask = CFM_FACE;

	SetSelectionCharFormat(cf);
}

void CGraphicalObjectEditCtrl::SetSelectionColor(COLORREF color) {

	CHARFORMAT cf = GetSelCharFormat();

	if (cf.dwEffects & CFE_AUTOCOLOR) cf.dwEffects -= CFE_AUTOCOLOR;
	
	cf.crTextColor = color;

	cf.dwMask = CFM_COLOR;

	SetSelectionCharFormat(cf);
}

COLORREF CGraphicalObjectEditCtrl::GetSelectionColor() {
    /*CHARFORMAT cf;
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_COLOR;*/

    CHARFORMAT cf = GetSelCharFormat();

    COLORREF cl = cf.crTextColor;
    return cl;
}

void CGraphicalObjectEditCtrl::SetParagraphLeft() {
    PARAFORMAT paraFormat;
	paraFormat.cbSize = sizeof(PARAFORMAT);
	paraFormat.dwMask = PFM_ALIGNMENT;    
	paraFormat.wAlignment = PFA_LEFT;
	
	SetParaFormat(paraFormat);
}

void CGraphicalObjectEditCtrl::SetParagraphCenter() {
    PARAFORMAT paraFormat;
	paraFormat.cbSize = sizeof(PARAFORMAT);
	paraFormat.dwMask = PFM_ALIGNMENT;    
    paraFormat.wAlignment = PFA_CENTER;
	
	SetParaFormat(paraFormat);

}

void CGraphicalObjectEditCtrl::SetParagraphRight() {
    PARAFORMAT paraFormat;
	paraFormat.cbSize = sizeof(PARAFORMAT);
	paraFormat.dwMask = PFM_ALIGNMENT;    
    paraFormat.wAlignment = PFA_RIGHT;
	
	SetParaFormat(paraFormat);
}
void CGraphicalObjectEditCtrl::OnEnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
    SELCHANGE *pSelChange = reinterpret_cast<SELCHANGE *>(pNMHDR);
    // TODO:  The control will not send this notification unless you override the
    // CRichEditCtrl::OnInitDialog() function to send the EM_SETEVENTMASK message
    // to the control with the ENM_SELCHANGE flag ORed into the lParam mask.

    // TODO:  Add your control notification handler code here

    GetParent()->SendMessage(WM_USER + 135, 0, 0);
    
    *pResult = 0;
}

#include "stdafx.h"
#include "ControlLineStyleSelector.h"

/*
#define msoLineSolid 0
const float arrLineSolid[]  = {(float)1.0, (float)0.0};
#define msoLineSquareDot 1
const float arrLineSquareDot[] = {(float)1.0, (float)1.0};
#define msoLineLongDash 2
const float arrLineLongDash[] = {(float)2.0, (float)2.0};

static CControlLineStyleSelector::CLineStyle lineStyles[] =
{
CControlLineStyleSelector::CLineStyle(msoLineSolid, IDS_LS_SOLID, (float *)arrLineSolid, 2),
CControlLineStyleSelector::CLineStyle(msoLineSquareDot, IDS_LS_SQUAREDOT, (float *)arrLineSquareDot, 2),
CControlLineStyleSelector::CLineStyle(msoLineLongDash, IDS_LS_DASH, (float *)arrLineLongDash, 2),
};
*/

IMPLEMENT_XTP_CONTROL(CControlLineStyleSelector, CXTPControl)


CControlLineStyleSelector::CControlLineStyleSelector()
{
#ifndef _LSUTL32_STATIC
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

    m_nSelected = 0;

    m_LineStyle = Gdiplus::DashStyleSolid;

    m_aAllLineStyles.Add(Gdiplus::DashStyleSolid);
    m_aAllLineStyles.Add(Gdiplus::DashStyleDot);
    m_aAllLineStyles.Add(Gdiplus::DashStyleDash);

    SetFlags(xtpFlagNoMovable);

    /*
    int nCount = sizeof(lineStyles) / sizeof(lineStyles[0]);
    for (int i = 0; i < nCount; i++)
    {
    lineStyles[i].strTip.LoadString(lineStyles[i].toolTipID);
    }
    */
}

CSize CControlLineStyleSelector::GetSize(CDC* /*pDC*/)
{
#ifndef _LSUTL32_STATIC
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

    int nCount = m_aAllLineStyles.GetSize();
    int iWidth = XTP_LS_SELECTOR_WIDTH + 2 * XTP_LS_SELECTOR_MARGIN;
    int iHeight = XTP_LS_SELECTOR_HEIGHT * nCount + 2 * XTP_LS_SELECTOR_MARGIN;
    return CSize (iWidth, iHeight);
}

/*
CString  CControlLineStyleSelector::GetTooltip(LPPOINT pPoint, LPRECT lpRectTip, int* nHit)
{
if (pPoint == NULL || lpRectTip == NULL || nHit == NULL) 
return  _T("");

int nIndex = HitTest(*pPoint);
if (nIndex != -1)
{
*nHit = lineStyles[nIndex].toolTipID;
*lpRectTip = GetRect(nIndex);
return lineStyles[nIndex].strTip;
}
return _T("");
}
*/

CRect CControlLineStyleSelector::GetRect(int nIndex)
{
    int iX = XTP_LS_SELECTOR_MARGIN + m_rcControl.left;
    int iWidth = XTP_LS_SELECTOR_WIDTH - 2 * XTP_LS_SELECTOR_MARGIN;	
    int iY = XTP_LS_SELECTOR_MARGIN + m_rcControl.top + nIndex * XTP_LS_SELECTOR_HEIGHT;
    int iHeight = XTP_LS_SELECTOR_HEIGHT;

    return CRect(iX, iY, iX+iWidth, iY+iHeight);
}

int CControlLineStyleSelector::HitTest(CPoint point)
{
    int nCount = m_aAllLineStyles.GetSize();

    if (!m_rcControl.PtInRect(point))
        return -1;

    for (int i = 0; i < nCount; i++)
    {
        if (GetRect(i).PtInRect(point))
            return i;
    }

    return -1;
}

void CControlLineStyleSelector::Draw(CDC* pDC)
{
#ifndef _LSUTL32_STATIC
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

    CRect rc(m_rcControl);
    int nCount = m_aAllLineStyles.GetSize();

    for (int i = 0; i < nCount; i++)
    {
        CRect rcButton = GetRect(i);

        bool bActiveSelection = m_aAllLineStyles[i] == m_LineStyle;
        bool bMouseOverEntry = i == m_nSelected;
        if (bMouseOverEntry || bActiveSelection)
        {
            BOOL bPressed = GetPressed() && bMouseOverEntry;

            GetPaintManager()->DrawRectangle(
                pDC, &rcButton, bMouseOverEntry, bPressed, m_bEnabled, 
                bActiveSelection, FALSE, 
                GetParent()->GetType(), GetParent()->GetPosition());
        }

        Gdiplus::Graphics gdipGraphics(pDC->m_hDC);
        Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 3);
        pen.SetDashCap(Gdiplus::DashCapFlat);
        //pen.SetDashPattern(lineStyles[i].dashValues, lineStyles[i].iDashValueCount);
        //pen.SetDashStyle(Gdiplus::DashStyleCustom);
        pen.SetDashStyle(m_aAllLineStyles[i]);

        int yPos = rcButton.top + XTP_LS_SELECTOR_HEIGHT / 2;

        if (!m_bEnabled) 
        {
            pen.SetColor(Gdiplus::Color(255, 128, 128, 128));
        }

        gdipGraphics.DrawLine(&pen, Gdiplus::Point(rcButton.left, yPos), Gdiplus::Point(rcButton.right, yPos));

    }
}

void CControlLineStyleSelector::OnMouseMove(CPoint point)
{
#ifndef _LSUTL32_STATIC
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

    int nHit = HitTest(point);
    if (nHit != -1)
    {
        m_nSelected = nHit;
        m_pParent->Invalidate(FALSE);

    }
}

BOOL CControlLineStyleSelector::OnSetSelected(int bSelected)
{
#ifndef _LSUTL32_STATIC
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

    if (!bSelected) m_nSelected = -1;

    return CXTPControl::OnSetSelected(bSelected);
}

int CControlLineStyleSelector::GetObjLineStyle()
{
#ifndef _LSUTL32_STATIC
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

    int iObjLineStyle = 0;
    for (int i=0; i<m_aAllLineStyles.GetSize(); ++i) {
        if (m_aAllLineStyles[i] == m_LineStyle) {
            iObjLineStyle = i;
            break;
        }
    }

    return iObjLineStyle;
}

void CControlLineStyleSelector::SetObjLineStyle(int iLineStyle)
{
#ifndef _LSUTL32_STATIC
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

    m_LineStyle = m_aAllLineStyles[iLineStyle];
}


void CControlLineStyleSelector::OnClick(BOOL bKeyboard, CPoint pt)
{
//bugfix  4782	Assistant mode>Drawing>Line>Change line style - No effect
//#ifndef _LSUTL32_STATIC
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());
//#endif

    if (!m_bEnabled) return;

    int nHit = HitTest(pt);
    if (nHit == -1)
        return;

    m_LineStyle = m_aAllLineStyles[nHit];

    if (!bKeyboard) 
    {			
       CXTPControl::ClickToolBarButton();
    } 
    else
    {
        OnExecute();
    }
}
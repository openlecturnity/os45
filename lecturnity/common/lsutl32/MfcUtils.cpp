#include "StdAfx.h"
#include "MfcUtils.h"
#include "resource.h"
#include "PublishingFormatDialog.h"
#include "lutils.h"

void MfcUtils::Localize(CWnd *pWnd, DWORD *adwIds)
{
   int nIndex = 0;
   DWORD dwId = adwIds[nIndex];
   while (dwId != -1)
   {
      CString csString;
      if (FALSE != csString.LoadString(dwId))
      {
         CWnd *pChild = pWnd->GetDlgItem(dwId);
         if (pChild)
            pWnd->SetDlgItemText(dwId, csString);
#ifdef _DEBUG
         else
         {
            CString csMsg;
            csMsg.Format(_T("MfcUtils::Localize()\n\nGetDlgItem(%d) returned NULL."), dwId);
            ::MessageBox(::GetForegroundWindow(), csMsg, _T("Warning"), MB_OK | MB_ICONWARNING);
         }
#endif
      }
#ifdef _DEBUG
      else
      {
         CString csMsg;
         csMsg.Format(_T("MfcUtils::Localize()\n\nLoadString failed for ID %d."), dwId);
         ::MessageBox(::GetForegroundWindow(), csMsg, _T("Warning"), MB_OK | MB_ICONWARNING);
      }
#endif
      dwId = adwIds[++nIndex];
   }
}

HRESULT MfcUtils::FitRectInRect(CRect& rcAvailable, CRect& rcToFit, bool bScaleLarger)
{
   if (rcAvailable.Width() <= 0 || rcAvailable.Height() <= 0 ||
      rcToFit.Width() <= 0 || rcToFit.Height() <= 0)
      return E_INVALIDARG; // calculation not possible

   double dZoomX = rcAvailable.Width() / (double)rcToFit.Width();
   double dZoomY = rcAvailable.Height() / (double)rcToFit.Height();

   double dZoom = dZoomX > dZoomY ? dZoomY : dZoomX;

   if (dZoom == 1.0)
      return S_FALSE;

   int iNewWidth = rcToFit.Width();
   int iNewHeight = rcToFit.Height();

   if (dZoom < 1.0 || bScaleLarger)
   {
      iNewWidth = (int)(dZoom * iNewWidth);
      iNewHeight = (int)(dZoom * iNewHeight);
   }

   int iLeft = rcAvailable.left + (rcAvailable.Width() - iNewWidth) / 2;
   int iTop = rcAvailable.top + (rcAvailable.Height() - iNewHeight) / 2;

   rcToFit.left = iLeft;
   rcToFit.top = iTop;
   rcToFit.right = iLeft + iNewWidth - 1;
   rcToFit.bottom = iTop + iNewHeight - 1;

   return S_OK;
}

void MfcUtils::GetPrivateBuild(CString& csBuild)
{
    // on error an empty string returned/string left untouched

    _TCHAR tszRevision[20]; ZeroMemory(tszRevision, 20 * sizeof _TCHAR);

    bool bQuerySuccess = LMisc::GetPrivateBuild(tszRevision);
    if (bQuerySuccess)
        csBuild = tszRevision;
}


int MfcUtils::ShowPublisherFormatSelection(HWND parent, CString csLrdname)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CWnd *pWnd = CWnd::FromHandle(parent);
    CPublishingFormatDialog formatDialog(pWnd, csLrdname);
    formatDialog.DoModal();

    int iNewFormat = formatDialog.GetCurrentSelection();

    return iNewFormat;
}

int MfcUtils::GetPublisherFormatSelection()
{
    DWORD dwPublishingFormat;
    bool bResult = LRegistry::ReadDwordRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher"), _T("DirectPublishFormat"), &dwPublishingFormat);

    if (bResult)
        return dwPublishingFormat;
    else
        return PublishingFormat::TYPE_NOTHING;
}

void MfcUtils::GetDesktopRectangle(CRect &rcDesktop) {
    /*
    rcDesktop.left = ::GetSystemMetrics( SM_XVIRTUALSCREEN );
    rcDesktop.top =	::GetSystemMetrics( SM_YVIRTUALSCREEN );
    rcDesktop.right = rcDesktop.left + ::GetSystemMetrics( SM_CXVIRTUALSCREEN );
    rcDesktop.bottom = rcDesktop.top + ::GetSystemMetrics( SM_CYVIRTUALSCREEN );
    */

    HMONITOR hMonitor = ::MonitorFromPoint(CPoint(0, 0), MONITOR_DEFAULTTONEAREST);

    MONITORINFO mi;
    RECT        rc;

    mi.cbSize = sizeof( mi );
    ::GetMonitorInfo( hMonitor, &mi );
    rc = mi.rcMonitor;

    rcDesktop.SetRect(rc.left, rc.top, rc.right, rc.bottom );

}

void MfcUtils::GetCurrentMonitorRectangle(CRect &rcDesktop, CPoint ptMonitor) {

    HMONITOR hMonitor = ::MonitorFromPoint(ptMonitor, MONITOR_DEFAULTTONEAREST);

    MONITORINFO mi;
    RECT        rc;

    mi.cbSize = sizeof( mi );
    ::GetMonitorInfo( hMonitor, &mi );
    rc = mi.rcMonitor;

    rcDesktop.SetRect(rc.left, rc.top, rc.right, rc.bottom );
}
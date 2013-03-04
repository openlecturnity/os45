#include "StdAfx.h"
#include "MfcUtils.h"

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
   // errors are ignored and an empty string returned/string left untouched

   DWORD dwVersionSize = 0;
   DWORD hVersionInfo = 0;

   _TCHAR tszFilename[2 * MAX_PATH];
   ZeroMemory(tszFilename, 2 * MAX_PATH * sizeof _TCHAR);
   _stprintf(tszFilename, _T("..\\AutoUpdate\\lver.dll"));

   // the above path does not always work; so try to find out the absolute installation path
   HKEY hkKey   = NULL;
   LONG lResult = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\imc AG\\LECTURNITY"), 0, KEY_READ, &hkKey);
   if (lResult == ERROR_SUCCESS)
   {
      _TCHAR tszInstallDir[MAX_PATH];
      DWORD dwBufferSize = MAX_PATH * sizeof _TCHAR;
      ZeroMemory(tszInstallDir, dwBufferSize);
      DWORD dwType = 0;
      lResult = ::RegQueryValueEx(hkKey, _T("InstallDir"), NULL, &dwType, (LPBYTE)tszInstallDir, &dwBufferSize);

      if (lResult == ERROR_SUCCESS)
      {
         _stprintf(tszFilename, tszInstallDir);
         if (tszFilename[_tcslen(tszFilename) - 1] != _T('\\'))
            _tcscat(tszFilename, _T("\\"));
         _tcscat(tszFilename, _T("AutoUpdate\\lver.dll"));
      }
   }

   dwVersionSize = ::GetFileVersionInfoSize(tszFilename, &hVersionInfo);
   if (dwVersionSize > 0)
   {
      BYTE *pBuffer = new BYTE[dwVersionSize];

      BOOL bSuccess = ::GetFileVersionInfo(tszFilename, hVersionInfo, dwVersionSize, pBuffer);

      if (bSuccess)
      {
         unsigned short *subBlock;
         unsigned int iLength = 0;

         if (VerQueryValue(pBuffer, _T("\\VarFileInfo\\Translation"), (void **)&subBlock, &iLength))
         {
            _TCHAR tszInfoKey[512];
            _TCHAR *tszInfoValue = NULL;
            iLength = 0;
            
            _stprintf(tszInfoKey, _T("\\StringFileInfo\\%04x%04x\\"), subBlock[0], subBlock[1]);
            _tcscat(tszInfoKey, _T("PrivateBuild"));
            
            if (VerQueryValue(pBuffer, tszInfoKey, (void **)&tszInfoValue, &iLength))
            {
               csBuild = tszInfoValue;
            }
         }
      }

      delete[] pBuffer;
   }

}

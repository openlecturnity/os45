#include "StdAfx.h"
#include "lsupdt32.h"
#include "resource.h"
#include "updateconfigure.h"
#include "autoupdatedialog.h"
#include "ConfigureDialog.h"
#include "KeyGenerator.h" // KerberokHandler

#include "LanguageSupport.h"     // lsutl32

CLsUpdt32Dll g_updateModule;

// ------------------------------

UINT __stdcall LecturnityUpdateCheck()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   CLanguageSupport::SetThreadLanguage();
   return g_updateModule.Update();
}

UINT __stdcall LecturnityUpdateConfigure()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   CLanguageSupport::SetThreadLanguage();
   
   return g_updateModule.Configure();
}

// -----------------------------

CLsUpdt32Dll::CLsUpdt32Dll() : CWinApp()
{
}

BOOL CLsUpdt32Dll::InitInstance()
{
   m_bHasChanged = false;
   ReadSettings();

   // Read version and language
   m_csVersion.LoadString(IDS_VERSION);
   //m_csLanguage.LoadString(IDS_LANG);

   return TRUE;
}

BOOL CLsUpdt32Dll::ExitInstance()
{
   if (HasChanged())
      WriteSettings();
   return TRUE;
}

UINT CLsUpdt32Dll::Update()
{
   CAutoUpdateDialog aud;
   return aud.DoCheck();
}

UINT CLsUpdt32Dll::Configure()
{
   CString csCaption;
   LString lsVersion;
   lsVersion.Allocate(256);
   CString csBuild;
   csBuild.LoadString(IDS_INTERNALBUILD);
   KerberokHandler::GetCompactVersionString(lsVersion, csBuild);
   csCaption.Format(IDS_CAPTION, lsVersion);

   CConfigureDialog configSheet(csCaption);

   CUpdateConfigure configPage;

   configSheet.AddPage(&configPage);

   configSheet.DoModal();

   return configPage.GetReturnValue();
}

void CLsUpdt32Dll::EnableAutoUpdate(bool bEnable)
{
   if (bEnable != m_bAutoUpdate)
   {
      m_bAutoUpdate = bEnable;
      SetChanged();
   }
}

void CLsUpdt32Dll::EnableAnnouncements(bool bEnable)
{
   if (bEnable != m_bAnnouncements)
   {
      m_bAnnouncements = bEnable;
      SetChanged();
   }
}

void CLsUpdt32Dll::SetCheckEvery(DWORD dwCheckEvery)
{
   if (dwCheckEvery != m_dwCheckEvery)
   {
      m_dwCheckEvery = dwCheckEvery;
      SetChanged();
   }
}

void CLsUpdt32Dll::SetLastUpdateTime(DWORD dwLastUpdate)
{
   if (dwLastUpdate != m_dwLastUpdate)
   {
      m_dwLastUpdate = dwLastUpdate;
      SetChanged();
   }
}

void CLsUpdt32Dll::SetLastAnnouncementTime(DWORD dwLastUpdate)
{
   if (dwLastUpdate != m_dwLastAnnouncement)
   {
      m_dwLastAnnouncement = dwLastUpdate;
      SetChanged();
   }
}

void CLsUpdt32Dll::SetLastAnnouncementUrl(CString csLastAnnUrl)
{
   if (csLastAnnUrl != m_csLastAnnUrl)
   {
      m_csLastAnnUrl = csLastAnnUrl;
      SetChanged();
   }
}

void CLsUpdt32Dll::SetChanged()
{
   m_bHasChanged = true;
}

bool CLsUpdt32Dll::HasChanged()
{
   return m_bHasChanged;
}

void CLsUpdt32Dll::ReadSettings()
{
   bool success = LRegistry::ReadSettingsDword(_T("AutoUpdate"), _T("CheckEvery"), 
      &m_dwCheckEvery, 3);
   success = success && LRegistry::ReadSettingsDword(_T("AutoUpdate"), _T("LastAnnouncement"),
      &m_dwLastAnnouncement, 0);
   success = success && LRegistry::ReadSettingsDword(_T("AutoUpdate"), _T("LastUpdate"),
      &m_dwLastUpdate, 0);
   success = success && LRegistry::ReadSettingsBool(_T("AutoUpdate"), _T("AutoUpdate"),
      &m_bAutoUpdate, true);
   success = success && LRegistry::ReadSettingsBool(_T("AutoUpdate"), _T("Announcements"),
      &m_bAnnouncements, true);
   TCHAR szUrl[1024];
   DWORD dwSize = 1024;
   success = success && LRegistry::ReadSettingsString(_T("AutoUpdate"), _T("LastAnnUrl"),
      szUrl, &dwSize, _T(""));

   m_csLastAnnUrl = szUrl;
}

void CLsUpdt32Dll::WriteSettings()
{
   bool success = LRegistry::WriteSettingsDword(_T("AutoUpdate"), _T("CheckEvery"), m_dwCheckEvery);
   success = success && LRegistry::WriteSettingsDword(_T("AutoUpdate"), _T("LastAnnouncement"), m_dwLastAnnouncement);
   success = success && LRegistry::WriteSettingsDword(_T("AutoUpdate"), _T("LastUpdate"), m_dwLastUpdate);
   success = success && LRegistry::WriteSettingsBool(_T("AutoUpdate"), _T("AutoUpdate"), m_bAutoUpdate);
   success = success && LRegistry::WriteSettingsBool(_T("AutoUpdate"), _T("Announcements"), m_bAnnouncements);
   success = success && LRegistry::WriteSettingsString(_T("AutoUpdate"), _T("LastAnnUrl"), m_csLastAnnUrl);

   m_bHasChanged = false;
}

CString CLsUpdt32Dll::GetLanguage()
{
   _TCHAR tszLanguageCode[3];
   KerberokHandler::GetLanguageCode(tszLanguageCode);

   CString csLanguage = _T("en");
   csLanguage = tszLanguageCode;

   return csLanguage;
}


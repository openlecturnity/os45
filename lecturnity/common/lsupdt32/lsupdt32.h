#ifndef __LSUPDT32_H__724B6B9B_29DA_4894_9F9F_37B1D45BDCD6
#define __LSUPDT32_H__724B6B9B_29DA_4894_9F9F_37B1D45BDCD6

class CLsUpdt32Dll : public CWinApp
{
public:
   CLsUpdt32Dll();

   virtual BOOL InitInstance();
   virtual BOOL ExitInstance();

   UINT Update();
   UINT Configure();

   void  EnableAutoUpdate(bool bEnable);
   bool  IsAutoUpdateEnabled() { return m_bAutoUpdate; }

   void  EnableAnnouncements(bool bEnable);
   bool  IsAnnouncementsEnabled() { return m_bAnnouncements; }

   void  SetCheckEvery(DWORD dwCheckEvery);
   DWORD GetCheckEvery() { return m_dwCheckEvery; }

   void  SetLastUpdateTime(DWORD dwLastUpdate);
   DWORD GetLastUpdateTime() { return m_dwLastUpdate; }

   void  SetLastAnnouncementTime(DWORD dwLastUpdate);
   DWORD GetLastAnnouncementTime() { return m_dwLastAnnouncement; }

   void    SetLastAnnouncementUrl(CString csLastAnnUrl);
   CString GetLastAnnouncementUrl() { return m_csLastAnnUrl; }

   CString GetVersion() { return m_csVersion; }
   CString GetLanguage();

private:
   void SetChanged();
   bool HasChanged();

   void ReadSettings();
   void WriteSettings();

   bool m_bHasChanged;

   DWORD   m_dwCheckEvery;
   bool    m_bAutoUpdate;
   bool    m_bAnnouncements;
   DWORD   m_dwLastUpdate;
   DWORD   m_dwLastAnnouncement;
   CString m_csLastAnnUrl;

   // More-or-less constants, read out once at loading the library
   // Version of the program, IDS_VERSION
   CString m_csVersion;

   // Language of this program, IDS_LANG
   // CString m_csLanguage;
   // Disabled with support of dynamic languages (2.0.p3).
};

#endif // __LSUPDT32_H__724B6B9B_29DA_4894_9F9F_37B1D45BDCD6

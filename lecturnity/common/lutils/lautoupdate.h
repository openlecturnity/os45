#ifndef __LUPDATE_H__B3CD6496_7A18_48e1_B544_6B8798753BDC
#define __LUPDATE_H__B3CD6496_7A18_48e1_B544_6B8798753BDC

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

class LAutoUpdate
{
public:
   LAutoUpdate();

   UINT CheckForUpdates();
   UINT Configure();

private:
   bool OpenLibraries();
   bool OpenLibrary(IN _TCHAR *szLibName, OUT HINSTANCE *phInstance);
   void CloseLibraries();

   HINSTANCE m_hGdiPlus;
   HINSTANCE m_hDrawSdk;
   HINSTANCE m_hFileSdk;
   HINSTANCE m_hLsUpdt32;
};

#endif // __LUPDATE_H__B3CD6496_7A18_48e1_B544_6B8798753BDC
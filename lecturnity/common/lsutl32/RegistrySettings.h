#pragma once

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif

class LSUTL32_EXPORT CRegistrySettings
{
public:
    CRegistrySettings(void);
    ~CRegistrySettings(void);

public:
    // Screen grabbing settings
    static void GetLastSelectionRectangle(CRect &rcLastSelection);
    static void SetLastSelectionRectangle(CRect &rcLastSelection);
    static bool GetRecordFullScreen();
    static void SetRecordFullScreen(bool bRecordFullScreen);
};

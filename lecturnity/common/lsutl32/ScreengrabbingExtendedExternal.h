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

class CLepSgml;

class LSUTL32_EXPORT CScreengrabbingExtendedExternal
{
public:
   // add or complete structure and full text search
   // and show structure editor for modified structure
   static HRESULT RunPostProcessing(CString csLrdFilename, bool bShowLaterButton = true, 
                                    bool bIsInLiveContextMode = false, CLepSgml *pLepFile = NULL);
   // remove existing structure and full text search
   static HRESULT RemoveStructure(CString csLrdFilename);
   // show structure editor
   static HRESULT EditStructure(CString csLrdFilename);

private:
   CScreengrabbingExtendedExternal(void) {};
   ~CScreengrabbingExtendedExternal(void) {};
};

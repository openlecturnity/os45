#include "StdAfx.h"

#include "ladfilter.h"
#include "LecturnitySource.h"
#include "RgbToYv12.h"

//
// Common DLL routines and tables
//

// The streams.h DLL entrypoint.
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

// The entrypoint required by the MSVC runtimes. This is used instead
// of DllEntryPoint directly to ensure global C++ classes get initialised.
BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {
   
   return DllEntryPoint(reinterpret_cast<HINSTANCE>(hDllHandle), dwReason, lpreserved);
}


STDAPI DllRegisterServer()
{
   return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
   return AMovieDllRegisterServer2(FALSE);
}




//
// LAD filter registration routines/structs
//

const AMOVIESETUP_MEDIATYPE CLadParser::sudLadInputTypes[] = {
   { &MEDIATYPE_Stream, &GUID_NULL },
};

const AMOVIESETUP_MEDIATYPE CLadParser::sudLadOutputTypes[] = {
   { &MEDIATYPE_Stream, &MEDIASUBTYPE_WAVE },
};

const AMOVIESETUP_PIN CLadParser::sudLadPins[] = { {
   L"Input",
      FALSE,          // Is this pin rendered?
      FALSE,          // Is it an output pin?
      FALSE,          // Can the filter create zero instances?
      FALSE,          // Does the filter create multiple instances?
      &GUID_NULL,     // Obsolete.
      NULL,           // Obsolete.
      1,              // Number of media types.
      CLadParser::sudLadInputTypes   // Pointer to media types.
}, {
   L"Output",
      FALSE,          // Is this pin rendered?
      TRUE,           // Is it an output pin?
      FALSE,          // Can the filter create zero instances?
      FALSE,          // Does the filter create multiple instances?
      &GUID_NULL,     // Obsolete.
      NULL,           // Obsolete.
      1,              // Number of media types.
      CLadParser::sudLadOutputTypes   // Pointer to media types.

} };

// setup data - allows the self-registration to work.
const AMOVIESETUP_FILTER CLadParser::sudLadFilter = {
   &CLSID_LadFilter
      , L"LAD Parser"
      , MERIT_UNLIKELY
      , 2
      , CLadParser::sudLadPins
};  





//
// LecturnitySource filter registration routines/structs
//

const AMOVIESETUP_MEDIATYPE s_defLsPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL      // Minor type
};

const AMOVIESETUP_PIN s_defLsOutputPin =
{
    L"Output",              // Pin string name
    FALSE,                  // Is it rendered
    TRUE,                   // Is it an output
    FALSE,                  // Can we have none
    FALSE,                  // Can we have many
    &CLSID_NULL,            // Connects to filter
    NULL,                   // Connects to pin
    1,                      // Number of types
    &s_defLsPinTypes };       // Pin details

// with this the filter is visible for GraphEdit and for intelligent connect
const AMOVIESETUP_FILTER s_defLsFilter =
{
    &CLSID_LecturnitySource,    // Filter CLSID
    L"Document Video Source", // String name
    MERIT_DO_NOT_USE,           // Filter merit
    1,                          // Number pins
    &s_defLsOutputPin             // Pin details
};



//
// RGB32-YV12 filter registration routines/structs
//

const AMOVIESETUP_MEDIATYPE CRgbToYv12::sudRgbToYv12PinTypes[] = {
    { &MEDIATYPE_Video, &MEDIASUBTYPE_NULL },
};

const AMOVIESETUP_PIN CRgbToYv12::sudRgbToYv12Pins[] = { {
   L"Input",          // strName
      FALSE,          // bRendered
      FALSE,          // bOutput
      FALSE,          // bZero
      FALSE,          // bMany
      &CLSID_NULL,    // clsConnectsToFilter
      L"",            // strConnectsToPin
      1,              // nTypes
      CRgbToYv12::sudRgbToYv12PinTypes // lpTypes
}, {
   L"Output",         // strName
      FALSE,          // bRendered
      TRUE,           // bOutput
      FALSE,          // bZero
      FALSE,          // bMany
      &CLSID_NULL,    // clsConnectsToFilter
      L"",            // strConnectsToPin
      1,              // nTypes
      CRgbToYv12::sudRgbToYv12PinTypes // lpTypes
} };

const AMOVIESETUP_FILTER CRgbToYv12::sudRgbToYv12Filter =
{ 
   &CLSID_RgbToYv12Filter             // clsID
      , L"RGB to YV12"                // strName
      , MERIT_DO_NOT_USE              // dwMerit
      , 2                             // nPins
      , CRgbToYv12::sudRgbToYv12Pins  // lpPin
};



// COM global table of objects in this dll

CFactoryTemplate g_Templates[] = {
   { 
      s_defLsFilter.strName
         , s_defLsFilter.clsID
         , CLecturnitySource::CreateInstance
         , NULL
         , &s_defLsFilter 
   }, 
   { 
      CLadParser::sudLadFilter.strName
         , CLadParser::sudLadFilter.clsID
         , CLadParser::CreateInstance
         , NULL
         , &CLadParser::sudLadFilter 
   },
   {  CRgbToYv12::sudRgbToYv12Filter.strName
         , CRgbToYv12::sudRgbToYv12Filter.clsID
         , CRgbToYv12::CreateInstance
         , NULL
         , &CRgbToYv12::sudRgbToYv12Filter
   }
};

int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

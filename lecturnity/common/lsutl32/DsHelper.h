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

#include <qedit.h>

class LSUTL32_EXPORT CDsHelper {
public:

static HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
static void RemoveFromRot(DWORD pdwRegister);

static HRESULT GetPin(CComPtr<IPin>& pPin, IBaseFilter *pFilter, PIN_DIRECTION PinDir, DWORD dwPinIndex);
static HRESULT GetFirstEmptyPin(CComPtr<IPin>& pPin, IBaseFilter *pFilter, PIN_DIRECTION PinDir);
static HRESULT ConnectFiltersSimple(IGraphBuilder *pGraph, IBaseFilter *pFilter1, IBaseFilter *pFilter2);
static HRESULT InsertTeeAndConnect(IGraphBuilder *pGraph, IPin *pPinLeft, IPin *pPinRight, CComPtr<IPin> &pPinSecond);
static HRESULT ConnectEmptyAviMux(IGraphBuilder *pGraph, IPin *pPinToDiscard);
static HRESULT RemoveRenderersSaveConnections(IGraphBuilder *pGraph, IPin **ppAudioPin, IPin **ppVideoPin);
static HRESULT CreateLameMp3Filter(CComPtr<IBaseFilter>& pMp3Encoder, int iAudioKBit, DWORD nSamplesPerSec=0);
static HRESULT WaitForFinishSimple(CComPtr<IGraphBuilder>& pGraph);


public:
    CDsHelper(void);
    ~CDsHelper(void);
};

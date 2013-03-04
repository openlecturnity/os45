#if !defined(AFX_LRDCONVERTER_H__1AB08F6D_18E4_47CB_A1B1_EA6B95D73B5F__INCLUDED_)
#define AFX_LRDCONVERTER_H__1AB08F6D_18E4_47CB_A1B1_EA6B95D73B5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define E_VID_FILE_NOT_FOUND  _HRESULT_TYPEDEF_(0x80c80001L)
#define E_VID_FILTERS_MISSING _HRESULT_TYPEDEF_(0x80c80002L)


#include <gdiplus.h>

#include "LrdFile.h"
#include "AVStreams.h"
#include "..\..\common\ladfilter\LecturnitySourceDefines.h"



class CLrd2VideoConverter  
{
public:
	CLrd2VideoConverter();
	virtual ~CLrd2VideoConverter();

   HRESULT SetInputFile(LPCTSTR tszLrdFile);
   HRESULT PrepareConversion(LPCTSTR tszOutputFile, bool bIsIgnored,
      float fFrameRate, UINT nVideoQFactor, UINT nAudioRateKbit,
      UINT nOutputWidth, UINT nOutputHeight, bool bEmbedClips, 
      bool bVideoInsteadOfPages, bool bTargetIPod);
   HRESULT StartConversion();
   HRESULT AbortConversion();
   int GetProgress() { return m_iCurrentProgress; }

   HRESULT CheckForFilters();

private:
   static DWORD WINAPI ConversionLauncher(void *pParameter);
   HRESULT ConversionThread();

   HRESULT CreateVideoCompressor(
       CComPtr<IBaseFilter> &pVideoCompressor, int nVideoQFactor,  float fFrameRate, 
       bool bTargetIPod, int iOutputWidth, int iOutputHeight);
   HRESULT CreateAudioCompressor(CComPtr<IBaseFilter> &pAudioCompressor, UINT nAudioKBit);
   HRESULT FindFilter(bool bAudio, LPCTSTR tszCodecName, IBaseFilter **ppTargetFilter);


   CString m_csLrdFilename;
   CLrdFile *m_pLrdFile;
   CComPtr<IGraphBuilder> m_pGraph;
   double m_dAudioSeconds;
   bool m_bConversionAborted;
   int m_iCurrentProgress;
   bool m_bConversionActive;
   CComPtr<ILecturnitySourceControl> m_pLecturnityControl;
};

#endif // !defined(AFX_LRDCONVERTER_H__1AB08F6D_18E4_47CB_A1B1_EA6B95D73B5F__INCLUDED_)

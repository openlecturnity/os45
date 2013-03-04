#if !defined(AFX_LECTURNITYSOURCE_H__0361AED3_20A4_4406_B0B5_2BFD76AFB511__INCLUDED_)
#define AFX_LECTURNITYSOURCE_H__0361AED3_20A4_4406_B0B5_2BFD76AFB511__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "LecturnitySourceDefines.h"
#include "WhiteboardStream.h"


class CClipInfo
{
public:
   CClipInfo();
   virtual ~CClipInfo();

   HRESULT Init(LPCTSTR tszClipFilename, int iStartMs);
   double GetFrameRate() { return m_dFrameRate; }
   bool ContainsMediaTime(int iMediaMs);
   LONG GetFrameNumber(UINT nMediaMs);
   int GetEndTimeMs() { return m_iStartMs + m_nLengthMs; }
   //int GetStartTimeMs() { return m_iStartMs; }

   PGETFRAME Open();
   void Close();

private:
   _TCHAR *m_tszClipFilename;
   double m_dFrameRate;
   int m_iStartMs;
   UINT m_nLengthMs;
   PGETFRAME m_pAviGetFrame;
};

class CLecturnitySource : CSource, public ILecturnitySourceControl
{
public:

   // reveals IWhiteboardSourceControl
   STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

   DECLARE_IUNKNOWN; // seems to be necessary to actually really inherit
                     // methods from CSourceStream, otherwise extending ILecturnitySourceControl
                     // which extends IUnknown gives errors

   static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

   // outside configuration
   STDMETHODIMP SetVideoSizes(UINT nOutputWidth, UINT nOutputHeight, UINT nInputWidth, UINT nInputHeight);
   STDMETHODIMP SetFrameRate(float fFrameRate);
   STDMETHODIMP SetIsEvaluation(bool bIsEvaluation);
   STDMETHODIMP SetMinimumLength(UINT nLengthMs);
   STDMETHODIMP ParseFiles(LPCTSTR tszEvqFilename, LPCTSTR tszObjFilename);
   STDMETHODIMP AddClip(LPCTSTR tszClipName, int iClipStartMs);
      
   STDMETHODIMP GetLastSampleTime(UINT *pnSampleTimeMs);



private:
   CLecturnitySource(LPUNKNOWN lpunk, HRESULT *phr);
	virtual ~CLecturnitySource();
};


class CLecturnityVideoStream : public CSourceStream
{
public:
	CLecturnityVideoStream(HRESULT *phr, CSource *pParent, LPCWSTR pPinName);
	virtual ~CLecturnityVideoStream();

   virtual HRESULT GetMediaType(CMediaType *pMediaType);
   virtual HRESULT CheckMediaType(const CMediaType *pMediaType);

   virtual HRESULT FillBuffer(IMediaSample *pSample);
   virtual HRESULT DecideBufferSize(IMemAllocator *pMemAllocator, ALLOCATOR_PROPERTIES *pAllocProperties);

   STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

   // outside configuration
   HRESULT SetVideoSizes(UINT nOutputWidth, UINT nOutputHeight, UINT nInputWidth, UINT nInputHeight);
   HRESULT SetFrameRate(float fFrameRate);
   HRESULT SetIsEvaluation(bool bIsEvaluation);
   HRESULT SetMinimumLength(UINT nLengthMs);
   HRESULT ParseFiles(LPCTSTR tszEvqFilename, LPCTSTR tszObjFilename);
   HRESULT AddClip(LPCTSTR tszClipName, int iClipStartMs);

   HRESULT GetLastSampleTime(UINT *pnSampleTimeMs);

private:
   Gdiplus::Bitmap* RetrieveClipFrame(UINT nMediaMs);
   HRESULT GetAviClipFrame(UINT nMediaMs, bool bGetSomeImage, Gdiplus::Bitmap **ppTargetImage);

   HRESULT CalculateDrawingRegion(UINT nOutputWidth, UINT nOutputHeight, 
      UINT nInputWidth, UINT nInputHeight, CRect *prcTarget, double *pdZoomFactor);

   int m_iRepeatTime;                  // Time in msec between frames
   CRefTime m_rtSampleTime;            // The time stamp for each sample
   double m_dZoomFactor;
   UINT m_nOutputWidth;
   UINT m_nOutputHeight;
   CRect m_rcWbArea;
   CWhiteboardStream *m_pWhiteboard;
   bool m_bWhiteboardOpened;
   Gdiplus::Bitmap *m_pBitmap;
   bool m_bIsEvaluation;
   UINT m_nMinimumLengthMs;

   Gdiplus::GdiplusStartupInput m_gdipStartupInput;
   ULONG_PTR m_gdipToken;

   CArray<CClipInfo *, CClipInfo *> m_aAllClips;
   CClipInfo *m_pLastCurrentClip;
   CComPtr<IMediaDet> m_pClipDet;
   bool m_bClipWasActive;
};

#endif // !defined(AFX_LECTURNITYSOURCE_H__0361AED3_20A4_4406_B0B5_2BFD76AFB511__INCLUDED_)

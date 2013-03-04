//------------------------------------------------------------------------------
// File: RgbToYv12.h
//
// Desc: DirectShow filter: RGB to YV12 conversion.
//------------------------------------------------------------------------------

// The CLSID used by the RGB to YV12 filter
// {94B33A11-866F-468d-A360-D9A313E2BF09}

DEFINE_GUID(CLSID_RgbToYv12Filter,
0x94B33A11, 0x866F, 0x468d, 0xA3, 0x60, 0xD9, 0xA3, 0x13, 0xE2, 0xBF, 0x09);

// CRgbToYv12
//
class CRgbToYv12
    : public CTransformFilter
{

public:

    static CUnknown *WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

    DECLARE_IUNKNOWN;

    static const AMOVIESETUP_FILTER sudRgbToYv12Filter;
    static const AMOVIESETUP_MEDIATYPE sudRgbToYv12PinTypes[];
    static const AMOVIESETUP_PIN    sudRgbToYv12Pins[];

    // Implementation of virtual functions from CTransformFilter
    HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
    HRESULT CheckTransform(const CMediaType *mtIn,const CMediaType *mtOut);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc,
                             ALLOCATOR_PROPERTIES *pProperties);
    // We accept RGB24, RGB32 and ARGB32. We'd return E_FAIL for any we didn't like.
    HRESULT CheckInputType(const CMediaType* mtIn);


private:

    // Constructor - just calls the base class constructor
    CRgbToYv12(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
    // Destructor
    ~CRgbToYv12();

    // Convert RGB to YV12
    HRESULT ConvertRgbToYv12(IMediaSample *pIn, IMediaSample *pOut);
    // Alternative conversion way (using code from huffyuv)
    HRESULT ConvertRgb32ToHuffYv12(IMediaSample *pIn, IMediaSample *pOut);

    // From huffyuv v2.1.1 (http://neuron2.net/www.math.berkeley.edu/benrg/huffyuv.html)
    unsigned char clip[896];
    void InitClip();
    unsigned char Clip(int x);

private: // Members

    // Video Subtype (RGB24, RGB32 or ARGB32)
    GUID m_VideoSubtype;

    // Video dimensions
    int m_nVideoWidth;
    int m_nVideoHeight;

}; // CRgbToYv12

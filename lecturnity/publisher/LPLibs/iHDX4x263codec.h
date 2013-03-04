#ifndef	iHDX4x263CodecH
#define	iHDX4x263CodecH

//------------------------------------------------------------------------------
// {03112886-43E1-4c80-9F46-6AD10AAAC380}
DEFINE_GUID(CLSID_HDX4x263Encoder, 
			0x03112886, 0x43e1, 0x4c80, 0x9f, 0x46, 0x6a, 0xd1, 0xa, 0xaa, 0xc3, 0x80);

// {03112886-43E1-4c80-9F46-6AD10AAAC381}
DEFINE_GUID(IID_IHDX4x263EncoderConfig, 
			0x03112886, 0x43e1, 0x4c80, 0x9f, 0x46, 0x6a, 0xd1, 0xa, 0xaa, 0xc3, 0x81);

// {03112886-43E1-4c80-9F46-6AD10AAAC382}
DEFINE_GUID(CLSID_HDX4x263Decoder, 
			0x03112886, 0x43e1, 0x4c80, 0x9f, 0x46, 0x6a, 0xd1, 0xa, 0xaa, 0xc3, 0x82);

// H.263/Flash compressed video stream types.
DEFINE_GUID(MEDIASUBTYPE_MP4V, 0x5634504D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);

const DWORD	AVC_FOURCC_h263	= MAKEFOURCC('h','2','6','3'); // 0x33363268
const DWORD	AVC_FOURCC_H263	= MAKEFOURCC('H','2','6','3'); // 0x33363248

DEFINE_GUID(MEDIASUBTYPE_h263, AVC_FOURCC_h263, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);
DEFINE_GUID(MEDIASUBTYPE_H263, AVC_FOURCC_H263, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);

#ifndef FLASH_VIDEO_NAMES
#define FLASH_VIDEO_NAMES
	const DWORD	AVC_FOURCC_flv1	= MAKEFOURCC('f','l','v','1'); // 0x31766C66
	const DWORD	AVC_FOURCC_FLV1	= MAKEFOURCC('F','L','V','1'); // 0x31564C46
	DEFINE_GUID(MEDIASUBTYPE_flv1, AVC_FOURCC_flv1, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);
	DEFINE_GUID(MEDIASUBTYPE_FLV1, AVC_FOURCC_FLV1, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);

	//-- 
	const DWORD	AVC_FOURCC_VP60	= MAKEFOURCC('V','P','6','0'); 
	const DWORD	AVC_FOURCC_vp60	= MAKEFOURCC('v','p','6','0'); 
	DEFINE_GUID(MEDIASUBTYPE_VP60, AVC_FOURCC_VP60, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);
	DEFINE_GUID(MEDIASUBTYPE_vp60, AVC_FOURCC_vp60, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);

	const DWORD	AVC_FOURCC_VP61	= MAKEFOURCC('V','P','6','1'); 
	const DWORD	AVC_FOURCC_vp61	= MAKEFOURCC('v','p','6','1'); 
	DEFINE_GUID(MEDIASUBTYPE_VP61, AVC_FOURCC_VP61, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);
	DEFINE_GUID(MEDIASUBTYPE_vp61, AVC_FOURCC_vp61, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);

	const DWORD	AVC_FOURCC_VP62	= MAKEFOURCC('V','P','6','2'); 
	const DWORD	AVC_FOURCC_vp62	= MAKEFOURCC('v','p','6','2'); 
	DEFINE_GUID(MEDIASUBTYPE_VP62, AVC_FOURCC_VP62, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);
	DEFINE_GUID(MEDIASUBTYPE_vp62, AVC_FOURCC_vp62, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b,0x71);
#endif


#ifdef __cplusplus
extern "C" 
{
#endif
	DECLARE_INTERFACE_(IHDX4x263EncoderConfig, IUnknown) 
	{
		STDMETHOD(SetXMLSettings) (THIS_ const BSTR bstrXMLSettings) PURE;
		STDMETHOD(GetXMLSettings) (THIS_ BSTR* pbstrXMLConfigString, THIS_ const BSTR bstrDocumentType) PURE;
	};

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
#endif /* iHDX4x263CodecH */
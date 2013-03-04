///////////////////////////////////////////////////////////////////////////////
//
//	file name:	common_mc.h
//
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005-2009 MainConcept GmbH
//  All rights are reserved. Reproduction in whole or in part is prohibited
//  without the written consent of the copyright owner.
//
//  MainConcept GmbH reserves the right to make changes to this software program
//  without notice at any time. MainConcept GmbH makes no warranty, expressed,
//  implied or statutory, including but not limited to any implied warranty of
//  merchantability of fitness for any particular purpose.
//  MainConcept GmbH does not represent or warrant that the programs furnished
//  hereunder are free of infringement of any third-party patents, copyright or
//  trademark.
//  In no event shall MainConcept GmbH be liable for any incidental, punitive, or
//  consequential damages of any kind whatsoever arising from the use of these
//  programs.
//
///////////////////////////////////////////////////////////////////////////////
//
//  Purpose:   IModuleConfig common parameter's identifiers
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _COMMON_MC_
#define _COMMON_MC_

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//    GUID                        Value Type        Available range        Default Value    Note
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Unified for all components
//
// MC_OPERATION_MODE            VT_INT            [OperationMode_t]    READONLY        Operation mode 
// MC_OEM_NAME                  VT_BSTR           Readonly             undefined       OEM customer Name
// MC_OUT_SAMPLE_SIZE           VT_INT            type limits          0               Output sample size: 0 - size should be calculated automatically
// MC_PRESET                    VT_INT            type limits          undefined       A number of grouped settings
// MC_AFFINITY_MASK             VT_UI4            type limits          undefined       A bit vector in which each bit represents the processors that a process or thread (depending on component implementation) is allowed to run on
// MC_AUTOPERSIST               VT_UI4            [0, 1]               1               Enables/disables automatic updating of registry settings
//
// Encoder's parameters  (video, audio)
//
// MC_BITRATE_AVG                VT_INT           type limits          undefined       Average Bit rate
// MC_BITRATE_MAX                VT_INT           type limits          undefined       Maximal Bit rate
// MC_BITRATE_MIN                VT_INT           type limits          undefined       Minimal Bit rate
// MC_BITRATE_MODE               VT_INT           type limits          undefined       Bit Rate Control Mode
// MC_BITRATE_PASS               VT_INT           type limits          undefined       Multipass encoding mode
// MC_BITRATE_STAT               VT_BSTR          type limits          undefined       File path for storage rate control statistics
// MC_PROFILE                    VT_INT           type limits          undefined       Profile
// MC_LEVEL                      VT_INT           type limits          undefined       Level
// MC_GOP_LENGTH                 VT_INT           0 ... type limits    undefined       Distance between key frames (I-pictures)
// MC_GOP_BCOUNT                 VT_INT           0 ... type limits    undefined       Number of B-pictures between reference frames
// MC_ASPECT_RATIO               VT_INT           type limits          undefined       Picture aspect ratio code
//
// Audio Encoder's parameters 
//
// MC_JOINT_CODING               VT_INT           type limits          undefined       Joint coding mode for stereo audio streams (mid-side, intensity or dual channel)
// MC_BANDWIDTH                  VT_INT           type limits          undefined       Bandwidth of the low-pass filter applied before encoding
// MC_CRC_PROTECTION             VT_INT           type limits          undefined       Enable CRC protection or not
//
//    Statistics (read-only)
//
// MC_ENCODED_FRAMES             VT_INT           type limits          undefined       Encoded and delivered frames (read-only)
// MC_FPS                        VT_R8            type limits          undefined       Frames per second. Should return actual speed per second (system time) (read-only)
// MC_CALC_PSNR                  VT_INT           [0, 1]               [0]             Calculate overall PSNR or not
// MC_OVERALL_PSNR               VT_R8            [0, limit of DOUBLE] READONLY        Overall PSNR (luma and chroma)
// MC_BITRATE_REAL               VT_R8            [0, limit of DOUBLE] READONLY        Real average bitrate
//
// Video Decoder's parameters
//
// MC_HardwareAcceleration       VT_UI4           [0, 1]               [0]             Switching between software and hardware decode modes
// MC_HardwareAccelerationState  VT_UI4           [0, 2]               [0]             Get hardware acceleration state (whether it is used or not currently)
// MC_PostProcess                VT_UI4           [0, 1]               [0]             Enable/disable deblocking and dering filtering (only software mode)
// MC_Brightness                 VT_UI4           [0, 255]             [128]           Brightness level
// MC_Contrast                   VT_UI4           [0, 255]             [128]           Contrast
// MC_Deinterlace                VT_UI4           [0, 3]               [0]             Selecting output deinterlacing mode
// MC_Quality                    VT_UI4           [0, 3]               [0]             Selecting skip mode(I,IP,IBP) or obey quality messages
// MC_OSD                        VT_UI4           [0, 1]               [0]             Enable/disable on screen display info (only software mode)
// MC_CapturePicture             VT_BYREF         []                   [0]             Get last decoded picture. Call GetValue(...) and send empty VARIANT argument. Reinterpreted pbVal to EM2VD_CapturePictureInfo
//                                                                                     struct pointer. Free allocated memory throw CoTaskMemFree on the end.
// MC_FramesDecoded              VT_UI4           []                   [0]             Count of decoded frames (read only)
// MC_FramesSkipped              VT_UI4           []                   [0]             Count of skipped frames (read only)
// MC_MultiThread                VT_UI4           [0, 1]               [1]             Use multi-thread if available (available on multi-processors systems, only software decode mode, only MPEG-2 streams)
// MC_NumThreads				 VT_UI4           [,codec limit]       [0]             Number of threads to be used by codec
// MC_MediaTimeStart             VT_I8            []                                   Media time of current frame (it may be a byte offset of current frame)
// MC_MediaTimeStop              VT_I8            []    
// MC_ResetStatistics            VT_UI4           [0, 1]               [0]             Reset decoder statistics (decoded and skipped frames counter).
// MC_ErrorConcealment           VT_UI4           [0, 1]               [1]             Don't show frames with errors
// MC_VMRMaintainAspectRatio     VT_UI4           [0, 1]               [1]             Enable "maintain aspect ratio" for VMR-7,9 while connection is complete
// MC_StreamOrder                VT_UI4           [0, 1]               [0]             Output frames in streams order
// MC_OutputQueue                VT_UI4           type limits          [0]             Output queue size. 0 - disable output queue.
//
// VES analysis
//
// MC_VESA_SETTINGS              VT_BYREF         []                   [0]             Set/get VESA settings (VESA_settings_t, mcvesa.h)
// MC_VESA_BUFFERS               VT_BYREF         []                   [1]             Get VESA buffers (VESA_buffers_t, mcvesa.h)

// MC_OutputYV12				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_YV12 on output pin
// MC_OutputYUY2				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_YUY2 on output pin
// MC_OutputUYVY				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_UYVY on output pin
// MC_OutputAYUV				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_AYUV on output pin
// MC_OutputRGB32				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_RGB32 on output pin
// MC_OutputRGB24				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_RGB24 on output pin
// MC_OutputRGB565				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_RGB24(RGB565) on output pin
// MC_OutputRGB555				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_RGB24(RGB555) on output pin

namespace MC_COMMON
{
    // Available values for MC_OPERATION_MODE parameter
    typedef enum OperationMode
    {
        NOTACTIVE_MODE          = 0,    // 
        DEMO_MODE               = 1,    // 
        EVALUATION_MODE         = 2,    // 
        EVALUATION_EXPIRED_MODE = 3,    // 
        FULL_MODE               = 4     // 
    } OperationMode_t;

	// 
	typedef enum _tStreamType 
	{
		STREAM_TYPE_UNKNOWN		= 0,
		STREAM_TYPE_SYSTEM		= 1,	// MPEG1 System 
		STREAM_TYPE_PROGRAM		= 2,	// MPEG2 Programm
		STREAM_TYPE_TRANSPORT	= 3,	// MPEG2 Transport
		STREAM_TYPE_MPEG2V		= 4,	// MPEG2 Video
		STREAM_TYPE_PVA			= 5,	// 
		STREAM_TYPE_MPEG2A		= 6,	// MPEG2 Audio
		STREAM_TYPE_AC3			= 7,	// AC3 Audio
		STREAM_TYPE_H264		= 8,	// H264/AVC Video
		STREAM_TYPE_MPEG4		= 9,	// MPEG4
		STREAM_TYPE_VC1			= 10,	// VC 1
		STREAM_TYPE_H263		= 11,	// H263
		STREAM_TYPE_MPEG4V		= 12,	// MPEG4 Video
		STREAM_TYPE_AAC			= 13,	// AAC Audio
		STREAM_TYPE_MPEG1V		= 14,	// MPEG1 Video
		STREAM_TYPE_MPEG1A		= 15,	// MPEG1 Audio
		STREAM_TYPE_DIV3		= 16,	// DivX 3
		STREAM_TYPE_DIV4		= 17,	// DivX 4
		STREAM_TYPE_DIV5		= 18,	// DivX 5
		STREAM_TYPE_VSSH		= 19,	// VideoSoft H264 (under development - no parcer)
		STREAM_TYPE_PCM			= 20,	// LPCM (under development)
		STREAM_TYPE_DVDSUB		= 21,	// DVD Sub-Picture
		STREAM_TYPE_AVS			= 22	//AVS video
	} STREAM_TYPE;
}

//
//    Unified for all components
//

//    Operation mode (read-only).
//    Should return component's operation mode (returned by License Control or tuned by application).
// {AFE11383-84D7-4f83-B114-ADC382F97D71}
DEFINE_GUID(MC_OPERATION_MODE,
0xAFE11383, 0x84D7, 0x4F83, 0xB1, 0x14, 0xAD, 0xC3, 0x82, 0xF9, 0x7D, 0x71);

//    OEM Name (read-only).
// {F9041053-1903-4497-82C4-519F0DA71196}
DEFINE_GUID(MC_OEM_NAME,
0xF9041053, 0x1903, 0x4497, 0x82, 0xC4, 0x51, 0x9F, 0xD, 0xA7, 0x11, 0x96);

//    Output sample size.
// {32DAF502-17D2-4cfd-ABD9-CF5C0F447D0B}
DEFINE_GUID(MC_OUT_SAMPLE_SIZE,
0x32DAF502, 0x17D2, 0x4CFD, 0xAB, 0xD9, 0xCF, 0x5C, 0xF, 0x44, 0x7D, 0xB);

//    Preset. A number of grouped settings. 
// {0D6239BD-E7DC-4ddb-AB8D-C21E8AFDA71C}
DEFINE_GUID(MC_PRESET,
0xD6239BD, 0xE7DC, 0x4DDB, 0xAB, 0x8D, 0xC2, 0x1E, 0x8A, 0xFD, 0xA7, 0x1C);

// Affinity mask. A bit vector in which each bit represents the processors that
// a process or thread (depending on component implementation) is allowed to run on
// {D030CB55-C27C-4eaa-93CC-1DD13FE2A515}
DEFINE_GUID(MC_AFFINITY_MASK,
0xD030CB55, 0xC27C, 0x4EAA, 0x93, 0xCC, 0x1D, 0xD1, 0x3F, 0xE2, 0xA5, 0x1);

// Auto persist management. Enables/disables automatic updating of registry settings
// {5B6B1EA9-7867-4f67-AD77-46F51C214E1D}
DEFINE_GUID(MC_AUTOPERSIST,
0x5B6B1EA9, 0x7867, 0x4F67, 0xAD, 0x77, 0x46, 0xF5, 0x1C, 0x21, 0x4E, 0x1D);


//
//    Uniform Encoder's parameters 
//
//    Average Bit rate 
// {BDD50372-38E2-4a26-BA15-482B70EE136C}
DEFINE_GUID(MC_BITRATE_AVG,
0xBDD50372, 0x38E2, 0x4A26, 0xBA, 0x15, 0x48, 0x2B, 0x70, 0xEE, 0x13, 0x6C);

//    Maximal Bit rate 
// {51EB9A74-B317-4fbb-832B-CA5C2DAA193F}
DEFINE_GUID(MC_BITRATE_MAX,
0x51EB9A74, 0xB317, 0x4FBB, 0x83, 0x2B, 0xCA, 0x5C, 0x2D, 0xAA, 0x19, 0x3F);

//    Minimal Bit rate
// {38A26DF5-D04E-4931-BBC2-19097F812069}
DEFINE_GUID(MC_BITRATE_MIN,
0x38A26DF5, 0xD04E, 0x4931, 0xBB, 0xC2, 0x19, 0x9, 0x7F, 0x81, 0x20, 0x69);

//    Bit Rate Control Mode
// {CD701824-5F65-4520-8DB0-26548212808C}
DEFINE_GUID(MC_BITRATE_MODE,
0xCD701824, 0x5F65, 0x4520, 0x8D, 0xB0, 0x26, 0x54, 0x82, 0x12, 0x80, 0x8C);

// Multipass encoding mode
// {9078A811-26BC-4ba0-8AE7-6FC1CCF7D6D1}
DEFINE_GUID(MC_BITRATE_PASS,
0x9078A811, 0x26BC, 0x4BA0, 0x8A, 0xE7, 0x6F, 0xC1, 0xCC, 0xF7, 0xD6, 0xD1);

// Multipass statistics file name
// {05E339D3-C15D-4928-A5B7-E5D165ACD7F1}
DEFINE_GUID(MC_BITRATE_STAT,
0x5E339D3, 0xC15D, 0x4928, 0xA5, 0xB7, 0xE5, 0xD1, 0x65, 0xAC, 0xD7, 0xF1);


//    Profile
// {FDD66DC1-DB28-4df9-ADE8-1FAD172FF045}
DEFINE_GUID(MC_PROFILE,
0xFDD66DC1, 0xDB28, 0x4DF9, 0xAD, 0xE8, 0x1F, 0xAD, 0x17, 0x2F, 0xF0, 0x45);

//    Level
// {68AB7A6D-A427-467b-B39D-6651E50C22C4}
DEFINE_GUID(MC_LEVEL,
0x68AB7A6D, 0xA427, 0x467B, 0xB3, 0x9D, 0x66, 0x51, 0xE5, 0xC, 0x22, 0xC4);


//
//    Video GOP structure
//
//    Distance between key frames (I-pictures).
// {72EE0B12-AF81-41c4-AAE9-BFB23725F554}
DEFINE_GUID(MC_GOP_LENGTH,
0x72EE0B12, 0xAF81, 0x41C4, 0xAA, 0xE9, 0xBF, 0xB2, 0x37, 0x25, 0xF5, 0x54);


//    Number of B-pictures between reference frames.
// {3ADA1696-B94D-4d7e-B7BF-253D1103E835}
DEFINE_GUID(MC_GOP_BCOUNT,
0x3ADA1696, 0xB94D, 0x4D7E, 0xB7, 0xBF, 0x25, 0x3D, 0x11, 0x3, 0xE8, 0x35);

//    Picture aspect ratio
// {57B6D4ED-52D6-4099-9528-C10995465C00}
DEFINE_GUID(MC_ASPECT_RATIO,
0x57B6D4ED, 0x52D6, 0x4099, 0x95, 0x28, 0xC1, 0x9, 0x95, 0x46, 0x5C, 0x0);


//
//    Audio Encoder's parameters 
//
// Joint coding mode for stereo audio streams (mid-side, intensity or dual channel);
// {706A844B-462C-49b2-991C-131988F3DD64}
DEFINE_GUID(MC_JOINT_CODING,
0x706A844B, 0x462C, 0x49B2, 0x99, 0x1C, 0x13, 0x19, 0x88, 0xF3, 0xDD, 0x64);

// {B2A86C3A-6D1B-4384-B8B5-735497EFAF9E}
DEFINE_GUID(MC_BANDWIDTH,
0xB2A86C3A, 0x6D1B, 0x4384, 0xB8, 0xB5, 0x73, 0x54, 0x97, 0xEF, 0xAF, 0x9E);

// Enable CRC protection or not
// {597881AE-E89C-4ac8-A15A-BA3DB2BDC15A}
DEFINE_GUID(MC_CRC_PROTECTION,
0x597881AE, 0xE89C, 0x4AC8, 0xA1, 0x5A, 0xBA, 0x3D, 0xB2, 0xBD, 0xC1, 0x5A);


//
//    Statistics 
//

// FLAGS

// Calculate overall PSNR or not
// {5CCB6D1B-BFCC-4f88-92DC-ADEAADF78B34}
DEFINE_GUID(MC_CALC_PSNR,
0x5CCB6D1B, 0xBFCC, 0x4F88, 0x92, 0xDC, 0xAD, 0xEA, 0xAD, 0xF7, 0x8B, 0x34);


// READONLY

//    Encoded and delivered frames (read-only)
// {16A9C0E0-9565-4ac8-91C7-82F5B59A0391}
DEFINE_GUID(MC_ENCODED_FRAMES,
0x16A9C0E0, 0x9565, 0x4AC8, 0x91, 0xC7, 0x82, 0xF5, 0xB5, 0x9A, 0x3, 0x91);


//    Frames per second. Should return actual speed per second (system time) (read-only)
// {A1885A39-275D-45ed-8AC1-DB37FA64D7CD}
DEFINE_GUID(MC_FPS,
0xA1885A39, 0x275D, 0x45ED, 0x8A, 0xC1, 0xDB, 0x37, 0xFA, 0x64, 0xD7, 0xCD);


// Overall PSNR (luma and chroma)
// {F0DCB6C2-F08B-4c87-97BF-E62E09C64C5D}
DEFINE_GUID(MC_OVERALL_PSNR,
0xF0DCB6C2, 0xF08B, 0x4C87, 0x97, 0xBF, 0xE6, 0x2E, 0x9, 0xC6, 0x4C, 0x5D);

// Real average bitrate
// {8AC75EE6-7F3A-4d51-BE5C-BCC510EC3EB5}
DEFINE_GUID(MC_BITRATE_REAL,
0x8AC75EE6, 0x7F3A, 0x4D51, 0xBE, 0x5C, 0xBC, 0xC5, 0x10, 0xEC, 0x3E, 0xB5);


// Video decoder's params

namespace MC_COMMON
{

    enum    State{
        State_Off = 0,
        State_On =1
    };

	enum DeinterlaceMode{
		Deinterlace_Weave				= 0,		// No deinterlace
		Deinterlace_VerticalFilter		= 1,		// Interpolation with using info from both fields
		Deinterlace_FieldInterpolation	= 2,		// Stretch one of fields
		Deinterlace_VMR					= 3,
		Deinterlace_Auto				= 4
	};

    enum QualityMode{
        Quality_ObeyQualityMessages        = 0,
        Quality_I                        = 1,
        Quality_IP                        = 2,
        Quality_IPB                        = 3
    };

    enum ErrorConcealmentMode{
        ErrorConcealment_Off            = 0,
        ErrorConcealment_NotShowFramesWithErrors = 1,
        ErrorConcealment_Temporal        = 2,
        ErrorConcealment_Spatial        = 3,
        ErrorConcealment_Smart            = 4
    };

    typedef struct _CapturePictureInfo{
        VOID * pBuffer;
        SIZE_T szBuffer;
    }CapturePictureInfo;

	enum AspectRatio {
		DefaultAR = 0,
		AR_1x1 = 1,
		AR_4x3 = 2,
		AR_16x9 = 3
	};

	enum SMPMode {
		SMPMode_Off = 0,
		SMPMode_Auto = 1,
		SMPMode_ByPictures = 2,
		SMPMode_BySlices = 3
	};

	enum HWACCState {
		HWACCState_Off = 0,
		HWACCState_DXVA1 = 1,
		HWACCState_DXVA2 = 2
	};
}

// {BA6DDF74-5F8A-4bdc-88BB-A2563314BC3E}
// Switching between software and hardware decode modes
DEFINE_GUID(MC_HardwareAcceleration,
0xBA6DDF74, 0x5F8A, 0x4BDC, 0x88, 0xBB, 0xA2, 0x56, 0x33, 0x14, 0xBC, 0x3E);

// {D035CBA4-15C2-490f-B694-C15D77A4B788}
DEFINE_GUID(MC_HardwareAccelerationState,
0xd035cba4, 0x15c2, 0x490f, 0xb6, 0x94, 0xc1, 0x5d, 0x77, 0xa4, 0xb7, 0x88);

// {9CF1A334-E72B-4a6d-BBE8-199595944546}
// Enable/disable deblocking and dering filtering (only software mode)
DEFINE_GUID(MC_PostProcess,
0x9CF1A334, 0xE72B, 0x4A6D, 0xBB, 0xE8, 0x19, 0x95, 0x95, 0x94, 0x45, 0x46);

// {9CF1A336-E72B-4a6d-BBE8-199595944546}
// Brightness level
DEFINE_GUID(MC_Brightness,
0x9CF1A336, 0xE72B, 0x4A6D, 0xBB, 0xE8, 0x19, 0x95, 0x95, 0x94, 0x45, 0x46);

// Contrast
// {23E2776D-3BB5-4aa5-9563-4DBE5AF9539E}
DEFINE_GUID(MC_Contrast,
0x23E2776D, 0x3BB5, 0x4AA5, 0x95, 0x63, 0x4D, 0xBE, 0x5A, 0xF9, 0x53, 0x9E);

// Selecting output deinterlacing mode
// {9CF1A332-E72B-4a6d-BBE8-199595944546}
DEFINE_GUID(MC_Deinterlace,
0x9CF1A332, 0xE72B, 0x4A6D, 0xBB, 0xE8, 0x19, 0x95, 0x95, 0x94, 0x45, 0x46);

// Selecting skip mode(I,IP,IBP) or obey quality messages
// {9CF1A330-E72B-4a6d-BBE8-199595944546}
DEFINE_GUID(MC_Quality,
0x9CF1A330, 0xE72B, 0x4A6D, 0xBB, 0xE8, 0x19, 0x95, 0x95, 0x94, 0x45, 0x46);

// Enable/disable on screen display info (only software mode)
// {F5C51906-ED89-4c6e-9C37-A5CCB34F5389}
DEFINE_GUID(MC_OSD,
0xF5C51906, 0xED89, 0x4C6E, 0x9C, 0x37, 0xA5, 0xCC, 0xB3, 0x4F, 0x53, 0x89);

// Get last decoded picture. Call GetValue(...) and send empty VARIANT argument. 
// Reinterpreted pbVal to EM2VD_CapturePictureInfo
// struct pointer. Free allocated memory throw CoTaskMemFree on the end.
// {6118A160-0FF0-43c8-94E4-345AC9E9F362}
DEFINE_GUID(MC_CapturePicture,
0x6118A160, 0xFF0, 0x43C8, 0x94, 0xE4, 0x34, 0x5A, 0xC9, 0xE9, 0xF3, 0x62);

// Count of decoded frames (read only)
// {575BA759-6F13-4a84-A126-005A5523D203}
DEFINE_GUID(MC_FramesDecoded,
0x575BA759, 0x6F13, 0x4A84, 0xA1, 0x26, 0x0, 0x5A, 0x55, 0x23, 0xD2, 0x3);

// Count of skipped frames (read only)
// {592A9FAB-CBF8-4592-A0B2-21D0C79DACE4}
DEFINE_GUID(MC_FramesSkipped,
0x592A9FAB, 0xCBF8, 0x4592, 0xA0, 0xB2, 0x21, 0xD0, 0xC7, 0x9D, 0xAC, 0xE4);

// Use multi-thread if available (available on multi-processors systems, only software decode mode, only MPEG-2 streams)
// {0612C1C6-DEF7-4d01-A0DA-90F426F9B312}
DEFINE_GUID(MC_MultiThread,
0x612C1C6, 0xDEF7, 0x4D01, 0xA0, 0xDA, 0x90, 0xF4, 0x26, 0xF9, 0xB3, 0x12);

// Number of threads to be used by codec. Codec uses number of threads which are equal to number of processors (logical, physical)
// which are installed on PC when MC_NumThreads equal to zero.
// {39108716-B2F5-491c-9E9B-C2ABF38475B4}
DEFINE_GUID(MC_NumThreads,
0x39108716, 0xb2f5, 0x491c, 0x9e, 0x9b, 0xc2, 0xab, 0xf3, 0x84, 0x75, 0xb4);


// Media time of current frame (it may be a byte offset of current frame)
// {3903A73E-6A89-4e09-8E9F-95E8A56F614D}
DEFINE_GUID(MC_MediaTimeStart,
0x3903A73E, 0x6A89, 0x4E09, 0x8E, 0x9F, 0x95, 0xE8, 0xA5, 0x6F, 0x61, 0x4D);

// Media time of current frame (it may be a byte offset of current frame)
// {BCF5D243-B80E-400a-9B60-035A1D3E5C38}
DEFINE_GUID(MC_MediaTimeStop,
0xBCF5D243, 0xB80E, 0x400A, 0x9B, 0x60, 0x3, 0x5A, 0x1D, 0x3E, 0x5C, 0x38);

// Reset decoder statistics (decoded and skipped frames counter).
// {164966A1-2BFD-4c74-A80E-E5769A219B9F}
DEFINE_GUID(MC_ResetStatistics,
0x164966A1, 0x2BFD, 0x4C74, 0xA8, 0xE, 0xE5, 0x76, 0x9A, 0x21, 0x9B, 0x9F);

// Don't show frames with errors
// {BB8F00E9-655B-47c4-966A-A4B4BBF8D2D2}
DEFINE_GUID(MC_ErrorConcealment,
0xBB8F00E9, 0x655B, 0x47C4, 0x96, 0x6A, 0xA4, 0xB4, 0xBB, 0xF8, 0xD2, 0xD2);

// Enable "maintain aspect ratio" for VMR-7,9 while connection is complete
// {7F261D15-62E2-46e5-86EB-3537DD72DD56}
DEFINE_GUID(MC_VMRMaintainAspectRatio,
0x7F261D15, 0x62E2, 0x46E5, 0x86, 0xEB, 0x35, 0x37, 0xDD, 0x72, 0xDD, 0x56);

// Set/get VESA settings (VESA_settings_t, mcvesa.h)
// {B1E68179-E7A1-4cf9-96B5-4D8F9E671C6C}
DEFINE_GUID(MC_VESA_SETTINGS, 
0xb1e68179, 0xe7a1, 0x4cf9, 0x96, 0xb5, 0x4d, 0x8f, 0x9e, 0x67, 0x1c, 0x6c);

//Get VESA buffers (VESA_buffers_t, mcvesa.h)
// {9238CD58-BC76-4679-90AB-808168CB720C}
DEFINE_GUID(MC_VESA_BUFFERS, 
0x9238cd58, 0xbc76, 0x4679, 0x90, 0xab, 0x80, 0x81, 0x68, 0xcb, 0x72, 0xc);

// {4B5E5B27-8D3F-492a-8B28-1265E7E5CE51}
DEFINE_GUID(MC_LowLatency, 
0x4b5e5b27, 0x8d3f, 0x492a, 0x8b, 0x28, 0x12, 0x65, 0xe7, 0xe5, 0xce, 0x51);

// {938C1C39-2D34-4ee5-9789-E67C0D1ADD40}
DEFINE_GUID(MC_StreamOrder,
0x938c1c39, 0x2d34, 0x4ee5, 0x97, 0x89, 0xe6, 0x7c, 0xd, 0x1a, 0xdd, 0x40);

// {0C58E5D5-B706-4724-8F00-43C65773F75D}
DEFINE_GUID(MC_OutputQueue,
0xc58e5d5, 0xb706, 0x4724, 0x8f, 0x0, 0x43, 0xc6, 0x57, 0x73, 0xf7, 0x5d);

// {4B0A9195-A6AE-41f6-BF58-8321D76E1661}
DEFINE_GUID(MC_OutputYUY2,
0x4b0a9195, 0xa6ae, 0x41f6, 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61);

// {4B0A9196-A6AE-41f6-BF58-8321D76E1661}
DEFINE_GUID(MC_OutputUYVY,
0x4b0a9196, 0xa6ae, 0x41f6, 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61);

// {4B0A9197-A6AE-41f6-BF58-8321D76E1661}
DEFINE_GUID(MC_OutputYV12,
0x4b0a9197, 0xa6ae, 0x41f6, 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61);

// {4B0A9198-A6AE-41f6-BF58-8321D76E1661}
DEFINE_GUID(MC_OutputRGB32,
0x4b0a9198, 0xa6ae, 0x41f6, 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61);

// {4B0A9199-A6AE-41f6-BF58-8321D76E1661}
DEFINE_GUID(MC_OutputRGB24,
0x4b0a9199, 0xa6ae, 0x41f6, 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61);

// {4B0A919A-A6AE-41f6-BF58-8321D76E1661}
DEFINE_GUID(MC_OutputRGB565,
0x4b0a919a, 0xa6ae, 0x41f6, 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61);

// {4B0A919B-A6AE-41f6-BF58-8321D76E1661}
DEFINE_GUID(MC_OutputRGB555,
0x4b0a919b, 0xa6ae, 0x41f6, 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61);

// {4B0A919C-A6AE-41f6-BF58-8321D76E1661}
DEFINE_GUID(MC_OutputAYUV,
0x4b0a919c, 0xa6ae, 0x41f6, 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61);

#endif // _COMMON_MC_

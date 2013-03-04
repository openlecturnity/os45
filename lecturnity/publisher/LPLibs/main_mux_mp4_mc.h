//////////////////////////////////////////////////////////////////////////
//
//	file name:	mux_mp4_mc.h
//
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008-2009 MainConcept GmbH
//  All rights are reserved.  Reproduction  in whole or  in part is  prohibited
//  without the written consent of the copyright owner.
//
//  MainConcept GmbH reserves the right  to make changes to this software program
//  without notice  at any time.  MainConcept GmbH makes no warranty,  expressed,
//  implied or statutory, including but not limited to  any implied warranty of
//  merchantability of fitness for any particular purpose.
//  MainConcept GmbH  does not represent  or warrant that  the programs furnished
//  hereunder are free of infringement of any third-party patents, copyright or
//  trademark.
//  In no event shall MainConcept GmbH be liable for any incidental, punitive, or
//  consequential damages of any kind  whatsoever arising from the use of these
//  programs.
//
///////////////////////////////////////////////////////////////////////////////
//
//  Purpose: The definition of the MainConcept MP4 Multiplexer parameter CLSIDs
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(__EMP4MUX_HEADER__)
#define __EMP4MUX_HEADER__


//========================================================================
//						FILTER PARAMETERS UIDS
//========================================================================

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	GUID								Value Type	Available range		Default Value	Note

//	EMP4Mux_OutputFormat         		VT_INT		0,3        	        0               Output file format (mp4, 3gpp, mj2, mov)
//	EMP4Mux_Compatibility         		VT_INT		0,5   				0               Stream compatibility format
//	EMP4Mux_MuxTime         			VT_I8		0,1        	        0               Current muxer time
//	EMP4Mux_OutputFileName				VT_BSTR		[0..255](Unicode)	""				Output file name for SonyPSP
//	EMP4Mux_LiveMode         			VT_INT		0,1   				1               Get times from media samples

//	EMP4Mux_FormatVersion				VT_INT		0..INT16MAX			0				Set format version for Sony PSP files
//	EMP4Mux_OwnerCode					VT_INT		0..INT16MAX			0				Set owner code for Sony PSP files
//  MP4Mux_VideoLen                     VT_INT      1-300               10

// {67F699AE-4856-4292-99F1-BB6FDB596E43}
DEFINE_GUID(EMP4Mux_OutputFormat, 
0x67f699ae, 0x4856, 0x4292, 0x99, 0xf1, 0xbb, 0x6f, 0xdb, 0x59, 0x6e, 0x43);

// {AB50A92F-1AF8-4f03-A35B-7EDEF95AE1F0}
DEFINE_GUID(EMP4Mux_Compatibility, 
0xab50a92f, 0x1af8, 0x4f03, 0xa3, 0x5b, 0x7e, 0xde, 0xf9, 0x5a, 0xe1, 0xf0);

// {895E4A88-2F56-4503-97E4-F9E6C35070AF}
DEFINE_GUID(EMP4Mux_MuxTime, 
0x895e4a88, 0x2f56, 0x4503, 0x97, 0xe4, 0xf9, 0xe6, 0xc3, 0x50, 0x70, 0xaf);

// {9ABE6978-4304-42b6-9C55-EC6A62D311E3}
DEFINE_GUID(EMP4Mux_OutputFileName, 
0x9abe6978, 0x4304, 0x42b6, 0x9c, 0x55, 0xec, 0x6a, 0x62, 0xd3, 0x11, 0xe3);

// {FD0BD388-078B-4f13-9C89-16760396EC03}
DEFINE_GUID(EMP4Mux_LiveMode, 
0xfd0bd388, 0x78b, 0x4f13, 0x9c, 0x89, 0x16, 0x76, 0x3, 0x96, 0xec, 0x3);

// {2CA7BE73-585F-4d80-AB96-770FE2A6C935}
DEFINE_GUID(EMP4Mux_RemoveSEI, 
0x2ca7be73, 0x585f, 0x4d80, 0xab, 0x96, 0x77, 0xf, 0xe2, 0xa6, 0xc9, 0x35);

// {26E31D9E-228A-4ace-A05E-C01B4E89DFB2}
DEFINE_GUID(EMP4Mux_KeepAUD, 
0x26e31d9e, 0x228a, 0x4ace, 0xa0, 0x5e, 0xc0, 0x1b, 0x4e, 0x89, 0xdf, 0xb2);

// {FFDEA675-B47C-458e-A0EB-403EC8FD5058}
DEFINE_GUID(EMP4Mux_KeepPS, 
0xffdea675, 0xb47c, 0x458e, 0xa0, 0xeb, 0x40, 0x3e, 0xc8, 0xfd, 0x50, 0x58);

///////////////////////////////////////////
// For Sony PSP files
///////////////////////////////////////////

// {277AA7A4-CF30-4c84-89BF-5DFECE9AAE57}
DEFINE_GUID(EMP4Mux_FormatVersion, 
0x277aa7a4, 0xcf30, 0x4c84, 0x89, 0xbf, 0x5d, 0xfe, 0xce, 0x9a, 0xae, 0x57);

// {427CD7D8-C6EC-4620-BAFF-41F088346BAB}
DEFINE_GUID(EMP4Mux_OwnerCode, 
0x427cd7d8, 0xc6ec, 0x4620, 0xba, 0xff, 0x41, 0xf0, 0x88, 0x34, 0x6b, 0xab);

// {43BE713B-6699-425c-B847-21B7D5B8AD85}
static const GUID MP4Mux_VideoLen = 
{ 0x43be713b, 0x6699, 0x425c, { 0xb8, 0x47, 0x21, 0xb7, 0xd5, 0xb8, 0xad, 0x85 } };

// 
DEFINE_GUID(EMP4Mux_AtomOrder,
0xbbed4372, 0xbe86, 0x4437, 0xa5, 0x9e, 0x1a, 0x80, 0xd, 0x48, 0x12, 0xc3);

namespace EMP4Muxer
{
    typedef enum tagOutputFormats {
        FF_MP4      = 0,
        FF_JPEG2000 = 1,
        FF_3GPP     = 2,

    } OutputFormats;
    
    typedef enum tagCompatibilityFormat{
        CF_Default  = 0,
        CF_ISMA,
        CF_SonyPSP,
        CF_iPod,
        CF_QT,
        CF_FLASH,
        CF_iPhone

    } CompatibilityFormat;

    typedef enum atom_order_e{
    AO_None = 0,
    AO_MoovFirst

    }atom_order_t;
};

#endif //#if !defined(__EMP4MUX_HEADER__)

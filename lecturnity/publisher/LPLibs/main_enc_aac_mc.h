///////////////////////////////////////////////////////////////////////////////
//
//  file name:  enc_aac_mc.h
//
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005-2009 MainConcept GmbH
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
//  Purpose: Definition of the MainConcept AAC encoder parameters CLSIDs
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(MC_AAC_ENCODER_CONFIG)
#define MC_AAC_ENCODER_CONFIG

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IModuleConfig GUIDs
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const GUID ELAACENC_MPEG_Version =
{0xfd882e86, 0xdcc2, 0x4120, {0xa6, 0xef, 0xe6, 0xe8, 0x5b, 0xc2, 0xd0, 0x59}};

static const GUID ELAACENC_Object_Type =
{0x49c7eb31, 0x2e21, 0x41ef, {0x92, 0xbe, 0x46, 0xc9, 0x10, 0x47, 0x45, 0xde}};

static const GUID ELAACENC_Output_Format =
{0xeeae27e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0xb7}};

static const GUID ELAACENC_HF_Cutoff =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x00}};

static const GUID ELAACENC_VBR =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x01}};

static const GUID ELAACENC_VBR_Mode =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x02}};

static const GUID ELAACENC_SBR =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x03}};

static const GUID ELAACENC_Frame_Count =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x04}};

static const GUID ELAACENC_Byte_Count =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x05}};

static const GUID ELAACENC_Byte_Count_Long = 
{ 0xad86ebf, 0x5fe3, 0x48bc, { 0xb8, 0xe0, 0xe2, 0x97, 0x78, 0x1d, 0x95, 0xae } };

static const GUID ELAACENC_Sample_Rate =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x06}};

static const GUID ELAACENC_Core_Sample_Rate =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x07}};

static const GUID ELAACENC_PS =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x08}};

static const GUID ELAACENC_Channel_Config =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x09}};

static const GUID ELAACENC_Core_Channel_Config =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x0a}};

static const GUID AACENC_SBR_Available =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x0b}};

static const GUID AACENC_PS_Available =
{0x553936e3, 0xd553, 0x4db0, { 0xa8, 0x3e, 0xe7, 0x28, 0x80, 0x33, 0xa0, 0x0c}};

static const GUID ELAACENC_Protect_ADTS_Stream = 
{0xb3a79c4c, 0xea47, 0x4ccb, { 0xbe, 0xb5, 0x76, 0xaa, 0xb0, 0xa3, 0x49, 0xf9}};

static const GUID ELAACENC_SetDefValues = 
{ 0x237776cf, 0x191b, 0x41e8, { 0xb7, 0x7d, 0x75, 0xd9, 0x60, 0x84, 0xf0, 0x1f } };



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GUID                         Type        Available range     Default value   Note
// MC_BITRATE_AVG               VT_I4       [0, 27]             [19]            Sets average output bit rate
// ELAACENC_MPEG_Version        VT_I4       [0, 1]              [0]             Sets the output format (MPEG-2 or MPEG-4)
// ELAACENC_Object_Type         VT_I4       [0, 1]              [1]             Sets the audio object type (Main or Low Complexity)
// ELAACENC_Output_Format       VT_I4       [0, 1]              [1]             Sets the output bit stream format (raw or with ADTS headers)
// ELAACENC_HF_Cutoff           VT_I4       [0, 1]              [1]             Enables/disables high frequency cut-off
// ELAACENC_VBR                 VT_I4       [0, 1]              [0]             Enables/disables variable bit rate encoding mode
// ELAACENC_VBR_Mode            VT_I4       [1, 9]              [6]             Sets variable bit rate mode quality
// ELAACENC_SBR                 VT_I4       [0, 1]              [0]             Enables/disables spectral band replication (HE AAC v1)
// ELAACENC_PS                  VT_I4       [0, 1]              [0]             Enables/disables parametric stereo (HE AAC v2)
// ELAACENC_Frame_Count         VT_I4                           Read only       Retrieves the number of encoded AAC frames
// ELAACENC_Byte_Count          VT_I4                           Read only       Retrieves the number of output bytes
// ELAACENC_Byte_Count_Long     VT_I8                           Read only       Retrieves the number of output bytes
// ELAACENC_Sample_Rate         VT_I4                           Read only       Retrieves the output sample rate
// ELAACENC_Core_Sample_Rate    VT_I4                           Read only       Retrieves the SBR core sample rate
// ELAACENC_Channel_Config      VT_I4                           Read only       Retrieves the output channel config
// ELAACENC_Core_Channel_Config VT_I4                           Read only       Retrieves the PS core channel config
// AACENC_SBR_Available         VT_I4                           Read only       Retrieves whether SBR encoding is available
// AACENC_PS_Available          VT_I4                           Read only       Retrieves whether PS encoding is available
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace EAACENC
{
    //
    // bit rate
    //
    enum aacenc_bit_rate_e
    {
        IMC_AACENC_BR_6 = 0,
        IMC_AACENC_BR_7,
        IMC_AACENC_BR_8,
        IMC_AACENC_BR_10,
        IMC_AACENC_BR_12,
        IMC_AACENC_BR_14,
        IMC_AACENC_BR_16,
        IMC_AACENC_BR_20,
        IMC_AACENC_BR_24,
        IMC_AACENC_BR_28,
        IMC_AACENC_BR_32,
        IMC_AACENC_BR_40,
        IMC_AACENC_BR_48,
        IMC_AACENC_BR_56,
        IMC_AACENC_BR_64,
        IMC_AACENC_BR_80,
        IMC_AACENC_BR_96,
        IMC_AACENC_BR_112,
        IMC_AACENC_BR_128,
        IMC_AACENC_BR_160,
        IMC_AACENC_BR_192,
        IMC_AACENC_BR_224,
        IMC_AACENC_BR_256,
        IMC_AACENC_BR_320,
        IMC_AACENC_BR_384,
        IMC_AACENC_BR_448,
        IMC_AACENC_BR_512,
        IMC_AACENC_BR_1024,
        IMC_AACENC_BR_END
    };

    const int idx2bit_rate[] =
    {
        6,
        7,
        8,
        10,
        12,
        14,
        16,
        20,
        24,
        28,
        32,
        40,
        48,
        56,
        64,
        80,
        96,
        112,
        128,
        160,
        192,
        224,
        256,
        320,
        384,
        448,
        512,
        1024
    };


    //
    // MPEG version
    //
    enum aacenc_mpeg_version_e
    {
        IMC_AACENC_MV_M4 = 0,
        IMC_AACENC_MV_M2,
        IMC_AACENC_MV_M4_PSP,
        IMC_AACENC_MV_END
    };

    //
    // audio object type
    //
    enum aacenc_aot_e
    {
        IMC_AACENC_AOT_MAIN = 0,  // forbidden from now on
        IMC_AACENC_AOT_LC,
        IMC_AACENC_AOT_END
    };

    //
    // output format
    //
    enum aacenc_output_mode_e
    {
        IMC_AACENC_OUT_RAW = 0,
        IMC_AACENC_OUT_ADTS = 1,
        IMC_AACENC_OUT_LATM = 2,
        IMC_AACENC_OUT_END
    };

    //
    // VBR quality
    //
    enum aacenc_vbr_mode_e
    {
        IMC_AACENC_VBR_LOW1     = 1,
        IMC_AACENC_VBR_LOW2     = 2,
        IMC_AACENC_VBR_LOW3     = 3,
        IMC_AACENC_VBR_MEDIUM1  = 4,
        IMC_AACENC_VBR_MEDIUM2  = 5,
        IMC_AACENC_VBR_MEDIUM3  = 6,
        IMC_AACENC_VBR_HIGH1    = 7,
        IMC_AACENC_VBR_HIGH2    = 8,
        IMC_AACENC_VBR_HIGH3    = 9
    };

    enum aacenc_trigger_e
    {
        IMC_AACENC_OFF = 0,
        IMC_AACENC_ON = 1
    };

    enum aacenc_preset_e
    {
        IMC_AACENC_PRESET_DEFAULT       = 0,
        IMC_AACENC_PRESET_PSP           = 0x00004000,
        IMC_AACENC_PRESET_IPOD          = 0x00005000,
        IMC_AACENC_PRESET_3GPP          = 0x00006000,
        IMC_AACENC_PRESET_3GPP2         = 0x00006100,
        IMC_AACENC_PRESET_ISMA          = 0x00006200,
        IMC_AACENC_PRESET_FLASH_LOWRES  = 0x00011010,
        IMC_AACENC_PRESET_FLASH_HIGHRES = 0x00011011,
        IMC_AACENC_PRESET_SILVERLIGHT   = 0x00012000
    };
};

#endif // MC_AAC_ENCODER_CONFIG


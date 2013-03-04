///////////////////////////////////////////////////////////////////////////////
//
//  file name:  enc_avc_mc.h
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
//  Purpose: Definition of the MainConcept AVC/H.264 encoder parameters CLSIDs
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(__PROPID_H264ENC_HEADER__)
#define __PROPID_H264ENC_HEADER__


//////////////////////////////////////////////////////////////////////////
//    Filter GUIDs
//////////////////////////////////////////////////////////////////////////

static const GUID CLSID_H264FileFormat =
{0x8d2d71cb, 0x243f, 0x45e3, {0xb2, 0xd8, 0x5f, 0xd7, 0x96, 0x7e, 0xc0, 0x9b}};

static const GUID CLSID_v210 =
{0x30313276, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};

#define H264VE_MAX_CHAPTER_LIST 1023


namespace H264VE
{
    typedef enum Profile
    {
        Profile_Baseline    = 0,
        Profile_Main        = 1,
        Profile_High        = 3,
        Profile_High10      = 4,
        Profile_High422     = 5,
        Profile_High444     = 6

    } Profile_t;

    typedef enum Level
    {
        Level_1     = 10,
        Level_1b    = 16,
        Level_11    = 11,
        Level_12    = 12,
        Level_13    = 13,
        Level_2     = 20,
        Level_21    = 21,
        Level_22    = 22,
        Level_3     = 30,
        Level_31    = 31,
        Level_32    = 32,
        Level_4     = 40,
        Level_41    = 41,
        Level_42    = 42,
        Level_5     = 50,
        Level_51    = 51,
        Level_Auto  = 100

    } Level_t;

    typedef enum ColourSampling
    {
        Chroma_400  = 1,
        Chroma_420  = 2,
        Chroma_422  = 3,
        Chroma_444  = 4

    } ColourSampling_t;

    typedef enum BitRateMode
    {
        BitRateMode_CBR = 0,
        BitRateMode_CQT = 1,
        BitRateMode_VBR = 2,
        BitRateMode_TQM = 3

    } BitRateMode_t;

    typedef enum BitRatePass
    {
        BitRatePass_Simple = 0,
        BitRatePass_Analyse = 1,
        BitRatePass_Encode = 2

    } BitRatePass_t;

    typedef enum EntropyCodingMode
    {
        EntropyCodingMode_CAVLC = 0,
        EntropyCodingMode_CABAC = 1

    } EntropyCodingMode_t;

    typedef enum VideoType
    {
        VideoType_BASELINE          = 0,
        VideoType_CIF               = 1,
        VideoType_MAIN              = 2,
        VideoType_SVCD              = 3,
        VideoType_D1                = 4,
        VideoType_HIGH              = 5,
        VideoType_DVD               = 6,
        VideoType_HD_DVD            = 7,
        VideoType_BD                = 8,
        VideoType_BD_HDMV           = 9,
        VideoType_PSP               = 10,
        VideoType_HDV_HD1           = 11,
        VideoType_HDV_HD2           = 12,
        VideoType_iPOD              = 15,
        VideoType_AVCHD             = 14,
        VideoType_1SEG              = 16,
        VideoType_INTRA_CLASS_50    = 18,
        VideoType_INTRA_CLASS_100   = 19,
        VideoType_DIVXPLUS          = 22,
        VideoType_3GP               = 25,
        VideoType_Silverlight       = 26

    } VideoType_t;

    typedef enum VideoPullDownFlag
    {
        VideoPullDownFlag_NONE  = 0,
        VideoPullDownFlag_23    = 1,
        VideoPullDownFlag_32    = 2,
        VideoPullDownFlag_23_p  = 4,
        VideoPullDownFlag_32_p  = 5,
        VideoPullDownFlag_Auto  = 6,
        VideoPullDownFlag_22_p  = 7,
        VideoPullDownFlag_33_p  = 8

    } VideoPullDownFlag_t;

    typedef enum IntraRefreshMode
    {
        IntraRefreshMode_Off    = 0,
        IntraRefreshMode_Slow   = 1,
        IntraRefreshMode_Medium = 2,
        IntraRefreshMode_Fast   = 3

    } IntraRefreshMode_t;

    typedef enum MESubpelMode
    {
        MESubpelMode_FullPel    = 0,
        MESubpelMode_HalfPel    = 1,
        MESubpelMode_QuarterPel = 2

    } MESubpelMode_t;

    typedef enum VSCDMode
    {
        VSCDMode_OFF    = 0,
        VSCDMode_IDR    = 1

    } VSCDMode_t;

    typedef enum VideoFormat
    {
        VideoFormat_Auto        = 0,
        VideoFormat_PAL         = 1,
        VideoFormat_NTSC        = 2,
        VideoFormat_SECAM       = 3,
        VideoFormat_MAC         = 4,
        VideoFormat_Unspecified = 5

    } VideoFormat_t;

    typedef enum StreamType
    {
        StreamTypeI = 0,
        StreamTypeIplusSEI = 1,
        StreamTypeII = 2,
        StreamTypeIIminusSEI = 3

    } StreamType_t;

    typedef enum CpuOptimization
    {
        CPU_Auto = 0,
        CPU_PlainC = 1,
        CPU_MMX = 2,
        CPU_MMX_Ext = 3,
        CPU_SSE = 4,
        CPU_SSE2 = 5,
        CPU_SSE3 = 6

    } CpuOptimization_t;

    typedef enum QuantMode
    {
        QuantMode_Ref = 0,
        QuantMode_Mode1 = 1,
        QuantMode_Mode2 = 2

    } QuantMode_t;

    typedef enum ColorSpace
    {
        ColorSpace_NA      = -1,
        ColorSpace_YUV_420 = 0,
        ColorSpace_YUV_422 = 1,
        ColorSpace_YUV_444 = 2,
        ColorSpace_RGB_24  = 3,
        ColorSpace_RGB_32  = 4

    } ColorSpace_t;

    typedef enum FpsConvType
    {
        FpscType_Fps        = 0,
        FpscType_Timestamps = 1

    } FpsConvType_t;

    typedef enum HrdParamWriting
    {
        HRD_None    = 0,
        HRD_NAL     = 1,
        HRD_VCL     = 2,
        HRD_Both    = 3

    } HrdParamWriting_t;

    typedef enum TimestampMode
    {
        TSMode_Original = 0,    // do not adjust, use exact input timestamps
        TSMode_AVI      = 1     // AVI-compatibility mode - ascending, non-overlapping timestamps (must exist for every sample)

    } TimestampMode_t;

    typedef enum OutputMediatype
    {
        OutMT_Original = 0,     // regular one
        OutMT_VSS      = 1      // VSSH fourCC

    } OutputMediatype_t;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    GUID                                  Type        Available range                     Default Val         Note
//MC_PRESET                                VT_INT      see VideoType_t                     [VideoType_BASELINE] Switching between presets
//EH264VE_VideoFormat                       VT_INT      see VideoFormat_t                   [VideoFormat_Auto]  Switching between Video formats

//MC_PROFILE                               VT_INT      see Profile_t                       [Profile_Baseline]  Switching between Profiles
//MC_LEVEL                                 VT_INT      see Level_t                         [Level_Auto]        Switching between Levels

//EH264VE_interlace_mode                    VT_INT      [0, 1]                              [0]                 Switching between sequence modes
//EH264VE_field_order                       VT_INT      [0, 1]                              [0]                 Field order (0 - TFF, 1 - BFF)
//EH264VE_video_pulldown_flag               VT_INT      see VideoPullDownFlag_t             [VideoPullDownFlag_Auto] Switching between Video PullDown Flag
//EH264VE_slice_arg                         VT_INT      [0, number of MB rows]              [1]                 Slice count

//MC_GOP_LENGTH                            VT_INT      [1, 300]                            [33]                Max GOP length
//MC_GOP_BCOUNT                            VT_INT      [0, 3]                              [0]                 Max number of b-frames
//EH264VE_idr_frequency                     VT_INT      [0, limit of INT]                   [1]                 Frequency of IDR-pictures
//EH264VE_gop_min_length                    VT_INT      [1, 300]                            [1]                 Min GOP length
//EH264VE_b_slice_reference                 VT_INT      [0, 1]                              [0]                 Use or not B-frame as reference
//EH264VE_b_slice_pyramid                   VT_INT      [0, 1]                              [0]                 Enable/Disable pyramidal coding
//EH264VE_adapt_b                           VT_INT      [0, 1]                              [0]                 Enable/Disable adaptive B-frames placement
//EH264VE_vcsd_mode                         VT_INT      see VSCDMode_t                      [VSCDMode_IDR]      Enable/Disable scene change detector

//MC_BITRATE_MODE                          VT_INT      see BitRateMode_t                   [BitRateMode_VBR]   Switching between rate control modes
//EH264VE_quant_pI                          VT_INT      [0, 51]                             [25]                Quantizer for I-slices
//EH264VE_quant_pP                          VT_INT      [0, 51]                             [25]                Quantizer for P-slices
//EH264VE_quant_pB                          VT_INT      [0, 51]                             [27]                Quantizer for B-slices
//EH264VE_cb_offset                         VT_INT      [-51, 51]                           [1]                 Cb chroma qp offset
//EH264VE_cr_offset                         VT_INT      [-51, 51]                           [1]                 Cr chroma qp offset
//MC_BITRATE_AVG                           VT_INT      [1024, 288000000]                   [600000]            Average bit rate (bits per second)
//MC_BITRATE_MAX                           VT_INT      [EH264VE_bit_rate, 288000000]       [1149952]           Hypothetical stream scheduler rate (bits per second)
//EH264VE_bit_rate_buffer_size              VT_INT      [1024, 288000000]                   [1149984]           Bit-rate buffer size (bits)
//EH264VE_vbv_buffer_fullness               VT_INT      [1, limit of INT]                   [67500]             Initial VBV buffer fullness (90 kHz)
//EH264VE_vbv_buffer_fullness_trg           VT_INT      [0, limit of INT]                   [0]                 Final VBV buffer fullness (90 kHz)
//MC_BITRATE_PASS                          VT_INT      see BitRatePass_t                   [BitRatePass_Simple] Multi pass encoding mode
//MC_BITRATE_STAT                          VT_BSTR                                                             Multi pass statistics file

//EH264VE_search_range                      VT_INT      [0, 511]                            [63]                Motion Estimation Search range
//EH264VE_num_reference_frames              VT_INT      [0, 16]                             [1]                 Number of reference frames
//EH264VE_inter_search_shape                VT_INT      [0, 1]                              [1]                 Use or not subblock search
//EH264VE_me_subpel_mode                    VT_INT      see MESubpelMode_t                  [MESubpelMode_QuarterPel] Subpixel depth
//EH264VE_me_weighted_p_mode                VT_INT      [0, 1]                              [0]                 Use or not weighted prediction
//EH264VE_fast_multi_ref_me                 VT_INT      [0, 1]                              [1]                 Use or not fast multi reference ME algo
//EH264VE_fast_sub_block_me                 VT_INT      [0, 1]                              [1]                 Use or not fast sub block ME algo
//EH264VE_allow_out_of_pic_mvs              VT_INT      [0, 1]                              [1]                 Allows mvs out of picture boundaries

//EH264VE_enable_intra_big                  VT_INT      [0, 1]                              [1]                 Enable Intra 16x16 mode in intra slices
//EH264VE_enable_intra_8x8                  VT_INT      [0, 1]                              [1]                 Enable Intra 8x8 mode in intra slices
//EH264VE_enable_intra_4x4                  VT_INT      [0, 1]                              [1]                 Enable Intra 4x4 mode in intra slices
//EH264VE_enable_intra_pcm                  VT_INT      [0, 1]                              [1]                 Enable Intra pcm mode in intra slices
//EH264VE_enable_inter_big                  VT_INT      [0, 1]                              [1]                 Enable Intra 16x16 mode in inter slices
//EH264VE_enable_inter_8x8                  VT_INT      [0, 1]                              [1]                 Enable Intra 8x8 mode in inter slices
//EH264VE_enable_inter_4x4                  VT_INT      [0, 1]                              [1]                 Enable Intra 4x4 mode in inter slices
//EH264VE_enable_inter_pcm                  VT_INT      [0, 1]                              [1]                 Enable Intra pcm mode in inter slices

//EH264VE_use_deblocking_filter             VT_INT      [0, 1]                              [1]                 Use deblocking filter or not
//EH264VE_deblocking_alphaC0_offset         VT_INT      [-6, 6]                             [-1]                Deblocking alpha c0 offset
//EH264VE_deblocking_beta_offset            VT_INT      [-6, 6]                             [-1]                Deblocking beta offset

//EH264VE_entropy_coding_mode               VT_INT      see EntropyCodingMode_t             [EntropyCodingMode_CAVLC] Switching between entropy coding modes
//EH264VE_rd_optimization                   VT_INT      [0, 1]                              [1]                 Enable/Disable RD optimization
//EH264VE_fast_rd_optimization              VT_INT      [0, 1]                              [1]                 Enable/Disable fast RD optimization
//EH264VE_use_hadamard_transform            VT_INT      [0, 1]                              [1]                 Enable/Disable Hadamard transform for mode decision
//EH264VE_enable_fast_inter_decisions       VT_INT      [0, 1]                              [1]                 Enable/Disable fast inter decision
//EH264VE_enable_fast_intra_decisions       VT_INT      [0, 1]                              [1]                 Enable/Disable fast intra decision
//EH264VE_quant_mode                        VT_INT      see QuantMode_t                     [QuantMode_Mode2]   Quantization optimization
//EH264VE_fgo_strength                      VT_INT      [0, 100]                            [0]                 Film grain optimization strength

//EH264VE_use_sample_aspect_ratio           VT_INT      [0, 1]                              [0]                 Indicates whether to use sample or picture AR
//EH264VE_auto_aspect_ratio                 VT_INT      [0, 1]                              [1]                 Auto picture aspect ratio
//EH264VE_aspect_ratioX                     VT_INT      [1, 1048576]                        [4]                 Picture aspect ratio x
//EH264VE_aspect_ratioY                     VT_INT      [1, 1048576]                        [3]                 Picture aspect ratio y
//EH264VE_auto_sample_aspect_ratio          VT_INT      [0, 1]                              [1]                 Auto sample aspect ratio
//EH264VE_sample_aspect_ratioX              VT_INT      [1, 1048576]                        [1]                 Sample aspect ratio x
//EH264VE_sample_aspect_ratioY              VT_INT      [1, 1048576]                        [1]                 Sample aspect ratio y

//EH264VE_stream_type                       VT_INT      see StreamType_t                    [StreamTypeII]      Output stream type
//EH264VE_video_full_range                  VT_INT      [0, 1]                              [0]                 Enable/Disable Full range colours
//EH264VE_write_au_delimiters               VT_INT      [0, 1]                              [1]                 Enable/Disable writing access unit delimiters
//EH264VE_write_seq_end_code                VT_INT      [0, 1]                              [1]                 Enable/Disable writing end of sequence code
//EH264VE_write_timestamps                  VT_INT      [0, 1]                              [1]                 Enable/Disable writing timestamps
//EH264VE_pic_order_present_flag            VT_INT      [0, 1]                              [0]                 Controls pic_order_present_flag value in PPS (e.g. for SBTVD-T)
//EH264VE_cpu_optimization                  VT_INT      see CpuOptimization_t               [CPU_Auto]          Enables all CPU tech optimization upto selected
//EH264VE_num_threads                       VT_INT      [0, 16]                             [0]                 Specifies maximum number of threads to be used

//EH264VE_FixedRate                         VT_INT      [0, 1]                              [0]                 Is sources frame rate fixed or not
//EH264VE_Framerate                         VT_R8       [1, 100]                            [in frame rate]     Frame rate if source frame rate is fixed
//EH264VE_OutFixedRate                      VT_INT      [0, 1]                              [0]                 Is output frame rate fixed or not
//EH264VE_OutFramerate                      VT_R8       [1, 100]                            [in frame rate]     Frame rate if output frame rate is fixed

//EH264VE_SetDefValues                      VT_INT      [0, 1]                              [0]                 Set default values

//EH264VE_OriginalFramerate                 VT_R8       [0, limit of DOUBLE]                READONLY            Source stream declared frame rate
//EH264VE_OriginalInterlace                 VT_INT      [-1, 1]                             READONLY            (0 - progressive, 1 - interlaced, -1 - not available)
//EH264VE_OriginalColorSpace                VT_INT      see ColorSpace_t                    READONLY            Original colour space
//EH264VE_OriginalBitDepth                  VT_INT      [0..16]                             READONLY            Bits per sample
//EH264VE_IsRunning                         VT_INT      [0, 1]                              READONLY            Is encoder running
//MC_ENCODED_FRAMES                        VT_INT      [0, limit of INT]                   READONLY            Encoded frames
//MC_FPS                                   VT_R8       [0, limit of DOUBLE]                READONLY            Average speed
//MC_CALC_PSNR                             VT_INT      [0, 1]                              [0]                 Calculate overall PSNR or not
//MC_OVERALL_PSNR                          VT_R8       [0, limit of DOUBLE]                READONLY            Overall PSNR (luma and chroma)
//MC_BITRATE_REAL                          VT_R8       [0, limit of DOUBLE]                READONLY            Real average bit rate
//EH264VE_def_horizontal_size               VT_INT      [0, limit of INT]                   READONLY            Horizontal size
//EH264VE_def_vertical_size                 VT_INT      [0, limit of INT]                   READONLY            Vertical size


//////////////////////////////////////////////////////////////////////////
//    Parameters GUIDs
//////////////////////////////////////////////////////////////////////////

// {F8A1E96D-A489-4ec1-9418-8B7D048364EF}
static const GUID EH264VE_VideoFormat =
{0xf8a1e96d, 0xa489, 0x4ec1, {0x94, 0x18, 0x8b, 0x7d, 0x4, 0x83, 0x64, 0xef}};


// {7CD6EF75-4420-4b77-BC22-4158B3F98956}
static const GUID EH264VE_interlace_mode =
{0x7cd6ef75, 0x4420, 0x4b77, {0xbc, 0x22, 0x41, 0x58, 0xb3, 0xf9, 0x89, 0x56}};

// {749C8599-F66A-4161-B640-9B8B75D62EBD}
static const GUID EH264VE_field_order =
{0x749c8599, 0xf66a, 0x4161, {0xb6, 0x40, 0x9b, 0x8b, 0x75, 0xd6, 0x2e, 0xbd}};

// {6DFB21B7-9F43-4ccb-81BF-5723B10E0533}
static const GUID EH264VE_video_pulldown_flag =
{0x6dfb21b7, 0x9f43, 0x4ccb, {0x81, 0xbf, 0x57, 0x23, 0xb1, 0xe, 0x5, 0x33}};

// {F519F267-3B17-481f-9F35-43CCA48FD098}
static const GUID EH264VE_slice_arg =
{0xf519f267, 0x3b17, 0x481f, {0x9f, 0x35, 0x43, 0xcc, 0xa4, 0x8f, 0xd0, 0x98}};


// {EECB285C-BDE6-4da5-9A9E-4C366BCACD94}
static const GUID EH264VE_idr_frequency =
{0xeecb285c, 0xbde6, 0x4da5, {0x9a, 0x9e, 0x4c, 0x36, 0x6b, 0xca, 0xcd, 0x94}};

// {9EAFCA29-42F3-4ca5-B40C-6B87B2AD4483}
static const GUID EH264VE_gop_min_length =
{0x9eafca29, 0x42f3, 0x4ca5, {0xb4, 0xc, 0x6b, 0x87, 0xb2, 0xad, 0x44, 0x83}};

// {90E40368-D85C-4139-8540-82A58C12B4C8}
static const GUID EH264VE_b_slice_reference =
{0x90e40368, 0xd85c, 0x4139, {0x85, 0x40, 0x82, 0xa5, 0x8c, 0x12, 0xb4, 0xc8}};

// {A3A30C66-9B00-44b0-827E-ABBC3BB0E4F4}
static const GUID EH264VE_b_slice_pyramid =
{0xa3a30c66, 0x9b00, 0x44b0, {0x82, 0x7e, 0xab, 0xbc, 0x3b, 0xb0, 0xe4, 0xf4}};

// {956A704B-00FC-436c-921E-BC6158955A82}
static const GUID EH264VE_adapt_b =
{0x956a704b, 0xfc, 0x436c, {0x92, 0x1e, 0xbc, 0x61, 0x58, 0x95, 0x5a, 0x82}};

// {CBFC3E4B-4E06-4ce1-9088-866CC96433FE}
static const GUID EH264VE_vcsd_mode =
{0xcbfc3e4b, 0x4e06, 0x4ce1, {0x90, 0x88, 0x86, 0x6c, 0xc9, 0x64, 0x33, 0xfe}};


// {F630A138-8661-4e3a-BF03-955F973AB110}
static const GUID EH264VE_quant_pI =
{0xf630a138, 0x8661, 0x4e3a, {0xbf, 0x3, 0x95, 0x5f, 0x97, 0x3a, 0xb1, 0x10}};

// {10E2FDCC-1FB5-4cbc-BB5E-75567AB39F69}
static const GUID EH264VE_quant_pP =
{0x10e2fdcc, 0x1fb5, 0x4cbc, {0xbb, 0x5e, 0x75, 0x56, 0x7a, 0xb3, 0x9f, 0x69}};

// {901DA7FD-F639-493f-8AD0-9492AEA72E1F}
static const GUID EH264VE_quant_pB =
{0x901da7fd, 0xf639, 0x493f, {0x8a, 0xd0, 0x94, 0x92, 0xae, 0xa7, 0x2e, 0x1f}};

// {522E3FE7-BED0-4bfa-B085-5ECE0B692CD0}
static const GUID EH264VE_quant_min =
{0x522e3fe7, 0xbed0, 0x4bfa, {0xb0, 0x85, 0x5e, 0xce, 0xb, 0x69, 0x2c, 0xd0}};

// {8D9B7175-1EB3-4f8d-A290-9415AC5BCBED}
static const GUID EH264VE_quant_max =
{0x8d9b7175, 0x1eb3, 0x4f8d, {0xa2, 0x90, 0x94, 0x15, 0xac, 0x5b, 0xcb, 0xed}};

// {7EAD13ED-47B2-4030-8C14-1BAB66A02BA3}
static const GUID EH264VE_cb_offset =
{0x7ead13ed, 0x47b2, 0x4030, {0x8c, 0x14, 0x1b, 0xab, 0x66, 0xa0, 0x2b, 0xa3}};

// {B4B009D0-E793-43e9-95A5-6D5D5C86E093}
static const GUID EH264VE_cr_offset =
{0xb4b009d0, 0xe793, 0x43e9, {0x95, 0xa5, 0x6d, 0x5d, 0x5c, 0x86, 0xe0, 0x93}};

// {76B17FDA-A1DA-45a3-B50E-4A2A2B35D108}
static const GUID EH264VE_bit_rate_buffer_size =
{0x76b17fda, 0xa1da, 0x45a3, {0xb5, 0xe, 0x4a, 0x2a, 0x2b, 0x35, 0xd1, 0x8}};

// {770776A5-76AF-4aa7-A7DA-CCB700945ABE}
static const GUID EH264VE_vbv_buffer_fullness =
{0x770776a5, 0x76af, 0x4aa7, {0xa7, 0xda, 0xcc, 0xb7, 0x0, 0x94, 0x5a, 0xbe}};

// {B419761A-839F-4df7-85F8-E0FB2CC5D81F}
static const GUID EH264VE_vbv_buffer_fullness_trg =
{0xb419761a, 0x839f, 0x4df7, {0x85, 0xf8, 0xe0, 0xfb, 0x2c, 0xc5, 0xd8, 0x1f}};



// {24A9A884-C4A0-4b5c-ACB3-212B3DBD7E04}
static const GUID EH264VE_search_range =
{0x24a9a884, 0xc4a0, 0x4b5c, {0xac, 0xb3, 0x21, 0x2b, 0x3d, 0xbd, 0x7e, 0x4}};

// {79686095-968E-4ce8-9247-6633F6F8DA3C}
static const GUID EH264VE_num_reference_frames =
{0x79686095, 0x968e, 0x4ce8, {0x92, 0x47, 0x66, 0x33, 0xf6, 0xf8, 0xda, 0x3c}};

// {8BC69F4F-6BE5-4fd7-90AE-D58DB21851A3}
static const GUID EH264VE_inter_search_shape =
{0x8bc69f4f, 0x6be5, 0x4fd7, {0x90, 0xae, 0xd5, 0x8d, 0xb2, 0x18, 0x51, 0xa3}};

// {8A5932D3-8860-454a-8A5B-4AE914AFE6E5}
static const GUID EH264VE_me_subpel_mode =
{0x8a5932d3, 0x8860, 0x454a, {0x8a, 0x5b, 0x4a, 0xe9, 0x14, 0xaf, 0xe6, 0xe5}};

// {FED84B7F-2006-4c6d-A49B-F1C0FE6A4F6F}
static const GUID EH264VE_me_weighted_p_mode =
{0xfed84b7f, 0x2006, 0x4c6d, {0xa4, 0x9b, 0xf1, 0xc0, 0xfe, 0x6a, 0x4f, 0x6f}};

// {52FD80EF-B814-4696-AB6A-F0BF188EAC1E}
static const GUID EH264VE_fast_multi_ref_me =
{0x52fd80ef, 0xb814, 0x4696, {0xab, 0x6a, 0xf0, 0xbf, 0x18, 0x8e, 0xac, 0x1e}};

// {62AEEF4F-C52A-47c2-8F73-1CE7AF1B84E0}
static const GUID EH264VE_fast_sub_block_me =
{0x62aeef4f, 0xc52a, 0x47c2, {0x8f, 0x73, 0x1c, 0xe7, 0xaf, 0x1b, 0x84, 0xe0}};

// {3FCFB0A8-92B0-4273-AB86-55F8918AA64B}
static const GUID EH264VE_allow_out_of_pic_mvs =
{0x3fcfb0a8, 0x92b0, 0x4273, {0xab, 0x86, 0x55, 0xf8, 0x91, 0x8a, 0xa6, 0x4b}};


// {DE932BFB-3D4B-4b27-B81C-371560C23541}
static const GUID EH264VE_enable_intra_big =
{0xde932bfb, 0x3d4b, 0x4b27, {0xb8, 0x1c, 0x37, 0x15, 0x60, 0xc2, 0x35, 0x41}};

// {446D663D-161F-4c3a-8A0B-72BA3096FB2A}
static const GUID EH264VE_enable_intra_8x8 =
{0x446d663d, 0x161f, 0x4c3a, {0x8a, 0xb, 0x72, 0xba, 0x30, 0x96, 0xfb, 0x2a}};

// {0A4AAC0E-F7E0-4060-8C57-C290E7383E5E}
static const GUID EH264VE_enable_intra_4x4 =
{0xa4aac0e, 0xf7e0, 0x4060, {0x8c, 0x57, 0xc2, 0x90, 0xe7, 0x38, 0x3e, 0x5e}};

// {055AF531-B06A-4927-B029-1472067DDDFF}
static const GUID EH264VE_enable_intra_pcm =
{0x55af531, 0xb06a, 0x4927, {0xb0, 0x29, 0x14, 0x72, 0x6, 0x7d, 0xdd, 0xff}};

// {FDCF749C-64F8-4e43-9299-6A3ED103F9FF}
static const GUID EH264VE_enable_inter_big =
{0xfdcf749c, 0x64f8, 0x4e43, {0x92, 0x99, 0x6a, 0x3e, 0xd1, 0x3, 0xf9, 0xff}};

// {412E3E96-0C95-4ec4-83A8-1FA4D82CE155}
static const GUID EH264VE_enable_inter_8x8 =
{0x412e3e96, 0xc95, 0x4ec4, {0x83, 0xa8, 0x1f, 0xa4, 0xd8, 0x2c, 0xe1, 0x55}};

// {28F1BC58-A32E-4bfd-855E-CA460634E44E}
static const GUID EH264VE_enable_inter_4x4 =
{0x28f1bc58, 0xa32e, 0x4bfd, {0x85, 0x5e, 0xca, 0x46, 0x6, 0x34, 0xe4, 0x4e}};

// {AED18519-8B8B-4676-88C3-1E6A136F767D}
static const GUID EH264VE_enable_inter_pcm =
{0xaed18519, 0x8b8b, 0x4676, {0x88, 0xc3, 0x1e, 0x6a, 0x13, 0x6f, 0x76, 0x7d}};



// {5FB3CC3C-67D6-42f6-AA9C-6AC05093661E}
static const GUID EH264VE_use_deblocking_filter =
{0x5fb3cc3c, 0x67d6, 0x42f6, {0xaa, 0x9c, 0x6a, 0xc0, 0x50, 0x93, 0x66, 0x1e}};

// {37882133-CAF5-4373-8F52-BA30068A704A}
static const GUID EH264VE_deblocking_alphaC0_offset =
{0x37882133, 0xcaf5, 0x4373, {0x8f, 0x52, 0xba, 0x30, 0x6, 0x8a, 0x70, 0x4a}};

// {F3049DC2-D2AD-484a-B2B9-4208051AFCB2}
static const GUID EH264VE_deblocking_beta_offset =
{0xf3049dc2, 0xd2ad, 0x484a, {0xb2, 0xb9, 0x42, 0x8, 0x5, 0x1a, 0xfc, 0xb2}};



// {39178668-4FF1-4991-A76B-1B6D54F709B2}
static const GUID EH264VE_entropy_coding_mode =
{0x39178668, 0x4ff1, 0x4991, {0xa7, 0x6b, 0x1b, 0x6d, 0x54, 0xf7, 0x9, 0xb2}};

// {9E63E1BB-08E3-42be-87CE-05D2F770361E}
static const GUID EH264VE_rd_optimization =
{0x9e63e1bb, 0x8e3, 0x42be, {0x87, 0xce, 0x5, 0xd2, 0xf7, 0x70, 0x36, 0x1e}};

// {48698A0E-D675-46b6-86EF-3D4E2C3ECAC3}
static const GUID EH264VE_fast_rd_optimization =
{0x48698a0e, 0xd675, 0x46b6, {0x86, 0xef, 0x3d, 0x4e, 0x2c, 0x3e, 0xca, 0xc3}};

// {29D87C71-49A4-4f67-B90D-BA821B58FE22}
static const GUID EH264VE_use_hadamard_transform =
{0x29d87c71, 0x49a4, 0x4f67, {0xb9, 0xd, 0xba, 0x82, 0x1b, 0x58, 0xfe, 0x22}};

// {2FE6D1AB-E54C-4afe-9566-44EEFBDFE321}
static const GUID EH264VE_enable_fast_inter_decisions =
{0x2fe6d1ab, 0xe54c, 0x4afe, {0x95, 0x66, 0x44, 0xee, 0xfb, 0xdf, 0xe3, 0x21}};

// {51BB55A6-D4AA-4d3a-AED4-D3DC2C61851C}
static const GUID EH264VE_enable_fast_intra_decisions =
{0x51bb55a6, 0xd4aa, 0x4d3a, {0xae, 0xd4, 0xd3, 0xdc, 0x2c, 0x61, 0x85, 0x1c}};

// {BBE49AA7-8ABA-4fd6-804D-00D129DCC458}
static const GUID EH264VE_quant_mode =
{0xbbe49aa7, 0x8aba, 0x4fd6, {0x80, 0x4d, 0x0, 0xd1, 0x29, 0xdc, 0xc4, 0x58}};

// {11716019-B4C8-4947-A1CF-49D373CDBEAE}
static const GUID EH264VE_fgo_strength =
{0x11716019, 0xb4c8, 0x4947, {0xa1, 0xcf, 0x49, 0xd3, 0x73, 0xcd, 0xbe, 0xae}};


// {E3ABEECF-D068-41bc-8FC2-A99A5EAA08C1}
static const GUID EH264VE_use_sample_aspect_ratio =
{0xe3abeecf, 0xd068, 0x41bc, {0x8f, 0xc2, 0xa9, 0x9a, 0x5e, 0xaa, 0x8, 0xc1}};

// {B80A21B6-5DE1-4676-9D3C-2F9B5D074444}
static const GUID EH264VE_auto_aspect_ratio =
{0xb80a21b6, 0x5de1, 0x4676, {0x9d, 0x3c, 0x2f, 0x9b, 0x5d, 0x7, 0x44, 0x44}};

// {93055399-DC58-4917-B206-88976B7BADEA}
static const GUID EH264VE_aspect_ratioX =
{0x93055399, 0xdc58, 0x4917, {0xb2, 0x6, 0x88, 0x97, 0x6b, 0x7b, 0xad, 0xea}};

// {4B844BD8-5B63-44df-9FF7-EE544AC19D85}
static const GUID EH264VE_aspect_ratioY =
{0x4b844bd8, 0x5b63, 0x44df, {0x9f, 0xf7, 0xee, 0x54, 0x4a, 0xc1, 0x9d, 0x85}};

// {598C7279-6AE2-4b3e-947C-6DA0C69F3A01}
static const GUID EH264VE_auto_sample_aspect_ratio =
{0x598c7279, 0x6ae2, 0x4b3e, {0x94, 0x7c, 0x6d, 0xa0, 0xc6, 0x9f, 0x3a, 0x1}};

// {80B714D9-EDCD-4cd5-AED7-20FC376B1DA5}
static const GUID EH264VE_sample_aspect_ratioX =
{0x80b714d9, 0xedcd, 0x4cd5, {0xae, 0xd7, 0x20, 0xfc, 0x37, 0x6b, 0x1d, 0xa5}};

// {603B2F96-C82F-4610-ADE3-4F2BA9C7BD33}
static const GUID EH264VE_sample_aspect_ratioY =
{0x603b2f96, 0xc82f, 0x4610, {0xad, 0xe3, 0x4f, 0x2b, 0xa9, 0xc7, 0xbd, 0x33}};



// {A541970F-4E41-4c81-9CC7-0B2BE385691C}
static const GUID EH264VE_stream_type =
{0xa541970f, 0x4e41, 0x4c81, {0x9c, 0xc7, 0xb, 0x2b, 0xe3, 0x85, 0x69, 0x1c}};

// {4E9B99A5-A388-452a-9B2D-C26B1230B210}
static const GUID EH264VE_video_full_range =
{0x4e9b99a5, 0xa388, 0x452a, {0x9b, 0x2d, 0xc2, 0x6b, 0x12, 0x30, 0xb2, 0x10}};

// {4586F612-A991-4010-8414-14763B3527F4}
static const GUID EH264VE_write_au_delimiters =
{0x4586f612, 0xa991, 0x4010, {0x84, 0x14, 0x14, 0x76, 0x3b, 0x35, 0x27, 0xf4}};

// {10AD7FE6-D3ED-4238-803B-630DBF7E8B0A}
static const GUID EH264VE_write_seq_end_code =
{0x10ad7fe6, 0xd3ed, 0x4238, {0x80, 0x3b, 0x63, 0xd, 0xbf, 0x7e, 0x8b, 0xa}};

// {C8CA406B-8220-4070-B5BE-1E27E4079147}
static const GUID EH264VE_write_timestamps =
{0xc8ca406b, 0x8220, 0x4070, {0xb5, 0xbe, 0x1e, 0x27, 0xe4, 0x7, 0x91, 0x47}};

// {5E431140-B9E4-424f-A90F-F067EBFEC28F}
static const GUID EH264VE_cpu_optimization =
{0x5e431140, 0xb9e4, 0x424f, {0xa9, 0xf, 0xf0, 0x67, 0xeb, 0xfe, 0xc2, 0x8f}};

// {39108716-B2F5-491c-9E9B-C2ABF38475B4}
static const GUID EH264VE_num_threads =
{0x39108716, 0xb2f5, 0x491c, {0x9e, 0x9b, 0xc2, 0xab, 0xf3, 0x84, 0x75, 0xb4}};


// {ACAC1419-D3D0-47ff-AC9F-889B1D143981}
static const GUID EH264VE_FixedRate =
{0xacac1419, 0xd3d0, 0x47ff, {0xac, 0x9f, 0x88, 0x9b, 0x1d, 0x14, 0x39, 0x81}};

// {3F3F4E8F-07BA-4a0d-86B7-FA79907B373E}
static const GUID EH264VE_Framerate =
{0x3f3f4e8f, 0x7ba, 0x4a0d, {0x86, 0xb7, 0xfa, 0x79, 0x90, 0x7b, 0x37, 0x3e}};

// {0CD0C8F4-2AE6-4c2c-A8A7-99967F2E12F2}
static const GUID EH264VE_OutFixedRate =
{0xcd0c8f4, 0x2ae6, 0x4c2c, {0xa8, 0xa7, 0x99, 0x96, 0x7f, 0x2e, 0x12, 0xf2}};

// {8F8A8859-A4C2-46c5-8043-18DA54F7034E}
static const GUID EH264VE_OutFramerate =
{0x8f8a8859, 0xa4c2, 0x46c5, {0x80, 0x43, 0x18, 0xda, 0x54, 0xf7, 0x3, 0x4e}};

// {838A8859-A4C2-46c5-8043-38DA54F00355}
static const GUID EH264VE_FpsConvType =
{0x838a8859, 0xa4c2, 0x46c5, {0x80, 0x43, 0x38, 0xda, 0x54, 0xf0, 0x3, 0x55}};

// {0670ACD2-5113-0002-9FF7-EE544AC19D85}
static const GUID EH264VE_TimestampMode =
{0x0670acd2, 0x5113, 0x0002, { 0x9f, 0xf7, 0xee, 0x54, 0x4a, 0xc1, 0x9d, 0x85}};

// {28C447DD-6581-4324-8A95-A62DF867379D}
static const GUID EH264VE_OutputMediatype =
{0x28c447dd, 0x6581, 0x4324, { 0x8a, 0x95, 0xa6, 0x2d, 0xf8, 0x67, 0x37, 0x9d}};


// {B1E3E1BF-6225-4351-862C-66989AEE22B4}
static const GUID EH264VE_SetDefValues =
{0xb1e3e1bf, 0x6225, 0x4351, {0x86, 0x2c, 0x66, 0x98, 0x9a, 0xee, 0x22, 0xb4}};


// {D02685C0-0946-4740-A6E7-504D750FE4F1}
static const GUID EH264VE_OriginalFramerate =
{0xd02685c0, 0x946, 0x4740, {0xa6, 0xe7, 0x50, 0x4d, 0x75, 0xf, 0xe4, 0xf1}};

// {654207C6-ECAD-4e8f-86F1-7819148D518A}
static const GUID EH264VE_OriginalInterlace =
{0x654207c6, 0xecad, 0x4e8f, {0x86, 0xf1, 0x78, 0x19, 0x14, 0x8d, 0x51, 0x8a}};

// {413AA8BB-F85C-4768-B8F6-91AA7BCE3DE6}
static const GUID EH264VE_OriginalColorSpace =
{0x413aa8bb, 0xf85c, 0x4768, {0xb8, 0xf6, 0x91, 0xaa, 0x7b, 0xce, 0x3d, 0xe6}};

// {413AA8BB-F85C-4768-B8F6-91AA7BCE3DE7}
static const GUID EH264VE_OriginalBitDepth =
{0x413aa8bb, 0xf85c, 0x4768, {0xb8, 0xf6, 0x91, 0xaa, 0x7b, 0xce, 0x3d, 0xe7}};

// {D0D2A38C-8DF6-47b5-A662-47E6D39BCFA7}
static const GUID EH264VE_IsRunning =
{0xd0d2a38c, 0x8df6, 0x47b5, {0xa6, 0x62, 0x47, 0xe6, 0xd3, 0x9b, 0xcf, 0xa7}};

// {DE6D6008-1A49-428a-B2ED-E760FF32909E}
static const GUID EH264VE_def_horizontal_size =
{0xde6d6008, 0x1a49, 0x428a, {0xb2, 0xed, 0xe7, 0x60, 0xff, 0x32, 0x90, 0x9e}};

// {D44C9617-C618-4d57-A992-DB8101911B4A}
static const GUID EH264VE_def_vertical_size =
{0xd44c9617, 0xc618, 0x4d57, {0xa9, 0x92, 0xdb, 0x81, 0x1, 0x91, 0x1b, 0x4a}};

// {D307B417-C618-4d57-A992-DB8101911B4A}
static const GUID EH264VE_chapter_list =
{0xd307b417, 0xc618, 0x4d57, {0xa9, 0x92, 0xdb, 0x81, 0x1, 0x91, 0x1b, 0x4a}};

// {3BDEAC38-3761-4a0a-92BD-F65828139E2E}
static const GUID EH264VE_pic_order_present_flag =
{0x3bdeac38, 0x3761, 0x4a0a, {0x92, 0xbd, 0xf6, 0x58, 0x28, 0x13, 0x9e, 0x2e}};

// {35BEBA77-A591-43c4-A1AA-A7B63041157A}
static const GUID EH264VE_fixed_frame_rate =
{0x35beba77, 0xa591, 0x43c4, { 0xa1, 0xaa, 0xa7, 0xb6, 0x30, 0x41, 0x15, 0x7a}};

// {22A03E42-6DCC-4597-B788-E059B4D79338}
static const GUID EH264VE_frame_based_timing =
{0x22a03e42, 0x6dcc, 0x4597, { 0xb7, 0x88, 0xe0, 0x59, 0xb4, 0xd7, 0x93, 0x38}};

// {DDE74F7E-4260-47e1-B83C-BC9A52EB4E9D}
static const GUID EH264VE_hrd_param_writing =
{0xdde74f7e, 0x4260, 0x47e1, { 0xb8, 0x3c, 0xbc, 0x9a, 0x52, 0xeb, 0x4e, 0x9d}};

// {2ED5EEA4-8E70-449c-9407-F1D232D32320}
static const GUID EH264VE_write_pic_struct =
{0x2ed5eea4, 0x8e70, 0x449c, { 0x94, 0x07, 0xf1, 0xd2, 0x32, 0xd3, 0x23, 0x20}};

// {79078EA9-167B-46e6-8DCC-6ED492B9271D}
static const GUID EH264VE_write_pic_timing_sei =
{0x79078ea9, 0x167b, 0x46e6, { 0x8d, 0xcc, 0x6e, 0xd4, 0x92, 0xb9, 0x27, 0x1d}};

// {00078EA9-167B-46e6-8DCC-6ED492B9271D}
static const GUID EH264VE_log2_max_frame_num =
{0x00078ea9, 0x167b, 0x46e6, { 0x8d, 0xcc, 0x6e, 0xd4, 0x92, 0xb9, 0x27, 0x1d}};

// {00178EA9-167B-46e6-8DCC-6ED492B9271D}
static const GUID EH264VE_pic_order_cnt_type =
{0x00178ea9, 0x167b, 0x46e6, { 0x8d, 0xcc, 0x6e, 0xd4, 0x92, 0xb9, 0x27, 0x1d}};


// {D92A3497-97A0-41ac-9F3C-42D5B238D500}
static const GUID EH264VE_aq_strength_luminance =
{0xd92a3497, 0x97a0, 0x41ac, {0x9f, 0x3c, 0x42, 0xd5, 0xb2, 0x38, 0xd5, 0x00}};

// {D92A3497-97A0-41ac-9F3C-42D5B238D501}
static const GUID EH264VE_aq_strength_contrast =
{0xd92a3497, 0x97a0, 0x41ac, {0x9f, 0x3c, 0x42, 0xd5, 0xb2, 0x38, 0xd5, 0x01}};

// {D92A3497-97A0-41ac-9F3C-42D5B238D502}
static const GUID EH264VE_aq_strength_complexity =
{0xd92a3497, 0x97a0, 0x41ac, {0x9f, 0x3c, 0x42, 0xd5, 0xb2, 0x38, 0xd5, 0x02}};


// {BAD62D94-3737-48e8-85CC-226FBE9BB894}
static const GUID EH264VE_chroma_sampling =
{0xbad62d94, 0x3737, 0x48e8, {0x85, 0xcc, 0x22, 0x6f, 0xbe, 0x9b, 0xb8, 0x94}};

// {87136EBC-2ABA-47f3-B82C-003F0D5ED074}
static const GUID EH264VE_luma_bit_depth =
{0x87136ebc, 0x2aba, 0x47f3, {0xb8, 0x2c, 0x0, 0x3f, 0xd, 0x5e, 0xd0, 0x74}};

// {87136EBC-2ABA-47f3-B82C-003F0D5ED075}
static const GUID EH264VE_chroma_bit_depth =
{0x87136ebc, 0x2aba, 0x47f3, {0xb8, 0x2c, 0x0, 0x3f, 0xd, 0x5e, 0xd0, 0x75}};

// {1c39c6cb-6fec-48fb-aff6-f8439ce6e60b}
static const GUID EH264VE_use_hrd =
{0x1c39c6cb, 0x6fec, 0x48fb, {0xaf, 0xf6, 0xf8, 0x43, 0x9c, 0xe6, 0xe6, 0xb}};

// {a27f3c3a-853e-4c03-a9a3-a55f47e82956}
static const GUID EH264VE_intra_refresh_mode =
{0xa27f3c3a, 0x853e, 0x4c03, {0xa9, 0xa3, 0xa5, 0x5f, 0x47, 0xe8, 0x29, 0x56}};

// {40513cf3-853e-4c03-a9a3-a55f47e82956}
static const GUID EH264VE_constr_intra_pred =
{0x40513cf3, 0x853e, 0x4c03, {0xa9, 0xa3, 0xa5, 0x5f, 0x47, 0xe8, 0x29, 0x56}};

// {11093431-74c1-4d37-beb8-3a4dc9bb0119}
static const GUID EH264VE_performance_preset =
{0x11093431, 0x74c1, 0x4d37, {0xbe, 0xb8, 0x3a, 0x4d, 0xc9, 0xbb, 0x1, 0x19}};

#endif // __PROPID_H264ENC_HEADER__

/********************************************************************
 Created: 2003/12/29 
 File name: enc_avc_guid.h
 Purpose: GUIDs for AVC/H.264 Encoder DS Filter

 Copyright (c) 2003-2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __AVCENC_GUIDS_H__
#define __AVCENC_GUIDS_H__ 

////////////////////////////////////////////////////////////

#undef  H264VE_HIDEICON

////////////////////////////////////////////////////////////

// {F9D05B58-A2D6-431a-9252-1F05EA2F3364}
static const GUID CLSID_H264VideoEncoder = 
{0xf9d05b58, 0xa2d6, 0x431a, {0x92, 0x52, 0x1f, 0x5, 0xea, 0x2f, 0x33, 0x64}};

// {9809C303-2BE5-4f4f-A0AF-7164C3E54867}
static const GUID CLSID_H264VideoEncAboutPropertyPage = 
{0x9809c303, 0x2be5, 0x4f4f, {0xa0, 0xaf, 0x71, 0x64, 0xc3, 0xe5, 0x48, 0x67}};

// {6975C63C-5D4F-4bb8-833D-E43ED16D1A9D}
static const GUID CLSID_H264VideoEncMainPropertyPage = 
{0x6975c63c, 0x5d4f, 0x4bb8, {0x83, 0x3d, 0xe4, 0x3e, 0xd1, 0x6d, 0x1a, 0x9d}};

// {10EBA2A7-4773-428e-9F29-3FAB2AB70B6F}
static const GUID CLSID_H264VideoEncAdvancedPropertyPage = 
{0x10eba2a7, 0x4773, 0x428e, {0x9f, 0x29, 0x3f, 0xab, 0x2a, 0xb7, 0xb, 0x6f}};

////////////////////////////////////////////////////////////

#endif // __AVCENC_GUIDS_H__ 

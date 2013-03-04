/********************************************************************
 Created: 2006/11/15 
 File name: mux_mp4_guid.h
 Purpose: GUIDs for MP4 Muxer DS Filter

 Copyright (c) 2005-2008 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __MP4MUX_GUID_H__
#define __MP4MUX_GUID_H__


////////////////////////////////////////////////////////////

#undef  COMPANY_HIDEICON

#define MP4MUXER_FILTER_MERIT 	MERIT_DO_NOT_USE

///////////////////////////////////////////////////////////////
#ifndef RC_INVOKED // don't let resource compiler see this part

// {0755FD77-C79A-4ed3-9E2B-8F90B1496510}
static const GUID CLSID_EMP4Mux = {0x755fd77, 0xc79a, 0x4ed3, {0x9e, 0x2b, 0x8f, 0x90, 0xb1, 0x49, 0x65, 0x10}};

// {D05ACB88-ECBA-4fad-9741-4467EAF621CA}
static const GUID CLSID_EMP4MuxPropertyPage= {0xd05acb88, 0xecba, 0x4fad, {0x97, 0x41, 0x44, 0x67, 0xea, 0xf6, 0x21, 0xca}};

// {049FEFAE-399E-4316-BC1C-1A5860BF9FFC}
static const GUID CLSID_EMP4MuxAboutPage = {0x49fefae, 0x399e, 0x4316, {0xbc, 0x1c, 0x1a, 0x58, 0x60, 0xbf, 0x9f, 0xfc}};

// {F8898AAF-0B6C-4fd3-9FD6-1FD080810F5E}
static const GUID CLSID_EMP4MuxAdvancedPropertyPage = { 0xf8898aaf, 0xb6c, 0x4fd3, {0x9f, 0xd6, 0x1f, 0xd0, 0x80, 0x81, 0xf, 0x5e}};

#endif // RC_INVOKED 
///////////////////////////////////////////////////////////////

#endif __MP4MUX_GUID_H__

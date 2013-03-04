/********************************************************************
 Created: 2004/11/03 
 File name: enc_aac_guid.h
 Purpose: GUIDs for AAC Encoder DS Filter

 Copyright (c) 2004-2008 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __AACENC_GUID_H__
#define __AACENC_GUID_H__


////////////////////////////////////////////////////////////

#undef  COMPANY_HIDEICON

#define COMPANY_DS_MERIT_AACENCODER (MERIT_SW_COMPRESSOR)


///////////////////////////////////////////////////////////////
#ifndef RC_INVOKED // don't let resource compiler see this part

// {34C7AD29-A6FD-48E1-9AEF-E7A7397912B7}
DEFINE_GUID(CLSID_AACEncoder,
0x34c7ad29, 0xa6fd, 0x48e1, 0x9a, 0xef, 0xe7, 0xa7, 0x39, 0x79, 0x12, 0xb7);

// {0F275E33-35AF-400A-A736-55BC95C66547}
DEFINE_GUID(CLSID_AACEncoderPropPageSettings,
0x0f275e33, 0x35af, 0x400a, 0xa7, 0x36, 0x55, 0xbc, 0x95, 0xc6, 0x65, 0x47);

// {903AB7C3-E13C-47E6-85EC-38076E49DE52}
DEFINE_GUID(CLSID_AACEncoderPropPageAbout,
0x903ab7c3, 0xe13c, 0x47e6, 0x85, 0xec, 0x38, 0x7, 0x6e, 0x49, 0xde, 0x52);

#endif // RC_INVOKED
////////////////////////////////////////////////////

#endif //__AACENC_GUID_H__



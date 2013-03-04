/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Jan 20 10:46:50 2006
 */
/* Compiler settings for D:\mm\dshowflt\AAC\EncoderDS\ILEncAAC.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __ILEncAAC_h__
#define __ILEncAAC_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ILMAACEncoder_FWD_DEFINED__
#define __ILMAACEncoder_FWD_DEFINED__
typedef interface ILMAACEncoder ILMAACEncoder;
#endif 	/* __ILMAACEncoder_FWD_DEFINED__ */


#ifndef __LMAACEncoder_FWD_DEFINED__
#define __LMAACEncoder_FWD_DEFINED__

#ifdef __cplusplus
typedef class LMAACEncoder LMAACEncoder;
#else
typedef struct LMAACEncoder LMAACEncoder;
#endif /* __cplusplus */

#endif 	/* __LMAACEncoder_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __LMAACEncoderLib_LIBRARY_DEFINED__
#define __LMAACEncoderLib_LIBRARY_DEFINED__

/* library LMAACEncoderLib */
/* [helpstring][version][uuid] */ 

#ifndef __ILMAACENCODER_H__
#define __ILMAACENCODER_H__
static const GUID CLSID_LMAACEncoder              = {0xe2b7dd70, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9}};
static const GUID IID_ILMAACEncoder               = {0xe2b7dd74, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9}};
static const GUID CLSID_LMAACEncoderPropertyPage	 = {0xe2b7dd72, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9}};
static const GUID CLSID_LMAACEncoderAboutPage     = {0xe2b7dd73, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9}};
static const GUID LIBID_LMAACEncoderLib	          = {0xe2b7dd71, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9}};
#endif 
typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_ILEncAAC_0000_0001
    {	OutputChannel_UNCHANGED	= 0,
	OutputChannel_MONO	= 1,
	OutputChannel_STEREO	= 2,
	OutputChannel_6CHANNELS	= 3
    }	eOutputChannel;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_ILEncAAC_0000_0002
    {	ObjectType_LOW	= 0,
	ObjectType_MAIN	= 1,
	ObjectType_LTP	= 3
    }	eObjectType;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_ILEncAAC_0000_0003
    {	MPEGVersion_MPEG4	= 0,
	MPEGVersion_MPEG2	= 1
    }	eMPEGVersion;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_ILEncAAC_0000_0004
    {	BlockType_NORMAL	= 0,
	BlockType_NOSHORT	= 1,
	BlockType_NOLONG	= 2
    }	eBlockType;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_ILEncAAC_0000_0005
    {	CONSTANT_QUALITY	= 0,
	CONSTANT_BITRATE	= 1
    }	eRateControl;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_ILEncAAC_0000_0006
    {	STREAMFORMAT_RAW	= 0,
	STREAMFORMAT_ADTS	= 1
    }	eStreamFormat;


EXTERN_C const IID LIBID_LMAACEncoderLib;

#ifndef __ILMAACEncoder_INTERFACE_DEFINED__
#define __ILMAACEncoder_INTERFACE_DEFINED__

/* interface ILMAACEncoder */
/* [unique][helpstring][uuid][local][object] */ 


EXTERN_C const IID IID_ILMAACEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E2B7DD74-38C5-11D5-91F6-00104BDB8FF9")
    ILMAACEncoder : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OutputBitrate( 
            /* [retval][out] */ long __RPC_FAR *pOutputBitrate) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OutputBitrate( 
            /* [in] */ long lOutputBitrate) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OutputChannels( 
            /* [retval][out] */ eOutputChannel __RPC_FAR *pOutputChannelFlag) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OutputChannels( 
            /* [in] */ eOutputChannel lOutputChannelFlag) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SimulateSurround( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSimulateSurround) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SimulateSurround( 
            /* [in] */ VARIANT_BOOL bSimulateSurround) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MPEGVersion( 
            /* [retval][out] */ eMPEGVersion __RPC_FAR *pMPEGVersion) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MPEGVersion( 
            /* [in] */ eMPEGVersion lMPEGVersion) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BlockType( 
            /* [retval][out] */ eBlockType __RPC_FAR *pBlockType) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_BlockType( 
            /* [in] */ eBlockType lBlockType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ObjectType( 
            /* [retval][out] */ eObjectType __RPC_FAR *pObjectType) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ObjectType( 
            /* [in] */ eObjectType lObjectType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_QFactor( 
            /* [retval][out] */ long __RPC_FAR *pQFactor) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_QFactor( 
            /* [in] */ long lQFactor) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RateControlMethod( 
            /* [retval][out] */ eRateControl __RPC_FAR *pRateControl) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RateControlMethod( 
            /* [in] */ eRateControl lRateControl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Cutoff( 
            /* [retval][out] */ long __RPC_FAR *pCutoff) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Cutoff( 
            /* [in] */ long lCutoff) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UseCutoff( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseCutoff) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UseCutoff( 
            /* [in] */ VARIANT_BOOL bUseCutoff) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StreamFormat( 
            /* [retval][out] */ eStreamFormat __RPC_FAR *pStreamFormat) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StreamFormat( 
            /* [in] */ eStreamFormat lStreamFormat) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OutputSampleRate( 
            /* [retval][out] */ long __RPC_FAR *plSampleRate) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OutputSampleRate( 
            /* [in] */ long lSampleRate) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILMAACEncoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILMAACEncoder __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILMAACEncoder __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OutputBitrate )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pOutputBitrate);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OutputBitrate )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ long lOutputBitrate);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OutputChannels )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ eOutputChannel __RPC_FAR *pOutputChannelFlag);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OutputChannels )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ eOutputChannel lOutputChannelFlag);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SimulateSurround )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSimulateSurround);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SimulateSurround )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bSimulateSurround);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MPEGVersion )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ eMPEGVersion __RPC_FAR *pMPEGVersion);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MPEGVersion )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ eMPEGVersion lMPEGVersion);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BlockType )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ eBlockType __RPC_FAR *pBlockType);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BlockType )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ eBlockType lBlockType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ObjectType )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ eObjectType __RPC_FAR *pObjectType);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ObjectType )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ eObjectType lObjectType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QFactor )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pQFactor);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_QFactor )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ long lQFactor);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RateControlMethod )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ eRateControl __RPC_FAR *pRateControl);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RateControlMethod )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ eRateControl lRateControl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Cutoff )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCutoff);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Cutoff )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ long lCutoff);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseCutoff )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseCutoff);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseCutoff )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bUseCutoff);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StreamFormat )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ eStreamFormat __RPC_FAR *pStreamFormat);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StreamFormat )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ eStreamFormat lStreamFormat);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OutputSampleRate )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plSampleRate);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OutputSampleRate )( 
            ILMAACEncoder __RPC_FAR * This,
            /* [in] */ long lSampleRate);
        
        END_INTERFACE
    } ILMAACEncoderVtbl;

    interface ILMAACEncoder
    {
        CONST_VTBL struct ILMAACEncoderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILMAACEncoder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILMAACEncoder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILMAACEncoder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILMAACEncoder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ILMAACEncoder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ILMAACEncoder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ILMAACEncoder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ILMAACEncoder_get_OutputBitrate(This,pOutputBitrate)	\
    (This)->lpVtbl -> get_OutputBitrate(This,pOutputBitrate)

#define ILMAACEncoder_put_OutputBitrate(This,lOutputBitrate)	\
    (This)->lpVtbl -> put_OutputBitrate(This,lOutputBitrate)

#define ILMAACEncoder_get_OutputChannels(This,pOutputChannelFlag)	\
    (This)->lpVtbl -> get_OutputChannels(This,pOutputChannelFlag)

#define ILMAACEncoder_put_OutputChannels(This,lOutputChannelFlag)	\
    (This)->lpVtbl -> put_OutputChannels(This,lOutputChannelFlag)

#define ILMAACEncoder_get_SimulateSurround(This,pSimulateSurround)	\
    (This)->lpVtbl -> get_SimulateSurround(This,pSimulateSurround)

#define ILMAACEncoder_put_SimulateSurround(This,bSimulateSurround)	\
    (This)->lpVtbl -> put_SimulateSurround(This,bSimulateSurround)

#define ILMAACEncoder_get_MPEGVersion(This,pMPEGVersion)	\
    (This)->lpVtbl -> get_MPEGVersion(This,pMPEGVersion)

#define ILMAACEncoder_put_MPEGVersion(This,lMPEGVersion)	\
    (This)->lpVtbl -> put_MPEGVersion(This,lMPEGVersion)

#define ILMAACEncoder_get_BlockType(This,pBlockType)	\
    (This)->lpVtbl -> get_BlockType(This,pBlockType)

#define ILMAACEncoder_put_BlockType(This,lBlockType)	\
    (This)->lpVtbl -> put_BlockType(This,lBlockType)

#define ILMAACEncoder_get_ObjectType(This,pObjectType)	\
    (This)->lpVtbl -> get_ObjectType(This,pObjectType)

#define ILMAACEncoder_put_ObjectType(This,lObjectType)	\
    (This)->lpVtbl -> put_ObjectType(This,lObjectType)

#define ILMAACEncoder_get_QFactor(This,pQFactor)	\
    (This)->lpVtbl -> get_QFactor(This,pQFactor)

#define ILMAACEncoder_put_QFactor(This,lQFactor)	\
    (This)->lpVtbl -> put_QFactor(This,lQFactor)

#define ILMAACEncoder_get_RateControlMethod(This,pRateControl)	\
    (This)->lpVtbl -> get_RateControlMethod(This,pRateControl)

#define ILMAACEncoder_put_RateControlMethod(This,lRateControl)	\
    (This)->lpVtbl -> put_RateControlMethod(This,lRateControl)

#define ILMAACEncoder_get_Cutoff(This,pCutoff)	\
    (This)->lpVtbl -> get_Cutoff(This,pCutoff)

#define ILMAACEncoder_put_Cutoff(This,lCutoff)	\
    (This)->lpVtbl -> put_Cutoff(This,lCutoff)

#define ILMAACEncoder_get_UseCutoff(This,pUseCutoff)	\
    (This)->lpVtbl -> get_UseCutoff(This,pUseCutoff)

#define ILMAACEncoder_put_UseCutoff(This,bUseCutoff)	\
    (This)->lpVtbl -> put_UseCutoff(This,bUseCutoff)

#define ILMAACEncoder_get_StreamFormat(This,pStreamFormat)	\
    (This)->lpVtbl -> get_StreamFormat(This,pStreamFormat)

#define ILMAACEncoder_put_StreamFormat(This,lStreamFormat)	\
    (This)->lpVtbl -> put_StreamFormat(This,lStreamFormat)

#define ILMAACEncoder_get_OutputSampleRate(This,plSampleRate)	\
    (This)->lpVtbl -> get_OutputSampleRate(This,plSampleRate)

#define ILMAACEncoder_put_OutputSampleRate(This,lSampleRate)	\
    (This)->lpVtbl -> put_OutputSampleRate(This,lSampleRate)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_OutputBitrate_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pOutputBitrate);


void __RPC_STUB ILMAACEncoder_get_OutputBitrate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_OutputBitrate_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ long lOutputBitrate);


void __RPC_STUB ILMAACEncoder_put_OutputBitrate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_OutputChannels_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ eOutputChannel __RPC_FAR *pOutputChannelFlag);


void __RPC_STUB ILMAACEncoder_get_OutputChannels_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_OutputChannels_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ eOutputChannel lOutputChannelFlag);


void __RPC_STUB ILMAACEncoder_put_OutputChannels_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_SimulateSurround_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pSimulateSurround);


void __RPC_STUB ILMAACEncoder_get_SimulateSurround_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_SimulateSurround_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bSimulateSurround);


void __RPC_STUB ILMAACEncoder_put_SimulateSurround_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_MPEGVersion_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ eMPEGVersion __RPC_FAR *pMPEGVersion);


void __RPC_STUB ILMAACEncoder_get_MPEGVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_MPEGVersion_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ eMPEGVersion lMPEGVersion);


void __RPC_STUB ILMAACEncoder_put_MPEGVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_BlockType_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ eBlockType __RPC_FAR *pBlockType);


void __RPC_STUB ILMAACEncoder_get_BlockType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_BlockType_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ eBlockType lBlockType);


void __RPC_STUB ILMAACEncoder_put_BlockType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_ObjectType_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ eObjectType __RPC_FAR *pObjectType);


void __RPC_STUB ILMAACEncoder_get_ObjectType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_ObjectType_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ eObjectType lObjectType);


void __RPC_STUB ILMAACEncoder_put_ObjectType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_QFactor_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pQFactor);


void __RPC_STUB ILMAACEncoder_get_QFactor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_QFactor_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ long lQFactor);


void __RPC_STUB ILMAACEncoder_put_QFactor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_RateControlMethod_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ eRateControl __RPC_FAR *pRateControl);


void __RPC_STUB ILMAACEncoder_get_RateControlMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_RateControlMethod_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ eRateControl lRateControl);


void __RPC_STUB ILMAACEncoder_put_RateControlMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_Cutoff_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCutoff);


void __RPC_STUB ILMAACEncoder_get_Cutoff_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_Cutoff_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ long lCutoff);


void __RPC_STUB ILMAACEncoder_put_Cutoff_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_UseCutoff_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pUseCutoff);


void __RPC_STUB ILMAACEncoder_get_UseCutoff_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_UseCutoff_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bUseCutoff);


void __RPC_STUB ILMAACEncoder_put_UseCutoff_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_StreamFormat_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ eStreamFormat __RPC_FAR *pStreamFormat);


void __RPC_STUB ILMAACEncoder_get_StreamFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_StreamFormat_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ eStreamFormat lStreamFormat);


void __RPC_STUB ILMAACEncoder_put_StreamFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_get_OutputSampleRate_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plSampleRate);


void __RPC_STUB ILMAACEncoder_get_OutputSampleRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMAACEncoder_put_OutputSampleRate_Proxy( 
    ILMAACEncoder __RPC_FAR * This,
    /* [in] */ long lSampleRate);


void __RPC_STUB ILMAACEncoder_put_OutputSampleRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILMAACEncoder_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_LMAACEncoder;

#ifdef __cplusplus

class DECLSPEC_UUID("E2B7DD70-38C5-11D5-91F6-00104BDB8FF9")
LMAACEncoder;
#endif
#endif /* __LMAACEncoderLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Aug 29 09:38:53 2006
 */
/* Compiler settings for F:\mm\DShowFlt\MPEG4\EncoderDS\ILEnc.idl:
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

#ifndef __ILEncMPEG4_h__
#define __ILEncMPEG4_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ILMMP4Encoder1_FWD_DEFINED__
#define __ILMMP4Encoder1_FWD_DEFINED__
typedef interface ILMMP4Encoder1 ILMMP4Encoder1;
#endif 	/* __ILMMP4Encoder1_FWD_DEFINED__ */


#ifndef __ILMMP4Encoder_FWD_DEFINED__
#define __ILMMP4Encoder_FWD_DEFINED__
typedef interface ILMMP4Encoder ILMMP4Encoder;
#endif 	/* __ILMMP4Encoder_FWD_DEFINED__ */


#ifndef __LMMP4Encoder_FWD_DEFINED__
#define __LMMP4Encoder_FWD_DEFINED__

#ifdef __cplusplus
typedef class LMMP4Encoder LMMP4Encoder;
#else
typedef struct LMMP4Encoder LMMP4Encoder;
#endif /* __cplusplus */

#endif 	/* __LMMP4Encoder_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __LMMP4EncoderLib_LIBRARY_DEFINED__
#define __LMMP4EncoderLib_LIBRARY_DEFINED__

/* library LMMP4EncoderLib */
/* [helpstring][version][uuid] */ 

#ifndef __ILMMP4ENCODER_H__
#define __ILMMP4ENCODER_H__
static const GUID CLSID_LMMP4Encoder                      = { 0xe2b7dd65, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9 }};
static const GUID CLSID_LMMP4EncoderPropertiy             = { 0xe2b7dd68, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9 }};
static const GUID CLSID_LMMP4EncoderAbout                 = { 0xe2b7dd69, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9 }};
static const GUID LIBID_LMMP4EncoderLib                   = { 0xe2b7dd67, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9 }};
static const GUID IID_ILMMP4Encoder1                      = { 0xe2b7dd66, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9 }};
static const GUID IID_ILMMP4Encoder                       = { 0xe2b7de33, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9 }};
#endif 
typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_ILEnc_0000_0001
    {	LMP4_MOTIONVECTOR_OnePerMacroblock	= 0,
	LMP4_MOTIONVECTOR_FourPerMacroblock	= 1
    }	eMotionVectorPerMacroblock;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_ILEnc_0000_0002
    {	LMP4_MOTIONALGORITHM_FullSearch	= 0,
	LMP4_MOTIONALGORITHM_FastSearch	= 1
    }	eMotionAlgorithm;

typedef /* [public][public][public][public][public] */ 
enum __MIDL___MIDL_itf_ILEnc_0000_0003
    {	LMP4_MOTIONACCURACY_FullPixel	= 1,
	LMP4_MOTIONACCURACY_HalfPixel	= 2
    }	eMotionAccuracy;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_ILEnc_0000_0004
    {	LMP4_QUANTTYPE_Linear	= 0,
	LMP4_QUANTTYPE_NonLinear	= 1
    }	eQuantType;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_ILEnc_0000_0005
    {	LMP4_OUTPUTFORMAT_mp4v	= 0,
	LMP4_OUTPUTFORMAT_LMP4	= 1,
	LMP4_OUTPUTFORMAT_XVID	= 2
    }	eOutputFormat;


EXTERN_C const IID LIBID_LMMP4EncoderLib;

#ifndef __ILMMP4Encoder1_INTERFACE_DEFINED__
#define __ILMMP4Encoder1_INTERFACE_DEFINED__

/* interface ILMMP4Encoder1 */
/* [unique][helpstring][uuid][local][object] */ 


EXTERN_C const IID IID_ILMMP4Encoder1;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E2B7DD66-38C5-11D5-91F6-00104BDB8FF9")
    ILMMP4Encoder1 : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OutputFrameRate( 
            /* [retval][out] */ double __RPC_FAR *pfFrameRate) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OutputFrameRate( 
            /* [in] */ double fFrameRate) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PFrames( 
            /* [retval][out] */ long __RPC_FAR *plPFrames) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PFrames( 
            /* [in] */ long lPFrames) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BFrames( 
            /* [retval][out] */ long __RPC_FAR *plBFrames) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_BFrames( 
            /* [in] */ long lBFrames) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SearchWidth( 
            /* [retval][out] */ long __RPC_FAR *plPSearchWidth) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SearchWidth( 
            /* [in] */ long lPSearchWidth) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SearchHeight( 
            /* [retval][out] */ long __RPC_FAR *plSearchHeight) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SearchHeight( 
            /* [in] */ long lSearchHeight) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_QFactor( 
            /* [retval][out] */ long __RPC_FAR *plQFactor) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_QFactor( 
            /* [in] */ long lQFactor) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_QuantType( 
            /* [retval][out] */ eQuantType __RPC_FAR *pQuantType) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_QuantType( 
            /* [in] */ eQuantType QuantType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MotionAlgorithm( 
            /* [retval][out] */ eMotionAlgorithm __RPC_FAR *pMotionAlgorithm) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MotionAlgorithm( 
            /* [in] */ eMotionAlgorithm MotionAlgorithm) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MotionAccurancy( 
            /* [retval][out] */ eMotionAccuracy __RPC_FAR *pMotionAccuracy) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MotionAccurancy( 
            /* [in] */ eMotionAccuracy MotionAccuracy) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MotionVectorPerMacroblock( 
            /* [retval][out] */ eMotionVectorPerMacroblock __RPC_FAR *plMotionVectorPerMacroblock) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MotionVectorPerMacroblock( 
            /* [in] */ eMotionVectorPerMacroblock lMotionVectorPerMacroblock) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteToRegistry( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OutputFormat( 
            /* [retval][out] */ eOutputFormat __RPC_FAR *pOutputFormat) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OutputFormat( 
            /* [in] */ eOutputFormat lOutputFormat) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SuperCompression( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbSuperCompression) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SuperCompression( 
            /* [in] */ VARIANT_BOOL bSuperCompression) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILMMP4Encoder1Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILMMP4Encoder1 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILMMP4Encoder1 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OutputFrameRate )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pfFrameRate);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OutputFrameRate )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ double fFrameRate);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PFrames )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPFrames);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PFrames )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ long lPFrames);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BFrames )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plBFrames);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BFrames )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ long lBFrames);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchWidth )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPSearchWidth);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SearchWidth )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ long lPSearchWidth);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchHeight )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plSearchHeight);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SearchHeight )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ long lSearchHeight);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QFactor )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plQFactor);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_QFactor )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ long lQFactor);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QuantType )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ eQuantType __RPC_FAR *pQuantType);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_QuantType )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ eQuantType QuantType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MotionAlgorithm )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ eMotionAlgorithm __RPC_FAR *pMotionAlgorithm);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MotionAlgorithm )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ eMotionAlgorithm MotionAlgorithm);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MotionAccurancy )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ eMotionAccuracy __RPC_FAR *pMotionAccuracy);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MotionAccurancy )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ eMotionAccuracy MotionAccuracy);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MotionVectorPerMacroblock )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ eMotionVectorPerMacroblock __RPC_FAR *plMotionVectorPerMacroblock);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MotionVectorPerMacroblock )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ eMotionVectorPerMacroblock lMotionVectorPerMacroblock);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteToRegistry )( 
            ILMMP4Encoder1 __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OutputFormat )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ eOutputFormat __RPC_FAR *pOutputFormat);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OutputFormat )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ eOutputFormat lOutputFormat);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SuperCompression )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbSuperCompression);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SuperCompression )( 
            ILMMP4Encoder1 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bSuperCompression);
        
        END_INTERFACE
    } ILMMP4Encoder1Vtbl;

    interface ILMMP4Encoder1
    {
        CONST_VTBL struct ILMMP4Encoder1Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILMMP4Encoder1_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILMMP4Encoder1_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILMMP4Encoder1_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILMMP4Encoder1_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ILMMP4Encoder1_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ILMMP4Encoder1_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ILMMP4Encoder1_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ILMMP4Encoder1_get_OutputFrameRate(This,pfFrameRate)	\
    (This)->lpVtbl -> get_OutputFrameRate(This,pfFrameRate)

#define ILMMP4Encoder1_put_OutputFrameRate(This,fFrameRate)	\
    (This)->lpVtbl -> put_OutputFrameRate(This,fFrameRate)

#define ILMMP4Encoder1_get_PFrames(This,plPFrames)	\
    (This)->lpVtbl -> get_PFrames(This,plPFrames)

#define ILMMP4Encoder1_put_PFrames(This,lPFrames)	\
    (This)->lpVtbl -> put_PFrames(This,lPFrames)

#define ILMMP4Encoder1_get_BFrames(This,plBFrames)	\
    (This)->lpVtbl -> get_BFrames(This,plBFrames)

#define ILMMP4Encoder1_put_BFrames(This,lBFrames)	\
    (This)->lpVtbl -> put_BFrames(This,lBFrames)

#define ILMMP4Encoder1_get_SearchWidth(This,plPSearchWidth)	\
    (This)->lpVtbl -> get_SearchWidth(This,plPSearchWidth)

#define ILMMP4Encoder1_put_SearchWidth(This,lPSearchWidth)	\
    (This)->lpVtbl -> put_SearchWidth(This,lPSearchWidth)

#define ILMMP4Encoder1_get_SearchHeight(This,plSearchHeight)	\
    (This)->lpVtbl -> get_SearchHeight(This,plSearchHeight)

#define ILMMP4Encoder1_put_SearchHeight(This,lSearchHeight)	\
    (This)->lpVtbl -> put_SearchHeight(This,lSearchHeight)

#define ILMMP4Encoder1_get_QFactor(This,plQFactor)	\
    (This)->lpVtbl -> get_QFactor(This,plQFactor)

#define ILMMP4Encoder1_put_QFactor(This,lQFactor)	\
    (This)->lpVtbl -> put_QFactor(This,lQFactor)

#define ILMMP4Encoder1_get_QuantType(This,pQuantType)	\
    (This)->lpVtbl -> get_QuantType(This,pQuantType)

#define ILMMP4Encoder1_put_QuantType(This,QuantType)	\
    (This)->lpVtbl -> put_QuantType(This,QuantType)

#define ILMMP4Encoder1_get_MotionAlgorithm(This,pMotionAlgorithm)	\
    (This)->lpVtbl -> get_MotionAlgorithm(This,pMotionAlgorithm)

#define ILMMP4Encoder1_put_MotionAlgorithm(This,MotionAlgorithm)	\
    (This)->lpVtbl -> put_MotionAlgorithm(This,MotionAlgorithm)

#define ILMMP4Encoder1_get_MotionAccurancy(This,pMotionAccuracy)	\
    (This)->lpVtbl -> get_MotionAccurancy(This,pMotionAccuracy)

#define ILMMP4Encoder1_put_MotionAccurancy(This,MotionAccuracy)	\
    (This)->lpVtbl -> put_MotionAccurancy(This,MotionAccuracy)

#define ILMMP4Encoder1_get_MotionVectorPerMacroblock(This,plMotionVectorPerMacroblock)	\
    (This)->lpVtbl -> get_MotionVectorPerMacroblock(This,plMotionVectorPerMacroblock)

#define ILMMP4Encoder1_put_MotionVectorPerMacroblock(This,lMotionVectorPerMacroblock)	\
    (This)->lpVtbl -> put_MotionVectorPerMacroblock(This,lMotionVectorPerMacroblock)

#define ILMMP4Encoder1_WriteToRegistry(This)	\
    (This)->lpVtbl -> WriteToRegistry(This)

#define ILMMP4Encoder1_get_OutputFormat(This,pOutputFormat)	\
    (This)->lpVtbl -> get_OutputFormat(This,pOutputFormat)

#define ILMMP4Encoder1_put_OutputFormat(This,lOutputFormat)	\
    (This)->lpVtbl -> put_OutputFormat(This,lOutputFormat)

#define ILMMP4Encoder1_get_SuperCompression(This,pbSuperCompression)	\
    (This)->lpVtbl -> get_SuperCompression(This,pbSuperCompression)

#define ILMMP4Encoder1_put_SuperCompression(This,bSuperCompression)	\
    (This)->lpVtbl -> put_SuperCompression(This,bSuperCompression)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_OutputFrameRate_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pfFrameRate);


void __RPC_STUB ILMMP4Encoder1_get_OutputFrameRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_OutputFrameRate_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ double fFrameRate);


void __RPC_STUB ILMMP4Encoder1_put_OutputFrameRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_PFrames_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPFrames);


void __RPC_STUB ILMMP4Encoder1_get_PFrames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_PFrames_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ long lPFrames);


void __RPC_STUB ILMMP4Encoder1_put_PFrames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_BFrames_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plBFrames);


void __RPC_STUB ILMMP4Encoder1_get_BFrames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_BFrames_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ long lBFrames);


void __RPC_STUB ILMMP4Encoder1_put_BFrames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_SearchWidth_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPSearchWidth);


void __RPC_STUB ILMMP4Encoder1_get_SearchWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_SearchWidth_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ long lPSearchWidth);


void __RPC_STUB ILMMP4Encoder1_put_SearchWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_SearchHeight_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plSearchHeight);


void __RPC_STUB ILMMP4Encoder1_get_SearchHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_SearchHeight_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ long lSearchHeight);


void __RPC_STUB ILMMP4Encoder1_put_SearchHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_QFactor_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plQFactor);


void __RPC_STUB ILMMP4Encoder1_get_QFactor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_QFactor_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ long lQFactor);


void __RPC_STUB ILMMP4Encoder1_put_QFactor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_QuantType_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ eQuantType __RPC_FAR *pQuantType);


void __RPC_STUB ILMMP4Encoder1_get_QuantType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_QuantType_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ eQuantType QuantType);


void __RPC_STUB ILMMP4Encoder1_put_QuantType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_MotionAlgorithm_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ eMotionAlgorithm __RPC_FAR *pMotionAlgorithm);


void __RPC_STUB ILMMP4Encoder1_get_MotionAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_MotionAlgorithm_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ eMotionAlgorithm MotionAlgorithm);


void __RPC_STUB ILMMP4Encoder1_put_MotionAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_MotionAccurancy_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ eMotionAccuracy __RPC_FAR *pMotionAccuracy);


void __RPC_STUB ILMMP4Encoder1_get_MotionAccurancy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_MotionAccurancy_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ eMotionAccuracy MotionAccuracy);


void __RPC_STUB ILMMP4Encoder1_put_MotionAccurancy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_MotionVectorPerMacroblock_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ eMotionVectorPerMacroblock __RPC_FAR *plMotionVectorPerMacroblock);


void __RPC_STUB ILMMP4Encoder1_get_MotionVectorPerMacroblock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_MotionVectorPerMacroblock_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ eMotionVectorPerMacroblock lMotionVectorPerMacroblock);


void __RPC_STUB ILMMP4Encoder1_put_MotionVectorPerMacroblock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_WriteToRegistry_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This);


void __RPC_STUB ILMMP4Encoder1_WriteToRegistry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_OutputFormat_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ eOutputFormat __RPC_FAR *pOutputFormat);


void __RPC_STUB ILMMP4Encoder1_get_OutputFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_OutputFormat_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ eOutputFormat lOutputFormat);


void __RPC_STUB ILMMP4Encoder1_put_OutputFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_get_SuperCompression_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbSuperCompression);


void __RPC_STUB ILMMP4Encoder1_get_SuperCompression_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder1_put_SuperCompression_Proxy( 
    ILMMP4Encoder1 __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bSuperCompression);


void __RPC_STUB ILMMP4Encoder1_put_SuperCompression_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILMMP4Encoder1_INTERFACE_DEFINED__ */


#ifndef __ILMMP4Encoder_INTERFACE_DEFINED__
#define __ILMMP4Encoder_INTERFACE_DEFINED__

/* interface ILMMP4Encoder */
/* [unique][helpstring][uuid][local][object] */ 


EXTERN_C const IID IID_ILMMP4Encoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E2B7DE33-38C5-11D5-91F6-00104BDB8FF9")
    ILMMP4Encoder : public ILMMP4Encoder1
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResetToDefaults( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MotionAccuracy( 
            /* [retval][out] */ eMotionAccuracy __RPC_FAR *pMotionAccuracy) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MotionAccuracy( 
            /* [in] */ eMotionAccuracy MotionAccuracy) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILMMP4EncoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILMMP4Encoder __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILMMP4Encoder __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OutputFrameRate )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pfFrameRate);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OutputFrameRate )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ double fFrameRate);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PFrames )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPFrames);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PFrames )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ long lPFrames);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BFrames )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plBFrames);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BFrames )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ long lBFrames);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchWidth )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPSearchWidth);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SearchWidth )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ long lPSearchWidth);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchHeight )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plSearchHeight);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SearchHeight )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ long lSearchHeight);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QFactor )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plQFactor);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_QFactor )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ long lQFactor);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QuantType )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ eQuantType __RPC_FAR *pQuantType);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_QuantType )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ eQuantType QuantType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MotionAlgorithm )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ eMotionAlgorithm __RPC_FAR *pMotionAlgorithm);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MotionAlgorithm )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ eMotionAlgorithm MotionAlgorithm);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MotionAccurancy )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ eMotionAccuracy __RPC_FAR *pMotionAccuracy);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MotionAccurancy )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ eMotionAccuracy MotionAccuracy);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MotionVectorPerMacroblock )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ eMotionVectorPerMacroblock __RPC_FAR *plMotionVectorPerMacroblock);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MotionVectorPerMacroblock )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ eMotionVectorPerMacroblock lMotionVectorPerMacroblock);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteToRegistry )( 
            ILMMP4Encoder __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OutputFormat )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ eOutputFormat __RPC_FAR *pOutputFormat);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OutputFormat )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ eOutputFormat lOutputFormat);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SuperCompression )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbSuperCompression);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SuperCompression )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bSuperCompression);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ResetToDefaults )( 
            ILMMP4Encoder __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MotionAccuracy )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [retval][out] */ eMotionAccuracy __RPC_FAR *pMotionAccuracy);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MotionAccuracy )( 
            ILMMP4Encoder __RPC_FAR * This,
            /* [in] */ eMotionAccuracy MotionAccuracy);
        
        END_INTERFACE
    } ILMMP4EncoderVtbl;

    interface ILMMP4Encoder
    {
        CONST_VTBL struct ILMMP4EncoderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILMMP4Encoder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILMMP4Encoder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILMMP4Encoder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILMMP4Encoder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ILMMP4Encoder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ILMMP4Encoder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ILMMP4Encoder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ILMMP4Encoder_get_OutputFrameRate(This,pfFrameRate)	\
    (This)->lpVtbl -> get_OutputFrameRate(This,pfFrameRate)

#define ILMMP4Encoder_put_OutputFrameRate(This,fFrameRate)	\
    (This)->lpVtbl -> put_OutputFrameRate(This,fFrameRate)

#define ILMMP4Encoder_get_PFrames(This,plPFrames)	\
    (This)->lpVtbl -> get_PFrames(This,plPFrames)

#define ILMMP4Encoder_put_PFrames(This,lPFrames)	\
    (This)->lpVtbl -> put_PFrames(This,lPFrames)

#define ILMMP4Encoder_get_BFrames(This,plBFrames)	\
    (This)->lpVtbl -> get_BFrames(This,plBFrames)

#define ILMMP4Encoder_put_BFrames(This,lBFrames)	\
    (This)->lpVtbl -> put_BFrames(This,lBFrames)

#define ILMMP4Encoder_get_SearchWidth(This,plPSearchWidth)	\
    (This)->lpVtbl -> get_SearchWidth(This,plPSearchWidth)

#define ILMMP4Encoder_put_SearchWidth(This,lPSearchWidth)	\
    (This)->lpVtbl -> put_SearchWidth(This,lPSearchWidth)

#define ILMMP4Encoder_get_SearchHeight(This,plSearchHeight)	\
    (This)->lpVtbl -> get_SearchHeight(This,plSearchHeight)

#define ILMMP4Encoder_put_SearchHeight(This,lSearchHeight)	\
    (This)->lpVtbl -> put_SearchHeight(This,lSearchHeight)

#define ILMMP4Encoder_get_QFactor(This,plQFactor)	\
    (This)->lpVtbl -> get_QFactor(This,plQFactor)

#define ILMMP4Encoder_put_QFactor(This,lQFactor)	\
    (This)->lpVtbl -> put_QFactor(This,lQFactor)

#define ILMMP4Encoder_get_QuantType(This,pQuantType)	\
    (This)->lpVtbl -> get_QuantType(This,pQuantType)

#define ILMMP4Encoder_put_QuantType(This,QuantType)	\
    (This)->lpVtbl -> put_QuantType(This,QuantType)

#define ILMMP4Encoder_get_MotionAlgorithm(This,pMotionAlgorithm)	\
    (This)->lpVtbl -> get_MotionAlgorithm(This,pMotionAlgorithm)

#define ILMMP4Encoder_put_MotionAlgorithm(This,MotionAlgorithm)	\
    (This)->lpVtbl -> put_MotionAlgorithm(This,MotionAlgorithm)

#define ILMMP4Encoder_get_MotionAccurancy(This,pMotionAccuracy)	\
    (This)->lpVtbl -> get_MotionAccurancy(This,pMotionAccuracy)

#define ILMMP4Encoder_put_MotionAccurancy(This,MotionAccuracy)	\
    (This)->lpVtbl -> put_MotionAccurancy(This,MotionAccuracy)

#define ILMMP4Encoder_get_MotionVectorPerMacroblock(This,plMotionVectorPerMacroblock)	\
    (This)->lpVtbl -> get_MotionVectorPerMacroblock(This,plMotionVectorPerMacroblock)

#define ILMMP4Encoder_put_MotionVectorPerMacroblock(This,lMotionVectorPerMacroblock)	\
    (This)->lpVtbl -> put_MotionVectorPerMacroblock(This,lMotionVectorPerMacroblock)

#define ILMMP4Encoder_WriteToRegistry(This)	\
    (This)->lpVtbl -> WriteToRegistry(This)

#define ILMMP4Encoder_get_OutputFormat(This,pOutputFormat)	\
    (This)->lpVtbl -> get_OutputFormat(This,pOutputFormat)

#define ILMMP4Encoder_put_OutputFormat(This,lOutputFormat)	\
    (This)->lpVtbl -> put_OutputFormat(This,lOutputFormat)

#define ILMMP4Encoder_get_SuperCompression(This,pbSuperCompression)	\
    (This)->lpVtbl -> get_SuperCompression(This,pbSuperCompression)

#define ILMMP4Encoder_put_SuperCompression(This,bSuperCompression)	\
    (This)->lpVtbl -> put_SuperCompression(This,bSuperCompression)


#define ILMMP4Encoder_ResetToDefaults(This)	\
    (This)->lpVtbl -> ResetToDefaults(This)

#define ILMMP4Encoder_get_MotionAccuracy(This,pMotionAccuracy)	\
    (This)->lpVtbl -> get_MotionAccuracy(This,pMotionAccuracy)

#define ILMMP4Encoder_put_MotionAccuracy(This,MotionAccuracy)	\
    (This)->lpVtbl -> put_MotionAccuracy(This,MotionAccuracy)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder_ResetToDefaults_Proxy( 
    ILMMP4Encoder __RPC_FAR * This);


void __RPC_STUB ILMMP4Encoder_ResetToDefaults_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder_get_MotionAccuracy_Proxy( 
    ILMMP4Encoder __RPC_FAR * This,
    /* [retval][out] */ eMotionAccuracy __RPC_FAR *pMotionAccuracy);


void __RPC_STUB ILMMP4Encoder_get_MotionAccuracy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMMP4Encoder_put_MotionAccuracy_Proxy( 
    ILMMP4Encoder __RPC_FAR * This,
    /* [in] */ eMotionAccuracy MotionAccuracy);


void __RPC_STUB ILMMP4Encoder_put_MotionAccuracy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILMMP4Encoder_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_LMMP4Encoder;

#ifdef __cplusplus

class DECLSPEC_UUID("E2B7DD65-38C5-11D5-91F6-00104BDB8FF9")
LMMP4Encoder;
#endif
#endif /* __LMMP4EncoderLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

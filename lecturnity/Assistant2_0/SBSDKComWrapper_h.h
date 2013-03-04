

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Fri Nov 30 09:55:36 2007
 */
/* Compiler settings for .\SBSDKComWrapper.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __SBSDKComWrapper_h_h__
#define __SBSDKComWrapper_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISBSDKBaseClass_FWD_DEFINED__
#define __ISBSDKBaseClass_FWD_DEFINED__
typedef interface ISBSDKBaseClass ISBSDKBaseClass;
#endif 	/* __ISBSDKBaseClass_FWD_DEFINED__ */


#ifndef __ISBSDKBaseClassEx_FWD_DEFINED__
#define __ISBSDKBaseClassEx_FWD_DEFINED__
typedef interface ISBSDKBaseClassEx ISBSDKBaseClassEx;
#endif 	/* __ISBSDKBaseClassEx_FWD_DEFINED__ */


#ifndef __ISBSDKBaseClass2_FWD_DEFINED__
#define __ISBSDKBaseClass2_FWD_DEFINED__
typedef interface ISBSDKBaseClass2 ISBSDKBaseClass2;
#endif 	/* __ISBSDKBaseClass2_FWD_DEFINED__ */


#ifndef ___ISBSDKBaseClassEvents_FWD_DEFINED__
#define ___ISBSDKBaseClassEvents_FWD_DEFINED__
typedef interface _ISBSDKBaseClassEvents _ISBSDKBaseClassEvents;
#endif 	/* ___ISBSDKBaseClassEvents_FWD_DEFINED__ */


#ifndef __SBSDKBaseClass_FWD_DEFINED__
#define __SBSDKBaseClass_FWD_DEFINED__

#ifdef __cplusplus
typedef class SBSDKBaseClass SBSDKBaseClass;
#else
typedef struct SBSDKBaseClass SBSDKBaseClass;
#endif /* __cplusplus */

#endif 	/* __SBSDKBaseClass_FWD_DEFINED__ */


#ifndef ___ISBSDKBaseClassExEvents_FWD_DEFINED__
#define ___ISBSDKBaseClassExEvents_FWD_DEFINED__
typedef interface _ISBSDKBaseClassExEvents _ISBSDKBaseClassExEvents;
#endif 	/* ___ISBSDKBaseClassExEvents_FWD_DEFINED__ */


#ifndef __SBSDKBaseClassEx_FWD_DEFINED__
#define __SBSDKBaseClassEx_FWD_DEFINED__

#ifdef __cplusplus
typedef class SBSDKBaseClassEx SBSDKBaseClassEx;
#else
typedef struct SBSDKBaseClassEx SBSDKBaseClassEx;
#endif /* __cplusplus */

#endif 	/* __SBSDKBaseClassEx_FWD_DEFINED__ */


#ifndef ___ISBSDKBaseClass2Events_FWD_DEFINED__
#define ___ISBSDKBaseClass2Events_FWD_DEFINED__
typedef interface _ISBSDKBaseClass2Events _ISBSDKBaseClass2Events;
#endif 	/* ___ISBSDKBaseClass2Events_FWD_DEFINED__ */


#ifndef __SBSDKBaseClass2_FWD_DEFINED__
#define __SBSDKBaseClass2_FWD_DEFINED__

#ifdef __cplusplus
typedef class SBSDKBaseClass2 SBSDKBaseClass2;
#else
typedef struct SBSDKBaseClass2 SBSDKBaseClass2;
#endif /* __cplusplus */

#endif 	/* __SBSDKBaseClass2_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ISBSDKBaseClass_INTERFACE_DEFINED__
#define __ISBSDKBaseClass_INTERFACE_DEFINED__

/* interface ISBSDKBaseClass */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 

typedef 
enum _SBCSDK_TOOL_TYPE
    {	SBC_NO_TOOL	= 0,
	SBC_PEN	= SBC_NO_TOOL + 1,
	SBC_ERASER	= SBC_PEN + 1,
	SBC_RECTANGLE	= SBC_ERASER + 1,
	SBC_LINE	= SBC_RECTANGLE + 1,
	SBC_CIRCLE	= SBC_LINE + 1,
	SBC_POLYGON	= SBC_CIRCLE + 1
    } 	SBCSDK_TOOL_TYPE;

typedef 
enum _SBCSDK_MOUSE_EVENT_FLAG
    {	SBCME_DEFAULT	= 0,
	SBCME_ALWAYS	= 0x1,
	SBCME_NEVER	= 0x2
    } 	SBCSDK_MOUSE_EVENT_FLAG;


EXTERN_C const IID IID_ISBSDKBaseClass;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("006EC885-2A99-4823-B6D2-9F86D6496C92")
    ISBSDKBaseClass : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKAttach( 
            /* [in] */ LONG hWnd,
            /* [in] */ VARIANT_BOOL bSendXMLAnnotations) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolType( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ SBCSDK_TOOL_TYPE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolColor( 
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolOpacity( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolFillColor( 
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolFillOpacity( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolWidth( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKIsABoardConnected( 
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetConnectedBoardCount( 
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetBoardCount( 
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKIsBoardProjected( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetBoardNumberFromPointerID( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetSoftwareVersion( 
            /* [out] */ LONG *iMajor,
            /* [out] */ LONG *iMinor,
            /* [out] */ LONG *iUpdate,
            /* [out] */ LONG *iBuild,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKDetach( 
            /* [in] */ LONG hWnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetDestinationWnd( 
            /* [out] */ LONG *hWnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKSetSendMouseEvents( 
            /* [in] */ LONG hAttachedWnd,
            /* [in] */ SBCSDK_MOUSE_EVENT_FLAG iTools,
            /* [in] */ LONG iPointerID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetCurrentBoard( 
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetCurrentTool( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISBSDKBaseClassVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISBSDKBaseClass * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISBSDKBaseClass * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISBSDKBaseClass * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISBSDKBaseClass * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISBSDKBaseClass * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISBSDKBaseClass * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISBSDKBaseClass * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKAttach )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG hWnd,
            /* [in] */ VARIANT_BOOL bSendXMLAnnotations);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolType )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ SBCSDK_TOOL_TYPE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolColor )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolOpacity )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolFillColor )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolFillOpacity )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolWidth )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKIsABoardConnected )( 
            ISBSDKBaseClass * This,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetConnectedBoardCount )( 
            ISBSDKBaseClass * This,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetBoardCount )( 
            ISBSDKBaseClass * This,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKIsBoardProjected )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetBoardNumberFromPointerID )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetSoftwareVersion )( 
            ISBSDKBaseClass * This,
            /* [out] */ LONG *iMajor,
            /* [out] */ LONG *iMinor,
            /* [out] */ LONG *iUpdate,
            /* [out] */ LONG *iBuild,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKDetach )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG hWnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetDestinationWnd )( 
            ISBSDKBaseClass * This,
            /* [out] */ LONG *hWnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKSetSendMouseEvents )( 
            ISBSDKBaseClass * This,
            /* [in] */ LONG hAttachedWnd,
            /* [in] */ SBCSDK_MOUSE_EVENT_FLAG iTools,
            /* [in] */ LONG iPointerID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetCurrentBoard )( 
            ISBSDKBaseClass * This,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetCurrentTool )( 
            ISBSDKBaseClass * This);
        
        END_INTERFACE
    } ISBSDKBaseClassVtbl;

    interface ISBSDKBaseClass
    {
        CONST_VTBL struct ISBSDKBaseClassVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISBSDKBaseClass_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISBSDKBaseClass_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISBSDKBaseClass_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISBSDKBaseClass_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISBSDKBaseClass_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISBSDKBaseClass_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISBSDKBaseClass_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISBSDKBaseClass_SBSDKAttach(This,hWnd,bSendXMLAnnotations)	\
    (This)->lpVtbl -> SBSDKAttach(This,hWnd,bSendXMLAnnotations)

#define ISBSDKBaseClass_SBSDKGetToolType(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolType(This,iPointerID,result)

#define ISBSDKBaseClass_SBSDKGetToolColor(This,iPointerID,red,green,blue)	\
    (This)->lpVtbl -> SBSDKGetToolColor(This,iPointerID,red,green,blue)

#define ISBSDKBaseClass_SBSDKGetToolOpacity(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolOpacity(This,iPointerID,result)

#define ISBSDKBaseClass_SBSDKGetToolFillColor(This,iPointerID,red,green,blue)	\
    (This)->lpVtbl -> SBSDKGetToolFillColor(This,iPointerID,red,green,blue)

#define ISBSDKBaseClass_SBSDKGetToolFillOpacity(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolFillOpacity(This,iPointerID,result)

#define ISBSDKBaseClass_SBSDKGetToolWidth(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolWidth(This,iPointerID,result)

#define ISBSDKBaseClass_SBSDKIsABoardConnected(This,result)	\
    (This)->lpVtbl -> SBSDKIsABoardConnected(This,result)

#define ISBSDKBaseClass_SBSDKGetConnectedBoardCount(This,result)	\
    (This)->lpVtbl -> SBSDKGetConnectedBoardCount(This,result)

#define ISBSDKBaseClass_SBSDKGetBoardCount(This,result)	\
    (This)->lpVtbl -> SBSDKGetBoardCount(This,result)

#define ISBSDKBaseClass_SBSDKIsBoardProjected(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKIsBoardProjected(This,iPointerID,result)

#define ISBSDKBaseClass_SBSDKGetBoardNumberFromPointerID(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetBoardNumberFromPointerID(This,iPointerID,result)

#define ISBSDKBaseClass_SBSDKGetSoftwareVersion(This,iMajor,iMinor,iUpdate,iBuild,result)	\
    (This)->lpVtbl -> SBSDKGetSoftwareVersion(This,iMajor,iMinor,iUpdate,iBuild,result)

#define ISBSDKBaseClass_SBSDKDetach(This,hWnd)	\
    (This)->lpVtbl -> SBSDKDetach(This,hWnd)

#define ISBSDKBaseClass_SBSDKGetDestinationWnd(This,hWnd)	\
    (This)->lpVtbl -> SBSDKGetDestinationWnd(This,hWnd)

#define ISBSDKBaseClass_SBSDKSetSendMouseEvents(This,hAttachedWnd,iTools,iPointerID)	\
    (This)->lpVtbl -> SBSDKSetSendMouseEvents(This,hAttachedWnd,iTools,iPointerID)

#define ISBSDKBaseClass_SBSDKGetCurrentBoard(This,result)	\
    (This)->lpVtbl -> SBSDKGetCurrentBoard(This,result)

#define ISBSDKBaseClass_SBSDKGetCurrentTool(This)	\
    (This)->lpVtbl -> SBSDKGetCurrentTool(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKAttach_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG hWnd,
    /* [in] */ VARIANT_BOOL bSendXMLAnnotations);


void __RPC_STUB ISBSDKBaseClass_SBSDKAttach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetToolType_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ SBCSDK_TOOL_TYPE *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetToolType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetToolColor_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG iPointerID,
    /* [out] */ LONG *red,
    /* [out] */ LONG *green,
    /* [out] */ LONG *blue);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetToolColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetToolOpacity_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ FLOAT *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetToolOpacity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetToolFillColor_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG iPointerID,
    /* [out] */ LONG *red,
    /* [out] */ LONG *green,
    /* [out] */ LONG *blue);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetToolFillColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetToolFillOpacity_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ FLOAT *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetToolFillOpacity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetToolWidth_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetToolWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKIsABoardConnected_Proxy( 
    ISBSDKBaseClass * This,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKIsABoardConnected_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetConnectedBoardCount_Proxy( 
    ISBSDKBaseClass * This,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetConnectedBoardCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetBoardCount_Proxy( 
    ISBSDKBaseClass * This,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetBoardCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKIsBoardProjected_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKIsBoardProjected_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetBoardNumberFromPointerID_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetBoardNumberFromPointerID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetSoftwareVersion_Proxy( 
    ISBSDKBaseClass * This,
    /* [out] */ LONG *iMajor,
    /* [out] */ LONG *iMinor,
    /* [out] */ LONG *iUpdate,
    /* [out] */ LONG *iBuild,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetSoftwareVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKDetach_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG hWnd);


void __RPC_STUB ISBSDKBaseClass_SBSDKDetach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetDestinationWnd_Proxy( 
    ISBSDKBaseClass * This,
    /* [out] */ LONG *hWnd);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetDestinationWnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKSetSendMouseEvents_Proxy( 
    ISBSDKBaseClass * This,
    /* [in] */ LONG hAttachedWnd,
    /* [in] */ SBCSDK_MOUSE_EVENT_FLAG iTools,
    /* [in] */ LONG iPointerID);


void __RPC_STUB ISBSDKBaseClass_SBSDKSetSendMouseEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetCurrentBoard_Proxy( 
    ISBSDKBaseClass * This,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetCurrentBoard_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass_SBSDKGetCurrentTool_Proxy( 
    ISBSDKBaseClass * This);


void __RPC_STUB ISBSDKBaseClass_SBSDKGetCurrentTool_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISBSDKBaseClass_INTERFACE_DEFINED__ */


#ifndef __ISBSDKBaseClassEx_INTERFACE_DEFINED__
#define __ISBSDKBaseClassEx_INTERFACE_DEFINED__

/* interface ISBSDKBaseClassEx */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 

typedef 
enum _SBCSDK_BOARD_STATUS
    {	SBC_BOARD_OPEN	= 0,
	SBC_BOARD_CLOSED	= SBC_BOARD_OPEN + 1,
	SBC_BOARD_DEVICE_IN_USE	= SBC_BOARD_CLOSED + 1,
	SBC_BOARD_DEVICE_NOT_FOUND	= SBC_BOARD_DEVICE_IN_USE + 1,
	SBC_BOARD_DEVICE_NOT_PRESENT	= SBC_BOARD_DEVICE_NOT_FOUND + 1,
	SBC_BOARD_NO_DATA	= SBC_BOARD_DEVICE_NOT_PRESENT + 1,
	SBC_BOARD_ANALYZING_DATA	= SBC_BOARD_NO_DATA + 1,
	SBC_BOARD_NOT_RESPONDING	= SBC_BOARD_ANALYZING_DATA + 1,
	SBC_BOARD_DISABLED	= SBC_BOARD_NOT_RESPONDING + 1,
	SBC_BOARD_USB_UNPLUGGED	= SBC_BOARD_DISABLED + 1
    } 	SBCSDK_BOARD_STATUS;

typedef 
enum _SBCSDK_MOUSE_STATE
    {	SBC_MOUSE_LEFT_CLICK	= 0,
	SBC_MOUSE_MIDDLE_CLICK	= SBC_MOUSE_LEFT_CLICK + 1,
	SBC_MOUSE_RIGHT_CLICK	= SBC_MOUSE_MIDDLE_CLICK + 1,
	SBC_MOUSE_CTRL_CLICK	= SBC_MOUSE_RIGHT_CLICK + 1,
	SBC_MOUSE_ALT_CLICK	= SBC_MOUSE_CTRL_CLICK + 1,
	SBC_MOUSE_SHIFT_CLICK	= SBC_MOUSE_ALT_CLICK + 1,
	SBC_MOUSE_FLOAT_CLICK	= SBC_MOUSE_SHIFT_CLICK + 1,
	SBC_MOUSE_NEXT_CLICK_NOT_SET	= SBC_MOUSE_FLOAT_CLICK + 1
    } 	SBCSDK_MOUSE_STATE;


EXTERN_C const IID IID_ISBSDKBaseClassEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("54EE21D3-6D4B-473B-BB82-0034DF1D2F1D")
    ISBSDKBaseClassEx : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKAttach( 
            /* [in] */ LONG hWnd,
            /* [in] */ VARIANT_BOOL bSendXMLAnnotations) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolType( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ SBCSDK_TOOL_TYPE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolColor( 
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolOpacity( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolFillColor( 
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolFillOpacity( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolWidth( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKIsABoardConnected( 
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetConnectedBoardCount( 
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetBoardCount( 
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKIsBoardProjected( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetBoardNumberFromPointerID( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetSoftwareVersion( 
            /* [out] */ LONG *iMajor,
            /* [out] */ LONG *iMinor,
            /* [out] */ LONG *iUpdate,
            /* [out] */ LONG *iBuild,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKDetach( 
            /* [in] */ LONG hWnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetDestinationWnd( 
            /* [out] */ LONG *hWnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKSetSendMouseEvents( 
            /* [in] */ LONG hAttachedWnd,
            /* [in] */ SBCSDK_MOUSE_EVENT_FLAG iTools,
            /* [in] */ LONG iPointerID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetCurrentBoard( 
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetCurrentTool( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKRegisterDataMonitor( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBDViTStartTrackerMode( 
            /* [in] */ LONG iBoardNumber) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBDViTStopTrackerMode( 
            /* [in] */ LONG iBoardNumber) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKIsBoardDViT( 
            /* [in] */ LONG iBoardNumber,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetBoardStatus( 
            /* [in] */ LONG iBoardNumber,
            /* [retval][out] */ SBCSDK_BOARD_STATUS *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKIsConnected( 
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetMouseState( 
            /* [retval][out] */ SBCSDK_MOUSE_STATE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKSetMouseState( 
            /* [in] */ SBCSDK_MOUSE_STATE iMouseState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetMultiPointerMode( 
            /* [in] */ LONG iBoardNumber) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISBSDKBaseClassExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISBSDKBaseClassEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISBSDKBaseClassEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISBSDKBaseClassEx * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKAttach )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG hWnd,
            /* [in] */ VARIANT_BOOL bSendXMLAnnotations);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolType )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ SBCSDK_TOOL_TYPE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolColor )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolOpacity )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolFillColor )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolFillOpacity )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolWidth )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKIsABoardConnected )( 
            ISBSDKBaseClassEx * This,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetConnectedBoardCount )( 
            ISBSDKBaseClassEx * This,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetBoardCount )( 
            ISBSDKBaseClassEx * This,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKIsBoardProjected )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetBoardNumberFromPointerID )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetSoftwareVersion )( 
            ISBSDKBaseClassEx * This,
            /* [out] */ LONG *iMajor,
            /* [out] */ LONG *iMinor,
            /* [out] */ LONG *iUpdate,
            /* [out] */ LONG *iBuild,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKDetach )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG hWnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetDestinationWnd )( 
            ISBSDKBaseClassEx * This,
            /* [out] */ LONG *hWnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKSetSendMouseEvents )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG hAttachedWnd,
            /* [in] */ SBCSDK_MOUSE_EVENT_FLAG iTools,
            /* [in] */ LONG iPointerID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetCurrentBoard )( 
            ISBSDKBaseClassEx * This,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetCurrentTool )( 
            ISBSDKBaseClassEx * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKRegisterDataMonitor )( 
            ISBSDKBaseClassEx * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBDViTStartTrackerMode )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iBoardNumber);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBDViTStopTrackerMode )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iBoardNumber);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKIsBoardDViT )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iBoardNumber,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetBoardStatus )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iBoardNumber,
            /* [retval][out] */ SBCSDK_BOARD_STATUS *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKIsConnected )( 
            ISBSDKBaseClassEx * This,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetMouseState )( 
            ISBSDKBaseClassEx * This,
            /* [retval][out] */ SBCSDK_MOUSE_STATE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKSetMouseState )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ SBCSDK_MOUSE_STATE iMouseState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetMultiPointerMode )( 
            ISBSDKBaseClassEx * This,
            /* [in] */ LONG iBoardNumber);
        
        END_INTERFACE
    } ISBSDKBaseClassExVtbl;

    interface ISBSDKBaseClassEx
    {
        CONST_VTBL struct ISBSDKBaseClassExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISBSDKBaseClassEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISBSDKBaseClassEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISBSDKBaseClassEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISBSDKBaseClassEx_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISBSDKBaseClassEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISBSDKBaseClassEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISBSDKBaseClassEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISBSDKBaseClassEx_SBSDKAttach(This,hWnd,bSendXMLAnnotations)	\
    (This)->lpVtbl -> SBSDKAttach(This,hWnd,bSendXMLAnnotations)

#define ISBSDKBaseClassEx_SBSDKGetToolType(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolType(This,iPointerID,result)

#define ISBSDKBaseClassEx_SBSDKGetToolColor(This,iPointerID,red,green,blue)	\
    (This)->lpVtbl -> SBSDKGetToolColor(This,iPointerID,red,green,blue)

#define ISBSDKBaseClassEx_SBSDKGetToolOpacity(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolOpacity(This,iPointerID,result)

#define ISBSDKBaseClassEx_SBSDKGetToolFillColor(This,iPointerID,red,green,blue)	\
    (This)->lpVtbl -> SBSDKGetToolFillColor(This,iPointerID,red,green,blue)

#define ISBSDKBaseClassEx_SBSDKGetToolFillOpacity(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolFillOpacity(This,iPointerID,result)

#define ISBSDKBaseClassEx_SBSDKGetToolWidth(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolWidth(This,iPointerID,result)

#define ISBSDKBaseClassEx_SBSDKIsABoardConnected(This,result)	\
    (This)->lpVtbl -> SBSDKIsABoardConnected(This,result)

#define ISBSDKBaseClassEx_SBSDKGetConnectedBoardCount(This,result)	\
    (This)->lpVtbl -> SBSDKGetConnectedBoardCount(This,result)

#define ISBSDKBaseClassEx_SBSDKGetBoardCount(This,result)	\
    (This)->lpVtbl -> SBSDKGetBoardCount(This,result)

#define ISBSDKBaseClassEx_SBSDKIsBoardProjected(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKIsBoardProjected(This,iPointerID,result)

#define ISBSDKBaseClassEx_SBSDKGetBoardNumberFromPointerID(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetBoardNumberFromPointerID(This,iPointerID,result)

#define ISBSDKBaseClassEx_SBSDKGetSoftwareVersion(This,iMajor,iMinor,iUpdate,iBuild,result)	\
    (This)->lpVtbl -> SBSDKGetSoftwareVersion(This,iMajor,iMinor,iUpdate,iBuild,result)

#define ISBSDKBaseClassEx_SBSDKDetach(This,hWnd)	\
    (This)->lpVtbl -> SBSDKDetach(This,hWnd)

#define ISBSDKBaseClassEx_SBSDKGetDestinationWnd(This,hWnd)	\
    (This)->lpVtbl -> SBSDKGetDestinationWnd(This,hWnd)

#define ISBSDKBaseClassEx_SBSDKSetSendMouseEvents(This,hAttachedWnd,iTools,iPointerID)	\
    (This)->lpVtbl -> SBSDKSetSendMouseEvents(This,hAttachedWnd,iTools,iPointerID)

#define ISBSDKBaseClassEx_SBSDKGetCurrentBoard(This,result)	\
    (This)->lpVtbl -> SBSDKGetCurrentBoard(This,result)

#define ISBSDKBaseClassEx_SBSDKGetCurrentTool(This)	\
    (This)->lpVtbl -> SBSDKGetCurrentTool(This)

#define ISBSDKBaseClassEx_SBSDKRegisterDataMonitor(This)	\
    (This)->lpVtbl -> SBSDKRegisterDataMonitor(This)

#define ISBSDKBaseClassEx_SBDViTStartTrackerMode(This,iBoardNumber)	\
    (This)->lpVtbl -> SBDViTStartTrackerMode(This,iBoardNumber)

#define ISBSDKBaseClassEx_SBDViTStopTrackerMode(This,iBoardNumber)	\
    (This)->lpVtbl -> SBDViTStopTrackerMode(This,iBoardNumber)

#define ISBSDKBaseClassEx_SBSDKIsBoardDViT(This,iBoardNumber,result)	\
    (This)->lpVtbl -> SBSDKIsBoardDViT(This,iBoardNumber,result)

#define ISBSDKBaseClassEx_SBSDKGetBoardStatus(This,iBoardNumber,result)	\
    (This)->lpVtbl -> SBSDKGetBoardStatus(This,iBoardNumber,result)

#define ISBSDKBaseClassEx_SBSDKIsConnected(This,result)	\
    (This)->lpVtbl -> SBSDKIsConnected(This,result)

#define ISBSDKBaseClassEx_SBSDKGetMouseState(This,result)	\
    (This)->lpVtbl -> SBSDKGetMouseState(This,result)

#define ISBSDKBaseClassEx_SBSDKSetMouseState(This,iMouseState)	\
    (This)->lpVtbl -> SBSDKSetMouseState(This,iMouseState)

#define ISBSDKBaseClassEx_SBSDKGetMultiPointerMode(This,iBoardNumber)	\
    (This)->lpVtbl -> SBSDKGetMultiPointerMode(This,iBoardNumber)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKAttach_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG hWnd,
    /* [in] */ VARIANT_BOOL bSendXMLAnnotations);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKAttach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetToolType_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ SBCSDK_TOOL_TYPE *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetToolType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetToolColor_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iPointerID,
    /* [out] */ LONG *red,
    /* [out] */ LONG *green,
    /* [out] */ LONG *blue);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetToolColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetToolOpacity_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ FLOAT *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetToolOpacity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetToolFillColor_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iPointerID,
    /* [out] */ LONG *red,
    /* [out] */ LONG *green,
    /* [out] */ LONG *blue);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetToolFillColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetToolFillOpacity_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ FLOAT *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetToolFillOpacity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetToolWidth_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetToolWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKIsABoardConnected_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKIsABoardConnected_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetConnectedBoardCount_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetConnectedBoardCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetBoardCount_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetBoardCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKIsBoardProjected_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKIsBoardProjected_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetBoardNumberFromPointerID_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetBoardNumberFromPointerID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetSoftwareVersion_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [out] */ LONG *iMajor,
    /* [out] */ LONG *iMinor,
    /* [out] */ LONG *iUpdate,
    /* [out] */ LONG *iBuild,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetSoftwareVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKDetach_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG hWnd);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKDetach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetDestinationWnd_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [out] */ LONG *hWnd);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetDestinationWnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKSetSendMouseEvents_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG hAttachedWnd,
    /* [in] */ SBCSDK_MOUSE_EVENT_FLAG iTools,
    /* [in] */ LONG iPointerID);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKSetSendMouseEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetCurrentBoard_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetCurrentBoard_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetCurrentTool_Proxy( 
    ISBSDKBaseClassEx * This);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetCurrentTool_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKRegisterDataMonitor_Proxy( 
    ISBSDKBaseClassEx * This);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKRegisterDataMonitor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBDViTStartTrackerMode_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iBoardNumber);


void __RPC_STUB ISBSDKBaseClassEx_SBDViTStartTrackerMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBDViTStopTrackerMode_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iBoardNumber);


void __RPC_STUB ISBSDKBaseClassEx_SBDViTStopTrackerMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKIsBoardDViT_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iBoardNumber,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKIsBoardDViT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetBoardStatus_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iBoardNumber,
    /* [retval][out] */ SBCSDK_BOARD_STATUS *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetBoardStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKIsConnected_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKIsConnected_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetMouseState_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [retval][out] */ SBCSDK_MOUSE_STATE *result);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetMouseState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKSetMouseState_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ SBCSDK_MOUSE_STATE iMouseState);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKSetMouseState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClassEx_SBSDKGetMultiPointerMode_Proxy( 
    ISBSDKBaseClassEx * This,
    /* [in] */ LONG iBoardNumber);


void __RPC_STUB ISBSDKBaseClassEx_SBSDKGetMultiPointerMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISBSDKBaseClassEx_INTERFACE_DEFINED__ */


#ifndef __ISBSDKBaseClass2_INTERFACE_DEFINED__
#define __ISBSDKBaseClass2_INTERFACE_DEFINED__

/* interface ISBSDKBaseClass2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 

typedef 
enum _SBCSDK_PEN_TRAY_TOOL
    {	SBCSDK_PTT_NONE	= 0,
	SBCSDK_PTT_ERASER	= SBCSDK_PTT_NONE + 1,
	SBCSDK_PTT_BLACK_PEN	= SBCSDK_PTT_ERASER + 1,
	SBCSDK_PTT_BLUE_PEN	= SBCSDK_PTT_BLACK_PEN + 1,
	SBCSDK_PTT_RED_PEN	= SBCSDK_PTT_BLUE_PEN + 1,
	SBCSDK_PTT_GREEN_PEN	= SBCSDK_PTT_RED_PEN + 1
    } 	SBCSDK_PEN_TRAY_TOOL;


EXTERN_C const IID IID_ISBSDKBaseClass2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1A12B0DC-6FD5-4C84-9977-6ABCB416E10A")
    ISBSDKBaseClass2 : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKAttach( 
            /* [in] */ LONG hWnd,
            /* [in] */ VARIANT_BOOL bSendXMLAnnotations) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKAttachWithMsgWnd( 
            /* [in] */ LONG hWnd,
            /* [in] */ VARIANT_BOOL bSendXMLAnnotations,
            /* [in] */ LONG hWndProcessData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKDetach( 
            /* [in] */ LONG hWnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetXYDestinationWnd( 
            /* [retval][out] */ LONG *lResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKProcessData( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolType( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ SBCSDK_TOOL_TYPE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolColor( 
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolOpacity( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolFillColor( 
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolFillOpacity( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetToolWidth( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKIsABoardConnected( 
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetConnectedBoardCount( 
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetBoardCount( 
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKIsBoardProjected( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetBoardNumberFromPointerID( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetSoftwareVersion( 
            /* [out] */ LONG *iMajor,
            /* [out] */ LONG *iMinor,
            /* [out] */ LONG *iUpdate,
            /* [out] */ LONG *iBuild,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetCurrentBoard( 
            /* [retval][out] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetCurrentTool( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKSetSendMouseEvents( 
            /* [in] */ LONG hAttachedWnd,
            /* [in] */ SBCSDK_MOUSE_EVENT_FLAG iTools,
            /* [in] */ LONG iPointerID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKSetSendXMLToolChanges( 
            /* [in] */ VARIANT_BOOL bSendXMLToolChanges) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetXMLToolStr( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetBoardStatus( 
            /* [in] */ LONG iBoardNumber,
            /* [retval][out] */ SBCSDK_BOARD_STATUS *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKIsConnected( 
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetMultiPointerMode( 
            /* [in] */ LONG iBoardNumber) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKSetMultiPointerMode( 
            /* [in] */ LONG iBoardNumber,
            /* [in] */ VARIANT_BOOL bOn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKSetToolXMLStr( 
            /* [in] */ BSTR bstrXMLTool) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKRegisterProjectedWindowByName( 
            /* [in] */ BSTR bstrWindowTitle,
            /* [in] */ VARIANT_BOOL bSendXMLAnnotations) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetPenTrayToolType( 
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ SBCSDK_PEN_TRAY_TOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKRegisterDataMonitor( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKGetMouseState( 
            /* [retval][out] */ SBCSDK_MOUSE_STATE *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKSetMouseState( 
            /* [in] */ SBCSDK_MOUSE_STATE mouseState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKIsBoardDViT( 
            /* [in] */ LONG iBoardNumber,
            /* [retval][out] */ VARIANT_BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SBSDKSetDViTTrackerMode( 
            /* [in] */ LONG iBoardNumber,
            /* [in] */ VARIANT_BOOL bOn) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISBSDKBaseClass2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISBSDKBaseClass2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISBSDKBaseClass2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISBSDKBaseClass2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKAttach )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG hWnd,
            /* [in] */ VARIANT_BOOL bSendXMLAnnotations);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKAttachWithMsgWnd )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG hWnd,
            /* [in] */ VARIANT_BOOL bSendXMLAnnotations,
            /* [in] */ LONG hWndProcessData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKDetach )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG hWnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetXYDestinationWnd )( 
            ISBSDKBaseClass2 * This,
            /* [retval][out] */ LONG *lResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKProcessData )( 
            ISBSDKBaseClass2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolType )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ SBCSDK_TOOL_TYPE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolColor )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolOpacity )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolFillColor )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iPointerID,
            /* [out] */ LONG *red,
            /* [out] */ LONG *green,
            /* [out] */ LONG *blue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolFillOpacity )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ FLOAT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetToolWidth )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKIsABoardConnected )( 
            ISBSDKBaseClass2 * This,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetConnectedBoardCount )( 
            ISBSDKBaseClass2 * This,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetBoardCount )( 
            ISBSDKBaseClass2 * This,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKIsBoardProjected )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetBoardNumberFromPointerID )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetSoftwareVersion )( 
            ISBSDKBaseClass2 * This,
            /* [out] */ LONG *iMajor,
            /* [out] */ LONG *iMinor,
            /* [out] */ LONG *iUpdate,
            /* [out] */ LONG *iBuild,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetCurrentBoard )( 
            ISBSDKBaseClass2 * This,
            /* [retval][out] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetCurrentTool )( 
            ISBSDKBaseClass2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKSetSendMouseEvents )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG hAttachedWnd,
            /* [in] */ SBCSDK_MOUSE_EVENT_FLAG iTools,
            /* [in] */ LONG iPointerID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKSetSendXMLToolChanges )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ VARIANT_BOOL bSendXMLToolChanges);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetXMLToolStr )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetBoardStatus )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iBoardNumber,
            /* [retval][out] */ SBCSDK_BOARD_STATUS *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKIsConnected )( 
            ISBSDKBaseClass2 * This,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetMultiPointerMode )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iBoardNumber);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKSetMultiPointerMode )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iBoardNumber,
            /* [in] */ VARIANT_BOOL bOn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKSetToolXMLStr )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ BSTR bstrXMLTool);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKRegisterProjectedWindowByName )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ BSTR bstrWindowTitle,
            /* [in] */ VARIANT_BOOL bSendXMLAnnotations);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetPenTrayToolType )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iPointerID,
            /* [retval][out] */ SBCSDK_PEN_TRAY_TOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKRegisterDataMonitor )( 
            ISBSDKBaseClass2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKGetMouseState )( 
            ISBSDKBaseClass2 * This,
            /* [retval][out] */ SBCSDK_MOUSE_STATE *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKSetMouseState )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ SBCSDK_MOUSE_STATE mouseState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKIsBoardDViT )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iBoardNumber,
            /* [retval][out] */ VARIANT_BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SBSDKSetDViTTrackerMode )( 
            ISBSDKBaseClass2 * This,
            /* [in] */ LONG iBoardNumber,
            /* [in] */ VARIANT_BOOL bOn);
        
        END_INTERFACE
    } ISBSDKBaseClass2Vtbl;

    interface ISBSDKBaseClass2
    {
        CONST_VTBL struct ISBSDKBaseClass2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISBSDKBaseClass2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISBSDKBaseClass2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISBSDKBaseClass2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISBSDKBaseClass2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISBSDKBaseClass2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISBSDKBaseClass2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISBSDKBaseClass2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISBSDKBaseClass2_SBSDKAttach(This,hWnd,bSendXMLAnnotations)	\
    (This)->lpVtbl -> SBSDKAttach(This,hWnd,bSendXMLAnnotations)

#define ISBSDKBaseClass2_SBSDKAttachWithMsgWnd(This,hWnd,bSendXMLAnnotations,hWndProcessData)	\
    (This)->lpVtbl -> SBSDKAttachWithMsgWnd(This,hWnd,bSendXMLAnnotations,hWndProcessData)

#define ISBSDKBaseClass2_SBSDKDetach(This,hWnd)	\
    (This)->lpVtbl -> SBSDKDetach(This,hWnd)

#define ISBSDKBaseClass2_SBSDKGetXYDestinationWnd(This,lResult)	\
    (This)->lpVtbl -> SBSDKGetXYDestinationWnd(This,lResult)

#define ISBSDKBaseClass2_SBSDKProcessData(This)	\
    (This)->lpVtbl -> SBSDKProcessData(This)

#define ISBSDKBaseClass2_SBSDKGetToolType(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolType(This,iPointerID,result)

#define ISBSDKBaseClass2_SBSDKGetToolColor(This,iPointerID,red,green,blue)	\
    (This)->lpVtbl -> SBSDKGetToolColor(This,iPointerID,red,green,blue)

#define ISBSDKBaseClass2_SBSDKGetToolOpacity(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolOpacity(This,iPointerID,result)

#define ISBSDKBaseClass2_SBSDKGetToolFillColor(This,iPointerID,red,green,blue)	\
    (This)->lpVtbl -> SBSDKGetToolFillColor(This,iPointerID,red,green,blue)

#define ISBSDKBaseClass2_SBSDKGetToolFillOpacity(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolFillOpacity(This,iPointerID,result)

#define ISBSDKBaseClass2_SBSDKGetToolWidth(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetToolWidth(This,iPointerID,result)

#define ISBSDKBaseClass2_SBSDKIsABoardConnected(This,result)	\
    (This)->lpVtbl -> SBSDKIsABoardConnected(This,result)

#define ISBSDKBaseClass2_SBSDKGetConnectedBoardCount(This,result)	\
    (This)->lpVtbl -> SBSDKGetConnectedBoardCount(This,result)

#define ISBSDKBaseClass2_SBSDKGetBoardCount(This,result)	\
    (This)->lpVtbl -> SBSDKGetBoardCount(This,result)

#define ISBSDKBaseClass2_SBSDKIsBoardProjected(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKIsBoardProjected(This,iPointerID,result)

#define ISBSDKBaseClass2_SBSDKGetBoardNumberFromPointerID(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetBoardNumberFromPointerID(This,iPointerID,result)

#define ISBSDKBaseClass2_SBSDKGetSoftwareVersion(This,iMajor,iMinor,iUpdate,iBuild,result)	\
    (This)->lpVtbl -> SBSDKGetSoftwareVersion(This,iMajor,iMinor,iUpdate,iBuild,result)

#define ISBSDKBaseClass2_SBSDKGetCurrentBoard(This,result)	\
    (This)->lpVtbl -> SBSDKGetCurrentBoard(This,result)

#define ISBSDKBaseClass2_SBSDKGetCurrentTool(This)	\
    (This)->lpVtbl -> SBSDKGetCurrentTool(This)

#define ISBSDKBaseClass2_SBSDKSetSendMouseEvents(This,hAttachedWnd,iTools,iPointerID)	\
    (This)->lpVtbl -> SBSDKSetSendMouseEvents(This,hAttachedWnd,iTools,iPointerID)

#define ISBSDKBaseClass2_SBSDKSetSendXMLToolChanges(This,bSendXMLToolChanges)	\
    (This)->lpVtbl -> SBSDKSetSendXMLToolChanges(This,bSendXMLToolChanges)

#define ISBSDKBaseClass2_SBSDKGetXMLToolStr(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetXMLToolStr(This,iPointerID,result)

#define ISBSDKBaseClass2_SBSDKGetBoardStatus(This,iBoardNumber,result)	\
    (This)->lpVtbl -> SBSDKGetBoardStatus(This,iBoardNumber,result)

#define ISBSDKBaseClass2_SBSDKIsConnected(This,result)	\
    (This)->lpVtbl -> SBSDKIsConnected(This,result)

#define ISBSDKBaseClass2_SBSDKGetMultiPointerMode(This,iBoardNumber)	\
    (This)->lpVtbl -> SBSDKGetMultiPointerMode(This,iBoardNumber)

#define ISBSDKBaseClass2_SBSDKSetMultiPointerMode(This,iBoardNumber,bOn)	\
    (This)->lpVtbl -> SBSDKSetMultiPointerMode(This,iBoardNumber,bOn)

#define ISBSDKBaseClass2_SBSDKSetToolXMLStr(This,bstrXMLTool)	\
    (This)->lpVtbl -> SBSDKSetToolXMLStr(This,bstrXMLTool)

#define ISBSDKBaseClass2_SBSDKRegisterProjectedWindowByName(This,bstrWindowTitle,bSendXMLAnnotations)	\
    (This)->lpVtbl -> SBSDKRegisterProjectedWindowByName(This,bstrWindowTitle,bSendXMLAnnotations)

#define ISBSDKBaseClass2_SBSDKGetPenTrayToolType(This,iPointerID,result)	\
    (This)->lpVtbl -> SBSDKGetPenTrayToolType(This,iPointerID,result)

#define ISBSDKBaseClass2_SBSDKRegisterDataMonitor(This)	\
    (This)->lpVtbl -> SBSDKRegisterDataMonitor(This)

#define ISBSDKBaseClass2_SBSDKGetMouseState(This,result)	\
    (This)->lpVtbl -> SBSDKGetMouseState(This,result)

#define ISBSDKBaseClass2_SBSDKSetMouseState(This,mouseState)	\
    (This)->lpVtbl -> SBSDKSetMouseState(This,mouseState)

#define ISBSDKBaseClass2_SBSDKIsBoardDViT(This,iBoardNumber,result)	\
    (This)->lpVtbl -> SBSDKIsBoardDViT(This,iBoardNumber,result)

#define ISBSDKBaseClass2_SBSDKSetDViTTrackerMode(This,iBoardNumber,bOn)	\
    (This)->lpVtbl -> SBSDKSetDViTTrackerMode(This,iBoardNumber,bOn)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKAttach_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG hWnd,
    /* [in] */ VARIANT_BOOL bSendXMLAnnotations);


void __RPC_STUB ISBSDKBaseClass2_SBSDKAttach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKAttachWithMsgWnd_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG hWnd,
    /* [in] */ VARIANT_BOOL bSendXMLAnnotations,
    /* [in] */ LONG hWndProcessData);


void __RPC_STUB ISBSDKBaseClass2_SBSDKAttachWithMsgWnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKDetach_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG hWnd);


void __RPC_STUB ISBSDKBaseClass2_SBSDKDetach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetXYDestinationWnd_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [retval][out] */ LONG *lResult);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetXYDestinationWnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKProcessData_Proxy( 
    ISBSDKBaseClass2 * This);


void __RPC_STUB ISBSDKBaseClass2_SBSDKProcessData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetToolType_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ SBCSDK_TOOL_TYPE *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetToolType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetToolColor_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iPointerID,
    /* [out] */ LONG *red,
    /* [out] */ LONG *green,
    /* [out] */ LONG *blue);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetToolColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetToolOpacity_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ FLOAT *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetToolOpacity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetToolFillColor_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iPointerID,
    /* [out] */ LONG *red,
    /* [out] */ LONG *green,
    /* [out] */ LONG *blue);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetToolFillColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetToolFillOpacity_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ FLOAT *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetToolFillOpacity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetToolWidth_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetToolWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKIsABoardConnected_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKIsABoardConnected_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetConnectedBoardCount_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetConnectedBoardCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetBoardCount_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetBoardCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKIsBoardProjected_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKIsBoardProjected_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetBoardNumberFromPointerID_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetBoardNumberFromPointerID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetSoftwareVersion_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [out] */ LONG *iMajor,
    /* [out] */ LONG *iMinor,
    /* [out] */ LONG *iUpdate,
    /* [out] */ LONG *iBuild,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetSoftwareVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetCurrentBoard_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [retval][out] */ LONG *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetCurrentBoard_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetCurrentTool_Proxy( 
    ISBSDKBaseClass2 * This);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetCurrentTool_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKSetSendMouseEvents_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG hAttachedWnd,
    /* [in] */ SBCSDK_MOUSE_EVENT_FLAG iTools,
    /* [in] */ LONG iPointerID);


void __RPC_STUB ISBSDKBaseClass2_SBSDKSetSendMouseEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKSetSendXMLToolChanges_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ VARIANT_BOOL bSendXMLToolChanges);


void __RPC_STUB ISBSDKBaseClass2_SBSDKSetSendXMLToolChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetXMLToolStr_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ BSTR *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetXMLToolStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetBoardStatus_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iBoardNumber,
    /* [retval][out] */ SBCSDK_BOARD_STATUS *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetBoardStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKIsConnected_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKIsConnected_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetMultiPointerMode_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iBoardNumber);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetMultiPointerMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKSetMultiPointerMode_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iBoardNumber,
    /* [in] */ VARIANT_BOOL bOn);


void __RPC_STUB ISBSDKBaseClass2_SBSDKSetMultiPointerMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKSetToolXMLStr_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ BSTR bstrXMLTool);


void __RPC_STUB ISBSDKBaseClass2_SBSDKSetToolXMLStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKRegisterProjectedWindowByName_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ BSTR bstrWindowTitle,
    /* [in] */ VARIANT_BOOL bSendXMLAnnotations);


void __RPC_STUB ISBSDKBaseClass2_SBSDKRegisterProjectedWindowByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetPenTrayToolType_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iPointerID,
    /* [retval][out] */ SBCSDK_PEN_TRAY_TOOL *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetPenTrayToolType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKRegisterDataMonitor_Proxy( 
    ISBSDKBaseClass2 * This);


void __RPC_STUB ISBSDKBaseClass2_SBSDKRegisterDataMonitor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKGetMouseState_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [retval][out] */ SBCSDK_MOUSE_STATE *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKGetMouseState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKSetMouseState_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ SBCSDK_MOUSE_STATE mouseState);


void __RPC_STUB ISBSDKBaseClass2_SBSDKSetMouseState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKIsBoardDViT_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iBoardNumber,
    /* [retval][out] */ VARIANT_BOOL *result);


void __RPC_STUB ISBSDKBaseClass2_SBSDKIsBoardDViT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISBSDKBaseClass2_SBSDKSetDViTTrackerMode_Proxy( 
    ISBSDKBaseClass2 * This,
    /* [in] */ LONG iBoardNumber,
    /* [in] */ VARIANT_BOOL bOn);


void __RPC_STUB ISBSDKBaseClass2_SBSDKSetDViTTrackerMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISBSDKBaseClass2_INTERFACE_DEFINED__ */



#ifndef __SBSDKComWrapperLib_LIBRARY_DEFINED__
#define __SBSDKComWrapperLib_LIBRARY_DEFINED__

/* library SBSDKComWrapperLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_SBSDKComWrapperLib;

#ifndef ___ISBSDKBaseClassEvents_DISPINTERFACE_DEFINED__
#define ___ISBSDKBaseClassEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISBSDKBaseClassEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISBSDKBaseClassEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("E5414F79-475C-47CD-87AE-116CA193BF15")
    _ISBSDKBaseClassEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISBSDKBaseClassEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISBSDKBaseClassEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISBSDKBaseClassEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISBSDKBaseClassEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISBSDKBaseClassEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISBSDKBaseClassEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISBSDKBaseClassEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISBSDKBaseClassEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISBSDKBaseClassEventsVtbl;

    interface _ISBSDKBaseClassEvents
    {
        CONST_VTBL struct _ISBSDKBaseClassEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISBSDKBaseClassEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ISBSDKBaseClassEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ISBSDKBaseClassEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ISBSDKBaseClassEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ISBSDKBaseClassEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ISBSDKBaseClassEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ISBSDKBaseClassEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISBSDKBaseClassEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SBSDKBaseClass;

#ifdef __cplusplus

class DECLSPEC_UUID("5180E2B4-FAF0-4C88-8615-8B6B75B0E689")
SBSDKBaseClass;
#endif

#ifndef ___ISBSDKBaseClassExEvents_DISPINTERFACE_DEFINED__
#define ___ISBSDKBaseClassExEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISBSDKBaseClassExEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISBSDKBaseClassExEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("E8C0C627-2CA0-4ED7-9CB9-A44F600D3D76")
    _ISBSDKBaseClassExEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISBSDKBaseClassExEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISBSDKBaseClassExEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISBSDKBaseClassExEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISBSDKBaseClassExEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISBSDKBaseClassExEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISBSDKBaseClassExEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISBSDKBaseClassExEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISBSDKBaseClassExEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISBSDKBaseClassExEventsVtbl;

    interface _ISBSDKBaseClassExEvents
    {
        CONST_VTBL struct _ISBSDKBaseClassExEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISBSDKBaseClassExEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ISBSDKBaseClassExEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ISBSDKBaseClassExEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ISBSDKBaseClassExEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ISBSDKBaseClassExEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ISBSDKBaseClassExEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ISBSDKBaseClassExEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISBSDKBaseClassExEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SBSDKBaseClassEx;

#ifdef __cplusplus

class DECLSPEC_UUID("F8827A48-FAD3-42DF-8C5D-1536D212728C")
SBSDKBaseClassEx;
#endif

#ifndef ___ISBSDKBaseClass2Events_DISPINTERFACE_DEFINED__
#define ___ISBSDKBaseClass2Events_DISPINTERFACE_DEFINED__

/* dispinterface _ISBSDKBaseClass2Events */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISBSDKBaseClass2Events;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("9395111E-381A-4D5B-91DE-E77706A54385")
    _ISBSDKBaseClass2Events : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISBSDKBaseClass2EventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISBSDKBaseClass2Events * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISBSDKBaseClass2Events * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISBSDKBaseClass2Events * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISBSDKBaseClass2Events * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISBSDKBaseClass2Events * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISBSDKBaseClass2Events * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISBSDKBaseClass2Events * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISBSDKBaseClass2EventsVtbl;

    interface _ISBSDKBaseClass2Events
    {
        CONST_VTBL struct _ISBSDKBaseClass2EventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISBSDKBaseClass2Events_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ISBSDKBaseClass2Events_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ISBSDKBaseClass2Events_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ISBSDKBaseClass2Events_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ISBSDKBaseClass2Events_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ISBSDKBaseClass2Events_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ISBSDKBaseClass2Events_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISBSDKBaseClass2Events_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SBSDKBaseClass2;

#ifdef __cplusplus

class DECLSPEC_UUID("F134CF0C-9C05-4277-B67D-A25E3030271D")
SBSDKBaseClass2;
#endif
#endif /* __SBSDKComWrapperLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



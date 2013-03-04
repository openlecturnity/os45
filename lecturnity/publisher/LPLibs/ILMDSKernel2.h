/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Aug 08 18:16:01 2006
 */
/* Compiler settings for F:\mm\DShowFlt\DSEvalKernel2\DSKernel.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __ILMDSKernel2_h__
#define __ILMDSKernel2_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ILMDSKernel2_FWD_DEFINED__
#define __ILMDSKernel2_FWD_DEFINED__
typedef interface ILMDSKernel2 ILMDSKernel2;
#endif 	/* __ILMDSKernel2_FWD_DEFINED__ */


#ifndef __LMDSKernel2_FWD_DEFINED__
#define __LMDSKernel2_FWD_DEFINED__

#ifdef __cplusplus
typedef class LMDSKernel2 LMDSKernel2;
#else
typedef struct LMDSKernel2 LMDSKernel2;
#endif /* __cplusplus */

#endif 	/* __LMDSKernel2_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __LMDSKernelLib2_LIBRARY_DEFINED__
#define __LMDSKernelLib2_LIBRARY_DEFINED__

/* library LMDSKernelLib2 */
/* [helpstring][version][uuid] */ 

#ifndef __DSKernel2_H__
#define __DSKernel2_H__
const CLSID CLSID_LMDSKernel2        = {0xE2B7DDA9, 0x38C5, 0x11D5, {0x91, 0xF6, 0x00, 0x10, 0x4B, 0xDB, 0x8F, 0xF9}};
const IID IID_ILMDSKernel2         = {0xE2B7DDA8, 0x38C5, 0x11D5, {0x91, 0xF6, 0x00, 0x10, 0x4B, 0xDB, 0x8F, 0xF9}};
const GUID LIBID_LMDSKernelLib2     = {0xE2B7DDA7, 0x38C5, 0x11D5, {0x91, 0xF6, 0x00, 0x10, 0x4B, 0xDB, 0x8F, 0xF9}};
#endif 
typedef /* [v1_enum] */ 
enum RegistrationStateConstants2
    {	REGSTATE_EXPIRED	= 0x1,
	REGSTATE_RELEASE	= 0x2,
	REGSTATE_EVAL	= 0x3,
	REGSTATE_MM	= 0x4,
	REGSTATE_LOCKED	= 0x10000000
    }	RegistrationStateConstants2;

typedef /* [v1_enum] */ 
enum UnLockFlagsConstants2
    {	UNLOCK_COMPUTER	= 0x1,
	UNLOCK_APP_ONCE	= 0x2,
	UNLOCK_APP_ALWAYS	= 0x3,
	UNLOCK_MM	= 0x80000000
    }	UnLockFlagsConstants2;

typedef /* [v1_enum] */ 
enum LockFlagsConstants2
    {	LOCK_COMPUTER	= 0x1,
	LOCK_APP	= 0x2
    }	LockFlagsConstants2;


EXTERN_C const IID LIBID_LMDSKernelLib2;

#ifndef __ILMDSKernel2_INTERFACE_DEFINED__
#define __ILMDSKernel2_INTERFACE_DEFINED__

/* interface ILMDSKernel2 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ILMDSKernel2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E2B7DDA8-38C5-11D5-91F6-00104BDB8FF9")
    ILMDSKernel2 : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnlockModule( 
            /* [in] */ BSTR Key,
            /* [in] */ long Flags,
            /* [in] */ BSTR AppID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LockModules( 
            /* [in] */ BSTR AppID,
            /* [in] */ long Flags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetModuleLockState( 
            /* [in] */ long lCategory,
            /* [in] */ long lModuleID,
            /* [out] */ long __RPC_FAR *State) = 0;
        
        virtual /* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE SetEvalExpiryDate( 
            /* [in] */ BSTR Key,
            /* [in] */ long lCategory,
            /* [in] */ long lModuleID,
            /* [in] */ long lYear,
            /* [in] */ long lMonth,
            /* [in] */ long lDay,
            /* [in] */ VARIANT_BOOL vbReset) = 0;
        
        virtual /* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE SetEvalExpiryDuration( 
            /* [in] */ BSTR Key,
            /* [in] */ long lCategory,
            /* [in] */ long lModuleID,
            /* [in] */ long lDays,
            /* [in] */ VARIANT_BOOL vbReset) = 0;
        
        virtual /* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE GetEvalExpiryDate( 
            /* [in] */ BSTR Key,
            /* [in] */ long lCategory,
            /* [in] */ long lModuleID,
            /* [out] */ long __RPC_FAR *plYear,
            /* [out] */ long __RPC_FAR *plMonth,
            /* [out] */ long __RPC_FAR *plDay) = 0;
        
        virtual /* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE GetModuleInfo( 
            /* [in] */ BSTR Key,
            /* [in] */ long lCategory,
            /* [in] */ long lModuleID,
            /* [out] */ long __RPC_FAR *plLastUsedYear,
            /* [out] */ long __RPC_FAR *plLastUsedMonth,
            /* [out] */ long __RPC_FAR *plLastUsedDay,
            /* [out] */ long __RPC_FAR *plExpiryYear,
            /* [out] */ long __RPC_FAR *plExpiryMonth,
            /* [out] */ long __RPC_FAR *plExpiryDay,
            /* [out] */ long __RPC_FAR *plReleaseCount,
            /* [out] */ long __RPC_FAR *plMMCount,
            /* [out] */ long __RPC_FAR *plModuleState) = 0;
        
        virtual /* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE LDSKernelService1( 
            /* [in] */ BSTR Key,
            /* [out] */ long __RPC_FAR *plParm1,
            /* [out] */ long __RPC_FAR *plParm2,
            /* [out] */ long __RPC_FAR *plParm3) = 0;
        
        virtual /* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE LDSKernelService3( 
            /* [in] */ BSTR Key,
            /* [out] */ long __RPC_FAR *plParm1,
            /* [out] */ long __RPC_FAR *plParm2,
            /* [out] */ long __RPC_FAR *plParm3) = 0;
        
        virtual /* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE LDSKernelService2( 
            /* [in] */ BSTR Key,
            /* [out] */ long __RPC_FAR *plParm1,
            /* [out] */ long __RPC_FAR *plParm2,
            /* [out] */ long __RPC_FAR *plParm3) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILMDSKernel2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILMDSKernel2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILMDSKernel2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnlockModule )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ BSTR Key,
            /* [in] */ long Flags,
            /* [in] */ BSTR AppID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LockModules )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ BSTR AppID,
            /* [in] */ long Flags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetModuleLockState )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ long lCategory,
            /* [in] */ long lModuleID,
            /* [out] */ long __RPC_FAR *State);
        
        /* [helpstring][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetEvalExpiryDate )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ BSTR Key,
            /* [in] */ long lCategory,
            /* [in] */ long lModuleID,
            /* [in] */ long lYear,
            /* [in] */ long lMonth,
            /* [in] */ long lDay,
            /* [in] */ VARIANT_BOOL vbReset);
        
        /* [helpstring][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetEvalExpiryDuration )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ BSTR Key,
            /* [in] */ long lCategory,
            /* [in] */ long lModuleID,
            /* [in] */ long lDays,
            /* [in] */ VARIANT_BOOL vbReset);
        
        /* [helpstring][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEvalExpiryDate )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ BSTR Key,
            /* [in] */ long lCategory,
            /* [in] */ long lModuleID,
            /* [out] */ long __RPC_FAR *plYear,
            /* [out] */ long __RPC_FAR *plMonth,
            /* [out] */ long __RPC_FAR *plDay);
        
        /* [helpstring][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetModuleInfo )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ BSTR Key,
            /* [in] */ long lCategory,
            /* [in] */ long lModuleID,
            /* [out] */ long __RPC_FAR *plLastUsedYear,
            /* [out] */ long __RPC_FAR *plLastUsedMonth,
            /* [out] */ long __RPC_FAR *plLastUsedDay,
            /* [out] */ long __RPC_FAR *plExpiryYear,
            /* [out] */ long __RPC_FAR *plExpiryMonth,
            /* [out] */ long __RPC_FAR *plExpiryDay,
            /* [out] */ long __RPC_FAR *plReleaseCount,
            /* [out] */ long __RPC_FAR *plMMCount,
            /* [out] */ long __RPC_FAR *plModuleState);
        
        /* [helpstring][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LDSKernelService1 )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ BSTR Key,
            /* [out] */ long __RPC_FAR *plParm1,
            /* [out] */ long __RPC_FAR *plParm2,
            /* [out] */ long __RPC_FAR *plParm3);
        
        /* [helpstring][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LDSKernelService3 )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ BSTR Key,
            /* [out] */ long __RPC_FAR *plParm1,
            /* [out] */ long __RPC_FAR *plParm2,
            /* [out] */ long __RPC_FAR *plParm3);
        
        /* [helpstring][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LDSKernelService2 )( 
            ILMDSKernel2 __RPC_FAR * This,
            /* [in] */ BSTR Key,
            /* [out] */ long __RPC_FAR *plParm1,
            /* [out] */ long __RPC_FAR *plParm2,
            /* [out] */ long __RPC_FAR *plParm3);
        
        END_INTERFACE
    } ILMDSKernel2Vtbl;

    interface ILMDSKernel2
    {
        CONST_VTBL struct ILMDSKernel2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILMDSKernel2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILMDSKernel2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILMDSKernel2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILMDSKernel2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ILMDSKernel2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ILMDSKernel2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ILMDSKernel2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ILMDSKernel2_UnlockModule(This,Key,Flags,AppID)	\
    (This)->lpVtbl -> UnlockModule(This,Key,Flags,AppID)

#define ILMDSKernel2_LockModules(This,AppID,Flags)	\
    (This)->lpVtbl -> LockModules(This,AppID,Flags)

#define ILMDSKernel2_GetModuleLockState(This,lCategory,lModuleID,State)	\
    (This)->lpVtbl -> GetModuleLockState(This,lCategory,lModuleID,State)

#define ILMDSKernel2_SetEvalExpiryDate(This,Key,lCategory,lModuleID,lYear,lMonth,lDay,vbReset)	\
    (This)->lpVtbl -> SetEvalExpiryDate(This,Key,lCategory,lModuleID,lYear,lMonth,lDay,vbReset)

#define ILMDSKernel2_SetEvalExpiryDuration(This,Key,lCategory,lModuleID,lDays,vbReset)	\
    (This)->lpVtbl -> SetEvalExpiryDuration(This,Key,lCategory,lModuleID,lDays,vbReset)

#define ILMDSKernel2_GetEvalExpiryDate(This,Key,lCategory,lModuleID,plYear,plMonth,plDay)	\
    (This)->lpVtbl -> GetEvalExpiryDate(This,Key,lCategory,lModuleID,plYear,plMonth,plDay)

#define ILMDSKernel2_GetModuleInfo(This,Key,lCategory,lModuleID,plLastUsedYear,plLastUsedMonth,plLastUsedDay,plExpiryYear,plExpiryMonth,plExpiryDay,plReleaseCount,plMMCount,plModuleState)	\
    (This)->lpVtbl -> GetModuleInfo(This,Key,lCategory,lModuleID,plLastUsedYear,plLastUsedMonth,plLastUsedDay,plExpiryYear,plExpiryMonth,plExpiryDay,plReleaseCount,plMMCount,plModuleState)

#define ILMDSKernel2_LDSKernelService1(This,Key,plParm1,plParm2,plParm3)	\
    (This)->lpVtbl -> LDSKernelService1(This,Key,plParm1,plParm2,plParm3)

#define ILMDSKernel2_LDSKernelService3(This,Key,plParm1,plParm2,plParm3)	\
    (This)->lpVtbl -> LDSKernelService3(This,Key,plParm1,plParm2,plParm3)

#define ILMDSKernel2_LDSKernelService2(This,Key,plParm1,plParm2,plParm3)	\
    (This)->lpVtbl -> LDSKernelService2(This,Key,plParm1,plParm2,plParm3)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILMDSKernel2_UnlockModule_Proxy( 
    ILMDSKernel2 __RPC_FAR * This,
    /* [in] */ BSTR Key,
    /* [in] */ long Flags,
    /* [in] */ BSTR AppID);


void __RPC_STUB ILMDSKernel2_UnlockModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILMDSKernel2_LockModules_Proxy( 
    ILMDSKernel2 __RPC_FAR * This,
    /* [in] */ BSTR AppID,
    /* [in] */ long Flags);


void __RPC_STUB ILMDSKernel2_LockModules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILMDSKernel2_GetModuleLockState_Proxy( 
    ILMDSKernel2 __RPC_FAR * This,
    /* [in] */ long lCategory,
    /* [in] */ long lModuleID,
    /* [out] */ long __RPC_FAR *State);


void __RPC_STUB ILMDSKernel2_GetModuleLockState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE ILMDSKernel2_SetEvalExpiryDate_Proxy( 
    ILMDSKernel2 __RPC_FAR * This,
    /* [in] */ BSTR Key,
    /* [in] */ long lCategory,
    /* [in] */ long lModuleID,
    /* [in] */ long lYear,
    /* [in] */ long lMonth,
    /* [in] */ long lDay,
    /* [in] */ VARIANT_BOOL vbReset);


void __RPC_STUB ILMDSKernel2_SetEvalExpiryDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE ILMDSKernel2_SetEvalExpiryDuration_Proxy( 
    ILMDSKernel2 __RPC_FAR * This,
    /* [in] */ BSTR Key,
    /* [in] */ long lCategory,
    /* [in] */ long lModuleID,
    /* [in] */ long lDays,
    /* [in] */ VARIANT_BOOL vbReset);


void __RPC_STUB ILMDSKernel2_SetEvalExpiryDuration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE ILMDSKernel2_GetEvalExpiryDate_Proxy( 
    ILMDSKernel2 __RPC_FAR * This,
    /* [in] */ BSTR Key,
    /* [in] */ long lCategory,
    /* [in] */ long lModuleID,
    /* [out] */ long __RPC_FAR *plYear,
    /* [out] */ long __RPC_FAR *plMonth,
    /* [out] */ long __RPC_FAR *plDay);


void __RPC_STUB ILMDSKernel2_GetEvalExpiryDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE ILMDSKernel2_GetModuleInfo_Proxy( 
    ILMDSKernel2 __RPC_FAR * This,
    /* [in] */ BSTR Key,
    /* [in] */ long lCategory,
    /* [in] */ long lModuleID,
    /* [out] */ long __RPC_FAR *plLastUsedYear,
    /* [out] */ long __RPC_FAR *plLastUsedMonth,
    /* [out] */ long __RPC_FAR *plLastUsedDay,
    /* [out] */ long __RPC_FAR *plExpiryYear,
    /* [out] */ long __RPC_FAR *plExpiryMonth,
    /* [out] */ long __RPC_FAR *plExpiryDay,
    /* [out] */ long __RPC_FAR *plReleaseCount,
    /* [out] */ long __RPC_FAR *plMMCount,
    /* [out] */ long __RPC_FAR *plModuleState);


void __RPC_STUB ILMDSKernel2_GetModuleInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE ILMDSKernel2_LDSKernelService1_Proxy( 
    ILMDSKernel2 __RPC_FAR * This,
    /* [in] */ BSTR Key,
    /* [out] */ long __RPC_FAR *plParm1,
    /* [out] */ long __RPC_FAR *plParm2,
    /* [out] */ long __RPC_FAR *plParm3);


void __RPC_STUB ILMDSKernel2_LDSKernelService1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE ILMDSKernel2_LDSKernelService3_Proxy( 
    ILMDSKernel2 __RPC_FAR * This,
    /* [in] */ BSTR Key,
    /* [out] */ long __RPC_FAR *plParm1,
    /* [out] */ long __RPC_FAR *plParm2,
    /* [out] */ long __RPC_FAR *plParm3);


void __RPC_STUB ILMDSKernel2_LDSKernelService3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE ILMDSKernel2_LDSKernelService2_Proxy( 
    ILMDSKernel2 __RPC_FAR * This,
    /* [in] */ BSTR Key,
    /* [out] */ long __RPC_FAR *plParm1,
    /* [out] */ long __RPC_FAR *plParm2,
    /* [out] */ long __RPC_FAR *plParm3);


void __RPC_STUB ILMDSKernel2_LDSKernelService2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILMDSKernel2_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_LMDSKernel2;

#ifdef __cplusplus

class DECLSPEC_UUID("E2B7DDA9-38C5-11D5-91F6-00104BDB8FF9")
LMDSKernel2;
#endif
#endif /* __LMDSKernelLib2_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

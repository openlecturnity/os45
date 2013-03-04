/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Aug 09 10:25:42 2006
 */
/* Compiler settings for D:\mm\dshowflt\ISO\mux\lmisomux.idl:
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

#ifndef __ILMIsoMux_h__
#define __ILMIsoMux_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ILMIsoMux_FWD_DEFINED__
#define __ILMIsoMux_FWD_DEFINED__
typedef interface ILMIsoMux ILMIsoMux;
#endif 	/* __ILMIsoMux_FWD_DEFINED__ */


#ifndef __LMIsoMux_FWD_DEFINED__
#define __LMIsoMux_FWD_DEFINED__

#ifdef __cplusplus
typedef class LMIsoMux LMIsoMux;
#else
typedef struct LMIsoMux LMIsoMux;
#endif /* __cplusplus */

#endif 	/* __LMIsoMux_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __LMIsoMuxLib_LIBRARY_DEFINED__
#define __LMIsoMuxLib_LIBRARY_DEFINED__

/* library LMIsoMuxLib */
/* [helpstring][version][uuid] */ 

#ifndef __LMISOMUX_H__
#define __LMISOMUX_H__
static const GUID CLSID_LMIsoMux             = {0x990d1978, 0xe48d, 0x43af, {0xb1, 0x2d, 0x24, 0xa7, 0x45, 0x6e, 0xc8, 0x9f}};
static const GUID LIBID_LMIsoMuxLib          = {0xE2B7DF22, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9}};
static const GUID IID_ILMIsoMux              = {0xE2B7DF23, 0x38c5, 0x11d5, {0x91, 0xf6, 0x00, 0x10, 0x4b, 0xdb, 0x8f, 0xf9}};
#endif 

//DEFINE_GUID(LIBID_LMIsoMuxLib,0xE2B7DF22,0x38C5,0x11D5,0x91,0xF6,0x00,0x10,0x4B,0xDB,0x8F,0xF9);

#ifndef __ILMIsoMux_INTERFACE_DEFINED__
#define __ILMIsoMux_INTERFACE_DEFINED__

/* interface ILMIsoMux */
/* [unique][helpstring][dual][uuid][object] */ 


//DEFINE_GUID(IID_ILMIsoMux,0xE2B7DF23,0x38C5,0x11D5,0x91,0xF6,0x00,0x10,0x4B,0xDB,0x8F,0xF9);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E2B7DF23-38C5-11D5-91F6-00104BDB8FF9")
    ILMIsoMux : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Title( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Title( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILMIsoMuxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILMIsoMux __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILMIsoMux __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILMIsoMux __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ILMIsoMux __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ILMIsoMux __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ILMIsoMux __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ILMIsoMux __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Title )( 
            ILMIsoMux __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Title )( 
            ILMIsoMux __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } ILMIsoMuxVtbl;

    interface ILMIsoMux
    {
        CONST_VTBL struct ILMIsoMuxVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILMIsoMux_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILMIsoMux_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILMIsoMux_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILMIsoMux_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ILMIsoMux_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ILMIsoMux_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ILMIsoMux_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ILMIsoMux_get_Title(This,pVal)	\
    (This)->lpVtbl -> get_Title(This,pVal)

#define ILMIsoMux_put_Title(This,newVal)	\
    (This)->lpVtbl -> put_Title(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILMIsoMux_get_Title_Proxy( 
    ILMIsoMux __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ILMIsoMux_get_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILMIsoMux_put_Title_Proxy( 
    ILMIsoMux __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ILMIsoMux_put_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILMIsoMux_INTERFACE_DEFINED__ */


//DEFINE_GUID(CLSID_LMIsoMux,0x990D1978,0xE48D,0x43af,0xB1,0x2D,0x24,0xA7,0x45,0x6E,0xC8,0x9F);

#ifdef __cplusplus

class DECLSPEC_UUID("990D1978-E48D-43af-B12D-24A7456EC89F")
LMIsoMux;
#endif
#endif /* __LMIsoMuxLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

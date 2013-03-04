// iohook.cpp : Definiert den Einsprungpunkt für die DLL-Anwendung.
//

#include "stdafx.h"
#include "iohook.h"
#include "stdio.h"
#include "TCHAR.h"

#pragma data_seg(".hook")


#pragma data_seg()

#pragma comment(linker, "/SECTION:.hook,RWS")


NMouseHook *NMouseHook::s_pMouseHookInstance = NULL;
HINSTANCE NMouseHook::s_hInstance = NULL;

/*
 * DLL Entry point
 */
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {

   NMouseHook::s_hInstance = (HINSTANCE)hModule;

   return TRUE;
}

/*
 * Initialization of hook procedures
 */
IOHOOK_API bool NMouseHook::InitMouseHook(IMouseHookListener *pListener) {
    // NOTE
    // Low level hooks are necessary to get all events, 
    // not only the events from the current thread.
    // And you somehow connect only to the current thread if specifying
    // a pointer to this thread; here with "pListener".
    // If this parameter is removed then also the normal mouse hook
    // gets all events.

    if (s_pMouseHookInstance == NULL)
        s_pMouseHookInstance = new NMouseHook();
    else
        s_pMouseHookInstance->Destroy();

    bool bSuccess = s_pMouseHookInstance->Init(pListener);

    return bSuccess;
}

IOHOOK_API void NMouseHook::DestroyMouseHook() {
  if (s_pMouseHookInstance != NULL)
      s_pMouseHookInstance->Destroy();

  delete s_pMouseHookInstance;
  s_pMouseHookInstance = NULL;

}

bool NMouseHook::Init(IMouseHookListener *pListener) {
    m_pListener = pListener;
    
    m_hKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProcLL, s_hInstance, NULL);
    m_hMouseHook = ::SetWindowsHookEx(WH_MOUSE_LL, MouseProcLL, s_hInstance, NULL);

    return (m_hMouseHook != NULL && m_hKeyboardHook != NULL);
}

void NMouseHook::Destroy() {
    if (m_hMouseHook != NULL)
        ::UnhookWindowsHookEx(m_hMouseHook);
    m_hMouseHook = NULL;

    if (m_hKeyboardHook != NULL)
        ::UnhookWindowsHookEx(m_hKeyboardHook);
    m_hKeyboardHook = NULL;
}

IOHOOK_API LRESULT CALLBACK NMouseHook::MouseProcLL(int nCode, WPARAM wParam, LPARAM lParam) {

    MOUSEHOOKSTRUCT *pMhs = (MOUSEHOOKSTRUCT *)lParam;

    IMouseHookListener *pListener = NULL;
    if (s_pMouseHookInstance != NULL)
        pListener = s_pMouseHookInstance->GetListener();

    if (nCode >= 0 && pListener != NULL) {
        POINT ptMouse;
        ptMouse.x = pMhs->pt.x;
        ptMouse.y = pMhs->pt.y;

        if (nCode >= 0) {
            switch (wParam) {
               case WM_MOUSEMOVE:
                   pListener->MouseMoved(ptMouse, 0);
                   break;
               case WM_LBUTTONDOWN:
               case WM_NCLBUTTONDOWN:
                   pListener->MouseDown(ptMouse, MH_LEFT_BUTTON);
                   break;
               case WM_LBUTTONUP:
                   pListener->MouseUp(ptMouse, MH_LEFT_BUTTON);
                   break;
               case WM_RBUTTONDOWN:
               case WM_NCRBUTTONDOWN:
                   pListener->MouseDown(ptMouse, MH_RIGHT_BUTTON);
                   break;
               case WM_RBUTTONUP:
                   pListener->MouseUp(ptMouse, MH_RIGHT_BUTTON);
                   break;
               case WM_MBUTTONDOWN:
               case WM_NCMBUTTONDOWN:
                   pListener->MouseDown(ptMouse, MH_MIDDLE_BUTTON);
                   break;
               case WM_MBUTTONUP:
                   pListener->MouseUp(ptMouse, MH_MIDDLE_BUTTON);
                   break;
            }
        }
    }

    return ::CallNextHookEx(NULL, nCode, wParam, lParam);
}

// KEYBOARD HOOK

IOHOOK_API LRESULT CALLBACK NMouseHook::KeyboardProcLL(int code, WPARAM wParam, LPARAM lParam) {

    KBDLLHOOKSTRUCT *pHookStruct = (KBDLLHOOKSTRUCT *)lParam;

    IMouseHookListener *pListener = NULL;
    if (s_pMouseHookInstance != NULL)
        pListener = s_pMouseHookInstance->GetListener();

    if (code >= 0 && pHookStruct != NULL && 
        (pHookStruct->vkCode == VK_CONTROL || 
         pHookStruct->vkCode == VK_LCONTROL || 
         pHookStruct->vkCode == VK_RCONTROL || 
         pHookStruct->vkCode == VK_SHIFT || 
         pHookStruct->vkCode == VK_LSHIFT || 
         pHookStruct->vkCode == VK_RSHIFT)) {

        bool bCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) > 0;
        bool bShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) > 0;

        bool bIsPanning = false;
        if (bShift && bCtrl)
            bIsPanning = true;

        if (pListener != NULL)
            pListener->SetPanningMode(bIsPanning);
    }

    return ::CallNextHookEx(NULL, code, wParam, lParam);
}
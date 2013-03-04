#ifndef __IOHOOK_DLL
#define __IOHOOK_DLL

// Der folgende ifdef-Block zeigt die Standardlösung zur Erstellung von Makros, die das Exportieren 
// aus einer DLL vereinfachen. Alle Dateien in dieser DLL wurden mit dem in der Befehlszeile definierten
// Symbol IOHOOK_EXPORTS kompiliert. Dieses Symbol sollte für kein Projekt definiert werden, das
// diese DLL verwendet. Auf diese Weise betrachtet jedes andere Projekt, dessen Quellcodedateien diese Datei 
// einbeziehen, IOHOOK_API-Funktionen als aus einer DLL importiert, während diese DLL mit diesem 
// Makro definierte Symbole als exportiert betrachtet.
#ifdef IOHOOK_EXPORTS
#define IOHOOK_API __declspec(dllexport)
#else
#define IOHOOK_API __declspec(dllimport)
#endif

#define MH_LEFT_BUTTON     0x00000001
#define MH_RIGHT_BUTTON    0x00000002
#define MH_MIDDLE_BUTTON   0x00000004
#define MH_BUTTON_DOWN     0x00000001
#define MH_BUTTON_UP       0x00000002
#define MH_BUTTON_DBLCLICK 0x00000004

class IMouseHookListener
{
public:
    IMouseHookListener() {}
    ~IMouseHookListener() {}

public:
    virtual void MouseMoved(POINT ptMouse, DWORD dwFlag) = 0;
    virtual void MouseDown(POINT ptMouse, DWORD dwFlag) = 0;
    virtual void MouseUp(POINT ptMouse, DWORD dwFlag) = 0;
    virtual void MouseDblClick(POINT ptMouse, DWORD dwFlag) = 0;
    virtual void SetPanningMode(bool bDoPanning) = 0;
};

class NMouseHook
{
public:
    NMouseHook() {}
    ~NMouseHook() {}

public:
   static IOHOOK_API bool InitMouseHook(IMouseHookListener *pListener);
   static IOHOOK_API void DestroyMouseHook();

   bool Init(IMouseHookListener *pListener);
   void Destroy();
   IMouseHookListener *GetListener() {return m_pListener;}

private:
   static IOHOOK_API LRESULT CALLBACK MouseProcLL(int nCode, WPARAM wParam, LPARAM lParam);
   static IOHOOK_API LRESULT CALLBACK KeyboardProcLL(int code, WPARAM wPara, LPARAM lParam);

public:
    static HINSTANCE s_hInstance;
    static NMouseHook *s_pMouseHookInstance;

    HHOOK m_hMouseHook;
    HHOOK m_hKeyboardHook;
    
    IMouseHookListener *m_pListener;
};


#endif

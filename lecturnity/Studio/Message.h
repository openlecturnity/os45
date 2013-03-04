#pragma once

class Message
{
public:
    Message(void);
    ~Message(void);

public:
    static void Show(UINT idMessage, UINT idCaption=0);
    static void Show(LPCTSTR tszMessage, LPCTSTR tszCaption=NULL);
    static void ShowError(UINT idMessage, UINT idCaption=0);
    static void ShowError(LPCTSTR tszMessage, LPCTSTR tszCaption=NULL);
    static void ShowWarning(UINT idMessage, UINT idCaption=0);
    static void ShowWarning(LPCTSTR tszMessage, LPCTSTR tszCaption=NULL);
    static void ShowInformation(UINT idMessage, UINT idCaption=0);
    static void ShowInformation(LPCTSTR tszMessage, LPCTSTR tszCaption=NULL);
};

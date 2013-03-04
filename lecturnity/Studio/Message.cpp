#include "StdAfx.h"
#include "Message.h"
#include "resource.h"

Message::Message(void)
{
}

Message::~Message(void)
{
}

void Message::Show(UINT idMessage, UINT idCaption)
{ 
    CString csCaption;
    if (idCaption != 0)
        csCaption.LoadString(idCaption);

    CString csMessage;
    csMessage.LoadString(idMessage);

    MessageBox(NULL, csMessage, csCaption, MB_OK | MB_TOPMOST);
}

void Message::Show(LPCTSTR tszMessage, LPCTSTR tszCaption)
{ 
    CString csCaption;
    if (tszCaption != NULL)
        csCaption = tszCaption;

    MessageBox(NULL, tszMessage, csCaption, MB_OK | MB_TOPMOST);
}


void Message::ShowError(UINT idMessage, UINT idCaption)
{ 
    CString csCaption;
    if (idCaption == 0)
        csCaption.LoadString(IDS_ERROR);
    else
        csCaption.LoadString(idCaption);

    CString csMessage;
    csMessage.LoadString(idMessage);

    MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONERROR | MB_TOPMOST);
}

void Message::ShowError(LPCTSTR tszMessage, LPCTSTR tszCaption)
{ 
    CString csCaption;
    if (tszCaption == NULL)
        csCaption.LoadString(IDS_ERROR);
    else 
        csCaption = tszCaption;

    MessageBox(NULL, tszMessage, csCaption, MB_OK | MB_ICONERROR | MB_TOPMOST);
}

void Message::ShowWarning(UINT idMessage, UINT idCaption)
{ 
    CString csCaption;
    if (idCaption == 0)
        csCaption.LoadString(IDS_WARNING);
    else
        csCaption.LoadString(idCaption);

    CString csMessage;
    csMessage.LoadString(idMessage);

    MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
}

void Message::ShowWarning(LPCTSTR tszMessage, LPCTSTR tszCaption)
{ 
    CString csCaption;
    if (tszCaption == NULL)
        csCaption.LoadString(IDS_WARNING);
    else 
        csCaption = tszCaption;

    MessageBox(NULL, tszMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
}

void Message::ShowInformation(UINT idMessage, UINT idCaption)
{ 
    CString csCaption;
    if (idCaption != 0)
        csCaption.LoadString(idCaption);

    CString csMessage;
    csMessage.LoadString(idMessage);

    MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
}

void Message::ShowInformation(LPCTSTR tszMessage, LPCTSTR tszCaption)
{ 
    CString csCaption;
    if (tszCaption != NULL)
        csCaption = tszCaption;

    MessageBox(NULL, tszMessage, csCaption, MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
}
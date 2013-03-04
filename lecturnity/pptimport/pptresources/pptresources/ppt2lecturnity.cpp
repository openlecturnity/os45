
#include <StdAfx.h>

#include "StartDialog.h"
#include "ConvWizard.h"
#include "MyProgressDlg.h"
#include "ResultDialog.h"
#include "LanguageSupport.h"
#include "lutils.h"       
#include "securityutils.h"

#pragma warning(disable: 4018 4244 4996)


struct ThreadStruct
{
   CEvent *pEvent;
   WCHAR *pSlideSelection;
   WCHAR *pLSDName;
   CString csPPTName;
   HWND *pHwnd;
   HWND *hParentWnd;
   int iSlideCount;
   int iCurrentSlide;
   short iRetVal;
   
};

//WCHAR **m_ppObjects = NULL;
//UINT m_nSize = 0;
CArray<CString, CString>g_aObjects;

UINT StartDialog(LPVOID pParam)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   ThreadStruct *pThreadStruct = (ThreadStruct *)pParam;

   int iSlideCount = pThreadStruct->iSlideCount;
   int iCurrentSlide = pThreadStruct->iCurrentSlide;

   CWnd *pParent = NULL;

   if (*pThreadStruct->hParentWnd != 0)
      pParent = CWnd::FromHandle(*pThreadStruct->hParentWnd);

   CConvWizard *pConvWiz = new CConvWizard(_T("Wizard"), pParent);

  
   pConvWiz->Init(pThreadStruct->csPPTName, iSlideCount, iCurrentSlide, pThreadStruct->pSlideSelection, pThreadStruct->pLSDName, pThreadStruct->pHwnd);

   if(pConvWiz->DoModal() != ID_WIZFINISH)
   {
	   *pThreadStruct->pHwnd = NULL;
	   pThreadStruct->iRetVal = 0;
	   pThreadStruct->pEvent->SetEvent();
	   return FALSE;
   }

   CMyProgressDlg *pMyProgress = new CMyProgressDlg(pParent);

   pMyProgress->Init(pThreadStruct->pHwnd, pThreadStruct->pEvent, true);

   pMyProgress->DoModal();
     
   
   if(pConvWiz)
	   delete(pConvWiz);
  
   if(pMyProgress)
	   delete(pMyProgress);
 
   return TRUE;
}


short __declspec(dllexport) CALLBACK  ShowImportWizard(const WCHAR *wszPPTName, int iSlideCount, int iCurrentSlide, 
                                                       WCHAR *wszSlideSelection, int *iSelectionLength, 
                                                       WCHAR *wszLSDName, int *iLSDNameLength, HWND *pHWnd, HWND *hParentWnd = NULL)
{
   short retVal = TRUE;
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   CEvent *pMyEvent = new CEvent();
   
   ThreadStruct threadStruct;
   threadStruct.pEvent = pMyEvent;
   threadStruct.iSlideCount = iSlideCount;
   threadStruct.iCurrentSlide = iCurrentSlide;
   threadStruct.csPPTName.SetString(wszPPTName);
   threadStruct.pLSDName = wszLSDName;
   threadStruct.pSlideSelection = wszSlideSelection;
   threadStruct.pHwnd = pHWnd;
   threadStruct.iRetVal = 1;
   threadStruct.hParentWnd = hParentWnd;

   CWinThread *thread = AfxBeginThread(StartDialog, &threadStruct, 0, 0, CREATE_SUSPENDED);
   thread->m_bAutoDelete = FALSE;
   thread->ResumeThread();

   DWORD ret = ::WaitForSingleObject(pMyEvent->m_hObject, INFINITE);

   if(threadStruct.iRetVal == 0)
   {
	   retVal = FALSE;
   }
   if(pMyEvent)
	   delete(pMyEvent);

   *iSelectionLength = wcslen(wszSlideSelection);
   *iLSDNameLength = wcslen(wszLSDName);

   return retVal;
}

short __declspec(dllexport) CALLBACK  ParseTitleMaster(HWND *pHWnd)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   HWND hWnd = (HWND)*pHWnd;

   WINDOWINFO wi;

   wi.cbSize = sizeof(WINDOWINFO);

   if(GetWindowInfo(hWnd, &wi) == 0)
   {
	   return FALSE;
   }

   if (hWnd)
   {
	   SendMessage(hWnd, WM_SELECT_CALL, CMyProgressDlg::TITLE_MASTER, 0); 
	   SendMessage(hWnd, WM_CHANGE_PROGRESS, 0, 0);
   }
   return TRUE;
}

short __declspec(dllexport) CALLBACK  ParseMaster(int iMasterCount, int iCurrentMaster, HWND *pHWnd)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   HWND hWnd = (HWND)*pHWnd;

   WINDOWINFO wi;

   wi.cbSize = sizeof(WINDOWINFO);

   if(GetWindowInfo(hWnd, &wi) == 0)
   {
	   return FALSE;
   }
  
   if (hWnd)
   {
	   SendMessage(hWnd, WM_SELECT_CALL, CMyProgressDlg::MASTER, 0);
	   SendMessage(hWnd, WM_CHANGE_PROGRESS, iMasterCount, iCurrentMaster);
   }
  

   return TRUE;
}

short __declspec(dllexport) CALLBACK  ImportTemplate(int iSlideCount, int iSlideNumber, HWND *pHWnd)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   HWND hWnd = (HWND)*pHWnd;

   WINDOWINFO wi;

   wi.cbSize = sizeof(WINDOWINFO);

   if(GetWindowInfo(hWnd, &wi) == 0)
   {
	   return FALSE;
   }
   
   if (hWnd)
   {
	   SendMessage(hWnd, WM_SELECT_CALL, CMyProgressDlg::TEMPLATE, 0);
	   SendMessage(hWnd, WM_CHANGE_PROGRESS, iSlideCount, iSlideNumber);
   }
   

   return TRUE;
}

short __declspec(dllexport) CALLBACK  ImportSlide(int iSlideCount, int iSlideNumber, HWND *pHWnd)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   HWND hWnd = (HWND)*pHWnd;

   WINDOWINFO wi;

   wi.cbSize = sizeof(WINDOWINFO);

   if(GetWindowInfo(hWnd, &wi) == 0)
   {
	   return FALSE;
   }
  
   if (hWnd)
   {
	   SendMessage(hWnd, WM_SELECT_CALL, CMyProgressDlg::SLIDE, 0);
	   SendMessage(hWnd, WM_CHANGE_PROGRESS, iSlideCount, iSlideNumber);
   }
 

   return TRUE;
}

short __declspec(dllexport) CALLBACK  EndProgressDialog(HWND *pHWnd)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   HWND hWnd = (HWND)*pHWnd;

   WINDOWINFO wi;

   wi.cbSize = sizeof(WINDOWINFO);

   if(GetWindowInfo(hWnd, &wi) == 0)
   {
	   return FALSE;
   }
  
   if (hWnd)
   {
	   SendMessage(hWnd, WM_END_PROGRESS, 0, 0);
   }
 

   return TRUE;
}

short __declspec(dllexport) CALLBACK  TimerUpdate(WCHAR *wszTimeLeft, HWND *pHWnd)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   HWND hWnd = (HWND)*pHWnd;

   WINDOWINFO wi;

   wi.cbSize = sizeof(WINDOWINFO);

   if(GetWindowInfo(hWnd, &wi) == 0)
   {
	   return FALSE;
   }
   
   if (hWnd)
   {
	   CString s;
	   s.SetString(wszTimeLeft);
	   SendMessage(hWnd, WM_CHANGE_PROG_TEXT, 0, (LPARAM)&s );
   }


   return TRUE;
}

short __declspec(dllexport) CALLBACK  GetMainCaption(WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.LoadString(IDS_DIALOG_TITLE);
	wcscpy(wszErrorMessage, temp.GetBuffer());
	return 0;
}

short __declspec(dllexport) CALLBACK  GetOpenLSDFileErrorMsg(WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.LoadString(ERR_CANNOT_OPEN_LSD);
	wcscpy(wszErrorMessage, temp.GetBuffer());
	return 0;
}

short __declspec(dllexport) CALLBACK  GetDiskFullMsg(WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.LoadString(ERR_DISK_FULL);
	wcscpy(wszErrorMessage, temp.GetBuffer());
	return 0;
}

short __declspec(dllexport) CALLBACK  GetOverwriteImageMsg(WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.LoadString(ERR_OVERWRITE_IMAGE);
	wcscpy(wszErrorMessage, temp.GetBuffer());
	return 0;
}

short __declspec(dllexport) CALLBACK  GetNoFileMsg(WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.LoadString(ERR_NO_OPENED_PRES);
	wcscpy(wszErrorMessage, temp.GetBuffer());
	return 0;
}

short __declspec(dllexport) CALLBACK  GetOffice2007Msg(WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.LoadString(IDS_OFFICE2007_WARNING);
	wcscpy(wszErrorMessage, temp.GetBuffer());
	return 0;
}

short __declspec(dllexport) CALLBACK  GetIllegalCharacterMsg(WCHAR *wszFilename, WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.Format(IDS_ILLEGALCHARACTER_WARNING, wszFilename);
	wcsncpy(wszErrorMessage, temp.GetBuffer(), 256);

	return 0;
}

short __declspec(dllexport) CALLBACK  GetMenuSaveAsLSD(WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.LoadString(MNU_SAVEASLSD);
	wcscpy(wszErrorMessage, temp.GetBuffer());
	return 0;
}

short __declspec(dllexport) CALLBACK  GetMenuOpenInAssistant(WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.LoadString(MNU_OPENINASSISTANT);
	wcscpy(wszErrorMessage, temp.GetBuffer());
	return 0;
}

short __declspec(dllexport) CALLBACK  GetReadOnlyErrorMessage(WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.LoadString(ERR_DOCUMENT_READONLY);
	wcscpy(wszErrorMessage, temp.GetBuffer());
	return 0;
}

short __declspec(dllexport) CALLBACK  GetPresentationIsFinalErrorMessage(WCHAR *wszErrorMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	CString temp;
	temp.LoadString(ERR_DOCUMENT_FINALMODE);
	wcscpy(wszErrorMessage, temp.GetBuffer());
	return 0;
}

short __declspec(dllexport) CALLBACK  GetSourceDirectory(WCHAR *wszSourceDirectory, int *iStringLength)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

   CString temp;
   int pathLength = 0;
   
	CSecurityUtils su;
	CString csLecHome;
	su.GetLecturnityHome(csLecHome);

   temp = csLecHome;
   if (!temp.IsEmpty())
      temp += _T("\\Source Documents");
   wcscpy(wszSourceDirectory, temp.GetBuffer());

   *iStringLength = temp.GetLength();

   return 0;
}

short __declspec(dllexport) CALLBACK  InsertRestrictedObject(WCHAR *wszObjectEntry, int nSize)
{
	/*if(!m_ppObjects)
	{
		m_ppObjects = new WCHAR*[nSize];
		for (int i = 0; i < nSize; ++i)
			m_ppObjects[i] = new WCHAR[wcslen(wszObjectEntry)];
	}
	
	wcscpy(m_ppObjects[m_nSize], wszObjectEntry);
	m_nSize ++;*/


	CString csTmp;
	csTmp.SetString(wszObjectEntry);
	g_aObjects.Add(csTmp);
		
	return 0;
}



short __declspec(dllexport) CALLBACK  ShowResultDialog(WCHAR *wszLSDName, short bPPTCall, short bDoRecord, HWND *pHWnd, HWND *pHWndParent)
{
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = CLanguageSupport::SetThreadLanguage();
	
	if(pHWnd != NULL)
	{
		HWND hWnd = (HWND)*pHWnd;


		if (hWnd)
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}

    CWnd *pWndParent = AfxGetApp()->GetMainWnd();
    if (pHWndParent != NULL) {
		HWND hWndParent = (HWND)*pHWndParent;
        pWndParent = CWnd::FromHandle(hWndParent);
    }

	CResultDialog *resultDlg = new CResultDialog(pWndParent); 

	if(!bPPTCall)
	{
		resultDlg->SetAssistantCall();
	}

	//if(m_ppObjects != NULL)
		//resultDlg->InsertObjects(m_ppObjects, m_nSize);
	if(g_aObjects.GetSize() != 0)
		resultDlg->InsertObjects(g_aObjects);

    CString csLSDName = wszLSDName;
	resultDlg->SetPath(csLSDName);
    resultDlg->SetDoRecord(bDoRecord);

	int res = resultDlg->DoModal();
	
   /*if (m_ppObjects != NULL)
   {
	   for(int i = 0; i < m_nSize; i++)
		   delete m_ppObjects[i];
	   delete[] m_ppObjects;
   }*/
  /* m_ppObjects = NULL;
   m_nSize = 0;*/

   g_aObjects.RemoveAll();

   if(resultDlg)
	   delete(resultDlg);

   if(res == IDCANCEL)
	   return FALSE;

	return TRUE;
}

UINT StartProgressDialog(LPVOID pParam)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   ThreadStruct *pThStruct = (ThreadStruct*)pParam;

   CMyProgressDlg *pMyProgress;
   CWnd *pParent = NULL;
   if (*pThStruct->hParentWnd != 0)
      pParent = CWnd::FromHandle(*pThStruct->hParentWnd);

   pMyProgress = new CMyProgressDlg(pParent);
   pMyProgress->Init(pThStruct->pHwnd, pThStruct->pEvent);

   pMyProgress->DoModal();

   if(pMyProgress)
      delete(pMyProgress);

   return 0;
}

short __declspec(dllexport) CALLBACK  ShowProgressDialog(HWND *pHWnd, HWND *hParentWnd = NULL)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CEvent *pEvent = new CEvent();

	ThreadStruct thStruct;
	thStruct.pEvent = pEvent;
	thStruct.pHwnd = pHWnd;
	thStruct.hParentWnd = hParentWnd;

	CWinThread *thread = AfxBeginThread(StartProgressDialog, &thStruct, 0, 0, CREATE_SUSPENDED);
	thread->m_bAutoDelete = FALSE;
	thread->ResumeThread();

	DWORD ret = ::WaitForSingleObject(pEvent->m_hObject, INFINITE);

	return TRUE;
}


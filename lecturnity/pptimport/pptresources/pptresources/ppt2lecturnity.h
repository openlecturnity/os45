short __declspec(dllexport) CALLBACK  ShowImportWizard(const WCHAR *wszPPTName, int iSlideCount, int iCurrentSlide, WCHAR *iSlideSelection, int *iSelectionLength, WCHAR *wszLSDName, int *iLSDNameLength, HWND *pHWnd);
short __declspec(dllexport) CALLBACK  ParseTitleMaster(HWND *pHWnd);
short __declspec(dllexport) CALLBACK  ParseMaster(int iMasterCount, int iCurrentMaster, HWND *pHWnd);
short __declspec(dllexport) CALLBACK  ImportTemplate(int iSlideCount, int iSlideNumber, HWND *pHWnd);
short __declspec(dllexport) CALLBACK  ImportSlide(int iSlideCount, int iSlideNumber, HWND *pHWnd);
short __declspec(dllexport) CALLBACK  TimerUpdate(WCHAR *wszTimeLeft, HWND *pHWnd);
short __declspec(dllexport) CALLBACK  GetMainCaption(WCHAR *wszErrorMessage);
short __declspec(dllexport) CALLBACK  GetOpenLSDFileErrorMsg(WCHAR *wszErrorMessage);
short __declspec(dllexport) CALLBACK  GetDiskFullMsg(WCHAR *wszErrorMessage);
short __declspec(dllexport) CALLBACK  GetOverwriteImageMsg(WCHAR *wszErrorMessage);
short __declspec(dllexport) CALLBACK  GetNoFileMsg(WCHAR *wszErrorMessage);
short __declspec(dllexport) CALLBACK  GetMenuSaveAsLSD(WCHAR *wszErrorMessage);
short __declspec(dllexport) CALLBACK  GetMenuOpenInAssistant(WCHAR *wszErrorMessage);
short __declspec(dllexport) CALLBACK  GetReadOnlyErrorMessage(WCHAR *wszErrorMessage);
short __declspec(dllexport) CALLBACK  GetPresentationIsFinalErrorMessage(WCHAR *wszErrorMessage);
short __declspec(dllexport) CALLBACK  ShowResultDialog(WCHAR *wszLSDName, short bPPTCall = true, short bDoRecord = false, HWND *pHWnd = NULL, HWND *pHWndParent = NULL);
short __declspec(dllexport) CALLBACK  InsertRestrictedObject(WCHAR *wszObjectEntry, int nSize);
short __declspec(dllexport) CALLBACK  ShowProgressDialog(HWND *pHWnd);


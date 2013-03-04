Attribute VB_Name = "ExternFunctions"
Option Explicit

'-----------------
' Win32 APIs
'-----------------
  
Declare Function GetActiveWindow Lib "user32" () As Long

Declare Sub GetLocalTime Lib "kernel32" (lpSystem As SYSTEMTIME)

Declare Function ShellExecute Lib "shell32.dll" Alias "ShellExecuteW" _
        (ByVal hWnd As Long, _
         ByVal lpOperation As Long, _
         ByVal lpFile As Long, _
         ByVal lpParameters As Long, _
         ByVal lpDirectory As Long, _
         ByVal nShowCmd As Long) As Long

Public Type SHITEMID
    cb      As Long
    abID    As Byte
End Type
Public Type ITEMIDLIST
    mkid    As SHITEMID
End Type
Public Declare Function SHGetSpecialFolderLocation Lib "shell32.dll" _
        (ByVal hwndOwner As Long, _
         ByVal nFolder As Long, _
         pidl As ITEMIDLIST) As Long
  
Public Declare Function SHGetPathFromIDList Lib "shell32.dll" Alias "SHGetPathFromIDListW" _
        (ByVal pidl As Long, _
         ByVal pszPath As Long) As Long
                         
' Use LockWindowUpdate to prevent/enable window refresh
Public Declare Function LockWindowUpdate Lib "user32" _
        (ByVal hwndLock As Long) As Long
                      
' Use UpdateWindow to force a refresh of the PowerPoint window
Public Declare Function UpdateWindow Lib "user32" (ByVal hWnd As Long) As Long

' Use LoadLibrary to load gdiplus.dll before running ppt2lecturnity.dll
Public Declare Function LoadLibrary Lib "kernel32" Alias "LoadLibraryW" _
        (ByVal strPath As Long) As Long
                      
Public Declare Function FreeLibrary Lib "kernel32" _
        (ByVal gdipHandle As Long) As Long
    
    
' MessageBox
Public Const IDOK As Long = 1
Public Const IDCANCEL As Long = 2
Public Const DABORT As Long = 3
Public Const IDRETRY As Long = 4
Public Const IDIGNORE As Long = 5
Public Const IDYES As Long = 6
Public Const IDNO As Long = 7

Public Const MB_ICONERROR As Long = &H10&
Public Const MB_ICONQUESTION As Long = &H20&
Public Const MB_ICONEXCLAMATION As Long = &H30&
Public Const MB_ICONINFORMATION As Long = &H40&
Public Const MB_TASKMODAL As Long = &H2000&
Public Const MB_YESNOCANCEL As Long = &H3&
Public Const MB_YESNO As Long = &H4&
Public Const MB_TOPMOST As Long = &H40000

Declare Function MessageBoxW Lib "user32.dll" _
        (ByVal hWnd As Long, _
         ByVal lpText As Long, _
         ByVal lpCaption As Long, _
         ByVal uType As Long) As Long
                          


' registry functions

Public Const HKEY_CLASSES_ROOT = &H80000000
Public Const HKEY_CURRENT_USER = &H80000001
Public Const HKEY_LOCAL_MACHINE = &H80000002
Public Const HKEY_USERS = &H80000003

Public Const READ_CONTROL = &H20000
Public Const STANDARD_RIGHTS_READ = (READ_CONTROL)
Public Const STANDARD_RIGHTS_ALL = &H1F0000
Public Const KEY_QUERY_VALUE = &H1
Public Const KEY_SET_VALUE = &H2
Public Const KEY_CREATE_SUB_KEY = &H4
Public Const KEY_CREATE_LINK = &H20
Public Const KEY_ENUMERATE_SUB_KEYS = &H8
Public Const KEY_NOTIFY = &H10
Public Const SYNCHRONIZE = &H100000

Public Const KEY_READ = ((STANDARD_RIGHTS_READ Or KEY_QUERY_VALUE Or KEY_ENUMERATE_SUB_KEYS Or KEY_NOTIFY) And (Not SYNCHRONIZE))
Public Const KEY_ALL_ACCESS = ((STANDARD_RIGHTS_ALL Or KEY_QUERY_VALUE Or KEY_SET_VALUE Or KEY_CREATE_SUB_KEY Or KEY_ENUMERATE_SUB_KEYS Or KEY_NOTIFY Or KEY_CREATE_LINK) And (Not SYNCHRONIZE))

Public Const ERROR_SUCCESS = &H0

Public Const REG_SZ = &H1
Public Const REG_BINARY = &H3
Public Const REG_DWORD = &H4

Public Declare Function RegOpenKeyEx Lib "advapi32.dll" Alias "RegOpenKeyExA" _
        (ByVal hKey As Long, _
         ByVal lpSubKey As String, _
         ByVal ulOptions As Long, _
         ByVal samDesired As Long, _
         phkResult As Long) As Long

Public Declare Function RegQueryValueEx Lib "advapi32.dll" Alias "RegQueryValueExA" _
        (ByVal hKey As Long, _
         ByVal lpValueName As String, _
         ByVal lpReserved As Long, _
         lpType As Long, _
         lpData As Any, _
         lpcbData As Long) As Long
     
Public Declare Function RegSetValueEx Lib "advapi32.dll" Alias "RegSetValueExA" _
        (ByVal hKey As Long, _
         ByVal lpValueName As String, _
         ByVal lpReserved As Long, _
         ByVal dwType As Long, _
         ByVal lpData As String, _
         ByVal cbData As Long) As Long

Public Declare Function RegCloseKey Lib "advapi32.dll" (ByVal hKey As Long) As Long


'--------------------
' code page functions
'--------------------
   
Public Const CP_ACP = 0
Public Const MB_PRECOMPOSED = &H1
Public Declare Function MultiByteToWideChar Lib "kernel32.dll" _
        (ByVal CodePage As Long, _
         ByVal dwFlags As Long, _
         lpMultiByteStr As Any, _
         ByVal cbMultiByte As Long, _
         lpWideCharStr As Any, _
         ByVal cchWideChar As Long) As Long
     
Public Declare Function WideCharToMultiByte Lib "kernel32.dll" _
        (ByVal CodePage As Long, _
         ByVal dwFlags As Long, _
         lpWideCharStr As Any, _
         ByVal cchWideChar As Long, _
         lpMultiByteStr As Any, _
         ByVal cbMultiByte As Long, _
         lpDefaultChar As Any, _
         lpUsedDefaultChar As Long) As Long
     
Public Declare Function GetACP Lib "kernel32.dll" () As Long
    
                 
'--------------------
' ppt2lecturnity functions
'--------------------

' Convert WMF files to PNG
Declare Function ConvertWMFtoPNG Lib "ppt2lecturnity.dll" _
        (ByVal WMFname As Long, _
         ByVal PNGname As Long, _
         ByVal bDelTrans As Integer) As Integer
 
Declare Function ConvertMetafile Lib "ppt2lecturnity.dll" _
        (ByVal szImagePath As Long, _
         ByVal szImageName As Long, _
         ByVal szImagePrefix As Long, _
         ByVal szImageSuffix As Long, _
         strLength As Long) As Boolean
                       
Declare Function CheckMetafile Lib "ppt2lecturnity.dll" _
        (ByVal szImagePath As Long) As Boolean
                       
Declare Function SaveMetafileAsPNG Lib "ppt2lecturnity.dll" _
        (ByVal szImagePath As Long, _
         ByVal szImageName As Long, _
         ByVal szImagePrefix As Long, _
         ByVal szImageSuffix As Long, _
         strLength As Long, _
         ByVal lHorizontalResolution As Long, _
         ByVal lVerticalResolution As Long, _
         x As Long, _
         y As Long, _
         width As Long, _
         heigth As Long) As Boolean
         
Declare Function SaveMetafileAsPNGWithoutGrid Lib "ppt2lecturnity.dll" _
        (ByVal szImagePath As Long, _
         ByVal szImageName As Long, _
         ByVal szImagePrefix As Long, _
         ByVal szImageSuffix As Long, _
         strLength As Long, _
         ByVal lHorizontalResolution As Long, _
         ByVal lVerticalResolution As Long, _
         x As Long, _
         y As Long, _
         width As Long, _
         heigth As Long) As Boolean
         
Declare Function GetImageDimension Lib "ppt2lecturnity.dll" _
        (ByVal szImagePath As Long, _
         ByVal szImagePrefix As Long, _
         ByVal szImageSuffix As Long, _
         ByVal lHorizontalResolution As Long, _
         ByVal lVerticalResolution As Long, _
         x As Long, _
         y As Long, _
         width As Long, _
         heigth As Long) As Boolean
         
Declare Function GetEMFDimension Lib "ppt2lecturnity.dll" _
        (ByVal szFullImageName As Long, _
         ByVal lHorizontalResolution As Long, _
         ByVal lVerticalResolution As Long, _
         x As Long, _
         y As Long, _
         width As Long, _
         heigth As Long) As Boolean
               
Declare Function RewritePng Lib "ppt2lecturnity.dll" _
        (ByVal szImagePath As Long, _
         ByVal szImageName As Long, _
         ByVal szImagePrefix As Long, _
         ByVal szImageSuffix As Long, _
         strLength As Long) As Boolean
                
Declare Function ExtractFontFamily Lib "ppt2lecturnity.dll" _
        (ByVal szImagePath As Long, _
         ByVal szFontFamily As Long, _
         strLength As Long) As Boolean
                   
Declare Function FontSupportsString Lib "ppt2lecturnity.dll" _
        (ByVal lpctString As Long, _
         ByVal iLength As Long, _
         ByVal lpctFontFamily As Long, _
         ByVal isBold As Long, _
         ByVal isItalic As Long) As Long
                       
Declare Function GetFontMetrics Lib "ppt2lecturnity.dll" _
        (ByVal lpctFontFamily As Long, _
         ByVal iFontSize As Long, _
         ByVal isBold As Long, _
         ByVal isItalic As Long, _
         iAscent As Double, _
         iDescent As Double) As Long
                            
                       
Declare Function StartGdiplus Lib "ppt2lecturnity.dll" _
        () As Boolean
Declare Function ShutdownGdiplus Lib "ppt2lecturnity.dll" _
        () As Boolean
              
      
'--------------------
' pptresources
'--------------------
   
Declare Function GetMainCaption Lib "pptresources.dll" _
        (ByVal szMessage As Long) As Integer
    
Declare Function GetOpenLSDFileErrorMsg Lib "pptresources.dll" _
        (ByVal szMessage As Long) As Integer

Declare Function GetDiskFullMsg Lib "pptresources.dll" _
        (ByVal szMessage As Long) As Integer

Declare Function GetOverwriteImageMsg Lib "pptresources.dll" _
        (ByVal szMessage As Long) As Integer
    
Declare Function GetNoFileMsg Lib "pptresources.dll" _
        (ByVal szMessage As Long) As Integer
    
Declare Function GetOffice2007Msg Lib "pptresources.dll" _
        (ByVal szMessage As Long) As Integer
        
Declare Function GetIllegalCharacterMsg Lib "pptresources.dll" _
        (ByVal szFilename As Long, ByVal szMessage As Long) As Integer

Declare Function GetMenuSaveAsLSD Lib "pptresources.dll" _
        (ByVal szMessage As Long) As Integer
    
Declare Function GetMenuOpenInAssistant Lib "pptresources.dll" _
        (ByVal szMessage As Long) As Integer
        
Declare Function GetReadOnlyErrorMessage Lib "pptresources.dll" _
        (ByVal szMessage As Long) As Integer
        
Declare Function GetPresentationIsFinalErrorMessage Lib "pptresources.dll" _
        (ByVal szMessage As Long) As Integer
    
Declare Function GetSourceDirectory Lib "pptresources.dll" _
        (ByVal szSourceDirectroy As Long, strLength As Long) As Integer

Declare Function ShowImportWizard Lib "pptresources.dll" _
        (ByVal lpctPPTName As Long, _
         ByVal iSlideCount As Long, _
         ByVal iCurrentSlide As Long, _
         ByVal lpctSlideSelection As Long, _
         iSelectionLength As Long, _
         ByVal lpctLSDName As Long, _
         iLSDNameLength As Long, _
         hWnd As Long, _
         hParentWnd As Long) As Boolean
                       
Declare Function ParseTitleMaster Lib "pptresources.dll" _
        (hWnd As Long) As Boolean
                      
Declare Function ParseMaster Lib "pptresources.dll" _
        (ByVal iMasterCount As Long, _
         ByVal iCurrentMaster As Long, _
         hWnd As Long) As Boolean
                      
Declare Function ImportTemplate Lib "pptresources.dll" _
        (ByVal iSlideCount As Long, _
         ByVal iCurrentSlide As Long, _
         hWnd As Long) As Boolean
                       
Declare Function ImportSlide Lib "pptresources.dll" _
        (ByVal iSlideCount As Long, _
         ByVal iCurrentSlide As Long, _
         hWnd As Long) As Boolean
         
Declare Function EndProgressDialog Lib "pptresources.dll" _
        (hWnd As Long) As Boolean
                       
Declare Function TimerUpdate Lib "pptresources.dll" _
        (ByVal szTimeLeft As Long, _
         hWnd As Long) As Boolean
         
Declare Function InsertRestrictedObject Lib "pptresources.dll" _
        (ByVal szObjectEntry As Long, _
         ByVal nSize As Long) As Boolean

Declare Function ShowResultDialog Lib "pptresources.dll" _
        (ByVal szLsdName As Long, _
         ByVal bQuiet As Integer, _
         ByVal bDoRecord As Integer, _
         hWnd As Long, _
         hWndParent As Long) As Boolean
                       
Declare Function ShowProgressDialog Lib "pptresources.dll" _
        (hWnd As Long, _
        hParentWnd As Long) As Boolean
                        


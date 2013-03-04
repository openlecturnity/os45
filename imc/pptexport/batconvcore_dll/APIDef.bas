Attribute VB_Name = "APIDef"
Option Explicit

'Requires Windows NT 3.1 or later; Requires Windows 95 or later
Public Declare Function FindWindow Lib "user32" Alias "FindWindowA" _
        (ByVal lpClassName As String, _
         ByVal lpWindowName As Long) As Long

Public Declare Function GetCurrentDirectory Lib "kernel32" Alias "GetCurrentDirectoryA" _
        (ByVal nBufferLength As Long, _
         ByVal lpBuffer As String) As Long

Public Declare Function GetFileTitle Lib "comdlg32.dll" Alias "GetFileTitleA" _
        (ByVal lpszFile As String, _
         ByVal lpszTitle As String, _
         ByVal cbBuf As Integer) As Integer

Public Declare Sub CoTaskMemFree Lib "ole32.dll" (ByVal hMem As Long)

Public Declare Function lstrcat Lib "kernel32" Alias "lstrcatA" _
        (ByVal lpString1 As String, _
         ByVal lpString2 As String) As Long

Public Declare Function GetFullPathName Lib "kernel32" Alias "GetFullPathNameA" _
        (ByVal lpFileName As String, _
         ByVal nBufferLength As Long, _
         ByVal lpBuffer As String, _
         ByVal lpFilePart As String) As Long

'Requires Windows NT 4.0 or later; Requires Windows 95 or later
Public Const BIF_RETURNONLYFSDIRS = 1
Public Const MAX_PATH = 260
Public Type BrowseInfo
     hwndOwner As Long
     pIDLRoot As Long
     pszDisplayName As Long
     lpszTitle As Long
     ulFlags As Long
     lpfnCallback As Long
     lParam As Long
     iImage As Long
End Type
Public Declare Function SHBrowseForFolder Lib "shell32" (lpbi As BrowseInfo) As Long

Public Declare Function SHGetPathFromIDList Lib "shell32" _
        (ByVal pidList As Long, _
         ByVal lpBuffer As String) As Long

'Requires Windows 2000 (or Windows NT 4.0 with Internet Explorer 4.0 or later);
'Requires Windows 98 (or Windows 95 with Internet Explorer 4.0 or later)
Public Declare Function PathCombine Lib "shlwapi.dll" Alias "PathCombineA" _
        (ByVal szDest As String, _
         ByVal lpszDir As String, _
         ByVal lpszFile As String) As Long

Public Function BrowseForFolder(hwndOwner As Long, sPrompt As String) As String
     Dim iNull As Integer
     Dim lpIDList As Long
     Dim lResult As Long
     Dim spath As String
     Dim udtBI As BrowseInfo
    ' init
     With udtBI
        .hwndOwner = hwndOwner
        .lpszTitle = lstrcat(sPrompt, "")
        .ulFlags = BIF_RETURNONLYFSDIRS
     End With
    ' API
     lpIDList = SHBrowseForFolder(udtBI)
     If lpIDList Then
        spath = String$(MAX_PATH, 0)
        lResult = SHGetPathFromIDList(lpIDList, spath)
        Call CoTaskMemFree(lpIDList)
        iNull = InStr(spath, vbNullChar)
        If iNull Then spath = Left$(spath, iNull - 1)
     End If
    ' sPath = "" if cancelled.
     BrowseForFolder = spath
End Function


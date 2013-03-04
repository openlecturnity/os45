Attribute VB_Name = "Registry"
Option Explicit

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


Public Function GetLecturnityHome() As String

    On Error GoTo ErrQuit
    
    Dim retOnError As String            '= "";
    
    Dim dwType As Long
    Dim dwBufSize As Long
    Dim regErr As Long
    
    Dim i As Long
    Dim binData As String
    
    Dim hKeyCurrentUser As Long
    
    'initialization
    dwBufSize = 0
    retOnError = ""
    
    hKeyCurrentUser = 0
    
    regErr = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\imc AG\LECTURNITY", 0, KEY_READ, hKeyCurrentUser)
    
    If (regErr <> ERROR_SUCCESS) Then
        GetLecturnityHome = retOnError
        Exit Function
    End If
    
    ' get buffer size
    regErr = RegQueryValueEx(hKeyCurrentUser, "LecturnityHome", 0, dwType, ByVal vbNullString, dwBufSize)
    
    If (regErr <> ERROR_SUCCESS) Then
        RegCloseKey (hKeyCurrentUser)
        GetLecturnityHome = retOnError
        Exit Function
    End If

    If (dwType <> REG_SZ) Then
        RegCloseKey (hKeyCurrentUser)
        GetLecturnityHome = retOnError
        Exit Function
    End If

    binData = Space(dwBufSize + 8)
    
    If IsNull(binData) Then
        RegCloseKey (hKeyCurrentUser)
        GetLecturnityHome = retOnError
        Exit Function
    End If

    regErr = RegQueryValueEx(hKeyCurrentUser, "LecturnityHome", 0, dwType, ByVal binData, dwBufSize)

    If (regErr <> ERROR_SUCCESS) Then
        'Erase binData
        RegCloseKey (hKeyCurrentUser)
        GetLecturnityHome = retOnError
        Exit Function
    End If
    
    RegCloseKey (hKeyCurrentUser)
    
    GetLecturnityHome = Left(binData, dwBufSize - 1)
    
    Exit Function

ErrQuit:
    If hKeyCurrentUser <> 0 Then
        RegCloseKey (hKeyCurrentUser)
    End If
    
    GetLecturnityHome = retOnError
    Exit Function

End Function



Public Function SetLecturnityHome(ByVal szLecturnityHome As String) As Boolean

    On Error GoTo ErrQuit
    
    Dim retOnError As Boolean: retOnError = False
    Dim hKeyCurrentUser As Long: hKeyCurrentUser = 0
    Dim regErr As Long
    
    regErr = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\imc AG\LECTURNITY", 0, KEY_ALL_ACCESS, hKeyCurrentUser)
    
    If (regErr <> ERROR_SUCCESS) Then
        SetLecturnityHome = retOnError
        Exit Function
    End If
    
    ' get buffer size
    regErr = RegSetValueEx(hKeyCurrentUser, "LecturnityHome", 0, REG_SZ, szLecturnityHome, Len(szLecturnityHome))
    
    If (regErr <> ERROR_SUCCESS) Then
        RegCloseKey (hKeyCurrentUser)
        SetLecturnityHome = retOnError
        Exit Function
    End If


    SetLecturnityHome = True
    Exit Function

ErrQuit:
    If hKeyCurrentUser <> 0 Then
        RegCloseKey (hKeyCurrentUser)
    End If
    
    SetLecturnityHome = retOnError
    Exit Function

End Function


Public Function SetShowSummary(ByVal lShowSummary As Long) As Boolean

    On Error GoTo ErrQuit
    
    Dim retOnError As Boolean: retOnError = False
    Dim hKeyCurrentUser As Long: hKeyCurrentUser = 0
    Dim regErr As Long
    
    regErr = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\imc AG\LECTURNITY\Assistant\Settings", 0, KEY_ALL_ACCESS, hKeyCurrentUser)
    
    If (regErr <> ERROR_SUCCESS) Then
        SetShowSummary = retOnError
        Exit Function
    End If
    
    ' get buffer size
    regErr = RegSetValueEx(hKeyCurrentUser, "ShowSummary", 0, REG_DWORD, Chr(lShowSummary), 4)
    
    If (regErr <> ERROR_SUCCESS) Then
        RegCloseKey (hKeyCurrentUser)
        SetShowSummary = retOnError
        Exit Function
    End If

    SetShowSummary = True
    
    Exit Function

ErrQuit:
    If hKeyCurrentUser <> 0 Then
        RegCloseKey (hKeyCurrentUser)
    End If
    
    SetShowSummary = retOnError
    Exit Function

End Function

Public Function GetShowSummary(lShowSummary As Long) As Boolean

    On Error GoTo ErrQuit
    
    Dim retOnError As Boolean            '= "";
    
    Dim dwType As Long
    Dim dwBufSize As Long
    Dim regErr As Long
    
    Dim i As Long
    Dim binData As String
    
    Dim hKeyCurrentUser As Long
    
    'initialization
    dwBufSize = 0
    retOnError = False
    
    hKeyCurrentUser = 0
    
    regErr = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\imc AG\LECTURNITY\Assistant\Settings", 0, KEY_READ, hKeyCurrentUser)
    
    If (regErr <> ERROR_SUCCESS) Then
        GetShowSummary = retOnError
        Exit Function
    End If
    
    ' get buffer size
    regErr = RegQueryValueEx(hKeyCurrentUser, "ShowSummary", 0, dwType, ByVal vbNullString, dwBufSize)
    
    If (regErr <> ERROR_SUCCESS) Then
        RegCloseKey (hKeyCurrentUser)
        GetShowSummary = retOnError
        Exit Function
    End If

    If (dwType <> REG_DWORD Or dwBufSize <> 4) Then
        RegCloseKey (hKeyCurrentUser)
        GetShowSummary = retOnError
        Exit Function
    End If

    regErr = RegQueryValueEx(hKeyCurrentUser, "ShowSummary", 0, dwType, lShowSummary, dwBufSize)

    If (regErr <> ERROR_SUCCESS) Then
        'Erase binData
        RegCloseKey (hKeyCurrentUser)
        GetShowSummary = retOnError
        Exit Function
    End If
    
    RegCloseKey (hKeyCurrentUser)
    
    GetShowSummary = True
    
    Exit Function

ErrQuit:
    If hKeyCurrentUser <> 0 Then
        RegCloseKey (hKeyCurrentUser)
    End If
    
    GetShowSummary = retOnError
    Exit Function

End Function



Public Function GetLecturnityInstall() As String

    On Error GoTo ErrQuit
    
    Dim retOnError As String            '= "";
    
    Dim dwType As Long
    Dim dwBufSize As Long
    Dim regErr As Long
    
    Dim i As Long
    Dim binData As String
    
    Dim hKeyLocalMachine As Long
    
    'initialization
    dwBufSize = 0
    retOnError = ""
    
    hKeyLocalMachine = 0
    
    regErr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\imc AG\LECTURNITY", 0, KEY_READ, hKeyLocalMachine)
    
    If (regErr <> ERROR_SUCCESS) Then
        GetLecturnityInstall = retOnError
        Exit Function
    End If
    
    ' get buffer size
    regErr = RegQueryValueEx(hKeyLocalMachine, "InstallDir", 0, dwType, ByVal vbNullString, dwBufSize)
    
    If (regErr <> ERROR_SUCCESS) Then
        RegCloseKey (hKeyLocalMachine)
        GetLecturnityInstall = retOnError
        Exit Function
    End If

    If (dwType <> REG_SZ) Then
        RegCloseKey (hKeyLocalMachine)
        GetLecturnityInstall = retOnError
        Exit Function
    End If

    binData = Space(dwBufSize + 8)
    
    If IsNull(binData) Then
        RegCloseKey (hKeyLocalMachine)
        GetLecturnityInstall = retOnError
        Exit Function
    End If

    regErr = RegQueryValueEx(hKeyLocalMachine, "InstallDir", 0, dwType, ByVal binData, dwBufSize)

    If (regErr <> ERROR_SUCCESS) Then
        'Erase binData
        RegCloseKey (hKeyLocalMachine)
        GetLecturnityInstall = retOnError
        Exit Function
    End If
    
    RegCloseKey (hKeyLocalMachine)
    
    GetLecturnityInstall = Left(binData, dwBufSize - 1)
    
    Exit Function

ErrQuit:
    If hKeyLocalMachine <> 0 Then
        RegCloseKey (hKeyLocalMachine)
    End If
    
    GetLecturnityInstall = retOnError
    Exit Function

End Function



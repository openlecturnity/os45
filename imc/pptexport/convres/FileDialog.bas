Attribute VB_Name = "FileDialog"
Option Explicit

Private Type BrowseInfo
    hOwner          As Long
    pidlRoot        As Long
    pszDisplayName  As String
    lpszTitle      As String
    ulFlags        As Long
    lpfn            As Long
    lParam          As Long
    iImage          As Long
End Type

Private Declare Function SHGetPathFromIDList Lib "shell32.dll" (ByVal pidl As Long, ByVal pszPath As String) As Long
Private Declare Function SHBrowseForFolder Lib "shell32.dll" (lpBrowseInfo As BrowseInfo) As Long
Private Declare Function CoInitialize Lib "ole32.dll" (ByRef lpVoid As Long) As Long
Private Declare Function CoUninitialize Lib "ole32.dll" () As Long

' Ermittelt Verzeichnisnamen und zeigt Windows-Dialog an
Public Function BrowseDirectory(Optional DialogTitel) As String
    Dim browseDirectoryInfo As BrowseInfo
    Dim ListenNr As Long
    Dim Pfad As String
  
    With browseDirectoryInfo
        .hOwner = 0
        .lpszTitle = IIf(IsMissing(DialogTitel), "Verzeichnispfad auswählen", CStr(DialogTitel))
        .ulFlags = &H40 ' BIF_RETURNONLYFSDIRS
    End With
    
    CoInitialize (0)
    If (True) Then
        ListenNr = SHBrowseForFolder(browseDirectoryInfo)
        Pfad = Space$(512)
        If SHGetPathFromIDList(ByVal ListenNr, ByVal Pfad) Then
            BrowseDirectory = Left(Pfad, InStr(Pfad, vbNullChar) - 1)
        Else
            BrowseDirectory = ""
        End If
        
    End If
    
    CoUninitialize
End Function

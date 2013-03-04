VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "ExtResource"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit
Public Version As String

'Public Function GetSaveAsCaption() As String
'    Dim langID As Integer
'    langID = LangInfo.GetProgramLanguage()
'GetSaveAsAllFileDesc
'    GetSaveAsCaption = LoadResString(EXT_SAVEAS + langID * 1000)
'End Function
Public Function GetSaveAsAllFileDesc() As String
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()

    GetSaveAsAllFileDesc = LoadResString(EXT_ALL_FILE_DESC + langID * 1000)
End Function

Public Function GetSourceDirectory() As String
    GetSourceDirectory = GetSourceDir()
End Function

Public Function GetSaveAsLsdDesc() As String
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()

    GetSaveAsLsdDesc = LoadResString(EXT_LSD_DESC + langID * 1000)
End Function

Public Function GetMenuSaveAsLSD() As String
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()

    GetMenuSaveAsLSD = LoadResString(MNU_SAVEASLSD + langID * 1000)
End Function

Public Function GetMenuOpenInAssistant() As String
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()

    GetMenuOpenInAssistant = LoadResString(MNU_OPENINASSISTANT + langID * 1000)
End Function

Public Function GetMainCaption() As String
    GetMainCaption = LoadResString(DLG_CAPTION)
End Function

Public Function GetNoFileMsg() As String
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()

    GetNoFileMsg = LoadResString(ERR_NO_OPENED_PRES + langID * 1000)
End Function

Public Function GetDiskFullMsg() As String
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()

    GetDiskFullMsg = LoadResString(ERR_DISK_FULL + langID * 1000)
End Function

Public Function GetOverwriteImageMsg() As String
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()

    GetOverwriteImageMsg = LoadResString(ERR_OVERWRITE_IMAGE + langID * 1000)
End Function

Public Function GetOpenLSDFileErrorMsg() As String
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()

    GetOpenLSDFileErrorMsg = LoadResString(ERR_CANNOT_OPEN_LSD + langID * 1000)
End Function

Public Function GetFilenameErrorMsg() As String
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()

    GetFilenameErrorMsg = LoadResString(ERR_WRONG_FILENAME + langID * 1000)
End Function

Public Function GetPresentationnameErrorMsg() As String
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()

    GetPresentationnameErrorMsg = LoadResString(ERR_WRONG_PRESENTATIONNAME + langID * 1000)
End Function

Public Function ConfirmDocSave(ver As String) As Long
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()
    
    If langID = -1 Then
        ' English message
        MsgBox LoadResString(ERR_LEC_NOT_INSTALLED + 1000), vbCritical, "Error"
        ConfirmDocSave = 0
        Exit Function
    End If
    
    With frmConfirmSave
        .Version = ver
        .LanguageID = langID
        .Show 1
        ConfirmDocSave = .RetValue
    End With
        
End Function

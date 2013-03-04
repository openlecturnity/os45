Attribute VB_Name = "CommonFunctions"

Const ERROR_MESSAGE_SHAPES_BEGIN = 700
Const ERROR_MESSAGE_WARNING_BEGIN = 730

Declare Function MessageBoxW Lib "user32.dll" _
                        (ByVal hwnd As Long, _
                         ByVal lpText As Long, _
                         ByVal lpCaption As Long, _
                         ByVal uType As Long) As Long
                          
                          
Public Const MB_ICONERROR As Long = &H10&
Public Const MB_ICONQUESTION As Long = &H20&
Public Const MB_ICONINFORMATION As Long = &H40&
Public Const MB_TASKMODAL As Long = &H2000&
Public Const MB_YESNOCANCEL As Long = &H3&
Public Const MB_YESNO As Long = &H4&

Public Const IDOK As Long = 1
Public Const IDCANCEL As Long = 2
Public Const DABORT As Long = 3
Public Const IDRETRY As Long = 4
Public Const IDIGNORE As Long = 5
Public Const IDYES As Long = 6
Public Const IDNO As Long = 7


Public Function AddToGrid(frm As Form, iPageNumber As Long, iObjectType As Integer, iObjectCount As Integer, iRowIndex As Integer) As Boolean
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()
    
    If langID = -1 Then
        ' GetProgramLanguage returns 1 means the security key is illegal.
        MsgBox "LECTURNITY was not installed.", vbCritical, "Error"
        Add = False
        Exit Function
    ElseIf langID > MAXLANGID Then
        MsgBox "Invalid language setting. Use German."
        langID = 0
    End If
    
    iRowIndex = iRowIndex + 1
    If frm.Grid.Rows <= iRowIndex Then
        frm.Grid.Rows = iRowIndex + 1
    End If
    
    If iPageNumber <> -1 Then
        frm.Grid.TextMatrix(iRowIndex, 0) = iPageNumber
    End If
    frm.Grid.TextMatrix(iRowIndex, 1) = LoadResString(ERROR_MESSAGE_SHAPES_BEGIN + iObjectType + langID * 1000)
    frm.Grid.TextMatrix(iRowIndex, 2) = iObjectCount
    frm.Grid.TextMatrix(iRowIndex, 3) = LoadResString(ERROR_MESSAGE_WARNING_BEGIN + iObjectType + langID * 1000)
    
    Dim i As Integer
    For i = 0 To 3
        Dim textWidth As Long: textWidth = frm.textWidth(frm.Grid.TextMatrix(iRowIndex, i)) + 80
        If textWidth > frm.Grid.ColWidth(i) Then
            frm.Grid.ColWidth(i) = textWidth
        End If
    Next
    
    AddToGrid = True
    
End Function


Public Function GetSourceDir() As String
    Dim szTemp As String: szTemp = ""
    
    Dim langID As Integer
    langID = LangInfo.GetProgramLanguage()
    If langID = -1 Then
        ' English message
        MsgBox LoadResString(ERR_LEC_NOT_INSTALLED + 1000), vbCritical, "Error"
        GetSourceDir = ""
        Exit Function
    End If
    
    Dim lecturnityHome As String
    lecturnityHome = Registry.GetLecturnityHome()
    
    
    Dim szTitle As String
    szTitle = LoadResString(DLG_CAPTION + g_iLanguage * 1000)
    Dim szText As String
        
    Dim bCreateLecturnityHome As Boolean: bCreateLecturnityHome = False
    Dim result As VbMsgBoxResult
    If lecturnityHome = "" Then
        szText = LoadResString(NO_LECTURNITY_HOME + langID * 1000)
        result = MessageBoxW(0, StrPtr(szText), StrPtr(szTitle), MB_ICONQUESTION Or MB_YESNO Or MB_TASKMODAL)
        If result = IDYES Then
            bCreateLecturnityHome = True
        End If
    Else
        szTemp = Dir(lecturnityHome, vbDirectory)
        If szTemp = "" Then
            szText = LoadResString(NO_VALID_LECTURNITY_HOME + langID * 1000)
            result = MessageBoxW(0, StrPtr(szText), StrPtr(szTitle), MB_ICONQUESTION Or MB_YESNO Or MB_TASKMODAL)
            If result = IDYES Then
                bCreateLecturnityHome = True
            End If
            lecturnityHome = ""
        End If
    End If
    
    If bCreateLecturnityHome Then
        lecturnityHome = FileDialog.BrowseDirectory(LoadResString(BROWSE_DIRECTORY + langID * 1000))
        If lecturnityHome <> "" Then
            Registry.SetLecturnityHome (lecturnityHome)
            
            Dim newDir As String
            newDir = lecturnityHome & "\" & LoadResString(SOURCE_DOCUMENT + langID * 1000)
            szTemp = Dir(newDir, vbDirectory)
            If szTemp = "" Then
                MkDir newDir
            End If
            newDir = lecturnityHome & "\" & LoadResString(RECORDINGS + langID * 1000)
            szTemp = Dir(newDir, vbDirectory)
            If szTemp = "" Then
                MkDir newDir
            End If
            newDir = lecturnityHome & "\" & LoadResString(CD_PROJECTS + langID * 1000)
            szTemp = Dir(newDir, vbDirectory)
            If szTemp = "" Then
                MkDir newDir
            End If
        End If
    End If
    
    If lecturnityHome <> "" Then
        GetSourceDir = lecturnityHome & "\" & LoadResString(SOURCE_DOCUMENT + langID * 1000)
    Else
        GetSourceDir = ""
    End If
    

End Function

Public Function FileExists(szFilePath As String) As Boolean
    Dim fLen As Integer, filepath As String
    
    On Error Resume Next
    fLen = Len(Dir$(szFilePath))
    If Err Or fLen = 0 Then
        FileExists = False
    Else
        FileExists = True
    End If
    On Error GoTo 0

End Function

Public Function GetFileName(fullFilename As String) As String
    ' get the file name
    Dim filename As String
    Dim slashPos As Long        ' the position of "\"
    
    filename = fullFilename

    slashPos = InStrRev(fullFilename, "\", -1, vbTextCompare)
    If slashPos > 0 Then
        filename = Right(fullFilename, Len(fullFilename) - slashPos)
    End If
    
    GetFileName = filename
End Function

Public Function GetInstallDir() As String
    
    GetInstallDir = Registry.GetLecturnityInstall()

End Function




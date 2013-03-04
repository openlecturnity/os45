VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "CBatchConvCore"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

Private hPPwnd As Long      ' PowerPoint window handle

Private bHasPPInstance As Boolean
Private ppApp As PowerPoint.Application
Private hListFile As Long

Private fileList        As Collection
Private pathList        As Collection
Private resultList      As Collection

Private bSaveToPPTDir   As Boolean
Private bMakeDir        As Boolean
Private strSavePath     As String

Private bAddInError     As Boolean

Private iErrCode        As Integer

Private g_szApplicationVersion As String

' Error code 1 - 20 are preserved for ppt2Lecturnity.
Private Const COMP_SUCCESS As Integer = 0
Private Const ERR_PPT_NOT_INSTALLED As Integer = 21
Private Const ERR_OPEN_PPT As Integer = 22
Private Const ERR_ADDIN_ERROR As Integer = 23
Private Const ERR_UNKNOWN As Integer = 29
Private Const ERR_WRONG_PPT_OPEN As Integer = 33
Private Const ERR_WRONG_PPT As Integer = 34

Private Function IsPowerPoint2007File(pptName As String) As Boolean

    Dim bIsPowerPoint2007File As Boolean: bIsPowerPoint2007File = False
    ' get the file name
    
    Dim szFilename As String: szFilename = pptName
    
    ' Find out the suffix (without dot)
    Dim szSuffix As String: szSuffix = pptName
    Dim dotPos As Long
    Do
        dotPos = InStr(1, szSuffix, ".", vbTextCompare)
        If dotPos > 0 Then
            szSuffix = Right(szSuffix, Len(szSuffix) - dotPos)
        End If
    Loop While (dotPos > 0)
    
    If Len(szSuffix) > 0 And StrComp(szSuffix, "pptx", vbTextCompare) = 0 Then
        bIsPowerPoint2007File = True
    End If
    
    IsPowerPoint2007File = bIsPowerPoint2007File
    
End Function

Private Function NotOnlyPowerPoint2007Opened() As Boolean

    Dim bNotOnlyPowerPoint2007Opened As Boolean: bNotOnlyPowerPoint2007Opened = False
    Dim hPowerPointWindow As Long: hPowerPointWindow = 0
    
    ' Test for PowerPoint 2003
    hPowerPointWindow = FindWindow("PP11FrameClass", 0&)   ' PowerPoint 2003
    If hPowerPointWindow <> 0 Then
        bNotOnlyPowerPoint2007Opened = True
    End If
     
    ' Test for PowerPoint 2002
    hPowerPointWindow = FindWindow("PP10FrameClass", 0&)   ' PowerPoint 2002
    If hPowerPointWindow <> 0 Then
        bNotOnlyPowerPoint2007Opened = True
    End If
    
    ' Test for PowerPoint 2000
    hPowerPointWindow = FindWindow("PP9FrameClass", 0&)   ' PowerPoint 2000
    If hPowerPointWindow <> 0 Then
        bNotOnlyPowerPoint2007Opened = True
    End If
    
    NotOnlyPowerPoint2007Opened = bNotOnlyPowerPoint2007Opened
    
End Function

Public Function Launch(ByVal sPPTName As String, ByVal sLsdName As String, ByVal parentHwnd As String, ByVal bDoRecord As Boolean) As Integer
    On Error GoTo ErrExp
    Dim bIsPowerPoint2007File As Boolean: bIsPowerPoint2007File = False
    bIsPowerPoint2007File = IsPowerPoint2007File(sPPTName)
    
    If bIsPowerPoint2007File And NotOnlyPowerPoint2007Opened() Then
        Launch = ERR_WRONG_PPT_OPEN
        Exit Function
    End If
    
    'ppt2lecturnity.Show 1, Me
    g_szApplicationVersion = ""
    If OpenPPT() Then
        ' dialogs are allowed during single conversion.
        SaveSetting "ppt2lecturnity-3", "Para", "showmsg", "showdlg"
        SaveSetting "ppt2lecturnity-3", "Para", "parentHwnd", parentHwnd
        Convert sPPTName, sLsdName, bDoRecord
        ClosePPT
    End If
    
    If iErrCode = 2 And g_szApplicationVersion <> "12.0" And bIsPowerPoint2007File Then
        iErrCode = ERR_WRONG_PPT
    End If
    
    Launch = iErrCode
    Exit Function

ErrExp:
    Launch = ERR_UNKNOWN
    Exit Function

End Function

Private Function OpenPPT() As Boolean
    
    bHasPPInstance = PPTIsRunning()      ' true: there is already a PP instance.
    
    On Error GoTo ErrNoPPT
    
    Set ppApp = CreateObject("PowerPoint.Application")
    On Error Resume Next
    
    If ppApp Is Nothing Then
        iErrCode = ERR_OPEN_PPT
        OpenPPT = False
        Exit Function
    End If
    
    g_szApplicationVersion = ppApp.Version
    OpenPPT = True
    Exit Function
    
ErrNoPPT:
    iErrCode = ERR_PPT_NOT_INSTALLED
    OpenPPT = False
    Exit Function
End Function

Private Function PPTIsRunning() As Boolean
    PPTIsRunning = GetPPWnd()
End Function

Private Function GetPPWnd() As Boolean

    hPPwnd = FindWindow("PP12FrameClass", 0&)   ' PowerPoint 2007
    
    If hPPwnd = 0 Then
        hPPwnd = FindWindow("PP11FrameClass", 0&)   ' PowerPoint 2003
    End If
    
    If hPPwnd = 0 Then
        hPPwnd = FindWindow("PP10FrameClass", 0&)   ' PowerPoint 2002
    End If
    
    If hPPwnd = 0 Then
        hPPwnd = FindWindow("PP9FrameClass", 0&)   ' PowerPoint 2000
    End If
    
    If hPPwnd <> 0 Then
        GetPPWnd = True
    Else
        GetPPWnd = False
    End If
        
End Function

Private Sub ClosePPT()
    If bHasPPInstance Then       ' do not close PP
    Else                         ' close PP
        If ppApp Is Nothing Then
        Else
            Dim ppActualApp As PowerPoint.Application
            On Error Resume Next
            Set ppActualApp = GetObject(, "PowerPoint.Application")
            If Err.Number = 0 Then
                ppApp.Quit
            Else
                Err.Clear
            End If
        End If
    End If
    
    Set ppApp = Nothing
    
End Sub

Private Sub Convert(sPPTName As String, sLstName As String, bDoRecord As Boolean)
    On Error Resume Next
    SaveSetting "ppt2lecturnity-3", "Para", "pptname", sPPTName
    SaveSetting "ppt2lecturnity-3", "Para", "lsdname", sLstName
    If bDoRecord Then
        SaveSetting "ppt2lecturnity-3", "Para", "bDoRecord", 1
    Else
        SaveSetting "ppt2lecturnity-3", "Para", "bDoRecord", 0
    End If
        
    ppApp.Run "ppt2Lecturnity3"
    
    iErrCode = CInt(GetSetting("ppt2lecturnity-3", "Result", "ErrorID"))

End Sub

Public Function LaunchBatch(ByVal lstFile As String, ByVal saveToPPTDir As Integer, _
                        ByVal makeDir As Integer, ByVal savePath As String) As Integer
    Dim i As Long
    
    bSaveToPPTDir = IIf(saveToPPTDir = 0, False, True)
    bMakeDir = IIf(makeDir = 0, False, True)
    strSavePath = savePath
    
    On Error Resume Next
    
    If OpenPPT() = False Then
        LaunchBatch = ERR_OPEN_PPT
        Exit Function
    End If
    
    ReadListFile lstFile
    
    ' doesn't show dialogs during batch conversion
    SaveSetting "ppt2lecturnity-3", "Para", "showmsg", "mute"
    
    For i = 1 To fileList.Count
        If bAddInError Then
            Exit For
        Else
            ConvertOne i
        End If
    Next
    
    ClosePPT
    
    LaunchBatch = iErrCode

End Function

Private Function ConvertOne(idx As Long)
    Dim ErrMsg As String
    ' save file name and save path into registry
    ' the converter read these settings from the registry
    SaveSetting "ppt2lecturnity-3", "Para", "fname", CStr(fileList(idx))
    SaveSetting "ppt2lecturnity-3", "Para", "spath", CStr(pathList(idx))
    
    On Error Resume Next
    ppApp.Run "ppt2Lecturnity3"
    
    If Err.Number <> 0 Then
        bAddInError = True
        iErrCode = ERR_ADDIN_ERROR
    Else
        ' get the error id and message from the registry
        ' the error messages were written by the converter
        iErrCode = GetSetting("ppt2lecturnity-3", "Result", "ErrorID")
        ErrMsg = GetSetting("ppt2lecturnity-3", "Result", "ErrorMsg")
    End If
    
    resultList.Add CStr(fileList(idx))
    resultList.Add ("  Conversion " & IIf(iErrCode = "0", "completed successfully. Saved to:", "failed."))
    resultList.Add ("  " & ErrMsg)
    resultList.Add " "

End Function

Private Function ReadListFile(lstFile As String) As Boolean
    Dim TextLine    As String
    Dim Buffer As String, Ret As Long
    Dim lstPath As String
    
    hListFile = FreeFile
    
    Open lstFile For Input As #hListFile   ' Open file.
    
    lstPath = GetPath(lstFile)
    
    Do While Not EOF(hListFile)   ' Loop until end of file.
       Line Input #hListFile, TextLine   ' Read line into variable.
        
       'create a buffer
       Buffer = Space(1023)
       PathCombine Buffer, lstPath, TextLine
            
       'remove the unnecessary chr$(0)'s
       fileList.Add StripTerminator(Buffer)
       
    Loop
    
    Close #hListFile   ' Close file.
    
    SetSavePath
    
End Function

'Remove all trailing Chr$(0)'s
Private Function StripTerminator(sInput As String) As String
    Dim ZeroPos As Long
    ZeroPos = InStr(1, sInput, Chr$(0))
    If ZeroPos > 0 Then
        StripTerminator = Left$(sInput, ZeroPos - 1)
    Else
        StripTerminator = sInput
    End If
End Function

Private Sub SetSavePath()
    Dim i As Long
    
    For i = 1 To pathList.Count     ' clear the path list
        pathList.Remove 1
    Next
    
    If bSaveToPPTDir Then
        If bMakeDir Then
            For i = 1 To fileList.Count
                pathList.Add fileList(i) & "0\"
            Next
        Else
            For i = 1 To fileList.Count
                pathList.Add GetPath(fileList(i))
            Next
        End If
    Else
        If bMakeDir Then
            For i = 1 To fileList.Count
                If Right(strSavePath, 1) <> "\" Then
                    strSavePath = strSavePath & "\"
                End If
                pathList.Add (strSavePath & GetFileName(fileList(i))) & "0" ' & "\")
            Next
        Else
            For i = 1 To fileList.Count
                pathList.Add (IIf(strSavePath = "", "", strSavePath)) ' & "\"))
            Next
        End If
    End If
    
End Sub

Private Function GetPath(str As String) As String
    Dim pos As Long, i As Long
    pos = 0
    Do
        i = InStr(pos + 1, str, "\", vbTextCompare)
        If i > 0 Then
            pos = i
        End If
    Loop While i > 0
    
    GetPath = Mid(str, 1, pos)
    
End Function

Private Function GetFileName(str As String) As String
    Dim Buffer As String
    Buffer = String(1024, 0)
    GetFileTitle str, Buffer, Len(Buffer)
    Buffer = Left$(Buffer, InStr(1, Buffer, Chr$(0)) - 1)
    GetFileName = Buffer
End Function

Private Sub Class_Initialize()
    Set fileList = New Collection
    Set pathList = New Collection
    Set resultList = New Collection
    iErrCode = COMP_SUCCESS
End Sub

Attribute VB_Name = "ppt2Lecturnity"
Option Explicit

' Last modified: Oct. 10. 2002 3:00pm
' by Zhang Hua

Public CvtOpt           As aofGlobalOptions
Public DlgCaption       As String

Public IndentLevel      As Long     ' control the indent of the lsd content.

Public GlobalPres       As Presentation     ' current presentation which should be converted.

Public InTemplateScript As Boolean          ' tell the Shape.Script() when the field <...> should be replaced.
Public CurPageNumber    As Long             ' recorde the current slide index in order to replace the field "<Nr.>"
Public CurrentPosition  As Long

Public GlobalTempSlide        As Slide        ' temporary slide during the conversion.
Public GlobalTempTitleMaster  As Slide
Public GlobalTempSlideMaster  As Slide

Public GlobalSlideWidth       As Long       ' the images have the same size with the slides.
Public GlobalSlideHeight      As Long

' invoked by menu item "Save as Lecturnity documents..."
' conversion range can be set
Sub ppt2Lecturnity()
    Dim AofDoc1         As AofDoc
    
    Dim beginPg     As Long
    Dim pgNum       As Long
    Dim NoFileMsg   As String
    
    On Error GoTo ErrHandler
    
    Dim extRes  As Object
    Set extRes = CreateObject("ConvRes.ExtResource")
    
    With extRes
        DlgCaption = .GetMainCaption() ' & CurrentVersion
        NoFileMsg = .GetNoFileMsg()
    End With
    
    Set extRes = Nothing
    If Application.Presentations.Count < 1 Then
        MsgBox NoFileMsg, vbExclamation, DlgCaption
        Exit Sub
    End If
    
    Set GlobalPres = ActivePresentation
    
    If (ReadOptions(beginPg, pgNum) = False) Then
        Exit Sub
    End If
    
    CvtOpt.MacroID = 1
    
    With frmProgress
        .BeginPage = beginPg
        .PageNum = pgNum
        .Show
    End With
        
    ShowResult
    Exit Sub

ErrHandler:
    MsgBox Err.Description, vbCritical, DlgCaption
    Exit Sub

End Sub

' invoked by menu item "Open with Lecturnity Assistant"
' convert the whole presentation, then open it with LA
Sub ppt2Lecturnity2()
    Dim AofDoc1         As AofDoc
    
    Dim beginPg     As Long
    Dim pgNum       As Long
    Dim extRes      As Object
    Dim NoFileMsg   As String
    
    On Error GoTo ErrHandler2
    
    Set extRes = CreateObject("ConvRes.ExtResource")
    
    With extRes
        DlgCaption = .GetMainCaption() ' & CurrentVersion
        'NoFileMsg = .GetNoFileMsg()
    End With
    
    
    If Application.Presentations.Count < 1 Then
        MsgBox NoFileMsg, vbExclamation, DlgCaption
        Set extRes = Nothing
        Exit Sub
    End If
    
    Set GlobalPres = ActivePresentation
    
    If GlobalPres.Saved = msoFalse Then
        Select Case extRes.ConfirmDocSave(CurrentVersion)
            Case 0  ' cancelled
                Set extRes = Nothing
                Exit Sub
            Case 1  ' save and continue
                GlobalPres.Save
            Case -1 ' do not save and continue
        End Select
    End If
    
    Set extRes = Nothing
    
    With CvtOpt
        .MacroID = 2
        .WBFileName = GetFormalFileName() & ".lsd"
        .WBFilePath = GetUserProfDir()
    End With
    
    SetOptions
    
    With frmProgress
        .BeginPage = 1
        .PageNum = GlobalPres.Slides.Count
        .Show
    End With
    
    If Not CvtOpt.Failed Then
        RunLsdFile CvtOpt.WBFilePath & CvtOpt.WBFileName
    End If
    Exit Sub
    
ErrHandler2:
    MsgBox Err.Description, vbCritical, DlgCaption
    Exit Sub

End Sub

Sub ppt2Lecturnity3()
    Dim FileName    As String
    Dim savepath    As String
    Dim showmsg     As String
    
    Dim AofDoc1         As AofDoc
    Dim NoFileMsg   As String
    
    Dim beginPg     As Long
    Dim pgNum       As Long
    
    On Error Resume Next
    
    FileName = GetSetting("ppt2lecturnity-3", "Para", "fname")
    savepath = GetSetting("ppt2lecturnity-3", "Para", "spath")
    If ("\" <> Right(savepath, 1)) Then
        savepath = savepath & "\"
    End If
    showmsg = GetSetting("ppt2lecturnity-3", "Para", "showmsg")
          
    If Err.Number <> 0 Then
        MsgBox Err.Description, vbCritical, "Error"
    End If
          
    If Not CreateNewDirectory(savepath) Then
        SaveSetting "ppt2lecturnity-3", "Result", "ErrorID", "1"
        SaveSetting "ppt2lecturnity-3", "Result", "ErrorMsg", _
            ("Cannot create directory: " & savepath)
        Exit Sub
    End If
       
    Set GlobalPres = Application.Presentations.Open(FileName, msoTrue, msoFalse, msoFalse)
    
    If GlobalPres Is Nothing Or Err.Number <> 0 Then
        SaveSetting "ppt2lecturnity-3", "Result", "ErrorID", "2"        'success
        SaveSetting "ppt2lecturnity-3", "Result", "ErrorMsg", Err.Description
        Exit Sub
    End If
    
    Dim extRes      As Object

    Set extRes = CreateObject("ConvRes.ExtResource")

    With extRes
        DlgCaption = .GetMainCaption() ' & CurrentVersion
    End With

    Set extRes = Nothing
    
    With CvtOpt
        .MacroID = 1
        .WBFileName = GetFormalFileName() & ".lsd"
        .WBFilePath = savepath     'GetUserProfDir()
    End With
    
    SetOptions
    
    If showmsg = "mute" Then
        CvtOpt.NoMsgDlg = True
        MuteConvert
    Else
        CvtOpt.NoMsgDlg = False
        With frmProgress
            .BeginPage = 1
            .PageNum = GlobalPres.Slides.Count
            .Show
        End With
    End If
    
    GlobalPres.Close
    Set GlobalPres = Nothing
    
    SaveSetting "ppt2lecturnity-3", "Result", "ErrorID", "0"        'success
    SaveSetting "ppt2lecturnity-3", "Result", "ErrorMsg", _
                (CvtOpt.WBFilePath & CvtOpt.WBFileName)

End Sub

' display the "option" dialog and set conversion options.
'
' args:         beginpg : the first page
'               pgnum   : page number
' return:       true    : options are set correctly
'               false   : dialogs are cancelled.
Private Function ReadOptions(ByRef beginPg As Long, ByRef pgNum As Long) As Boolean
    
    Dim DlgBegin        As Object  'BeginDlg
    Dim bContinue       As Boolean
    Dim ProgramLanguage As Integer
    
    Set DlgBegin = CreateObject("ConvRes.BeginDlg") 'New BeginDlg
    
    If DlgBegin.Show(1&, GlobalPres.Slides.Count, bContinue, CurrentVersion) Then
        If Not bContinue Then   ' Begin dialog cancelled.
            ReadOptions = False
            Set DlgBegin = Nothing
            Exit Function
        End If
        
        beginPg = DlgBegin.BeginPage
        pgNum = DlgBegin.EndPage - DlgBegin.BeginPage + 1
        
        If Not GetSaveInfo(CvtOpt.WBFilePath, CvtOpt.WBFileName, ProgramLanguage) Then
            ReadOptions = False
            Set DlgBegin = Nothing
            Exit Function
        End If
    Else    ' security key is illegal.
        ReadOptions = False
        Set DlgBegin = Nothing
        Exit Function
    End If
    
    SetOptions
    
    Set DlgBegin = Nothing
    ReadOptions = True
End Function

Private Sub SetOptions()
    
    With CvtOpt
        ' replace the blank instead of "_"
        .PPTFileName = GlobalPres.FullName
        .ImgFileNamePrefix = MyReplace(Left(.WBFileName, Len(.WBFileName) - 4), " ", "_")
        '.ConvertBackgAsImage = False
        .ConvertBackgAsImage = True
        .NoGroup = True
        .WithDebugInfo = True
        .LogFileName = .WBFilePath & .WBFileName & ".log"
        .ExpImageSuffix = ".wmf"
        '.ExpImageFormat = ppShapeFormatWMF
        .WithImageSize = True
'        .ExpImageSuffix = ".gif"
'        .ExpImageFormat = ppShapeFormatGIF
'        .WithImageSize = False
        .Failed = False
    End With
    
End Sub

' Display the conversion result, include the resulting files and conversion time.
Private Sub ShowResult()
    
    Dim mes         As String
    Dim DlgEnd      As Object 'EndDlg
    Dim bOpenFile   As Boolean
    
    If CvtOpt.Failed Then
        Exit Sub
    End If
    
    mes = CvtOpt.WBFilePath & CvtOpt.WBFileName
    
    On Error Resume Next
    
    Set DlgEnd = CreateObject("ConvRes.EndDlg") 'New EndDlg
    
    DlgEnd.Show mes, bOpenFile, CurrentVersion
    
    If bOpenFile Then
        RunLsdFile CvtOpt.WBFilePath & CvtOpt.WBFileName
    End If
    
    Set DlgEnd = Nothing
    
End Sub

' Adds an item to the File/Datei menu.
' This procedure runs automatically when the add-in loads.
Sub Auto_Open()
    
    Dim offCmdBrPp As CommandBarPopup
    Dim offCmdBtn  As CommandBarButton
    Dim extRes As Object
    Dim MenuString As String, MenuString2 As String
    
    Set extRes = CreateObject("ConvRes.ExtResource")
    
    With extRes
        MenuString = .GetMenuSaveAsLSD()
        MenuString2 = .GetMenuOpenInAssistant()
    End With
    
    Set extRes = Nothing
    
    ' Create an object variable referring to the Tools menu.
    Set offCmdBrPp = _
        Application.CommandBars("Menu Bar").Controls(1)
    ' If the Address Book Report command already exists, delete it.
    
    On Error Resume Next
    
    offCmdBrPp.Controls(MenuString).Delete
    
    ' Add a command in the fourth position on the Tools menu.
    Set offCmdBtn = _
        offCmdBrPp.Controls.Add(Type:=msoControlButton, Before:=6)
    ' Set properties on the new command.
    With offCmdBtn
        .Caption = MenuString
        .OnAction = "ppt2Lecturnity"
    End With
    
    
    offCmdBrPp.Controls(MenuString2).Delete
    ' Add a command in the fourth position on the Tools menu.
    Set offCmdBtn = _
        offCmdBrPp.Controls.Add(Type:=msoControlButton, Before:=6)
    ' Set properties on the new command.
    With offCmdBtn
    '    .Caption = "Products Slide Show"
        .Caption = MenuString2 '"Convert to epx..."
        .OnAction = "ppt2Lecturnity2"
    End With

End Sub

' Deletes an item on the File/Datei menu.
' This procedure runs automatically when the add-in unloads.
' If the Address Book Report command exists, delete it.
Sub Auto_Close()
    Dim extRes As Object
    Dim MenuString As String, MenuString2 As String
    
    Set extRes = CreateObject("ConvRes.ExtResource")
    
    With extRes
        MenuString = .GetMenuSaveAsLSD()
        MenuString2 = .GetMenuOpenInAssistant()
    End With
    
    Set extRes = Nothing
    
    On Error Resume Next
    Application.CommandBars("Menu Bar").Controls(1).Controls(MenuString).Delete
    Application.CommandBars("Menu Bar").Controls(1).Controls(MenuString2).Delete
    
        
End Sub




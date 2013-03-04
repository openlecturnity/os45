Attribute VB_Name = "ppt2Lecturnity"
Option Explicit

#Const COMPILE_VERSION = 2007

#If COMPILE_VERSION = 2007 Then
Public Sub OpenWithLec(ctrl As IRibbonControl)
    Import_OpenWithLecturnityAssistant
End Sub

Public Sub SaveAsLec(ctrl As IRibbonControl)
    Import_SaveAsLecturnitySourceDocument
End Sub

Public Sub ChangeOpenWithLabel(ctrl As IRibbonControl, ByRef NewLabel)
    Dim pptresourceHandle As Long
    pptresourceHandle = LoadPPTResources '

        Dim szMessage As String
        szMessage = String(256, vbNullChar)

    GetMenuOpenInAssistant (StrPtr(szMessage))
    Dim szOpenInMenuString As String: szOpenInMenuString = szMessage

    NewLabel = szOpenInMenuString

    If pptresourceHandle <> 0 Then
        FreeLoadedLibrary (pptresourceHandle)
    End If

End Sub

Public Sub ChangeSaveAsLabel(ctrl As IRibbonControl, ByRef NewLabel)
    Dim pptresourceHandle As Long
    pptresourceHandle = LoadPPTResources '

        Dim szMessage As String
        szMessage = String(256, vbNullChar)

    GetMenuSaveAsLSD (StrPtr(szMessage))
    Dim szSaveMenuString As String: szSaveMenuString = szMessage

    NewLabel = szSaveMenuString

    If pptresourceHandle <> 0 Then
        FreeLoadedLibrary (pptresourceHandle)
    End If

End Sub

#End If

' invoked by menu item "Save as Lecturnity documents..."
' conversion range can be set
Sub Import_SaveAsLecturnitySourceDocument()

    LoadLibraries
    
    g_szDlgCaption = GetMessageCaption()
    
    If NoPresentationIsOpen() Then
        Exit Sub
    End If
    
    
    ' Set active presentation
    Set g_currentPresentation = ActivePresentation
    
    
    ' Initialize global variable (Application version, page width and height, ...)
    InitGlobalVariables
    
    If g_lApplicationVersion = 2007 Then
        Dim szMessage As String
        szMessage = String(256, vbNullChar)
        Dim result As Long
        
        If PresentationIsMarkedAsFinal() Then
            GetPresentationIsFinalErrorMessage (StrPtr(szMessage))
            result = MessageBoxW(0, StrPtr(szMessage), StrPtr(g_szDlgCaption), _
                                 MB_ICONEXCLAMATION Or MB_TOPMOST)
            Exit Sub
        End If
        
        If PresentationIsReadOnly() Then
            GetReadOnlyErrorMessage (StrPtr(szMessage))
            result = MessageBoxW(0, StrPtr(szMessage), StrPtr(g_szDlgCaption), _
                                 MB_ICONEXCLAMATION Or MB_TOPMOST)
            Exit Sub
        End If
    End If
    
    ' Call import wizard
    Dim bSuccess As Boolean: bSuccess = True
    bSuccess = SetOptions
    If (bSuccess = False) Then
        Exit Sub
    End If
    
    ' Import of the slides
    StartImportProcess
    
    FreeLibraries
End Sub


' invoked by menu item "Open with Lecturnity Assistant"
' convert the whole presentation, then open it with LA
Sub Import_OpenWithLecturnityAssistant()
    Dim AofDoc1         As AofDoc
    
    Dim beginPg     As Long
    Dim pgNum       As Long
    Dim subDir      As String
    
    Dim result As Long
    
    LoadLibraries
    
    g_szDlgCaption = GetMessageCaption()
    
    If NoPresentationIsOpen() Then
        Exit Sub
    End If
    
    ' Set active presentation
    Set g_currentPresentation = ActivePresentation
    
    ' Initialize global variable (Application version, page width and height, ...)
    InitGlobalVariables
    
    If g_lApplicationVersion = 2007 Then
        Dim szMessage As String
        szMessage = String(256, vbNullChar)
        
        If PresentationIsMarkedAsFinal() Then
            GetPresentationIsFinalErrorMessage (StrPtr(szMessage))
            result = MessageBoxW(0, StrPtr(szMessage), StrPtr(g_szDlgCaption), _
                                 MB_ICONEXCLAMATION Or MB_TOPMOST)
            Exit Sub
        End If
        
        If PresentationIsReadOnly() Then
            GetReadOnlyErrorMessage (StrPtr(szMessage))
            result = MessageBoxW(0, StrPtr(szMessage), StrPtr(g_szDlgCaption), _
                                 MB_ICONEXCLAMATION Or MB_TOPMOST)
            Exit Sub
        End If
    End If
    
    Dim szSavePath    As String
    szSavePath = String(256, vbNullChar)
    Dim iStringLength: iStringLength = Len(szSavePath)
    Dim iResult As Integer: iResult = GetSourceDirectory(StrPtr(szSavePath), iStringLength)
    
    Dim savePath As String: savePath = Left(szSavePath, iStringLength)
    
    With g_conversionOptions
        Dim szFormalFilename As String
        szFormalFilename = GetFormalFileName()
        .MacroID = 2
        .WBFileName = szFormalFilename & ".lsd"
        If savePath = "" Then
            .WBFilePath = GetUserProfileDirectory()
        Else
            .WBFilePath = savePath & "\"
        End If
        '.WBFileName = TranslateForCodepage(.WBFileName)
        
        If Not DirectoryExists(.WBFilePath) Then
            MakeDirectory (.WBFilePath) ' if the dir exist, a run-time error raised.
        End If
        
        subDir = szFormalFilename
        .WBFilePath = .WBFilePath & subDir & "\"
        If Not DirectoryExists(.WBFilePath) Then
            MakeDirectory (.WBFilePath) ' if the dir exist, a run-time error raised.
        End If
    End With
    
    SetCommonOptions
    
    FillDefaultSlideList
    
    Dim bResult As Boolean: bResult = True
    bResult = StartQuietImportProcess(True)
    
    If bResult <> 0 And g_bConversionSucceeded And Not g_conversionOptions.Failed Then
        RunLsdFile g_conversionOptions.WBFilePath & g_conversionOptions.WBFileName
    End If
    
    FreeLibraries
    
End Sub

Sub ppt2Lecturnity3()
    Dim pptname    As String
    Dim lsdname    As String
    Dim showmsg     As String
    
    Dim AofDoc1         As AofDoc
    Dim NoFileMsg   As String
    
    Dim beginPg     As Long
    Dim pgNum       As Long
    
    On Error Resume Next
    
    Dim result As Long
    
    LoadLibraries
    
    g_szDlgCaption = GetMessageCaption()
    
    pptname = GetSetting("ppt2lecturnity-3", "Para", "pptname")
    lsdname = GetSetting("ppt2lecturnity-3", "Para", "lsdname")
    showmsg = GetSetting("ppt2lecturnity-3", "Para", "showmsg")
    Dim bDoRecord As Integer
    bDoRecord = GetSetting("ppt2lecturnity-3", "Para", "bDoRecord")
    
    Dim szParentHwnd As String
    szParentHwnd = GetSetting("ppt2lecturnity-3", "Para", "parentHwnd")
          
    If Err.Number <> 0 Then
        MsgBox Err.Description, vbCritical, "Error"
    End If
       
    Set g_currentPresentation = Application.Presentations.Open(pptname, msoTrue, msoFalse, msoFalse)
  
    InitGlobalVariables
    
    If g_lApplicationVersion = 2007 Then
        Dim szMessage As String
        szMessage = String(256, vbNullChar)
        
        If PresentationIsMarkedAsFinal() Then
            GetPresentationIsFinalErrorMessage (StrPtr(szMessage))
            result = MessageBoxW(0, StrPtr(szMessage), StrPtr(g_szDlgCaption), _
                                 MB_ICONEXCLAMATION Or MB_TOPMOST)
            SaveSetting "ppt2lecturnity-3", "Result", "ErrorID", "1"        'Do not load lsd
            Exit Sub
        End If
        
        ' No question for read only, because in this case it is always read only
    End If
    
    If g_currentPresentation Is Nothing Or Err.Number <> 0 Then
        SaveSetting "ppt2lecturnity-3", "Result", "ErrorID", "2"        'success
        SaveSetting "ppt2lecturnity-3", "Result", "ErrorMsg", Err.Description
        Exit Sub
    End If
    
    With g_conversionOptions
        .MacroID = 3
        .WBFileName = GetFileName(lsdname)
        .WBFilePath = GetDirectory(lsdname)
    End With
    
    SetCommonOptions
    
    FillDefaultSlideList

    Dim bResult As Boolean: bResult = True
    bResult = StartQuietImportProcess(False, szParentHwnd, bDoRecord)
    
    g_currentPresentation.Close
    Set g_currentPresentation = Nothing
    
    If bResult And g_bConversionSucceeded Then
        SaveSetting "ppt2lecturnity-3", "Result", "ErrorID", "0"        'success
    Else
        SaveSetting "ppt2lecturnity-3", "Result", "ErrorID", "1"        'cancelled
    End If
        
    SaveSetting "ppt2lecturnity-3", "Result", "ErrorMsg", _
                (g_conversionOptions.WBFilePath & g_conversionOptions.WBFileName)
                
    FreeLibraries

End Sub

' Display the conversion result, include the resulting files and conversion time.

' Adds an item to the File/Datei menu.
' This procedure runs automatically when the add-in loads.
Sub Auto_Open()
   
    If (StrComp(Application.Version, "12.0", vbTextCompare) <> 0 And StrComp(Application.Version, "14.0", vbTextCompare) <> 0) Then
         Dim pptresourceHandle As Long
         pptresourceHandle = LoadPPTResources
             
             Dim szMessage As String
             szMessage = String(256, vbNullChar)
         
         GetMenuSaveAsLSD (StrPtr(szMessage))
         Dim szSaveMenuString As String: szSaveMenuString = szMessage
         
         GetMenuOpenInAssistant (StrPtr(szMessage))
         Dim szOpenInMenuString As String: szOpenInMenuString = szMessage
        
         Dim offCmdBrPp As CommandBarPopup
         Dim offCmdBtn  As CommandBarButton
         ' Create an object variable referring to the Tools menu.
         Set offCmdBrPp = _
             Application.CommandBars("Menu Bar").Controls(1)
         ' If the Address Book Report command already exists, delete it.
         
         On Error Resume Next
         
         offCmdBrPp.Controls(szSaveMenuString).Delete
         
         ' Add a command in the fourth position on the Tools menu.
         Set offCmdBtn = _
             offCmdBrPp.Controls.Add(Type:=msoControlButton, Before:=6)
         ' Set properties on the new command.
         With offCmdBtn
             .Caption = szSaveMenuString
             .OnAction = "Import_SaveAsLecturnitySourceDocument"
             ' this Tag is used to identify the CommandBarControl
             .Tag = "menu_ppt2Lecturnity"
             .Enabled = False
         End With
         
         
         offCmdBrPp.Controls(szOpenInMenuString).Delete
         ' Add a command in the fourth position on the Tools menu.
         Set offCmdBtn = _
             offCmdBrPp.Controls.Add(Type:=msoControlButton, Before:=6)
         ' Set properties on the new command.
         With offCmdBtn
         '    .Caption = "Products Slide Show"
             .Caption = szOpenInMenuString '"Convert to epx..."
             .OnAction = "Import_OpenWithLecturnityAssistant"
             ' this Tag is used to identify the CommandBarControl
             .Tag = "menu_ppt2Lecturnity2"
             .Enabled = False
         End With
         
         If pptresourceHandle <> 0 Then
             FreeLoadedLibrary (pptresourceHandle)
         End If
         
         ' begin to trap events
         TrapEvents
    End If
End Sub

' Deletes an item on the File/Datei menu.
' This procedure runs automatically when the add-in unloads.
' If the Address Book Report command exists, delete it.
Sub Auto_Close()

    If (StrComp(Application.Version, "12.0", vbTextCompare) <> 0 And StrComp(Application.Version, "14.0", vbTextCompare) <> 0) Then
        Dim pptresourceHandle As Long
        pptresourceHandle = LoadPPTResources
        
        Dim MenuString As String, MenuString2 As String
        
        Dim szMessage As String
        szMessage = String(256, vbNullChar)
        
        GetMenuSaveAsLSD (StrPtr(szMessage))
        MenuString = szMessage
        
        GetMenuOpenInAssistant (StrPtr(szMessage))
        MenuString2 = szMessage
        
        On Error Resume Next
        Application.CommandBars("Menu Bar").Controls(1).Controls(MenuString).Delete
        Application.CommandBars("Menu Bar").Controls(1).Controls(MenuString2).Delete
        
        If pptresourceHandle <> 0 Then
            FreeLoadedLibrary (pptresourceHandle)
        End If
        
        ' release event trap
        ReleaseTrap
    End If
End Sub


Attribute VB_Name = "ImportFunctions"
Option Explicit

Private m_pptResourceHandle     As Long
Private m_ppt2LecturnityHandle  As Long
Private m_gdiplusHandle         As Long

Public Declare Function FindWindow Lib "user32" Alias "FindWindowA" _
        (ByVal lpClassName As String, _
         ByVal lpWindowName As Long) As Long

Private Function GetPptWnd() As Long
    Dim hPPwnd As Long      ' PowerPoint window handle
	hPPwnd = FindWindow("PP14FrameClass", 0&)   ' PowerPoint 2010
    If hPPwnd = 0 Then
		hPPwnd = FindWindow("PP12FrameClass", 0&)   ' PowerPoint 2007
	End If
    If hPPwnd = 0 Then
        hPPwnd = FindWindow("PP11FrameClass", 0&)   ' PowerPoint 2003
    End If
    If hPPwnd = 0 Then
        hPPwnd = FindWindow("PP10FrameClass", 0&)   ' PowerPoint 2002
    End If
    If hPPwnd = 0 Then
        hPPwnd = FindWindow("PP9FrameClass", 0&)   ' PowerPoint 2000
    End If
End Function

Public Function StartImportProcess() As Boolean
    
    g_bConversionSucceeded = True
    
    Set g_LecturnityDocument = New AofDoc
    g_LecturnityDocument.PreConversion
    
    If g_bConversionSucceeded Then
        DoEvents
        If g_lApplicationVersion = 2007 Then
            g_LecturnityDocument.ParseTemplates_2007
        Else
            g_LecturnityDocument.ParseTemplates
        End If
    End If
    
    If g_bConversionSucceeded Then
        DoEvents
        g_LecturnityDocument.ConvertSlides
    End If
    
    ' first close the lsd file because
    ' open with Assistant is not possible if it is not closed)
    
    g_LecturnityDocument.PostConversion
   
    If g_bConversionSucceeded Then
        'dlgProgress.Hide
        'Dim lTop As Long: lTop = dlgProgress.GetTop()
        'Dim lLeft As Long: lLeft = dlgProgress.GetLeft()
        ShowResult 1
    End If
    
    Dim result As Long
    If g_conversionOptions.Failed Then
        If g_conversionOptions.FailMsg <> "" Then
            result = MessageBoxW(0, StrPtr(g_conversionOptions.FailMsg), StrPtr(g_szDlgCaption), _
                                           MB_ICONERROR Or MB_TOPMOST)
        Else
            result = MessageBoxW(0, StrPtr(g_szMsgDiskFull), StrPtr(g_szDlgCaption), _
                                           MB_ICONERROR Or MB_TOPMOST)
        End If
    End If
    
    Set g_LecturnityDocument = Nothing
    
    StartImportProcess = True
End Function

Public Function StartQuietImportProcess(bReset As Boolean, Optional szParentHwnd As String = "", Optional bDoRecord As Integer = 0) As Boolean
    
    g_bConversionSucceeded = True
    
    Set g_LecturnityDocument = New AofDoc
    g_LecturnityDocument.PreConversion
    
    g_ImportWizardHwnd = 0
    Dim bResult As Boolean
    
    ' if the progress dialog is called from the Assistant (Not bReset)
    ' the progress dialog have to be called without the PowerPoint window.
    Dim parentHwnd As Long: parentHwnd = 0
    If szParentHwnd <> "" Then
        parentHwnd = CLng(szParentHwnd)
    End If
    If bReset Then
        bResult = ShowProgressDialog(g_ImportWizardHwnd, GetPptWnd())
    Else
        bResult = ShowProgressDialog(g_ImportWizardHwnd, parentHwnd)
    End If
    
    If g_bConversionSucceeded Then
        DoEvents
        If g_lApplicationVersion = 2007 Then
            g_LecturnityDocument.ParseTemplates_2007
        Else
            g_LecturnityDocument.ParseTemplates
        End If
    End If
    
    If g_bConversionSucceeded Then
        DoEvents
        g_LecturnityDocument.ConvertSlides bReset
    End If
      
    g_LecturnityDocument.PostConversion
    
    bResult = True
    If g_bConversionSucceeded Then
        If g_iNotSupportedCount > 0 Then
            bResult = ShowResult(0, bDoRecord, parentHwnd)
        End If
    End If
    
    Dim result As Long
    If g_conversionOptions.Failed Then
        If g_conversionOptions.FailMsg <> "" Then
            result = MessageBoxW(0, StrPtr(g_conversionOptions.FailMsg), StrPtr(g_szDlgCaption), _
                                           MB_ICONERROR Or MB_TOPMOST)
        Else
            result = MessageBoxW(0, StrPtr(g_szMsgDiskFull), StrPtr(g_szDlgCaption), _
                                           MB_ICONERROR Or MB_TOPMOST)
        End If
    End If
    
    Set g_LecturnityDocument = Nothing
    
    StartQuietImportProcess = bResult
    
End Function

Private Function InitializeRestrictedObjectWarning() As Boolean
    
    ' find out the total number of warnings
    Dim iTotalWarnings As Long: iTotalWarnings = 0
    Dim i As Integer
    Dim j As Integer
    For i = 1 To g_iNotSupportedCount
        For j = 1 To g_iNotSupportedObjectCount
            If g_lsNotSupported(i).iObjectList(j).iObject > 0 Then
               iTotalWarnings = iTotalWarnings + 1
            End If
        Next
    Next
    
    Dim bResult As Boolean
    Dim szWarning As String
    For i = 1 To g_iNotSupportedCount
        For j = 1 To g_iNotSupportedObjectCount
            If g_lsNotSupported(i).iObjectList(j).iObject > 0 Then
                szWarning = String(256, vbNullChar)
                szWarning = g_lsNotSupported(i).iPage & ", " & _
                            j & ", " & _
                            g_lsNotSupported(i).iObjectList(j).iObject
                bResult = InsertRestrictedObject(StrPtr(szWarning), iTotalWarnings)
            End If
        Next
    Next
    
    InitializeRestrictedObjectWarning = bResult
    
End Function

Public Function ShowResult(bQuiet As Integer, Optional bDoRecord As Integer = 0, Optional hWnd As Long = 0) As Boolean
    
    Dim szLsdName As String: szLsdName = g_conversionOptions.WBFilePath & g_conversionOptions.WBFileName
    If g_conversionOptions.Failed Then
        ShowResult = False
        Exit Function
    End If
    
    On Error Resume Next
    
    InitializeRestrictedObjectWarning
    
    Dim bResult As Boolean
    bResult = ShowResultDialog(StrPtr(szLsdName), bQuiet, bDoRecord, g_ImportWizardHwnd, hWnd)
    
    If (bResult = 0) Then
        ShowResult = False
    Else
        ShowResult = True
    End If
    
End Function


Public Function ShowQuietResult(lTop As Long, lLeft As Long) As Boolean
    
    If g_conversionOptions.Failed Then
        Exit Function
    End If
    
    On Error Resume Next
    
    Dim bResult As Boolean
    
    bResult = InitializeRestrictedObjectWarning
    
    ShowQuietResult = bResult
    
End Function

Public Function SetCommonOptions()

    With g_conversionOptions
        .pptFileName = g_currentPresentation.FullName
        .NoGroup = True
        .ImgFileNamePrefix = Replace(Left(.WBFileName, Len(.WBFileName) - 4), " ", "_")
        .ConvertBackgAsImage = True
        .WithDebugInfo = True
        .LogFileName = .WBFilePath & .WBFileName & ".log"
        .Failed = False
        .WithImageSize = True
    End With

    If g_lApplicationVersion = 2000 Then
        SetImageExportOptions_9_0
    Else
        SetImageExportOptions_10_0
    End If
    
End Function

' Setzen der Variablen für den Import und
' Anzeigen des Dialogs für die Seiten und Dateiauswahl
Public Function SetOptions() As Boolean
    
    Dim szPPTname As String: szPPTname = ActivePresentation.FullName
    Dim iSlideCount As Long: iSlideCount = ActivePresentation.Slides.Count
    Dim iCurrentSlide As Long: iCurrentSlide = GetCurrentSlideNumber()
    Dim szSlideSelection As String: szSlideSelection = String(256, vbNullChar)
    Dim iSelectionLength As Long: iSelectionLength = 0
    Dim szLsdName As String: szLsdName = String(256, vbNullChar)
    Dim iLSDNameLength As Long: iLSDNameLength = 0
    
    g_ImportWizardHwnd = 0
    
    Dim bResult As Boolean: bResult = False
    bResult = ShowImportWizard(StrPtr(szPPTname), iSlideCount, iCurrentSlide, _
                               StrPtr(szSlideSelection), iSelectionLength, _
                               StrPtr(szLsdName), iLSDNameLength, _
                               g_ImportWizardHwnd, GetPptWnd())
    If bResult = 0 Then
        bResult = False
    Else
        bResult = True
    End If
    
    
    If bResult Then
        szSlideSelection = Left(szSlideSelection, iSelectionLength)
        szLsdName = Left(szLsdName, iLSDNameLength)
    
        FillSlideList (szSlideSelection)
    
        With g_conversionOptions
            ' replace the blank instead of "_"
            .WBFileName = GetFileName(szLsdName)
            .WBFilePath = GetDirectory(szLsdName)
        End With
    
        SetCommonOptions
    End If
    
    SetOptions = bResult
    
End Function

Public Sub SetImageExportOptions_9_0()
    With g_conversionOptions
        .ExpImageSuffix = ".wmf"
        .ExpImageFormat = ppShapeFormatWMF
        
        .ExpMetafileSuffix = ".wmf"
        .ExpMetafileFormat = ppShapeFormatWMF
        
        .ExpTextSuffix = ".wmf"
        .ExpTextFormat = ppShapeFormatWMF
        
        .ExpOLESuffix = ".wmf"
        .ExpOLEFormat = ppShapeFormatWMF
    End With
End Sub

Public Sub SetImageExportOptions_10_0()
    With g_conversionOptions
        .ExpImageSuffix = ".png"
        .ExpImageFormat = ppShapeFormatPNG
        
        .ExpMetafileSuffix = ".emf"
        .ExpMetafileFormat = ppShapeFormatEMF
        
        .ExpTextSuffix = ".emf"
        .ExpTextFormat = ppShapeFormatEMF
        
        .ExpOLESuffix = ".wmf"
        .ExpOLEFormat = ppShapeFormatWMF
    End With
End Sub

Public Function GetCurrentSlideNumber() As Long

    Dim iCurrentSlide As Long: iCurrentSlide = 1
    
    On Error Resume Next
    
    If g_currentPresentation.Windows.Count > 0 Then
        If g_currentPresentation.Windows.Item(1).Selection.SlideRange.Count = 1 Then
            iCurrentSlide = g_currentPresentation.Windows.Item(1).Selection.SlideRange.SlideIndex
        ElseIf g_currentPresentation.Windows.Item(1).Selection.SlideRange.Count > 1 Then
            iCurrentSlide = g_currentPresentation.Windows.Item(1).Selection.SlideRange.Item(1).SlideIndex
        End If
    End If
    
    GetCurrentSlideNumber = iCurrentSlide
    
End Function

Public Sub FillSlideList(szSlideSelection As String)
    Dim i As Long
    Dim j As Long
    
    Dim lTotalSlideCount As Long: lTotalSlideCount = g_currentPresentation.Slides.Count
    Dim lTmpSlideList() As Long
    ReDim lTmpSlideList(1 To lTotalSlideCount) As Long
    
    For i = 1 To lTotalSlideCount
        lTmpSlideList(i) = -1
    Next
    
    Dim szSlideNumber As String: szSlideNumber = ""
    Dim iRangeBegin As Long: iRangeBegin = -1
    Dim iRangeEnd As Long: iRangeEnd = -1
    If szSlideSelection <> "" Then
        For i = 1 To Len(szSlideSelection)
            Dim szChar As String: szChar = Mid(szSlideSelection, i, 1)
            If IsNumeric(szChar) Then
                szSlideNumber = szSlideNumber + szChar
            ElseIf szChar = "-" Then
                If Len(szSlideNumber) > 0 Then
                    iRangeBegin = CLng(szSlideNumber)
                Else
                    iRangeBegin = -1
                End If
                szSlideNumber = ""
            ElseIf szChar = ";" Then
                If iRangeBegin <> -1 Then
                    If Len(szSlideNumber) > 0 Then
                        iRangeEnd = CLng(szSlideNumber)
                    Else
                        iRangeEnd = -1
                    End If
                    If iRangeEnd <> -1 Then
                        For j = iRangeBegin To iRangeEnd
                            lTmpSlideList(j) = 1
                        Next
                    Else
                        lTmpSlideList(iRangeBegin) = 1
                    End If
                    szSlideNumber = ""
                    iRangeBegin = -1
                    iRangeEnd = -1
                Else
                    If Len(szSlideNumber) > 0 Then
                        j = CLng(szSlideNumber)
                        lTmpSlideList(j) = 1
                    End If
                    szSlideNumber = ""
                End If
            End If
        Next
        
        If Len(szSlideNumber) > 0 Then
            If iRangeBegin <> -1 Then
                iRangeEnd = CLng(szSlideNumber)
                For j = iRangeBegin To iRangeEnd
                    lTmpSlideList(j) = 1
                Next
                szSlideNumber = ""
                iRangeBegin = -1
                iRangeEnd = -1
            Else
                j = CLng(szSlideNumber)
                lTmpSlideList(j) = 1
                szSlideNumber = ""
            End If
        End If
    Else
        For j = 1 To lTotalSlideCount
            lTmpSlideList(j) = 1
        Next
    End If
    
    g_lSlideCount = 0
    ReDim g_lSlideList(1 To lTotalSlideCount) As Long
    For i = 1 To lTotalSlideCount
        If lTmpSlideList(i) <> -1 Then
            g_lSlideCount = g_lSlideCount + 1
            g_lSlideList(g_lSlideCount) = i
            If i <> g_currentPresentation.Slides(i).SlideIndex Then
                MsgBox ("Index Problem")
            End If
        End If
    Next
End Sub

Public Sub FillDefaultSlideList()
    Dim i As Long
    
    g_lSlideCount = g_currentPresentation.Slides.Count
    ReDim g_lSlideList(1 To g_lSlideCount) As Long
    For i = 1 To g_lSlideCount
        g_lSlideList(i) = i
    Next
    
End Sub

Public Sub FillPlaceholderStrings()

    Dim tmpPres As Presentation
    Set tmpPres = Application.Presentations.Add(msoTrue)

    Dim tmpShp As Shape
    For Each tmpShp In tmpPres.SlideMaster.Shapes
        If tmpShp.Type = msoPlaceholder Then
        
            If tmpShp.PlaceholderFormat.Type = ppPlaceholderFooter Then
                g_szFooter = tmpShp.TextFrame.TextRange.text
            End If
            
            If tmpShp.PlaceholderFormat.Type = ppPlaceholderSlideNumber Then
                g_szSlideNumber = tmpShp.TextFrame.TextRange.text
            End If
            
            If tmpShp.PlaceholderFormat.Type = ppPlaceholderDate Then
                g_szDateAndTime = tmpShp.TextFrame.TextRange.text
            End If
            
        End If

    Next

    tmpPres.Close
    tmpPres = Nothing
     
End Sub

Public Function RunLsdFile(lsdFile As String) As Boolean
    
    Dim ret As Long
    
    ret = ShellExecute(0, StrPtr("open"), StrPtr(lsdFile), StrPtr(""), StrPtr(""), 1) ' winuser.h: #define SW_SHOWNORMAL       1
    If ret > 32 Then            ' return value greater than 32 if successful.
        RunLsdFile = True
    Else
        Dim ErrMSg As String
        ErrMSg = String(256, vbNullChar)
    
        GetOpenLSDFileErrorMsg (StrPtr(ErrMSg))
    
        Dim result As Long: result = MessageBoxW(0, StrPtr(ErrMSg), StrPtr(g_szDlgCaption), _
                                                 MB_ICONERROR Or MB_TOPMOST)
        RunLsdFile = False
    End If
    
End Function

Public Function ComputeTimeLeft(sStartSlideConversion As Single, lCurrentPage As Long, lTotalSlides As Long, ByRef sLastTimeLeft As Single) As String

    Dim sTimeForOnePage As Single
    Dim sTotalTime As Single: sTotalTime = 0
    Dim sCurrentTime As Single: sCurrentTime = Timer
    Dim sDuration As Single: sDuration = sCurrentTime - sStartSlideConversion

    sTimeForOnePage = sDuration / lCurrentPage
    
    If sLastTimeLeft = -1 Then
        sTotalTime = ((sTimeForOnePage * lTotalSlides) - sDuration)
    Else
        sTotalTime = ((sTimeForOnePage * lTotalSlides) - sDuration)
        If (sTotalTime - sLastTimeLeft) > 0 Then
            If (sTotalTime - sLastTimeLeft) > 10 Then
                sTotalTime = sLastTimeLeft + 5
            ElseIf (sTotalTime - sLastTimeLeft) > 5 Then
                sTotalTime = sLastTimeLeft + 3
            Else
                sTotalTime = sLastTimeLeft + 1
            End If
        Else
            If (sTotalTime - sLastTimeLeft) < -10 Then
                sTotalTime = sLastTimeLeft - 3
            Else
                sTotalTime = sLastTimeLeft - 1
            End If
            If sTotalTime <= 0 Then
                sTotalTime = 1
            End If
        End If
    End If
        
    sLastTimeLeft = sTotalTime
     
    ComputeTimeLeft = ConvertSecondsToTimeString(sTotalTime)
End Function

                  
' Initialisierung der globalen Variablen
Public Sub InitGlobalVariables()

    ' PowerPoint-Version setzen
    If StrComp(g_currentPresentation.Application.Version, "9.0", vbTextCompare) = 0 Then
        g_lApplicationVersion = 2000
    ElseIf StrComp(g_currentPresentation.Application.Version, "10.0", vbTextCompare) = 0 Then
        g_lApplicationVersion = 2002
    ElseIf StrComp(g_currentPresentation.Application.Version, "11.0", vbTextCompare) = 0 Then
        g_lApplicationVersion = 2003
    ElseIf StrComp(g_currentPresentation.Application.Version, "12.0", vbTextCompare) = 0 Then
        g_lApplicationVersion = 2007
	ElseIf StrComp(g_currentPresentation.Application.Version, "14.0", vbTextCompare) = 0 Then
        g_lApplicationVersion = 2007
    Else
        g_lApplicationVersion = 2002
    End If
        
    ' Foliengröße und Auflösung bestimmen (960x720 ~ 96x96)
    g_lSlideWidth = g_currentPresentation.PageSetup.SlideWidth
    g_lSlideHeight = g_currentPresentation.PageSetup.SlideHeight
    g_lHorizontalResolution = g_currentPresentation.PageSetup.SlideWidth * 96 / 960
    g_lVerticalResolution = g_currentPresentation.PageSetup.SlideHeight * 96 / 720
    
    ' Liste für nicht unterstützte Objekte initialisieren
    
    Dim iSlideCount As Long: iSlideCount = g_currentPresentation.Slides.Count
    ReDim g_lsNotSupported(1 To iSlideCount) As NotSupportedObjects
    Dim i As Long
    For i = 1 To iSlideCount
        ReDim g_lsNotSupported(i).iObjectList(1 To g_iNotSupportedObjectCount) As ObjectList
        Dim j As Integer
        For j = 1 To g_iNotSupportedObjectCount
            g_lsNotSupported(i).iObjectList(j).iObject = 0
            g_lsNotSupported(i).iObjectList(j).iObjectCount = 0
        Next
    Next
    g_iNotSupportedCount = 0
    
    ' Get language dependant messages
    Dim szMessage As String
    
    szMessage = String(256, vbNullChar)
    GetDiskFullMsg (StrPtr(szMessage))
    Dim endPos As Integer
    endPos = InStr(1, szMessage, vbNullChar, vbTextCompare)
    If endPos > 0 Then
        szMessage = Left(szMessage, endPos - 1)
    End If
    g_szMsgDiskFull = szMessage
    
    szMessage = String(256, vbNullChar)
    GetOverwriteImageMsg (StrPtr(szMessage))
    endPos = InStr(1, szMessage, vbNullChar, vbTextCompare)
    If endPos > 0 Then
        szMessage = Left(szMessage, endPos - 1)
    End If
    g_szMsgOverwriteImage = szMessage
    
    g_lCurrentPageNumber = 0
    g_lCurrentIndex = 1
    
End Sub

Public Sub LoadLibraries()

    ' Load pptresource.dll (Import wizard and string resources)
    m_pptResourceHandle = LoadPPTResources
    
    ' Load ppt2lecturnity.dll (Image conversions)
    m_ppt2LecturnityHandle = LoadPPT2Lecturnity
    
    ' Load Gdiplus Library
    m_gdiplusHandle = LoadGdiplus
    
End Sub

' Unload pptresources, ppt2lecturnity and gdiplus libraries
Public Sub FreeLibraries()

    ' Free the loaded libraries
    If m_pptResourceHandle <> 0 Then
        FreeLoadedLibrary (m_pptResourceHandle)
    End If
    
    If m_ppt2LecturnityHandle <> 0 Then
        FreeLoadedLibrary (m_ppt2LecturnityHandle)
    End If
    
    If m_gdiplusHandle <> 0 Then
        FreeGdiplus (m_gdiplusHandle)
    End If
    
End Sub
 
' Neuen Eintrag in Liste für nicht unterstützte Objekte einfügen
Public Sub SetObjectWarningMessage(iActualType As Integer, iSlideNumber As Long)

    Dim bInsertNewPage As Boolean: bInsertNewPage = False
    If g_iNotSupportedCount = 0 Then
        bInsertNewPage = True
    ElseIf g_lsNotSupported(g_iNotSupportedCount).iPage < iSlideNumber Then
        bInsertNewPage = True
    End If
    
    If bInsertNewPage Then
        g_iNotSupportedCount = g_iNotSupportedCount + 1
        g_lsNotSupported(g_iNotSupportedCount).iPage = iSlideNumber
    End If
    
    g_lsNotSupported(g_iNotSupportedCount).iObjectList(iActualType).iObject = g_lsNotSupported(g_iNotSupportedCount).iObjectList(iActualType).iObject + 1
    
End Sub

Public Function GetMessageCaption() As String

    Dim szMessage As String
    szMessage = String(256, vbNullChar)
    
    ' String to display in message box title
    GetMainCaption (StrPtr(szMessage))
    
    Dim endPos As Integer
    endPos = InStr(1, szMessage, vbNullChar, vbTextCompare)
    If endPos > 0 Then
        szMessage = Left(szMessage, endPos - 1)
    End If
    
    GetMessageCaption = szMessage
    
End Function


Public Function NoPresentationIsOpen() As Boolean

    Dim szMessage As String
    szMessage = String(256, vbNullChar)
    
    ' String to display if no presentation openend
    GetNoFileMsg (StrPtr(szMessage))
    Dim endPos As Integer
    endPos = InStr(1, szMessage, vbNullChar, vbTextCompare)
    If endPos > 0 Then
        szMessage = Left(szMessage, endPos - 1)
    End If
    
    Dim szNoFileMsg As String: szNoFileMsg = szMessage
    
    Dim result As Long: result = 0
    If Application.Presentations.Count < 1 Then
        result = MessageBoxW(0, StrPtr(szNoFileMsg), StrPtr(g_szDlgCaption), _
                             MB_ICONEXCLAMATION Or MB_TOPMOST)
        NoPresentationIsOpen = True
    Else
        NoPresentationIsOpen = False
    End If
    
End Function


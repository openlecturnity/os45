VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "AofDoc"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

#Const COMPILE_VERSION = 2007

' Imported templates
Public m_lecturnityTitleMaster          As aofTemplate  ' PowerPoint TitleMaster (only PowerPoint 2000 - PowerPoint 2003)

Private m_lecturnitySlideMasters()      As aofTemplate  ' PowerPoint SlideMaster
Private m_lecturnitySlideMasterCount    As Long

' Imported pages
Private m_lecturnityPages()             As aofSlide

Public m_tempSlide                      As Slide        ' temporary slide during the conversion.

' variable for text stream
Private m_ts                            As Object

  
''''''''''''''''''''''''''
' Constructor/Destructor '
''''''''''''''''''''''''''

Private Sub Class_Initialize()

    m_lecturnitySlideMasterCount = 0
    
End Sub

Private Sub Class_Terminate()

    On Error Resume Next
    
    ' Slides has been set to Nothing during the conversion.
    Dim i As Long
    For i = 1 To m_lecturnitySlideMasterCount
        Set m_lecturnitySlideMasters(i) = Nothing
    Next
    Erase m_lecturnitySlideMasters
    
    Set m_lecturnityTitleMaster = Nothing
End Sub
  
  
'''''''''''''''''''''''''''''''''''
' Public functions and procedures '
'''''''''''''''''''''''''''''''''''

Public Sub PreConversion()

    On Error Resume Next
    
    If Not CreateNewWBFile() Then
        g_conversionOptions.Failed = True
        Exit Sub
    End If
    
    WriteDocHeader
    
    GetLocalTime g_conversionOptions.TimeBegin
   
    ' Create empty page to ungroup shapes etc.
    Set m_tempSlide = g_currentPresentation.Slides.Add(g_currentPresentation.Slides.Count + 1, ppLayoutBlank)
    
    ' Get the placeholder texts
    FillPlaceholderStrings
    
    ReDim m_lecturnityPages(1 To g_lSlideCount) As aofSlide
    
End Sub

Public Sub PostConversion()
    On Error Resume Next
  
    If Not g_conversionOptions.Failed Then
        WriteDocEnd
        GetLocalTime g_conversionOptions.TimeEnd
    End If
    
    CloseWBFile
    
    m_tempSlide.Delete
    Set m_tempSlide = Nothing
    
End Sub

Public Sub FillSlideMaster_9_0(sld As Slide)
    Dim tmpMaster   As Master
    m_lecturnitySlideMasterCount = 1
    ReDim m_lecturnitySlideMasters(1 To 1) As aofTemplate
    Set m_lecturnitySlideMasters(1) = New aofTemplate
    Set tmpMaster = g_currentPresentation.SlideMaster
    If Err.Number = 0 Then
        m_lecturnitySlideMasters(1).ParseMaster sld, tmpMaster
    End If
    
    Dim bSuccess As Boolean
    bSuccess = ProgressParseMaster(1, 1)
    
    If bSuccess = False Then
        g_bConversionSucceeded = False
    End If
End Sub

Public Sub FillSlideMaster_10_0(sld As Slide)
    Dim tmpMaster   As Master
    Dim i As Long
    Dim bSuccess As Boolean
    m_lecturnitySlideMasterCount = g_currentPresentation.Designs.Count
    ReDim m_lecturnitySlideMasters(1 To m_lecturnitySlideMasterCount) As aofTemplate
    For i = 1 To m_lecturnitySlideMasterCount
        Set m_lecturnitySlideMasters(i) = New aofTemplate
        
        m_lecturnitySlideMasters(i).bHasTitleMaster = False
        
        If g_currentPresentation.Designs.Item(i).HasTitleMaster Then
            Set tmpMaster = g_currentPresentation.Designs.Item(i).TitleMaster
            m_lecturnitySlideMasters(i).bHasTitleMaster = True
            Set m_lecturnitySlideMasters(i).TitleMaster = New aofTemplate
            m_lecturnitySlideMasters(i).TitleMaster.ParseMaster sld, tmpMaster
        End If
        
        Set tmpMaster = g_currentPresentation.Designs.Item(i).SlideMaster
        
        If Err.Number = 0 Then
            m_lecturnitySlideMasters(i).ParseMaster sld, tmpMaster
        End If
        bSuccess = ProgressParseMaster(m_lecturnitySlideMasterCount, i)
        
        If bSuccess = False Then
            g_bConversionSucceeded = False
            Exit For
        End If
    Next
End Sub

Public Sub ConvertSlides(Optional bReset As Boolean = True)

    Dim i As Long
    Dim j As Long
    Dim dump As Double
    Dim percent As Single
    Dim rEnd  As Single
    Dim rTotal As Single
    Dim rBlock As Single
    Dim rWidth As Single
    Dim nBlockNum As Long
    Dim sCurrentTime As Single
    Dim szTimeLeft As String
    Dim sLastTimeLeft As Single: sLastTimeLeft = -1
    
    Dim iCurrentSlide As Long: iCurrentSlide = GetCurrentSlideNumber()
    
    ' Convert Slides
    Dim sStartSlideConversion As Single: sStartSlideConversion = Timer
    Dim bSuccess As Boolean: bSuccess = True
    Dim bSlideIsHidden As Boolean: bSlideIsHidden = False
    For i = 1 To g_lSlideCount
        If bSuccess Then
        
            If g_lApplicationVersion <> 2000 Then
                  ' In PowerPoint 2002 und 2003 (Version <> 9.0) koennen Seiten ausgeblendet werden
                If g_currentPresentation.Slides(g_lSlideList(i)).SlideShowTransition.Hidden Then
                     bSlideIsHidden = True
                 Else
                     bSlideIsHidden = False
                 End If
             End If
             
             If Not bSlideIsHidden Then
                 If Not g_conversionOptions.Failed Then
                     DoEvents
                     ConvertSlide g_lSlideList(i), i
                     ' Increase size of Label1 incrementally
                     bSuccess = ProgressImportSlide(g_lSlideCount, i)
                 Else
                     Exit For
                 End If
                 
             End If
             
        End If
        
        szTimeLeft = ComputeTimeLeft(sStartSlideConversion, i, g_lSlideCount, sLastTimeLeft)
        bSuccess = TimerUpdate(StrPtr(szTimeLeft), g_ImportWizardHwnd)
        If bSuccess = 0 Then
            bSuccess = False
        Else
            bSuccess = True
        End If
        
        If Not bSuccess Then
            g_bConversionSucceeded = False
            Exit For
        End If
    Next i
    
    bSuccess = EndProgress
    
    If bReset And g_currentPresentation.Slides.Count > 0 Then
         g_currentPresentation.Slides.Item(iCurrentSlide).Select
    End If
End Sub

' parse the templates which are used in current presentation
'
' check all slides, save the (converted) templates into the array Templates().
Public Sub ParseTemplates()

    If g_currentPresentation.Slides.Count < 1 Then
        Exit Sub
    End If
    
    Dim sld As Slide
    Set sld = g_currentPresentation.Slides(1)
    
    Set m_lecturnityTitleMaster = New aofTemplate
    On Error Resume Next
    Err.Clear
    
    Dim bSuccess As Boolean
    bSuccess = ProgressParseTitleMaster()
    
    If bSuccess = False Then
        g_bConversionSucceeded = False
        Exit Sub
    End If

    Dim tmpMaster   As Master
    Set tmpMaster = g_currentPresentation.TitleMaster
    If Err.Number = 0 Then
        m_lecturnityTitleMaster.ParseMaster sld, tmpMaster
    End If
    Err.Clear
    
    If g_lApplicationVersion = 2000 Then
        FillSlideMaster_9_0 sld
    Else
        FillSlideMaster_10_0 sld
    End If
    Err.Clear
    
    Dim i As Long
    For i = 1 To g_lSlideCount
        
        If bSuccess Then
            ExtractTemplate tmpMaster, i
            DoEvents
            bSuccess = ProgressImportTemplate(g_lSlideCount, i)
            If bSuccess = 0 Then
                bSuccess = False
            Else
                bSuccess = True
            End If
        End If
        
        If Not bSuccess Then
            g_bConversionSucceeded = False
            Exit For
        End If
        
    Next
    
End Sub


' parse the templates which are used in current presentation
'
' check all slides, save the (converted) templates into the array Templates().
Public Sub ParseTemplates_2007()

    If g_currentPresentation.Slides.Count < 1 Then
        Exit Sub
    End If
    
    InitializeMasters_2007
    
    Dim bSuccess As Boolean: bSuccess = True
    
    ' Needed for correct wizard display
    bSuccess = ProgressParseTitleMaster()
    
    If bSuccess Then
        bSuccess = ProgressParseMaster(1, 1)
    End If
    
    Dim i As Long
    For i = 1 To g_lSlideCount
        
        If bSuccess Then
            ExtractTemplate_2007 (i)
            DoEvents
            bSuccess = ProgressImportTemplate(g_lSlideCount, i)
        End If
        
        If Not bSuccess Then
            g_bConversionSucceeded = False
            Exit For
        End If
        
    Next
    
End Sub


''''''''''''''''''''''
' Private functions  '
''''''''''''''''''''''


''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Common Functions (PowerPoint 200, 2002, 2003, 2007 '
''''''''''''''''''''''''''''''''''''''''''''''''''''''

Private Sub ConvertSlide(lPPTIndex As Long, iLecturnityIndex As Long)

    On Error Resume Next
    
    ' conversion begin
    If m_lecturnityPages(iLecturnityIndex) Is Nothing Then
        Set m_lecturnityPages(iLecturnityIndex) = New aofSlide
    End If
    
    m_lecturnityPages(iLecturnityIndex).Perform g_currentPresentation.Slides.Item(lPPTIndex), lPPTIndex
    
    WriteToWBFile m_lecturnityPages(iLecturnityIndex).Script()
    
    Set m_lecturnityPages(iLecturnityIndex) = Nothing
    
End Sub

Private Sub InitPage(index As Long)

    Set m_lecturnityPages(index) = New aofSlide
    
End Sub

' create the output file
'
' args
' return        true : create successfully
'               false: error occured
Private Function CreateNewWBFile() As Boolean
    
    On Error Resume Next

    If Not DirectoryExists(g_conversionOptions.WBFilePath) Then
        MakeDirectory (g_conversionOptions.WBFilePath) ' if the dir exist, a run-time error raised.
    End If
    
    Dim szFilePath As String
    szFilePath = g_conversionOptions.WBFilePath & g_conversionOptions.WBFileName
    
    Const ForReading = 1, ForWriting = 2, ForAppending = 3
    Const TristateUseDefault = -2, TristateTrue = -1, TristateFalse = 0
    Dim fs, f, S
    Set fs = CreateObject("Scripting.FileSystemObject")
    fs.CreateTextFile szFilePath            'Erstellen einer Datei
    Set f = fs.GetFile(szFilePath)
    
    If IsEmpty(f) Then
        CreateNewWBFile = False
        
        If Err.Number <> 0 Then
            Dim result As Long: result = MessageBoxW(0, StrPtr(Err.Description), StrPtr(g_szDlgCaption), _
                                                     MB_ICONERROR Or MB_TOPMOST)
        End If
    Else
        Set m_ts = f.OpenAsTextStream(ForWriting, TristateTrue) ' TristateUseDefault = ASCII, TristateTrue = UNICODE
        CreateNewWBFile = True
    End If
    
End Function

Private Sub CloseWBFile()
    On Error GoTo DiskFullError:
    m_ts.Close
    Exit Sub
DiskFullError:
    If Err.Number = 61 Then
        g_conversionOptions.FailInfo = Err.Number 'Err.Description
'        MsgBox Err.Description
        g_conversionOptions.FailMsg = Err.Description
        g_conversionOptions.Failed = True
    Else
        Resume Next
    End If
End Sub

' write the document header
Private Sub WriteDocHeader()
    'WriteToWBFile "<ASSISTANT name=""" & g_conversionOptions.WBFileName & """>"
    Dim Name As String
    Name = Replace(Left(g_conversionOptions.WBFileName, Len(g_conversionOptions.WBFileName) - 4), " ", "_")
    WriteToWBFile "<ASSISTANT codepage=""" & GetCodepage() & """ name=""" & Name & """>"
End Sub


' write a string into the output file
Private Sub WriteToWBFile(str As String)

    On Error GoTo DiskFullError:
    
    m_ts.Write str
    
    Exit Sub
    
DiskFullError:

    If Err.Number = 61 Then
        g_conversionOptions.FailInfo = Err.Number 'Err.Description
'        MsgBox Err.Description
        g_conversionOptions.FailMsg = Err.Description
        g_conversionOptions.Failed = True
    Else
        Resume Next
    End If
    
End Sub

Private Sub WriteDocEnd()
    WriteToWBFile vbLf & "</ASSISTANT>"
End Sub


'''''''''''''''''''''''''''''''''''''''''''''
' Functions for PowerPoint 2000, 2002, 2003 '
'''''''''''''''''''''''''''''''''''''''''''''

Private Sub ExtractTemplate(tmpMaster As Master, index As Long)

    Dim tmpSlide    As Slide        ' temp variable
    
    Dim tmpTmpl     As aofTemplate
    Dim i           As Long
    Dim j           As Long
    
    Dim lPPTIndex As Long: lPPTIndex = g_lSlideList(index)
    Set tmpSlide = g_currentPresentation.Slides(lPPTIndex)
    InitPage (index)
    
    
    Dim bFound As Boolean: bFound = False

    If g_lApplicationVersion = 2000 Then
    
        If tmpSlide.Master.Name = m_lecturnityTitleMaster.m_szTemplateName Then
            Set tmpTmpl = m_lecturnityTitleMaster
            Set tmpMaster = g_currentPresentation.TitleMaster
        Else
            Set tmpTmpl = m_lecturnitySlideMasters(1)
            Set tmpMaster = g_currentPresentation.SlideMaster
        End If
    Else
        Dim iDsgIndex As Long
        iDsgIndex = GetDesignIndex(tmpSlide)
        
        If tmpSlide.layout = ppLayoutTitle And _
           m_lecturnitySlideMasters(iDsgIndex).bHasTitleMaster Then
            Set tmpTmpl = m_lecturnitySlideMasters(iDsgIndex).TitleMaster
            Set tmpMaster = GetTitleMaster(iDsgIndex)
        Else
            Set tmpTmpl = m_lecturnitySlideMasters(iDsgIndex)
            Set tmpMaster = GetSlideMaster(iDsgIndex)
        End If
    End If
    
    m_lecturnityPages(index).PerformTemplate tmpSlide, tmpMaster, tmpTmpl
    
    Set tmpTmpl = Nothing
    Set tmpMaster = Nothing
End Sub


'''''''''''''''''''''''''''''''''
' Functions for PowerPoint 2007 '
'''''''''''''''''''''''''''''''''

Private Sub ExtractTemplate_2007(index As Long)
  
#If COMPILE_VERSION = 2007 Then
    Dim lPPTIndex As Long: lPPTIndex = g_lSlideList(index)
    
    InitPage (index)

    Dim tmpSlide As Slide        ' temp variable
    Set tmpSlide = g_currentPresentation.Slides(lPPTIndex)
    
    Dim iDsgnIndex As Long: iDsgnIndex = GetDesignIndex(tmpSlide)
    Dim iLayoutIndex As Long: iLayoutIndex = GetCustomLayoutIndex(tmpSlide)
    
    Dim bSuccess As Boolean: bSuccess = True
    
    ' Extract slide master
    Dim pptSlideMaster As Master
    Set pptSlideMaster = GetSlideMaster(iDsgnIndex)
    If m_lecturnitySlideMasters(iDsgnIndex) Is Nothing Then
    
        Set m_lecturnitySlideMasters(iDsgnIndex) = New aofTemplate
        m_lecturnitySlideMasters(iDsgnIndex).ParseSlideTemplate_2007 tmpSlide, pptSlideMaster.Name, _
                                               pptSlideMaster.Shapes, pptSlideMaster.Background
        Dim iLayoutCount As Long: iLayoutCount = GetCustomLayoutCount(iDsgnIndex)
        m_lecturnitySlideMasters(iDsgnIndex).InitLayouts (iLayoutCount)
    End If
    
    Dim lecSlideMaster As aofTemplate
    Set lecSlideMaster = m_lecturnitySlideMasters(iDsgnIndex)
    
    ' Extract CustomLayout
    Dim lecCustomLayout As aofTemplate
    Set lecCustomLayout = m_lecturnitySlideMasters(iDsgnIndex).GetLayout(iLayoutIndex)
    
    If lecCustomLayout Is Nothing Then
        m_lecturnitySlideMasters(iDsgnIndex).InitLayout (iLayoutIndex)
        Set lecCustomLayout = m_lecturnitySlideMasters(iDsgnIndex).GetLayout(iLayoutIndex)
        
        If LayoutIsCustomLayout(iDsgnIndex, tmpSlide.CustomLayout.index) Then
            Dim sldLayout As CustomLayout
            Set sldLayout = pptSlideMaster.CustomLayouts(iLayoutIndex)
            lecCustomLayout.ParseSlideTemplate_2007 tmpSlide, _
                                                    sldLayout.Name, sldLayout.Shapes, _
                                                    sldLayout.Background
            If sldLayout.FollowMasterBackground = msoTrue Then
                lecCustomLayout.m_bFollowMasterBackground = True
            Else
                lecCustomLayout.m_bFollowMasterBackground = False
            End If
            If sldLayout.DisplayMasterShapes = msoTrue Then
                lecCustomLayout.m_bDisplayMasterShapes = True
            Else
                lecCustomLayout.m_bDisplayMasterShapes = False
            End If
        Else
            Dim sldMaster As Master: Set sldMaster = GetCustomLayoutAsMaster(iDsgnIndex, iLayoutIndex)
            lecCustomLayout.ParseSlideTemplate_2007 tmpSlide, sldMaster.Name, sldMaster.Shapes, sldMaster.Background
        End If
    End If
    
    ' Extract objects
    
    m_lecturnityPages(index).PerformTemplate_2007 tmpSlide, iDsgnIndex, iLayoutIndex, _
                                                  lecSlideMaster, lecCustomLayout
    
    
#End If
End Sub

Private Sub InitializeMasters_2007()

    m_lecturnitySlideMasterCount = g_currentPresentation.Designs.Count
    ReDim m_lecturnitySlideMasters(1 To m_lecturnitySlideMasterCount) As aofTemplate
    
End Sub

Private Function LayoutIsCustomLayout(iDsgnIndex As Long, iLayoutIndex As Long) As Boolean

#If COMPILE_VERSION = 2007 Then
    If TypeOf g_currentPresentation.Designs(iDsgnIndex).SlideMaster.CustomLayouts(iLayoutIndex) Is CustomLayout Then
        LayoutIsCustomLayout = True
    Else
        LayoutIsCustomLayout = False
    End If
#End If

End Function


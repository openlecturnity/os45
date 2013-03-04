VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "AofDoc"
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False

Option Explicit

'Private TemplateNum     As Long         ' the number of templates
'Private Templates()     As aofTemplate  ' array of templates

Private PageNum         As Long         ' the slide number of current presentation
Private Pages()         As aofSlide     ' array of aof pages

Private BeginPage       As Long         ' first page to be convert
Private EndPage         As Long         ' last page to be convert

Private FileHandle      As Integer      ' file handle of output file (.epx file)

Private PresDoc         As Presentation ' current presentation

Private PPTitleMaster   As aofTemplate  ' PowerPoint TitleMaster
Private PPSlideMaster   As aofTemplate  ' PowerPoint SlideMaster

' implement the conversion.
'
' args      pres    : presentation to be convert
'           beginpg : (optional, default = 0) the first page
'           pagnum  : (optional, default = 1) page number
Public Sub Perform(pres As Presentation, Optional beginPg As Long = 0, Optional pagnum As Long = 1)
    On Error Resume Next
    
    Set PresDoc = pres
    PageNum = PresDoc.Slides.Count
    
    ' set the conversion range
    If beginPg > 0 Then
        BeginPage = beginPg
        EndPage = beginPg + pagnum - 1
    Else
        BeginPage = 1
        EndPage = PageNum
    End If
    
    ReDim Pages(1 To PageNum) As aofSlide
    
    If Not CreateNewWBFile() Then
        CvtOpt.Failed = True
        Exit Sub
    End If
    
    WriteDocHeader
    
    GetLocalTime CvtOpt.TimeBegin
    
    ParseTemplates
    
    Dim i As Long
    Dim page As aofSlide
    
    With PresDoc.Slides         ' conversion begin
        If BeginPage > 0 Then
            For i = BeginPage To EndPage
                Pages(i).Perform .Item(i), i
                WriteToWBFile Pages(i).Script()
                Set Pages(i) = Nothing
            Next
        Else
            For i = 1 To PageNum
                Pages(i).Perform .Item(i), i
                WriteToWBFile Pages(i).Script()
                Set Pages(i) = Nothing
            Next
        End If
    End With
    
    WriteDocEnd
    CloseWBFile
    
    ' set the "Saved" property as true to avoid the save file warning.
    GlobalPres.Saved = msoTrue
    
    GetLocalTime CvtOpt.TimeEnd
    CvtOpt.Failed = False
End Sub

Public Sub PreConversion(pres As Presentation, Optional beginPg As Long = 0, Optional pagnum As Long = 1)
    On Error Resume Next
    
    Set PresDoc = pres
    PageNum = PresDoc.Slides.Count
    
    ' set the conversion range
    If beginPg > 0 Then ' And beginpg <= PageNum Then
        BeginPage = beginPg
        EndPage = beginPg + pagnum - 1
    Else
        BeginPage = 1
        EndPage = PageNum
    End If
    
    ReDim Pages(1 To PageNum) As aofSlide
    
    If Not CreateNewWBFile() Then
        CvtOpt.Failed = True
        Exit Sub
    End If
    
    CurPageNumber = 0
    CurrentPosition = 1
    
    WriteDocHeader
    
    GetLocalTime CvtOpt.TimeBegin
    
    Set GlobalTempSlide = GlobalPres.Slides.Add(GlobalPres.Slides.Count + 1, ppLayoutBlank)
    With GlobalTempSlide
        .DisplayMasterShapes = msoFalse
        With .HeadersFooters
            .DateAndTime.Visible = msoFalse
            .footer.Visible = msoFalse
            .SlideNumber.Visible = msoFalse
        End With
        .FollowMasterBackground = msoFalse
        .Background.Fill.Transparency = 1
        .Background.Fill.Visible = False
    End With
    
    Set GlobalTempTitleMaster = GlobalPres.Slides.Add(GlobalPres.Slides.Count + 1, ppLayoutTitle)
    With GlobalTempTitleMaster
        .DisplayMasterShapes = msoFalse
        With .HeadersFooters
            .DateAndTime.Visible = msoFalse
            .footer.Visible = msoFalse
            .SlideNumber.Visible = msoFalse
        End With
    End With
    
    Set GlobalTempSlideMaster = GlobalPres.Slides.Add(GlobalPres.Slides.Count + 1, ppLayoutTitle)
    With GlobalTempSlideMaster
        .DisplayMasterShapes = msoFalse
        With .HeadersFooters
            .DateAndTime.Visible = msoFalse
            .footer.Visible = msoFalse
            .SlideNumber.Visible = msoFalse
        End With
    End With
    
    ParseTemplates

End Sub

Public Sub ConvertSlide(i As Long)
    On Error Resume Next
   ' conversion begin
    If Pages(i) Is Nothing Then
        Set Pages(i) = New aofSlide
    End If
    Pages(i).Perform PresDoc.Slides.Item(i), i
    WriteToWBFile Pages(i).Script()
    Set Pages(i) = Nothing
End Sub

Public Sub PostConversion()
    GlobalTempSlide.Delete
    Set GlobalTempSlide = Nothing
    GlobalTempTitleMaster.Delete
    Set GlobalTempTitleMaster = Nothing
    GlobalTempSlideMaster.Delete
    Set GlobalTempSlideMaster = Nothing
    
    If Not CvtOpt.Failed Then
        WriteDocEnd
        GetLocalTime CvtOpt.TimeEnd
    End If
    CloseWBFile
End Sub

' create the output file
'
' args
' return        true : create successfully
'               false: error occured
Private Function CreateNewWBFile() As Boolean
    
    On Error Resume Next
    FileHandle = FreeFile
    
    Open CvtOpt.WBFilePath & CvtOpt.WBFileName For Output As #FileHandle
    If Err.Number = 0 Then
        CreateNewWBFile = True
    Else
        CreateNewWBFile = False
        MsgBox Err.Description, vbCritical, DlgCaption
    End If
End Function

' write the document header
Private Sub WriteDocHeader()
    'WriteToWBFile "<ASSISTANT name=""" & CvtOpt.WBFileName & """>"
    Dim Name As String
    Name = MyReplace(Left(CvtOpt.WBFileName, Len(CvtOpt.WBFileName) - 4), " ", "_")
    WriteToWBFile "<ASSISTANT name=""" & Name & """>"
End Sub

' write a string into the output file
Private Sub WriteToWBFile(str As String)
    On Error GoTo DiskFullError:
    Print #FileHandle, str
    Exit Sub
DiskFullError:
    If Err.Number = 61 Then
        CvtOpt.FailInfo = Err.Number 'Err.Description
'        MsgBox Err.Description
        CvtOpt.Failed = True
    Else
        Resume Next
    End If
End Sub

Private Sub WriteDocEnd()
    WriteToWBFile vbLf & "</ASSISTANT>"
End Sub

Private Sub CloseWBFile()
    On Error GoTo DiskFullError:
    Close #FileHandle
    Exit Sub
DiskFullError:
    If Err.Number = 61 Then
        CvtOpt.FailInfo = Err.Number 'Err.Description
'        MsgBox Err.Description
        CvtOpt.Failed = True
    Else
        Resume Next
    End If
End Sub

' parse the templates which are used in current presentation
'
' check all slides, save the (converted) templates into the array Templates().
Private Sub ParseTemplates()
    Dim i           As Long
    Dim j           As Long
    Dim tmpSlide    As Slide        ' temp variable
    Dim tmpMaster   As Master
    Dim tmpTmpl     As aofTemplate

    If PresDoc.Slides.Count < 1 Then
        Exit Sub
    End If

    Set PPTitleMaster = New aofTemplate
    Set PPSlideMaster = New aofTemplate

    On Error Resume Next
    Err.Clear
    Set tmpMaster = PresDoc.TitleMaster
    If Err.Number = 0 Then
        PPTitleMaster.ParseMaster tmpMaster
    Else
        Err.Clear
    End If
    Set tmpMaster = PresDoc.SlideMaster
    If Err.Number = 0 Then
        PPSlideMaster.ParseMaster tmpMaster
    Else
        Err.Clear
    End If

    GlobalSlideWidth = tmpMaster.Width
    GlobalSlideHeight = tmpMaster.Height
    
    For i = BeginPage To EndPage

        Set tmpSlide = PresDoc.Slides(i)
        Set Pages(i) = New aofSlide

        If tmpSlide.Master.Name = PPTitleMaster.Name Then
            Set tmpTmpl = PPTitleMaster
            Set tmpMaster = PresDoc.TitleMaster
        ElseIf tmpSlide.Master.Name = PPSlideMaster.Name Then
            Set tmpTmpl = PPSlideMaster
            Set tmpMaster = PresDoc.SlideMaster
        End If

        With Pages(i)
            .BackColor = GetBackgroundColor(tmpSlide)

            Set .CurTemplate = New aofTemplate
            .CurTemplate.InitObjs (tmpTmpl.ObjNum)
            
            .CurTemplate.SetHeadersFootersStrings tmpSlide.HeadersFooters
            
            If tmpSlide.DisplayMasterShapes Then
                For j = 1 To tmpTmpl.ObjNum
                    If tmpTmpl.IsTextObject(j) Then
                        .CurTemplate.ParseTextShape tmpMaster, j, i
                    Else
                        .CurTemplate.SetObjs j, tmpTmpl.GetObjs(j)      ' copy image objects
                    End If
                Next
            Else
                .CurTemplate.ObjNum = 0
            End If
            
            .CurTemplate.SetHeadersFootersStatus tmpSlide

            If tmpSlide.FollowMasterBackground Then
                Set .CurTemplate.Background = tmpTmpl.Background
            Else
                Set .CurTemplate.Background = New aofShape
                .CurTemplate.ParseBackground tmpSlide.Background
                ' the background is a Rectangle shape without dimension properties,
                ' an error arose when parsing it as a rectangle.
                ' So, we must set the properties correct here.
                Err.Clear
                With .CurTemplate.Background
                    .X = 0
                    .Y = 0
                    .Width = tmpSlide.Master.Width
                    .Height = tmpSlide.Master.Height
                    If .ShapeType <> aofShapeTypeImage Then
                        .ShapeType = aofShapeTypeRect
                    Else        ' the background is not solid filled
                        .bConvertToPNG = False      ' as Gabriela's Email (29.22.02)
                        .iCutTransOff = 0
                        .ConvertToPNG         ' convert wmf to png
                    End If
                End With
            End If
        End With

    Next
End Sub
Private Function GetMLBBackgroundColor(sld As Slide) As String
    GetMLBBackgroundColor = "#ffffff"
    With sld.Background
        If .Visible = msoTrue And .Fill.Visible = msoTrue Then
            Select Case .Fill.Type
            Case msoFillSolid
'                If sld.FollowMasterBackground = msoTrue Then    ' using the setting of Master Page
'                    GetMLBBackgroundColor = MLBColor(sld.Master.Background.Fill.ForeColor)
'                Else
                    GetMLBBackgroundColor = MLBColor(sld.Background.Fill.ForeColor)
'                End If
            Case Else
                'ErrLog aofErrUnknown, "Slide background is not fillsolid."
            End Select
        End If
    End With
End Function

Private Function GetBackgroundColor(sld As Slide) As String
    GetBackgroundColor = "#ffffff"
    If sld.FollowMasterBackground Then
        With sld.Parent.SlideMaster.Background
            If .Visible = msoTrue And .Fill.Visible = msoTrue Then
                Select Case .Fill.Type
                Case msoFillSolid
                    GetBackgroundColor = MLBColor(sld.Parent.SlideMaster.Background.Fill.ForeColor)
                Case Else
                    'ErrLog aofErrUnknown, "Slide background is not fillsolid."
                End Select
            End If
        End With
    Else
        With sld.Background
            If .Visible = msoTrue And .Fill.Visible = msoTrue Then
                Select Case .Fill.Type
                Case msoFillSolid
                    GetBackgroundColor = MLBColor(sld.Background.Fill.ForeColor)
                Case Else
                    'ErrLog aofErrUnknown, "Slide background is not fillsolid."
                End Select
            End If
        End With
    End If
End Function

Private Sub Class_Terminate()
    ' Slides has been set to Nothing during the conversion.
    Set PPSlideMaster = Nothing
    Set PPTitleMaster = Nothing
    Set PresDoc = Nothing
End Sub

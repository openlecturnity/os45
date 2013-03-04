VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "aofTemplate"
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False

Option Explicit

Public Name                As String

Private SlideLayout        As Long  ' As PpSlideLayout
Private BackColor           As String

Public Background          As aofShape
Private DateTimeString            As String
Private FooterString              As String
Private SlideNumberString         As String

Private ShowDateAndTime             As Boolean
Private ShowFooter                  As Boolean
Private ShowSlideNumber             As Boolean

Public TemplateIndex        As Long

Private Objs()              As aofShape
Public ObjNum              As Long

Private TextObjTable()          As Long

Public Sub InitObjs(num As Long)
    If num > 0 Then
        ObjNum = num
        ReDim Objs(1 To num) As aofShape
    End If
End Sub

Public Function IsTextObject(idx As Long) As Boolean
    IsTextObject = IIf(TextObjTable(idx) = 1, True, False)
End Function

Public Sub SetObjs(i As Long, obj As aofShape)
    Set Objs(i) = obj
End Sub

Public Function GetObjs(i As Long) As aofShape
    Set GetObjs = Objs(i)
End Function

Public Sub CopyObjs(tmpl As aofTemplate)
    Dim i As Long
    ObjNum = tmpl.ObjNum
    If ObjNum < 1 Then
        Exit Sub
    End If
    
    ReDim Objs(1 To ObjNum) As aofShape
    For i = 1 To ObjNum
        Set Objs(i) = tmpl.GetObjs(i)
    Next
End Sub

Public Sub ParseBackground(bg As ShapeRange)
    ' analyze background
    Dim mst
    With bg
        Set Background = New aofShape
        
        If .Item(1).Fill.Type = msoFillSolid Then
            Background.Perform .Item(1)
        Else
            Set mst = bg.Parent
            If mst.Name = "TitleMaster" Then
                Background.SetSlideAsImage GlobalTempTitleMaster
            ElseIf mst.Name = "SlideMaster" Then
                Background.SetSlideAsImage GlobalTempSlideMaster
            Else    ' bg is the private background of a slide.
                Background.SetAsImage .Item(1)
            End If
        End If
    End With
End Sub

Public Sub RefreshHeadersFootersStrings()
    Dim i As Long
    For i = 1 To ObjNum
        Objs(i).SetReplaceText DateTimeString, FooterString, SlideNumberString
    Next
End Sub

Public Sub SetHeadersFootersStrings(hdf As HeadersFooters)
    ' analyze headersfooters
    Dim TempState As Long 'MsoTriState
    Dim txt As String
    
    On Error Resume Next
    With hdf
        If .DateAndTime.Visible = msoTrue Then
            TempState = .DateAndTime.UseFormat
            If Err.Number <> 0 Then ' UseFormat not applied
                DateTimeString = ""
                Err.Clear
            Else
                If TempState = msoFalse Then    ' fixed date
                    DateTimeString = .DateAndTime.Text
                Else                            ' automatic date
                    ' in fact, the date format depends on the
                    ' property .Format, but it's not implemented now
                    DateTimeString = Format(Date, DateTimeFormatString(.DateAndTime.Format))
                End If
            End If
        Else
            DateTimeString = ""
        End If
        
        If .footer.Visible = msoTrue Then
            FooterString = .footer.Text
        Else
            FooterString = ""
        End If
        
        If .SlideNumber.Visible = msoTrue Then
            txt = .SlideNumber.Text
            If Err.Number <> 0 Then
                SlideNumberString = ""
                Err.Clear
            Else
                SlideNumberString = txt
            End If
        Else
            SlideNumberString = ""
        End If
    End With
End Sub

' only parse text shapes
Public Sub ParseTextShape(mst As Master, i As Long, sldNum As Long)
    With mst.Shapes
        Set Objs(i) = New aofShape
        
        Select Case .Item(i).Type
            Case MsoShapeType.msoPlaceholder
                Select Case .Item(i).PlaceholderFormat.Type
                    Case ppPlaceholderFooter, ppPlaceholderSlideNumber, ppPlaceholderDate
                        If .Item(i).PlaceholderFormat.Type = ppPlaceholderDate Then
                            Objs(i).SetFootersHeadersType aofDateTime
                        ElseIf .Item(i).PlaceholderFormat.Type = ppPlaceholderFooter Then
                            Objs(i).SetFootersHeadersType aofFooter
                        Else
                            Objs(i).SetFootersHeadersType aofSlideNumber
                        End If
                        Objs(i).PerformTemplateTextbox .Item(i), DateTimeString, FooterString, CStr(sldNum) 'SlideNumberString
                    Case Else
                      ' ignore these kinds of Placeholder.
                End Select
            Case MsoShapeType.msoGroup, MsoShapeType.msoPicture, _
                MsoShapeType.msoEmbeddedOLEObject, MsoShapeType.msoLinkedOLEObject, _
                MsoShapeType.msoLinkedPicture, MsoShapeType.msoMedia, _
                MsoShapeType.msoOLEControlObject
'                    Objs(i).SetAsImage .Item(i)
'                    Objs(i).SetVerticesRect .Item(i)   ' set bounding box as rectangle
'                    Objs(i).bConvertToPNG = True       ' convert wmf to png
'                    Objs(i).iCutTransOff = 0           ' preserve the transparent part
'                    Objs(i).ConvertToPNG               ' convert
'                    ImageTable(i) = 1               ' this object is an image
            Case Else
                Objs(i).Perform .Item(i)
                If .Item(i).HasTextFrame Then
                    If Objs(i).ShapeType = aofShapeTypeTextGroup Then  ' delete the text, the text may be incorrect
                        Objs(i).HasGroupItem = False
                        Objs(i).GroupItemNum = 0
                    End If
                    Objs(i).ContainText = True
                    Set Objs(i).TextInShape = New aofShape
                    Objs(i).TextInShape.PerformTemplateTextbox .Item(i), DateTimeString, FooterString, CStr(sldNum) 'SlideNumberString
                End If
        End Select
    End With
End Sub

Public Sub ParseMaster(mst As Master, Optional bImageOnly As Boolean = True)
    If mst Is Nothing Then
        ObjNum = 0
        Exit Sub
    End If
    
    On Error Resume Next
    
    Dim i As Long
    Dim TempState       As Long 'MsoTriState
    
    Name = mst.Name
    TemplateIndex = -1
    
    ' shapes and background, date/time, footer, slidenumber
    ObjNum = mst.Shapes.Count
    InitObjs (ObjNum)    'ReDim Objs(1 To mst.Shapes.Count) As aofShape
    
    ' A table indecate which object is image.
    ' ImageTable(n): 1,image; 0 not image
    ReDim TextObjTable(1 To mst.Shapes.Count) As Long
    
    ' analyze background
    ParseBackground mst.Background
    
    ' the background is a Rectangle shape without dimension properties,
    ' an error arose when parsing it as a rectangle.
    ' So, we must set the properties correct here.
    Err.Clear
    With Background
        .X = 0
        .Y = 0
        .Width = mst.Width
        .Height = mst.Height
        If .ShapeType <> aofShapeTypeImage Then
            .ShapeType = aofShapeTypeRect
        Else        ' the background is not solid filled
            .bConvertToPNG = False      ' as Gabriela's Email (29.22.02)
            .iCutTransOff = 0
            .ConvertToPNG         ' convert wmf to png
        End If
    End With
    
    With mst.Shapes
        For i = 1 To .Count
            Set Objs(i) = New aofShape
            
            Select Case .Item(i).Type
                Case MsoShapeType.msoPlaceholder
                    Select Case .Item(i).PlaceholderFormat.Type
                        Case ppPlaceholderFooter, ppPlaceholderSlideNumber, ppPlaceholderDate
                            If Not bImageOnly Then
                                Objs(i).PerformTemplateTextbox .Item(i), DateTimeString, FooterString, SlideNumberString
                            End If
                            TextObjTable(i) = 1
                        Case Else
                          ' ignore these kinds of Placeholder.
                    End Select
                Case MsoShapeType.msoGroup, MsoShapeType.msoPicture, _
                    MsoShapeType.msoEmbeddedOLEObject, MsoShapeType.msoLinkedOLEObject, _
                    MsoShapeType.msoLinkedPicture, MsoShapeType.msoMedia, _
                    MsoShapeType.msoOLEControlObject
                        If bImageOnly Then
                            Objs(i).SetAsImage .Item(i)
    '                        Objs(i).SetNewAnchor .Item(i)
                            Objs(i).SetVerticesRect .Item(i)   ' set bounding box as rectangle
                            Objs(i).bConvertToPNG = True       ' convert wmf to png
                            Objs(i).iCutTransOff = 0           ' preserve the transparent part
                            Objs(i).ConvertToPNG               ' convert
                        End If
                Case Else
                    Objs(i).Perform .Item(i)
                    If .Item(i).Type <> msoTextBox And .Item(i).HasTextFrame Then
                        If .Item(i).TextFrame.hasText Then
                            If .Item(i).TextFrame.TextRange.Text <> "" Then
                                If Objs(i).ShapeType = aofShapeTypeTextGroup Then  ' delete the text, the text may be incorrect
                                    Objs(i).HasGroupItem = False
                                    Objs(i).GroupItemNum = 0
                                End If
                                Objs(i).ContainText = True
                                Set Objs(i).TextInShape = New aofShape
                                Objs(i).TextInShape.PerformTemplateTextbox .Item(i), DateTimeString, FooterString, SlideNumberString
                                TextObjTable(i) = 1
                            End If
                        End If
                    End If
            End Select
            
        Next
    End With
    
End Sub

Public Function Script() As String
    Dim i           As Long
    
    On Error Resume Next
    InTemplateScript = True
    
    Script = Script & Background.Script
    
    For i = 1 To ObjNum
        If Objs(i) Is Nothing Then
        Else
            With Objs(i)
                If (Not ShowDateAndTime And .GetFootersHeadersType() = aofDateTime) Or _
                   (Not ShowFooter And .GetFootersHeadersType() = aofFooter) Or _
                   (Not ShowSlideNumber And .GetFootersHeadersType() = aofSlideNumber) Then
                   ' do not show this object.
                Else
                    Script = Script & .Script
                End If
            End With
        End If
    Next
    
    InTemplateScript = False
    
End Function

' Aug 29, 2002
' Something important:
' 1, Slide.Master = Presentation.SlideMaster or Presentation.TitleMaster
' 2, only if slide.DisplayMasterShapes = true, the shapes in Slide.Master
'    should be converted.
' 3, if Slide.FollowMasterBackground = true, the Background of Slide.Master
'    should be converted, otherwise, the Slide.Background should be converted.
Public Sub SetHeadersFootersStatus(sld As Slide)
    
    If sld.HeadersFooters.DateAndTime.Visible And sld.Master.HeadersFooters.DateAndTime.Visible Then
        ShowDateAndTime = True
    Else
        ShowDateAndTime = False
    End If
    
    ' determin if the footer should be shown.
    If sld.HeadersFooters.footer.Visible And sld.Master.HeadersFooters.footer.Visible Then
        ShowFooter = True
    Else
        ShowFooter = False
    End If
    
    If sld.HeadersFooters.SlideNumber.Visible And sld.Master.HeadersFooters.SlideNumber.Visible Then
        ShowSlideNumber = True
    Else
        ShowSlideNumber = False
    End If
        
    ' the Title Slide is special.
    If sld.Master.HeadersFooters.DisplayOnTitleSlide = msoFalse And sld.Layout = ppLayoutTitle Then
        ShowDateAndTime = False
        ShowFooter = False
        ShowSlideNumber = False
    End If

End Sub

Private Function DateTimeFormatString(daf As Long) As String
    Dim fmt As String
    Select Case daf
        Case PpDateTimeFormat.ppDateTimeMdyy
            fmt = "dd.mm.yy"
        Case PpDateTimeFormat.ppDateTimeddddMMMMddyyyy
            fmt = "dddd, mmmm dd. yyyy"
'        Case PpDateTimeFormat.ppDateTimedMMMMyyyy
'        Case PpDateTimeFormat.ppDateTimeMMMMdyyyy
'        Case PpDateTimeFormat.ppDateTimedMMMyy
'        Case PpDateTimeFormat.ppDateTimeMMMMyy
'        Case PpDateTimeFormat.ppDateTimeMMyy
'        Case PpDateTimeFormat.ppDateTimeMMddyyHmm
'        Case PpDateTimeFormat.ppDateTimeMMddyyhmmAMPM
'        Case PpDateTimeFormat.ppDateTimeHmm
'        Case PpDateTimeFormat.ppDateTimeHmmss
'        Case PpDateTimeFormat.ppDateTimehmmAMPM
'        Case PpDateTimeFormat.ppDateTimehmmssAMPM
        Case Else 'PpDateTimeFormat.ppDateTimeFormatMixed
            fmt = "dd.mm.yy"
    End Select
    
    DateTimeFormatString = fmt
End Function

Private Sub Class_Initialize()
    ObjNum = 0
End Sub

Private Sub Class_Terminate()
    Dim i As Long
    
    On Error Resume Next
    Set Background = Nothing
    For i = 1 To ObjNum
        Set Objs(i) = Nothing
    Next
    Erase TextObjTable
End Sub

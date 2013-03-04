VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "aofTemplate"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Public m_szTemplateName         As String
Public m_lecBackground          As aofShape
Public TemplateIndex            As Long

Private SlideLayout             As PpSlideLayout
Private BackColor               As String

Private DateTimeString          As String
Private FooterString            As String
Private SlideNumberString       As String

Private ShowDateAndTime         As Boolean
Private ShowFooter              As Boolean
Private ShowSlideNumber         As Boolean

Private m_templateObjects()     As aofShape
Private m_lObjectCount          As Long


Public bHasTitleMaster          As Boolean
Public TitleMaster              As aofTemplate

Private lecCustomLayouts()      As aofTemplate
Private iLecCustomLayoutNum     As Long

Private m_textObjects()         As Long

Public m_bFollowMasterBackground    As Boolean
Public m_bDisplayMasterShapes       As Boolean

Private Sub Class_Initialize()

    m_lObjectCount = 0
    iLecCustomLayoutNum = 0
    
    m_bFollowMasterBackground = True
    m_bDisplayMasterShapes = True
    
End Sub

Private Sub Class_Terminate()
    Dim i As Long
    
    On Error Resume Next
    Set m_lecBackground = Nothing
    For i = 1 To m_lObjectCount
        Set m_templateObjects(i) = Nothing
    Next
    Erase m_templateObjects
    
    For i = 1 To iLecCustomLayoutNum
        Set lecCustomLayouts(i) = Nothing
    Next
    Erase lecCustomLayouts
    
    Erase m_textObjects
    
End Sub


Public Sub InitializeObjects(num As Long)
    If num > 0 Then
        m_lObjectCount = num
        ReDim m_templateObjects(1 To num) As aofShape
    End If
End Sub

Public Sub InitLayouts(num As Long)
    If num > 0 Then
        iLecCustomLayoutNum = num
        ReDim lecCustomLayouts(1 To iLecCustomLayoutNum) As aofTemplate
    End If
End Sub

Public Function GetLayout(iNum As Long) As aofTemplate
    Set GetLayout = lecCustomLayouts(iNum)
End Function

Public Sub InitLayout(iNum As Long)
    Set lecCustomLayouts(iNum) = New aofTemplate
End Sub

Public Function IsTextObject(idx As Long) As Boolean
    IsTextObject = IIf(m_textObjects(idx) = 1, True, False)
End Function

Public Function GetObjectCount() As Long
    GetObjectCount = m_lObjectCount
End Function

Public Sub SetObjectCount(lObjectCount As Long)
    m_lObjectCount = lObjectCount
End Sub

Public Sub SetObjs(i As Long, obj As aofShape)
    Set m_templateObjects(i) = obj
End Sub

Public Function GetObjs(i As Long) As aofShape
    Set GetObjs = m_templateObjects(i)
End Function

Public Sub CopyObjs(tmpl As aofTemplate)
    Dim i As Long
    m_lObjectCount = tmpl.GetObjectCount()
    If m_lObjectCount < 1 Then
        Exit Sub
    End If
    
    ReDim m_templateObjects(1 To m_lObjectCount) As aofShape
    For i = 1 To m_lObjectCount
        Set m_templateObjects(i) = tmpl.GetObjs(i)
    Next
End Sub

Public Sub ParseBackground(sld As Slide, bg As ShapeRange)
    ' analyze background
    With bg
        Set m_lecBackground = New aofShape
        
        If .Item(1).Fill.Type = msoFillSolid Then
            m_lecBackground.Perform sld, .Item(1)
        Else
            m_lecBackground.ImportShapeAsImage .Item(1)
        End If
    End With
    
End Sub

Public Sub ParseBackground_2007(sld As Slide, szTemplateName As String, bg As ShapeRange)
    ' analyze background
    With bg
        Set m_lecBackground = New aofShape
        
        If .Item(1).Fill.Type = msoFillSolid Then
            If .Item(1).Type = msoAutoShape And _
               .Item(1).AutoShapeType = msoShapeRectangle Then
                m_lecBackground.SetBackgroundAsRect_2007 .Item(1)
            Else
                m_lecBackground.Perform sld, .Item(1)
            End If
        Else
            m_lecBackground.SetBackgroundAsImage_2007 sld, szTemplateName, .Item(1)
        End If
    End With
    
End Sub

Public Sub RefreshHeadersFootersStrings(slideNumber As Long)
    Dim i As Long
    For i = 1 To m_lObjectCount
        m_templateObjects(i).SetReplaceText DateTimeString, FooterString, CStr(slideNumber)
    Next
End Sub

Public Sub SetHeadersFootersStrings(hdf As HeadersFooters)
    ' analyze headersfooters
    Dim TempState As MsoTriState
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
                    DateTimeString = .DateAndTime.text
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
            FooterString = .footer.text
        Else
            FooterString = ""
        End If
        
        If .slideNumber.Visible = msoTrue Then
            txt = .slideNumber.text
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
Public Sub ParseTextShape(sld As Slide, mst As Master, i As Long)
    With mst.Shapes
        Set m_templateObjects(i) = New aofShape
        
        Select Case .Item(i).Type
            Case MsoShapeType.msoPlaceholder
                Select Case .Item(i).PlaceholderFormat.Type
                    Case ppPlaceholderFooter, ppPlaceholderSlideNumber, ppPlaceholderDate
                        m_templateObjects(i).PerformTemplateTextbox sld, .Item(i), DateTimeString, FooterString, CStr(sld.SlideIndex) 'SlideNumberString
                        If .Item(i).PlaceholderFormat.Type = ppPlaceholderDate Then
                            m_templateObjects(i).FootersHeadersType = LEC_FOOTERHEADER_DATETIME
                        ElseIf .Item(i).PlaceholderFormat.Type = ppPlaceholderFooter Then
                            m_templateObjects(i).FootersHeadersType = LEC_FOOTERHEADER_FOOTER
                        Else
                            m_templateObjects(i).FootersHeadersType = LEC_FOOTERHEADER_SLIDENUMBER
                        End If
                    Case Else
                      ' ignore these kinds of Placeholder.
                End Select
            Case MsoShapeType.msoGroup, MsoShapeType.msoPicture, _
                MsoShapeType.msoEmbeddedOLEObject, MsoShapeType.msoLinkedOLEObject, _
                MsoShapeType.msoLinkedPicture, MsoShapeType.msoMedia, _
                MsoShapeType.msoOLEControlObject
'                    m_templateObjects(i).SetAsImage .Item(i)
'                    m_templateObjects(i).SetVerticesRect .Item(i)   ' set bounding box as rectangle
'                    m_templateObjects(i).bConvertToPNG = True       ' convert wmf to png
'                    m_templateObjects(i).iCutTransOff = 0           ' preserve the transparent part
'                    m_templateObjects(i).ConvertToPNG               ' convert
'                    ImageTable(i) = 1               ' this object is an image
            Case Else
                m_templateObjects(i).Perform sld, .Item(i)
'                If .Item(i).HasTextFrame Then
'                    If m_templateObjects(i).ShapeType = LEC_SHAPETYPE_TEXTGROUP Then  ' delete the text, the text may be incorrect
'                        m_templateObjects(i).HasGroupItem = False
'                        m_templateObjects(i).GroupItemNum = 0
'                    End If
'                    m_templateObjects(i).ContainText = True
'                    Set m_templateObjects(i).TextInShape = New aofShape
'                    m_templateObjects(i).TextInShape.PerformTemplateTextbox .Item(i), DateTimeString, FooterString, CStr(sldNum) 'SlideNumberString
'                End If
        End Select
    End With
End Sub


' only parse text shapes
Public Sub ParseTextShape_2007(sld As Slide, masterShapes As Shapes, i As Long, lOffset As Long)
    With masterShapes
        Set m_templateObjects(i + lOffset) = New aofShape
        
        Select Case .Item(i).Type
            Case MsoShapeType.msoPlaceholder
                Select Case .Item(i).PlaceholderFormat.Type
                    Case ppPlaceholderFooter, ppPlaceholderSlideNumber, ppPlaceholderDate
                        m_templateObjects(i + lOffset).PerformTemplateTextbox sld, .Item(i), DateTimeString, FooterString, CStr(sld.SlideIndex) 'SlideNumberString
                        If .Item(i).PlaceholderFormat.Type = ppPlaceholderDate Then
                            m_templateObjects(i + lOffset).FootersHeadersType = LEC_FOOTERHEADER_DATETIME
                        ElseIf .Item(i).PlaceholderFormat.Type = ppPlaceholderFooter Then
                            m_templateObjects(i + lOffset).FootersHeadersType = LEC_FOOTERHEADER_FOOTER
                        Else
                            m_templateObjects(i + lOffset).FootersHeadersType = LEC_FOOTERHEADER_SLIDENUMBER
                        End If
                    Case Else
                      ' ignore these kinds of Placeholder.
                End Select
            Case MsoShapeType.msoGroup, MsoShapeType.msoPicture, _
                MsoShapeType.msoEmbeddedOLEObject, MsoShapeType.msoLinkedOLEObject, _
                MsoShapeType.msoLinkedPicture, MsoShapeType.msoMedia, _
                MsoShapeType.msoOLEControlObject
'                    m_templateObjects(i).SetAsImage .Item(i)
'                    m_templateObjects(i).SetVerticesRect .Item(i)   ' set bounding box as rectangle
'                    m_templateObjects(i).bConvertToPNG = True       ' convert wmf to png
'                    m_templateObjects(i).iCutTransOff = 0           ' preserve the transparent part
'                    m_templateObjects(i).ConvertToPNG               ' convert
'                    ImageTable(i) = 1               ' this object is an image
            Case Else
                m_templateObjects(i + lOffset).Perform sld, .Item(i)
'                If .Item(i).HasTextFrame Then
'                    If m_templateObjects(i).ShapeType = LEC_SHAPETYPE_TEXTGROUP Then  ' delete the text, the text may be incorrect
'                        m_templateObjects(i).HasGroupItem = False
'                        m_templateObjects(i).GroupItemNum = 0
'                    End If
'                    m_templateObjects(i).ContainText = True
'                    Set m_templateObjects(i).TextInShape = New aofShape
'                    m_templateObjects(i).TextInShape.PerformTemplateTextbox .Item(i), DateTimeString, FooterString, CStr(sldNum) 'SlideNumberString
'                End If
        End Select
    End With
End Sub

Public Sub ParseMaster(sld As Slide, mst As Master, Optional bImageOnly As Boolean = True)
    If mst Is Nothing Then
        m_lObjectCount = 0
        Exit Sub
    End If
    
    On Error Resume Next
    
    Dim i As Long
    Dim TempState       As MsoTriState
    
    m_szTemplateName = mst.Name
    TemplateIndex = -1
    
    ' shapes and background, date/time, footer, slidenumber
    m_lObjectCount = mst.Shapes.Count
    InitializeObjects (m_lObjectCount)    'ReDim Objs(1 To mst.Shapes.Count) As aofShape
    
    ' A table indecate which object is image.
    ' ImageTable(n): 1,image; 0 not image
    ReDim m_textObjects(1 To mst.Shapes.Count) As Long
    
    ' analyze background
    ParseBackground sld, mst.Background
    
    ' the background is a Rectangle shape without dimension properties,
    ' an error arose when parsing it as a rectangle.
    ' So, we must set the properties correct here.
    Err.Clear
    With m_lecBackground
        .x = 0
        .y = 0
        .width = mst.width
        .height = mst.height
        If .GetShapeType() <> LEC_SHAPETYPE_IMAGE Then
            .SetShapeType (LEC_SHAPETYPE_RECTANGLE)
        Else        ' the background is not solid filled
            .m_bConvertToPNG = False      ' as Gabriela's Email (29.22.02)
            .m_iCutTransOff = 0
            .ConvertToPNG         ' convert wmf to png
        End If
    End With
    
    With mst.Shapes
        For i = 1 To .Count
            Set m_templateObjects(i) = New aofShape
            
            Select Case .Item(i).Type
                Case MsoShapeType.msoPlaceholder
                    Select Case .Item(i).PlaceholderFormat.Type
                        Case ppPlaceholderFooter, ppPlaceholderSlideNumber, ppPlaceholderDate
                            If Not bImageOnly Then
                                m_templateObjects(i).PerformTemplateTextbox sld, .Item(i), DateTimeString, FooterString, SlideNumberString
                            End If
                            m_textObjects(i) = 1
                        Case Else
                          ' ignore these kinds of Placeholder.
                    End Select
                Case MsoShapeType.msoPicture, _
                    MsoShapeType.msoEmbeddedOLEObject, MsoShapeType.msoLinkedOLEObject, _
                    MsoShapeType.msoLinkedPicture, MsoShapeType.msoMedia, _
                    MsoShapeType.msoOLEControlObject
                        If bImageOnly Then
                            m_templateObjects(i).m_bConvertToPNG = True       ' convert wmf to png
                            m_templateObjects(i).ImportShapeAsImage .Item(i)
    '                        m_templateObjects(i).SetNewAnchor .Item(i)
                            m_templateObjects(i).SetVerticesRect .Item(i)   ' set bounding box as rectangle
                            m_templateObjects(i).m_iCutTransOff = 0           ' preserve the transparent part
                            m_templateObjects(i).ConvertToPNG               ' convert
                        End If
                Case Else
                    If .Item(i).Type = msoGroup Then
                        m_templateObjects(i).Perform sld, .Item(i)
                    Else
                        m_templateObjects(i).Perform sld, .Item(i)
                    End If
                    
                    m_templateObjects(i).SetZPosition 0
                
'                    If .Item(i).HasTextFrame Then
'                        If .Item(i).TextFrame.hasText Then
'                            If .Item(i).TextFrame.TextRange.Text <> "" Then
'                                If m_templateObjects(i).ShapeType = LEC_SHAPETYPE_TEXTGROUP Then  ' delete the text, the text may be incorrect
'                                    m_templateObjects(i).HasGroupItem = False
'                                    m_templateObjects(i).GroupItemNum = 0
'                                End If
'                                m_templateObjects(i).ContainText = True
'                                Set m_templateObjects(i).TextInShape = New aofShape
'                                m_templateObjects(i).TextInShape.PerformTemplateTextbox .Item(i), DateTimeString, FooterString, SlideNumberString
'                                m_textObjects(i) = 1
'                            End If
'                        End If
'                    End If
            End Select
            
        Next
    End With
    
End Sub


Public Sub ParseSlideTemplate_2007(currentSlide As Slide, szTemplateName As String, templateShapes As Shapes, _
                            templateBackground As ShapeRange, Optional bImageOnly As Boolean = True)
    
    On Error Resume Next
    
    ' analyze background
    ParseBackground_2007 currentSlide, szTemplateName, templateBackground
    
    If templateShapes.Count < 1 Then
        Exit Sub
    End If
    
    Dim i As Long
    
    m_szTemplateName = szTemplateName
    
    ' Initialize shapes for background items, date/time, footer and slide number
    m_lObjectCount = templateShapes.Count
    InitializeObjects (m_lObjectCount)    'ReDim m_templateObjects(1 To mst.Shapes.Count) As aofShape
    
    ' A table indecate which object is a text object.
    ' m_textObjects(n): 1 = text; 0 = no text
    ReDim m_textObjects(1 To templateShapes.Count) As Long
    
    ' the background is a Rectangle shape without dimension properties,
    ' an error arose when parsing it as a rectangle.
    ' So, we must set the properties correct here.
    Err.Clear
    With m_lecBackground
        .x = 0
        .y = 0
        .width = g_lSlideWidth
        .height = g_lSlideHeight
        If .GetShapeType() <> LEC_SHAPETYPE_IMAGE Then
            .SetShapeType (LEC_SHAPETYPE_RECTANGLE)
        Else        ' the background is not solid filled
            .m_bConvertToPNG = False      ' as Gabriela's Email (29.22.02)
            .m_iCutTransOff = 0
            .ConvertToPNG         ' convert wmf to png
        End If
    End With
    
    With templateShapes
        For i = 1 To .Count
            Set m_templateObjects(i) = New aofShape
            
            Select Case .Item(i).Type
                Case MsoShapeType.msoPlaceholder
                    Select Case .Item(i).PlaceholderFormat.Type
                        Case ppPlaceholderFooter, ppPlaceholderSlideNumber, ppPlaceholderDate
                            If Not bImageOnly Then
                                m_templateObjects(i).PerformTemplateTextbox currentSlide, .Item(i), DateTimeString, FooterString, SlideNumberString
                            End If
                            m_textObjects(i) = 1
                        Case Else
                          ' ignore these kinds of Placeholder.
                    End Select
                Case MsoShapeType.msoPicture, _
                    MsoShapeType.msoEmbeddedOLEObject, MsoShapeType.msoLinkedOLEObject, _
                    MsoShapeType.msoLinkedPicture, MsoShapeType.msoMedia, _
                    MsoShapeType.msoOLEControlObject
                        If bImageOnly Then
                            m_templateObjects(i).m_bConvertToPNG = True       ' convert wmf to png
                            Dim bSizecalculated As Boolean
                            m_templateObjects(i).PerformAsImage .Item(i)
                        End If
                Case Else
                
                    If .Item(i).Type = msoGroup Then
                        m_templateObjects(i).Perform currentSlide, .Item(i)
                    Else
                        m_templateObjects(i).Perform currentSlide, .Item(i)
                    End If
                    
                    m_templateObjects(i).SetZPosition 0
            End Select
            
        Next
    End With
    
End Sub

Public Function Script() As String
    Dim i           As Long
    
    On Error Resume Next
    g_shapeIsInTemplate = True
    
    Script = Script & m_lecBackground.Script
    
    For i = 1 To m_lObjectCount
        If m_templateObjects(i) Is Nothing Then
        Else
            With m_templateObjects(i)
                If (Not ShowDateAndTime And .FootersHeadersType = LEC_FOOTERHEADER_DATETIME) Or _
                   (Not ShowFooter And .FootersHeadersType = LEC_FOOTERHEADER_FOOTER) Or _
                   (Not ShowSlideNumber And .FootersHeadersType = LEC_FOOTERHEADER_SLIDENUMBER) Then
                   ' do not show this object.
                Else
                    Script = Script & .Script
                End If
            End With
        End If
    Next
    
    g_shapeIsInTemplate = False
    
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
    
    If sld.HeadersFooters.slideNumber.Visible And sld.Master.HeadersFooters.slideNumber.Visible Then
        ShowSlideNumber = True
    Else
        ShowSlideNumber = False
    End If
        
    ' the Title Slide is special.
    If sld.Master.HeadersFooters.DisplayOnTitleSlide = msoFalse And sld.layout = ppLayoutTitle Then
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

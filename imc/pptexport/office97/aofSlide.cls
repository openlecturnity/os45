VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "aofSlide"
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False

Option Explicit

Private SlideID         As String
Private SlideIndex      As Long             ' slide index in presentation, set in Perform()

Private SldWidth        As Long             ' width of current slide
Private SldHeight       As Long             ' height of current slide

Public CurTemplate      As aofTemplate      ' the template (reference)

Private Name            As String           ' slide name
Public BackColor        As String           ' background color

Private AnimList()      As aofAnimSetting   ' animation setting of current slide
Private AnimNum         As Long             ' element number of array "AnimList"

Private GroundLayer()   As aofShape         ' array of shapes without animation
Private GroundLayerNum  As Long             ' shape number of array "GroundLayer"

Private UpperLayers()   As aofLayer         ' array of layers (animation)
Private UpperLayerNum   As Long             ' layer number of array "UpperLayers"
                                            ' it may NOT equal to the number of overlays
                                            ' since one aofLayer may generate several overlays

Private ObjNum          As Long             ' the shape number of current slide

Private SldDoc          As Slide            ' current slide

' set the template
Public Sub ApplyTemplate(tmpl As aofTemplate)
    Set CurTemplate = tmpl
End Sub

' set the size of slide
Private Sub SetSlideFormat()
    SldWidth = CLng(SldDoc.Master.Width)
    SldHeight = CLng(SldDoc.Master.Height)
End Sub

' generate the script of current slide
'
' args
' return        script of current slide
Function Script() As String
    Dim i   As Long
    Dim strTemp  As String
    
    IncIndent
    
    ' slide header
    strTemp = NewLine() & "<PAGE name=""" & Name _
                & """ width=""" & SldWidth & """ height=""" & SldHeight _
                & """ color=""" & BackColor & """ slideid=""" & SlideID & """>"
    
    CurPageNumber = SlideIndex
    
    If CurTemplate Is Nothing Then
    Else
        CurTemplate.RefreshHeadersFootersStrings
        Script = Script & CurTemplate.Script()
    End If
    
    ' groundlayer
    For i = 1 To GroundLayerNum
        Script = Script & GroundLayer(i).Script
    Next
    
    If UpperLayerNum > 0 Then
        For i = 1 To UpperLayerNum
            Script = Script & UpperLayers(i).Script
        Next
    End If
    
    Script = strTemp & Script & NewLine() & "</PAGE>"
    
    DecIndent
    
End Function

Private Sub SetSlideID()
    SlideID = CStr(SldDoc.SlideID)
End Sub

Private Function SlashifySlideTitle(titleString As String) As String
    Dim i       As Long
    Dim ascV    As Long
    Dim returnS As String
    Dim babyS   As String
    
    returnS = ""
    For i = 1 To Len(titleString)
        babyS = Mid(titleString, i, 1)
        ascV = AscW(babyS)
        Select Case ascV
            Case 34, 8220, 8222
                babyS = "\"""     ' normale und typographische doppelte Anführungszeichen
            Case 8216, 8218
                babyS = "'"       ' typographische einfache Anführungszeichen
            Case 60
                babyS = "\<"
            Case 62
                babyS = "\>"
            Case 92
                babyS = "\\"
            Case 11
                babyS = " "       ' Zeilenumbruch gegen Space ersetzen
        End Select
        returnS = returnS & babyS
    Next
    
    SlashifySlideTitle = returnS
End Function

' Don't know what this was; the selection of characters to replace
' seems somewhat arbitrary....
' Replaced by SlashifySlideTitle()
Private Function ValidString(str As String) As String
    Dim i       As Long
    Dim ret     As String
    Dim baby    As String
    
    ret = ""
    For i = 1 To Len(str)
        baby = Mid(str, i, 1)
        Select Case AscW(baby)
            Case 8220, 8221, 34     ' " (left) and " (right)
                baby = "\"""
            Case 8216, 8217
                baby = "'"
            Case 11
                baby = " "
        End Select
        ret = ret & baby
    Next
    
    ValidString = ret
End Function

Public Sub Perform(pptSld As Slide, idx As Long)
    ' if there is a group shape, which is ungrouped and not
    ' regrouped, then the shape list of the slide will be
    ' changed, the Shapes(i) cannot get the right shape any
    ' more. Shps() caches the original shapes in order to
    ' avoid this problem.
    Dim shps() As Shape
    Dim zpos As Long
    
    Set SldDoc = pptSld
    
    SetSlideID
    SetSlideFormat
    
    SlideIndex = idx
    CurPageNumber = idx
    CurrentPosition = 1
    
    On Error Resume Next    ' the textRange may not exist.
    Name = SlashifySlideTitle(pptSld.Shapes.Title.TextFrame.TextRange.Text)
    On Error GoTo 0     ' close error trap
    
    'ErrLog aofErrSlideInfo, "" & SlideIndex
    
    ObjNum = SldDoc.Shapes.Count
    
    If ObjNum < 1 Then  'no shapes in slide.
        Exit Sub
    End If
    
    ' initial the object and animation setting
    PreProcess
    
    Dim i           As Long
    Dim CurObj      As aofShape
    Dim tmp         As Long
    Dim tmpSld      As Slide
    Dim tmpShp      As Shape
    
    ReDim shps(1 To ObjNum) As Shape
    
    For i = 1 To ObjNum
        Set shps(i) = SldDoc.Shapes(i)
    Next
    
    For i = 1 To ObjNum
        Set CurObj = New aofShape
        zpos = shps(i).ZOrderPosition
        If shps(i).Type = msoGroup Then
            Set tmpSld = GlobalPres.Slides.Add(GlobalPres.Slides.Count + 1, ppLayoutBlank)
            shps(i).Copy
            Set tmpShp = tmpSld.Shapes.Paste().Item(1)
            CurObj.Perform tmpShp
            'tmpShp.Delete
            tmpSld.Delete
        Else
            CurObj.Perform shps(i) 'SldDoc.Shapes(i)
        End If
            
        CurObj.SetZPosition zpos
        
        If AnimList(i).AnimOrder = 0 Then       ' no animation
            tmp = tmp + 1
            Set GroundLayer(tmp) = CurObj       ' add to GroundLayer
        Else    ' with aniamtion
            Set UpperLayers(AnimList(i).UpperLevelIndex) = New aofLayer
            UpperLayers(AnimList(i).UpperLevelIndex).Perform CurObj, AnimList(i).AnimOrder, _
                    AnimList(i).HasSubAnim, AnimList(i).Resistant, _
                    AnimList(i).AnimInReverse, AnimList(i).AnimBeforeLevel ' add ot UpperLayer
        End If
    Next
    
    Erase shps
    ' release the reference
    Set SldDoc = Nothing
    
End Sub

' set animation setting of each shape
Private Sub PreProcess()
    If ObjNum < 1 Then
        Exit Sub
    End If
    
    AnimNum = ObjNum
    ReDim AnimList(1 To AnimNum) As aofAnimSetting
    
    On Error Resume Next
    Dim i   As Long
    For i = 1 To AnimNum
        With SldDoc.Shapes(i).AnimationSettings
            If .Animate = msoTrue Then      ' with animation
                AnimList(i).AnimOrder = .AnimationOrder
                Select Case .AfterEffect
                    Case ppAfterEffectHide, ppAfterEffectHideOnClick
                        AnimList(i).Resistant = 0
                    Case Else
                        AnimList(i).Resistant = 1
                End Select
                Select Case .TextLevelEffect
                    Case ppAnimateLevelNone, ppAnimateByAllLevels, ppAnimateLevelMixed
                        AnimList(i).HasSubAnim = False
                    Case Else
                        AnimList(i).HasSubAnim = True
                        AnimList(i).AnimBeforeLevel = .TextLevelEffect
                        If .AnimateTextInReverse = msoTrue Then
                            AnimList(i).AnimInReverse = True
                        End If
                End Select
                UpperLayerNum = UpperLayerNum + 1
            End If
        End With
    Next
    
    SortAnimIndex
    
    ' get the shape number of Groundlayer
    GroundLayerNum = SldDoc.Shapes.Count - UpperLayerNum
    
    ' init GroundLayer
    If GroundLayerNum > 0 Then
        ReDim GroundLayer(1 To GroundLayerNum) As aofShape
    End If
    
    ' init UpperLayer
    If UpperLayerNum > 0 Then
        ReDim UpperLayers(1 To UpperLayerNum) As aofLayer
    End If
    
    On Error GoTo 0
        
End Sub

Private Sub SortAnimIndex()
    Dim i As Long
    Dim ord As Long
    Dim idx As Long
    
    idx = 0
    ord = 1
    i = 1
    Do While idx < UpperLayerNum
        If AnimList(i).AnimOrder = ord Then
            idx = idx + 1
            AnimList(i).UpperLevelIndex = idx
        End If
        i = i + 1
        If i > AnimNum Then
            ord = ord + 1
            i = 1
        End If
    Loop
End Sub

Private Sub Class_Terminate()
    Dim i As Long
    
    On Error Resume Next
    Set CurTemplate = Nothing
    For i = 1 To GroundLayerNum
        Set GroundLayer(i) = Nothing
    Next i
    For i = 1 To UpperLayerNum
        Set UpperLayers(i) = Nothing
    Next
    Set SldDoc = Nothing
End Sub



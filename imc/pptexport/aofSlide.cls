VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "aofSlide"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

#Const COMPILE_VERSION = 2007

Public m_lecTemplate        As aofTemplate      ' the template (reference)
Public m_szBackgroundColor  As String           ' background color

Private m_pptSlide          As Slide            ' current PowerPoint slide
Private m_szName            As String           ' slide name
Private m_szSlideID         As String
Private m_lSlideIndex       As Long             ' slide index in presentation, set in Perform()
Private m_lSlideNumber      As Long

Private m_lSlideWidth       As Long             ' width of current slide
Private m_lSlideHeight      As Long             ' height of current slide

Private m_lObjectCount      As Long                 ' the shape number of current slide

Private m_AnimationList()   As aofAnimSetting   ' animation setting of current slide
Private m_lAnimationCount   As Long             ' element number of array "AnimList"

Private m_GroundLayer()     As ExtendedShape    ' array of shapes without animation
Private m_lGroundLayerCount As Long             ' shape number of array "GroundLayer"

Private m_UpperLayers()     As aofLayer         ' array of layers (animation)
Private m_lUpperLayerCount  As Long             ' layer number of array "UpperLayers"
                                                ' it may NOT equal to the number of overlays
                                                ' since one aofLayer may generate several overlays
   
''''''''''''''''''''''''''
' Constructor/Destructor '
''''''''''''''''''''''''''

Private Sub Class_Initialize()
    m_lAnimationCount = 0
    m_lGroundLayerCount = 0
    m_lObjectCount = 0

    m_szBackgroundColor = "#ffffff"
End Sub

Private Sub Class_Terminate()
    Dim i As Long
    
    On Error Resume Next
    
    Set m_lecTemplate = Nothing
    
    For i = 1 To m_lGroundLayerCount
        Set m_GroundLayer(i).lecturnityShape = Nothing
    Next i
    Erase m_GroundLayer
    
    For i = 1 To m_lUpperLayerCount
        Set m_UpperLayers(i) = Nothing
    Next
    Erase m_UpperLayers
    
    Erase m_AnimationList
    
    Set m_pptSlide = Nothing
End Sub
  
  
'''''''''''''''''''''''''''''''''''
' Public functions and procedures '
'''''''''''''''''''''''''''''''''''

Public Sub Perform(pptSld As Slide, idx As Long)
   
    
    Set m_pptSlide = pptSld
    
    SetSlideID
    SetSlideDimension
    
    m_lSlideNumber = pptSld.slideNumber
    
    m_lSlideIndex = idx
    g_lCurrentPageNumber = idx
    g_lCurrentIndex = 1
    
    On Error Resume Next    ' the textRange may not exist.
    m_szName = SlashifySlideTitle(pptSld.Shapes.Title.TextFrame.TextRange.text)
    On Error GoTo 0     ' close error trap
    
    'ErrLog LEC_ERRORTYPE_SLIDEINFO, "" & m_lSlideIndex
    
    m_lObjectCount = m_pptSlide.Shapes.Count
    
    If m_lObjectCount < 1 Then  'no shapes in slide.
        Exit Sub
    End If
    
    
    ' initial the object and animation setting
    If g_lApplicationVersion = 2000 Then
        PreProcess_9_0
        PerformObjects_9_0
    Else
        PreProcess_10_0
        PerformObjects_10_0
    End If
End Sub


Public Sub PerformTemplate(pptSlide As Slide, _
                           pptMaster As Master, lecTemplate As aofTemplate)

    m_szBackgroundColor = GetBackgroundColor(pptSlide)
    
    Set m_lecTemplate = New aofTemplate
    m_lecTemplate.InitializeObjects (lecTemplate.GetObjectCount())
    
    m_lecTemplate.SetHeadersFootersStrings pptSlide.HeadersFooters
    m_lecTemplate.SetHeadersFootersStatus pptSlide
        
    Dim j As Long
    If pptSlide.DisplayMasterShapes Then
        For j = 1 To lecTemplate.GetObjectCount()
            If lecTemplate.IsTextObject(j) Then
                m_lecTemplate.ParseTextShape pptSlide, pptMaster, j
            Else
                m_lecTemplate.SetObjs j, lecTemplate.GetObjs(j)      ' copy image objects
            End If
        Next
    Else
        m_lecTemplate.SetObjectCount (0)
    End If
    
        If pptSlide.FollowMasterBackground Then
            Set m_lecTemplate.m_lecBackground = lecTemplate.m_lecBackground
        Else
        Set m_lecTemplate.m_lecBackground = New aofShape
        m_lecTemplate.ParseBackground pptSlide, pptSlide.Background
        ' the background is a Rectangle shape without dimension properties,
        ' an error arose when parsing it as a rectangle.
        ' So, we must set the correct properties here.
        Err.Clear
        With m_lecTemplate.m_lecBackground
            .x = 0
            .y = 0
            .width = pptSlide.Master.width
            .height = pptSlide.Master.height
            If .GetShapeType() <> LEC_SHAPETYPE_IMAGE Then
                .SetShapeType (LEC_SHAPETYPE_RECTANGLE)
            Else        ' the background is not solid filled
                .m_bConvertToPNG = False      ' as Gabriela's Email (29.22.02)
                .m_iCutTransOff = 0
                .ConvertToPNG         ' convert wmf to png
            End If
        End With
    End If
End Sub

Public Sub PerformTemplate_2007(pptSlide As Slide, _
                                iDesignIndex As Long, iLayoutIndex As Long, _
                                lecSlideMaster As aofTemplate, lecCustomLayout As aofTemplate)

#If COMPILE_VERSION = 2007 Then
    Dim pptSlideMaster As Master
    Set pptSlideMaster = GetSlideMaster(iDesignIndex)
        
    Set m_lecTemplate = New aofTemplate
        
        If pptSlide.DisplayMasterShapes Then
        
        Dim lObjects As Long: lObjects = 0
        lObjects = lecCustomLayout.GetObjectCount()
        
        If lecCustomLayout.m_bDisplayMasterShapes Then
            lObjects = lObjects + lecSlideMaster.GetObjectCount()
        End If

        m_lecTemplate.InitializeObjects (lObjects)

        m_lecTemplate.SetHeadersFootersStrings pptSlide.HeadersFooters
        m_lecTemplate.SetHeadersFootersStatus pptSlide

        
        Dim lStartIndex As Long: lStartIndex = 0
        Dim j As Long
        
        If lecCustomLayout.m_bDisplayMasterShapes Then
            For j = 1 To lecSlideMaster.GetObjectCount()
                If lecSlideMaster.IsTextObject(j) Then
                    m_lecTemplate.ParseTextShape_2007 _
                        pptSlide, pptSlideMaster.Shapes, j, lStartIndex
                Else
                    m_lecTemplate.SetObjs j + lStartIndex, lecSlideMaster.GetObjs(j)
                End If
            Next
            lStartIndex = lecSlideMaster.GetObjectCount()
        End If
        
        For j = 1 To lecCustomLayout.GetObjectCount()
            If lecCustomLayout.IsTextObject(j) Then
                m_lecTemplate.ParseTextShape_2007 _
                    pptSlide, pptSlideMaster.CustomLayouts(iLayoutIndex).Shapes, j, lStartIndex
            Else
                m_lecTemplate.SetObjs j + lStartIndex, lecCustomLayout.GetObjs(j)
            End If
        Next

    
    Else
        m_lecTemplate.SetObjectCount (0)
    End If
    
    If pptSlide.FollowMasterBackground Then
        If lecCustomLayout.m_bFollowMasterBackground Then
            m_szBackgroundColor = GetBackgroundColor_2007(pptSlideMaster.Background)
            Set m_lecTemplate.m_lecBackground = lecSlideMaster.m_lecBackground
        Else
            m_szBackgroundColor = GetBackgroundColor_2007(pptSlideMaster.CustomLayouts(iLayoutIndex).Background)
            Set m_lecTemplate.m_lecBackground = lecCustomLayout.m_lecBackground
        End If
    Else
        m_szBackgroundColor = GetBackgroundColor_2007(pptSlide.Background)
            
        Set m_lecTemplate.m_lecBackground = New aofShape
        m_lecTemplate.ParseBackground_2007 pptSlide, "", pptSlide.Background
        ' the background is a Rectangle shape without dimension properties,
        ' an error arose when parsing it as a rectangle.
        ' So, we must set the correct properties here.
        Err.Clear
        With m_lecTemplate.m_lecBackground
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
    End If
    
#End If
    
End Sub

' generate the script of current slide
'
' args
' return        script of current slide
Public Function Script() As String
    Dim i   As Long
    Dim strTemp  As String
    
    On Error Resume Next
    IncIndent
    
    ' slide header
    strTemp = NewLine() & "<PAGE name=""" & m_szName _
                & """ width=""" & m_lSlideWidth & """ height=""" & m_lSlideHeight _
                & """ color=""" & m_szBackgroundColor & """ slideid=""" & m_szSlideID & """>"
    
    g_lCurrentPageNumber = m_lSlideIndex
    
    If m_lecTemplate Is Nothing Then
    Else
        m_lecTemplate.RefreshHeadersFootersStrings m_lSlideNumber
        Script = Script & NewLine() _
                 & "<GROUP type=""MASTER"">" _
                 & m_lecTemplate.Script() & NewLine() _
                 & "</GROUP>"
    End If
    
    ' groundlayer
    For i = 1 To m_lGroundLayerCount
        Script = Script & m_GroundLayer(i).lecturnityShape.Script
    Next
    
    If m_lUpperLayerCount > 0 Then
        For i = 1 To m_lUpperLayerCount
            If Not (m_UpperLayers(i) Is Nothing) Then
                Script = Script & m_UpperLayers(i).Script
            End If
        Next
    End If
    
    Script = strTemp & Script & NewLine() & "</PAGE>"
    
    DecIndent
    
    On Error GoTo 0
    
End Function

' set the size of slide
Private Sub SetSlideDimension()
    m_lSlideWidth = g_lSlideWidth
    m_lSlideHeight = g_lSlideHeight
End Sub

Private Sub SetSlideID()
    m_szSlideID = CStr(m_pptSlide.SlideID)
End Sub
                                     
                                     
'
' Private Functions
'

' set animation setting of each shape
Private Sub PreProcess_9_0()
    If m_lObjectCount < 1 Then
        Exit Sub
    End If
    
    m_lAnimationCount = m_lObjectCount
    ReDim m_AnimationList(1 To m_lAnimationCount) As aofAnimSetting
    
    On Error Resume Next
    Dim i   As Long
    For i = 1 To m_lAnimationCount
        With m_pptSlide.Shapes(i).AnimationSettings
            If .Animate = msoTrue Then      ' with animation
                m_AnimationList(i).AnimOrder = .AnimationOrder
                m_AnimationList(i).AfterEffect = .AfterEffect
                Select Case .TextLevelEffect
                    Case ppAnimateLevelNone, ppAnimateByAllLevels, ppAnimateLevelMixed
                        m_AnimationList(i).HasSubAnim = False
                    Case Else
                        m_AnimationList(i).HasSubAnim = True
                        m_AnimationList(i).AnimBeforeLevel = .TextLevelEffect
                        If .AnimateTextInReverse = msoTrue Then
                            m_AnimationList(i).AnimInReverse = True
                        End If
                End Select
                m_lUpperLayerCount = m_lUpperLayerCount + 1
            End If
        End With
    Next
    
    SortAnimIndex
    
    ' get the shape number of Groundlayer
    m_lGroundLayerCount = m_pptSlide.Shapes.Count - m_lUpperLayerCount
    
    ' init GroundLayer
    If m_lGroundLayerCount > 0 Then
        ReDim m_GroundLayer(1 To m_lGroundLayerCount) As ExtendedShape
    End If
    
    ' init UpperLayer
    If m_lUpperLayerCount > 0 Then
        ReDim m_UpperLayers(1 To m_lUpperLayerCount) As aofLayer
    End If
    
    On Error GoTo 0
    
End Sub

' set animation setting of each shape
Private Sub PreProcess_10_0()

    Dim i   As Long
    
    If m_lObjectCount < 1 Then
        Exit Sub
    End If
    
    On Error Resume Next
    
    ' get animated object count
    m_lAnimationCount = GetAnimationCount()
    ReDim m_AnimationList(1 To m_lAnimationCount) As aofAnimSetting
    
    On Error Resume Next
    
    ' There have to be space for all objects
    Dim ObjectCount As LoadPictureConstants
    ObjectCount = m_pptSlide.Shapes.Count
    
    Dim paragraphs() As Long
    Dim ParagraphCount As Long
    
    ' init GroundLayer
    If ObjectCount > 0 Then
    
        ReDim m_GroundLayer(1 To ObjectCount) As ExtendedShape
        
        m_lGroundLayerCount = 0
        For i = 1 To ObjectCount
            Dim bInsertObject As Boolean: bInsertObject = True
            ParagraphCount = 0
            
            ' search for object in list of animated objects
            With m_pptSlide.TimeLine
    
                Dim bAnimatedCompletely As Boolean: bAnimatedCompletely = False
                Dim animationEffect As effect
                Set animationEffect = m_pptSlide.TimeLine.MainSequence.FindFirstAnimationFor(m_pptSlide.Shapes(i))
                If Not (animationEffect Is Nothing) Then
                    If animationEffect.Shape Is Nothing Then
                        bAnimatedCompletely = True
                        If animationEffect.effectType <> msoAnimEffectMediaPause And _
                           animationEffect.effectType <> msoAnimEffectMediaPlay And _
                           animationEffect.effectType <> msoAnimEffectMediaStop Then
                            bInsertObject = False
                        End If
                    End If
                    Dim j As Long: j = 1
                    Do While (Not bAnimatedCompletely) And (j <= .MainSequence.Count)
                        If Not (.MainSequence.Item(j).Shape Is Nothing) Then
                            If m_pptSlide.Shapes(i).Id = .MainSequence.Item(j).Shape.Id Then
                                Dim index As Long: index = GetParagraphIndex(.MainSequence, j)
                                '
                                Dim bAnimated As Boolean
                                bAnimated = IsAnimated(.MainSequence.Item(j))
                                ' if there is no special paragraph animated in a text object
                                ' the whole object is animated
                                If bAnimated Then
                                    bAnimatedCompletely = Not (HasTextInShape(.MainSequence.Item(j).Shape) And index <> 0)
                                    ' highlighted objects and text objects which are not
                                    ' completely animated will be inserted in GroundLayer
                                    bInsertObject = Not bAnimatedCompletely
                                    ' if shape is already inserted in GroundLayer only store
                                    ' paragraph
                                    If bInsertObject Then
                                        ' allocate space for paragraphs. There can be not more paragraphs than animated objects.
                                        If ParagraphCount = 0 Then
                                            ReDim paragraphs(1 To .MainSequence.Count) As Long
                                        End If
                                        Dim bIsAlreadyInserted As Boolean: bIsAlreadyInserted = False
                                        Dim k As Long
                                        For k = 1 To ParagraphCount
                                            If paragraphs(k) = .MainSequence.Item(j).Paragraph Then
                                                bIsAlreadyInserted = True
                                                Exit For
                                            End If
                                        Next
                                        If Not bIsAlreadyInserted Then
                                            ParagraphCount = ParagraphCount + 1
                                            paragraphs(ParagraphCount) = .MainSequence.Item(j).Paragraph
                                        End If
                                    End If
                                End If
                            End If
                        End If
                        j = j + 1
                    Loop
                End If
                
                For k = 1 To .InteractiveSequences.Count
                    j = 1
                    Do While (Not bAnimatedCompletely) And (j <= .InteractiveSequences(k).Count)
                        If Not (.InteractiveSequences(k).Item(j).Shape Is Nothing) Then
                        
                            If m_pptSlide.Shapes(i).Id = .InteractiveSequences(k).Item(j).Shape.Id Then
                                index = GetParagraphIndex(.InteractiveSequences(k), j)
                                '
                                bAnimated = False
                                If .InteractiveSequences(k).Item(j).Timing.TriggerType = msoAnimTriggerOnShapeClick Then
                                    If .InteractiveSequences(k).Item(j).effectType <> msoAnimEffectMediaPause And _
                                       .InteractiveSequences(k).Item(j).effectType <> msoAnimEffectMediaPlay And _
                                       .InteractiveSequences(k).Item(j).effectType <> msoAnimEffectMediaStop Then
                                        bAnimated = True
                                    End If
                                End If
                                ' if there is no special paragraph animated in a text object
                                ' the whole object is animated
                                If bAnimated Then
                                    bAnimatedCompletely = Not (HasTextInShape(.InteractiveSequences(k).Item(j).Shape) And index <> 0)
                                    ' highlighted objects and text objects which are not
                                    ' completely animated will be inserted in GroundLayer
                                    bInsertObject = Not bAnimatedCompletely
                                    ' if shape is already inserted in GroundLayer only store
                                    ' paragraph
                                    If bInsertObject Then
                                        ' allocate space for paragraphs. There can be not more paragraphs than animated objects.
                                        If ParagraphCount = 0 Then
                                            ReDim paragraphs(1 To .InteractiveSequences(k).Count) As Long
                                        End If
                                        bIsAlreadyInserted = False
                                        Dim l As Long
                                        For l = 1 To ParagraphCount
                                            If paragraphs(l) = .InteractiveSequences(k).Item(j).Paragraph Then
                                                bIsAlreadyInserted = True
                                                Exit For
                                            End If
                                        Next
                                        If Not bIsAlreadyInserted Then
                                            ParagraphCount = ParagraphCount + 1
                                            paragraphs(ParagraphCount) = .InteractiveSequences(k).Item(j).Paragraph
                                        End If
                                    End If
                                End If
                            End If
                        End If
                        j = j + 1
                    Loop
                Next
                
                
                If bInsertObject Then
                    m_lGroundLayerCount = m_lGroundLayerCount + 1
                    Set m_GroundLayer(m_lGroundLayerCount).pptShape = m_pptSlide.Shapes(i)
                    If ParagraphCount > 0 Then
                        m_GroundLayer(m_lGroundLayerCount).ParagraphCount = ParagraphCount
                        ReDim m_GroundLayer(m_lGroundLayerCount).animatedParagraph(1 To ParagraphCount) As Long
                        For j = 1 To ParagraphCount
                            m_GroundLayer(m_lGroundLayerCount).animatedParagraph(j) = paragraphs(j)
                        Next
                        Erase paragraphs
                    Else
                        m_GroundLayer(m_lGroundLayerCount).ParagraphCount = 0
                    End If
                End If
            End With
        Next
        
    End If
    
    On Error GoTo 0
        
End Sub


Private Sub PerformObjects_9_0()
    ' if there is a group shape, which is ungrouped and not
    ' regrouped, then the shape list of the slide will be
    ' changed, the Shapes(i) cannot get the right shape any
    ' more. Shps() caches the original shapes in order to
    ' avoid this problem.
    Dim shps() As Shape
    Dim zPos As Long
    Dim i           As Long
    Dim CurObj      As aofShape
    Dim tmp         As Long
    Dim tmpShp      As Shape
    
    ReDim shps(1 To m_lObjectCount) As Shape
    
    For i = 1 To m_lObjectCount
        Set shps(i) = m_pptSlide.Shapes(i)
    Next
    
    For i = 1 To m_lObjectCount
        Set CurObj = New aofShape
        zPos = shps(i).ZOrderPosition
        If shps(i).Type = msoGroup Then
            shps(i).Copy
            Set tmpShp = g_LecturnityDocument.m_tempSlide.Shapes.Paste().Item(1)
            CurObj.Perform g_LecturnityDocument.m_tempSlide, tmpShp
            tmpShp.Delete
        Else
            CurObj.Perform m_pptSlide, shps(i)
        End If
            
        CurObj.SetZPosition zPos
        
        If m_AnimationList(i).AnimOrder = 0 Then       ' no animation
            tmp = tmp + 1
            Set m_GroundLayer(tmp).lecturnityShape = CurObj       ' add to GroundLayer
        Else    ' with aniamtion
            Set m_UpperLayers(m_AnimationList(i).UpperLevelIndex) = New aofLayer
            If m_AnimationList(i).AfterEffect = ppAfterEffectHide Or _
               m_AnimationList(i).AfterEffect = ppAfterEffectHideOnClick Then
                m_AnimationList(i).AnimationType = 2
            Else
                m_AnimationList(i).AnimationType = 1
            End If
            m_AnimationList(i).AnimationId = i
            m_AnimationList(i).TriggerId = -1
            m_AnimationList(i).CallObjectId = -1
            m_UpperLayers(m_AnimationList(i).UpperLevelIndex).Perform_9_0 CurObj, m_AnimationList(i)  ' add to UpperLayer
        End If
        
    Next
    
    Erase shps
    ' release the reference
    Set m_pptSlide = Nothing
    
End Sub

Private Sub PerformObjects_10_0()
    ' if there is a group shape, which is ungrouped and not
    ' regrouped, then the shape list of the slide will be
    ' changed, the Shapes(i) cannot get the right shape any
    ' more. Shps() caches the original shapes in order to
    ' avoid this problem.
    Dim zPos As Long
    Dim CurObj      As aofShape
    Dim tmp         As Long
    Dim tmpShp      As Shape
    Dim animSetting As aofAnimSetting
    
    Dim i As Long
    Dim j As Long
    For i = 1 To m_lGroundLayerCount
        Set CurObj = New aofShape
        zPos = m_GroundLayer(i).pptShape.ZOrderPosition
        CurObj.ObjectLinkIds = GetInteractionId(m_GroundLayer(i).pptShape)
        CurObj.ObjectId = m_GroundLayer(i).pptShape.Id
    
        Dim animatedParagraphs As String: animatedParagraphs = ""
        If m_GroundLayer(i).ParagraphCount > 0 Then
            animatedParagraphs = "-not:"
            For j = 1 To m_GroundLayer(i).ParagraphCount
               animatedParagraphs = animatedParagraphs & m_GroundLayer(i).animatedParagraph(j) & ":"
            Next
        End If
        CurObj.Perform m_pptSlide, m_GroundLayer(i).pptShape, animatedParagraphs
      
        CurObj.SetZPosition zPos
    
        If CurObj.FootersHeadersType <> 0 Then
            For j = 1 To m_lecTemplate.GetObjectCount
                If m_lecTemplate.GetObjs(j).FootersHeadersType = CurObj.FootersHeadersType Then
                    m_lecTemplate.GetObjs(j).m_bVisible = False
                End If
            Next
        End If
        
        tmp = tmp + 1
        Set m_GroundLayer(tmp).lecturnityShape = CurObj       ' add to GroundLayer
    Next
    

    ReDim m_UpperLayers(1 To GetAnimationCount()) As aofLayer
    m_lUpperLayerCount = 0
        
    SearchClickIdsForInteraction
    SearchClickIdsForMainSequence
    
    
    ' Search for click Ids
    
    
    
    ' release the reference
    Set m_pptSlide = Nothing
    
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
            Case 9
                babyS = " "       ' Replace tab with space
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
    Dim ascV    As Long
    Dim ret     As String
    Dim baby    As String
    
    ret = ""
    For i = 1 To Len(str)
        baby = Mid(str, i, 1)
        ascV = AscW(baby)
        Select Case ascV
            Case 8220, 8221, 34     ' normale und typographische Anführungszeichen
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


Private Function HasTextInShape(shp As Shape) As Boolean
    On Error Resume Next
    With shp
        If .HasTextFrame = msoFalse Then
            HasTextInShape = False
        ElseIf .TextFrame.hasText = msoFalse Then
            HasTextInShape = False
        ElseIf .TextFrame.TextRange.text = "" Then
            HasTextInShape = False
        Else
            HasTextInShape = True
        End If
    End With

End Function

Private Function GetParagraphIndex(ByVal actualSequence_ As Variant, index As Long) As Long
    On Error GoTo NoParagraph
    Dim actualSequence As Sequence
    Set actualSequence = actualSequence_
    With actualSequence.Item(index)
        GetParagraphIndex = .Paragraph
    End With
    
    Exit Function
    
NoParagraph:
    GetParagraphIndex = 0
End Function

Private Function GetAnimationCount() As Long

    Dim lAnimationCount As Long: lAnimationCount = 0
    Dim i As Long
    
    With m_pptSlide.TimeLine
        For i = 1 To .InteractiveSequences.Count
            lAnimationCount = lAnimationCount + .InteractiveSequences(i).Count
        Next
        lAnimationCount = lAnimationCount + .MainSequence.Count
    End With
    
    GetAnimationCount = lAnimationCount

End Function

Private Function GetFirstClickNumber(currentSequence_ As Variant) As Long
    On Error GoTo ClickZero
        
    Dim currentSequence As Sequence
    Set currentSequence = currentSequence_
    
    Dim firstEffect As effect
    Set firstEffect = currentSequence.FindFirstAnimationForClick(1)
    ' Click 1 is not first object in TimeLine
    If firstEffect.index > 1 Then
        GetFirstClickNumber = 0
    Else
        GetFirstClickNumber = 1
    End If
        
    Exit Function
    
ClickZero:
    GetFirstClickNumber = 0
End Function

Private Function GetInteractionId(pptShape As Shape) As String

    GetInteractionId = ""
    With m_pptSlide.TimeLine
        If .InteractiveSequences.Count <= 0 Then
            Exit Function
        End If
    
        Dim i As Long
        Dim j As Long
        For i = 1 To .InteractiveSequences.Count
            For j = 1 To .InteractiveSequences(i).Count
                If Not (.InteractiveSequences(i).Item(j).Shape Is Nothing) Then
                    If .InteractiveSequences(i).Item(j).Timing.TriggerType = msoAnimTriggerOnShapeClick And _
                    .InteractiveSequences(i).Item(j).Timing.TriggerShape.Id = pptShape.Id Then
                        GetInteractionId = GetInteractionId & .InteractiveSequences(i).Item(j).Shape.Id & " "
                    End If
                End If
            Next
        Next
    End With
End Function

Private Sub SearchClickIdsForInteraction()

    Dim CurObj      As aofShape
    Dim tmpShp      As Shape
    Dim pptShape    As Shape
    Dim pptEffect   As effect
    
    Dim i As Long
    Dim j As Long
    Dim k As Long
    With m_pptSlide.TimeLine
        If .InteractiveSequences.Count > 0 Then
            For i = 1 To .InteractiveSequences.Count
                If .InteractiveSequences(i).Count > 0 Then
    
                    For j = 1 To .InteractiveSequences(i).Count
    
                        If .InteractiveSequences(i).Item(j).Timing.TriggerType = msoAnimTriggerOnShapeClick Then
    
                            CheckIfAnimationEffectIsSupported .InteractiveSequences(i).Item(j), m_pptSlide.slideNumber
                            Set CurObj = New aofShape
                            Set pptShape = .InteractiveSequences(i).Item(j).Shape
                            If pptShape Is Nothing Then
                                For k = 1 To m_pptSlide.Shapes.Count
                                    ' TODO
                                    If m_pptSlide.Shapes(k).AnimationSettings.Animate = msoTrue Then
                                        Set pptEffect = m_pptSlide.TimeLine.InteractiveSequences(i).FindFirstAnimationFor(m_pptSlide.Shapes(k))
                                        If Not (pptEffect Is Nothing) Then
                                            If pptEffect.index = .InteractiveSequences(i).Item(j).index Then
                                                If pptEffect.effectType <> msoAnimEffectMediaPause And _
                                                   pptEffect.effectType <> msoAnimEffectMediaPlay And _
                                                   pptEffect.effectType <> msoAnimEffectMediaStop Then
                                                    Set pptShape = m_pptSlide.Shapes(k)
                                                End If
                                                Exit For
                                            End If
                                        End If
                                    End If
                                Next
                            End If
                            If Not (pptShape Is Nothing) Then
                                Dim zPos As Long: zPos = pptShape.ZOrderPosition
                                CurObj.ObjectLinkIds = GetInteractionId(pptShape)
                                CurObj.ObjectId = pptShape.Id
                                Dim hasText As Boolean: hasText = HasTextInShape(pptShape)
                                If hasText Then
                                    Dim index As Long: index = GetParagraphIndex(m_pptSlide.TimeLine.InteractiveSequences(i), j)
                                    If index > 0 Then
                                        Dim animatedParagraphs As String: animatedParagraphs = ""
                                        animatedParagraphs = "-only:" & index & ":"
                                        CurObj.Perform m_pptSlide, pptShape, animatedParagraphs
                                    Else
                                        CurObj.Perform m_pptSlide, pptShape
                                    End If
                                Else
                                    CurObj.Perform m_pptSlide, pptShape
                                End If
    
                                CurObj.SetZPosition zPos
    
                                m_lUpperLayerCount = m_lUpperLayerCount + 1
                                Set m_UpperLayers(m_lUpperLayerCount) = New aofLayer
                                Dim animSetting As aofAnimSetting
                                If .InteractiveSequences(i).Item(j).Exit = msoTrue Then
                                    animSetting.AnimationType = 0
                                Else
                                    If .InteractiveSequences(i).Item(j).EffectInformation.AfterEffect = msoAnimAfterEffectHide Or _
                                       .InteractiveSequences(i).Item(j).EffectInformation.AfterEffect = msoAnimAfterEffectHideOnNextClick Then
                                        animSetting.AnimationType = 2
                                    Else
                                        animSetting.AnimationType = 1
                                    End If
                                End If
                                animSetting.AnimationId = pptShape.Id
                                animSetting.TriggerId = 1
                                If Not (.InteractiveSequences(i).Item(j).Timing.TriggerShape Is Nothing) Then
                                    animSetting.CallObjectId = .InteractiveSequences(i).Item(j).Timing.TriggerShape.Id
                                Else
                                    animSetting.CallObjectId = -1
                                End If
                                
    
                                m_UpperLayers(m_lUpperLayerCount).Perform_10_0 CurObj, animSetting ' add to UpperLayerIf indexCount > 0 Then
    
                                m_UpperLayers(m_lUpperLayerCount).m_lClickId = 0
                            End If
                        End If
                    Next
                End If
            Next
        End If
    End With

End Sub

Private Sub SearchClickIdsForMainSequence()
    
    Dim CurObj      As aofShape
    Dim tmpShp      As Shape
    Dim pptShape    As Shape
    Dim pptEffect   As effect
    
    Dim i As Long
    Dim k As Long
    
    If m_pptSlide.TimeLine.MainSequence.Count > 0 Then
        
        Dim bFinished As Boolean: bFinished = False
        Dim indexCount As Long: indexCount = 0
        Dim indexList() As Long
        ReDim indexList(1 To m_pptSlide.TimeLine.MainSequence.Count) As Long
        
        ' Try if the first click is 0 or 1
        ' Loop finished if the last click is reached
        
        ' First click can be 0 or 1
        Dim click As Long: click = GetFirstClickNumber(m_pptSlide.TimeLine.MainSequence)
           
        On Error GoTo AnimationClickFound
        i = 1
        
        Dim firstEffect As effect
        Dim effectIndex As Long
        Do While 1
            Set firstEffect = m_pptSlide.TimeLine.MainSequence.FindFirstAnimationForClick(click)
            If Not (firstEffect Is Nothing) Then
                effectIndex = firstEffect.index
                indexList(i) = effectIndex
                indexCount = i
                i = i + 1
                click = click + 1
            Else
                Exit Do
            End If
        Loop
    End If
    
AnimationClickFound:
    
    Dim nextIndex As Long
    If m_pptSlide.TimeLine.MainSequence.Count > 0 Then
        If indexCount > 0 Then
            nextIndex = indexList(1)
        Else
            nextIndex = 0
        End If
            
        
        Dim hasText As Boolean
        For i = 1 To m_pptSlide.TimeLine.MainSequence.Count
            With m_pptSlide.TimeLine.MainSequence
            
                CheckIfAnimationEffectIsSupported .Item(i), m_pptSlide.slideNumber
                
                If IsAnimated(.Item(i)) Then
                    Set CurObj = New aofShape
                    Set pptShape = .Item(i).Shape
                    If pptShape Is Nothing Then
                        For k = 1 To m_pptSlide.Shapes.Count
                            If m_pptSlide.Shapes(k).AnimationSettings.Animate = msoTrue Then
                                Set pptEffect = m_pptSlide.TimeLine.MainSequence.FindFirstAnimationFor(m_pptSlide.Shapes(k))
                                If Not (pptEffect Is Nothing) Then
                                    If pptEffect.index = .Item(i).index Then
                                        If pptEffect.effectType <> msoAnimEffectMediaPause And _
                                           pptEffect.effectType <> msoAnimEffectMediaPlay And _
                                           pptEffect.effectType <> msoAnimEffectMediaStop Then
                                            Set pptShape = m_pptSlide.Shapes(k)
                                        End If
                                        Exit For
                                    End If
                                End If
                            End If
                        Next
                    End If
                    If Not (pptShape Is Nothing) Then
                        Dim zPos As Long: zPos = pptShape.ZOrderPosition
                        CurObj.ObjectLinkIds = GetInteractionId(pptShape)
                        CurObj.ObjectId = pptShape.Id
                        hasText = HasTextInShape(pptShape)
                        If hasText Then
                            Dim index As Long: index = GetParagraphIndex(m_pptSlide.TimeLine.MainSequence, i)
                            If index > 0 Then
                                Dim animatedParagraphs As String: animatedParagraphs = ""
                                animatedParagraphs = "-only:" & index & ":"
                                CurObj.Perform m_pptSlide, pptShape, animatedParagraphs
                            Else
                                CurObj.Perform m_pptSlide, pptShape
                            End If
                        Else
                            CurObj.Perform m_pptSlide, pptShape
                        End If
                        
                        CurObj.SetZPosition zPos
                        
                        m_lUpperLayerCount = m_lUpperLayerCount + 1
                        Set m_UpperLayers(m_lUpperLayerCount) = New aofLayer
                        Dim animSetting As aofAnimSetting
                        If .Item(i).Exit = msoTrue Then
                            animSetting.AnimationType = 0
                        Else
                            If .Item(i).EffectInformation.AfterEffect = msoAnimAfterEffectHide Or _
                               .Item(i).EffectInformation.AfterEffect = msoAnimAfterEffectHideOnNextClick Then
                                animSetting.AnimationType = 2
                            Else
                                animSetting.AnimationType = 1
                            End If
                        End If
                        animSetting.AnimationId = pptShape.Id
                        animSetting.TriggerId = -1
                        animSetting.CallObjectId = -1
                        m_UpperLayers(m_lUpperLayerCount).Perform_10_0 CurObj, animSetting ' add to UpperLayer
                    
                        If indexCount > 0 Then
                            If .Item(i).index < indexList(nextIndex) Then
                                m_UpperLayers(m_lUpperLayerCount).m_lClickId = nextIndex - 1
                            ElseIf .Item(i).index >= indexList(nextIndex) Then
                                m_UpperLayers(m_lUpperLayerCount).m_lClickId = nextIndex
                                nextIndex = nextIndex + 1
                                If (nextIndex > indexCount) Then
                                    nextIndex = indexCount
                                End If
                            'Else
                            '    Do While (.Item(i).index > indexList(nextIndex)) And _
                            '             (nextIndex < indexCount)
                            '        nextIndex = nextIndex + 1
                            '    Loop
                            '    m_UpperLayers(m_lUpperLayerCount).m_lClickId = nextIndex
                            '    nextIndex = nextIndex + 1
                            '    If (nextIndex > indexCount) Then
                            '        nextIndex = indexCount
                            '    End If
                            End If
                        Else
                            m_UpperLayers(m_lUpperLayerCount).m_lClickId = -1
                        End If
                    End If
                End If
            End With
        Next
    End If
End Sub

Private Sub CheckIfAnimationEffectIsSupported(animationEffect_ As Variant, iSlideNumber As Long)
    Dim bAnimationIsSupported As Boolean: bAnimationIsSupported = True
    
    Dim iActualType As Integer
    Dim animationEffect As effect
    Set animationEffect = animationEffect_
    If animationEffect.Shape Is Nothing Then
        iActualType = NOT_SUPPORTED_ANIMATIONEFFECT_OTHER
    Else
        If animationEffect.effectType <> msoAnimEffectAppear Or _
           animationEffect.EffectInformation.SoundEffect.Type <> ppSoundNone Or _
           animationEffect.Timing.TriggerDelayTime <> 0 Then
            bAnimationIsSupported = False
        End If
        
        If Not bAnimationIsSupported Then
            If IsAnimated(animationEffect) Then
                If animationEffect.Exit = msoTrue Then
                    iActualType = NOT_SUPPORTED_ANIMATIONEFFECT_HIDE
                Else
                    iActualType = NOT_SUPPORTED_ANIMATIONEFFECT_SHOW
                End If
            ElseIf IsHighlightning(animationEffect) Or IsPathEffect(animationEffect) Then
                iActualType = NOT_SUPPORTED_ANIMATIONEFFECT_OTHER
            Else
                iActualType = NOT_SUPPORTED_ANIMATIONEFFECT_OTHER
            End If
        End If
    End If
        
    If iActualType > 0 Then
        SetObjectWarningMessage iActualType, iSlideNumber
    End If
    
End Sub

Private Sub SortAnimIndex()
    Dim i As Long
    Dim ord As Long
    Dim idx As Long
    
    idx = 0
    ord = 1
    i = 1
    Do While idx < m_lUpperLayerCount
        If m_AnimationList(i).AnimOrder = ord Then
            idx = idx + 1
            m_AnimationList(i).UpperLevelIndex = idx
        End If
        i = i + 1
        If i > m_lAnimationCount Then
            ord = ord + 1
            i = 1
        End If
    Loop
End Sub

Private Function IsBlendEffect(animationEffectType As Variant) As Boolean

    Dim effectType As MsoAnimationType: effectType = animationEffectType
    
    Select Case effectType
        Case msoAnimEffectAppear, _
             msoAnimEffectDissolve, _
             msoAnimEffectFly, _
             msoAnimEffectDiamond, _
             msoAnimEffectWedge, _
             msoAnimEffectPlus, _
             msoAnimEffectCheckerboard, _
             msoAnimEffectBox, _
             msoAnimEffectCircle, _
             msoAnimEffectWipe
            IsBlendEffect = True
        Case msoAnimEffectFadedSwivel, _
             msoAnimEffectFadedZoom, _
             msoAnimEffectPeek, _
             msoAnimEffectSplit, _
             msoAnimEffectFlashOnce, _
             msoAnimEffectWheel, _
             msoAnimEffectCrawl, _
             msoAnimEffectBlinds, _
             msoAnimEffectRandomBars, _
             msoAnimEffectRandomEffects
            IsBlendEffect = True
        Case msoAnimEffectStrips, _
             msoAnimEffectExpand, _
             msoAnimEffectFade, _
             msoAnimEffectThinLine, _
             msoAnimEffectAscend, _
             msoAnimEffectDescend, _
             msoAnimEffectRiseUp, _
             msoAnimEffectStretch, _
             msoAnimEffectStretchy, _
             msoAnimEffectCenterRevolve
             IsBlendEffect = True
        Case msoAnimEffectGrowAndTurn, _
             msoAnimEffectSpinner, _
             msoAnimEffectEaseIn, _
             msoAnimEffectCredits, _
             msoAnimEffectSwivel, _
             msoAnimEffectFold, _
             msoAnimEffectArcUp, _
             msoAnimEffectLightSpeed, _
             msoAnimEffectSpiral, _
             msoAnimEffectPinwheel
            IsBlendEffect = True
        Case msoAnimEffectBoomerang, _
             msoAnimEffectGlide, _
             msoAnimEffectZip, _
             msoAnimEffectSling, _
             msoAnimEffectBounce, _
             msoAnimEffectFloat, _
             msoAnimEffectSwish, _
             msoAnimEffectWhip, _
             msoAnimEffectUnfold, _
             msoAnimEffectColorReveal, _
             msoAnimEffectFlip, _
             msoAnimEffectZoom
            IsBlendEffect = True
'        Case msoAnimEffectShimmer, _
'             msoAnimEffectTeeter
'            IsBlendEffect = True
        Case Else
            IsBlendEffect = False
    End Select
End Function

Private Function IsPathEffect(animationEffect_ As Variant) As Boolean

    Dim animationEffect As effect
    Set animationEffect = animationEffect_
    
    Select Case animationEffect.effectType
        Case msoAnimEffectPath4PointStar, _
             msoAnimEffectPath5PointStar, _
             msoAnimEffectPath6PointStar, _
             msoAnimEffectPath8PointStar, _
             msoAnimEffectPathArcDown, _
             msoAnimEffectPathArcLeft, _
             msoAnimEffectPathArcRight, _
             msoAnimEffectPathArcUp, _
             msoAnimEffectPathBean, _
             msoAnimEffectPathBounceLeft
            IsPathEffect = True
        Case msoAnimEffectPathBounceRight, _
             msoAnimEffectPathBuzzsaw, _
             msoAnimEffectPathCircle, _
             msoAnimEffectPathCrescentMoon, _
             msoAnimEffectPathCurvedSquare, _
             msoAnimEffectPathCurvedX, _
             msoAnimEffectPathCurvyLeft, _
             msoAnimEffectPathCurvyRight, _
             msoAnimEffectPathCurvyStar, _
             msoAnimEffectPathDecayingWave
            IsPathEffect = True
        Case msoAnimEffectPathDiagonalDownRight, _
             msoAnimEffectPathDiagonalUpRight, _
             msoAnimEffectPathDiamond, _
             msoAnimEffectPathDown, _
             msoAnimEffectPathEqualTriangle, _
             msoAnimEffectPathFigure8Four, _
             msoAnimEffectPathFootball, _
             msoAnimEffectPathFunnel, _
             msoAnimEffectPathHeart, _
             msoAnimEffectPathHeartbeat
            IsPathEffect = True
        Case msoAnimEffectPathHexagon, _
             msoAnimEffectPathHorizontalFigure8, _
             msoAnimEffectPathInvertedSquare, _
             msoAnimEffectPathInvertedTriangle, _
             msoAnimEffectPathLeft, _
             msoAnimEffectPathLoopdeLoop, _
             msoAnimEffectPathNeutron, _
             msoAnimEffectPathOctagon, _
             msoAnimEffectPathParallelogram, _
             msoAnimEffectPathPeanut
            IsPathEffect = True
        Case msoAnimEffectPathPentagon, _
             msoAnimEffectPathPlus, _
             msoAnimEffectPathPointyStar, _
             msoAnimEffectPathRightTriangle, _
             msoAnimEffectPathSCurve1, _
             msoAnimEffectPathSCurve2, _
             msoAnimEffectPathSineWave, _
             msoAnimEffectPathSpiralLeft, _
             msoAnimEffectPathSpiralRight, _
             msoAnimEffectPathSpring
            IsPathEffect = True
        Case msoAnimEffectPathSquare, _
             msoAnimEffectPathStairsDown, _
             msoAnimEffectPathSwoosh, _
             msoAnimEffectPathTeardrop, _
             msoAnimEffectPathTrapezoid, _
             msoAnimEffectPathTurnDown, _
             msoAnimEffectPathTurnRight, _
             msoAnimEffectPathTurnUp, _
             msoAnimEffectPathTurnUpRight, _
             msoAnimEffectPathVerticalFigure8
             IsPathEffect = True
        Case msoAnimEffectPathWave, _
             msoAnimEffectPathZigzag
            IsPathEffect = True
        Case Else
            IsPathEffect = False
    End Select
End Function

Private Function IsHighlightning(animationEffect_ As Variant) As Boolean
    Dim animationEffect As effect
    Set animationEffect = animationEffect_
    
    Select Case animationEffect.effectType
        Case msoAnimEffectChangeFillColor, _
             msoAnimEffectChangeFont, _
             msoAnimEffectChangeFontColor, _
             msoAnimEffectChangeFontSize, _
             msoAnimEffectChangeFontStyle, _
             msoAnimEffectChangeLineColor, _
             msoAnimEffectBrushOnUnderline, _
             msoAnimEffectBrushOnColor, _
             msoAnimEffectColorWave, _
             msoAnimEffectTransparency
            IsHighlightning = True
        Case msoAnimEffectGrowShrink, _
             msoAnimEffectComplementaryColor, _
             msoAnimEffectComplementaryColor2, _
             msoAnimEffectSpin, _
             msoAnimEffectBoldFlash, _
             msoAnimEffectDarken, _
             msoAnimEffectFlashBulb, _
             msoAnimEffectColorBlend, _
             msoAnimEffectContrastingColor, _
             msoAnimEffectDesaturate
            IsHighlightning = True
        Case msoAnimEffectLighten, _
             msoAnimEffectGrowWithColor, _
             msoAnimEffectVerticalGrow, _
             msoAnimEffectWave, _
             msoAnimEffectBlast, _
             msoAnimEffectFlicker, _
             msoAnimEffectStyleEmphasis, _
             msoAnimEffectBoldReveal
            IsHighlightning = True
        Case Else
            IsHighlightning = False
    End Select
  
End Function

' not dedicated types
'msoAnimEffectArcUp
'msoAnimEffectAscend
'msoAnimEffectBlast
'msoAnimEffectBoldReveal
'msoAnimEffectBoomerang
'msoAnimEffectBounce
'msoAnimEffectCenterRevolve
'msoAnimEffectColorReveal
'msoAnimEffectCredits
'msoAnimEffectCustom
'msoAnimEffectDescend
'msoAnimEffectEaseIn
'msoAnimEffectFadedAscend
'msoAnimEffectFlicker
'msoAnimEffectFlip
'msoAnimEffectFloat
'msoAnimEffectFold
'msoAnimEffectGlide
'msoAnimEffectGrowAndTurn
'msoAnimEffectGrowWithColor
'msoAnimEffectMediaPause
'msoAnimEffectMediaPlay
'msoAnimEffectMediaStop
'msoAnimEffectPinwheel
'msoAnimEffectRiseUp
'msoAnimEffectShimmer
'msoAnimEffectSling
'msoAnimEffectSpinner
'msoAnimEffectSpiral
'msoAnimEffectStretch
'msoAnimEffectStretchy
'msoAnimEffectStyleEmphasis
'msoAnimEffectSwish
'msoAnimEffectSwivel
'msoAnimEffectTeeter
'msoAnimEffectThinLine
'msoAnimEffectUnfold
'msoAnimEffectWave
'msoAnimEffectWhip
'msoAnimEffectZip
'msoAnimEffectZoom
'msoAnimEffectLightSpeed

Private Function IsAnimated(animationEffect As Variant) As Boolean

    Dim effect As effect
    Set effect = animationEffect
    
    If effect.Shape Is Nothing Then
        IsAnimated = True
        Exit Function
    End If
    
    If IsBlendEffect(effect.effectType) Then
        IsAnimated = True
    Else
        IsAnimated = False
    End If
        
End Function


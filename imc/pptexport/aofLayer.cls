VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "aofLayer"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Public m_lClickId               As Long


Private m_lAfterEffect          As Long

' the layer in ppt file contains only one object, but sometimes a text shape
' may have sub-animations, so it should be divided into several layers, therefore
' the result may contain several "overlay"s.
Private m_SubAnimation          As aofAnimSetting   ' if there are sub-animations in the layer

Private m_lObjectCount          As Long             ' number of objects, (=1)
Private m_lecShape              As aofShape         ' the object in this layer

 
''''''''''''''''''''''''''
' Constructor/Destructor '
''''''''''''''''''''''''''

Private Sub Class_Initialize()

    m_lObjectCount = 0
    
End Sub

Private Sub Class_Terminate()

    Set m_lecShape = Nothing
    
End Sub


'''''''''''''''''''''''''''''''''''
' Public functions and procedures '
'''''''''''''''''''''''''''''''''''

Public Function Script() As String
    Dim i       As Long
    Dim strTmp  As String
    Dim strBackg    As String
    
    Dim lastPara    As Long
    Dim lastIndentLevel As Long
    
    IncIndent
    
    ' set the text animation
    With m_lecShape
        If .GetShapeType() = LEC_SHAPETYPE_TEXTGROUP And .m_bHasSubAnim = True Then
            ' now convert a text group with sub-animations.
            ' the oder of the overlays depend on the setting of "SubAnim.AnimInReverse"
            
            If .m_lecDrawStyle <> LEC_DRAWSTYLE_NONE Then    ' has background
                .SetShapeType (LEC_SHAPETYPE_RECTANGLE)
                strBackg = WrapAsLayer(.Script())       ' overlay of background
                .SetShapeType (LEC_SHAPETYPE_TEXTGROUP)
            End If
            
            strTmp = strTmp & .GetGroupItem(1).Script()
            lastPara = .GetGroupItem(1).ParaIndex
            lastIndentLevel = .GetGroupItem(1).TextIndentLevel
            
            For i = 2 To .GroupItemNum
                With .GetGroupItem(i)
                    If .ParaIndex = lastPara Then       ' still in the same Paragraph
                        strTmp = strTmp & .Script()
                    Else
                        lastPara = .ParaIndex
                        
                        If .TextIndentLevel > m_SubAnimation.AnimBeforeLevel Then
                                strTmp = strTmp & .Script()     ' in the same layer
                        Else        ' added as a new overlay
                            If m_SubAnimation.AnimInReverse Then
                                Script = WrapAsLayer(strTmp) & Script
                            Else
                                Script = Script & WrapAsLayer(strTmp)
                            End If
                            strTmp = ""
                        End If
                        strTmp = strTmp & .Script()
                        lastIndentLevel = .TextIndentLevel
                    End If
                End With
                
            Next
            
            ' add the background overlay
            If m_SubAnimation.AnimInReverse Then
                Script = strBackg & WrapAsLayer(strTmp) & Script
            Else
                Script = strBackg & Script & WrapAsLayer(strTmp)
            End If
        Else    ' no sub-animations. convert to one overlay
            Script = WrapAsLayer(.Script())
        End If
    End With
    
    DecIndent
    
End Function

' make a new layer
'
' args      aofShp  : the object in current layer
'           anim    : the animation setting of the shape in current layer
Public Sub Perform_9_0(aofShp As aofShape, anim As aofAnimSetting)

    m_lObjectCount = 1
    
    m_SubAnimation = anim
    
    Set m_lecShape = aofShp
    
    m_lAfterEffect = m_SubAnimation.AfterEffect

End Sub

Public Sub Perform_10_0(aofShp As aofShape, anim As aofAnimSetting)

   m_lObjectCount = 1
    
    Set m_lecShape = aofShp
    m_lecShape.m_bHasSubAnim = False
    
    m_SubAnimation = anim
End Sub


''''''''''''''''''''''''''''''''''''
' Private functions and procedures '
''''''''''''''''''''''''''''''''''''

Private Function AddInteraction() As String
    Dim tmpString As String
    
    tmpString = ""
    
    If m_SubAnimation.TriggerId <> -1 Then
        tmpString = tmpString & " trigger=""" & 1 & """"
    End If
    
    If m_SubAnimation.CallObjectId <> -1 Then
        tmpString = tmpString & " callId=""" & m_SubAnimation.CallObjectId & """"
    End If
    
    AddInteraction = tmpString
    
End Function

Private Function WrapAsLayer(str As String) As String
    If m_SubAnimation.AnimationType = 2 Then
        Dim tmpStr1 As String
        tmpStr1 = NewLine() & "<ANIMATED id=""" & m_SubAnimation.AnimationId & """ type=""" & 1 & """ clickId=""" & m_lClickId & """" & AddInteraction & ">" & str & NewLine() & "</ANIMATED>"
        Dim tmpStr2 As String
        Dim zwClickId As Long: zwClickId = m_lClickId
        If m_lClickId <> 0 Then
            zwClickId = m_lClickId + 1
        End If
        tmpStr2 = NewLine() & "<ANIMATED id=""" & m_SubAnimation.AnimationId & """ type=""" & 0 & """ clickId=""" & zwClickId & """" & AddInteraction & ">" & str & NewLine() & "</ANIMATED>"
        WrapAsLayer = tmpStr1 & tmpStr2
    Else
        WrapAsLayer = NewLine() & "<ANIMATED id=""" & m_SubAnimation.AnimationId & """ type=""" & m_SubAnimation.AnimationType & """ clickId=""" & m_lClickId & """" & AddInteraction & ">" & str & NewLine() & "</ANIMATED>"
    End If
End Function


VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "aofLayer"
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False

Option Explicit

'Public IsUserDefinedLayer       As Boolean      ' useless at the moment.

Private Resistant           As Long             ' resistant property of aof overlay, useless in lsd file
Private BackColor           As String           ' background color
Private Name                As String           ' name

' the layer in ppt file contains only one object, but sometimes a text shape
' may have sub-animations, so it should be divided into several layers, therefore
' the result may contain several "overlay"s.
Private SubAnim             As aofAnimSetting   ' if there are sub-animations in the layer
Private ShapeObj            As aofShape         ' the object in this layer

Private ObjNum              As Long             ' number of objects, (=1)

Public Function Script() As String
    Dim i       As Long
    Dim strTmp  As String
    Dim strBackg    As String
    
    Dim lastPara    As Long
    Dim lastIndentLevel As Long
    
    IncIndent
    
    ' set the text animation
    With ShapeObj
        If .ShapeType = aofShapeTypeTextGroup And .HasSubAnim = True Then
            ' now convert a text group with sub-animations.
            ' the oder of the overlays depend on the setting of "SubAnim.AnimInReverse"
            
            If .GraphStyle <> aofGraphStyleNone Then    ' has background
                .ShapeType = aofShapeTypeRect
                strBackg = WrapAsLayer(.Script())       ' overlay of background
                .ShapeType = aofShapeTypeTextGroup
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
                        
                        If .TextIndentLevel > SubAnim.AnimBeforeLevel Then
                                strTmp = strTmp & .Script()     ' in the same layer
                        Else        ' added as a new overlay
                            If SubAnim.AnimInReverse Then
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
            If SubAnim.AnimInReverse Then
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
'  Type aofAnimSetting
'      AnimOrder         As Long         ' recorde the animation order of one shape, no anim = 0
'      HasSubAnim        As Boolean         ' the animation within a shape (Text shape)
'      Resistant         As Long
'      AnimInReverse     As Boolean
'      AnimBeforeLevel   As Long
'  End Type
'Public Sub Perform(aofShp As aofShape, anim As aofAnimSetting)
Public Sub Perform(aofShp As aofShape, ao As Long, hsa As Boolean, _
                    res As Long, air As Boolean, abl As Long)
    ObjNum = 1
    'SubAnim = anim
    With SubAnim
        .AnimBeforeLevel = abl
        .AnimInReverse = air
        .AnimOrder = ao
        .HasSubAnim = hsa
        .Resistant = res
    End With
    
    Set ShapeObj = aofShp
    
    Resistant = SubAnim.Resistant
End Sub

Private Function WrapAsLayer(str As String) As String
    WrapAsLayer = NewLine() & "<ANIMATED>" & str & NewLine() & "</ANIMATED>"
End Function

Private Sub Class_Terminate()
    Set ShapeObj = Nothing
End Sub

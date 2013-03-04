VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "aofPoints"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Private Const nStep As Long = 10
Private nSize  As Long
Private nCount As Long
Private points() As Single  'Long

Property Get Count() As Long
    Count = nCount
End Property

'Public Sub Add(x As Long, y As Long)
Public Sub Add(x As Single, y As Single)
    If nCount >= nSize Then
        nSize = nSize + nStep
        ReDim Preserve points(1 To 2, 1 To nSize) As Single 'Long
    End If
    
    If nCount > 0 Then
        If points(1, nCount) <> x Or points(2, nCount) <> y Then  ' no duplicated point
            nCount = nCount + 1
            points(1, nCount) = x
            points(2, nCount) = y
        End If
    Else    ' the first point
            nCount = nCount + 1
            points(1, nCount) = x
            points(2, nCount) = y
    End If
End Sub

Public Function IsClosed() As Boolean
    On Error Resume Next
    If points(1, 1) = points(1, nCount) And points(2, 1) = points(2, nCount) Then
        IsClosed = True
    Else
        IsClosed = False
    End If
End Function

Public Function PrintString(Optional nBegin As Long = -1, Optional nEnd As Long = -1, Optional strh As String = "{", Optional strm As String = " ", Optional strt As String = "} ") As String
    Dim i As Long
    Dim str As String
    Dim nb  As Long
    Dim ne  As Long
    
    str = ""
    
    If nCount = 1 Then
        If nBegin < 0 And nEnd < 0 Then
            str = strh & CLng(points(1, 1)) & strm & CLng(points(2, 1)) & strt
            ' write the point twice to avoid a polyline with only one point.
            str = str & str
        End If
        PrintString = str
        Exit Function
    End If
    
    ' if nBegin and nEnd are -1, then print all points.
    nb = IIf(nBegin < 0, 1, nBegin)
    ne = IIf(nEnd < 0, nCount, nEnd)
    
    For i = nb To ne
        str = str & strh & CLng(points(1, i)) & strm & CLng(points(2, i)) & strt
    Next
    PrintString = str
End Function

Public Sub SetV(v As Variant)
    Dim i As Long
    For i = 1 To nCount
        v(i, 1) = points(1, i)
        v(i, 2) = points(2, i)
    Next

End Sub

Public Sub GetPoint(idx As Long, x As Single, y As Single)
    If idx > 0 And idx <= nCount Then
        x = points(1, idx)
        y = points(2, idx)
    Else
        x = 0
        y = 0
    End If
End Sub

Public Sub GetDimension(l As Single, t As Single, w As Single, h As Single)
    Dim i As Long
    If nCount > 0 Then
        l = points(1, 1)    ' left
        t = points(2, 1)    ' top
        w = points(1, 1)    ' right
        h = points(2, 1)    ' bottom
    Else
        l = 0: t = 0: w = 0: h = 0 ' no points
        Exit Sub
    End If
    
    For i = 2 To nCount
        If points(1, i) < l Then
            l = points(1, i)
        ElseIf points(1, i) > w Then
            w = points(1, i)
        End If
        If points(2, i) < t Then
            t = points(2, i)
        ElseIf points(2, i) > h Then
            h = points(2, i)
        End If
    Next i
    
    w = w - l       ' width
    h = h - t       ' height
        
End Sub

Public Sub Clear()
    Class_Initialize
End Sub

Private Sub Class_Initialize()
    nSize = nStep
    ReDim points(1 To 2, 1 To nSize) As Single  'Long
    nCount = 0
End Sub

Private Sub Class_Terminate()
    nCount = 0
    Erase points
End Sub

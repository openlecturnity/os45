VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "aofTextItem"
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False

Option Explicit

Public X           As Long          ' left
Public Y           As Long          ' top
Public OffX        As Long          ' x offset between the text and the text box
Public OffY        As Long          ' y offset "

Public FontName    As String
Public FontColor   As String
Public FontSize    As Long

Public Text        As String

Public Family       As String
Public Weight       As String
Public Slant        As String
Public Position     As String       ' "Position" property of mlb font. NOT the position in aofwb.
Public Length       As Long

Public Width        As Long         ' test text width in lsd file   Nov. 12. 2002

Function Script() As String
    Script = NewLine() & """ & text & """
End Function

Private Function MLBFontSize(fnt As Font) As String
    MLBFontSize = fnt.Size
End Function

Private Function MLBWeight(fnt As Font) As String
    If fnt.Bold = msoTrue Then
        MLBWeight = "bold"
    Else
        MLBWeight = "normal"
    End If
End Function

Private Function MLBSlant(fnt As Font) As String
    If fnt.Italic = msoTrue Then
        MLBSlant = "italic"
    Else
        MLBSlant = "roman"
    End If
End Function

Private Function MLBFamily(fnt As Font) As String
    ' New font name supported by Assistant v1.5.0
    MLBFamily = fnt.Name
End Function

Private Function MLBAsciiFamily(fnt As Font) As String
    ' New font name supported by Assistant v1.5.0
    MLBAsciiFamily = fnt.NameAscii
End Function

Public Sub Perform(ByVal txtSeg As TextRange, Optional off_x As Long = 0, Optional off_y As Long = 0)
    On Error Resume Next
    With txtSeg
        X = CLng(.BoundLeft)
        Text = StandardizeText(.Text)
        FontColor = MLBColor(.Font.Color)
        FontSize = MLBFontSize(.Font)
        Family = MLBFamily(.Font)
        Slant = MLBSlant(.Font)
        Weight = MLBWeight(.Font)
        Length = Len(Text)
        Position = TextPosition(.Font)  '"normal"
        ' test text width in lsd file
        'Width = CLng(.BoundWidth)
        ' sometime the BoundWidth doesn't give the right value
        ' so, we should figure out the real width.
        'Width = CLng(.Characters(.Length, 1).BoundLeft + .Characters(.Length, 1).BoundWidth - .Characters(1, 1).BoundLeft)
'        If width1 <> Width Then
'            MsgBox .Text & vbLf & "BoundWidth: " & width1 & vbLf & "Width: " & Width
'        End If
    End With
    ' get the real width of the text (without invisible chars at the end)
    Width = CLng(GetTextWidth(txtSeg))
    
End Sub

Private Function GetTextWidth(tr As TextRange) As Single
    Dim i As Long
    Dim idx As Long
    
    idx = tr.Length
    For i = tr.Length To 1 Step -1
        Select Case AscW(tr.Characters(i, 1).Text)
            Case 1 To 31        ' because the invisible chars were deleted in StandizeText()
                idx = i - 1
            Case Else
                Exit For
        End Select
    Next
            
    GetTextWidth = (tr.Characters(idx, 1).BoundLeft + tr.Characters(idx, 1).BoundWidth - tr.BoundLeft)
                
End Function

Private Function TextPosition(fnt As Font) As String
    On Error Resume Next
    With fnt
        If .Subscript = msoTrue Then
            'TextPosition = "down"
            OffY = -.Size * .BaselineOffset
            FontSize = CLng(.Size * 0.8)
        ElseIf .Superscript = msoTrue Then
            'TextPosition = "up"
            OffY = -.Size * .BaselineOffset
            FontSize = CLng(.Size * 0.8)
        Else
            TextPosition = "normal"
        End If
    End With
    
    ' text position ignored in Assistant 1.4.0.b8w
    TextPosition = "normal"

End Function
' convert current textrange as bullet.
Public Sub PerformAsBullet(ByVal tr As TextRange, fntSize As Long, xpos As Single)
    
    On Error Resume Next
    FontSize = fntSize
    X = CLng(xpos)
    
    With tr.ParagraphFormat.Bullet
        If .UseTextFont = msoTrue Then
            Family = MLBAsciiFamily(.Font)
        Else
        Family = MLBFamily(.Font)
        End If
        Slant = MLBSlant(.Font)
        Weight = MLBWeight(.Font)
'        Width = CLng(.BoundWidth)
'        Width = CLng(.Characters(.Length, 1).BoundLeft + .Characters(.Length, 1).BoundWidth - .Characters(1, 1).BoundLeft)
    End With
    ' get the real width of the text (without invisible chars at the end)
    'Width = CLng(GetTextWidth(tr))         ' test for bug 686
    Width = 0
    
    With tr.ParagraphFormat.Bullet
        FontColor = MLBColor(.Font.Color)
        Text = ChrW(.Character) ' do not need WBChrMap any more.
    End With
    
    Length = Len(Text)
    Position = "normal"
    
End Sub

Private Function StandardizeText(txt As String) As String
    Dim baby        As String
    Dim i           As Long
    Dim val         As Long
    Dim res         As String
    
    res = ""
    For i = 1 To Len(txt)
        baby = Mid(txt, i, 1)
        val = CLng(AscW(baby))
        Select Case AscW(baby)   'AscW(baby)
'            Case 10, 11, 13, 9 ' delete linefeeds and returns
            Case 1 To 31
            Case 8346          ' €
                res = res & Chr$(128)
            Case 8218           ' ‚
                res = res & Chr$(130)
            Case 402            ' ƒ
                res = res & Chr$(131)
            Case 8222           ' „
                res = res & Chr$(132)
            Case 8230           ' …
                res = res & Chr$(133)
            Case 8224           ' †
                res = res & Chr$(134)
            Case 8225           ' ‡
                res = res & Chr$(135)
            Case 710            ' ˆ
                res = res & Chr$(136)
            Case 8240           ' ‰
                res = res & Chr$(137)
            Case 352            ' Š
                res = res & Chr$(138)
            Case 8249           ' ‹
                res = res & Chr$(139)
            Case 338            ' Œ
                res = res & Chr$(140)
            Case 381            ' Ž
                res = res & Chr$(142)
            Case 8216           ' ‘
                res = res & Chr$(145)
            Case 8217           ' ’
                res = res & Chr$(146)
            Case 8220           ' “
                res = res & Chr$(147)
            Case 8221           ' ”
                res = res & Chr$(148)
            Case 8226               ' •
                res = res & Chr$(149)
            Case 8211               ' –
                res = res & Chr$(150)
            Case 8212               ' —
                res = res & Chr$(151)
            Case 732                ' ˜
                res = res & Chr$(152)
            Case 8482               ' ™
                res = res & Chr$(153)
            Case 353                ' š
                res = res & Chr$(154)
            Case 8250               ' ›
                res = res & Chr$(155)
            Case 339                ' œ
                res = res & Chr$(156)
            Case 382                ' ž
                res = res & Chr$(158)
            Case 339                ' Ÿ
                res = res & Chr$(159)
'            Case 129, 141, 143, 144, 157, 160   ' invisible characters
'                res = res & " "
'            Case 150, 173
'                res = res & "-"
'            Case -3872  '->'
'                res = res & "->"
'            Case -3911          ' !=
'                res = res & "!="
'            Case -4054          ' '*'
'                res = res & "*"
'            Case -4034
'                res = res & ">"
'            Case -4035
'                res = res & "="
'            Case -4062          ' any
'                res = res & ChrW(-4062)
            Case Else
                res = res & baby
        End Select
    Next
    
    StandardizeText = res
End Function

' translate the arabic num to roman num
'
' args:     num - number
'           bUC - true: Roman number in Upper case
' return:   number in roman format
Private Function RomanNum(num As Long, bUC As Boolean) As String
    Dim rm  As String
    Dim dv As Long
    Dim rs   As Long
    Dim i As Long
    
    rm = String(num \ 1000, "M")
    
    rs = num Mod 1000
    dv = rs \ 100
    Select Case dv
        Case 9
            rm = rm & "CM"
        Case 5 To 8
            rm = rm & "D" & String(dv - 5, "C")
        Case 4
            rm = rm & "CD"
        Case 1 To 3
            rm = rm & String(dv, "C")
    End Select
    
    rs = rs Mod 100
    dv = rs \ 10
    Select Case dv
        Case 9
            rm = rm & "XC"
        Case 5 To 8
            rm = rm & "L" & String(dv - 5, "X")
        Case 4
            rm = rm & "XL"
        Case 1 To 3
            rm = rm & String(dv, "X")
    End Select
    
    dv = rs Mod 10
    Select Case dv
        Case 9
            rm = rm & "IX"
        Case 5 To 8
            rm = rm & "V" & String(dv - 5, "I")
        Case 4
            rm = rm & "IV"
        Case 1 To 3
            rm = rm & String(dv, "I")
    End Select
    If Not bUC Then
        rm = LCase(rm)
    End If
    RomanNum = rm
End Function


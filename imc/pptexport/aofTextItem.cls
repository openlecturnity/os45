VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "aofTextItem"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Public x           As Single          ' left
Public y           As Single          ' top
Public OffX        As Single          ' x offset between the text and the text box
Public offY        As Single          ' y offset "

Public HypertextAddress As String
Public CurrentDirectory As String
Public InternLink       As String
Public ObjectLinkIds    As String
Public ObjectId         As Long
Public LinkColor        As String

Public FontName    As String
Public FontColor   As String
Public FontSize    As Long

Public text        As String

Public Family       As String
Public Weight       As String
Public Slant        As String
Public Style        As Long
Public Position     As String       ' "Position" property of mlb font. NOT the position in aofwb.
Public length       As Long

Public width        As Single         ' test text width in lsd file   Nov. 12. 2002

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

Private Function MLBStyle(fnt As Font) As Long
    ' Introduced in order to support the style "underline"
    ' But can also contains/uses all the other "text effects": bold, superscript, emboss....
    Dim nStyle As Long
    
    nStyle = 0 ' simple plain text
    
    If fnt.Bold = msoTrue Then
        nStyle = nStyle + 1
    End If
    If fnt.Italic = msoTrue Then
        nStyle = nStyle + 2
    End If
    If fnt.Emboss = msoTrue Then
        nStyle = nStyle + 4
    End If
    If fnt.Shadow = msoTrue Then
        nStyle = nStyle + 8
    End If
    If fnt.Underline = msoTrue Then
        nStyle = nStyle + 16
    End If
    If fnt.Subscript = msoTrue Then
        nStyle = nStyle + 32
    End If
    If fnt.Superscript = msoTrue Then
        nStyle = nStyle + 64
    End If
    
    MLBStyle = nStyle
End Function

Public Function GetFontFamily(fnt As Font) As String

    If fnt.NameAscii <> "" Then
        GetFontFamily = fnt.NameAscii
    ElseIf fnt.NameOther <> "" Then
        GetFontFamily = fnt.NameOther
    Else
        GetFontFamily = "Arial"
    End If

End Function

Public Function MLBFamily(fnt As Font) As String
    ' New font name supported by Assistant v1.5.0
    If fnt.Name <> "" Then
        MLBFamily = fnt.Name
    ElseIf fnt.NameAscii <> "" Then
        MLBFamily = fnt.NameAscii
    ElseIf fnt.NameOther <> "" Then
        MLBFamily = fnt.NameOther
    ElseIf fnt.NameComplexScript <> "" Then
        MLBFamily = fnt.NameComplexScript
    ElseIf fnt.NameFarEast <> "" Then
        MLBFamily = fnt.NameFarEast
    Else
        MLBFamily = "Arial"
    End If
End Function

Public Function MLBAsciiFamily(fnt As Font) As String
    ' New font name supported by Assistant v1.5.0
    MLBAsciiFamily = fnt.NameAscii
End Function

Public Sub Perform(sld As Slide, ByVal txtSeg As TextRange, fontFamily As String, Optional bAllCaps As Boolean = True, Optional off_x As Long = 0, Optional off_y As Long = 0)
    On Error Resume Next
    Dim setLinkColor As Boolean
    
    With txtSeg
        x = .BoundLeft
        text = StandardizeText(.text)
        If bAllCaps Then
            text = UCase(text)
        End If
        
        setLinkColor = False
        If txtSeg.ActionSettings.Item(ppMouseClick).Hyperlink.address <> "" Then
            setLinkColor = True
            HypertextAddress = ReplaceBackslash(txtSeg.ActionSettings.Item(ppMouseClick).Hyperlink.address)
            CurrentDirectory = ReplaceBackslash(ActivePresentation.path)
        End If
        
        If g_lApplicationVersion = 2007 Then
            If txtSeg.ActionSettings.Item(ppMouseClick).Action = ppActionHyperlink Then
                setLinkColor = True
            End If
        End If
        
        If txtSeg.ActionSettings.Item(ppMouseClick).Hyperlink.SubAddress <> "" Then
            setLinkColor = True
            InternLink = ExtractSlideID(txtSeg.ActionSettings.Item(ppMouseClick).Hyperlink.SubAddress)
        End If
        
        If setLinkColor Then
            LinkColor = GetActiveLinkColor(sld, .Font.color)
            FontColor = GetLinkColor(sld, .Font.color)
        Else
            FontColor = GetRGBColorString(.Font.color)
        End If
        
        FontSize = MLBFontSize(.Font)
        If fontFamily <> "" Then
            Family = fontFamily
        Else
            Family = MLBFamily(.Font)
        End If
        Slant = MLBSlant(.Font)
        Weight = MLBWeight(.Font)
        Style = MLBStyle(.Font)
        If setLinkColor And .Font.Underline <> msoTrue Then
            Style = Style + 16
        End If
        length = Len(text)
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
    width = GetTextWidth(txtSeg)
    
End Sub

Private Function GetTextWidth(tr As TextRange) As Single
    Dim i As Long
    Dim idx As Long
    
    idx = tr.length
    For i = tr.length To 1 Step -1
        Select Case AscW(tr.Characters(i, 1).text)
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
            offY = -.Size * .BaselineOffset
            FontSize = CLng(.Size * 0.66)
        ElseIf .Superscript = msoTrue Then
            'TextPosition = "up"
            offY = -.Size * .BaselineOffset
            FontSize = CLng(.Size * 0.66)
        Else
            TextPosition = "normal"
        End If
    End With
    
    ' text position ignored in Assistant 1.4.0.b8w
    TextPosition = "normal"

End Function
' convert current textrange as bullet.
Public Sub PerformAsBullet(ByVal tr As TextRange, fntSize As Long, xpos As Single, bulletWidth As Single)
    
    On Error Resume Next
    FontSize = fntSize
    x = xpos
    
    With tr.ParagraphFormat.Bullet
        If .UseTextFont = msoTrue Then
            Family = MLBAsciiFamily(.Font)
        Else
            Family = MLBFamily(.Font)
        End If
        If g_lApplicationVersion = 2007 Then
            Slant = "roman"
            Weight = "normal"
        Else
            If .Font.Italic = msoTrue Then
                Slant = "italic"
            Else
                Slant = "roman"
            End If
            If .Font.Bold = msoTrue Then
                Weight = "bold"
            Else
                Weight = "normal"
            End If
        End If
        Style = 0
'        Width = CLng(.BoundWidth)
'        Width = CLng(.Characters(.Length, 1).BoundLeft + .Characters(.Length, 1).BoundWidth - .Characters(1, 1).BoundLeft)
    End With
    ' get the real width of the text (without invisible chars at the end)
    'Width = CLng(GetTextWidth(tr))         ' test for bug 686
    If g_lApplicationVersion <> 2007 Then
    width = 0
    Else
        width = bulletWidth
    End If
    
    With tr.ParagraphFormat.Bullet
        Select Case .Type
            Case PpBulletType.ppBulletNone
                
            Case PpBulletType.ppBulletNumbered
                If g_lApplicationVersion = 2007 And tr.ParagraphFormat.Bullet.UseTextColor Then
                    'FontColor = GetRGBColorString(tr.Font.color)
                    FontColor = GetRGBColorString(tr.Characters(0, 1).Font.color)
                Else
                    FontColor = GetRGBColorString(.Font.color)
                End If
                Family = MLBAsciiFamily(.Font)
                text = GetNumBulletString(tr.ParagraphFormat.Bullet)
            Case PpBulletType.ppBulletUnnumbered
                ' using font "Symble" instead. normally, in .ppt using font "Wingding"
                If g_lApplicationVersion = 2007 And tr.ParagraphFormat.Bullet.UseTextColor Then
                    'FontColor = GetRGBColorString(tr.Font.color)
                    FontColor = GetRGBColorString(tr.Characters(0, 1).Font.color)
                Else
                    FontColor = GetRGBColorString(.Font.color)
                End If
                
                text = ChrW(.Character) ' do not need WBChrMap any more.

'                If StrComp(Family, "symbol", vbTextCompare) = 0 Then
'                    Text = chr$(.Character)
'                Else
'                    Text = WBChrMap(.Character)
'                End If

    '        Case PpBulletType.ppBulletMixed
    '        Case PpBulletType.ppBulletPicture
            
            Case Else
                'ErrLog LEC_ERRORTYPE_UNKNOWNBULLET, "An unknown Bullet, (not Text, not image)."
'                Text = WBChrMap(-1)
'                FontColor = GetRGBColorString(.Font.color)
'                'ErrLog LEC_ERRORTYPE_UNKNOWNBULLET, "Set as default."
        End Select
    End With
    
    length = Len(text)
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

' if the bullet style is "numbered", return the corresponding string
'
' args:     blt - current bullet format
' return:   the bullet in right form
Public Function GetNumBulletString(blt As BulletFormat) As String
    Dim bul As String
    Select Case blt.Style
'        Case PpNumberedBulletStyle.ppBulletArabicAbjadDash
'        Case PpNumberedBulletStyle.ppBulletArabicAlphaDash
'        Case PpNumberedBulletStyle.ppBulletArabicDBPeriod
'        Case PpNumberedBulletStyle.ppBulletArabicDBPlain
        Case PpNumberedBulletStyle.ppBulletArabicParenBoth
            bul = "(" & blt.Number & ")"
        Case PpNumberedBulletStyle.ppBulletArabicParenRight
            bul = blt.Number & ")"
        Case PpNumberedBulletStyle.ppBulletArabicPeriod
            bul = blt.Number & "."
        Case PpNumberedBulletStyle.ppBulletArabicPlain
            bul = blt.Number
        Case PpNumberedBulletStyle.ppBulletRomanUCPeriod
            bul = RomanNum(blt.Number, True) & "."
        Case PpNumberedBulletStyle.ppBulletRomanLCPeriod
            bul = RomanNum(blt.Number, False) & "."
        Case PpNumberedBulletStyle.ppBulletAlphaUCPeriod
            bul = Chr(64 + blt.Number) & "."
        Case PpNumberedBulletStyle.ppBulletAlphaUCParenRight
            bul = Chr(64 + blt.Number) & ")"
        Case PpNumberedBulletStyle.ppBulletAlphaLCPeriod
            bul = Chr(96 + blt.Number) & "."
        Case PpNumberedBulletStyle.ppBulletAlphaLCParenRight
            bul = Chr(96 + blt.Number) & ")"
        Case Else
            bul = blt.Number & "."
    End Select
    GetNumBulletString = bul
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


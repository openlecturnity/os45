VERSION 5.00
Object = "{0D452EE1-E08F-101A-852E-02608C4D0BB4}#2.0#0"; "FM20.DLL"
Begin VB.Form frmStart 
   BorderStyle     =   1  'Fest Einfach
   Caption         =   "Form1"
   ClientHeight    =   4335
   ClientLeft      =   45
   ClientTop       =   315
   ClientWidth     =   6330
   Icon            =   "frmStart.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4335
   ScaleWidth      =   6330
   Begin VB.Frame WizardCaption 
      BackColor       =   &H00FFFFFF&
      BorderStyle     =   0  'Kein
      Caption         =   "Frame3"
      Height          =   795
      Left            =   0
      TabIndex        =   12
      Top             =   0
      Width           =   6372
      Begin MSForms.Label Header 
         Height          =   255
         Left            =   840
         TabIndex        =   13
         Top             =   120
         Width           =   4455
         VariousPropertyBits=   8388627
         Size            =   "7857;450"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
      Begin MSForms.Label SubHeader 
         Height          =   375
         Left            =   840
         TabIndex        =   14
         Top             =   360
         Width           =   4455
         VariousPropertyBits=   8388627
         Size            =   "7857;660"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
      Begin VB.Image bmpLogoRight 
         Height          =   795
         Left            =   4920
         Picture         =   "frmStart.frx":0E42
         Top             =   0
         Width           =   1470
      End
      Begin VB.Image bmpLogoLeft 
         Height          =   795
         Left            =   0
         Picture         =   "frmStart.frx":2738
         Top             =   0
         Width           =   780
      End
   End
   Begin VB.Frame BottomFrame 
      BorderStyle     =   0  'Kein
      Caption         =   "Frame3"
      Height          =   855
      Left            =   0
      TabIndex        =   4
      Top             =   3480
      Width           =   6375
      Begin VB.Frame Frame2 
         Height          =   30
         Index           =   1
         Left            =   0
         TabIndex        =   7
         Top             =   120
         Width           =   6345
      End
      Begin MSForms.CommandButton cmdPrev 
         Height          =   375
         Left            =   2280
         TabIndex        =   8
         Top             =   360
         Width           =   1215
         Caption         =   "< Zurück"
         Size            =   "2139;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3
      End
      Begin MSForms.CommandButton cmdNext 
         Height          =   375
         Left            =   3480
         TabIndex        =   6
         Top             =   360
         Width           =   1215
         Caption         =   "Weiter >"
         Size            =   "2139;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3
      End
      Begin MSForms.CommandButton cmdCancel 
         Height          =   375
         Left            =   4920
         TabIndex        =   5
         Top             =   360
         Width           =   1215
         Caption         =   "Abbrechen"
         Size            =   "2144;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3
      End
   End
   Begin VB.Frame TopSeparator 
      Height          =   30
      Left            =   0
      TabIndex        =   3
      Top             =   796
      Width           =   6345
   End
   Begin VB.Frame Frame1 
      BorderStyle     =   0  'Kein
      Height          =   2775
      Left            =   0
      TabIndex        =   0
      Top             =   840
      Width           =   6375
      Begin VB.TextBox txtSelection 
         Height          =   285
         Left            =   1440
         TabIndex        =   10
         Top             =   1200
         Width           =   4455
      End
      Begin MSForms.OptionButton optSelection 
         Height          =   495
         Index           =   1
         Left            =   3120
         TabIndex        =   9
         Top             =   480
         Width           =   1695
         BackColor       =   -2147483633
         ForeColor       =   -2147483630
         DisplayStyle    =   5
         Size            =   "2992;874"
         Value           =   "0"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
      Begin MSForms.OptionButton optSelection 
         Height          =   495
         Index           =   2
         Left            =   360
         TabIndex        =   2
         Top             =   1080
         Width           =   1095
         BackColor       =   -2147483633
         ForeColor       =   -2147483630
         DisplayStyle    =   5
         Size            =   "1933;874"
         Value           =   "0"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
      Begin MSForms.OptionButton optSelection 
         Height          =   495
         Index           =   0
         Left            =   360
         TabIndex        =   1
         Top             =   480
         Width           =   2055
         BackColor       =   -2147483633
         ForeColor       =   -2147483630
         DisplayStyle    =   5
         Size            =   "3628;874"
         Value           =   "1"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
      Begin MSForms.Label txtDescription 
         Height          =   735
         Left            =   360
         TabIndex        =   11
         Top             =   1680
         Width           =   5535
         VariousPropertyBits=   8388627
         Size            =   "9764;1297"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
   End
End
Attribute VB_Name = "frmStart"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public g_iTotalSlideCount    As Long
Public g_iCurrentSlide       As Long
Public g_szSlideSelection    As String
Public g_bCancelled         As Boolean
Public g_iLanguage          As Integer
Public g_iSelection         As Integer
Public g_lTop               As Long
Public g_lLeft              As Long

Const c_iWizardCaptionHeight = 900

Private Sub Form_Initialize()

    g_lTop = -1
    g_lLeft = -1
    Me.Top = (Screen.Height - Me.Height) / 2
    Me.Left = (Screen.Width - Me.Width) / 2
    
End Sub

Private Sub Form_Activate()
    Me.Caption = LoadResString(DLG_CAPTION + g_iLanguage * 1000)
    
    Me.Header.Caption = LoadResString(HEADER_START + g_iLanguage * 1000)
    Me.SubHeader.Caption = LoadResString(SUBHEADER_START + g_iLanguage * 1000)

    Me.optSelection(0).Caption = LoadResString(STR_ALLSLIDES + g_iLanguage * 1000)
    Me.optSelection(0).Value = False
    
    Me.optSelection(1).Caption = LoadResString(STR_CURSLIDE + g_iLanguage * 1000)
    Me.optSelection(1).Value = False
    
    Me.optSelection(2).Caption = LoadResString(STR_SELSLIDES + g_iLanguage * 1000)
    Me.optSelection(2).Value = False
    
    Me.optSelection(g_iSelection).Value = True

    Me.txtDescription.Caption = LoadResString(SELECTION_DESCRIPTION + g_iLanguage * 1000)
    Me.txtSelection.Text = g_szSlideSelection
    
    Me.cmdPrev.Caption = LoadResString(CMD_PREV + g_iLanguage * 1000)
    Me.cmdPrev.Enabled = False
    Me.cmdNext.Caption = LoadResString(CMD_NEXT + g_iLanguage * 1000)
    Me.cmdCancel.Caption = LoadResString(CMD_CANCEL + g_iLanguage * 1000)

    Me.g_bCancelled = False
    
    Me.cmdNext.SetFocus
    
    RefreshSlideSetting
End Sub


Private Sub RefreshSlideSetting()
    Me.txtSelection.Enabled = Me.optSelection(2).Value
End Sub



Private Function CheckValid(szText As String) As Boolean
    CheckValid = True
    
    Dim szSlideNumber As String: szSlideNumber = ""
    Dim iSlideNumber As Long
    Dim iRangeCount As Integer: iRangeCount = 0
    Dim i As Long
    For i = 1 To Len(szText)
        Dim szChar As String: szChar = Mid(szText, i, 1)
        If IsNumeric(szChar) Then
            szSlideNumber = szSlideNumber + szChar
        ElseIf szChar = "-" Or szChar = ";" Then
            If szChar = "-" Then
                If iRangeCount = 1 Then
                    iRangeCount = 0
                    CheckValid = False
                    Beep
                    Exit Function
                End If
                iRangeCount = iRangeCount + 1
            Else
                iRangeCount = 0
            End If
            If Len(szSlideNumber) > 0 Then
                iSlideNumber = CLng(szSlideNumber)
                If iSlideNumber < 1 Or iSlideNumber > g_iTotalSlideCount Then
                    CheckValid = False
                    Beep
                    Exit Function
                End If
                szSlideNumber = ""
            End If
        Else
            CheckValid = False
            Beep
            Exit Function
        End If
    Next
    
    If Len(szSlideNumber) > 0 Then
        iSlideNumber = CLng(szSlideNumber)
        If iSlideNumber < 1 Or iSlideNumber > g_iTotalSlideCount Then
            CheckValid = False
            Beep
            Exit Function
        End If
        szSlideNumber = ""
    End If
    
    
End Function

Private Sub FillSlideArrayWithAll()
    g_szSlideSelection = "1-" & g_iTotalSlideCount
    g_iSelection = 0
End Sub

Private Sub FillSlideArrayWithCurrent()
    g_szSlideSelection = g_iCurrentSlide
    g_iSelection = 1
End Sub

Private Sub FillSlideArrayWithSelection()
    g_szSlideSelection = Me.txtSelection.Text
    g_iSelection = 2
End Sub

Private Function ShowMsg(msgID As Long) As Long
    Dim szTitle As String: szTitle = LoadResString(DLG_CAPTION + g_iLanguage * 1000)
    msgID = msgID + g_iLanguage * 1000
    Dim szText As String: szText = LoadResString(msgID)
    Dim iButton As Long: iButton = MessageBoxW(0, StrPtr(szText), StrPtr(szTitle), MB_ICONINFORMATION Or MB_TASKMODAL)
End Function


Private Sub optSelection_Click(Index As Integer)
    RefreshSlideSetting
End Sub

Private Sub txtSelection_Change()
    
    If Not CheckValid(Me.txtSelection.Text) Then
        'ShowMsg (SELECTION_ERROR)
    End If
    
End Sub


Private Sub txtSelection_KeyPress(keyascii As Integer)

    Dim szChar As String
    szChar = Chr(keyascii)

    If Asc(szChar) = 8 Then ' Backspace
        Exit Sub
    End If
    
    If Not (IsNumeric(szChar) Or szChar = " " Or szChar = ";" Or szChar = "-") Then
        keyascii = 0
        'ShowMsg (SELECTION_ERROR)
    Else
        Dim szText      As String
        Dim szTextLeft  As String
        Dim szTextRight As String
        Dim iActPos As Integer: iActPos = Me.txtSelection.SelStart
        Dim iLength As Integer: iLength = Len(Me.txtSelection.Text)
        szTextLeft = Left(Me.txtSelection.Text, iActPos)
        szTextRight = Right(Me.txtSelection.Text, iLength - iActPos)
        szText = szTextLeft & szChar & szTextRight
        If Not CheckValid(szText) Then
            keyascii = 0
        End If
    End If
    

End Sub

Private Sub cmdNext_Click()

    If Me.optSelection(0).Value Then
        FillSlideArrayWithAll
    ElseIf Me.optSelection(1).Value Then
        FillSlideArrayWithCurrent
    Else
        If Not CheckValid(Me.txtSelection.Text) Then
            Exit Sub
        Else
            FillSlideArrayWithSelection
        End If
    End If
    
    g_lTop = Me.Top
    g_lLeft = Me.Left
    Unload Me
End Sub

Private Sub cmdCancel_Click()
    g_bCancelled = True
    Unload Me
End Sub


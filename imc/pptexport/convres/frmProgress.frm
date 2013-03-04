VERSION 5.00
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.3#0"; "comctl32.ocx"
Object = "{0D452EE1-E08F-101A-852E-02608C4D0BB4}#2.0#0"; "FM20.DLL"
Begin VB.Form frmProgress 
   BorderStyle     =   1  'Fest Einfach
   Caption         =   "Progress"
   ClientHeight    =   4335
   ClientLeft      =   45
   ClientTop       =   315
   ClientWidth     =   6330
   Icon            =   "frmProgress.frx":0000
   LinkTopic       =   "Progress"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4335
   ScaleWidth      =   6330
   ShowInTaskbar   =   0   'False
   Begin VB.Timer Timer1 
      Left            =   5880
      Top             =   3000
   End
   Begin VB.Frame WizardCaption 
      BackColor       =   &H00FFFFFF&
      BorderStyle     =   0  'Kein
      Caption         =   "Frame3"
      Height          =   795
      Left            =   0
      TabIndex        =   10
      Top             =   0
      Width           =   6372
      Begin VB.Image bmpLogoLeft 
         Height          =   795
         Left            =   0
         Picture         =   "frmProgress.frx":0E42
         Top             =   0
         Width           =   780
      End
      Begin VB.Image bmpLogoRight 
         Height          =   795
         Left            =   4920
         Picture         =   "frmProgress.frx":1D48
         Top             =   0
         Width           =   1470
      End
      Begin MSForms.Label Header 
         Height          =   255
         Left            =   840
         TabIndex        =   12
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
         TabIndex        =   11
         Top             =   360
         Width           =   4455
         VariousPropertyBits=   8388627
         Size            =   "7857;660"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
   End
   Begin ComctlLib.ProgressBar ProgressBar 
      Height          =   285
      Left            =   240
      TabIndex        =   6
      Top             =   2040
      Width           =   5895
      _ExtentX        =   10398
      _ExtentY        =   503
      _Version        =   327682
      Appearance      =   0
   End
   Begin VB.Frame BottomFrame 
      BorderStyle     =   0  'Kein
      Caption         =   "Frame3"
      Height          =   855
      Left            =   0
      TabIndex        =   0
      Top             =   3480
      Width           =   6375
      Begin VB.Frame Frame2 
         Height          =   30
         Index           =   1
         Left            =   0
         TabIndex        =   1
         Top             =   120
         Width           =   6345
      End
      Begin MSForms.CommandButton cmdPrev 
         Height          =   375
         Left            =   2280
         TabIndex        =   2
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
         TabIndex        =   3
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
         TabIndex        =   4
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
      TabIndex        =   5
      Top             =   796
      Width           =   6345
   End
   Begin MSForms.Label txtProgressDuration 
      Height          =   375
      Left            =   3120
      TabIndex        =   9
      Top             =   2520
      Width           =   3015
      Caption         =   "Label2"
      Size            =   "5317;661"
      FontHeight      =   165
      FontCharSet     =   0
      FontPitchAndFamily=   2
      ParagraphAlign  =   2
   End
   Begin MSForms.Label txtProgressTimeLeft 
      Height          =   375
      Left            =   240
      TabIndex        =   8
      Top             =   2520
      Width           =   2775
      Caption         =   "Label2"
      Size            =   "4894;661"
      FontHeight      =   165
      FontCharSet     =   0
      FontPitchAndFamily=   2
   End
   Begin MSForms.Label txtProgressSlide 
      Height          =   255
      Left            =   240
      TabIndex        =   7
      Top             =   1680
      Width           =   5895
      Caption         =   "Label1"
      Size            =   "10398;450"
      FontHeight      =   165
      FontCharSet     =   0
      FontPitchAndFamily=   2
   End
End
Attribute VB_Name = "frmProgress"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public g_bImportFinished    As Boolean
Public g_iLanguage          As Integer
Public g_lTop               As Long
Public g_lLeft              As Long
Public g_bWizard            As Boolean
Private g_sStartTime        As Single

Private Enum ESetWindowPosStyles
    SWP_SHOWWINDOW = &H40
    SWP_HIDEWINDOW = &H80
    SWP_FRAMECHANGED = &H20 ' The frame changed: send WM_NCCALCSIZE
    SWP_NOACTIVATE = &H10
    SWP_NOCOPYBITS = &H100
    SWP_NOMOVE = &H2
    SWP_NOOWNERZORDER = &H200 ' Don't do owner Z ordering
    SWP_NOREDRAW = &H8
    SWP_NOREPOSITION = SWP_NOOWNERZORDER
    SWP_NOSIZE = &H1
    SWP_NOZORDER = &H4
    SWP_DRAWFRAME = SWP_FRAMECHANGED
    HWND_TOP = 0
    HWND_TOPMOST = -1
    HWND_NOTOPMOST = -2
End Enum


Private Declare Function SetWindowPos Lib "user32" _
    (ByVal hwnd As Long, ByVal hWndInsertAfter As Long, _
     ByVal X As Long, ByVal Y As Long, ByVal cx As Long, ByVal cy As Long, _
     ByVal wFlags As Long) As Long

Private Sub Form_Initialize()

    g_lTop = -1
    g_lLeft = -1
    Me.Top = (Screen.Height - Me.Height) / 2
    Me.Left = (Screen.Width - Me.Width) / 2
    
    g_bWizard = True
    Me.ProgressBar.Value = 0
    
End Sub

Private Sub Form_Load()
    g_sStartTime = Timer
    Timer1.Interval = 500
    Timer1.Enabled = True
End Sub

Private Sub Timer1_Timer()
    Dim sCurrentTime As Single: sCurrentTime = Timer
    Dim sDuration As Single: sDuration = sCurrentTime - g_sStartTime
 
    Dim iMinutes As Integer: iMinutes = Int(sDuration / 60)
    Dim iSeconds As Integer: iSeconds = sDuration - (60 * iMinutes)
    Dim szDuration As String: szDuration = ""
    If iSeconds < 10 Then
        szDuration = iMinutes & ":0" & iSeconds
    Else
        szDuration = iMinutes & ":" & iSeconds
    End If
    txtProgressDuration = LoadResString(TXT_DURATION + g_iLanguage * 1000) & _
                                        " " & szDuration
    'txtProgressDuration.Refresh

End Sub


Private Sub Form_Activate()

    Me.Caption = LoadResString(DLG_CAPTION + g_iLanguage * 1000)

    Me.Header.Caption = LoadResString(HEADER_PROGRESS + g_iLanguage * 1000)
    Me.SubHeader.Caption = LoadResString(SUBHEADER_PROGRESS + g_iLanguage * 1000)

    If g_bWizard Then
        Me.cmdPrev.Caption = LoadResString(CMD_PREV + g_iLanguage * 1000)
        Me.cmdPrev.Enabled = False
    
        Me.cmdNext.Caption = LoadResString(CMD_NEXT + g_iLanguage * 1000)
        Me.cmdNext.Enabled = False
    
        Me.cmdCancel.Caption = LoadResString(CMD_CANCEL + g_iLanguage * 1000)
        Me.cmdCancel.Enabled = True
    Else
        Me.cmdPrev.Visible = False
        Me.cmdNext.Visible = False
        Me.cmdCancel.Left = (Me.Width - Me.cmdCancel.Width) / 2
    End If
  
    Me.txtProgressSlide.Caption = ""
    Me.txtProgressTimeLeft.Caption = LoadResString(TXT_TIME_LEFT + g_iLanguage * 1000)
    Me.txtProgressDuration.Caption = LoadResString(TXT_DURATION + g_iLanguage * 1000)
    
    g_sStartTime = Timer
    
    'Me.cmdCancel.SetFocus
    
    g_bImportFinished = False
    
    SetWindowPos Me.hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE Or SWP_FRAMECHANGED Or SWP_NOSIZE
End Sub


Private Sub cmdCancel_Click()
    Me.MousePointer = vbHourglass
    g_bImportFinished = True
End Sub


VERSION 5.00
Object = "{0ECD9B60-23AA-11D0-B351-00A0C9055D8E}#6.0#0"; "mshflxgd.ocx"
Begin VB.Form frmResult 
   BorderStyle     =   1  'Fest Einfach
   Caption         =   "Form1"
   ClientHeight    =   4335
   ClientLeft      =   45
   ClientTop       =   1080
   ClientWidth     =   6330
   Icon            =   "frmResult.frx":0000
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
      Begin VB.Image bmpLogoLeft 
         Height          =   795
         Left            =   0
         Picture         =   "frmResult.frx":0E42
         Top             =   0
         Width           =   780
      End
      Begin VB.Image bmpLogoRight 
         Height          =   795
         Left            =   4920
         Picture         =   "frmResult.frx":1D48
         Top             =   0
         Width           =   1470
      End
      Begin MSForms.Label Header 
         Height          =   255
         Left            =   840
         TabIndex        =   14
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
         TabIndex        =   13
         Top             =   360
         Width           =   4455
         VariousPropertyBits=   8388627
         Size            =   "7857;660"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
   End
   Begin VB.Frame DetailsFrame 
      BorderStyle     =   0  'Kein
      Caption         =   "DetailsFrame"
      Height          =   1092
      Left            =   240
      TabIndex        =   4
      Top             =   2280
      Width           =   5895
      Begin MSForms.CommandButton Details 
         Height          =   375
         Left            =   0
         TabIndex        =   5
         Top             =   600
         Width           =   1455
         Caption         =   "Details"
         Size            =   "2551;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3
      End
      Begin MSForms.Label WarningMessage 
         Caption         =   "WarningMessage"
         Height          =   375
         Left            =   0
         TabIndex        =   7
         Top             =   240
         Width           =   5535
         VariousPropertyBits=   8388627
         Size            =   "9769;660"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
   End
   Begin VB.Frame BottomFrame 
      BorderStyle     =   0  'Kein
      Caption         =   "BottomFrame"
      Height          =   855
      Left            =   0
      TabIndex        =   1
      Top             =   3480
      Width           =   6375
      Begin MSForms.CommandButton cmdCancel 
         Height          =   375
         Left            =   4920
         TabIndex        =   11
         Top             =   360
         Width           =   1215
         Caption         =   "Abbrechen"
         Size            =   "2139;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3
      End
      Begin MSForms.CommandButton cmdNext 
         Height          =   375
         Left            =   3480
         TabIndex        =   10
         Top             =   360
         Width           =   1215
         Caption         =   "Weiter >"
         Size            =   "2139;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3
      End
      Begin MSForms.CommandButton cmdPrev 
         Default         =   -1  'True
         Height          =   375
         Left            =   2280
         TabIndex        =   9
         Top             =   360
         Width           =   1215
         Caption         =   "< Zurück"
         Size            =   "2139;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3

      End
      Begin VB.Frame Separator 
         Height          =   30
         Index           =   1
         Left            =   0
         TabIndex        =   3
         Top             =   120
         Width           =   6345
      End
      Begin MSForms.CommandButton cmdOpenIt 
         Height          =   375
         Left            =   240
         TabIndex        =   2
         Top             =   360
         Width           =   1335
         Caption         =   "Start"
         Size            =   "2356;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3

      End
   End
   Begin VB.Frame SeparatorTop 
      Height          =   30
      Left            =   0
      TabIndex        =   0
      Top             =   796
      Width           =   6345
   End
   Begin MSHierarchicalFlexGridLib.MSHFlexGrid Grid 
      Height          =   24
      Left            =   240
      TabIndex        =   6
      Top             =   3480
      Width           =   5892
      _ExtentX        =   10398
      _ExtentY        =   53
      _Version        =   393216
      BackColor       =   16777215
      FixedCols       =   0
      BackColorSel    =   16777215
      BackColorBkg    =   -2147483639
      BackColorUnpopulated=   16777215
      AllowBigSelection=   0   'False
      GridLines       =   0
      AllowUserResizing=   1
      _NumberOfBands  =   1
      _Band(0).Cols   =   2
   End
   Begin MSForms.Label lblConvSucc 
      Height          =   372
      Left            =   240
      TabIndex        =   8
      Top             =   1440
      Width           =   4332         
	VariousPropertyBits=   8388627
      Size            =   "7647;656"
      FontHeight      =   165
      FontCharSet     =   0
      FontPitchAndFamily=   2

   End
End
Attribute VB_Name = "frmResult"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public bOpenIt As Boolean
Public strInfo  As String
Public LanguageID As Integer
Public Version As String

Private p_bShowDetails As Boolean
Private p_iWindowHeight As Integer
Private p_iBottomFrameTop As Integer

Public g_lTop               As Long
Public g_lLeft              As Long

Const c_iGridHeight = 3000

Private Sub Form_Initialize()

    g_lTop = -1
    g_lLeft = -1
    Me.Top = (Screen.Height - Me.Height) / 2
    Me.Left = (Screen.Width - Me.Width) / 2
    
End Sub

Private Sub Form_Activate()
    Me.Caption = LoadResString(DLG_CAPTION + LanguageID * 1000)
    
    p_iBottomFrameTop = Me.BottomFrame.Top
    p_iWindowHeight = Me.Height
        
    p_bShowDetails = False
    Me.Grid.Visible = False
    
    Me.Header.Caption = LoadResString(HEADER_SUMMARY + LanguageID * 1000)
    Me.SubHeader.Caption = LoadResString(SUBHEADER_SUMMARY + LanguageID * 1000)
    
    Me.lblConvSucc.Caption = LoadResString(TXT_CONVERSION_FINISHED + LanguageID * 1000)

    Me.cmdOpenIt.Caption = LoadResString(CMD_OPEN + LanguageID * 1000)
    Me.cmdOpenIt.Enabled = True
    
    Me.cmdPrev.Caption = LoadResString(CMD_PREV + LanguageID * 1000)
    Me.cmdPrev.Enabled = False
    
    Me.cmdNext.Caption = LoadResString(CMD_QUIT + LanguageID * 1000)
    Me.cmdNext.Enabled = True
    
    Me.cmdCancel.Caption = LoadResString(CMD_CANCEL + LanguageID * 1000)
    Me.cmdCancel.Enabled = False

    Me.WarningMessage.Caption = LoadResString(OBJECT_WARNING_MESSAGE + LanguageID * 1000)
    Me.Details.Caption = LoadResString(SHOW_DETAILS + LanguageID * 1000)
    
    Me.cmdNext.SetFocus

End Sub


Private Sub cmdNext_Click()
    bOpenIt = False
    Unload Me
End Sub

Private Sub cmdCancel_Click()
    bOpenIt = False
    Unload Me
End Sub

Private Sub cmdOpenIt_Click()
    bOpenIt = True
    Unload Me
End Sub

Private Sub Details_Click()
    p_bShowDetails = Not p_bShowDetails
    If p_bShowDetails Then
        Me.Grid.Visible = True
        Me.Grid.Height = 3000
        Me.Height = p_iWindowHeight + Me.Grid.Height
        Me.Grid.Top = Me.DetailsFrame.Top + Me.DetailsFrame.Height
        p_iBottomFrameTop = p_iBottomFrameTop + Me.Grid.Height
        Me.Details.Caption = LoadResString(HIDE_DETAILS + LanguageID * 1000)
    Else
        Me.Grid.Visible = False
        Me.Height = p_iWindowHeight
        p_iBottomFrameTop = p_iBottomFrameTop - Me.Grid.Height
        Me.Details.Caption = LoadResString(SHOW_DETAILS + LanguageID * 1000)
    End If
    
    Me.BottomFrame.Top = p_iBottomFrameTop
        
End Sub

VERSION 5.00
Object = "{0ECD9B60-23AA-11D0-B351-00A0C9055D8E}#6.0#0"; "mshflxgd.ocx"
Object = "{0D452EE1-E08F-101A-852E-02608C4D0BB4}#2.0#0"; "FM20.DLL"
Begin VB.Form frmImportWarnings 
   Caption         =   "Form1"
   ClientHeight    =   5535
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6300
   Icon            =   "frmImportWarnings.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   5535
   ScaleWidth      =   6300
   Begin MSForms.CheckBox checkShowNeverAgain 
      Height          =   255
      Left            =   240
      TabIndex        =   10
      Top             =   4440
      Width           =   5535
      BackColor       =   -2147483633
      ForeColor       =   -2147483630
      DisplayStyle    =   4
      Size            =   "9771;450"
      Value           =   "0"
      Caption         =   "Show never again"
      FontHeight      =   165
      FontCharSet     =   0
      FontPitchAndFamily=   2
   End
   Begin VB.Frame SeparatorTop 
      Height          =   30
      Left            =   0
      TabIndex        =   8
      Top             =   796
      Width           =   6345
   End
   Begin VB.Frame BottomFrame 
      BorderStyle     =   0  'Kein
      Caption         =   "BottomFrame"
      Height          =   855
      Left            =   0
      TabIndex        =   5
      Top             =   4680
      Width           =   6375
      Begin VB.Frame Separator 
         Height          =   30
         Index           =   1
         Left            =   0
         TabIndex        =   6
         Top             =   120
         Width           =   6345
      End
      Begin MSForms.CommandButton cmdOk 
         Height          =   375
         Left            =   2520
         TabIndex        =   7
         Top             =   360
         Width           =   1335
         Caption         =   "OK"
         Size            =   "2356;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
         ParagraphAlign  =   3
      End
   End
   Begin VB.Frame DetailsFrame 
      BorderStyle     =   0  'Kein
      Caption         =   "DetailsFrame"
      Height          =   375
      Left            =   240
      TabIndex        =   3
      Top             =   960
      Width           =   5895
      Begin MSForms.Label WarningMessage 
         Height          =   375
         Left            =   0
         TabIndex        =   4
         Top             =   0
         Width           =   5535
         VariousPropertyBits=   8388627
         Caption         =   "WarningMessage"
         Size            =   "9771;661"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
   End
   Begin VB.Frame WizardCaption 
      BackColor       =   &H00FFFFFF&
      BorderStyle     =   0  'Kein
      Caption         =   "Frame3"
      Height          =   795
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   6372
      Begin MSForms.Label SubHeader 
         Height          =   375
         Left            =   840
         TabIndex        =   2
         Top             =   360
         Width           =   4455
         VariousPropertyBits=   8388627
         Size            =   "7857;660"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
      Begin MSForms.Label Header 
         Height          =   255
         Left            =   840
         TabIndex        =   1
         Top             =   120
         Width           =   4455
         VariousPropertyBits=   8388627
         Size            =   "7857;450"
         FontHeight      =   165
         FontCharSet     =   0
         FontPitchAndFamily=   2
      End
      Begin VB.Image bmpLogoRight 
         Height          =   795
         Left            =   4920
         Picture         =   "frmImportWarnings.frx":0E42
         Top             =   0
         Width           =   1470
      End
      Begin VB.Image bmpLogoLeft 
         Height          =   795
         Left            =   0
         Picture         =   "frmImportWarnings.frx":2738
         Top             =   0
         Width           =   780
      End
   End
   Begin MSHierarchicalFlexGridLib.MSHFlexGrid Grid 
      Height          =   2895
      Left            =   240
      TabIndex        =   9
      Top             =   1320
      Width           =   5895
      _ExtentX        =   10398
      _ExtentY        =   5106
      _Version        =   393216
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
End
Attribute VB_Name = "frmImportWarnings"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public bOpenIt              As Boolean
Public strInfo              As String
Public LanguageID           As Integer
Public Version              As String
Public g_lShowNeverAgain    As Integer
Public g_lTop               As Long
Public g_lLeft              As Long

Const c_iGridHeight = 3000

Private Sub Form_Initialize()

    g_lTop = -1
    g_lLeft = -1
    Me.Top = (Screen.Height - Me.Height) / 2
    Me.Left = (Screen.Width - Me.Width) / 2
    
    g_lShowNeverAgain = False
End Sub

Private Sub Form_Activate()
    
    Me.Caption = LoadResString(DLG_CAPTION + LanguageID * 1000)
    
    Me.Header.Caption = LoadResString(HEADER_SUMMARY + LanguageID * 1000)
    Me.SubHeader.Caption = LoadResString(SUBHEADER_SUMMARY + LanguageID * 1000)
    
    Me.WarningMessage.Caption = LoadResString(OBJECT_WARNING_MESSAGE + LanguageID * 1000)
    
    Me.checkShowNeverAgain.Caption = LoadResString(SHOW_NEVER_AGAIN_MESSAGE + LanguageID * 1000)
    Me.checkShowNeverAgain.Value = g_lShowNeverAgain
    
    Me.cmdOk.Enabled = True
    Me.cmdOk.SetFocus

End Sub

Private Sub cmdOK_Click()
    g_lShowNeverAgain = Me.checkShowNeverAgain.Value
    
    If g_lShowNeverAgain Then
        Registry.SetShowSummary (0)
    Else
        Registry.SetShowSummary (1)
    End If
    Unload Me
End Sub
